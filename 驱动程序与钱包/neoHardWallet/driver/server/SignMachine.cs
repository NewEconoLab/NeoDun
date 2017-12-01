using AntShares.Compiler;
using Microsoft.Owin;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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
            else if (relativePath == "sign")
            {
                await sign(context, formdata);
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
        private static async Task ver(IOwinContext context, FormData formdata)
        {
            MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
            json["tag"] = new MyJson.JsonNode_ValueNumber(0);
            MyJson.JsonNode_Array maps = new MyJson.JsonNode_Array();
            json.SetDictValue("msg", "SignMachine 0.01");
            await context.Response.WriteAsync(json.ToString());
            return;
        }
        private static async Task sign(IOwinContext context, FormData formdata)
        {
            if(formdata.mapParams.ContainsKey("data")==false
                ||formdata.mapParams.ContainsKey("source")==false)
            {
                await context.Response.WriteAsync("need param，data & source");
                return;
            }
            var src = formdata.mapParams["source"];
            var data = formdata.mapParams["data"];

            if (driver.Form1.address!= src)
            {
                await context.Response.WriteAsync("no this address info,can not sign it.");
                return;
            }

            var databin = NEO.AllianceOfThinWallet.Cryptography.Helper.HexString2Bytes(data);

            var signdata = NEO.AllianceOfThinWallet.Cryptography.Helper.Sign(databin, driver.Form1.privateKey);
            string outstr = "publicKey=" + driver.Form1.publicKey_NoComp;
            outstr+= "&signature="+
                NEO.AllianceOfThinWallet.Cryptography.Helper.Bytes2HexString(signdata);
            outstr += "&transaction=" + data;


            await context.Response.WriteAsync(outstr);
            return;
        }


    }

}
