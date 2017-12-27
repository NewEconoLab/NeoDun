using Microsoft.Owin;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using NeoDun;
using driver_win;
namespace hhgate
{
    public class SignMachine : CustomServer.IParser
    {


        public async Task HandleRequest(IOwinContext context, string rootpath, string relativePath)
        {
            var api = relativePath.ToLower();
            var formdata = await FormData.FromRequest(context.Request);
            if (formdata == null)
            {
                context.Response.StatusCode = 500;
                context.Response.ContentType = "text/plain";
                MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                json["msg"] = new MyJson.JsonNode_ValueString("formdata format error.");
                json["tag"] = new MyJson.JsonNode_ValueNumber(-1);
                await context.Response.WriteAsync(json.ToString());
                return;
            }

            if (relativePath == "ver")
            {
                await ver(context, formdata);
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
                await context.Response.WriteAsync(jsonr.ToString());
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
                await context.Response.WriteAsync(jsonr.ToString());
            }
            else if (relativePath == "sign")
            {
                //await sign(context, formdata);
                await driver_win.DriverCtr.Ins.Sign(context, formdata);
                return;
            }
            else if (relativePath == "addaddress")
            {
                await addAddress(context,formdata);
                return;
            }
            else if (relativePath == "deladdress")
            {
                await delAddress(context, formdata);
                return;
            }
            else if (relativePath == "addressinfo")
            {
                await addressinfo(context,formdata);
                return;
            }
            else
            {
                MyJson.JsonNode_Object jsonr = new MyJson.JsonNode_Object();
                jsonr["tag"] = new MyJson.JsonNode_ValueNumber(-1000);
                jsonr["msg"] = new MyJson.JsonNode_ValueString("unknown cmd.");
                await context.Response.WriteAsync(jsonr.ToString());
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
        private static async Task ver(IOwinContext context, FormData formdata)
        {
            MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
            json["tag"] = new MyJson.JsonNode_ValueNumber(0);
            MyJson.JsonNode_Array maps = new MyJson.JsonNode_Array();
            json.SetDictValue("msg", "SignMachine 0.01");
            await context.Response.WriteAsync(json.ToString());
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
        private static async Task sign(IOwinContext context, FormData formdata)
        {
            if (formdata.mapParams.ContainsKey("data") == false
                || formdata.mapParams.ContainsKey("source") == false)
            {
                await context.Response.WriteAsync("need param，data & source");
                return;
            }
            var src = formdata.mapParams["source"];

            var data = NeoDun.SignTool.HexString2Bytes(formdata.mapParams["data"]);
            var hash = NeoDun.SignTool.ComputeSHA256(data, 0, data.Length);
            var hashstr = NeoDun.SignTool.Bytes2HexString(hash, 0, hash.Length);

            var signer = driver_win.MainWindow.signer;
            {//发送待签名数据块
                var block = signer.dataTable.newOrGet(hashstr, (UInt32)data.Length, NeoDun.DataBlockFrom.FromDriver);
                block.data = data;
                signer.SendDataBlock(block);//need a finish callback.
            }
            //轮询等待发送完成，需要加入超时机制
            uint remoteid = 0;
            while (true)
            {
                await Task.Delay(5);
                var __block = signer.dataTable.getBlockBySha256(hashstr);
                //Console.WriteLine("__block.dataidRemote:" + __block.dataidRemote);
                if (__block.dataidRemote > 0 && __block.Check())
                {
                    remoteid = __block.dataidRemote;
                    break;
                }
            }
            Console.WriteLine(22222222222222);
            Watcher watcher = new Watcher();
            signer.watcherColl.AddWatcher(watcher);//加入监视器

            NeoDun.Message signMsg = new NeoDun.Message();

            {//发送签名报文
                var add = signer.addressPool.getAddress(NeoDun.AddressType.Neo, src);
                var addbytes = add.GetAddbytes();

                signMsg.tag1 = 0x02;
                signMsg.tag2 = 0x0a;//签
                signMsg.msgid = NeoDun.SignTool.RandomShort();
                signMsg.writeUInt16(0, (UInt16)add.type);//neo tag
                Array.Copy(addbytes, 0, signMsg.data, 2, addbytes.Length);//addbytes

                //这个dataid 要上一个block 传送完毕了才知道
                signMsg.writeUInt32(42, remoteid);

                signer.SendMessage(signMsg, true);
            }

            var msg = await WaitBack(watcher, signMsg);
            byte[] outdata = null;
            string outdatahash = null;
            if (msg.tag1 == 0x02 && msg.tag2 == 0xa8)
            {//签名成功
                //继续努力
                outdatahash = msg.readHash256(4);
                //轮询直到reciveid的数据被收到
                while (true)
                {
                    await Task.Delay(5);
                    var __block = signer.dataTable.getBlockBySha256(outdatahash);
                    if (__block.Check())
                    {
                        outdata = __block.data;
                        break;
                    }
                }
            }

            //读出来，拼为http响应，发回去
            MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
            json["tag"] = new MyJson.JsonNode_ValueNumber(0);
            json["srchash"] = new MyJson.JsonNode_ValueString(hashstr);
            var pubkeylen = outdata[0];
            var pubkey = new byte[pubkeylen];
            Array.Copy(outdata, 1, pubkey, 0, pubkeylen);
            var signdata = outdata.Skip(pubkeylen + 1).ToArray();

            json["signdata"] = new MyJson.JsonNode_ValueString(SignTool.Bytes2HexString(signdata, 0, signdata.Length));
            json["pubkey"] = new MyJson.JsonNode_ValueString(SignTool.Bytes2HexString(pubkey, 0, pubkey.Length));

            Console.WriteLine(json.ToString());
            await context.Response.WriteAsync(json.ToString());

            signer.watcherColl.RemoveWatcher(watcher);
            return;
        } 
        private static async Task addAddress(IOwinContext context,FormData formdata)
        {
            /// privateKey : 35673ECF9F18F44EBD1C9FC7CAC4327D72EEB08104EFFD20FBBB621341B80AC1
            if (formdata.mapParams.ContainsKey("privatekey") == false)
            {
                await context.Response.WriteAsync("need param，privatekey");
                return;
            }
            string str_addressType = formdata.mapParams.ContainsKey("privatetype")? formdata.mapParams["privatetype"]:"Neo";
            UInt16 addressType = (UInt16)Enum.Parse(typeof(AddressType), str_addressType);
            byte[] privateKey = NeoDun.SignTool.HexString2Bytes(formdata.mapParams["privatekey"]);
            byte[] publicKey = NeoDun.SignTool.GetPublicKeyFromPrivateKey(privateKey);
            string str_address = NeoDun.SignTool.GetAddressFromPublicKey(publicKey);
            byte[] bytes_address = NeoDun.SignTool.DecodeBase58(str_address);
            byte[] hash = NeoDun.SignTool.ComputeSHA256(privateKey,0,privateKey.Length);
            string str_hash = NeoDun.SignTool.Bytes2HexString(hash,0,hash.Length);

            var signer = NeoDun.Signer.Ins;
            NeoDun.DataBlock block = signer.dataTable.newOrGet(str_hash,(UInt32)privateKey.Length,NeoDun.DataBlockFrom.FromDriver);
            block.data = privateKey;
            signer.SendDataBlock(block);
            //轮询等待发送完成，需要加入超时机制
            uint remoteid = 0;
            while (true)
            {
                await Task.Delay(5);
                var __block = signer.dataTable.getBlockBySha256(str_hash);
                if (__block.dataidRemote > 0 && __block.Check())
                {
                    remoteid = __block.dataidRemote;
                    break;
                }
            }
            Watcher watcher = new Watcher();
            signer.watcherColl.AddWatcher(watcher);//加入监视器
            NeoDun.Message signMsg = new NeoDun.Message();
            signMsg.tag1 = 0x02;
            signMsg.tag2 = 0x04;//增
            signMsg.msgid = NeoDun.SignTool.RandomShort();
            signMsg.writeUInt16(0, addressType);
            Array.Copy(bytes_address,0, signMsg.data,2,bytes_address.Length);
            //这个dataid 要上一个block 传送完毕了才知道
            signMsg.writeUInt32(42, remoteid);
            signer.SendMessage(signMsg, true);

            var msg = await WaitBack(watcher, signMsg);
            MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
            json["tag"] = new MyJson.JsonNode_ValueNumber(0);
            if (msg.tag1 == 0x02 && msg.tag2 == 0xb1)
            {
                json["msg"] = new MyJson.JsonNode_ValueString("success");
            }
            else
            {
                json["msg"] = new MyJson.JsonNode_ValueString("failed");
            }

            await context.Response.WriteAsync(json.ToString());

            signer.watcherColl.RemoveWatcher(watcher);
            return;

        }
        private static async Task delAddress(IOwinContext context, FormData formdata) 
        {
            if(formdata.mapParams.ContainsKey("addresstext") == false)
            {
                await context.Response.WriteAsync("need param，addresstext");
                return;
            }
            string str_addressType = formdata.mapParams.ContainsKey("privatetype") ? formdata.mapParams["privatetype"] : "Neo";
            UInt16 addressType = (UInt16)Enum.Parse(typeof(AddressType), str_addressType);
            byte[] bytes_addressText = NeoDun.SignTool.DecodeBase58(formdata.mapParams["addresstext"]);

            var signer = NeoDun.Signer.Ins;

            Watcher watcher = new Watcher();
            signer.watcherColl.AddWatcher(watcher);//加入监视器
            NeoDun.Message signMsg = new NeoDun.Message();
            signMsg.tag1 = 0x02;
            signMsg.tag2 = 0x03;//删除
            signMsg.msgid = NeoDun.SignTool.RandomShort();
            signMsg.writeUInt16(0, addressType);
            Array.Copy(bytes_addressText, 0, signMsg.data, 2, bytes_addressText.Length);
            signer.SendMessage(signMsg, true);

            MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
            var msg = await WaitBack(watcher, signMsg);
            json["tag"] = new MyJson.JsonNode_ValueNumber(0);
            //回复尚未协定好 先用增加的临时用
            if (msg.tag1 == 0x02 && msg.tag2 == 0xc1)
            {
                json["msg"] = new MyJson.JsonNode_ValueString("success");
            }
            else
            {
                json["msg"] = new MyJson.JsonNode_ValueString("failed");
            }

            await context.Response.WriteAsync(json.ToString());

            signer.watcherColl.RemoveWatcher(watcher);
            return;
        }
        private static async Task addressinfo(IOwinContext context, FormData formdata)
        {//获取私钥,暂时只想到能获取私钥，如有别的再加
            if (formdata.mapParams.ContainsKey("addresstext")==false||formdata.mapParams.ContainsKey("addresstype")==false)
            {
                await context.Response.WriteAsync("need param , addresstext&&addresstype");
                return;
            }

            string addressText = formdata.mapParams["addresstext"];
            //ushort addressType = (ushort)Enum.Parse(typeof(AddressType), formdata.mapParams["addresstype"]);

            var signer = NeoDun.Signer.Ins;
            Watcher watcher = new Watcher();
           signer.watcherColl.AddWatcher(watcher);
            driver_win.DriverCtr.Ins.privateKey = "";
           driver_win.DriverCtr.Ins.BackUpAddress(formdata.mapParams["addresstype"],addressText);
            string prikey = "";
            while (true)
            {
                if (!string.IsNullOrEmpty(driver_win.DriverCtr.Ins.privateKey))
                {
                    prikey = driver_win.DriverCtr.Ins.privateKey;
                    break;

                }
                await System.Threading.Tasks.Task.Delay(50);

            }

            //NeoDun.Message signMsg = new NeoDun.Message();
            //signMsg.tag1 = 0x02;
            //signMsg.tag2 = 0x06;
            //signMsg.writeUInt16(4, addressType);
            //byte[] hash_address = SignTool.DecodeBase58(addressText);
            //Array.Copy(hash_address,0,signMsg.data,6,hash_address.Length);
            //signer.SendMessage(signMsg,true);

            //var msg = await WaitBack(watcher, signMsg);
            MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
            //json["tag"] = new MyJson.JsonNode_ValueNumber(0);
            //if (msg.tag1 == 0x02 && msg.tag2 == 0xa4)
            //{
            //    string outdataHash = msg.readHash256(4);
            //    byte[] outdata;
            //    //轮询直到reciveid的数据被收到
            //    while (true)
            //    {
            //        await System.Threading.Tasks.Task.Delay(5);
            //        var __block = signer.dataTable.getBlockBySha256(outdataHash);
            //        if (__block.Check())
            //        {
            //            outdata = __block.data;
            //            break;
            //        }
            //    }
            //    byte privatekeylen = outdata[0];
            //    byte[] privatekey = new byte[privatekeylen];
            //    Array.Copy(outdata, 1, privatekey, 0, privatekeylen);
            //    string prikey = NeoDun.SignTool.Bytes2HexString(privatekey, 0, privatekey.Length);
            //
            //    //string privateKey = msg.readHash256(4);
                json["prikey"] = new MyJson.JsonNode_ValueString(prikey);
                await context.Response.WriteAsync(json.ToString());
            //}
            //else
            //{
            //    json["prikey"] = new MyJson.JsonNode_ValueString("error");
            //    await context.Response.WriteAsync(json.ToString());
            //}
            //
            //signer.watcherColl.RemoveWatcher(watcher);
            //return;
        }
    }

}
