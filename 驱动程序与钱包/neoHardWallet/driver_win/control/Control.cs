using driver_win.helper;
using NeoDun;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace driver_win.control
{
    abstract class Control : IControl
    {
        protected Result result = new Result();

        protected Signer signer = Signer.Ins;

        protected bool wait;

        public void Done(params object[] _params)
        {
            HandleMsg(_params);
            wait = false;
        }

        public void Release()
        {
            wait = false;
        }

        public void Init()
        {
            result.data = null;
            result.errorCode = EnumError.CommonFailed;
        }

        public async Task<Result> ToDo(params object[] _params)
        {
            Init();

            if (!await SendMsg(_params))
                return result;

            wait = true;

            while (wait)
            {
                await Task.Delay(100);
            }

            return result;
        }

        public abstract Task<bool> SendMsg(params object[] _params);
        public abstract void HandleMsg(params object[] _params);
    }
}
