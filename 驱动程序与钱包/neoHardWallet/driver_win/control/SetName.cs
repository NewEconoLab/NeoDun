using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using driver_win.helper;

namespace driver_win.control
{
    class SetName : Control
    {

        public override void HandleMsg(params object[] _params)
        {
            result.errorCode = (EnumError)_params[0];
        }

        public override void SendMsg(params object[] _params)
        {
            string address = (string)_params[0];
            byte[] bytes_name = (byte[])_params[1];

            if (bytes_name.Length > 6)
            {
                result.errorCode = EnumError.LongName;
                return;
            }


            byte[] bytes_address = NeoDun.SignTool.DecodeBase58(address);
            NeoDun.Message msg = new NeoDun.Message();
            msg.tag1 = 0x02;
            msg.tag2 = 0x02;
            msg.msgid = NeoDun.SignTool.RandomShort();
            Array.Copy(bytes_address, 0, msg.data, 0, bytes_address.Length);
            msg.writeUInt16(26, (ushort)bytes_name.Length);
            Array.Copy(bytes_name, 0, msg.data, 28, bytes_name.Length);
            signer.SendMessage(msg, true);

        }
    }
}
