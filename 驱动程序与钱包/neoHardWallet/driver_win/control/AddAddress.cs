using driver_win.helper;
using NeoDun;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace driver_win.control
{
    class AddAddress:Control
    {
        public override void HandleMsg(params object[] _params)
        {
            result.errorCode = (EnumError)_params[0];
        }

        public async override Task<bool> SendMsg(params object[] _params)
        {
            string wif = (string)_params[0];
            byte[] privateKey;
            byte[] publicKey;
            string str_address = "";
            try
            {
                privateKey = NeoDun.SignTool.GetPrivateKeyFromWif(wif);
                publicKey = NeoDun.SignTool.GetPublicKeyFromPrivateKey(privateKey);
                str_address = NeoDun.SignTool.GetAddressFromPublicKey(publicKey);
            }
            catch (Exception e)
            {
                result.errorCode = EnumError.IncorrectWif;
                return false;
            }

            //地址查重 
            foreach (var add in signer.addressPool.addresses)
            {
                if (add.AddressText == str_address)
                {
                    result.errorCode = EnumError.DuplicateWif;
                    return false;
                }
            }
            try
            {
                //将私钥进行aes加密
                privateKey = SignTool.AesEncrypt(privateKey,ECDH.Ins.M);

                string str_addressType = "Neo";
                UInt16 addressType = (UInt16)Enum.Parse(typeof(AddressType), str_addressType);
                byte[] bytes_address = NeoDun.SignTool.DecodeBase58(str_address);
                byte[] hash = NeoDun.SignTool.ComputeSHA256(privateKey, 0, privateKey.Length);
                string str_hash = NeoDun.SignTool.Bytes2HexString(hash, 0, hash.Length);

                NeoDun.DataBlock block = signer.dataTable.newOrGet(str_hash, (UInt32)privateKey.Length, NeoDun.DataBlockFrom.FromDriver);
                block.data = privateKey;
                signer.SendDataBlock(block);

                uint remoteid;
                var __block = signer.dataTable.getBlockBySha256(str_hash);
                while (true)
                {
                    await Task.Delay(100);
                    if (__block.dataidRemote > 0 && __block.Check())
                    {
                        remoteid = __block.dataidRemote;
                        break;
                    }
                }
                //uint remoteid = await __block.GetRemoteid();
                if (remoteid == 0)
                {
                    result.errorCode = EnumError.CommonFailed;
                    Release();
                    return false;
                }
                __block.dataidRemote = 0;

                NeoDun.Message signMsg = new NeoDun.Message();
                signMsg.tag1 = 0x02;
                signMsg.tag2 = 0x04;//增
                signMsg.msgid = NeoDun.SignTool.RandomShort();
                signMsg.writeUInt16(0, addressType);
                Array.Copy(bytes_address, 0, signMsg.data, 2, bytes_address.Length);
                //这个dataid 要上一个block 传送完毕了才知道
                signMsg.writeUInt32(42, (uint)remoteid);
                signer.SendMessage(signMsg, true);
                return true;
            }
            catch (Exception e)
            {
                result.errorCode = EnumError.IncorrectWif;
                Release();
                return false;
            }
        }
    }
}
