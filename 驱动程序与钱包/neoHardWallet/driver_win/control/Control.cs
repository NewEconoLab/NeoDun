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

        public virtual void Done(params object[] _params)
        {
            wait = false;
        }

        public void Release()
        {
            wait = false;
        }

        public void Init()
        {
            signer.eventHandler = null;
            signer.eventHandler += Done;
            result.data = null;
            result.errorCode = EnumError.CommonSuc;
        }

        public async Task<Result> ToDo(params object[] _params)
        {
            Init();

            sendMsg(_params);

            wait = true;

            while (wait)
            {
                await Task.Delay(100);
            }

            return result;
        }

        public abstract void sendMsg(params object[] _params);
    }
}
