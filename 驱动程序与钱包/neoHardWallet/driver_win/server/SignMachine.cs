using Microsoft.Owin;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using NeoDun;
using driver_win;
using System.Text.RegularExpressions;

namespace hhgate
{
    public class SignMachine : CustomServer.IParser
    {


        public void HandleRequest(IOwinContext context, string rootpath, string relativePath)
        {
            var api = relativePath.ToLower();
            var formdata = FormData.FromRequest(context.Request);
            if (formdata == null)
            {
                context.Response.StatusCode = 500;
                context.Response.ContentType = "text/plain";
                MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                json["msg"] = new MyJson.JsonNode_ValueString("formdata format error.");
                json["tag"] = new MyJson.JsonNode_ValueNumber(-1);
                context.Response.Write(json.ToString());
                return;
            }
            if (linking)
            {
                MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                json["tag"] = new MyJson.JsonNode_ValueNumber(-1);
                json["msg"] = new MyJson.JsonNode_ValueString("server is busy");
                context.Response.Write(json.ToString());
                return;
            }
            if (relativePath == "ver")
            {
                ver(context, formdata);
                return;
            }
            else if (relativePath == "state")
            {//查询状态
                var signer = NeoDun.Signer.Ins;
                MyJson.JsonNode_Object jsonr = new MyJson.JsonNode_Object();
                jsonr["tag"] = new MyJson.JsonNode_ValueNumber(0);
                int dc = signer.CheckDevice();
                jsonr["count"] = new MyJson.JsonNode_ValueNumber(dc);

                if (dc > 0)
                {
                    jsonr["type"] = new MyJson.JsonNode_ValueString(signer.CheckDeviceTag());
                }
                else
                {
                    jsonr["type"] = new MyJson.JsonNode_ValueString("_null_");
                }
                context.Response.Write(jsonr.ToString());
            }
            else if (relativePath == "listaddress")
            {
                var signer = NeoDun.Signer.Ins;
                MyJson.JsonNode_Object jsonr = new MyJson.JsonNode_Object();
                jsonr["tag"] = new MyJson.JsonNode_ValueNumber(0);
                MyJson.JsonNode_Array adds = new MyJson.JsonNode_Array();
                jsonr["addresses"] = adds;
                int dc = signer.CheckDevice();
                if (dc > 0)
                {
                    foreach (var add in signer.addressPool.addresses)
                    {
                        MyJson.JsonNode_Object item = new MyJson.JsonNode_Object();
                        adds.Add(item);
                        item["type"] = new MyJson.JsonNode_ValueString(add.type.ToString());
                        item["address"] = new MyJson.JsonNode_ValueString(add.AddressText);
                    }
                }
                context.Response.Write(jsonr.ToString());
            }
            else if (relativePath == "sign")
            {
                linking = true;
                sign(context, formdata);
                return;
            }
            else if (relativePath == "addaddress")
            {
                linking = true;
                addAddress(context, formdata);
            }
            else if (relativePath == "deladdress")
            {
                linking = true;
                delAddress(context, formdata);
                return;
            }
            else if (relativePath == "addressinfo")
            {
                linking = true;
                addressinfo(context, formdata);
                return;
            }
            else if (relativePath == "comfirmpassword")
            {
                comfirming = true;
                comfirmpassword(context, formdata);
                return;
            }
            else
            {
                MyJson.JsonNode_Object jsonr = new MyJson.JsonNode_Object();
                jsonr["tag"] = new MyJson.JsonNode_ValueNumber(-1000);
                jsonr["msg"] = new MyJson.JsonNode_ValueString("unknown cmd.");
                //await context.Response.WriteAsync(jsonr.ToString());
            }

            return;

        }
        public class MessagePair
        {
            public MessagePair(Message msg, Message srcmsg)
            {
                this.msg = msg;
                this.srcmsg = srcmsg;
            }
            public Message msg;
            public Message srcmsg;
        }

        public class Watcher : NeoDun.IWatcher
        {
            public System.Collections.Concurrent.ConcurrentQueue<MessagePair> onRecvs = new System.Collections.Concurrent.ConcurrentQueue<MessagePair>();
            public void OnRecv(Message recv, Message src)
            {
                onRecvs.Enqueue(new MessagePair(recv, src));
            }

            public void OnSend(Message send, bool needBack)
            {
            }
        }
        private static void ver(IOwinContext context, FormData formdata)
        {
            MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
            json["tag"] = new MyJson.JsonNode_ValueNumber(0);
            MyJson.JsonNode_Array maps = new MyJson.JsonNode_Array();
            json.SetDictValue("msg", "SignMachine 0.01");
            context.Response.Write(json.ToString());
            return;
        }
        private static async Task<NeoDun.Message> WaitBack(Watcher watcher, NeoDun.Message msg)
        {
            while (true)
            {
                await Task.Delay(5);
                MessagePair p;
                if (watcher.onRecvs.TryDequeue(out p))
                {
                    if (p.srcmsg == msg)
                        return p.msg;
                }
            }
        }

