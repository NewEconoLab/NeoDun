using System;
using System.Collections.Generic;
using System.IO;
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
    /// AddressListDialogue.xaml 的交互逻辑
    /// </summary>
    public partial class AddressListDialogue : Window
    {
        private DriverControl driverControl;
        public AddressListDialogue(DriverControl _driverControl)
        {
            InitializeComponent();
            driverControl = _driverControl;

            GetAddressList();
        }

        private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }

        private void Image_MouseEnter(object sender, MouseEventArgs e)
        {
            this.lab_tips1.Visibility = Visibility.Visible;
        }

        private void Image_MouseLeave(object sender, MouseEventArgs e)
        {
            this.lab_tips1.Visibility = Visibility.Hidden;
        }

        private void Btn_CloseDialogue(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
        //获取下位机地址数据
        private async void GetAddressList()
        {
            this.listbox.Items.Clear();
            var address = await driverControl.GetAddressList();
            var demoItem = this.listboxDemo.Items[0] as ListBoxItem;
            for (var i =0;i<address.Count;i++)
            {
                string xaml = System.Windows.Markup.XamlWriter.Save(demoItem);
                ListBoxItem item = System.Windows.Markup.XamlReader.Parse(xaml) as ListBoxItem;
                var add = address.ToList()[i];
                var label_address = item.FindName("address") as Label;
                var label_index = item.FindName("index") as Label;
                var label_name = item.FindName("name") as Label;
                var label_type = item.FindName("type") as Label;
                var btn_copy = item.FindName("copy") as Button;
                var btn_delete = item.FindName("delete") as Button;
                label_address.Content = add.AddressText;
                label_name.Content = add.type;
                label_type.Content = add.type;
                label_index.Content = (i + 1).ToString("00");
                btn_copy.Click += Btn_Copy;
                btn_delete.Click += Btn_Delete;
                this.listbox.Items.Add(item);
            }

        }
        private void Btn_Copy(object sender, RoutedEventArgs e)
        {
            var listboxitem = ((sender as Button).Parent as Grid).Parent as ListBoxItem;
            var address = (listboxitem.FindName("address") as Label).Content;
            StringBuilder Copystr = new StringBuilder();
            Copystr.Append(address.ToString());
            Clipboard.SetText(Copystr.ToString());

            MessageDialogue messageDialogue = new MessageDialogue("复制成功");
            messageDialogue.WindowStartupLocation = WindowStartupLocation.CenterOwner;
            messageDialogue.Owner = this;
            messageDialogue.Show(1);
        }

        private async void Btn_Delete(object sender, RoutedEventArgs e)
        {

            var listboxitem = ((sender as Button).Parent as Grid).Parent as ListBoxItem;
            var address = (listboxitem.FindName("address") as Label).Content;
            var type = (listboxitem.FindName("type") as Label).Content;
            var name = (listboxitem.FindName("type") as Label).Content;

            var lb_msg_name = this.message.FindName("lb_msg_name") as Run;
            lb_msg_name.Text = name.ToString() ;
            var lb_msg_addr = this.message.FindName("lb_msg_addr") as Run;
            lb_msg_addr.Text = address.ToString();
            this.message.Visibility = Visibility.Visible;

            string result = await driverControl.DeleteAddress(type.ToString(),address.ToString());
            MessageDialogue messageDialogue = new MessageDialogue(result);
            messageDialogue.WindowStartupLocation = WindowStartupLocation.CenterOwner;
            messageDialogue.Owner = this;
            messageDialogue.Show(1);
            GetAddressList();
        }
    }
}
