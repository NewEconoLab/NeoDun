using hhgate;
using Microsoft.Owin;
using NeoDun;
using System;
using System.Linq;
using System.Threading.Tasks;
using static hhgate.SignMachine;

namespace driver_win
{
    public class DriverCtr: NeoDun.IWatcher
    {

        private static DriverCtr ins;
        public static DriverCtr Ins
        {
            get
            {
                if (ins == null)
                    ins = new DriverCtr();
                return ins;
            }
        }

        private static Signer signer = Signer.Ins;

        private bool[] bools = new bool[6];

        private MyJson.JsonNode_Object isFirstConfirm = new MyJson.JsonNode_Object();

        private MyJson.JsonNode_Object json_setting = new MyJson.JsonNode_Object();
        private DriverCtr()
        {
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
            signer.getSingerInfoEventHandler += getSingerInfoCallBack;
            signer.addAddressEventHandler += AddAddressCallBack;
            signer.getAddressListEventHandler += GetAddressCallBack;
            signer.delAddressEventHandler += DeleteAddressCallBack;
            signer.signEventHandler += ConfirmSignCallBack;
            signer.setSettingInfoEventHandler += SetSettingInfoCallBack;
            signer.errorEventHandler += ErrorCallBack;
            signer.confirmPasswordEventHandler += SendSecretCallBack;
        }

        private void UInit()
        {
            signer.getSingerInfoEventHandler -= getSingerInfoCallBack;
            signer.addAddressEventHandler -= AddAddressCallBack;
            signer.getAddressListEventHandler -= GetAddressCallBack;
            signer.delAddressEventHandler -= DeleteAddressCallBack;
            signer.signEventHandler -= ConfirmSignCallBack;
            signer.setSettingInfoEventHandler -= SetSettingInfoCallBack;
            signer.errorEventHandler -= ErrorCallBack;

        }

        #region 连接签名机
        bool _islinking = false;
        public void LinkSinger(int count = 5,int time=5)
        {
            int _count = count;
            int _time = time;
            System.Windows.Threading.DispatcherTimer timer = new System.Windows.Threading.DispatcherTimer();
            timer.Interval = TimeSpan.FromSeconds(1.0);
            timer.Tick += new EventHandler(async (_s, _e) => {
                _time--;
                if (!string.IsNullOrEmpty(signer.CheckDevice()))//有签名机连接了
                {
                    _time = time;
                    _islinking = true;
                    //只在第一次连接的时候    连接成功后去请求签名机的状态
                    if (!_islinking && "hid" == signer.CheckDevice())
                    {
                        LinkCallBack("连接成功", System.Windows.Visibility.Collapsed, true);
                        ShowBalloonTipCallBack("NeoDun已经连接");
                        GetSingerInfo();
                    }
                }
                else
                {
                    if (_islinking)
                    {
                        ShowBalloonTipCallBack("NeoDun已经拔出");
                        _islinking = false;
                    }
                    LinkCallBack("连接中……（" + _time + "s）", System.Windows.Visibility.Collapsed, false);
                    _time = _time <= 0 ? time : _time;
                }
                await Task.Delay(1000);
            });
            timer.Start();
        }
        public delegate void LinkSingerEventHandlerCallBack(string str ,System.Windows.Visibility visibility,bool _islink);
        public event LinkSingerEventHandlerCallBack linkSingerEventHandlerCallBack;
        public void LinkCallBack(string str, System.Windows.Visibility visibility,bool _islink)
        {
            if(linkSingerEventHandlerCallBack != null)
                linkSingerEventHandlerCallBack(str, visibility, _islink);
        }

        public delegate void ShowBalloonTipEventHandlerCallBack(string str);
        public event ShowBalloonTipEventHandlerCallBack showBalloonTipEventHandlerCallBack;
        public void ShowBalloonTipCallBack(string str)
        {
            if(showBalloonTipEventHandlerCallBack != null)
                showBalloonTipEventHandlerCallBack(str);
        }
        #endregion