        private static int timeoutTime = 30000;
        private static int time = 0;

        private static bool linking = false;
        private static bool comfirming = false;
        private static IOwinContext iOwinContext;
        private static IOwinContext iOwinContext_pw;
        //增加地址
        private static void  addAddress(IOwinContext context, FormData formdata)
        {
            /// privateKey : 35673ECF9F18F44EBD1C9FC7CAC4327D72EEB08104EFFD20FBBB621341B80AC1
            if (formdata.mapParams.ContainsKey("wif") == false)
            {
                context.Response.Write("need param，privatekey");
                return;
            }
            string _wif = formdata.mapParams["wif"];
            byte[] privateKey = NeoDun.SignTool.GetPrivateKeyFromWif(_wif);
            byte[] publicKey = NeoDun.SignTool.GetPublicKeyFromPrivateKey(privateKey);
            string str_address = NeoDun.SignTool.GetAddressFromPublicKey(publicKey);

            iOwinContext = context;
            driver_win.DriverCtr.Ins.addAddressEventHandlerCallBack += addAddressCallBack;
            driver_win.DriverCtr.Ins.AddAddress(str_address, _wif);
            while (linking)
            {
                System.Threading.Thread.Sleep(100);
                time += 100;
                if (time > timeoutTime)
                {
                    time = 0;
                    linking = false;
                    iOwinContext.Response.Write("timeout");
                    driver_win.DriverCtr.Ins.GetAddressList();
                }
            }
        }
        private static void addAddressCallBack(bool suc)
        {
            if (iOwinContext == null)
                return;
            MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
            json["tag"] = new MyJson.JsonNode_ValueNumber(0);
            if (suc)
            {
                json["msg"] = new MyJson.JsonNode_ValueString("success");
            }
            else
            {
                json["msg"] = new MyJson.JsonNode_ValueString("failed");
            }
            linking = false;
            iOwinContext.Response.Write(json.ToString());
            driver_win.DriverCtr.Ins.addAddressEventHandlerCallBack -= addAddressCallBack;
            iOwinContext = null;
        }

        //删除地址
        private static void delAddress(IOwinContext context ,FormData formdata)
        {
            if (formdata.mapParams.ContainsKey("addresstext") == false)
            {
                context.Response.Write("need param，addresstext");
                return;
            }
            string addressType = formdata.mapParams.ContainsKey("privatetype") ? formdata.mapParams["privatetype"] : "Neo";
            string addressText = formdata.mapParams["addresstext"];

            iOwinContext = context;
            driver_win.DriverCtr.Ins.deleteAddressEventHandlerCallBack += delAddressCallBack;
            driver_win.DriverCtr.Ins.DeleteAddress(addressType, addressText);
            while (linking)
            {
                System.Threading.Thread.Sleep(100);
                time += 100;
                if (time > timeoutTime)
                {
                    time = 0;
                    linking = false;
                    iOwinContext.Response.Write("timeout");
                    driver_win.DriverCtr.Ins.GetAddressList();
                }
            }

        }
        private static void delAddressCallBack(bool suc)
        {
            if (iOwinContext == null)
                return;
            MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
            json["tag"] = new MyJson.JsonNode_ValueNumber(0);
            if (suc)
            {
                json["msg"] = new MyJson.JsonNode_ValueString("success");
            }
            else
            {
                json["msg"] = new MyJson.JsonNode_ValueString("failed");
            }
            linking = false;
            iOwinContext.Response.WriteAsync(json.ToString());
            driver_win.DriverCtr.Ins.deleteAddressEventHandlerCallBack -= delAddressCallBack;
            iOwinContext = null;
        }

        //获取私钥
        private static void addressinfo(IOwinContext context, FormData formdata)
        {
            if (formdata.mapParams.ContainsKey("addresstext") == false)
            {
                context.Response.Write("need param , addresstext");
                return;
            }
            string addressType = formdata.mapParams.ContainsKey("privatetype") ? formdata.mapParams["privatetype"] : "Neo";
            string addressText = formdata.mapParams["addresstext"];

            iOwinContext = context;
            driver_win.DriverCtr.Ins.backUpAddressEventHandlerCallBack += addressinfoCallBack;
            driver_win.DriverCtr.Ins.BackUpAddress(addressType, addressText);
            while (linking)
            {
                System.Threading.Thread.Sleep(100);
                time += 100;
                if (time > timeoutTime)
                {
                    time = 0;
                    linking = false;
                    iOwinContext.Response.Write("timeout");
                    driver_win.DriverCtr.Ins.GetAddressList();
                }
            }
        }
        private static void addressinfoCallBack(bool suc,string _privateKey)
        {
            if (iOwinContext == null)
                return;
            MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
            if (suc)
            {
                json["tag"] = new MyJson.JsonNode_ValueNumber(0);
                json["msg"] = new MyJson.JsonNode_ValueString("success");
                json["prikey"] = new MyJson.JsonNode_ValueString(_privateKey);
            }
            else
            {
                json["tag"] = new MyJson.JsonNode_ValueNumber(-1);
                json["msg"] = new MyJson.JsonNode_ValueString("faild");
            }
            linking = false;
            iOwinContext.Response.WriteAsync(json.ToString());

            driver_win.DriverCtr.Ins.backUpAddressEventHandlerCallBack -= addressinfoCallBack;
            iOwinContext = null;
        }


