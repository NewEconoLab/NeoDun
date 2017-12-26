using NeoDun;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace driver_win
{
    /// <summary>
    /// WindowSimHardware.xaml 的交互逻辑
    /// </summary>
    public partial class WindowSimHardware : Window
    {
        public WindowSimHardware()
        {
            InitializeComponent();
        }
        private void Window_Activated(object sender, EventArgs e)
        {
            passwordRefresh = true;
        }
        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            this.Hide();
            e.Cancel = true;
            DriverS.simdriver.bActive = false;

        }
        void SendMsg(NeoDun.Message msg, bool needBack = false)
        {
            DriverS.simdriver.__SignerSend(msg, needBack);
            Action call = () =>
            {

                this.list1.Items.Add("send msg:" + msg.ToString() + "|" + DateTime.Now.ToString());
            };
            this.Dispatcher.Invoke(call);
        }
        void SendBlock(NeoDun.DataBlock data)
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

            SendMsg(msg, true);
        }
        DataTable dataTable = new DataTable();
        AddressPool addresspool = new AddressPool();
        PersonalInfo personalinfo = new PersonalInfo();

        void onRecvMsg(NeoDun.Message recv, NeoDun.Message src)//标记 hkh
        {
            if (recv.tag1 == 0x01 && recv.tag2 == 0x01)
            {
                //recv a file
                var len = recv.readUInt32(0);
                var hash = recv.readHash256(4);
                var block = dataTable.newOrGet(hash, len, NeoDun.DataBlockFrom.FromDriver);
                dataUpdate = true;

                var piececount = len / 50;
                if (len % 50 > 0)
                    piececount++;

                System.Threading.ThreadPool.QueueUserWorkItem((_state) =>
                {
                    NeoDun.Message msg = new NeoDun.Message();
                    msg.tag1 = 0x01;
                    msg.tag2 = 0x10;
                    msg.msgid = NeoDun.SignTool.RandomShort();
                    msg.writeUInt16(0, 0);
                    msg.writeUInt16(2, (UInt16)(piececount - 1));
                    msg.writeHash256(4, hash);
                    SendMsg(msg, true);
                });
            }
            if (recv.tag1 == 0x01 && recv.tag2 == 0x10)//请求数据
            {
                int blockbegin = BitConverter.ToUInt16(recv.data, 0);
                int blockend = BitConverter.ToUInt16(recv.data, 2);
                string hash = SignTool.Bytes2HexString(recv.data, 4, 32);
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
                        block.FillPieceMessage(msg1, recv.msgid, (UInt16)i);
                        SendMsg(msg1, false);
                    }
                    //Send0103
                    Message msg2 = new Message();
                    msg2.tag1 = 0x01;
                    msg2.tag2 = 0xa3;
                    msg2.msgid = recv.msgid;//这是一条回复消息
                    SendMsg(msg2, false);
                }
            }
            if (recv.tag1 == 0x01 && recv.tag2 == 0xa2)//收到一个分片
            {
                var hash = src.readHash256(4);
                var data = dataTable.getBlockBySha256(hash);
                data.FromPieceMsg(recv);
            }
            if (recv.tag1 == 0x01 && recv.tag2 == 0xa3)//接收完毕
            {
                var hash = src.readHash256(4);
                var data = dataTable.getBlockBySha256(hash);
                bool bcheck = data.Check();
                if (bcheck)
                {//数据接收完整
                    System.Threading.ThreadPool.QueueUserWorkItem((_state) =>
                    {
                        NeoDun.Message msg = new NeoDun.Message();
                        msg.tag1 = 0x01;
                        msg.tag2 = 0x11;
                        msg.msgid = NeoDun.SignTool.RandomShort();
                        msg.writeUInt32(0, data.dataid);
                        msg.writeHash256(4, hash);
                        SendMsg(msg, false);
                    });
                }
                else
                {//数据接收完毕，但是hash256 不匹配
                    System.Threading.ThreadPool.QueueUserWorkItem((_state) =>
                    {
                        NeoDun.Message msg = new NeoDun.Message();
                        msg.tag1 = 0x01;
                        msg.tag2 = 0x12;
                        msg.msgid = NeoDun.SignTool.RandomShort();
                        msg.writeUInt32(0, data.dataid);
                        msg.writeHash256(4, hash);
                        SendMsg(msg, false);
                    });
                }
                this.dataUpdate = true;
            }
            if (recv.tag1 == 0x01 && recv.tag2 == 0x11)//给数据分配编号
            {
                var remoteid = recv.readUInt32(0);
                var hash = recv.readHash256(4);
                var block = dataTable.getBlockBySha256(hash);
                block.dataidRemote = remoteid;
                this.dataUpdate = true;
            }
            if (recv.tag1 == 0x02 && recv.tag2 == 0x04)//增加一个地址
            {
                var type = recv.readUInt16(0);
                if (type == 0x0101)
                {//neo
                    UInt32 data0id = recv.readUInt32(42);
                    //neo addressbyte 长度25
                    var address = NeoDun.SignTool.EncodeBase58(recv.data, 2, 25);
                    var _data = dataTable.getBlockByDataId(data0id);
                    //验证一下，看私钥和公钥是否匹配
                    var pubkey = NeoDun.SignTool.GetPublicKeyFromPrivateKey(_data.data);
                    var addresscalc = NeoDun.SignTool.GetAddressFromPublicKey(pubkey);


                    var add = new NeoDun.Address();
                    add.AddressText = address;
                    add.privatekey = _data.data;
                    add.type = AddressType.Neo;

                    bool suc = addresspool.AddAddress(add);

                    if (address == addresscalc && suc)
                    {

                        //地址匹配
                        addressUpdate = true;

                        System.Threading.ThreadPool.QueueUserWorkItem((_state) =>
                        {
                            NeoDun.Message msg = new NeoDun.Message();
                            msg.tag1 = 0x02;
                            msg.tag2 = 0xb1;
                            msg.msgid = recv.msgid;
                            Array.Copy(recv.data, msg.data, 40 + 2);
                            SendMsg(msg, true);
                        });

                    }
                    else
                    {   //地址和私钥不匹配
                        System.Threading.ThreadPool.QueueUserWorkItem((_state) =>
                        {
                            NeoDun.Message msg = new NeoDun.Message();
                            msg.tag1 = 0x02;
                            msg.tag2 = 0xb2;
                            msg.msgid = recv.msgid;
                            Array.Copy(recv.data, msg.data, 40 + 2);
                            SendMsg(msg, true);
                        });
                    }
                }
                else
                {   //不是neo直接失败
                    System.Threading.ThreadPool.QueueUserWorkItem((_state) =>
                    {
                        NeoDun.Message msg = new NeoDun.Message();
                        msg.tag1 = 0x02;
                        msg.tag2 = 0xb2;
                        msg.msgid = recv.msgid;
                        Array.Copy(recv.data, msg.data, 40 + 2);
                        SendMsg(msg, true);
                    });
                }
            }
            if (recv.tag1 == 0x02 && recv.tag2 == 0x01)//查询地址
            {
                //                S2D 02a0 （返回一条地址）
                //4 5字节 是uint16 当前地址index，
                //6 7 字节是 uint16 总共有多少个地址

                //            8 9是 地址类型 01 01 表示小蚁地址

                //            10~61 是 hash格式地址，长度不够的地方填0
                //        S2D 02a1(返回地址结束)
                //4 5 字节是 uint16 总共有多少个地址
                System.Threading.ThreadPool.QueueUserWorkItem((_state) =>
                {
                    int imax = addresspool.addresses.Count;
                    int iseek = 0;
                    foreach (var add in addresspool.addresses)
                    {
                        NeoDun.Message msg1 = new NeoDun.Message();
                        msg1.tag1 = 0x02;
                        msg1.tag2 = 0xa0;
                        msg1.msgid = recv.msgid;
                        msg1.writeUInt16(0, (UInt16)iseek);
                        msg1.writeUInt16(2, (UInt16)imax);
                        msg1.writeUInt16(4, (UInt16)add.type);
                        var bytes = add.GetAddbytes();

                        Array.Copy(bytes, 0, msg1.data, 6, bytes.Length);

                        iseek++;
                        SendMsg(msg1, true);
                    }
                    NeoDun.Message msg2 = new NeoDun.Message();
                    msg2.tag1 = 0x02;
                    msg2.tag2 = 0xa1;
                    msg2.msgid = recv.msgid;
                    msg2.writeUInt16(0, (UInt16)imax);
                    SendMsg(msg2, true);
                });
            }
            if (recv.tag1 == 0x02 && recv.tag2 == 0x0a)//签名
            {
                var type = (NeoDun.AddressType)recv.readUInt16(0);
                UInt32 data0id = recv.readUInt32(42);
                NeoDun.Address addr = null;
                if (type == NeoDun.AddressType.Neo)
                {//neo
                    //neo addressbyte 长度25
                    var address = NeoDun.SignTool.EncodeBase58(recv.data, 2, 25);
                    addr = this.addresspool.getAddress(NeoDun.AddressType.Neo, address);
                }
                var data = this.dataTable.getBlockByDataId(data0id);
                SignTool.ContractTransaction ct = null;
                try
                {
                    //ct = SignTool.ParseTransforData(data.data) as SignTool.ContractTransaction;
                    //string stex = "有一笔交易需要你签名，是否同意。\n";
                    //if (ct != null)
                    //{
                    //    foreach (var o in ct.outputs)
                    //    {
                    //        if (o.toAddress == addr.AddressText)
                    //            continue;
                    //        stex += "对" + o.toAddress + "支付:" + o.assetId + "=" + o.assetCount;
                    //    }
                    //}
                    //if (MessageBox.Show(stex, "", MessageBoxButton.OKCancel) == MessageBoxResult.OK)
                    if(true)
                    {
                        var datas = SignTool.SignData(addr.privatekey, data.data);
                        var datapubkey = SignTool.GetPublicKeyFromPrivateKey(addr.privatekey);
                        byte pkeylen = (byte)datapubkey.Length;
                        var outdata = new byte[] { pkeylen }.Concat(datapubkey).Concat(datas).ToArray();
                        //outdata 签名后的数据，先写一个字节的公钥长度，再写公钥，再写签名后数据

                        //var outdata = SignTool.SignData(addr.privatekey, data.data);
                        var hash = SignTool.ComputeSHA256(outdata, 0, outdata.Length);

                        {//准备好数据块，飞回去
                            var block = dataTable.newOrGet(SignTool.Bytes2HexString(hash, 0, hash.Length), (UInt32)outdata.Length, DataBlockFrom.FromSigner);
                            block.data = outdata;
                            this.dataUpdate = true;
                            //将outdata 发回上位机
                            this.SendBlock(block);
                        }
                        //再发条通知消息 告诉上位机hash
                        NeoDun.Message msg = new Message();
                        msg.tag1 = 0x02;
                        msg.tag2 = 0xa8;
                        msg.msgid = recv.msgid;
                        msg.writeUInt32(0, (UInt32)outdata.Length);
                        msg.writeHash256(4, hash);
                        this.SendMsg(msg);
                    }
                    else
                    {
                       
                        NeoDun.Message msg = new Message();
                        msg.tag1 = 0x02; 
                        msg.tag2 = 0xe0;
                        msg.msgid = recv.msgid;
                        this.SendMsg(msg);
                    }
                    //ok .可以签名了
                }
                catch
                {//发生异常，无法签名
                    MessageBox.Show("签名数据异常，不是认识的交易");
                    NeoDun.Message msg = new Message();
                    msg.tag1 = 0x02;
                    msg.tag2 = 0xe0;
                    msg.msgid = recv.msgid;
                    this.SendMsg(msg);
                }
            }
            if (recv.tag1 == 0x02 && recv.tag2 == 0x03)//删除一个地址
            {
                var type = recv.readUInt16(0);
                if (type == 0x0101)
                {//neo
                    //neo addressbyte 长度25
                    var address = NeoDun.SignTool.EncodeBase58(recv.data, 2, 25);

                    var add = new NeoDun.Address();
                    add.AddressText = address;
                    add.type = AddressType.Neo;
                    bool _suc = addresspool.DelAddress(add);
                    //地址匹配
                    addressUpdate = true;

                    if (_suc)
                    {
                       

                        System.Threading.ThreadPool.QueueUserWorkItem((_state) =>
                        {
                            NeoDun.Message msg = new NeoDun.Message();
                            msg.tag1 = 0x02;
                            msg.tag2 = 0xc1;
                            msg.msgid = recv.msgid;
                            Array.Copy(recv.data, msg.data, 40 + 2);
                            SendMsg(msg, true);
                        });

                    }
                    else
                    {   
                        System.Threading.ThreadPool.QueueUserWorkItem((_state) =>
                        {
                            NeoDun.Message msg = new NeoDun.Message();
                            msg.tag1 = 0x02;
                            msg.tag2 = 0xc2;
                            msg.msgid = recv.msgid;
                            Array.Copy(recv.data, msg.data, 40 + 2);
                            SendMsg(msg, true);
                        });
                    }
                }
                else
                {   //不是neo直接失败
                    System.Threading.ThreadPool.QueueUserWorkItem((_state) =>
                    {
                        NeoDun.Message msg = new NeoDun.Message();
                        msg.tag1 = 0x02;
                        msg.tag2 = 0xb2;
                        msg.msgid = recv.msgid;
                        Array.Copy(recv.data, msg.data, 40 + 2);
                        SendMsg(msg, true);
                    });
                }
            }
            if (recv.tag1 == 0x02 && recv.tag2 == 0x06)//获取地址安全信息
            {
                AddressType addressType = (NeoDun.AddressType)recv.readUInt16(4);
                string addressText = NeoDun.SignTool.EncodeBase58(recv.data, 6, 25);
                Address address = addresspool.getAddress(addressType,addressText);
                //发消息给驱动
                NeoDun.Message msg = new Message();
                if (address != null)
                {
                    byte pkeylen = (byte)address.privatekey.Length;
                    var outdata = new byte[] { pkeylen }.Concat(address.privatekey).ToArray();
                    byte[] hash = SignTool.ComputeSHA256(outdata, 0, outdata.Length);

                    {//准备好数据块，飞回去
                        var block = dataTable.newOrGet(SignTool.Bytes2HexString(hash, 0, hash.Length), (UInt32)outdata.Length, DataBlockFrom.FromSigner);
                        block.data = outdata;
                        this.dataUpdate = true;
                        //将outdata 发回上位机
                        this.SendBlock(block);
                    }

                    msg.tag1 = 0x02;
                    msg.tag2 = 0xa4;
                    msg.msgid = recv.msgid;
                    msg.writeUInt32(0, (UInt32)address.privatekey.Length);
                    msg.writeHash256(4,hash);

                }
                else
                {
                    msg.tag1 = 0x02;
                    msg.tag2 = 0xe0;
                    msg.msgid = recv.msgid;
                }
                this.SendMsg(msg);
            }
            if (recv.tag1 == 0x02 && recv.tag2 == 0x0b)//设置钱包密码
            {
                byte passwordlen = (byte)recv.readUInt16(0);
                byte[] bytes_password = new byte[passwordlen];
                Array.Copy(recv.data, 4, bytes_password, 0, passwordlen);
                string str_password = personalinfo.GetString(bytes_password);
                string realpassword = "";
                this.Dispatcher.Invoke((Action)delegate () {
                    //将传过来的伪密码转换成实际密码
                    for (int i = 0; i < str_password.Length; i++)
                    {
                        var items = this.password.Items;
                        var item = items.GetItemAt(int.Parse(str_password[i].ToString()) - 1) as ListViewItem;
                        realpassword += item.Content;
                    }
                });
                personalinfo.SetPassword(realpassword);

                NeoDun.Message msg = new NeoDun.Message();
                msg.tag1 = 0x02;
                msg.tag2 = 0xc3;
                msg.msgid = recv.msgid;
                this.SendMsg(msg);
            }
            if (recv.tag1 == 0x02 && recv.tag2 == 0x0c)//验证钱包密码
            {
                byte passwordlen = (byte)recv.readUInt16(0);
                byte[] bytes_password = new byte[passwordlen];
                Console.WriteLine();
                Array.Copy(recv.data, 4, bytes_password, 0, passwordlen);
                string str_password = personalinfo.GetString(bytes_password);
                string realpassword = "";
                this.Dispatcher.Invoke((Action)delegate() {
                    //将传过来的伪密码转换成实际密码
                    for (int i = 0; i < str_password.Length; i++)
                    {
                        var items = this.password.Items;
                        var item = items.GetItemAt(int.Parse(str_password[i].ToString())-1) as ListViewItem;
                        realpassword += item.Content;
                    }
                });
                NeoDun.Message msg = new NeoDun.Message();
                if (personalinfo.ConfirmPassword(realpassword))
                {
                    msg.tag1 = 0x02;
                    msg.tag2 = 0xc5;
                }
                else
                {
                    msg.tag1 = 0x02;
                    msg.tag2 = 0xc6;
                }
                msg.msgid = recv.msgid;
                this.SendMsg(msg);
            }
            if (recv.tag1 == 0x02 && recv.tag2 == 0x0d)//检查钱包是不是新钱包(废弃)
            {
                if (string.IsNullOrEmpty(personalinfo.password))
                {//新钱包
                    NeoDun.Message msg = new NeoDun.Message();
                    msg.tag1 = 0x02;
                    msg.tag2 = 0xb5;
                    msg.msgid = recv.msgid;
                    this.SendMsg(msg);
                }
                else
                {
                    NeoDun.Message msg = new NeoDun.Message();
                    msg.tag1 = 0x02;
                    msg.tag2 = 0xb6;
                    msg.msgid = recv.msgid;
                    this.SendMsg(msg);
                }

            }
            if (recv.tag1 == 0x02 && recv.tag2 == 0x1b)
            {
                NeoDun.Message msg = new NeoDun.Message();
                msg.tag1 = 0x02;
                msg.tag2 = 0xd1;
                msg.msgid = recv.msgid;
                string[] _setting = personalinfo.setting;
                msg.writeUInt16(0, UInt16.Parse(_setting[0]));
                msg.writeUInt16(2, UInt16.Parse(_setting[1]));
                msg.writeUInt16(4, UInt16.Parse(_setting[2]));
                msg.writeUInt16(6, UInt16.Parse(_setting[3]));
                msg.writeUInt16(8, UInt16.Parse(_setting[4]));
                msg.writeUInt16(10, UInt16.Parse(_setting[5]));
                msg.writeUInt16(12, UInt16.Parse(_setting[6]));
                Console.WriteLine("收到驱动获取消息的通知并开始返还");
                this.SendMsg(msg);
            }
            if (recv.tag1 == 0x02 && recv.tag2 == 0x1a)
            {//赶 写死 -- 后续优化
                string[] setting = new string[7];
                setting[0] = personalinfo.setting[0];
                setting[1] = recv.readUInt16(2).ToString();
                setting[2] = recv.readUInt16(4).ToString();
                setting[3] = recv.readUInt16(6).ToString();
                setting[4] = recv.readUInt16(8).ToString();
                setting[5] = recv.readUInt16(10).ToString();
                setting[6] = recv.readUInt16(12).ToString();
                personalinfo.SetSetting(setting);
                NeoDun.Message msg = new NeoDun.Message();
                msg.tag1 = 0x02;
                msg.tag2 = 0xd3;
                msg.msgid = recv.msgid;
                this.SendMsg(msg);
            }
            if (recv.tag1 == 0x02 && recv.tag2 == 0x1c)
            {
                NeoDun.Message msg = new NeoDun.Message();
                msg.tag1 = 0x02;
                msg.tag2 = 0xe1;
                msg.msgid = recv.msgid;
                this.SendMsg(msg);
            }
            Action call = () =>
                {
                    this.list1.Items.Add("recv msg:" + recv.ToString() + "|" + DateTime.Now.ToString());
                };
            this.Dispatcher.Invoke(call);
        }

        private void Button_Click_4(object sender, RoutedEventArgs e)
        {
            this.list1.Items.Clear();
        }
        System.Windows.Threading.DispatcherTimer timer;
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            //加载地址
            this.addresspool.Load();
            this.addressUpdate = true;

            //加载个人信息
            this.personalinfo.Load();

            timer = new System.Windows.Threading.DispatcherTimer(TimeSpan.FromSeconds(1.0), System.Windows.Threading.DispatcherPriority.Normal, (s, e1) =>
            {
                this.UpdateUI();
            }, this.Dispatcher);

            DriverS.simdriver.__SignerRecv(onRecvMsg);
            screen.Items.Add("Welcome to Neodun");
            screen.Items.Add("2");
            screen.Items.Add("3");
            screen.Items.Add("4");

            //初始化密码页面
            passwordRefresh = true;
        }
        bool dataUpdate = false;
        bool addressUpdate = false;
        bool passwordRefresh = false;
        void UpdateUI()
        {
            if (dataUpdate)
            {
                dataUpdate = false;
                this.list2.Items.Clear();
                foreach (var data in dataTable.blocks)
                {
                    this.list2.Items.Add(data.ToString());
                }
            }
            if (addressUpdate)
            {
                dataUpdate = false;
                this.listAddress.Items.Clear();
                foreach (var add in addresspool.addresses)
                {
                    this.listAddress.Items.Add(add.ToString());
                }
            }
            if (passwordRefresh)
            {
                passwordRefresh = false;
                this.password.Items.Clear();
                //随机十个数字
                List<int> arr_int = new List<int>();
                for (int i = 1; i < 10; i++)
                {
                    arr_int.Add(i);
                }
                //将数字打乱
                for (int i = 0; i < arr_int.Count; i++)
                {
                    Random rd = new Random();
                    int num = rd.Next(9);
                    if (num != i)
                    {
                        arr_int[i] = arr_int[i] + arr_int[num];
                        arr_int[num] = arr_int[i] - arr_int[num];
                        arr_int[i] = arr_int[i] - arr_int[num];
                    }
                }
                //将打乱的数字呈现在页面上
                for (int i = 0; i < arr_int.Count; i++)
                {
                    ListViewItem item = new ListViewItem();
                    item.Name = "btnPw" + i;
                    item.Content = arr_int[i];
                    item.HorizontalContentAlignment = HorizontalAlignment.Center;
                    item.VerticalContentAlignment = VerticalAlignment.Center;
                    item.BorderThickness = new Thickness(1,1,1,1);
                    item.BorderBrush = new SolidColorBrush(Colors.Black);
                    this.password.Items.Add(item);
                }
                this.passwordbox.Header = "密码：" +personalinfo.password;
            }
        }
    }
}
