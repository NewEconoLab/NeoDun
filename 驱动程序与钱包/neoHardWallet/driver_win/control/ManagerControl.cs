using driver_win.helper;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace driver_win.control
{
    class ManagerControl
    {
        private Dictionary<EnumControl, IControl> dic = new Dictionary<EnumControl, IControl>();
        private IControl curControl;
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
        }

        public async Task<Result> ToDo(EnumControl enumControl, params object[] _params)
        {
            if (Lock)
            {
                Result result = new Result();
                result.errorCode = EnumError.Doing;
                return result;
            }
            Lock = true;
            try
            {
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

        public void Release()
        {
            if(curControl != null)
                curControl.Release();
        }
    }
}