        //签名
        private static void sign(IOwinContext context, FormData formdata)
        {
            if (formdata.mapParams.ContainsKey("data") == false||formdata.mapParams.ContainsKey("source") == false)
            {
                context.Response.Write("need param，data & source");
                return;
            }

            iOwinContext = context;
            driver_win.DriverCtr.Ins.signEventHandlerCallBack += signCallBack;
            driver_win.DriverCtr.Ins.Sign(formdata.mapParams["data"], formdata.mapParams["source"]);
            while (linking)
            {
                System.Threading.Thread.Sleep(100);
                time += 100;
                if (time > timeoutTime)
                {
                    linking = false;
                    time = 0;
                    iOwinContext.Response.Write("timeout");
                    driver_win.DriverCtr.Ins.GetAddressList();
                }
            }

        }
        private static void signCallBack(byte[] outdata,string hashstr,bool suc)
        {
            if (iOwinContext == null)
                return;
            //读出来，拼为http响应，发回去
            MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
            if (suc)
            {
                json["tag"] = new MyJson.JsonNode_ValueNumber(0);
                json["msg"] = new MyJson.JsonNode_ValueString("success");
                json["srchash"] = new MyJson.JsonNode_ValueString(hashstr);

                var pubkeylen = outdata[0];
                var pubkey = new byte[pubkeylen];
                Array.Copy(outdata, 1, pubkey, 0, pubkeylen);
                var signdata = outdata.Skip(pubkeylen + 1).ToArray();

                json["signdata"] = new MyJson.JsonNode_ValueString(SignTool.Bytes2HexString(signdata, 0, signdata.Length));
                json["pubkey"] = new MyJson.JsonNode_ValueString(SignTool.Bytes2HexString(pubkey, 0, pubkey.Length));
            }
            else
            {
                json["tag"] = new MyJson.JsonNode_ValueNumber(-1);
                json["msg"] = new MyJson.JsonNode_ValueString("faild");
            }
            linking = false;
            iOwinContext.Response.Write(json.ToString());

            driver_win.DriverCtr.Ins.signEventHandlerCallBack -= signCallBack;
            iOwinContext = null;
        }


        //验证密码
        private static void comfirmpassword(IOwinContext context, FormData formdata)
        {
            if (formdata.mapParams.ContainsKey("password") == false)
            {
                context.Response.Write("need param，password");
                return;
            }

            string password = formdata.mapParams["password"];

            //判断密码是否是6位并且全部是数字
            if (password.Length == 6 && !Regex.IsMatch(password, @"^\d*$"))
            {
                context.Response.Write("need six length and number password");
                return;
            }

            iOwinContext_pw = context;
            driver_win.DriverCtr.Ins.confirmPasswordEventHandlerCallBack2 += comfirmpasswordCallBack;
            driver_win.DriverCtr.Ins.confirmPasswordfaildEventHandlerCallBack2 += comfirmpasswordFaildCallBack;
            driver_win.DriverCtr.Ins.SetOrConfirmPassword(password);
            while (comfirming)
            {
                int time = 0;
                System.Threading.Thread.Sleep(100);
                time += 100;
                if (time > 30000)
                {
                    context.Response.Write("timeout");
                    driver_win.DriverCtr.Ins.GetAddressList();
                    break;
                }
            }
        }
        //成功验证回掉
        private static void comfirmpasswordCallBack()
        {
            if (iOwinContext_pw == null)
                return;
            MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
            json["tag"] = new MyJson.JsonNode_ValueNumber(0);
            json["msg"] = new MyJson.JsonNode_ValueString("success");
            iOwinContext_pw.Response.Write(json.ToString());
            comfirming = false;
        }
        //失败验证回掉
        private static void comfirmpasswordFaildCallBack()
        {
            if (iOwinContext == null)
                return;
            MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
            json["tag"] = new MyJson.JsonNode_ValueNumber(0);
            json["msg"] = new MyJson.JsonNode_ValueString("faild");
            comfirming = false;
            iOwinContext_pw.Response.Write(json.ToString());
            driver_win.DriverCtr.Ins.confirmPasswordEventHandlerCallBack2 -= comfirmpasswordCallBack;
            driver_win.DriverCtr.Ins.confirmPasswordfaildEventHandlerCallBack2 -= comfirmpasswordFaildCallBack;
            iOwinContext_pw = null;
        }
    }





}
