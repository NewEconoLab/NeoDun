using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using driver_win.control;
using Microsoft.Owin;

namespace hhgate
{
    class Api : CustomServer.IParser
    {
        public static Api Ins
        {
            get
            {
                if (ins == null)
                    ins = new Api();
                return ins;
            }
        }

        private static Api ins;
        System.Threading.AutoResetEvent autoReset = new System.Threading.AutoResetEvent(false);


        //加一个锁 防止重复请求
        bool doing = false;

        public async void HandleRequest(IOwinContext context, string rootpath, string relativePath)
        {
            try
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
                else if (relativePath == "state")
                {//查询状态
                    var signer = NeoDun.Signer.Ins;
                    MyJson.JsonNode_Object jsonr = new MyJson.JsonNode_Object();
                    jsonr["tag"] = new MyJson.JsonNode_ValueNumber(0);
                    string dc = signer.CheckDevice();
                    jsonr["count"] = new MyJson.JsonNode_ValueString(dc);

                    if (!string.IsNullOrEmpty(dc))
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
                    string dc = signer.CheckDevice();
                    if (!string.IsNullOrEmpty(dc))
                    {
                        foreach (var add in signer.addressPool.addresses)
                        {
                            MyJson.JsonNode_Object item = new MyJson.JsonNode_Object();
                            adds.Add(item);
                            item["type"] = new MyJson.JsonNode_ValueString(add.type.ToString());
                            item["address"] = new MyJson.JsonNode_ValueString(add.AddressText);
                            item["name"] = new MyJson.JsonNode_ValueString(add.name);
                        }
                    }
                    context.Response.Write(jsonr.ToString());
                }
                else if (relativePath == "sign")
                {
                    if (doing)
                        return;
                    if (formdata.mapParams.ContainsKey("address") == false || formdata.mapParams.ContainsKey("data") == false)
                    {
                        context.Response.Write("need param，address & data");
                        return;
                    }
                    var address = formdata.mapParams["address"];
                    var data = formdata.mapParams["data"];

                    MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();

                    doing = true;

                    sign(json,address,data);

                    while (true)
                    {
                        if (json.Keys.Count > 0)
                        {
                            doing = false;
                            //读出来，拼为http响应，发回去
                            context.Response.Write(json.ToString());
                            break;
                        }

                    }
                }
                else
                {
                    MyJson.JsonNode_Object jsonr = new MyJson.JsonNode_Object();
                    jsonr["tag"] = new MyJson.JsonNode_ValueNumber(-1000);
                    jsonr["msg"] = new MyJson.JsonNode_ValueString("unknown cmd.");
                }
                return;

            }
            catch (Exception e)
            {
                context.Response.Write(e.ToString());
            }

            async void sign(MyJson.JsonNode_Object json,string address,string data)
            {
                driver_win.helper.Result _result = await ManagerControl.Ins.ToDo(driver_win.helper.EnumControl.SignData, data, address);
                var result = (MyJson.JsonNode_Object)_result.data;
                var enumError = _result.errorCode;
                json["msg"] = new MyJson.JsonNode_ValueString(((UInt16)enumError).ToString("x4"));
                if (result != null)
                {
                    json["signdata"] = result.ContainsKey("signdata") ? result["signdata"] : new MyJson.JsonNode_ValueString("");
                    json["pubkey"] = result.ContainsKey("pubkey") ? result["pubkey"] : new MyJson.JsonNode_ValueString("");
                }

            }


        }

    }
}
