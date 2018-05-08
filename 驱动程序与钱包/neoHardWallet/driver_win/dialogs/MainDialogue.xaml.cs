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

            GetPackageInfo();
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

        private async void GetPackageInfo()
        {
            //获取当前最高版本的bin文件
            //未加密的数据包
            var files = Directory.GetFiles("./bin/", "*.bin");
            byte[] data = System.IO.File.ReadAllBytes("./bin/a.bin");



            return;
            //获取固件插件版本号
            MyJson.JsonNode_Array JA_PackageInfo = await driverControl.GetPackageInfo();

            if (JA_PackageInfo.Count > 0)
            {
                this.label_gjversion.Content = (JA_PackageInfo[0] as MyJson.JsonNode_Object)["version"].ToString();

                for (var i = 1; i < JA_PackageInfo.Count; i++)
                {

                }
            }
            else
            {

            }
        }

        private void Btn_ManageAddr(object sender, RoutedEventArgs e)
        {
            DialogueControl.ShowAddressListDialogue(driverControl, this);
        }

        private void Btn_ImportWif(object sender, RoutedEventArgs e)
        {
            DialogueControl.ShowImportWifDialogue(driverControl,this);
        }

        private void Click_install_gujian(object sender, RoutedEventArgs e)
        {

        }

        private void Btn_ImportWallet(object sender, RoutedEventArgs e)
        {
            DialogueControl.ShowImportWalletDialogue(driverControl,this);
        }

        private void Window_ShowWebWallet(object sender, RoutedEventArgs e)
        {
            Process.Start("https://wallet.nel.group/");
        }
    }
}
