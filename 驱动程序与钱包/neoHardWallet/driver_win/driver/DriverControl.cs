using hhgate;
using Microsoft.Owin;
using NeoDun;
using System;
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

        private MyJson.JsonNode_Object isFirstConfirm = new MyJson.JsonNode_Object();

        private MyJson.JsonNode_Object json_setting = new MyJson.JsonNode_Object();
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

            signer.errorEventHandler += ErrorCallBack;
        }

        private void UInit()
        {
            signer.addAddressEventHandler -= AddAddressCallBack;
            signer.getAddressListEventHandler -= GetAddressCallBack;
            signer.delAddressEventHandler -= DeleteAddressCallBack;
            signer.getPackageInfoEventHandler -= GetPackageInfoCallBack;
            signer.errorEventHandler -= ErrorCallBack;

        }

        #region 连接签名机
        bool _islinking = false;
        public void LinkSinger()
        {
            System.Windows.Threading.DispatcherTimer timer = new System.Windows.Threading.DispatcherTimer();
            timer.Interval = TimeSpan.FromSeconds(1.0);
            timer.Tick += new EventHandler(async (_s, _e) => {
                if (!string.IsNullOrEmpty(signer.CheckDevice()))//有签名机连接了
                {
                    //只在第一次连接的时候    连接成功后去请求签名机的状态
                    if (!_islinking && "bootloader" != signer.CheckDevice())
                    {
                        _islinking = true;
                        LinkCallBack("连接成功", System.Windows.Visibility.Collapsed, true);
                        ShowBalloonTipCallBack("NeoDun已经连接");
                    }
                }
                else
                {
                    if (_islinking)
                    {
                        ShowBalloonTipCallBack("NeoDun已经拔出");
                        _islinking = false;
                    }
                    LinkCallBack("连接中……", System.Windows.Visibility.Collapsed, false);
                }
                await Task.Delay(1000);
            });
            timer.Start();
        }
        public delegate void LinkSingerEventHandlerCallBack(string str, System.Windows.Visibility visibility, bool _islink);
        public event LinkSingerEventHandlerCallBack linkSingerEventHandlerCallBack;
        public void LinkCallBack(string str, System.Windows.Visibility visibility, bool _islink)
        {
            if (linkSingerEventHandlerCallBack != null)
                linkSingerEventHandlerCallBack(str, visibility, _islink);
        }

        public delegate void ShowBalloonTipEventHandlerCallBack(string str);
        public event ShowBalloonTipEventHandlerCallBack showBalloonTipEventHandlerCallBack;
        public void ShowBalloonTipCallBack(string str)
        {
            if (showBalloonTipEventHandlerCallBack != null)
                showBalloonTipEventHandlerCallBack(str);
        }
        #endregion


        #region
        MyJson.JsonNode_Array JA_PackageInfo = new MyJson.JsonNode_Array();
        public async Task<MyJson.JsonNode_Array> GetPackageInfo()
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
            return JA_PackageInfo;
        }
        public void GetPackageInfoCallBack(byte[] data)
        {
            MyJson.JsonNode_Array Ja_packageInfo = new MyJson.JsonNode_Array();
            string appVersion = data[0] + "." + data[1];
            MyJson.JsonNode_Object myjson = new MyJson.JsonNode_Object();
            myjson["type"] = new MyJson.JsonNode_ValueString("gujian");
            myjson["version"] = new MyJson.JsonNode_ValueString(appVersion);
            //获取有几种插件
            for (var i = 2; i < data.Length; i = i + 4)
            {
                var type = (NeoDun.AddressType)BitConverter.ToInt16(data, i);
                var version = data[i + 2] + "." + data[i + 3];
                myjson = new MyJson.JsonNode_Object();
                myjson["type"] = new MyJson.JsonNode_ValueString(type.ToString());
                myjson["version"] = new MyJson.JsonNode_ValueString(version);
                Ja_packageInfo.Add(myjson);
            }
            JA_PackageInfo = Ja_packageInfo;
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

        public async void UpdateApp(byte[] data)
        {
            try
            {
                string wif = "L2EHemxzCYKxhH81QVwPDwUT5Bd8yBgbPt7GnUFpGuttiiYroRFi"; //"JceDDbDThUsUMbxbAdjtvvqfqM1ziwFUp4eyH6NKU4JMave1rBg9BDy3yvA2bWvDEaNt9vHSHMQUeDDhDFhEA9DuTq3kqFbMgcwgQRmqZ54FhRgp92k5TLhsWBAQUL18MAfHcWq98gYWDg"
                //对未加密的数据包hash以方便下位机验证
                byte[] hash = NeoDun.SignTool.ComputeSHA256(data, 0, data.Length);

                //经过ecc加密之后的hash
                byte[] data_ecc = SignTool.EccEncrypt(hash, wif);


                //拼装新数据包
                byte[] dataHead = new byte[] { (byte)data_ecc.Length };
                data = dataHead.Concat(data_ecc).Concat(data).ToArray();
                hash = NeoDun.SignTool.ComputeSHA256(data, 0, data.Length);
                string str_hash = NeoDun.SignTool.Bytes2HexString(hash, 0, hash.Length); ;


                var length = data.Length;
                if (length > 50 * 1024)
                {

                }

                NeoDun.DataBlock block = signer.dataTable.newOrGet(str_hash, (UInt32)data.Length, NeoDun.DataBlockFrom.FromDriver);
                block.data = data;
                signer.SendDataBlock(block);

                var __block = signer.dataTable.getBlockBySha256(str_hash);
                uint remoteid = await __block.GetRemoteid();
                __block.dataidRemote = 0;

                NeoDun.Message signMsg = new NeoDun.Message();
                signMsg.tag1 = 0x02;
                signMsg.tag2 = 0x0b;
                signMsg.msgid = NeoDun.SignTool.RandomShort();
                Array.Copy(hash, 0, signMsg.data, 0, hash.Length);
                //这个dataid 要上一个block 传送完毕了才知道
                signMsg.writeUInt32(42, remoteid);
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
