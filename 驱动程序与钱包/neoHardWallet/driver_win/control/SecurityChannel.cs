using driver_win.helper;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace driver_win.control
{
    class SecurityChannel : Control
    {
        public override void HandleMsg(params object[] _params)
        {
            EnumError enumError = (EnumError)_params[0];
            if (enumError == EnumError.SecurityChannelSuc)
            {
                byte[] data = (byte[])_params[1];
                ECDH.Ins.pubKey_B = data;
                ECDH.Ins.CalM(data);
            }

        }

        public async override Task<bool> SendMsg(params object[] _params)
        {
            var data = ECDH.Ins.pubKey;
            var hash = NeoDun.SignTool.ComputeSHA256(data, 0, data.Length);
            var hashstr = NeoDun.SignTool.Bytes2HexString(hash, 0, hash.Length);

            //发送数据块
            var block = signer.dataTable.newOrGet(hashstr, (UInt32)data.Length, NeoDun.DataBlockFrom.FromDriver);
            block.data = data;
            signer.SendDataBlock(block);
            var __block = signer.dataTable.getBlockBySha256(hashstr);

            uint remoteid = await __block.GetRemoteid();

            NeoDun.Message signMsg = new NeoDun.Message();

            {
                signMsg.tag1 = 0x04;
                signMsg.tag2 = 0x01;
                signMsg.msgid = NeoDun.SignTool.RandomShort();

                //这个dataid 要上一个block 传送完毕了才知道
                signMsg.writeUInt32(42, (uint)remoteid);
                signer.SendMessage(signMsg, true);
            }
            return true;
        }
    }
}
