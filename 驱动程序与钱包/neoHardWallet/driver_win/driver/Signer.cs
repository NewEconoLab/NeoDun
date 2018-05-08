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

        public delegate void GetSingerInfoEventHandler(MyJson.JsonNode_Object _myjson);
        public GetSingerInfoEventHandler getSingerInfoEventHandler;

        public delegate void SetPasswordEventHandler();
        public SetPasswordEventHandler setPasswordEventHandler;

        public delegate void ConfirmPasswordEventHandler(bool _suc);
        public ConfirmPasswordEventHandler confirmPasswordEventHandler;

        public delegate void AddAddressEventHandler(bool _suc);
        public AddAddressEventHandler addAddressEventHandler;

        public delegate void GetAddressListEventHandler();
        public GetAddressListEventHandler getAddressListEventHandler;

        public delegate void DelAddressEventHandler(bool _suc);
        public DelAddressEventHandler delAddressEventHandler;

        public delegate void BackUpAddressEventHandler(bool suc ,string _privateKey);
        public BackUpAddressEventHandler backUpAddressEventHandler;

        public delegate void SignEventHandler(byte[] _bytes,bool suc);
        public SignEventHandler signEventHandler;

        public delegate void ShowSignerPasswordPageEventHandler();
        public ShowSignerPasswordPageEventHandler showSignerPasswordPageEventHandler;

        public delegate void SetSettingInfoEventHandler();
        public SetSettingInfoEventHandler setSettingInfoEventHandler;

        public delegate void ErrorEventHandler(string _str,string _header);
        public ErrorEventHandler errorEventHandler;

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
                if (msg.tag1 == 0x01 && msg.tag2 == 0x11)//给数据分配编号
                {
                    var remoteid = msg.readUInt32(0);
                    var hash = msg.readHash256(4);
                    var block = dataTable.getBlockBySha256(hash);
                    block.dataidRemote = remoteid;

                }
                if (msg.tag1 == 0x02 && msg.tag2 == 0xa0)//收到地址,//加进地址池子里
                {
                    var pos = msg.readUInt16(0);
                    var count = msg.readUInt16(2);
                    var type = (NeoDun.AddressType)msg.readUInt16(4);
                    var add = new NeoDun.Address();
                    add.type = type;
                    add.AddressText = SignTool.EncodeBase58(msg.data, 6, 25);
                    addressPool.addresses.Add(add);
                }
                if (msg.tag1 == 0x02 && msg.tag2 == 0xa1)
                {
                    if(getAddressListEventHandler != null)
                        getAddressListEventHandler();
                    //var count = msg.readUInt16(0);
                    //if (count == 0 && addressPool.addresses.Count != 0)
                    //{
                     //   InitAddressPool();
                    //    return;
                    //}
                    //else if (count == 0 && addressPool.addresses.Count == 0)
                    //{
                    //    return;
                    //}
                    //while (true)
                    //{
                    //    if (count <= addressPool.addresses.Count)
                    //    {
                    //        addressUpdate = true;
                    //        break;
                    //    }
                    //}
                }
                if (msg.tag1 == 0x02 && msg.tag2 == 0xb1)
                {
                    if(addAddressEventHandler!=null)
                        addAddressEventHandler(true);
                }
                if (msg.tag1 == 0x02 && msg.tag2 == 0xb2)
                {
                    if (addAddressEventHandler != null)
                        addAddressEventHandler(false);
                }
                if (msg.tag1 == 0x02 && msg.tag2 == 0xa3)
                {
                    if(delAddressEventHandler!=null)
                        delAddressEventHandler(true);
                }
                if (msg.tag1 == 0x02 && msg.tag2 == 0xe3)
                {
                    if (errorEventHandler != null)
                        delAddressEventHandler(false);
                }
                if (msg.tag1 == 0x02 && msg.tag2 == 0xa4)
                {
                    string outdataHash = msg.readHash256(4);
                    byte[] outdata;
                    //轮询直到reciveid的数据被收到
                    while (true)
                   {
                       await System.Threading.Tasks.Task.Delay(5);
                       var __block = dataTable.getBlockBySha256(outdataHash);
                       if (__block.Check())
                       {
                           outdata = __block.data;
                           break;
                       }
                   }
                    byte privatekeylen = outdata[0];
                    byte[] privatekey = new byte[privatekeylen];
                    Array.Copy(outdata, 1, privatekey, 0, privatekeylen);
                    if(backUpAddressEventHandler!=null)
                        backUpAddressEventHandler(true,NeoDun.SignTool.Bytes2HexString(privatekey, 0, privatekey.Length));
                    
                }
                if (msg.tag1 == 0x02 && msg.tag2 == 0xe0)
                {
                    if (backUpAddressEventHandler != null)
                        backUpAddressEventHandler(false,"");
                }
                if (msg.tag1 == 0x02 && msg.tag2 == 0xe3)
                {
                    if (signEventHandler != null)
                        signEventHandler(null,false);
                }
                if (msg.tag1 == 0x02 && msg.tag2 == 0xc3)//设置密码成功
                {
                    if(setPasswordEventHandler!=null)
                        setPasswordEventHandler();
                }
                if (msg.tag1 == 0x02 && msg.tag2 == 0xc5)//密钥验证合规
                {
                    if(confirmPasswordEventHandler!=null)
                        confirmPasswordEventHandler(true);
                }
                if (msg.tag1 == 0x02 && msg.tag2 == 0xc6)//密钥验证不合规
                {
                    if(confirmPasswordEventHandler!=null)
                        confirmPasswordEventHandler(false);
                }
                if (msg.tag1 == 0x02 && msg.tag2 == 0xd1)
                {
                    Console.WriteLine("msg:"+ msg);
                    MyJson.JsonNode_Object myjson = new MyJson.JsonNode_Object();
                    myjson["是否是新设备"] = new MyJson.JsonNode_ValueNumber(msg.readUInt16(0));
                    myjson["连接钱包后是否自动弹出驱动界面"] = new MyJson.JsonNode_ValueNumber(msg.readUInt16(2));
                    myjson["开机时是否自动检查更新"] = new MyJson.JsonNode_ValueNumber(msg.readUInt16(4));
                    myjson["新增地址时是否要密码验证"] = new MyJson.JsonNode_ValueNumber(msg.readUInt16(6));
                    myjson["删除地址是否要密码验证"] = new MyJson.JsonNode_ValueNumber(msg.readUInt16(8));
                    myjson["备份地址是否要密码验证"] = new MyJson.JsonNode_ValueNumber(msg.readUInt16(10));
                    myjson["备份钱包时进行是否要加密"] = new MyJson.JsonNode_ValueNumber(msg.readUInt16(12));
                    if (getSingerInfoEventHandler != null)
                    {
                        Console.WriteLine("收到签名机的消息并准备执行");
                        getSingerInfoEventHandler(myjson);
                    }

                }
                if (msg.tag1 == 0x02&& msg.tag2 == 0xd2)
                {
                    if (errorEventHandler != null)
                        errorEventHandler("设置失败","错误");
                }
                if (msg.tag1 == 0x02 && msg.tag2 == 0xd3)
                {
                    if(setSettingInfoEventHandler != null)
                        setSettingInfoEventHandler();
                }
                if (msg.tag1 == 0x02 && msg.tag2 == 0xd4)
                {
                }
                if (msg.tag1 == 0x02 && msg.tag2 == 0xa8)
                {//签名成功
                 //继续努力
                    byte[] outdata = null;
                    string outdatahash = null;
                    outdatahash = msg.readHash256(4);
                    //轮询直到reciveid的数据被收到
                    while (true)
                    {
                        await Task.Delay(5);
                        var __block = dataTable.getBlockBySha256(outdatahash);
                        if (__block.Check())
                        {
                            outdata = __block.data;
                            break;
                        }
                    }
                    if(signEventHandler!=null)
                        signEventHandler(outdata,true);
                }
                if (msg.tag1 == 0x02 && msg.tag2 == 0xd5)
                {
                    showSignerPasswordPageEventHandler();
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
