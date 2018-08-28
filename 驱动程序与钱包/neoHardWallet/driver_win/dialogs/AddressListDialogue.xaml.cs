using driver_win.control;
using driver_win.helper;
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
        public AddressListDialogue()
        {
            InitializeComponent();
            InitPage();
            GetAddressList();
        }

        public void InitPage()
        {
            this.lb_page_address.Content = Mgr_Language.Ins.Code2Word(this.lb_page_address.Content.ToString());
            this.lb_page_empty.Content = Mgr_Language.Ins.Code2Word(this.lb_page_empty.Content.ToString());
            this.lb_page_index.Content = Mgr_Language.Ins.Code2Word(this.lb_page_index.Content.ToString());
            this.lb_page_name.Content = Mgr_Language.Ins.Code2Word(this.lb_page_name.Content.ToString());
            this.lb_page_wallet.Content = Mgr_Language.Ins.Code2Word(this.lb_page_wallet.Content.ToString());
            this.lb_page_message_1.Text = Mgr_Language.Ins.Code2Word(this.lb_page_message_1.Text);
            this.lb_page_message_2.Text = Mgr_Language.Ins.Code2Word(this.lb_page_message_2.Text);
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
            this.gif_loading.Visibility = Visibility.Visible;
            this.DialogResult = true;
        }
        //获取下位机地址数据
        private async void GetAddressList()
        {
            this.listbox.Items.Clear();
            Result result = await ManagerControl.Ins.ToDo(EnumControl.GetAddress);
            if (result.msgCode != EnumMsgCode.GetAddressSuc)
                return;
            var address = (System.Collections.Concurrent.ConcurrentBag<NeoDun.Address>)result.data;
            var demoItem = this.listboxDemo.Items[0] as ListBoxItem;
            for (var i =0;i<address.Count;i++)
            {
                string xaml = System.Windows.Markup.XamlWriter.Save(demoItem);
                ListBoxItem item = System.Windows.Markup.XamlReader.Parse(xaml) as ListBoxItem;
                var add = address.ToList()[i];
                var label_address = item.FindName("address") as Label;
                var label_index = item.FindName("index") as Label;
                var label_name = item.FindName("name") as TextBox;
                var label_type = item.FindName("type") as Label;
                var btn_copy = item.FindName("copy") as Button;
                var btn_delete = item.FindName("delete") as Button;
                var btn_setName = item.FindName("setName") as Button;
                label_address.Content = add.AddressText;

                if (!string.IsNullOrEmpty(add.name))
                {
                    label_name.Text = add.name;
                }
                label_type.Content = add.type;
                label_index.Content = (i + 1).ToString("00");
                label_name.GotFocus += Actrion_GotFocus;
                btn_setName.Click += Click_SetName;
                btn_copy.Click += Click_Copy;
                btn_delete.Click += Click_Delete;
                this.listbox.Items.Add(item);
            }
            if (address.Count == 0)
            {
                this.noWallet.Visibility = Visibility.Visible;
            }
            this.gif_loading.Visibility = Visibility.Hidden;
        }
        private void Click_Copy(object sender, RoutedEventArgs e)
        {
            var listboxitem = ((sender as Button).Parent as Grid).Parent as ListBoxItem;
            var address = (listboxitem.FindName("address") as Label).Content;
            StringBuilder Copystr = new StringBuilder();
            Copystr.Append(address.ToString());
            Clipboard.SetText(Copystr.ToString());

            DialogueControl.ShowMessageDialogue("复制成功", 1, this);
        }

        private async void Click_Delete(object sender, RoutedEventArgs e)
        {

            var listboxitem = ((sender as Button).Parent as Grid).Parent as ListBoxItem;
            var address = (listboxitem.FindName("address") as Label).Content;
            var type = (listboxitem.FindName("type") as Label).Content;
            var name = (listboxitem.FindName("type") as Label).Content;

            var a = "{0}";

            var lb_msg_name = this.message.FindName("lb_msg_name") as Run;
            lb_msg_name.Text = name.ToString() ;
            var lb_msg_addr = this.message.FindName("lb_msg_addr") as Run;
            lb_msg_addr.Text = address.ToString();
            this.message.Visibility = Visibility.Visible;

            //string result = await driverControl.DeleteAddress(type.ToString(),address.ToString());

            Result result = await ManagerControl.Ins.ToDo(EnumControl.DelAddress, type.ToString(), address.ToString());
            if (result.msgCode == EnumMsgCode.DeletingName)
                return;
            this.message.Visibility = Visibility.Collapsed;
            //DialogueControl.ShowMessageDialogue(result.errorCode.ToString(), 1, this);
            GetAddressList();
        }

        private async void Click_SetName(object sender, RoutedEventArgs e)
        {
            this.gif_loading.Visibility = Visibility.Visible;
            Button setName = sender as Button;
            TextBox tb = (setName.Parent as Grid).FindName("name") as TextBox;
            Label lb = (setName.Parent as Grid).FindName("address") as Label;
            var name = tb.Text;
            var address = lb.Content.ToString();
            byte[] bytes_name = System.Text.Encoding.UTF8.GetBytes(name);

            //string result = await driverControl.SetName(address, bytes_name);
            Result result = await ManagerControl.Ins.ToDo(EnumControl.SetName, address, bytes_name);
            this.gif_loading.Visibility = Visibility.Hidden;
            setName.Focus();
            setName.Visibility = Visibility.Hidden;

            DialogueControl.ShowMessageDialogue(result.errorCode.ToString(), 2,this);
        }

        Button cur_setName;
        private void Actrion_GotFocus(object sender, RoutedEventArgs e)
        {
            if(cur_setName != null)
                cur_setName.Visibility = Visibility.Hidden;

            TextBox tb = sender as TextBox;
            Button setName = (tb.Parent as Grid).FindName("setName") as Button;
            setName.Visibility = Visibility.Visible;
            cur_setName = setName;
        }
    }
}
