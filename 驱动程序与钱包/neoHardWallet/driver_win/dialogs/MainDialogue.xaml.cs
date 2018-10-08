using driver_win.control;
using driver_win.helper;
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
        bool _islinking = false;

        public MainDialogue()
        {
            InitializeComponent();
            var ins = Mgr_Language.Ins;
            mainDialogue = this;
            this.WindowStartupLocation = WindowStartupLocation.CenterScreen;
            InitPage();
            DriverS.Init();

            byte[] a = new byte[2] { 0x01, 0x04 };
            int value;
            value = (int)((a[0] & 0xFF) | ((a[1] & 0xFF) << 8));

            this.list_btns.Add(this.Btn_Framework_update);
            this.list_btns.Add(this.manageAddr);
            this.list_btns.Add(this.importWif);
            this.list_btns.Add(this.importWallet);
            Notify();
            Signer.Ins.deleInstallFramework += InstallFramework;
            Signer.Ins.Start();

            //CreateSimHardware();

            LinkSinger();

            hhgate.CustomServer.BeginServer();

        }

        public static MainDialogue mainDialogue;

        void InitPage()
        {
            this.lb_page_apps.Content = Mgr_Language.Ins.Code2Word(this.lb_page_apps.Content.ToString());
            this.lb_page_autoimport.Content = Mgr_Language.Ins.Code2Word(this.lb_page_autoimport.Content.ToString());
            this.lb_page_browser.Content = Mgr_Language.Ins.Code2Word(this.lb_page_browser.Content.ToString());
            this.lb_page_frameware.Content = Mgr_Language.Ins.Code2Word(this.lb_page_frameware.Content.ToString());
            this.lb_page_import.Content = Mgr_Language.Ins.Code2Word(this.lb_page_import.Content.ToString());
            this.lb_page_manager.Content = Mgr_Language.Ins.Code2Word(this.lb_page_manager.Content.ToString());
            this.lb_page_state.Content = Mgr_Language.Ins.Code2Word(this.lb_page_state.Content.ToString());
            this.lb_page_title.Content = Mgr_Language.Ins.Code2Word(this.lb_page_title.Content.ToString());
            this.lb_page_unlink.Content = Mgr_Language.Ins.Code2Word(this.lb_page_unlink.Content.ToString());
            this.lb_page_wallet.Content = Mgr_Language.Ins.Code2Word(this.lb_page_wallet.Content.ToString());
            this.lb_page_support.Content = Mgr_Language.Ins.Code2Word(this.lb_page_support.Content.ToString());
            this.Btn_Framework_update.Content = Mgr_Language.Ins.Code2Word(this.Btn_Framework_update.Content.ToString());
            this.btn_install.Content = Mgr_Language.Ins.Code2Word(this.btn_install.Content.ToString());
            this.btn_update.Content = Mgr_Language.Ins.Code2Word(this.btn_update.Content.ToString());
            this.btn_uninstall.Content = Mgr_Language.Ins.Code2Word(this.btn_uninstall.Content.ToString());
        }

        //模拟插入钱包
        WindowSimHardware hard = new WindowSimHardware();
        private async void CreateSimHardware()
        {
            DriverS.simdriver.bActive = true;
            hard.Show();
            await Task.Delay(2000);
        }

        //托盤相關操作
        private void Notify()
        {
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
        }

        private void LinkSinger()
        {
            System.Windows.Threading.DispatcherTimer timer = new System.Windows.Threading.DispatcherTimer();
            timer.Interval = TimeSpan.FromSeconds(1.0);
            timer.Tick += new EventHandler(async (_s, _e) => {
                if (!string.IsNullOrEmpty(Signer.Ins.CheckDevice()))//有签名机连接了
                {
                    if (!_islinking)
                    {
                        _islinking = true;
                        GetPackageInfo();
                        ShowUnlinkPage(true);
                        ShowBalloonTip(Mgr_Language.Ins.Code2Word("$page_connected"));
                    }
                }
                else
                {
                    if (_islinking)
                    {
                        ShowBalloonTip(Mgr_Language.Ins.Code2Word("$page_disconnected"));
                        _islinking = false;
                    }
                    ManagerControl.Ins.Release();
                    ShowUnlinkPage(false);
                    DialogueControl.CloseAllWindow();
                }
                await Task.Delay(1000);
            });
            timer.Start();
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
            //this.DragMove();
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
        //顯示斷綫頁面
        private void ShowUnlinkPage(bool islink)
        {
            Dispatcher.Invoke((Action)delegate ()
            {
                this.unlink.Visibility = islink ? Visibility.Hidden : Visibility.Visible;
                this.main.Visibility = islink ? Visibility.Visible : Visibility.Hidden;
                //关掉当前页面所打开的二级页面
            });
        }
        MyJson.JsonNode_Object servicePackageInfo;
        //獲取下位機的固件和插件的版本信息
        private async void GetPackageInfo()
        {
            ForbidAllBtnClick();

            byte[] postdata;
            //从服务器获取固件和插件的版本信息
            var url = HttpHelper.MakeRpcUrlPost("https://apiaggr.nel.group/api/testnet", "getneodunversion", out postdata);
            var res = await HttpHelper.HttpPost(url, postdata);
            var json = MyJson.Parse(res).AsDict()["result"].AsList();

            servicePackageInfo = new MyJson.JsonNode_Object();
            for (var i = 0; i < json.Count; i++)
            {
                servicePackageInfo[json[i].AsDict()["Name"].AsString()] = new MyJson.JsonNode_ValueString(json[i].AsDict()["Version"].AsString());
            }

            //获取固件插件版本号
            //MyJson.JsonNode_Object JA_PackageInfo = await driverControl.GetPackageInfo();
            Result result = await ManagerControl.Ins.ToDo(EnumControl.GetPackage);
            MyJson.JsonNode_Object JA_PackageInfo = (MyJson.JsonNode_Object)result.data;

            if (JA_PackageInfo!= null && JA_PackageInfo.Count > 0)
            {
                this.label_FrameworkVersion.Content = "Framework " +JA_PackageInfo["Framework"].ToString();
                double nowgjversion = double.Parse(JA_PackageInfo["Framework"].ToString());
                //如果下位机固件版本低于服务器版本就显示升级按钮
                if (nowgjversion < double.Parse(servicePackageInfo["Framework"].ToString()))
                {
                    this.Btn_Framework_update.Visibility = Visibility.Visible;
                }
                else
                {
                    this.Btn_Framework_update.Visibility = Visibility.Hidden;
                }


                this.listbox.Items.Clear();

                //现在只有neo 先这么搞
                //初始化 neo的boxlist
                var demoItem = this.listboxDemo.Items[0] as ListBoxItem;
                string xaml = System.Windows.Markup.XamlWriter.Save(demoItem);
                ListBoxItem item = System.Windows.Markup.XamlReader.Parse(xaml) as ListBoxItem;
                var img_icon = item.FindName("img_icon") as Image;
                var label_version = item.FindName("label_version") as Label;
                label_version.Content = "Neo 0.00";

                var btn_install = item.FindName("btn_install") as Button;
                btn_install.Click += new RoutedEventHandler(Click_Install);
                if (this.FindName("Neo_" + btn_install.Name) != null)
                    this.UnregisterName("Neo_" + btn_install.Name);
                this.RegisterName("Neo_" + btn_install.Name, btn_install);
                btn_install.Visibility = Visibility.Visible;
                this.list_btns.Add(btn_install);

                var btn_uninstall = item.FindName("btn_uninstall") as Button;
                btn_uninstall.Click += new RoutedEventHandler(Click_Uninstall);
                if (this.FindName("Neo_" + btn_uninstall.Name) != null)
                    this.UnregisterName("Neo_" + btn_uninstall.Name);
                this.RegisterName("Neo_" + btn_uninstall.Name, btn_uninstall);
                btn_uninstall.Visibility = Visibility.Collapsed;
                this.list_btns.Add(btn_uninstall);

                var btn_update = item.FindName("btn_update") as Button;
                btn_update.Click += new RoutedEventHandler(Click_Install);
                if (this.FindName("Neo_" + btn_update.Name) != null)
                    this.UnregisterName("Neo_" + btn_update.Name);
                this.RegisterName("Neo_" + btn_update.Name, btn_update);
                btn_update.Visibility = Visibility.Collapsed;
                this.list_btns.Add(btn_update);

                var git_loading = item.FindName("gif_loading") as Image;
                if (this.FindName("Neo_" + git_loading.Name) != null)
                    this.UnregisterName("Neo_" + git_loading.Name);
                this.RegisterName("Neo_" + git_loading.Name, git_loading);
                git_loading.Visibility = Visibility.Collapsed;

                if (JA_PackageInfo.ContainsKey("Neo"))
                {
                    var verson = float.Parse(JA_PackageInfo["Neo"].ToString());
                    label_version.Content = "Neo " + verson.ToString("0.00");
                    if (verson < double.Parse(servicePackageInfo["Neo"].ToString()))
                    {
                        btn_update.Visibility = Visibility.Visible;
                        btn_update.Margin = new Thickness(-160, 10, 0, 0);
                        btn_uninstall.Visibility = Visibility.Visible;
                    }
                    else
                    {
                        btn_uninstall.Visibility = Visibility.Visible;
                        btn_update.Visibility = Visibility.Hidden;
                    }
                    btn_install.Visibility = Visibility.Hidden;

                }
                else
                {
                    label_version.Content = "Neo 未安装";
                    btn_install.Visibility = Visibility.Visible;
                }
                this.listbox.Items.Add(item);
                Result addressList = await ManagerControl.Ins.ToDo(EnumControl.GetAddress);
            }
            else
            {
                this.Btn_Framework_update.Visibility = Visibility.Hidden;
            }
            AllowAllBtnClick();
        }
        //點擊打開地址管理頁面
        private void Btn_ManageAddr(object sender, RoutedEventArgs e)
        {
            DialogueControl.ShowAddressListDialogue(this);
        }
        //點擊開打導入私鑰頁面
        private void Btn_ImportWif(object sender, RoutedEventArgs e)
        {
            DialogueControl.ShowImportWifDialogue(this);
        }
        //點擊打開導入錢包頁面
        private void Btn_ImportWallet(object sender, RoutedEventArgs e)
        {
            DialogueControl.ShowImportWalletDialogue(this);
        }
        //點擊打開網頁
        private void Window_ShowWebWallet(object sender, RoutedEventArgs e)
        {
            Process.Start("https://wallet.nel.group/");
        }

        //申請更新固件
        private async void Click_ApplyInstall_Framework(object sender, RoutedEventArgs e)
        {
            ForbidAllBtnClick();

            //安装按钮隐藏  等待按钮显示
            gif_Framework_loading.Visibility = Visibility.Visible;
            Btn_Framework_update.Visibility = Visibility.Hidden;

            //bool result = await driverControl.ApplyForUpdate();
            Result result = await ManagerControl.Ins.ToDo(EnumControl.ApplyInstallFramework);
            if (result.msgCode != EnumMsgCode.AgreeUpdate)
            {
                gif_Framework_loading.Visibility = Visibility.Hidden;
                Btn_Framework_update.Visibility = Visibility.Visible;

                AllowAllBtnClick();
            }

        }

        //更新固件
        public async void InstallFramework()
        {
            await Dispatcher.InvokeAsync(async ()=>
            {
                ForbidAllBtnClick();

                //安装按钮隐藏  等待按钮显示
                gif_Framework_loading.Visibility = Visibility.Visible;
                Btn_Framework_update.Visibility = Visibility.Hidden;

                var pluginType = this.label_FrameworkVersion.Content.ToString().Split(' ')[0];
                var version = servicePackageInfo[pluginType].ToString();
                var language = Mgr_Language.Ins.curEnumLanguage.ToString();

                byte[] postdata;
                //从服务器获取固件和插件的版本信息
                var url = HttpHelper.MakeRpcUrlPost("https://apiaggr.nel.group/api/testnet", "downloadplugin", out postdata, new MyJson.JsonNode_ValueString(pluginType + "_" + version+"_"+ language));
                var res = await HttpHelper.HttpPost(url, postdata);
                var str_plugin = MyJson.Parse(res).AsDict()["result"].AsList()[0].AsDict()["plugin"].ToString();
                byte[] data = ThinNeo.Helper.HexString2Bytes(str_plugin);


                EnumInstallType type = EnumInstallType.Framework;
                EnumPluginType content = EnumPluginType.Unknow;

                Result result = await ManagerControl.Ins.ToDo(EnumControl.InstallFramework, data, type, content,version);
                gif_Framework_loading.Visibility = Visibility.Hidden;
                Btn_Framework_update.Visibility = Visibility.Visible;

                AllowAllBtnClick();
            });

        }

        //點擊安裝插件
        private async void Click_Install(object sender, RoutedEventArgs e)
        {
            Button btn = (sender as Button);
            var pluginType = ((btn.Parent as StackPanel).Children[1] as Label).Content.ToString().Split(' ')[0];
            var version = servicePackageInfo[pluginType].ToString();
            ForbidAllBtnClick();

            //安装按钮隐藏  等待按钮显示
            Image img = this.FindName(pluginType + "_gif_loading") as Image;
            img.Visibility = Visibility.Visible;
            btn.Visibility = Visibility.Hidden;

            EnumInstallType type = EnumInstallType.Plugin;
            EnumPluginType content = (EnumPluginType)Enum.Parse(typeof(EnumPluginType), pluginType);

            byte[] postdata;
            //从服务器获取固件或插件
            var url = HttpHelper.MakeRpcUrlPost("https://apiaggr.nel.group/api/testnet", "downloadplugin", out postdata,new MyJson.JsonNode_ValueString(pluginType+"_"+version));
            var res = await HttpHelper.HttpPost(url, postdata);
            var str_plugin = MyJson.Parse(res).AsDict()["result"].AsList()[0].AsDict()["plugin"].ToString();
            byte[] data = ThinNeo.Helper.HexString2Bytes(str_plugin);
            
            Result result = await ManagerControl.Ins.ToDo(EnumControl.InstallPlugin, data, type, content, version);
            GetPackageInfo();
            img.Visibility = Visibility.Hidden;
            btn.Visibility = Visibility.Visible;
            AllowAllBtnClick();
        }
        //點擊卸載插件
        private async void Click_Uninstall(object sender, RoutedEventArgs e)
        {
            Button btn = (sender as Button);
            var pluginType = ((btn.Parent as StackPanel).Children[1] as Label).Content.ToString().Split(' ')[0];
            EnumPluginType content = (EnumPluginType)Enum.Parse(typeof(EnumPluginType), pluginType);
            bool needConfirm =false;
            foreach (var addr in Signer.Ins.addressPool.addresses)
            {
                if ((UInt16)addr.type == (UInt16)content)
                {
                    needConfirm = true;
                    break;
                }
            }
            if (needConfirm)
            {
                ND_MessageBoxResult nD_MessageBoxResult =await DialogueControl.ShowMessageBox("你有地址需要用到这个插件，是否确认卸载",ND_MessageBoxButton.OKCancel,99999);

                if (nD_MessageBoxResult == ND_MessageBoxResult.OK)
                {
                    Result result = await ManagerControl.Ins.ToDo(EnumControl.UninstallPlugin, content);
                    GetPackageInfo();
                }
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
