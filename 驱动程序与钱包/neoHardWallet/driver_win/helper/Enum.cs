using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace driver_win.helper
{
    interface IControl
    {
        Task<Result> ToDo(params object[] _params);
        void Done(params object[] _params);
        void Release();
        Task<bool> SendMsg(params object[] _params);
        void HandleMsg(params object[] _params);
    }

    struct Result
    {
        public EnumError errorCode;
        public object data;
    }

    enum EnumInstallType
    {
        Framework = 0x0000,
        Plugin = 0x0001,
    }

    enum EnumPluginType
    {
        Unknow = 0x0000,
        Neo = 0x0101,
    }

    enum EnumControl
    {
        Common,
        GetAddress,
        AddAddress,
        DelAddress,
        SetName,
        GetPackage,
        ApplyInstallFramework,
        InstallFramework,
        SignData,
        InstallPlugin,
        UninstallPlugin,
        SecurityChannel,
    }


    enum EnumError
    {
        Doing = 0x0000,   //正在执行

        CommonSuc = 0x0001, //常规成功
        CommonFailed = 0x0002,//常规失败

        AddAddressSuc = 0x02a4,//增加地址成功
        DelAddressSuc = 0x02a3,//删除地址成功
        SetNameSuc = 0x02a2,//设置名字成功
        SignSuc = 0x02a5,//签名成功
        AgreeInstallFramework = 0x03a2,//同意更新固件
        InstallSuc = 0x03a1,//成功安裝固件或插件
        UninstallSuc = 0x03a3, //卸載插件成功
        SecurityChannelSuc = 0x04a1,//安全通道成功回复

        AddAddressFailed = 0x02e4,//增加地址失败
        DelAddressFailed = 0x02e3,//删除地址失败
        SetNameFailed = 0x02e2,//设置名字失败
        SignFailed = 0x02e5,//签名失败
        InstallFailed = 0x03e1,//安裝固件或插件失敗
        RefuseInstallFramework = 0x03e2,//拒絕更新固件
        UninstallFailed = 0x03e3,//卸載插件失敗

        IncorrectWif = 0x0205,//错误的wif
        DuplicateWif = 0x0203,//重复导入了wif

        LongName = 0x0601,//名字太长

    }

    //
    // 摘要:
    //     Specifies which message box button that a user clicks. System.Windows.MessageBoxResult
    //     is returned by the Overload:System.Windows.MessageBox.Show method.
    public enum ND_MessageBoxResult
    {
        //
        // 摘要:
        //     The message box returns no result.
        None = 0,
        //
        // 摘要:
        //     The result value of the message box is OK.
        OK = 1,
        //
        // 摘要:
        //     The result value of the message box is Cancel.
        Cancel = 2,
        //
        // 摘要:
        //     The result value of the message box is Yes.
        Yes = 6,
        //
        // 摘要:
        //     The result value of the message box is No.
        No = 7
    }

    //
    // 摘要:
    //     Specifies the buttons that are displayed on a message box. Used as an argument
    //     of the Overload:System.Windows.MessageBox.Show method.
    public enum ND_MessageBoxButton
    {
        //
        // 摘要:
        //     The message box displays an OK button.
        OK = 0,
        //
        // 摘要:
        //     The message box displays OK and Cancel buttons.
        OKCancel = 1,
        //
        // 摘要:
        //     The message box displays Yes, No, and Cancel buttons.
        None = 2

    }

    public enum EnumLanguage
    {
        cn = 1,
        en = 2
    }
}
