using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using NeoDun;
using Microsoft.Owin;
using hhgate;
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

        private System.Windows.Threading.DispatcherTimer timer;

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
            signer.setPasswordEventHandler += SetPasswordCallBack;
            signer.confirmPasswordEventHandler += ConfirmPasswordCallBack;
            signer.getAddressListEventHandler += GetAddressCallBack;
            signer.delAddressEventHandler += DeleteAddressCallBack;
            signer.backUpAddressEventHandler += BackUpAddressCallBack;
        }

        private void UInit()
        {
            signer.getSingerInfoEventHandler -= getSingerInfoCallBack;
            signer.addAddressEventHandler -= AddAddressCallBack;
            signer.setPasswordEventHandler -= SetPasswordCallBack;
            signer.confirmPasswordEventHandler -= ConfirmPasswordCallBack;
            signer.getAddressListEventHandler -= GetAddressCallBack;
            signer.delAddressEventHandler -= DeleteAddressCallBack;
            signer.backUpAddressEventHandler -= BackUpAddressCallBack;
        }

        #region 连接签名机
        public void LinkSinger(int count = 5,int time=5)
        {
            int _count = count;
            int _time = time;
            timer = new System.Windows.Threading.DispatcherTimer();
            timer.Interval = TimeSpan.FromSeconds(1.0);
            timer.Tick += new EventHandler(async (_s, _e) => {
                _time--;
                if (_count <= 0)
                {
                    //显示未连接 
                    timer.Stop();
                    timer = null;
                    LinkCallBack("未连接",System.Windows.Visibility.Visible);
                }
                else if (_time < 0)
                {
                    //显示重连失败
                    _count--;
                    _time = time;
                    LinkCallBack("连接失败", System.Windows.Visibility.Collapsed);
                    await Task.Delay(2000);
                }
                else
                {
                    //显示重连中
                    if (signer.CheckDevice() > 0)//有签名机连接了
                    {
                        timer.Stop();
                        timer = null;
                        LinkCallBack("连接成功",System.Windows.Visibility.Collapsed);

                        //连接成功后去请求签名机的状态
                        GetSingerInfo();
                    }
                    else
                    {
                        LinkCallBack("连接中……（" + _time + "s）", System.Windows.Visibility.Collapsed);
                        await Task.Delay(1000);
                    }
                }
            });
            timer.Start();
        }
        public delegate void LinkSingerEventHandlerCallBack(string str ,System.Windows.Visibility visibility);
        public event LinkSingerEventHandlerCallBack linkSingerEventHandlerCallBack;
        public void LinkCallBack(string str, System.Windows.Visibility visibility)
        {
            linkSingerEventHandlerCallBack(str, visibility);
        }
        #endregion

        #region 获取签名机中保存的驱动的默认设置
        public void GetSingerInfo()
        {
            NeoDun.Message signMsg = new NeoDun.Message();
            signMsg.tag1 = 0x02;
            signMsg.tag2 = 0x1b;
            signMsg.msgid = NeoDun.SignTool.RandomShort();
            signer.SendMessage(signMsg, true);
        }
        public delegate void GetSingerInfoEventHandlerCallBack(string _str,MyJson.JsonNode_Object myjson);
        public event GetSingerInfoEventHandlerCallBack getSiggerInfoEventHandlerCallBack;
        public void getSingerInfoCallBack(MyJson.JsonNode_Object _json)
        {
            json_setting = _json;
            if (_json["是否是新设备"] as MyJson.JsonNode_ValueNumber)
            {
                getSiggerInfoEventHandlerCallBack("检测到插入的钱包是新设备，请初始化钱包的密码", _json);
            }
            else
            {
                getSiggerInfoEventHandlerCallBack("请输入你的密码", _json);
                //json_setting = MyJson.Parse(_str) as MyJson.JsonNode_Object;
            }
        }

        //根据设置判断是不是要验证密码
        public bool IsNeedConfirmPassword(NeoDun.Enum_DriverFun _funName)
        {
            if (json_setting[_funName.ToString()] as MyJson.JsonNode_ValueNumber)
            {//如果配置需要验证密码   就先回掉弹出密码验证 再把当前委托再赋值给验证密码callback委托
                return true;
            }
            else//如果配置不需要验证密码 就直接执行委托
            {
                return false;
            }
          
        }

        public delegate void IsNeedConfirmPasswordEventHandlerCallBack(string _str);
        public event IsNeedConfirmPasswordEventHandlerCallBack isNeedConfirmPasswordEventHandlerCallBack;
        public void isNeedConfirmPasswordCallBack(string _str="请输入你的密码")
        {
            isNeedConfirmPasswordEventHandlerCallBack(_str);
        }
        #endregion

        #region 上报签名机驱动的新设置
        public void SetSettingInfo(bool[] bools)
        {
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
            signMsg.writeUInt16(2,Convert.ToUInt16(bools[0]));
            signMsg.writeUInt16(4,Convert.ToUInt16(bools[1]));
            signMsg.writeUInt16(6,Convert.ToUInt16(bools[2]));
            signMsg.writeUInt16(8,Convert.ToUInt16(bools[3]));
            signMsg.writeUInt16(10,Convert.ToUInt16(bools[4]));
            signMsg.writeUInt16(12,Convert.ToUInt16(bools[5]));
            signer.SendMessage(signMsg, true);
        }
        #endregion

        #region 密码的验证和设置
        public void SetOrConfirmPassword(string str_password)
        {
            if (json_setting["是否是新设备"] as MyJson.JsonNode_ValueNumber == true)
            {
                confirmPasswordEventHandlerCallBack = null;
                confirmPasswordEventHandlerCallBack += GetSingerInfo;

                //byte[] bytes_password = NeoDun.SignTool.DecodeBase58(str_password);

                NeoDun.Message signMsg = new NeoDun.Message();
                signMsg.tag1 = 0x02;
                signMsg.tag2 = 0x0b;//设置密码
                signMsg.msgid = NeoDun.SignTool.RandomShort();
                signMsg.writeUInt16(0, (UInt16)str_password.Length);
                signMsg.writeHash256(2, str_password);
                signer.SendMessage(signMsg, true);
            }
            else
            { //向钱包验证密码
                if (confirmPasswordEventHandlerCallBack == null)
                {//设置验证密码默认做的是获得地址
                    confirmPasswordEventHandlerCallBack += GetAddressList;
                }

                byte[] bytes_password = NeoDun.SignTool.DecodeBase58(str_password);
                NeoDun.Message signMsg = new NeoDun.Message();
                signMsg.tag1 = 0x02;
                signMsg.tag2 = 0x0c;//验证密码
                signMsg.msgid = NeoDun.SignTool.RandomShort();
                signMsg.writeUInt16(0, (UInt16)bytes_password.Length);
                Array.Copy(bytes_password, 0, signMsg.data, 2, bytes_password.Length);
                signer.SendMessage(signMsg, true);
            }
        }

        //设置（修改）密码成功
        public delegate void SetPasswordEventHandlerCallBack(string str);
        public event SetPasswordEventHandlerCallBack setPasswordEventHandlerCallBack;
        public void SetPasswordCallBack()
        {
            json_setting["是否是新设备"] = new MyJson.JsonNode_ValueNumber(false);
            confirmPasswordEventHandlerCallBack = null;
            setPasswordEventHandlerCallBack("请输入你的密码");
        }
        //验证密码成功
        public delegate void ConfirmPasswordEventHandlerCallBack();
        public event ConfirmPasswordEventHandlerCallBack confirmPasswordEventHandlerCallBack;//密码验证成功之后要做的事情
        public event ConfirmPasswordEventHandlerCallBack confirmPasswordfaildEventHandlerCallBack;//密码验证失败之后要做的事情
        public void ConfirmPasswordCallBack(bool _suc)
        {
            if (_suc)
            {
                confirmPasswordEventHandlerCallBack();
                confirmPasswordEventHandlerCallBack = null;
            }
            else
            {
                confirmPasswordfaildEventHandlerCallBack();
            }
        }
        #endregion

        #region 增加地址
        public delegate void PrivateKey2AddressEventHandlerCallBack(string privateKey,string address,string err);
        public event PrivateKey2AddressEventHandlerCallBack privateKey2AddressEventHandlerCallBack;
        public void PrivateKey2Address_FromText(string _str)
        {
            try
            {
                byte[] privateKey = NeoDun.SignTool.HexString2Bytes(_str);
                byte[] publicKey = NeoDun.SignTool.GetPublicKeyFromPrivateKey(privateKey);
                string str_address = NeoDun.SignTool.GetAddressFromPublicKey(publicKey);
                privateKey2AddressEventHandlerCallBack(_str, str_address, "添加成功");
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
                    string str_privateKey = System.IO.File.ReadAllText(dialog.FileName);
                    byte[] privateKey = NeoDun.SignTool.HexString2Bytes(str_privateKey);
                    byte[] publicKey = NeoDun.SignTool.GetPublicKeyFromPrivateKey(privateKey);
                    string str_address = NeoDun.SignTool.GetAddressFromPublicKey(publicKey);
                    privateKey2AddressEventHandlerCallBack(str_privateKey, str_address, "");
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


        private string str_address;
        private string str_privateKey;
        public void AddAddress(string _address,string _privateKey)
        {
            if (string.IsNullOrEmpty(_address) || string.IsNullOrEmpty(_privateKey))
                return;
            str_address = _address;
            str_privateKey = _privateKey;
            if (IsNeedConfirmPassword(NeoDun.Enum_DriverFun.新增地址时是否要密码验证))
            {
                //需要密码验证
                isNeedConfirmPasswordCallBack();
                confirmPasswordEventHandlerCallBack = null;
                confirmPasswordEventHandlerCallBack += ConfirmAddAddress;
            }
            else
            {
                ConfirmAddAddress();
            }
        }
        private async void ConfirmAddAddress()
        {
            string str_addressType = "Neo";
            UInt16 addressType = (UInt16)Enum.Parse(typeof(AddressType), str_addressType);
            byte[] bytes_address = NeoDun.SignTool.DecodeBase58(str_address);
            byte[] privateKey = NeoDun.SignTool.HexString2Bytes(str_privateKey);
            byte[] hash = NeoDun.SignTool.ComputeSHA256(privateKey, 0, privateKey.Length);
            string str_hash = NeoDun.SignTool.Bytes2HexString(hash, 0, hash.Length);

            NeoDun.DataBlock block = signer.dataTable.newOrGet(str_hash, (UInt32)privateKey.Length, NeoDun.DataBlockFrom.FromDriver);
            block.data = privateKey;
            signer.SendDataBlock(block);
            //轮询等待发送完成，需要加入超时机制
            uint remoteid = 0;
            while (true)
            {
                await Task.Delay(5);
                var __block = signer.dataTable.getBlockBySha256(str_hash);
                if (__block.dataidRemote > 0 && __block.Check())
                {
                    remoteid = __block.dataidRemote;
                    break;
                }
            }
            NeoDun.Message signMsg = new NeoDun.Message();
            signMsg.tag1 = 0x02;
            signMsg.tag2 = 0x04;//增
            signMsg.msgid = NeoDun.SignTool.RandomShort();
            signMsg.writeUInt16(0, addressType);
            Array.Copy(bytes_address, 0, signMsg.data, 2, bytes_address.Length);
            //这个dataid 要上一个block 传送完毕了才知道
            signMsg.writeUInt32(42, remoteid);
            signer.SendMessage(signMsg, true);
        }
        public delegate void AddAddressEventHandlerCallBack();
        public event AddAddressEventHandlerCallBack addAddressEventHandlerCallBack;
        public void AddAddressCallBack()
        {
            if (addAddressEventHandlerCallBack != null)
            {
                confirmPasswordEventHandlerCallBack = null;
                addAddressEventHandlerCallBack();
            }

            //重新获取一次地址
            GetAddressList();
        }
        #endregion

        #region 查询地址
        public void GetAddressList()
        {
            signer.InitAddressPool();
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
            confirmPasswordEventHandlerCallBack = null;
            getAddresslistEventHandlerCallBack();
        }
        #endregion

        #region 删除地址
        private string str_addressType;
        private string str_addressText;
        public void DeleteAddress(string _addressType, string _addressText)
        {
            str_addressType = _addressType;
            str_addressText = _addressText;
            if (IsNeedConfirmPassword(NeoDun.Enum_DriverFun.删除地址是否要密码验证))
            {
                //需要密码验证
                isNeedConfirmPasswordCallBack();
                confirmPasswordEventHandlerCallBack = null;
                confirmPasswordEventHandlerCallBack += ConfirmDeletaAddress;
            }
            else
            {
                ConfirmDeletaAddress();
            }
        }
        private void ConfirmDeletaAddress()
        {
            UInt16 addressType = (UInt16)Enum.Parse(typeof(AddressType), str_addressType);
            byte[] bytes_addressText = NeoDun.SignTool.DecodeBase58(str_addressText);
            NeoDun.Message signMsg = new NeoDun.Message();
            signMsg.tag1 = 0x02;
            signMsg.tag2 = 0x03;//删除
            signMsg.msgid = NeoDun.SignTool.RandomShort();
            signMsg.writeUInt16(0, addressType);
            Array.Copy(bytes_addressText, 0, signMsg.data, 2, bytes_addressText.Length);
            signer.SendMessage(signMsg, true);
        }
        public delegate void DeleteAddressEventHandlerCallBack();
        public event DeleteAddressEventHandlerCallBack deleteAddressEventHandlerCallBack;
        public void DeleteAddressCallBack()
        {
            confirmPasswordEventHandlerCallBack = null;
            if (deleteAddressEventHandlerCallBack!=null)
                deleteAddressEventHandlerCallBack();
            //重新获取地址列表
            GetAddressList();
        }

        #endregion

        #region 备份地址
        public void BackUpAddress(string _addressType , string _addressText)
        {
            str_addressType = _addressType;
            str_addressText = _addressText;
            if (IsNeedConfirmPassword(NeoDun.Enum_DriverFun.备份地址是否要密码验证))
            {
                //需要密码验证
                isNeedConfirmPasswordCallBack();
                confirmPasswordEventHandlerCallBack = null;
                confirmPasswordEventHandlerCallBack += ConfirmBackupAddress;
            }
            else
            {
                ConfirmBackupAddress();
            }
        }
        private void ConfirmBackupAddress()
        {
            ushort addressType = (ushort)Enum.Parse(typeof(AddressType), str_addressType);

            NeoDun.Message signMsg = new NeoDun.Message();
            signMsg.tag1 = 0x02;
            signMsg.tag2 = 0x06;
            signMsg.writeUInt16(4, addressType);
            byte[] hash_address = SignTool.DecodeBase58(str_addressText);
            Array.Copy(hash_address, 0, signMsg.data, 6, hash_address.Length);
            signer.SendMessage(signMsg, true);
        }
        public delegate void BackUpAddressEventHandlerCallBack();
        public event BackUpAddressEventHandlerCallBack backUpAddressEventHandlerCallBack;
        public void BackUpAddressCallBack(string _privateKey)
        {
            confirmPasswordEventHandlerCallBack = null;
            System.IO.File.WriteAllText("backup.sim.save.txt", _privateKey);
            backUpAddressEventHandlerCallBack();
            GetAddressList();
        }
        #endregion

        #region 重置密码
        string str_password_save = "";

        public void ResetPassword()
        {
            //需要密码验证
            isNeedConfirmPasswordCallBack();
            confirmPasswordEventHandlerCallBack = null;
            confirmPasswordEventHandlerCallBack += ConfirmResetPassword;
        }
        public delegate void ConfirmResetPasswordEventHandlerCallBack(string _str);
        public event ConfirmResetPasswordEventHandlerCallBack confirmResetPasswordEventHandlerCallBack;
        private void ConfirmResetPassword()
        {
            confirmResetPasswordEventHandlerCallBack("请输入新密码");
            json_setting["是否是新设备"] = new MyJson.JsonNode_ValueNumber(true);
        }


        #endregion




        #region  签名 sign   -------后面改
        static IOwinContext context;
        static FormData formdata;
        string hashstr;
        public void Sign(IOwinContext _context, FormData _formdata)
        {
            context = _context;
            formdata = _formdata;
            //需要密码验证
            isNeedConfirmPasswordCallBack();
            confirmPasswordEventHandlerCallBack = null;
            confirmPasswordEventHandlerCallBack += ConfirmSign;
        }
        private async void ConfirmSign()
        {
            if (formdata.mapParams.ContainsKey("data") == false
                || formdata.mapParams.ContainsKey("source") == false)
            {
                await context.Response.WriteAsync("need param，data & source");
                return;
            }
            var src = formdata.mapParams["source"];
            var data = NeoDun.SignTool.HexString2Bytes(formdata.mapParams["data"]);
            var hash = NeoDun.SignTool.ComputeSHA256(data, 0, data.Length);
            hashstr = NeoDun.SignTool.Bytes2HexString(hash, 0, hash.Length);

            var signer = driver_win.MainWindow.signer;
            {//发送待签名数据块
                var block = signer.dataTable.newOrGet(hashstr, (UInt32)data.Length, NeoDun.DataBlockFrom.FromDriver);
                block.data = data;
                signer.SendDataBlock(block);//need a finish callback.
            }
            //轮询等待发送完成，需要加入超时机制
            uint remoteid = 0;
            while (true)
            {
                await Task.Delay(5);
                var __block = signer.dataTable.getBlockBySha256(hashstr);
                if (__block.dataidRemote > 0 && __block.Check())
                {
                    remoteid = __block.dataidRemote;
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

            signer.watcherColl.RemoveWatcher(watcher);
        }

        private async void ConfirmSignCallBack(byte[] outdata)
        {
            //读出来，拼为http响应，发回去
            MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
            json["tag"] = new MyJson.JsonNode_ValueNumber(0);
            json["srchash"] = new MyJson.JsonNode_ValueString(hashstr);
            var pubkeylen = outdata[0];
            var pubkey = new byte[pubkeylen];
            Array.Copy(outdata, 1, pubkey, 0, pubkeylen);
            var signdata = outdata.Skip(pubkeylen + 1).ToArray();

            json["signdata"] = new MyJson.JsonNode_ValueString(SignTool.Bytes2HexString(signdata, 0, signdata.Length));
            json["pubkey"] = new MyJson.JsonNode_ValueString(SignTool.Bytes2HexString(pubkey, 0, pubkey.Length));

            await context.Response.WriteAsync(json.ToString());
        }


        #endregion

    }
}