        #region 获取签名机中保存的驱动的默认设置
        public void GetSingerInfo()
        {
            NeoDun.Message signMsg = new NeoDun.Message();
            signMsg.tag1 = 0x02;
            signMsg.tag2 = 0x1b;
            signMsg.msgid = NeoDun.SignTool.RandomShort();
            Console.WriteLine("开始发送获取信息");
            signer.SendMessage(signMsg, true);
        }
        public delegate void GetSingerInfoEventHandlerCallBack(MyJson.JsonNode_Object myjson);
        public event GetSingerInfoEventHandlerCallBack getSiggerInfoEventHandlerCallBack;
        public void getSingerInfoCallBack(MyJson.JsonNode_Object _json)
        {
            _islinking = true;

            json_setting = _json;

            isFirstConfirm["新增地址时是否要密码验证"] = new MyJson.JsonNode_ValueNumber(true);
            isFirstConfirm["删除地址是否要密码验证"] = new MyJson.JsonNode_ValueNumber(true);
            isFirstConfirm["备份地址是否要密码验证"] = new MyJson.JsonNode_ValueNumber(true);
            getSiggerInfoEventHandlerCallBack(_json);
            GetAddressList();
        }

        #endregion

        #region 上报签名机驱动的新设置
        public void SetSettingInfo(bool[] _bools)
        {
            for (int i = 0; i < _bools.Length; i++)
            {
                bools[i] = _bools[i];
            }

            json_setting["连接钱包后是否自动弹出驱动界面"] = new MyJson.JsonNode_ValueNumber(bools[0]);
            json_setting["开机时是否自动检查更新"] = new MyJson.JsonNode_ValueNumber(bools[1]);
            json_setting["新增地址时是否要密码验证"] = new MyJson.JsonNode_ValueNumber(bools[2]);
            json_setting["删除地址是否要密码验证"] = new MyJson.JsonNode_ValueNumber(bools[3]);
            json_setting["备份地址是否要密码验证"] = new MyJson.JsonNode_ValueNumber(bools[4]);
            json_setting["备份钱包时进行是否要加密"] = new MyJson.JsonNode_ValueNumber(bools[5]);


            NeoDun.Message signMsg = new NeoDun.Message();
            signMsg.tag1 = 0x02;
            signMsg.tag2 = 0x1a;
            signMsg.msgid = NeoDun.SignTool.RandomShort();
            signMsg.writeUInt16(2, Convert.ToUInt16(bools[0]));
            signMsg.writeUInt16(4, Convert.ToUInt16(bools[1]));
            signMsg.writeUInt16(6, Convert.ToUInt16(bools[2]));
            signMsg.writeUInt16(8, Convert.ToUInt16(bools[3]));
            signMsg.writeUInt16(10, Convert.ToUInt16(bools[4]));
            signMsg.writeUInt16(12, Convert.ToUInt16(bools[5]));
            signer.SendMessage(signMsg, true);
        }
        public delegate void SetSettingInfoEventHandlerCallBack();
        public SetSettingInfoEventHandlerCallBack setSetingInfoEventHandlerCallBack;
        private void SetSettingInfoCallBack()
        {
            if (setSetingInfoEventHandlerCallBack != null)
                setSetingInfoEventHandlerCallBack();
            //重新获取地址列表
            GetAddressList();
        }
        #endregion

        #region 增加地址
        public delegate void PrivateKey2AddressEventHandlerCallBack(string privateKey,string address,string err);
        public event PrivateKey2AddressEventHandlerCallBack privateKey2AddressEventHandlerCallBack;
        public void PrivateKey2Address_FromText(string _wif)
        {
            try
            {
                byte[] privateKey = NeoDun.SignTool.GetPrivateKeyFromWif(_wif);
                byte[] publicKey = NeoDun.SignTool.GetPublicKeyFromPrivateKey(privateKey);
                string str_address = NeoDun.SignTool.GetAddressFromPublicKey(publicKey);
                privateKey2AddressEventHandlerCallBack(_wif, str_address, "添加成功");
            }
            catch
            {
                privateKey2AddressEventHandlerCallBack("", "","请输入有效的地址");
            }
        }
        public void PrivateKey2Address_FromBackup()
        {
            try
            {
                Microsoft.Win32.OpenFileDialog dialog = new Microsoft.Win32.OpenFileDialog();
                dialog.Filter = "文本文件|*.txt";
                if (dialog.ShowDialog() == true)
                {
                    string _wif = System.IO.File.ReadAllText(dialog.FileName);
                    byte[] privateKey = NeoDun.SignTool.GetPrivateKeyFromWif(_wif);
                    byte[] publicKey = NeoDun.SignTool.GetPublicKeyFromPrivateKey(privateKey);
                    string str_address = NeoDun.SignTool.GetAddressFromPublicKey(publicKey);
                    privateKey2AddressEventHandlerCallBack(_wif, str_address, "");
                }
                else
                {

                }
            }
            catch
            {
                privateKey2AddressEventHandlerCallBack("", "", "请选择有效的备份");
            }

        }


