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
            this.label_Wif.Text = "";
        }

        private void Action_LostFocus(object sender, RoutedEventArgs e)
        {
            if (string.IsNullOrEmpty(this.label_Wif.Text))
            {
                this.label_Wif.Text = "请输入您的WIF";
            }
        }
    }
}
