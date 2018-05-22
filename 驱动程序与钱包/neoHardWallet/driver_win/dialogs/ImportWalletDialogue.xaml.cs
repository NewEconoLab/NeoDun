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
            // 使用一个IntPtr类型值来存储加密字符串的起始点  
            IntPtr p = System.Runtime.InteropServices.Marshal.SecureStringToBSTR(this.pwb_pw.SecurePassword);

            // 使用.NET内部算法把IntPtr指向处的字符集合转换成字符串  
            string password = System.Runtime.InteropServices.Marshal.PtrToStringBSTR(p);

            if (string.IsNullOrEmpty(password))
            {
                DialogueControl.ShowMessageDialogue("请输入密码",2,this);
                return;
            }

            this.DialogResult = true;
            if (nep6wallet == null)
            {
                DialogueControl.ShowMessageDialogue("错误的钱包格式",2,this);
                return;
            }
            DialogueControl.ShowImportAddressListDialogue(nep6wallet, password, driverControl, this.Owner);
        }

        private void Action_PwGotFouces(object sender, RoutedEventArgs e)
        {
            this.label_pw.Visibility = Visibility.Collapsed;
        }

        private void Action_PwLostFouces(object sender, RoutedEventArgs e)
        {
            // 使用一个IntPtr类型值来存储加密字符串的起始点  
            IntPtr p = System.Runtime.InteropServices.Marshal.SecureStringToBSTR(this.pwb_pw.SecurePassword);

            // 使用.NET内部算法把IntPtr指向处的字符集合转换成字符串  
            string password = System.Runtime.InteropServices.Marshal.PtrToStringBSTR(p);

            if (string.IsNullOrEmpty(password))
            {
                this.label_pw.Visibility = Visibility.Visible;
            }
        }
    }
}
