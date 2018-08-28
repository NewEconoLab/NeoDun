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
using driver_win.helper;

namespace driver_win.dialogs
{
    /// <summary>
    /// MessageBox.xaml 的交互逻辑
    /// </summary>
    public partial class MessageBox
    {
        public MessageBox()
        {
            InitializeComponent();
            InitPage();
        }

        public void InitPage()
        {
            this.confirm.Text = Mgr_Language.Ins.Code2Word(this.confirm.Text);
            this.cancel.Text = Mgr_Language.Ins.Code2Word(this.cancel.Text);
        }

        public ND_MessageBoxResult nD_MessageBoxResult;

        public void Show(string messageBoxText,ND_MessageBoxButton button,long waitTime=0)
        {
            this.restart.Visibility = Visibility.Hidden;
            this.msg.Content = messageBoxText;
            if (button == ND_MessageBoxButton.OK)
            {
                Grid.SetColumn(this.confirm,2);
                Grid.SetRow(this.confirm, 1);
                this.confirm.Visibility = Visibility.Visible;
                this.cancel.Visibility = Visibility.Hidden;
            }
            else if (button == ND_MessageBoxButton.OKCancel)
            {
                Grid.SetColumn(this.confirm, 1);
                Grid.SetRow(this.confirm, 1);
                Grid.SetColumn(this.cancel, 3);
                Grid.SetRow(this.cancel, 1);
                this.confirm.Visibility = Visibility.Visible;
                this.cancel.Visibility = Visibility.Visible;
            }
            else
            {
                this.confirm.Visibility = Visibility.Hidden;
                this.cancel.Visibility = Visibility.Hidden;
            }
            this.Show();
            if (waitTime == 0)
                return;
            int i = 0;
            while (true)
            {
                System.Threading.Thread.Sleep(1000);
                i++;
                if (i >= waitTime)
                    break;
            }
            this.Hide();
        }

        private void Click_OK(object sender, RoutedEventArgs e)
        {
            nD_MessageBoxResult = ND_MessageBoxResult.OK;
            this.DialogResult = true;
        }

        private void Click_CANCEL(object sender, RoutedEventArgs e)
        {
            nD_MessageBoxResult = ND_MessageBoxResult.Cancel;
            this.DialogResult = true;
        }

        private void Click_Restart(object sender, MouseButtonEventArgs e)
        {

        }
    }
}
