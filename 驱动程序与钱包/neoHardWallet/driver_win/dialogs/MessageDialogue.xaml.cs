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
    /// MessageDialogue.xaml 的交互逻辑
    /// </summary>
    public partial class MessageDialogue : Window
    {
        public MessageDialogue(string message)
        {
            InitializeComponent();

            this.msg.Content = message;
        }

        public void Show(int time)
        {
            this.Show();
            int i = 0;
            while (true)
            {
                System.Threading.Thread.Sleep(1000);
                i++;
                if (i >= time)
                    break;
            }
            this.Close();
        }
    }
}
