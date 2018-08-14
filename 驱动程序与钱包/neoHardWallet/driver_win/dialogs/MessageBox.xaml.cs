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
        }

        public ND_MessageBoxResult nD_MessageBoxResult;

        public void Show(string messageBoxText,ND_MessageBoxButton button,long waitTime=9999999999)
        {
            this.msg.Text = messageBoxText;
            if (button == ND_MessageBoxButton.OK)
            {
                Grid.SetColumn(this.ok,2);
                Grid.SetRow(this.ok, 1);
                this.ok.Visibility = Visibility.Visible;
                this.cancel.Visibility = Visibility.Hidden;
            }
            else if (button == ND_MessageBoxButton.OKCancel)
            {
                Grid.SetColumn(this.ok, 1);
                Grid.SetRow(this.ok, 1);
                Grid.SetColumn(this.cancel, 3);
                Grid.SetRow(this.cancel, 1);
                this.ok.Visibility = Visibility.Visible;
                this.cancel.Visibility = Visibility.Visible;
            }
            else
            {
                this.ok.Visibility = Visibility.Hidden;
                this.cancel.Visibility = Visibility.Hidden;
            }
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
    }
}
