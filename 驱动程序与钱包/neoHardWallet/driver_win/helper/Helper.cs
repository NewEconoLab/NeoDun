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
        void SendMsg(params object[] _params);
        void HandleMsg(params object[] _params);
    }

    struct Result
    {
        public EnumError errorCode;
        public object data;
    }

    enum EnumInstallType
    {
        Framework = 0x0000,
        Plugin = 0x0001,
    }

    enum EnumPluginType
    {
        Unknow = 0x0000,
        Neo = 0x0101,
    }

    enum EnumControl
    {
        Common,
        GetAddress,
        AddAddress,
        DelAddress,
        SetName,
        GetPackage,
        ApplyInstallFramework,
        InstallFramework,
        SignData,
        InstallPlugin,
        UninstallPlugin
    }


    enum EnumError
    {
        Doing = 0x0000,   //正在执行

        CommonSuc = 0x0001, //常规成功
        CommonFailed = 0x0002,//常规失败

        AddAddressSuc=0x02a4,//增加地址成功
        DelAddressSuc=0x02a3,//删除地址成功
        SetNameSuc = 0x02a2,//设置名字成功
        SignSuc=0x02a5,//签名成功
        AgreeInstallFramework = 0x03a2,//同意更新固件
        InstallSuc=0x03a1,//成功安裝固件或插件
        UninstallSuc = 0x03a3, //卸載插件成功

        AddAddressFailed = 0x02e4,//增加地址失败
        DelAddressFailed = 0x02e3,//删除地址失败
        SetNameFailed = 0x02e2,//设置名字失败
        SignFailed = 0x02e5,//签名失败
        InstallFailed = 0x03e1,//安裝固件或插件失敗
        RefuseInstallFramework = 0x03e2,//拒絕更新固件
        UninstallFailed = 0x03e3,//卸載插件失敗

        IncorrectWif = 0x0205,//错误的wif
        DuplicateWif = 0x0203,//重复导入了wif

        LongName = 0x0601,//名字太长

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
