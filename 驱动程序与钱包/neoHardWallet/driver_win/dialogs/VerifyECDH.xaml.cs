using driver_win.control;
using driver_win.helper;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
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
    /// VerifyECDH.xaml 的交互逻辑
    /// </summary>
    public partial class VerifyECDH : Window
    {
        public VerifyECDH()
        {
            InitializeComponent();
            Init();
        }

        public async void Init()
        {
            //和下位机通讯获取下位机的pubkey
            await ManagerControl.Ins.ToDo(EnumControl.SecurityChannel);
            //已经获取到了
            this.FillInHash.Visibility = Visibility.Visible;
            ChangeFocusable(1);
            this.textBox1.Focus();
        }

        private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {

        }

        private void ChangeFocusable(int index)
        {
            this.textBox1.Focusable = index==1? true:false;
            this.textBox2.Focusable = index==2? true:false;
            this.textBox3.Focusable = index==3? true:false;
            this.textBox4.Focusable = index==4? true:false;
        }

        private async void Text1Changed(object sender, TextChangedEventArgs e)
        {
            string str = this.textBox1.Text;
            if (str.Length >= 1)
            {
                str = str[str.Length - 1].ToString();
                Regex re = new Regex("[^0-9a-zA-Z.-]+");
                if (re.IsMatch(this.textBox1.Text))
                {
                    str = "";
                    await Task.Delay(500);
                    this.textBox1.Text = str;
                }
                else
                {
                    this.textBox1.Text = str;
                    ChangeFocusable(2);
                    this.textBox2.Focus();
                }

            }
        }
        private async void Text2Changed(object sender, TextChangedEventArgs e)
        {
            string str = this.textBox2.Text;
            if (str.Length >= 1)
            {
                str = str[str.Length - 1].ToString();
                Regex re = new Regex("[^0-9a-zA-Z.-]+");
                if (re.IsMatch(this.textBox2.Text))
                {
                    str = "";
                    await Task.Delay(500);
                    this.textBox2.Text = str;
                }
                else
                {
                    this.textBox2.Text = str;
                    ChangeFocusable(3);
                    this.textBox3.Focus();
                }

            }
        }
        private async void Text3Changed(object sender, TextChangedEventArgs e)
        {
            string str = this.textBox3.Text;
            if (str.Length >= 1)
            {
                str = str[str.Length - 1].ToString();
                Regex re = new Regex("[^0-9a-zA-Z.-]+");
                if (re.IsMatch(this.textBox3.Text))
                {
                    str = "";
                    await Task.Delay(500);
                    this.textBox3.Text = str;
                }
                else
                {
                    this.textBox3.Text = str;
                    ChangeFocusable(4);
                    this.textBox4.Focus();
                }
            }
        }
        private async void Text4Changed(object sender, TextChangedEventArgs e)
        {
            string str = this.textBox4.Text;
            if (str.Length >= 1)
            {
                str = str[str.Length - 1].ToString();
                Regex re = new Regex("[^0-9a-zA-Z.-]+");
                if (re.IsMatch(this.textBox4.Text))
                {
                    str = "";
                    await Task.Delay(500);
                    this.textBox4.Text = str;
                }
                else
                {
                    this.textBox4.Text = str;
                    ChangeFocusable(5);
                    //验证hash
                    StartVerity();
                }

            }
        }

        private async void StartVerity()
        {
            this.lb_state2.Content = "验证中...";
            this.border_state2.Visibility = Visibility.Visible;

            //应sb产品需求自我等待500毫秒以欺骗使用者
            await Task.Delay(500);
            string str = this.textBox1.Text + this.textBox2.Text + this.textBox3.Text + this.textBox4.Text;
            ECDH.Ins.CheckPubHash(str);
            if (ECDH.Ins.CheckResult)
            {//验证成功
                this.lb_state2.Content= "安全检查已完成！";
                this.state.Content = "安全验证已通过！";
                System.Windows.Media.Color color = (System.Windows.Media.Color)System.Windows.Media.ColorConverter.ConvertFromString("#2DDE4F");
                this.border1.BorderBrush = new SolidColorBrush(color);
                this.border2.BorderBrush = new SolidColorBrush(color);
                this.border3.BorderBrush = new SolidColorBrush(color);
                this.border4.BorderBrush = new SolidColorBrush(color);
                await Task.Delay(1000);
                this.Close();
            }
            else
            {//验证失败
                this.lb_state2.Content = "安全验证未通过！";
                this.state.Content = "安全验证未通过！";
                System.Windows.Media.Color color = (System.Windows.Media.Color)System.Windows.Media.ColorConverter.ConvertFromString("#FF6C6C");
                this.border1.BorderBrush = new SolidColorBrush(color);
                this.border2.BorderBrush = new SolidColorBrush(color);
                this.border3.BorderBrush = new SolidColorBrush(color);
                this.border4.BorderBrush = new SolidColorBrush(color);
                this.help.Visibility = Visibility.Visible;
                this.helpLink.Visibility = Visibility.Visible;
                this.restart.Visibility = Visibility.Visible;
            }
        }

        private void Btn_Restart(object sender, RoutedEventArgs e)
        {
            this.state.Content = "正在进行安全验证...";
            this.border_state2.Visibility = Visibility.Hidden;
            this.textBox1.Text = "";
            this.textBox2.Text = "";
            this.textBox3.Text = "";
            this.textBox4.Text = "";
            ChangeFocusable(1);
            this.textBox1.Focus();
            System.Windows.Media.Color color = (System.Windows.Media.Color)System.Windows.Media.ColorConverter.ConvertFromString("#6676C8");
            this.border1.BorderBrush = new SolidColorBrush(color);
            this.border2.BorderBrush = new SolidColorBrush(color);
            this.border3.BorderBrush = new SolidColorBrush(color);
            this.border4.BorderBrush = new SolidColorBrush(color);
        }

        private void GoToHelp(object sender, RoutedEventArgs e)
        {
            Hyperlink link = sender as Hyperlink;
            Process.Start(new ProcessStartInfo(link.NavigateUri.AbsoluteUri));
        }

        private void Btn_CloseDialogue(object sender, RoutedEventArgs e)
        {
            this.DialogResult = true;
        }
    }
}
