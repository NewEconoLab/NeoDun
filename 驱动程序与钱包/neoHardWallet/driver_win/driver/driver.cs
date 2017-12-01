using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
interface IDriver
{
    string Tag();
    void Check();
    int GetCount();
    void StartRead(Action<byte[]> onRead);
    void StopRead();
    bool IsActive();
    bool Send(byte[] data);
}
class Empty
{

}
class DriverS
{
    public static SIMDriver simdriver;
    public static void Init()
    {
        drivers = new System.Collections.Concurrent.ConcurrentBag<IDriver>();
        drivers.Add(new USBHIDDriver());
        simdriver = new SIMDriver();
        drivers.Add(simdriver);

        System.Threading.Thread t = new System.Threading.Thread(() =>
        {
            while (bexit == false)
            {
                System.Threading.Thread.Sleep(1000);
                _Check();
            }
        }
        );
        bexit = false;
        t.Start();
        _Check();

    }
    static void _Check()
    {
        foreach (var d in drivers)
        {
            d.Check();
        }
        lock (locktag)
        {
            if (last != null && last.IsActive() == false)
            {
                if (onDriverLost != null)
                    onDriverLost();
            }
            if (last == null || last.IsActive() == false)
            {
                last = getActive();
                if (onDriverGot != null)
                    onDriverGot();
            }
        }
    }
    public static void Stop()
    {
        bexit = true;
    }

    static bool bexit;
    static System.Collections.Concurrent.ConcurrentBag<IDriver> drivers = null;
    static IDriver last = null;
    static public Action onDriverLost;
    static public Action onDriverGot;

    static Empty locktag = new Empty();
    static public void StartRead(Action<byte[]> onRead)
    {
        foreach (var d in drivers)
        {
            IDriver driver = d;
            d.StartRead((data) =>
            {
                lock (locktag)
                {
                    if (driver == last)
                    {
                        onRead(data);
                    }
                }
            }
            );
        }
    }

    static public void StopRead()
    {
        foreach (var d in drivers)
        {
            d.StopRead();
        }
    }
    static public void Send(byte[] data)
    {
        lock (locktag)
        {
            if (last != null)
            {
                last.Send(data);
            }
        }
    }
    static public IDriver getActive()
    {
        foreach (var d in drivers)
        {
            if (d.IsActive())
                return d;
        }
        return null;
    }
}