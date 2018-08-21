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
using driver_win.control;
using driver_win.helper;
using NBitcoin;

namespace driver_win.dialogs
{
    /// <summary>
    /// ImportWifDialogue.xaml 的交互逻辑
    /// </summary>
    public partial class ImportWifDialogue : Window
    {
        public ImportWifDialogue()
        {
            InitializeComponent();
            InitPage();
        }

        public void InitPage()
        {
            this.lb_page_add.Content = Mgr_Language.Ins.Code2Word(this.lb_page_add.Content.ToString());
            this.lb_page_add2.Content = Mgr_Language.Ins.Code2Word(this.lb_page_add2.Content.ToString());
            this.lb_page_ps.Content = Mgr_Language.Ins.Code2Word(this.lb_page_ps.Content.ToString());
            this.lb_page_seed.Content = Mgr_Language.Ins.Code2Word(this.lb_page_seed.Content.ToString());
            this.lb_page_wif.Content = Mgr_Language.Ins.Code2Word(this.lb_page_wif.Content.ToString());
            this.lb_page_wif2.Content = Mgr_Language.Ins.Code2Word(this.lb_page_wif2.Content.ToString());

            this.Tb_wordlist.Text = Mgr_Language.Ins.Code2Word(this.Tb_wordlist.Text);
            this.label_Wif.Text = Mgr_Language.Ins.Code2Word(this.label_Wif.Text);
        }

        private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }

        private async void btn_Add(object sender, RoutedEventArgs e)
        {
            if (!ECDH.Ins.CheckM() || !ECDH.Ins.CheckResult)
            {
                DialogueControl.ShowVerityECDH(this);
            }
            if (ECDH.Ins.CheckResult)
            {
                Result result = await ManagerControl.Ins.ToDo(EnumControl.AddAddress, this.label_Wif.Text.ToString());

                if (result.errorCode == EnumError.AddAddressSuc)
                {
                    this.Close();

                    DialogueControl.ShowMessageDialogue("添加成功", 2, this.Owner);

                    await Task.Delay(500);

                    DialogueControl.ShowAddressListDialogue(this.Owner);
                }
                else
                {
                    DialogueControl.ShowMessageDialogue(result.errorCode.ToString(), 2, this);
                }
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
            /*
            var wordlist = "cancel okay bulk depth blame hospital veteran angle method estate immense expire ball cycle sauce sheriff funny strike play among parrot hen include scale";

            var a = ThinNeo.Helper.HexString2Bytes("fbcd88deffda644507dd650347918df0d2452350d3a320c0ea089c3655bd22bb090856ff72f02b0c48cd49be652df63e5e370bfbbf03798b82f9c4c6855f783d");
            var s = System.Text.UTF8Encoding.UTF8.GetBytes("mnemonic");

            s = s.Concat(new byte[] { 0,0,0,1}).ToArray();

            a = UTF8Encoding.UTF8.GetBytes(wordlist);

            var ss = new byte[] { 0x04,0x88,0xB2,0x1E };

            //var wordlist = this.Tb_wordlist.Text;
            var words = wordlist.Split(' ');
            if (words.Length != 24)
            {
                DialogueControl.ShowMessageDialogue("助记词错误",2,this);
                return;
            }
            var mn = new NBitcoin.Mnemonic(wordlist);
            var ext = mn.DeriveExtKey();
            var sc = ext.ScriptPubKey.ToHex();
            var root = ext.GetWif(Network.Main);
            KeyPath kp = new KeyPath("m/44'/0'/0'/0/0");
            ext = kp.Indexes.Aggregate(ext, (current, index) => current.Derive(index));
            var prk = ext.PrivateKey.GetWif(Network.Main).ToString();
            var pk = ThinNeo.Helper.GetPrivateKeyFromWIF(prk);
            var pb = ThinNeo.Helper.GetPublicKeyFromPrivateKey(pk);
            //var addr = ThinNeo.Helper.GetAddressFromPublicKey(pb);

            string result = await driverControl.AddAddressByWif(prk);
            if (result == "suc")
            {
                this.Close();

                DialogueControl.ShowMessageDialogue("添加成功", 2, this.Owner);

                await Task.Delay(2000);

                DialogueControl.ShowAddressListDialogue(this.Owner);
            }
            else
            {
                DialogueControl.ShowMessageDialogue(result, 2, this);
            }
            */
        }
    }
}
