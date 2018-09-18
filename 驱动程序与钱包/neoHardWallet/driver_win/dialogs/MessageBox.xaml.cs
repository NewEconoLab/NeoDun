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
            this.restart.Text = Mgr_Language.Ins.Code2Word(this.restart.Text);
        }

        public ND_MessageBoxResult nD_MessageBoxResult = ND_MessageBoxResult.None;

        public async Task<ND_MessageBoxResult> Show(string messageBoxText,ND_MessageBoxButton button,long waitTime)
        {
            this.msg.Content = messageBoxText;
            if (this.msg.Content.ToString().Length < 40)
                this.msg.FontSize = 15;
            else
            {
                var num = (this.msg.Content.ToString().Length - 40) / 5;
                this.msg.FontSize = (15 - (int)num) <=9 ?9:(15 - (int)num);
            }
            if (button == ND_MessageBoxButton.OK)
            {
                Grid.SetColumn(this.confirm, 3);
                Grid.SetRow(this.confirm, 0);
                this.confirm.Visibility = Visibility.Visible;
                this.cancel.Visibility = Visibility.Hidden;
                this.restart.Visibility = Visibility.Hidden;

            }
            else if (button == ND_MessageBoxButton.OKCancel)
            {
                Grid.SetColumn(this.confirm, 3);
                Grid.SetRow(this.confirm, 0);
                Grid.SetColumn(this.cancel, 4);
                Grid.SetRow(this.cancel, 0);
                this.confirm.Visibility = Visibility.Visible;
                this.cancel.Visibility = Visibility.Visible;
                this.restart.Visibility = Visibility.Hidden;

            }
            else if (button == ND_MessageBoxButton.RestartCancel)
            {
                Grid.SetColumn(this.restart, 3);
                Grid.SetRow(this.restart, 0);
                Grid.SetColumn(this.cancel, 4);
                Grid.SetRow(this.cancel, 0);
                this.confirm.Visibility = Visibility.Hidden;
                this.cancel.Visibility = Visibility.Visible;
                this.restart.Visibility = Visibility.Visible;
            }
            else
            {
                this.confirm.Visibility = Visibility.Hidden;
                this.cancel.Visibility = Visibility.Hidden;
                this.restart.Visibility = Visibility.Hidden;

            }
            this.Show();
            if (waitTime ==  999999)
            {
                return nD_MessageBoxResult;
            }
            int i = 0;
            while (true)
            {
                await Task.Delay(1000);
                i++;
                if (i >= waitTime)
                {
                    this.Close();
                    break;
                }
                if (nD_MessageBoxResult != ND_MessageBoxResult.None)
                {
                    this.Close();
                    break;
                }
            }
            return nD_MessageBoxResult;
        }

        private void Click_OK(object sender, RoutedEventArgs e)
        {
            nD_MessageBoxResult = ND_MessageBoxResult.OK;
        }

        private void Click_CANCEL(object sender, RoutedEventArgs e)
        {
            nD_MessageBoxResult = ND_MessageBoxResult.Cancel;
        }

        private void Click_Restart(object sender, MouseButtonEventArgs e)
        {
            nD_MessageBoxResult = ND_MessageBoxResult.Restart;
        }
    }
}