        public async void AddAddress(string _address,string _privateKey)
        {
            if (string.IsNullOrEmpty(_address) || string.IsNullOrEmpty(_privateKey))
                return;

            //地址查重 
            foreach (var add in signer.addressPool.addresses)
            {
                if (add.AddressText == _address)
                {
                    ErrorCallBack("地址重复","通知");
                    return;

                }
            }
            string str_address = _address;

            byte[] privateKey = NeoDun.SignTool.GetPrivateKeyFromWif(_privateKey);

            string str_addressType = "Neo";
            UInt16 addressType = (UInt16)Enum.Parse(typeof(AddressType), str_addressType);
            byte[] bytes_address = NeoDun.SignTool.DecodeBase58(str_address);
            //byte[] privateKey = NeoDun.SignTool.HexString2Bytes(str_privateKey);
            byte[] hash = NeoDun.SignTool.ComputeSHA256(privateKey, 0, privateKey.Length);
            string str_hash = NeoDun.SignTool.Bytes2HexString(hash, 0, hash.Length);

            NeoDun.DataBlock block = signer.dataTable.newOrGet(str_hash, (UInt32)privateKey.Length, NeoDun.DataBlockFrom.FromDriver);
            block.data = privateKey;
            signer.SendDataBlock(block);
            //轮询等待发送完成，需要加入超时机制

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
            //await Task.Delay(50);
            signer.SendMessage(signMsg, true);
        }

        public delegate void AddAddressEventHandlerCallBack(bool _suc);
        public event AddAddressEventHandlerCallBack addAddressEventHandlerCallBack;
        public void AddAddressCallBack(bool _suc)
        {
            if (addAddressEventHandlerCallBack != null)
            {
                addAddressEventHandlerCallBack(_suc);
            }

            //重新获取一次地址
            GetAddressList();
        }
        #endregion

        #region 查询地址
        public void GetAddressList()
        {
            NeoDun.Message msg = new NeoDun.Message();
            msg.tag1 = 0x02;
            msg.tag2 = 0x01;//查
            msg.msgid = NeoDun.SignTool.RandomShort();
            signer.SendMessage(msg, true);
        }
        public delegate void GetAddresslistEventHandlerCallBack();
        public event GetAddresslistEventHandlerCallBack getAddresslistEventHandlerCallBack;
        public void GetAddressCallBack()
        {
            getAddresslistEventHandlerCallBack();
        }
        #endregion

        #region 删除地址
        public void DeleteAddress(string _addressType, string _addressText)
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
        }

        public delegate void DeleteAddressEventHandlerCallBack(bool suc);
        public event DeleteAddressEventHandlerCallBack deleteAddressEventHandlerCallBack;
        public void DeleteAddressCallBack(bool suc)
        {
            if (deleteAddressEventHandlerCallBack!=null)
                deleteAddressEventHandlerCallBack(suc);
            //重新获取地址列表
            GetAddressList();
        }

        #endregion

