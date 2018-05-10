using hhgate;
using Microsoft.Owin;
using NeoDun;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace driver_win
{
    public class DriverControl : NeoDun.IWatcher
    {

        private static Signer signer = Signer.Ins;

        private int waitTime = 30000;

        private bool needLoop = true;

        private bool[] bools = new bool[6];

        public DriverControl()
        {
            signer.Start();
            Init();
        }
        public void OnRecv(Message recv, Message src)
        {
            throw new NotImplementedException();
        }
        public void OnSend(Message send, bool needBack)
        {
            throw new NotImplementedException();
        }

        private void Init()
        {
            //初始化各种委托
            signer.addAddressEventHandler += AddAddressCallBack;
            signer.getAddressListEventHandler += GetAddressCallBack;
            signer.delAddressEventHandler += DeleteAddressCallBack;
            signer.getPackageInfoEventHandler += GetPackageInfoCallBack;
            signer.updateApp += UpdateApp;
            signer.errorEventHandler += ErrorCallBack;
        }

        private void UInit()
        {
            signer.addAddressEventHandler -= AddAddressCallBack;
            signer.getAddressListEventHandler -= GetAddressCallBack;
            signer.delAddressEventHandler -= DeleteAddressCallBack;
            signer.getPackageInfoEventHandler -= GetPackageInfoCallBack;
            signer.errorEventHandler -= ErrorCallBack;
            signer.updateApp -= UpdateApp;
        }

        #region 
        public string CheckDevice()
        {
           return signer.CheckDevice();
        }
        #endregion

        #region 查询固件版本信息
        public MyJson.JsonNode_Object Jo_PackageInfo = new MyJson.JsonNode_Object();
        public async Task<MyJson.JsonNode_Object> GetPackageInfo()
        {
            NeoDun.Message msg = new NeoDun.Message();
            msg.tag1 = 0x03;
            msg.tag2 = 0x04;//查
            msg.msgid = NeoDun.SignTool.RandomShort();
            signer.SendMessage(msg, true);
            needLoop = true;
            int time = 0;
            while (needLoop && time <= waitTime)
            {
                await Task.Delay(100);
                time += 100;
            }
            needLoop = false;
            return Jo_PackageInfo;
        }
        public void GetPackageInfoCallBack(byte[] data)
        {
            string appVersion = data[0] + "." + data[1];
            if (appVersion == "0.0")
                return;

            Jo_PackageInfo["gj"] = new MyJson.JsonNode_ValueString(appVersion);
            //获取有几种插件
            for (var i = 2; i < data.Length; i = i + 4)
            {
                var version = data[i + 2] + "." + data[i + 3];
                if (version == "0.0")
                    break;
                var type = (NeoDun.AddressType)BitConverter.ToInt16(data, i);
                Jo_PackageInfo[type.ToString()] = new MyJson.JsonNode_ValueString(version);
            }
            needLoop = false;
        }
        #endregion

        #region 增加地址
        bool addResult;
        public async Task<string> AddAddressByWif(string wif)
        {
            byte[] privateKey = NeoDun.SignTool.GetPrivateKeyFromWif(wif);
            byte[] publicKey = NeoDun.SignTool.GetPublicKeyFromPrivateKey(privateKey);
            string str_address = NeoDun.SignTool.GetAddressFromPublicKey(publicKey);

            //地址查重 
            foreach (var add in signer.addressPool.addresses)
            {
                if (add.AddressText == str_address)
                {
                    return "地址重复";

                }
            }
            try
            {
                string str_addressType = "Neo";
                UInt16 addressType = (UInt16)Enum.Parse(typeof(AddressType), str_addressType);
                byte[] bytes_address = NeoDun.SignTool.DecodeBase58(str_address);
                byte[] hash = NeoDun.SignTool.ComputeSHA256(privateKey, 0, privateKey.Length);
                string str_hash = NeoDun.SignTool.Bytes2HexString(hash, 0, hash.Length);

                NeoDun.DataBlock block = signer.dataTable.newOrGet(str_hash, (UInt32)privateKey.Length, NeoDun.DataBlockFrom.FromDriver);
                block.data = privateKey;
                signer.SendDataBlock(block);

                var __block = signer.dataTable.getBlockBySha256(str_hash);
                uint remoteid = await __block.GetRemoteid();
                __block.dataidRemote = 0;

                NeoDun.Message signMsg = new NeoDun.Message();
                signMsg.tag1 = 0x02;
                signMsg.tag2 = 0x04;//增
                signMsg.msgid = NeoDun.SignTool.RandomShort();
                signMsg.writeUInt16(0, addressType);
                Array.Copy(bytes_address, 0, signMsg.data, 2, bytes_address.Length);
                //这个dataid 要上一个block 传送完毕了才知道
                signMsg.writeUInt32(42, remoteid);
                signer.SendMessage(signMsg, true);

                needLoop = true;
                addResult = false;
                int time = 0;
                while (needLoop && time <= waitTime)
                {
                    await Task.Delay(100);
                    time += 100;
                }
                needLoop = false;
                return addResult ? "suc" : "Neodun拒绝";
            }
            catch (Exception e)
            {
                return "请输入正确的私钥";
            }
        }
        public void AddAddressCallBack(bool _suc)
        {
            addResult = _suc;
            needLoop = false;
        }
        #endregion

        #region 查询地址
        public async Task<System.Collections.Concurrent.ConcurrentBag<Address>> GetAddressList()
        {
            signer.InitAddressPool();

            NeoDun.Message msg = new NeoDun.Message();
            msg.tag1 = 0x02;
            msg.tag2 = 0x01;//查
            msg.msgid = NeoDun.SignTool.RandomShort();
            signer.SendMessage(msg, true);
            needLoop = true;
            int time = 0;
            while (needLoop && time <= waitTime)
            {
                await Task.Delay(100);
                time += 100;
            }
            needLoop = false;
            return signer.addressPool.addresses;
        }
        public void GetAddressCallBack()
        {
            needLoop = false;
        }
        #endregion

        #region 删除地址
        bool deleteResult = false;
        public async Task<string> DeleteAddress(string _addressType, string _addressText)
        {
            UInt16 addressType = (UInt16)Enum.Parse(typeof(AddressType), _addressType);
            byte[] bytes_addressText = NeoDun.SignTool.DecodeBase58(_addressText);
            NeoDun.Message signMsg = new NeoDun.Message();
            signMsg.tag1 = 0x02;
            signMsg.tag2 = 0x03;//删除
            signMsg.msgid = NeoDun.SignTool.RandomShort();
            signMsg.writeUInt16(0, addressType);
            Array.Copy(bytes_addressText, 0, signMsg.data, 2, bytes_addressText.Length);
            signer.SendMessage(signMsg, true);

            deleteResult = false;
            needLoop = true;
            int time = 0;
            while (needLoop && time <= waitTime)
            {
                await Task.Delay(100);
                time += 100;
            }
            needLoop = false;
            return deleteResult?"删除成功":"Neodun拒绝";
        }
        public void DeleteAddressCallBack(bool suc)
        {
            deleteResult = suc;
            needLoop = false;
        }

        #endregion

        #region 安装更新app
        //请求更新固件
        bool applyResult = false;
        public async Task<bool> ApplyForUpdate()
        {
            NeoDun.Message msg = new NeoDun.Message();
            msg.tag1 = 0x03;
            msg.tag2 = 0x02;
            msg.msgid = NeoDun.SignTool.RandomShort();
            signer.SendMessage(msg, true);
            applyResult = false;
            needLoop = true;
            int time = 0;
            while (needLoop && time <= waitTime)
            {
                await Task.Delay(100);
                time += 100;
            }
            needLoop = false;
            return applyResult;
        }
        public void ApplyForUpdateCallBack(bool suc)
        {
            applyResult = suc;
            needLoop = false;
        }


        public async void UpdateApp(byte[] data,UInt16 type,UInt16 content, UInt16 version)
        {
            try
            {
                var hash = NeoDun.SignTool.ComputeSHA256(data, 0,data.Length);
                string str_hash = NeoDun.SignTool.Bytes2HexString(hash, 0, hash.Length);
                int num = data.Length / (1024 * 50) + 1;

                List<UInt32> remoteids = new List<UInt32>();

                for (var i = 0; i < num; i++)
                {
                    byte[] _data = data.Skip(1024*50*i).Take(1024*50).ToArray();
                    var _hash = NeoDun.SignTool.ComputeSHA256(_data, 0, _data.Length);
                    string _str_hash = NeoDun.SignTool.Bytes2HexString(_hash, 0, _hash.Length);
                    NeoDun.DataBlock block = signer.dataTable.newOrGet(_str_hash, (UInt32)_data.Length, NeoDun.DataBlockFrom.FromDriver);
                    block.data = _data;
                    signer.SendDataBlock(block);

                    var __block = signer.dataTable.getBlockBySha256(_str_hash);
                    UInt32 remoteid = await __block.GetRemoteid();
                    remoteids.Add(remoteid);
                    __block.dataidRemote = 0;
                }
                NeoDun.Message signMsg = new NeoDun.Message();
                signMsg.tag1 = 0x03;
                signMsg.tag2 = 0x01;
                signMsg.msgid = NeoDun.SignTool.RandomShort();
                signMsg.writeUInt16(0, type);
                signMsg.writeUInt16(2, content);
                Array.Copy(hash, 0, signMsg.data, 4, hash.Length);
                signMsg.writeUInt16(40, version);
                //这个dataid 要上一个block 传送完毕了才知道
                //for (var i = 0; i < remoteids.Count; i++)
                //{
                //    signMsg.writeUInt32(42, remoteids[i]);
                //}
                signer.SendMessage(signMsg, true);
            }
            catch (Exception e)
            {
            }
        }
        #endregion

        #region 各种失败汇总
        public delegate void ErrorEventHandlerCallBack(string msg, string header = "警告");
        public event ErrorEventHandlerCallBack errorEventHandlerCallBack;
        private void ErrorCallBack(string msg, string header)
        {
            if (errorEventHandlerCallBack != null)
                errorEventHandlerCallBack(msg, header);
            GetAddressList();
        }
        #endregion

    }
}
