using NBitcoin;
using NeoDun;
using System;
using System.Collections.Generic;
using System.Diagnostics;
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
    /// MainDialogue.xaml 的交互逻辑
    /// </summary>
    public partial class MainDialogue : Window
    {
        private System.Windows.Forms.NotifyIcon notifyIcon;
        public DriverControl driverControl;
        public MainDialogue()
        {
            InitializeComponent();
            DriverS.Init();

            driverControl = new DriverControl();

            hhgate.CustomServer.BeginServer();
            hhgate.Api.Ins.driverControl = driverControl;

            this.notifyIcon = new System.Windows.Forms.NotifyIcon();
            this.notifyIcon.Icon = new System.Drawing.Icon(@"Neodun.ico");
            this.notifyIcon.Visible = true;
            //打开菜单项
            System.Windows.Forms.MenuItem open = new System.Windows.Forms.MenuItem("Open");
            open.Click += new EventHandler(Show);
            //退出菜单项
            System.Windows.Forms.MenuItem exit = new System.Windows.Forms.MenuItem("Exit");
            exit.Click += new EventHandler(Close);
            //关联托盘控件
            System.Windows.Forms.MenuItem[] childen = new System.Windows.Forms.MenuItem[] { open, exit };
            notifyIcon.ContextMenu = new System.Windows.Forms.ContextMenu(childen);

            this.notifyIcon.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler((o, e) =>
            {
                if (e.Button == System.Windows.Forms.MouseButtons.Left) this.Show(o, e); 
            });
            CreateSimHardware();

            this.list_btns.Add(this.Btn_gj_update);
            this.list_btns.Add(this.manageAddr);
            this.list_btns.Add(this.importWif);
            this.list_btns.Add(this.importWallet);

            LinkSinger();

            //hhgate.CustomServer.BeginServer();
        }

        bool _islinking = false;
        private void LinkSinger()
        {
            System.Windows.Threading.DispatcherTimer timer = new System.Windows.Threading.DispatcherTimer();
            timer.Interval = TimeSpan.FromSeconds(1.0);
            timer.Tick += new EventHandler(async (_s, _e) => {
                if (!string.IsNullOrEmpty(driverControl.CheckDevice()))//有签名机连接了
                {
                    if (!_islinking)
                    {
                        _islinking = true;
                        GetPackageInfo();
                        ShowUnlinkPage(true);
                        ShowBalloonTip("NeoDun已经连接");
                    }
                }
                else
                {
                    if (_islinking)
                    {
                        ShowBalloonTip("NeoDun已经拔出");
                        _islinking = false;
                    }
                    ShowUnlinkPage(false);
                    DialogueControl.CloseAllWindow();
                }
                await Task.Delay(1000);
            });
            timer.Start();
        }


        //模拟插入钱包
        WindowSimHardware hard = new WindowSimHardware();
        private async void CreateSimHardware()
        {
            DriverS.simdriver.bActive = true;
            hard.Show();
            await Task.Delay(2000);
        }

        private void Show(object sender, EventArgs e)
        {
            this.Activate();
            this.Visibility = System.Windows.Visibility.Visible;
            this.ShowInTaskbar = true;
        }
        private void Close(object sender, EventArgs e)
        {
            Environment.Exit(0);
        }

        private void Window_Activated(object sender, EventArgs e)
        {

        }
        private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }

        private void Btn_Minimized(object sender, RoutedEventArgs e)
        {
            this.WindowState = WindowState.Minimized;
        }

        private void Btn_Hide(object sender, RoutedEventArgs e)
        {
            this.Hide();
        }

        private void btn_gujian_update_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {

        }

        //气泡框的显示
        private void ShowBalloonTip(string content)
        {
            System.Windows.Forms.ToolTipIcon type = System.Windows.Forms.ToolTipIcon.Info;
            this.notifyIcon.ShowBalloonTip(1000, "通知", content, type);
        }

        private void ShowUnlinkPage(bool islink)
        {
            Dispatcher.Invoke((Action)delegate ()
            {
                this.unlink.Visibility = islink ? Visibility.Hidden : Visibility.Visible;
                this.main.Visibility = islink ? Visibility.Visible : Visibility.Hidden;
                //关掉当前页面所打开的二级页面
            });
        }

        private async void GetPackageInfo()
        {
            ForbidAllBtnClick();

            //从服务器获取固件和插件的版本信息
            MyJson.JsonNode_Object servicePackageInfo = new MyJson.JsonNode_Object();
            servicePackageInfo["gj"] = new MyJson.JsonNode_ValueNumber(3.1);
            servicePackageInfo["Neo"] = new MyJson.JsonNode_ValueNumber(9.1);


            //获取固件插件版本号
            MyJson.JsonNode_Object JA_PackageInfo = await driverControl.GetPackageInfo();

            if (JA_PackageInfo.Count > 0)
            {
                this.label_gjversion.Content = "固件(V" +JA_PackageInfo["gj"].ToString()+")";
                double nowgjversion = double.Parse(JA_PackageInfo["gj"].ToString());
                //如果下位机固件版本低于服务器版本就显示升级按钮
                this.Btn_gj_update.Visibility = nowgjversion < double.Parse(servicePackageInfo["gj"].ToString())? Visibility.Visible : Visibility.Hidden;


                this.listbox.Items.Clear();

                //现在只有neo 先这么搞
                var demoItem = this.listboxDemo.Items[0] as ListBoxItem;
                string xaml = System.Windows.Markup.XamlWriter.Save(demoItem);
                ListBoxItem item = System.Windows.Markup.XamlReader.Parse(xaml) as ListBoxItem;
                var img_icon = item.FindName("img_icon") as Image;
                var label_version = item.FindName("label_version") as Label;
                label_version.Content = "Neo(V0.00)";

                var btn_install = item.FindName("btn_install") as Button;
                btn_install.Click += new RoutedEventHandler(Click_Install);
                btn_install.Name = "Neo_" + btn_install.Name;
                btn_install.Visibility = Visibility.Visible;
                this.list_btns.Add(btn_install);

                var btn_uninstall = item.FindName("btn_uninstall") as Button;
                btn_uninstall.Click += new RoutedEventHandler(Click_Uninstall);
                btn_uninstall.Name = "Neo_" + btn_uninstall.Name;
                btn_uninstall.Visibility = Visibility.Collapsed;
                this.list_btns.Add(btn_uninstall);

                var btn_update = item.FindName("btn_update") as Button;
                btn_update.Click += new RoutedEventHandler(Click_Install);
                btn_update.Name = "Neo_" + btn_update.Name;
                btn_update.Visibility = Visibility.Collapsed;
                this.list_btns.Add(btn_update);

                var git_loading = item.FindName("gif_loading") as Image;
                if (this.FindName("Neo_" + git_loading.Name) != null)
                    this.UnregisterName("Neo_" + git_loading.Name);
                this.RegisterName("Neo_" + git_loading.Name, git_loading);
                git_loading.Visibility = Visibility.Hidden;

                if (JA_PackageInfo.ContainsKey("Neo"))
                {
                    var verson = float.Parse(JA_PackageInfo["Neo"].ToString());
                    label_version.Content = "Neo(V" + verson.ToString("0.00") + ")";
                    if (verson < double.Parse(servicePackageInfo["Neo"].ToString()))
                    {
                        btn_update.Visibility = Visibility.Visible;
                        btn_update.Margin = new Thickness(-160, 10, 0, 0);
                        btn_uninstall.Visibility = Visibility.Visible;
                    }
                    else
                    {
                        btn_update.Visibility = Visibility.Hidden;
                    }
                    btn_install.Visibility = Visibility.Hidden;

                }
                else
                {
                    label_version.Content = "Neo(未安装)";
                    btn_install.Visibility = Visibility.Visible;
                }
                this.listbox.Items.Add(item);
            }
            else
            {
                this.Btn_gj_update.Visibility = Visibility.Hidden;
            }

            await driverControl.GetAddressList();

            AllowAllBtnClick();
        }

        private void Btn_ManageAddr(object sender, RoutedEventArgs e)
        {
            DialogueControl.ShowAddressListDialogue(driverControl, this);
        }

        private void Btn_ImportWif(object sender, RoutedEventArgs e)
        {
            DialogueControl.ShowImportWifDialogue(driverControl,this);
        }

        private void Btn_ImportWallet(object sender, RoutedEventArgs e)
        {
            DialogueControl.ShowImportWalletDialogue(driverControl,this);
        }

        private void Window_ShowWebWallet(object sender, RoutedEventArgs e)
        {
            Process.Start("https://wallet.nel.group/");
        }

        private async void Click_update_gujian(object sender, RoutedEventArgs e)
        {
            Button btn = (sender as Button);
            ForbidAllBtnClick();
            var str_content = btn.Name.Split('_')[0];

            //安装按钮隐藏  等待按钮显示
            Image img = this.FindName("gj_gif_loading") as Image;
            img.Visibility = Visibility.Visible;
            btn.Visibility = Visibility.Hidden;

            UInt16 type = 0x0000;
            UInt16 content = 0x0000;
            //从本地获取需要安装的插件
            //获取最新的bin
            byte[] data = System.IO.File.ReadAllBytes("./gujian.bin");
            bool result = await driverControl.ApplyForUpdate();
            if (result)
            {
                DialogueControl.ShowMessageDialogue("同意更新固件", 2, this);

                result = await driverControl.Update();
                if (result)
                {
                    DialogueControl.ShowMessageDialogue("安装成功", 2, this);
                }
                else
                {
                    DialogueControl.ShowMessageDialogue("安装失败", 2, this);
                }

            }
            else
            {
                DialogueControl.ShowMessageDialogue("拒绝更新固件", 2, this);
            }

            img.Visibility = Visibility.Hidden;
            btn.Visibility = Visibility.Visible;

            AllowAllBtnClick();
        }

        private async void Click_Install(object sender, RoutedEventArgs e)
        {
            Button btn = (sender as Button);
            ForbidAllBtnClick();
            var str_content = btn.Name.Split('_')[0];

            //安装按钮隐藏  等待按钮显示
            Image img = this.FindName(str_content + "_gif_loading") as Image;
            img.Visibility = Visibility.Visible;
            btn.Visibility = Visibility.Hidden;

            UInt16 type = 0x0001;
            UInt16 content = (UInt16)Enum.Parse(typeof(AddressType), str_content);
            //从本地获取需要安装的插件
            //获取最新的bin
            byte[] data = System.IO.File.ReadAllBytes("./neo.bin");
            bool result = await driverControl.UpdateApp(data, type, content, 0x0001);
            if (result)
            {
                DialogueControl.ShowMessageDialogue("安装成功",2, this);
                GetPackageInfo();
            }
            else
            {
                DialogueControl.ShowMessageDialogue("安装失败",2, this);
            }


            AllowAllBtnClick();
        }

        private async void Click_Uninstall(object sender, RoutedEventArgs e)
        {
            var str_content = (sender as Button).Name.Split('_')[0];
            UInt16 content = (UInt16)Enum.Parse(typeof(AddressType), str_content);
            bool result = await driverControl.UninstallApp(content);
            if (result)
            {
                DialogueControl.ShowMessageDialogue("卸载成功",2, this);
                GetPackageInfo();
            }
            else
            {
                DialogueControl.ShowMessageDialogue("卸载失败", 2, this);
            }
        }


        List<Button> list_btns = new List<Button>();
        //禁止所有的按键点击
        private void ForbidAllBtnClick()
        {
            for(var i =0;i<list_btns.Count;i++)
            {
                var btn = list_btns[i];
                if (this.FindName(btn.Name) == null)
                    list_btns.Remove(btn);
                else
                    btn.IsEnabled = false;
            }
        }

        //禁止所有的按键点击
        private void AllowAllBtnClick()
        {
            for (var i = 0; i < list_btns.Count; i++)
            {
                var btn = list_btns[i];
                if (this.FindName(btn.Name) == null)
                    list_btns.Remove(btn);
                else
                    btn.IsEnabled = true;
            }
        }
    }
}
