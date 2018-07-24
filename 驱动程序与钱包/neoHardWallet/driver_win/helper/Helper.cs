using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace driver_win.helper
{
    interface IControl
    {
        Task<Result> ToDo(params object[] _params);
        void Done(params object[] _params);
        void Release();
        void sendMsg(params object[] _params);
    }

    struct Result
    {
        public EnumError errorCode;
        public object data;
    }

    enum EnumControl
    {
        GetAddress,
        AddAddress,
        DelAddress,
        SetName,
        GetPackage,
    }


    enum EnumError
    {
        Doing = 0x0000,

        CommonSuc = 0x0001,
        CommonFailed = 0x0002,

        AddAddressSuc=0x02a4,
        DelAddressSuc=0x02a3,
        SetNameSuc = 0x2a2,

        AddAddressFailed = 0x02e4,
        DelAddressFailed = 0x02e3,
        SetNameFailed = 0x02e2,

        IncorrectWif = 0x0205,
        DuplicateWif = 0x0203,

        LongName = 0x0601,

    }

    public class HttpHelper
    {
        public static string MakeRpcUrlPost(string url, string method, out byte[] data, params MyJson.IJsonNode[] _params)
        {
            //if (url.Last() != '/')
            //    url = url + "/";
            var json = new MyJson.JsonNode_Object();
            json["id"] = new MyJson.JsonNode_ValueNumber(1);
            json["jsonrpc"] = new MyJson.JsonNode_ValueString("2.0");
            json["method"] = new MyJson.JsonNode_ValueString(method);
            StringBuilder sb = new StringBuilder();
            var array = new MyJson.JsonNode_Array();
            for (var i = 0; i < _params.Length; i++)
            {

                array.Add(_params[i]);
            }
            json["params"] = array;
            data = System.Text.Encoding.UTF8.GetBytes(json.ToString());
            return url;
        }
        public static string MakeRpcUrl(string url, string method, params MyJson.IJsonNode[] _params)
        {
            StringBuilder sb = new StringBuilder();
            if (url.Last() != '/')
                url = url + "/";

            sb.Append(url + "?jsonrpc=2.0&id=1&method=" + method + "&params=[");
            for (var i = 0; i < _params.Length; i++)
            {
                _params[i].ConvertToString(sb);
                if (i != _params.Length - 1)
                    sb.Append(",");
            }
            sb.Append("]");
            return sb.ToString();
        }
        /// <summary>
        /// 同步get请求
        /// </summary>
        /// <param name="url">链接地址</param>    
        /// <param name="formData">写在header中的键值对</param>
        /// <returns></returns>
        public static async Task<string> HttpGet(string url)
        {
            WebClient wc = new WebClient();
            return await wc.DownloadStringTaskAsync(url);
        }
        public static async Task<string> HttpPost(string url, byte[] data)
        {
            WebClient wc = new WebClient();
            wc.Headers["content-type"] = "text/plain;charset=UTF-8";
            byte[] retdata = await wc.UploadDataTaskAsync(url, "POST", data);
            return System.Text.Encoding.UTF8.GetString(retdata);
        }
    }
}
