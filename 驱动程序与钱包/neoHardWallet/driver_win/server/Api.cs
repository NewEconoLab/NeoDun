using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
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

        public driver_win.DriverControl driverControl;

        public async void HandleRequest(IOwinContext context, string rootpath, string relativePath)
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
                    }
                }
                context.Response.Write(jsonr.ToString());
            }
            else if (relativePath == "sign")
            {
                if (formdata.mapParams.ContainsKey("address") == false || formdata.mapParams.ContainsKey("data") == false)
                {
                    context.Response.Write("need param，address & data");
                    return;
                }
                var address = formdata.mapParams["address"];
                var data = formdata.mapParams["data"];
                var time = 0;
                MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                json["tag"] = new MyJson.JsonNode_ValueNumber(0);
                json["data"] = new MyJson.JsonNode_ValueString(data);
                MyJson.JsonNode_Object result = new MyJson.JsonNode_Object();
                json["msg"] = new MyJson.JsonNode_ValueString("0501");
                driverControl.Sign(data,address);
                while (time <= 10000)
                {
                    System.Threading.Thread.Sleep(100);
                    result = driverControl.result;
                    if (result.Count>0)
                    {
                        json["msg"] = result["msg"];
                        json["signdata"] = result["signdata"];
                        json["pubkey"] = result["pubkey"];
                    }
                    time += 100;
                }
                //读出来，拼为http响应，发回去
                context.Response.Write(json.ToString());
            }
            else if (relativePath == "addaddress")
            {

            }
            else if (relativePath == "deladdress")
            {

            }
            else
            {
                MyJson.JsonNode_Object jsonr = new MyJson.JsonNode_Object();
                jsonr["tag"] = new MyJson.JsonNode_ValueNumber(-1000);
                jsonr["msg"] = new MyJson.JsonNode_ValueString("unknown cmd.");
            }

            return;
        }

    }
}
