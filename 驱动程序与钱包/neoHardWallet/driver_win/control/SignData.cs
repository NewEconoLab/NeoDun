using driver_win.helper;
using NeoDun;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace driver_win.control
{
    class SignData : Control
    {
        public override void HandleMsg(params object[] _params)
        {
            EnumError enumError = (EnumError)_params[0];
            MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
            if (enumError == EnumError.SignSuc)
            {
                byte[] _outdata = (byte[])_params[1];
                var pubkeylen = _outdata[0];
                var pubkey = new byte[pubkeylen];
                Array.Copy(_outdata, 1, pubkey, 0, pubkeylen);
                var signdata = _outdata.Skip(pubkeylen + 1).ToArray();
                json["signdata"] = new MyJson.JsonNode_ValueString(SignTool.Bytes2HexString(signdata, 0, signdata.Length));
                json["pubkey"] = new MyJson.JsonNode_ValueString(SignTool.Bytes2HexString(pubkey, 0, pubkey.Length));
                json["tag"] = new MyJson.JsonNode_ValueNumber(0);
            }
            else
            {
                json["signdata"] = new MyJson.JsonNode_ValueString("");
                json["pubkey"] = new MyJson.JsonNode_ValueString("");
                json["tag"] = new MyJson.JsonNode_ValueNumber(0);
            }
            result.errorCode = enumError;
            result.data = json;
        }

        public async override void SendMsg(params object[] _params)
        {
            string str_data = (string)_params[0];
            string str_address = (string)_params[1];

            var data = NeoDun.SignTool.HexString2Bytes(str_data);
            var hash = NeoDun.SignTool.ComputeSHA256(data, 0, data.Length);
            var hashstr = NeoDun.SignTool.Bytes2HexString(hash, 0, hash.Length);

            //发送待签名数据块
            var block = signer.dataTable.newOrGet(hashstr, (UInt32)data.Length, NeoDun.DataBlockFrom.FromDriver);
            block.data = data;
            signer.SendDataBlock(block);
            var __block = signer.dataTable.getBlockBySha256(hashstr);

            uint remoteid = await __block.GetRemoteid();

            NeoDun.Message signMsg = new NeoDun.Message();

            {//发送签名报文
                var add = signer.addressPool.getAddress(NeoDun.AddressType.Neo, str_address);
                MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                if (add == null)
                {
                    json["signdata"] = new MyJson.JsonNode_ValueString("");
                    json["pubkey"] = new MyJson.JsonNode_ValueString("");
                    json["tag"] = new MyJson.JsonNode_ValueNumber(0);
                    json["msg"] = new MyJson.JsonNode_ValueString("0206");
                    result.data = result;
                    Release();
                    return;
                }

                var addbytes = add.GetAddbytes();
                signMsg.tag1 = 0x02;
                signMsg.tag2 = 0x05;//签
                signMsg.msgid = NeoDun.SignTool.RandomShort();
                signMsg.writeUInt16(0, (UInt16)add.type);//neo tag
                Array.Copy(addbytes, 0, signMsg.data, 2, addbytes.Length);//addbytes

                //这个dataid 要上一个block 传送完毕了才知道
                signMsg.writeUInt32(42, (uint)remoteid);
                signer.SendMessage(signMsg, true);
            }
        }
    }
}
