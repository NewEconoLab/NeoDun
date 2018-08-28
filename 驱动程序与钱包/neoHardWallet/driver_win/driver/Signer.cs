using driver_win.helper;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace NeoDun
{

    public interface IWatcher
    {
        void OnRecv(Message recv, Message src);
        void OnSend(Message send, bool needBack);
    }
    public class WatcherColl : IWatcher
    {
        public void AddWatcher(IWatcher watcher)
        {
            watchers[watcher] = 1;
        }
        public void RemoveWatcher(IWatcher watcher)
        {
            int v;
            watchers.TryRemove(watcher, out v);
        }
        System.Collections.Concurrent.ConcurrentDictionary<IWatcher, int> watchers = new ConcurrentDictionary<IWatcher, int>();
        public void OnRecv(Message recv, Message src)
        {
            foreach (var k in watchers)
            {
                k.Key.OnRecv(recv, src);
            }
        }

        public void OnSend(Message send, bool needBack)
        {
            foreach (var k in watchers)
            {
                k.Key.OnSend(send, needBack);
            }
        }
    }
    public class Signer
    {
        private static Signer ins;
        public static Signer Ins
        {
            get {
                if (ins == null)
                    ins = new Signer();
                return ins;
            }
        }

        public delegate void AddAddressEventHandler(bool _suc);
        public AddAddressEventHandler addAddressEventHandler;

        public delegate void SetNameEventHandler(string _result);
        public SetNameEventHandler setNameEventHandler;

        public delegate void GetAddressListEventHandler();
        public GetAddressListEventHandler getAddressListEventHandler;

        public delegate void DelAddressEventHandler(bool _suc);
        public DelAddressEventHandler delAddressEventHandler;

        public delegate void GetPackageInfoEventHandler(byte[] _bytes);
        public GetPackageInfoEventHandler getPackageInfoEventHandler;

        public delegate void SignEventHandler(byte[] _bytes,UInt16 _suc);
        public SignEventHandler signEventHandler;

        public delegate Task<bool> UpdateApp(byte[] data, UInt16 type, UInt16 content, UInt16 version);
        public UpdateApp updateApp;

        public delegate void ApplyUpdateEventHandler(bool _suc);
        public ApplyUpdateEventHandler applyUpdateEventHandler;

        public delegate void UpdateAppEventHandler(bool _suc);
        public UpdateAppEventHandler updateAppEventHandler;

        public delegate void UninstallAppEventHandler(bool _suc);
        public UninstallAppEventHandler uninstallAppEventHandler;

        public delegate void ErrorEventHandler(string _str,string _header);
        public ErrorEventHandler errorEventHandler;


        public delegate void EventHandler(Object enumControl, params object[] args);
        public EventHandler eventHandler;

        public delegate void DeleInstallFramework();
        public DeleInstallFramework deleInstallFramework;

        public WatcherColl watcherColl = new WatcherColl();


        ConcurrentQueue<Message> msgForSend = new ConcurrentQueue<Message>();
        ConcurrentDictionary<UInt16, Message> needBackMessage = new ConcurrentDictionary<ushort, Message>();
        public DataTable dataTable = new DataTable();
        public AddressPool addressPool = new AddressPool();
        public bool _new = false;

        byte[] rx_data = new byte[65];
        public void Start(IWatcher watcher = null)// deleUserHandleRecv _userHandleRecv, deleUserHandleSend _userHandleSend)
        {
            if (watcher != null)
                watcherColl.AddWatcher(watcher);

            DriverS.StartRead((rx_data) =>
            {
                Message m = new Message();
                m.FromData(rx_data);
                OnMsg(m);
            });
            System.Threading.Thread t = new System.Threading.Thread(() =>
              {
                  bExit = false;
                  StartSendQueue();
              });
            t.Start();
        }
        public void Start()// deleUserHandleRecv _userHandleRecv, deleUserHandleSend _userHandleSend)
        {
            DriverS.StartRead((rx_data) =>
            {
                Message m = new Message();
                m.FromData(rx_data);
                OnMsg(m);
            });
            System.Threading.Thread t = new System.Threading.Thread(() =>
            {
                bExit = false;
                StartSendQueue();
            });
            t.Start();
        }
        //消息发送线程
        void StartSendQueue()
        {
            var data = new byte[64];
            while (bExit == false)
            {
                if (DriverS.getActive() != null)
                {
                    if (msgForSend.Count > 0)
                    {
                        Message msg = null;
                        if (msgForSend.TryDequeue(out msg))
                        {
                            //Console.WriteLine("123"+msg);
                            msg.ToData(data);
                            DriverS.Send(data);

                        }
                    }
                }
                System.Threading.Thread.Sleep(5);
            }
        }
        void OnMsg(Message msg)
        {
            Message srcmsg = null; 
            if (msg.tag1 == 0x00 || msg.tag2 >= 0xa0)//如果是一条回复消息，找原始消息
            {
                srcmsg = this.needBackMessage[msg.msgid];
            }


            System.Threading.ThreadPool.QueueUserWorkItem(async (_state) =>
            {

                #region 0x01
                if (msg.tag1 == 0x01 && msg.tag2 == 0x01)
                {
                    //recv a file
                    var len = msg.readUInt32(0);
                    var hash = msg.readHash256(4);
                    var block = dataTable.newOrGet(hash, len, NeoDun.DataBlockFrom.FromSigner);

                    var piececount = len / 50;
                    if (len % 50 > 0)
                        piececount++;

                    await System.Threading.Tasks.Task.Delay(50);//add bu hkh 1216

                    System.Threading.ThreadPool.QueueUserWorkItem((__state) =>
                    {
                        NeoDun.Message _msg = new NeoDun.Message();
                        _msg.tag1 = 0x01;
                        _msg.tag2 = 0x10;
                        _msg.msgid = NeoDun.SignTool.RandomShort();
                        _msg.writeUInt16(0, 0);
                        _msg.writeUInt16(2, (UInt16)(piececount - 1));
                        _msg.writeHash256(4, hash);
                        SendMessage(_msg, true);
                    });
                }
                if (msg.tag1 == 0x01 && msg.tag2 == 0x10)//请求数据
                {
                    int blockbegin = BitConverter.ToUInt16(msg.data, 0);
                    int blockend = BitConverter.ToUInt16(msg.data, 2);
                    string hash = SignTool.Bytes2HexString(msg.data, 4, 32);
                    var block = dataTable.getBlockBySha256(hash);
                    if (block == null)
                    {
                        //发送错误回应，下一步再说
                    }
                    else
                    {
                        for (var i = blockbegin; i < blockend + 1; i++)
                        {
                            Message msg1 = new Message();
                            block.FillPieceMessage(msg1, msg.msgid, (UInt16)i);
                            SendMessage(msg1, false);
                        }
                        //Send0103
                        Message msg2 = new Message();
                        msg2.tag1 = 0x01;
                        msg2.tag2 = 0xa3;
                        msg2.msgid = msg.msgid;//这是一条回复消息
                        SendMessage(msg2, false);
                    }
                }
                if (msg.tag1 == 0x01 && msg.tag2 == 0x11)//给数据分配编号
                {
                    var remoteid = msg.readUInt32(0);
                    var hash = msg.readHash256(4);
                    var block = dataTable.getBlockBySha256(hash);
                    block.dataidRemote = (uint)remoteid;

                }
                if (msg.tag1 == 0x01 && msg.tag2 == 0x12)
                {
                    var remoteid = msg.readUInt32(0);
                    var hash = msg.readHash256(4);
                    var block = dataTable.getBlockBySha256(hash);
                    block.dataidRemote = (uint)remoteid;
                }
                if (msg.tag1 == 0x01 && msg.tag2 == 0xa2)//收到一个分片
                {
                    var hash = srcmsg.readHash256(4);
                    var data = dataTable.getBlockBySha256(hash);
                    data.FromPieceMsg(msg);
                }
                if (msg.tag1 == 0x01 && msg.tag2 == 0xa3)//接收完毕
                {
                    var hash = srcmsg.readHash256(4);
                    var data = dataTable.getBlockBySha256(hash);
                    bool bcheck = data.Check();
                    if (bcheck)
                    {//数据接收完整
                        System.Threading.ThreadPool.QueueUserWorkItem((__state) =>
                        {
                            NeoDun.Message _msg = new NeoDun.Message();
                            _msg.tag1 = 0x01;
                            _msg.tag2 = 0x11;
                            _msg.msgid = NeoDun.SignTool.RandomShort();
                            _msg.writeUInt32(0, data.dataid);
                            _msg.writeHash256(4, hash);
                            SendMessage(_msg, false);
                        });
                    }
                    else
                    {//数据接收完毕，但是hash256 不匹配
                        System.Threading.ThreadPool.QueueUserWorkItem((__state) =>
                        {
                            NeoDun.Message _msg = new NeoDun.Message();
                            _msg.tag1 = 0x01;
                            _msg.tag2 = 0x12;
                            _msg.msgid = NeoDun.SignTool.RandomShort();
                            _msg.writeUInt32(0, data.dataid);
                            _msg.writeHash256(4, hash);
                            SendMessage(_msg, false);
                        });
                    }
                }
                #endregion

                //设置地址名称失败
                if (msg.tag1 == 0x02 && msg.tag2 == 0xe2)
                {
                    EnumErrorCode errorCode =(EnumErrorCode) msg.readUInt16(0);
                    eventHandler( EnumControl.SetName,EnumMsgCode.SetNameFailed,errorCode);
                }
                //删除地址失败
                if (msg.tag1 == 0x02 && msg.tag2 == 0xe3)
                {
                    var a = msg.readUInt16(0);
                    var aa = a.ToString("x4");
                    EnumErrorCode errorCode = (EnumErrorCode)msg.readUInt16(0);
                    eventHandler(EnumControl.DelAddress, EnumMsgCode.DeleteNameFailed, errorCode);
                }
                //增加地址失败
                if (msg.tag1 == 0x02 && msg.tag2 == 0xe4)
                {
                    EnumErrorCode errorCode = (EnumErrorCode)msg.readUInt16(0);
                    eventHandler(EnumControl.AddAddress, EnumMsgCode.AddAddressFailed,errorCode);
                }
                //签名失败
                if (msg.tag1 == 0x02 && msg.tag2 == 0xe5)
                {
                    EnumErrorCode errorCode = (EnumErrorCode)msg.readUInt16(0);
                    eventHandler(EnumControl.SignData, EnumMsgCode.SignFailed, errorCode);
                }
                //安装失败
                if (msg.tag1 == 0x03 && msg.tag2 == 0xe1)
                {
                    EnumErrorCode errorCode = (EnumErrorCode)msg.readUInt16(0);
                    eventHandler(EnumControl.Common, EnumMsgCode.InstallFailed, errorCode);
                }
                //拒绝更新固件
                if (msg.tag1 == 0x03 && msg.tag2 == 0xe2)
                {
                    eventHandler(EnumControl.ApplyInstallFramework, EnumMsgCode.RefuseUpdate,EnumErrorCode.NoError);
                }
                //卸载失败
                if (msg.tag1 == 0x03 && msg.tag2 == 0xe3)
                {
                    EnumErrorCode errorCode = (EnumErrorCode)msg.readUInt16(0);
                    eventHandler(EnumControl.UninstallPlugin, EnumMsgCode.UninstallFailed, errorCode);
                }
                //收到地址,//加进地址池子里
                if (msg.tag1 == 0x02 && msg.tag2 == 0xa0)
                {
                    var epoch = (DateTime.Now.ToUniversalTime().Ticks - 621355968000000000) / 10000000;
                    var pos = msg.readUInt16(0);
                    var count = msg.readUInt16(2);
                    var type = (NeoDun.AddressType)msg.readUInt16(4);
                    var add = new NeoDun.Address();
                    add.type = type;
                    add.AddressText = SignTool.EncodeBase58(msg.data, 6, 25);
                    add.name = System.Text.Encoding.UTF8.GetString(msg.data, 32, 6).Replace("\0", ""); 
                    addressPool.addresses.Add(add);
                }
                //地址接受完毕
                if (msg.tag1 == 0x02 && msg.tag2 == 0xa1)
                {
                    eventHandler(EnumControl.GetAddress,EnumMsgCode.GetAddressSuc,EnumErrorCode.NoError);
                }
                //设置地址名称成功
                if (msg.tag1 == 0x02 && msg.tag2 == 0xa2)
                {
                    eventHandler(EnumControl.SetName,EnumMsgCode.SetNameSuc, EnumErrorCode.NoError);
                }
                //删除地址成功
                if (msg.tag1 == 0x02 && msg.tag2 == 0xa3)
                {
                    eventHandler(EnumControl.DelAddress, EnumMsgCode.DeleteNameSuc, EnumErrorCode.NoError);
                }
                //增加地址成功
                if (msg.tag1 == 0x02 && msg.tag2 == 0xa4)
                {
                    eventHandler(EnumControl.AddAddress, EnumMsgCode.AddAddressSuc, EnumErrorCode.NoError);
                }
                //签名成功
                if (msg.tag1 == 0x02 && msg.tag2 == 0xa5)
                {
                    byte[] outdata = null;
                    string outdatahash = null;
                    outdatahash = msg.readHash256(4);
                    //轮询直到reciveid的数据被收到
                    while (true)
                    {
                        await Task.Delay(5);
                        var __block = dataTable.getBlockBySha256(outdatahash);
                        if (__block !=null && __block.Check())
                        {
                            outdata = __block.data;
                            break;
                        }
                    }
                    eventHandler(EnumControl.SignData, EnumMsgCode.SignSuc, EnumErrorCode.NoError, outdata);
                }
                //安装成功
                if (msg.tag1 == 0x03 && msg.tag2 == 0xa1)
                {
                    eventHandler(EnumControl.Common, EnumMsgCode.InstallSuc, EnumErrorCode.NoError);
                }
                //同意更新固件
                if (msg.tag1 == 0x03 && msg.tag2 == 0xa2)
                {
                    eventHandler(EnumControl.ApplyInstallFramework, EnumMsgCode.AgreeUpdate, EnumErrorCode.NoError);
                }
                //卸载成功
                if (msg.tag1 == 0x03 && msg.tag2 == 0xa3)
                {
                    eventHandler(EnumControl.UninstallPlugin, EnumMsgCode.UninstallSuc, EnumErrorCode.NoError);
                }
                //查询固件插件版本回复
                if (msg.tag1 == 0x03 && msg.tag2 == 0xa4)
                {
                    byte[] outdata = null;
                    outdata = msg.data;
                    eventHandler(EnumControl.GetPackage, EnumMsgCode.GetMessageSuc, EnumErrorCode.NoError, outdata);
                }
                //下位机请求更新固件
                if (msg.tag1 == 0x03 && msg.tag2 == 0x11)
                {
                    deleInstallFramework();
                }
                //安全通道回复
                if (msg.tag1 == 0x04 && msg.tag2 == 0xa1)
                {
                    byte[] outdata = null;
                    string outdatahash = null;
                    outdatahash = msg.readHash256(4);
                    //轮询直到reciveid的数据被收到
                    while (true)
                    {
                        await Task.Delay(5);
                        var __block = dataTable.getBlockBySha256(outdatahash);
                        if (__block != null && __block.Check())
                        {
                            outdata = __block.data;
                            break;
                        }
                    }
                    eventHandler(EnumControl.SecurityChannel, EnumMsgCode.BuildSecurityChannelSuc,EnumErrorCode.NoError, outdata);
                }
            });
            watcherColl.OnRecv(msg, srcmsg);
            //if (userHandleRecv != null)
            //{
            //    userHandleRecv(msg, srcmsg);
            //}
        }
        bool bExit = false;
        public void Stop()
        {
            bExit = true;
            DriverS.StopRead();
            DriverS.Stop();
        }

        /// <summary>
        /// 返回激活的设备的总数
        /// </summary>
        /// <returns></returns>
        public string CheckDevice()//检查设备
        {
            var driver = DriverS.getActive();
            if (driver == null)
            {
                return string.Empty;
            }
            int count = driver.GetCount();
            string driverTag = count > 0 ? driver.Tag() : string.Empty;
            return driverTag;
        }
        /// <summary>
        /// 返回激活设备的tag
        /// </summary>
        /// <returns></returns>
        public string CheckDeviceTag()//检查设备
        {
            var driver = DriverS.getActive();
            if (driver == null)
            {
                return "";
            }
            return driver.Tag();
        }

        //重置地址池
        public void InitAddressPool()
        {
            addressPool = new AddressPool();
        }

        public void SendMessage(Message msg, bool needback)
        {
            if (needback)
            {
                needBackMessage[msg.msgid] = msg;//这是一条需要回复的消息，记下来
                watcherColl.OnSend(msg, true);
                //if (this.userHandleSend != null)
                //{
                //    this.userHandleSend(msg, true);
                //}
            }
            else
            {
                watcherColl.OnSend(msg, false);

                //if (this.userHandleSend != null)
                //{
                //    this.userHandleSend(msg, false);
                //}
            }
            //crc会在sendmsg 自动发送
            msgForSend.Enqueue(msg);
        }
        public void SendDataBlock(DataBlock data)
        {
            Message msg = new Message();
            msg.tag1 = 0x01; //0字节 
            msg.tag2 = 0x01; //1字节
            msg.msgid = SignTool.RandomShort();// 2 3 字节

            //4~7 字节
            var byteLength = BitConverter.GetBytes((UInt32)data.data.Length);
            for (var i = 0; i < 4; i++)
            {
                msg.data[i] = byteLength[i];
            }
            var sha = SignTool.HexString2Bytes(data.sha256);
            //8 到39 字节
            for (var i = 0; i < 32; i++)
            {
                msg.data[i + 4] = sha[i];
            }

            SendMessage(msg, true);
        }
        //发送包
        //public void SendPackage(byte[] data)
        //{
        //    byte[] sha = SignTool.ComputeSHA256(data, 0, data.Length);
        //    string key = SignTool.Hex2String(sha, 0, sha.Length);
        //    datapool[key] = data;
        //    Message msg = new Message();
        //    msg.tag1 = 0x01; //0字节 
        //    msg.tag2 = 0x01; //1字节
        //    msg.msgid = SignTool.RandomShort();// 2 3 字节

        //    //4~7 字节
        //    var byteLength = BitConverter.GetBytes((UInt32)data.Length);
        //    for (var i = 0; i < 4; i++)
        //    {
        //        msg.data[i] = byteLength[i];
        //    }
        //    //8 到39 字节
        //    for (var i = 0; i < 32; i++)
        //    {
        //        msg.data[i + 4] = sha[i];
        //    }

        //    SendMessage(msg, true);
        //}
    }
}
