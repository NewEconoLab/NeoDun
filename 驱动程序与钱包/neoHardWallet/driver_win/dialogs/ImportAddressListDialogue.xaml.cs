using driver_win.control;
using driver_win.helper;
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
    /// ImportAddressListDialogue.xaml 的交互逻辑
    /// </summary>
    public partial class ImportAddressListDialogue : Window
    {
        public ImportAddressListDialogue()
        {
            InitializeComponent();
            InitPage();
        }

        public void InitPage()
        {
            this.lb_page_address.Content = Mgr_Language.Ins.Code2Word(this.lb_page_address.Content.ToString());
            this.lb_page_addressmgr.Content = Mgr_Language.Ins.Code2Word(this.lb_page_addressmgr.Content.ToString());
            this.lb_page_index.Content = Mgr_Language.Ins.Code2Word(this.lb_page_index.Content.ToString());
        }

        private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }

        private void Btn_CloseDialogue(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        public void ShowAddressInfo(ThinNeo.NEP6.NEP6Wallet nep6wallet,string password)
        {
            var wifs = new List<string>();
            var demoItem = this.listboxDemo.Items[0] as ListBoxItem;
            for (var i= 0;i< nep6wallet.accounts.Count; i++)
            {
                var v = nep6wallet.accounts.ToList()[i];
                ThinNeo.NEP6.NEP6Account acc = v.Value as ThinNeo.NEP6.NEP6Account;
                if (acc == null)
                    return ;
                if (acc.nep2key == null)
                    return ;

                string xaml = System.Windows.Markup.XamlWriter.Save(demoItem);
                ListBoxItem item = System.Windows.Markup.XamlReader.Parse(xaml) as ListBoxItem;
                var label_address = item.FindName("address") as Label;
                var label_index = item.FindName("index") as Label;
                var img_state = item.FindName("img_state") as Image;
                var label_state = item.FindName("label_state") as Label;

                try
                {
                    var prikey = acc.GetPrivate(nep6wallet.scrypt, password);
                    var wif = NeoDun.SignTool.GetWifFromPrivateKey(prikey);
                    wifs.Add(wif);
                    label_address.Content = ThinNeo.Helper.GetAddressFromScriptHash(acc.ScriptHash);
                    label_index.Content = (i + 1).ToString("00");
                    label_state.Content = "等待中";
                    label_state.Foreground = new SolidColorBrush(Color.FromRgb(191, 191, 191));
                    img_state.Visibility = Visibility.Hidden;
                    this.listbox.Items.Add(item);
                }
                catch
                {
                    label_address.Content = "密码和地址不匹配，数据错误";
                    label_index.Content = (i + 1).ToString("00");
                    label_state.Visibility = Visibility.Hidden;
                    img_state.Visibility = Visibility.Hidden;
                    this.listbox.Items.Add(item);
                }
            }
            AddAddress(wifs);
        }

        private async void AddAddress(List<string> wifs)
        {
            //先获取一下地址，可以提前避免重复添加
            await ManagerControl.Ins.ToDo(EnumControl.GetAddress);

            for (var i = 0; i < wifs.Count; i++)
            {
                var item = this.listbox.Items[i] as ListBoxItem;
                var img_state = item.FindName("img_state") as Image;
                var label_state = item.FindName("label_state") as Label;

                label_state.Visibility = Visibility.Visible;
                label_state.Content = "正在导入...";
                label_state.Foreground = new SolidColorBrush(Color.FromRgb(191, 191, 191));

                var wif = wifs[i];
                if (this.Visibility != Visibility.Visible)
                    return;
                //var result =await driverControl.AddAddressByWif(wif);
                Result result = await ManagerControl.Ins.ToDo(EnumControl.AddAddress, wif);
                if (result.errorCode == EnumError.AddAddressSuc)
                {
                    img_state.Visibility = Visibility.Visible;
                    BitmapImage myBitmapImage = new BitmapImage();
                    myBitmapImage.BeginInit();
                    myBitmapImage.UriSource = new Uri("pack://application:,,,/res/打勾.png");
                    myBitmapImage.EndInit();
                    img_state.Source = myBitmapImage;
                    label_state.Visibility = Visibility.Hidden;
                }
                else
                {
                    img_state.Visibility = Visibility.Visible;
                    BitmapImage myBitmapImage = new BitmapImage();
                    myBitmapImage.BeginInit();
                    myBitmapImage.UriSource = new Uri("pack://application:,,,/res/警告.png");
                    myBitmapImage.EndInit();
                    img_state.Source = myBitmapImage;
                    label_state.Visibility = Visibility.Visible;
                    label_state.Content = result.errorCode;
                    label_state.Foreground = new SolidColorBrush(Color.FromRgb(255,106,106));
                }

            }
        }
    }
}
