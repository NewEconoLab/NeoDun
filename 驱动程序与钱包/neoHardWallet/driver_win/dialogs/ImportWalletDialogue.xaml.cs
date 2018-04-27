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
        public ImportWalletDialogue()
        {
            InitializeComponent();
        }

        private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }

        private void Btn_CloseDialogue(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void Btn_OpenFile(object sender, RoutedEventArgs e)
        {
            try
            {
                Microsoft.Win32.OpenFileDialog dialog = new Microsoft.Win32.OpenFileDialog();
                dialog.Filter = "文本文件|*.json";
                if (dialog.ShowDialog() == true)
                {

                }
                else
                {

                }
            }
            catch
            {
            }
        }

        private void Btn_Add(object sender, RoutedEventArgs e)
        {

        }
    }
}
