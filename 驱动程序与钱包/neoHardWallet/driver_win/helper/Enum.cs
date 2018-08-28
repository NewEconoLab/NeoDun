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
        public EnumMsgCode msgCode;
        public EnumErrorCode errorCode;
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

    enum EnumErrorCode
    {
        Unknow = 0xffff,//未知
        NoError = 0x0000,//没有错误
        SecurityChannelFailed = 0x0100, //安装文件未通过安全验证
        InsufficienStorage = 0x0101,//设备存储空间不足
        PluginIsUsing = 0x0102,//app正在使用中
        IncorrectPluginType = 0x0103,//app类型错误
        IncorrectHash = 0x0104,//hash不对  数据异常
        WifQuantityLimit = 0x0201,//私钥数量已达上限
        DuplicateWif = 0x0202,//该私钥已经导入
        AddressNameExist = 0x0203,//地址名称重复
        PluginInexistence = 0x0204,//未安装该币种所需插件
        IncorrectWif = 0x0205,//私钥格式错误
        AddressInexistence = 0x0206,//地址不存在
        AddressNameTooLong = 0x0207,//地址名称太长
        WifInexistence = 0x0301,//未导入所需私钥
        NoMultiPartySign = 0x0302,//不支持多方签名
        IncorrectSignData = 0x0303,//签名数据包错误
        Refuse = 0x0401,//用户拒绝
    }

    enum EnumMsgCode
    {
        Unknow = 0x0000,//未知
        PrepareTransmitData = 0x0100,//准备传输数据
        DataTransmission = 0x01a2,//数据传输中
        DataTooLarge = 0x01e0,//数据过大
        DataIsWrong = 0x00e0,//报文出错
        HashIsWrong = 0x0112,//hash出错
        Installing = 0x0301,//正在进行安装
        InstallSuc = 0x03a1,//安装成功
        InstallFailed = 0x03e1,//安装失败
        TryUpdate = 0x0302,//申请升级
        AgreeUpdate = 0x03a2,//同意升级
        RefuseUpdate = 0x03e2,//拒绝升级
        Uninstalling = 0x0303,//卸载中
        UninstallSuc = 0x03a3,//卸载成功
        UninstallFailed = 0x03e3,//卸载失败
        GettingMessage = 0x0304,//正在获取neodun信息
        GetMessageSuc=0x03a4,//获取信息成功
        GettingAddress = 0x0201,//正在查询钱包地址
        GetAddressSuc=0x02a1,//查询完成
        BuildingSecurityChannel = 0x0401,//正在建立安全通道
        BuildSecurityChannelSuc = 0x04a1,//建立成功
        SettingName=0x0202,//正在设置地址名称
        SetNameSuc=0x02a2,//设置地址名称成功
        SetNameFailed=0x02e2,//设置地址名称失败
        DeletingName = 0x0203,//删除地址中
        DeleteNameSuc = 0x02a3,//删除地址成功
        DeleteNameFailed = 0x02e2,//删除地址失败
        AddingAddress = 0x0204,//增加地址中
        AddAddressSuc = 0x02a4,//导入钱包成功
        AddAddressFailed = 0x02e4,//导入钱包失败
        Signing = 0x0205,//签名中
        SignSuc = 0x02a5,//签名成功
        SignFailed = 0x02e5,//签名失败
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
