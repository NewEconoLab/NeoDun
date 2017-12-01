using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace driver_win
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window , NeoDun.IWatcher
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        public static NeoDun.Signer signer = new NeoDun.Signer();
        private void Window_Activated(object sender, EventArgs e)
        {
            Console.WriteLine("windowActivate");
        
        }
        Action<string, UInt32> onFileSend;

        public void OnRecv(NeoDun.Message recv, NeoDun.Message src)
        {
            if (recv.tag1 == 0x01 && recv.tag2 == 0x11)
            {
                this.dataUpdate = true;
                var remoteid = recv.readUInt32(0);
                var hash = recv.readHash256(4);
                if (onFileSend != null)
                    onFileSend(hash, remoteid);
            }
            if (recv.tag1 == 0x01 && recv.tag2 == 0xa3)
            {
                this.dataUpdate = true;
            }
            if (recv.tag1 == 0x02 && recv.tag2 == 0xa1)
            {
                this.addressUpdate = true;
            }
            Action call = () =>
            {

                this.list1.Items.Add("recv msg:" + recv.ToString() + "|" + DateTime.Now.ToString());


            };
            this.Dispatcher.Invoke(call);
        }

        public void OnSend(NeoDun.Message send, bool needBack)
        {
            Action call = () =>
            {

                string tag = "send msg: " + send.tag1.ToString("X02") + send.tag2.ToString("X02");
                if (needBack)
                    tag += " 需要回复";
                tag += "  " + DateTime.Now.ToString();
                this.list1.Items.Add(tag);
            };
            this.Dispatcher.Invoke(call);
        }
        bool dataUpdate = false;
        bool addressUpdate = false;
        void CheckDevice()
        {
            var signerCount = signer.CheckDevice();

            this.info1.Content = signer.CheckDeviceTag() + "=" + signerCount.ToString();
            if (dataUpdate)
            {
                dataUpdate = false;
                this.list2.Items.Clear();
                foreach (var data in signer.dataTable.blocks)
                {
                    this.list2.Items.Add(data);
                }
            }
            if (addressUpdate)
            {
                dataUpdate = false;
                this.listAddress.Items.Clear();
                foreach (var add in signer.addressPool.addresses)
                {
                    this.listAddress.Items.Add(add);
                }
            }
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            signer.Stop();//.StopRead();//不停止读取，有个线程一直卡着的
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {//随机发送数据包
            byte[] data = new byte[NeoDun.SignTool.RandomShort() / 3];

            NeoDun.SignTool.RandomData(data);
            var hash = NeoDun.SignTool.ComputeSHA256(data, 0, data.Length);
            var hashstr = NeoDun.SignTool.Bytes2HexString(hash, 0, hash.Length);
            var block = signer.dataTable.newOrGet(hashstr, (uint)data.Length, NeoDun.DataBlockFrom.FromDriver);
            block.data = data;

            block.Check();
            signer.SendDataBlock(block);
            this.dataUpdate = true;
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {//clear
            this.list1.Items.Clear();
        }

        private void Button_Click_2(object sender, RoutedEventArgs e)
        {
            var data = new byte[] { 0x01, 0x02 };
            DriverS.Send(data);

        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            Console.WriteLine("windowLoad");
            hhgate.CustomServer.BeginServer();

            System.Windows.Threading.DispatcherTimer timer = new System.Windows.Threading.DispatcherTimer(TimeSpan.FromSeconds(1.0), System.Windows.Threading.DispatcherPriority.Normal, (s, e1) =>
              {
                  this.CheckDevice();
              }, this.Dispatcher);
            DriverS.Init();
            CheckDevice();
            signer.Start(this);
            //onRecvMsg, onSendMsg);
        }
        WindowSimHardware hard = new WindowSimHardware();
        private void Button_Click_3(object sender, RoutedEventArgs e)
        {
            DriverS.simdriver.bActive = true;
            hard.Show();
        }

        private void Button_Click_4(object sender, RoutedEventArgs e)
        {
            this.list1.Items.Clear();
        }

        /// <summary>
        ///将wif 作为数据包发送
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Button_Click_5(object sender, RoutedEventArgs e)
        {
            string _wif = this.wif.Text;
            var data = NeoDun.SignTool.GetPrivateKeyFromWif(_wif);
            var hash = NeoDun.SignTool.ComputeSHA256(data, 0, data.Length);
            string strhash = NeoDun.SignTool.Bytes2HexString(hash, 0, hash.Length);

            var block = signer.dataTable.newOrGet(strhash, (UInt32)data.Length, NeoDun.DataBlockFrom.FromDriver);
            block.data = data;
            block.Check();

            Action<string, UInt32> callback = null;
            callback = (_hash, id) =>
                  {
                      if (_hash == strhash)
                      {
                          onFileSend -= callback;
                          //知道发送的id
                          Action call = () =>
                            {
                                this.dataid.Text = id.ToString();
                            };
                          this.Dispatcher.Invoke(call);
                      }
                  };
            onFileSend += callback;


            signer.SendDataBlock(block);
            this.dataUpdate = true;
        }

        /// <summary>
        /// 通知signer将此数据包作为Neo私钥导入
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Button_Click_6(object sender, RoutedEventArgs e)
        {
            string _wif = this.wif.Text;
            var data = NeoDun.SignTool.GetPrivateKeyFromWif(_wif);
            var pkey = NeoDun.SignTool.GetPublicKeyFromPrivateKey(data);
            var address = NeoDun.SignTool.GetAddressFromPublicKey(pkey);
            var addbytes = NeoDun.SignTool.DecodeBase58(address);

            NeoDun.Message msg = new NeoDun.Message();
            msg.tag1 = 0x02;
            msg.tag2 = 0x04;//增
            msg.msgid = NeoDun.SignTool.RandomShort();
            msg.writeUInt16(0, 0x0101);//neo tag
            Array.Copy(addbytes, 0, msg.data, 2, addbytes.Length);//addbytes
            var dataid = UInt32.Parse(this.dataid.Text);
            msg.writeUInt32(42, dataid);

            signer.SendMessage(msg, true);
        }

        /// <summary>
        ///查询signer上的地址
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Button_Click_7(object sender, RoutedEventArgs e)
        {
            NeoDun.Message msg = new NeoDun.Message();
            msg.tag1 = 0x02;
            msg.tag2 = 0x01;//查
            msg.msgid = NeoDun.SignTool.RandomShort();
            signer.SendMessage(msg, true);
        }

        /// <summary>
        /// 用选中地址签名数据包
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Button_Click_8(object sender, RoutedEventArgs e)
        {
            var add = this.textaddress.Tag as NeoDun.Address;
            var block = this.dataid.Tag as NeoDun.DataBlock;
            if (add == null || block == null)
                return;

            var addbytes = add.GetAddbytes();


            NeoDun.Message msg = new NeoDun.Message();
            msg.tag1 = 0x02;
            msg.tag2 = 0x0a;//签
            msg.msgid = NeoDun.SignTool.RandomShort();
            msg.writeUInt16(0, (UInt16)add.type);//neo tag
            Array.Copy(addbytes, 0, msg.data, 2, addbytes.Length);//addbytes
            var dataid = block.dataid;
            msg.writeUInt32(42, dataid);

            signer.SendMessage(msg, true);
        }

        /// <summary>
        /// 发送交易数据包
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Button_Click_9(object sender, RoutedEventArgs e)
        {
            string src2 = "800000010c8fa242aaf7461f802bf0d6e0fc8694e3daed2d66de4dc70fe017012d075350010002e72d286979ee6cb1b7e65dfddfb2e384100b8d148e7758de42e4168b71792c6080969800000000001b02c180df019e6113a985411cae62db80f90db4e72d286979ee6cb1b7e65dfddfb2e384100b8d148e7758de42e4168b71792c607098df2817000000738679b1fd7dbc21fa7ebc1218e74f08e6afbdae";

            byte[] data = NeoDun.SignTool.HexString2Bytes(src2);

            var hash = NeoDun.SignTool.ComputeSHA256(data, 0, data.Length);
            var hashstr = NeoDun.SignTool.Bytes2HexString(hash, 0, hash.Length);
            var block = signer.dataTable.newOrGet(hashstr, (uint)data.Length, NeoDun.DataBlockFrom.FromDriver);
            block.data = data;

            block.Check();
            signer.SendDataBlock(block);
            this.dataUpdate = true;
        }

        private void list2_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var item = list2.SelectedItem as NeoDun.DataBlock;
            if (item == null)
                return;
            dataid.Text = item.dataidRemote.ToString();
            dataid.Tag = item;
        }

        private void listAddress_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var item = listAddress.SelectedItem as NeoDun.Address;
            if (item == null)
                return;
            textaddress.Text = item.AddressText;
            textaddress.Tag = item;
        }
    }


}
