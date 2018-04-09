using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


class BootloaderHidDriver : IDriver
{
    public string Tag()
    {
        return "bootloader";
    }

    const UInt16 vid = 1155;
    const UInt16 pid = 22352;
    static HidSharp.HidDeviceLoader loader = new HidSharp.HidDeviceLoader();


    HidSharp.HidStream stream;
    HidSharp.HidDevice atcdevice;

    public int writeErrorCount;
    public int readErrorCount;

    Action<byte[]> _onRead;

    public void StartRead(Action<byte[]> onRead)
    {
        //用異步接口，不用開綫程
        byte[] buf = new byte[65];
        _onRead = onRead;
        System.Threading.Thread t = new System.Threading.Thread(() =>
        {
            while (_onRead != null)
            {


                System.Threading.Thread.Sleep(5);
                if (stream != null)
                {
                    //try
                    {
                        int read = 0;
                        Action readlast = null;
                        readlast = () =>
                        {
                            stream.BeginRead(buf, read, 65 - read, (a) =>
                            {

                                if (_onRead == null || stream == null)
                                    return;
                                int c = 0;
                                try
                                {
                                    c = stream.EndRead(a);
                                }
                                catch (System.IO.IOException err)//io异常，断开
                                {
                                    atcdevice = null;
                                    stream = null;
                                    return;
                                }
                                catch (TimeoutException err)//超时异常，记录
                                {
                                    readErrorCount++;
                                }
                                read += c;
                                if (read < 65)
                                {
                                    readlast();
                                }
                                else
                                {

                                    _onRead(buf.Skip(1).ToArray());
                                    read = 0;

                                    readlast();
                                }
                            }, null);
                            while (true)
                            {
                                if (stream == null)
                                    break;
                                System.Threading.Thread.Sleep(5);
                            }
                        };


                        readlast();

                        //var buf = stream.Read();
                        //_onRead(buf);
                    }
                    //catch (System.IO.IOException err)//io异常，断开
                    //{
                    //    atcdevice = null;
                    //    stream = null;
                    //}
                    //catch (TimeoutException err)//超时异常，记录
                    //{
                    //    readErrorCount++;
                    //}
                }
            }

        });
        t.Start();
    }
    public void StopRead()
    {
        _onRead = null;
    }

    public bool IsActive()
    {
        return stream != null;
    }
    int signerCount;
    public void Check()
    {
        var devices = loader.GetDevices(vid, pid);
        signerCount = devices.Count();
        if (devices.Count() == 0)//直接移除设备
        {
            atcdevice = null;
            stream = null;
            return;
        }
        if (atcdevice == null)//打开设备
        {
            atcdevice = devices.First();
            stream = atcdevice.Open();
            return;
        }
        bool bFind = false;
        foreach (var d in devices)
        {
            if (d.DevicePath == atcdevice.DevicePath)
            {//same
                bFind = true;

            }
        }
        //找不到设备了
        if (!bFind)
        {
            atcdevice = null;
            stream = null;
        }
    }
    public int GetCount()
    {
        return signerCount;
    }
    public bool Send(byte[] bs)
    {
        if (stream == null)
            return false;
        try
        {
            byte[] outb = new byte[atcdevice.MaxOutputReportLength];
            outb[0] = 0;//fix hiddriver
            Array.Copy(bs, 0, outb, 1, Math.Min(bs.Length, outb.Length - 1));
            stream.Write(outb);
            return true;
        }
        catch (System.IO.IOException err)//io异常，断开
        {
            atcdevice = null;
            stream = null;
            return false;
        }
        catch (TimeoutException err)//超时异常，记录
        {
            writeErrorCount++;
            return false;
        }
    }
}