        #region  签名 sign
        string hashstr;
        byte[] data;
        string src;
        public void Sign(string _data ,string _src)
        {
            src = _src;
            data = NeoDun.SignTool.HexString2Bytes(_data);
            var hash = NeoDun.SignTool.ComputeSHA256(data, 0, data.Length);
            hashstr = NeoDun.SignTool.Bytes2HexString(hash, 0, hash.Length);

            {//发送待签名数据块
                var block = signer.dataTable.newOrGet(hashstr, (UInt32)data.Length, NeoDun.DataBlockFrom.FromDriver);
                block.data = data;
                signer.SendDataBlock(block);//need a finish callback.
            }
            //轮询等待发送完成，需要加入超时机制
            uint remoteid = 0;
            while (true)
            {
                System.Threading.Thread.Sleep(100);
                var __block = signer.dataTable.getBlockBySha256(hashstr);
                if (__block.dataidRemote > 0 && __block.Check())
                {
                    remoteid = __block.dataidRemote;
                    __block.dataidRemote = 0;
                    break;
                }
            }

            Watcher watcher = new Watcher();
            signer.watcherColl.AddWatcher(watcher);//加入监视器

            NeoDun.Message signMsg = new NeoDun.Message();

            {//发送签名报文
                var add = signer.addressPool.getAddress(NeoDun.AddressType.Neo, src);
                var addbytes = add.GetAddbytes();
                signMsg.tag1 = 0x02;
                signMsg.tag2 = 0x0a;//签
                signMsg.msgid = NeoDun.SignTool.RandomShort();
                signMsg.writeUInt16(0, (UInt16)add.type);//neo tag
                Array.Copy(addbytes, 0, signMsg.data, 2, addbytes.Length);//addbytes

                //这个dataid 要上一个block 传送完毕了才知道
                signMsg.writeUInt32(42, remoteid);
                signer.SendMessage(signMsg, true);
            }

        }

        public delegate void SignEventHandlerCallBack(byte[] outdata,string hashstr,bool suc);
        public event SignEventHandlerCallBack signEventHandlerCallBack;
        private void ConfirmSignCallBack(byte[] _outdata,bool suc)
        {
            if (signEventHandlerCallBack != null)
                signEventHandlerCallBack(_outdata, hashstr, suc);

            System.Threading.Thread.Sleep(100);

            getAddresslistEventHandlerCallBack();
        }
        #endregion

        #region 发送加密密钥
        public void SendSecret()
        {
            byte[] secret = new byte[32];
            for (int i = 0; i < 32; i++)
            {
                secret[i] = (byte)i;
            }

            NeoDun.Message signMsg = new NeoDun.Message();
            signMsg.tag1 = 0x02;
            signMsg.tag2 = 0x0c;
            signMsg.msgid = NeoDun.SignTool.RandomShort();
            signMsg.writeUInt16(0, (UInt16)secret.Length);//neo tag
            Array.Copy(secret, 0, signMsg.data, 2, secret.Length);
            signer.SendMessage(signMsg, true);
        }

        public delegate void SendSecretEventHandlerCallBack();
        public event SendSecretEventHandlerCallBack sendSecretEventHandlerCallBack;
        public void SendSecretCallBack(bool _bool)
        {
            if (_bool)
                UpdateApp();
            else
            {
                if (sendSecretEventHandlerCallBack != null)
                    sendSecretEventHandlerCallBack();
            }

        }
        #endregion

        #region 安装更新app
        public async void UpdateApp()
        {
            byte[] secret = new byte[32];
            for (int i = 0; i < 32; i++)
            {
                secret[i] = (byte)i;
            }
            try
            {
                Microsoft.Win32.OpenFileDialog dialog = new Microsoft.Win32.OpenFileDialog();
                dialog.Filter = "安装文件|*.bin";
                if (dialog.ShowDialog() == true)
                {
                    //未加密的数据包
                    byte[] data = System.IO.File.ReadAllBytes(dialog.FileName);
                    //对未加密的数据包hash以方便下位机验证
                    byte[] hash = NeoDun.SignTool.ComputeSHA256(data, 0, data.Length);

                    //经过aes加密之后的数据包
                    byte[] data_aes = SignTool.AesEncrypt(secret, secret);
                    string str_hash_aes2 = NeoDun.SignTool.Bytes2HexString(data_aes, 0, data_aes.Length);

                    byte[] hash_aes = NeoDun.SignTool.ComputeSHA256(data_aes, 0, data_aes.Length);
                    string str_hash_aes = NeoDun.SignTool.Bytes2HexString(hash_aes, 0, hash_aes.Length);

                    NeoDun.DataBlock block = signer.dataTable.newOrGet(str_hash_aes, (UInt32)data_aes.Length, NeoDun.DataBlockFrom.FromDriver);
                    block.data = data_aes;
                    signer.SendDataBlock(block);

                    var __block = signer.dataTable.getBlockBySha256(str_hash_aes);
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
                else
                {

                }
            }
            catch(Exception e)
            {
                privateKey2AddressEventHandlerCallBack("", "", e.ToString());
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
