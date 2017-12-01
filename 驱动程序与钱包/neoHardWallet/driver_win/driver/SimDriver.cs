using NeoDun;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Text;



class SIMDriver : IDriver
{
    public string Tag()
    {
        return "sim";
    }
    Action<byte[]> _onRead;
    public void StartRead(Action<byte[]> onRead)
    {
        _onRead = onRead;
    }
    public void StopRead()
    {
        _onRead = null;
    }
    public bool bActive = false;
    public bool IsActive()
    {
        return bActive;
    }
    public void Check()
    {

    }
    public int GetCount()
    {
        return bActive ? 1 : 0;
    }
    public bool Send(byte[] bs)
    {
        if (!bActive)
            return false;
        byte[] data = new byte[64];
        Array.Copy(bs, data, Math.Min(bs.Length, 64));
        NeoDun.Message msg = new NeoDun.Message();
        msg.FromData(data);
        NeoDun.Message srcmsg = null;
        if (msg.tag1 == 0x00 || msg.tag2 >= 0xa0)//如果是一条回复消息，找原始消息
        {
            srcmsg = this.needBackMessage[msg.msgid];
        }
        if (_msg != null)
            _msg(msg, srcmsg);
        return true;
    }


    public ConcurrentDictionary<UInt16, NeoDun.Message> needBackMessage = new ConcurrentDictionary<ushort, NeoDun.Message>();
    public delegate void deleSignerRecv(NeoDun.Message recv, NeoDun.Message src);

    deleSignerRecv _msg;
    public void __SignerRecv(deleSignerRecv msg)
    {
        _msg = msg;
    }
    public void __SignerSend(NeoDun.Message msg, bool needBack)
    {
        if (needBack)
        {
            needBackMessage[msg.msgid] = msg;
        }
        byte[] data = new byte[64];
        msg.ToData(data);
        _onRead(data);
    }
}