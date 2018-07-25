using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using driver_win.helper;

namespace driver_win.control
{
    class GetPackageInfo : Control
    {

        public override void HandleMsg(params object[] _params)
        {
            result.errorCode = EnumError.CommonSuc;
            byte[] data = (byte[])_params[0];
            string appVersion = data[0] + "." + data[1];

            MyJson.JsonNode_Object Jo_PackageInfo = new MyJson.JsonNode_Object();

            Jo_PackageInfo["Framework"] = new MyJson.JsonNode_ValueString(appVersion);
            //获取有几种插件
            for (var i = 2; i < data.Length; i = i + 4)
            {
                var version = data[i + 2] + "." + data[i + 3];
                if (version == "0.0")
                    break;
                var type = (NeoDun.AddressType)BitConverter.ToInt16(data, i);
                Jo_PackageInfo[type.ToString()] = new MyJson.JsonNode_ValueString(version);
            }

            result.data = Jo_PackageInfo;
        }

        public override void SendMsg(params object[] _params)
        {
            NeoDun.Message msg = new NeoDun.Message();
            msg.tag1 = 0x03;
            msg.tag2 = 0x04;//查
            msg.msgid = NeoDun.SignTool.RandomShort();
            signer.SendMessage(msg, true);
        }
    }
}
