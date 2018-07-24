using driver_win.helper;
using NeoDun;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace driver_win.control
{
    class GetAddress : Control
    {

        public override void Done(params object[] _params)
        {
            result.errorCode = EnumError.CommonSuc;
            result.data = (object)signer.addressPool.addresses;
            base.Done(_params);
        }

        public override void sendMsg(params object[] _params)
        {

            signer.InitAddressPool();

            NeoDun.Message msg = new NeoDun.Message();
            msg.tag1 = 0x02;
            msg.tag2 = 0x01;//查
            msg.msgid = NeoDun.SignTool.RandomShort();

            signer.SendMessage(msg, true);


        }
    }
}
