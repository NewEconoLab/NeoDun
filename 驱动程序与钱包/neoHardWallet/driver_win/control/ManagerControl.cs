using driver_win.helper;
using NeoDun;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace driver_win.control
{
    class ManagerControl
    {
        private Dictionary<EnumControl, IControl> dic = new Dictionary<EnumControl, IControl>();
        private IControl curControl;
        private EnumControl curEnumControl;
        private bool Lock = false;

        public static ManagerControl Ins
        {
            get
            {
                if (ins == null)
                    ins = new ManagerControl();
                return ins;
            }
        }

        private static ManagerControl ins;

        public ManagerControl()
        {
            dic.Add(EnumControl.GetAddress,new GetAddress());
            dic.Add(EnumControl.AddAddress,new AddAddress());
            dic.Add(EnumControl.DelAddress, new DelAddress());
            dic.Add(EnumControl.SetName, new SetName());
            dic.Add(EnumControl.GetPackage, new GetPackageInfo());
            dic.Add(EnumControl.SignData, new SignData());
            dic.Add(EnumControl.ApplyInstallFramework,new ApplyUpdateFramework());
            dic.Add(EnumControl.InstallFramework, new InstallFramework()); 
            dic.Add(EnumControl.InstallPlugin, new InstallPlugin());
            dic.Add(EnumControl.UninstallPlugin, new UninstallPlugin());
            dic.Add(EnumControl.SecurityChannel, new SecurityChannel());
        }

        public async Task<Result> ToDo(EnumControl enumControl,params object[] _params)
        {
            if (Lock && enumControl != EnumControl.InstallFramework)
            {
                Result result = new Result();
                return result;
            }
            Lock = true;
            try
            {
                Signer.Ins.eventHandler = null;
                Signer.Ins.eventHandler += Done;
                curEnumControl = enumControl;
                curControl = dic[enumControl];
                var result =await curControl.ToDo(_params);
                Lock = false;
                return result;
            }
            catch (Exception e)
            {
                throw new NotImplementedException(e.Message);
            }
        }


        public void Done(Object _enumControl, params object[] _params)
        {
            EnumControl enumControl = (EnumControl)_enumControl;
            if (enumControl == EnumControl.Common || enumControl == curEnumControl)
            {
                if (curControl != null)
                    curControl.Done(_params);
            }
        }

        public void Release()
        {
            if (curControl != null)
            {
                curControl.Release();
                ECDH.Ins.Release();
            }
        }
    }
}
