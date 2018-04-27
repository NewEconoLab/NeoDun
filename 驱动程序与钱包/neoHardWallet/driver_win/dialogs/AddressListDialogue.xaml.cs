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
    /// AddressListDialogue.xaml 的交互逻辑
    /// </summary>
    public partial class AddressListDialogue : Window
    {
        public AddressListDialogue()
        {
            InitializeComponent();
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
    }
}
