using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

class USBHIDDriver
{
    const UInt16 vid = 0x0483;
    const UInt16 pid = 0x572b;
    static HidSharp.HidDeviceLoader loader = new HidSharp.HidDeviceLoader();


    static HidSharp.HidStream stream;
    static HidSharp.HidDevice atcdevice;
    
    public static int writeErrorCount;
    public static int readErrorCount;

    static Action<byte[]> _onRead;

    public static void StartRead(Action<byte[]> onRead)
    {
        _onRead = onRead;
        System.Threading.Thread t = new System.Threading.Thread(() =>
        {
            while (_onRead != null)
            {
                if (stream != null)
                {
                    try
                    {
                        var buf = stream.Read();
                        _onRead(buf);
                    }
                    catch (System.IO.IOException err)//io异常，断开
                    {
                        atcdevice = null;
                        stream = null;
                    }
                    catch (TimeoutException err)//超时异常，记录
                    {
                        readErrorCount++;
                    }
                }
            }

        });
        t.Start();
    }
    public static void StopRead()
    {
        _onRead = null;
    }

    public static bool IsActive()
    {
        return stream != null;
    }
    public static int CheckDevice()
    {
        var devices = loader.GetDevices(0x0483, 0x572b);
        var signerCount = devices.Count();
        if (devices.Count() == 0)//直接移除设备
        {
            atcdevice = null;
            stream = null;
            return 0;
        }
        if (atcdevice == null)//打开设备
        {
            atcdevice = devices.First();
            stream = atcdevice.Open();
            return signerCount;
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
        return signerCount;
    }
    public static bool Send(byte[] bs)
    {
        try
        {
            byte[] outb = new byte[atcdevice.MaxOutputReportLength];
            outb[0] = 0;//fix hiddriver
            Array.Copy(bs, 0, outb, 1, Math.Min(bs.Length, outb.Length - 1));
            stream.Write(bs);
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
    public static bool Send(string str)
    {
        byte[] outb = new byte[atcdevice.MaxOutputReportLength];
        outb[0] = 0;
        var bs = System.Text.Encoding.UTF8.GetBytes(str);
        Array.Copy(bs, 0, outb, 1, Math.Min(bs.Length, outb.Length-1));
        return Send(outb);
    }

}