using driver_win.helper;
using NeoDun;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Threading;

namespace driver_win.control
{
    abstract class Control : IControl
    {
        protected Result result = new Result();

        protected Signer signer = Signer.Ins;

        protected bool needRestart = false;

        protected bool wait;

        public async void Done(params object[] _params)
        {
            HandleMsg(_params);
            wait = false;
        }

        public void Release()
        {
            wait = false;
        }

        public abstract void Init();

        public async Task<Result> ToDo(params object[] _params)
        {
            Init();
            await dialogs.DialogueControl.ShowMessageBox(Mgr_Language.Ins.Code2Word(result.msgCode, result.errorCode), ND_MessageBoxButton.None);
            if (!await SendMsg(_params))
                return result;

            wait = true;

            while (wait)
            {
                await Task.Delay(100);
            }
            if (needRestart&& result.errorCode!= EnumErrorCode.NoError)
            {
                var r =await dialogs.DialogueControl.ShowMessageBox(Mgr_Language.Ins.Code2Word(result.msgCode, result.errorCode), ND_MessageBoxButton.RestartCancel,999);
                if (r == ND_MessageBoxResult.Restart)
                    return await this.ToDo(_params);
            }
            else
            {
                await dialogs.DialogueControl.ShowMessageBox(Mgr_Language.Ins.Code2Word(result.msgCode, result.errorCode), ND_MessageBoxButton.None, 3);
            }
            return result;
        }

        public abstract Task<bool> SendMsg(params object[] _params);
        public abstract void HandleMsg(params object[] _params);
    }
}
