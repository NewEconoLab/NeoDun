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

namespace driver_win.dialogs
{
    /// <summary>
    /// ImportWalletDialogue.xaml 的交互逻辑
    /// </summary>
    public partial class ImportWalletDialogue : Window
    {
        private DriverControl driverControl;
        public ImportWalletDialogue(DriverControl _driverControl)
        {
            InitializeComponent();

            driverControl = _driverControl;

        }

        private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }

        private void Btn_CloseDialogue(object sender, RoutedEventArgs e)
        {
            this.DialogResult = true;
        }

        ThinNeo.NEP6.NEP6Wallet nep6wallet;
        private void Btn_OpenFile(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.OpenFileDialog ofd = new Microsoft.Win32.OpenFileDialog();
            ofd.Filter = "*.json|*.json";
            if (ofd.ShowDialog() == true)
            {
                try
                {
                    this.label_path.Text = ofd.FileName;
                    nep6wallet = new ThinNeo.NEP6.NEP6Wallet(this.label_path.Text);
                }
                catch (Exception err)
                {
                    DialogueControl.ShowMessageDialogue(err.ToString(), 2, this);
                }
            }
        }

        private void Btn_Add(object sender, RoutedEventArgs e)
        {
            if (string.IsNullOrEmpty(this.label_pw.Text) || this.label_pw.Text == "请输入密码")
            {
                DialogueControl.ShowMessageDialogue("请输入密码",2,this);
                return;
            }

            foreach (var v in nep6wallet.accounts)
            {
                ThinNeo.NEP6.NEP6Account acc = v.Value as ThinNeo.NEP6.NEP6Account;
                if (acc == null)
                    return;
                if (acc.nep2key == null)
                    return;
                try
                {
                    var prikey = acc.GetPrivate(nep6wallet.scrypt, this.label_pw.Text);
                    var wif = NeoDun.SignTool.GetWifFromPrivateKey(prikey);

                    var result = driverControl.AddAddressByWif(wif);
                }
                catch
                {
                    DialogueControl.ShowMessageDialogue("密码错误", 2, this);
                    break;
                }

                this.DialogResult = true;
                DialogueControl.ShowAddressListDialogue(driverControl, this.Owner);
            }

        }
    }
}
