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
using System.Windows.Shapes;
using NBitcoin;

namespace driver_win.dialogs
{
    /// <summary>
    /// ImportWifDialogue.xaml 的交互逻辑
    /// </summary>
    public partial class ImportWifDialogue : Window
    {
        private DriverControl driverControl;
        public ImportWifDialogue(DriverControl _driverControl)
        {
            InitializeComponent();

            driverControl = _driverControl;
        }

        private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }

        private async void btn_Add(object sender, RoutedEventArgs e)
        {
            string result = await driverControl.AddAddressByWif(this.label_Wif.Text.ToString());
            if (result == "suc")
            {
                this.Close();

                DialogueControl.ShowMessageDialogue("添加成功", 2, this.Owner);

                await Task.Delay(2000);

                DialogueControl.ShowAddressListDialogue(driverControl, this.Owner);
            }
            else
            {
                DialogueControl.ShowMessageDialogue(result,2,this);
            }
        }

        private void Btn_CloseDialogue(object sender, RoutedEventArgs e)
        {
            this.DialogResult = true;
        }

        private void Action_GotFocus(object sender, RoutedEventArgs e)
        {
            if(this.Tb_wordlist.Text == "请输入助记词，按空格分隔")
                this.Tb_wordlist.Text = "";
        }

        private void Action_LostFocus(object sender, RoutedEventArgs e)
        {
            if (string.IsNullOrEmpty(this.label_Wif.Text))
            {
                this.label_Wif.Text = "请输入您的WIF";
            }
        }

        private void Btn_ShowBip(object sender, RoutedEventArgs e)
        {
            this.bip.Visibility = Visibility.Visible;
            this.wif.Visibility = Visibility.Hidden;
            this.Btn_Bip.Visibility = Visibility.Hidden;
            this.Btn_Wif.Visibility = Visibility.Visible;
        }

        private void Btn_ShowWif(object sender, RoutedEventArgs e)
        {
            this.bip.Visibility = Visibility.Hidden;
            this.wif.Visibility = Visibility.Visible;
            this.Btn_Bip.Visibility = Visibility.Visible;
            this.Btn_Wif.Visibility = Visibility.Hidden;
        }

        private async void btn_Add2(object sender, RoutedEventArgs e)
        {
            //var wordlist = "bleak version runway tell hour unfold donkey defy digital abuse glide please omit much cement sea sweet tenant demise taste emerge inject cause link";
            var wordlist = this.Tb_wordlist.Text;
            var words = wordlist.Split(' ');
            if (words.Length != 24)
            {
                DialogueControl.ShowMessageDialogue("助记词错误",2,this);
                return;
            }
            var mn = new NBitcoin.Mnemonic(wordlist);
            var ext = mn.DeriveExtKey("bitcoin");
            KeyPath kp = new KeyPath("m/888'/0'/0'/0/0");
            ext = kp.Indexes.Aggregate(ext, (current, index) => current.Derive(index));
            //var bitcoinExtKey = ext.GetWif(Network.Main);
            var wif = ext.PrivateKey.GetWif(Network.Main).ToString();
            //var pk = ThinNeo.Helper.GetPrivateKeyFromWIF(wif2);
            //var pb = ThinNeo.Helper.GetPublicKeyFromPrivateKey(pk);
            //var addr = ThinNeo.Helper.GetAddressFromPublicKey(pb);

            string result = await driverControl.AddAddressByWif(wif);
            if (result == "suc")
            {
                this.Close();

                DialogueControl.ShowMessageDialogue("添加成功", 2, this.Owner);

                await Task.Delay(2000);

                DialogueControl.ShowAddressListDialogue(driverControl, this.Owner);
            }
            else
            {
                DialogueControl.ShowMessageDialogue(result, 2, this);
            }
        }
    }
}
