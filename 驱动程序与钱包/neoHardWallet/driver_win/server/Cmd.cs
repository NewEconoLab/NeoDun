using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace hhgate
{
    class Cmd
    {

        public static string Call(string path, string text)
        {
            int _end = 0;
            string outstr = "";
            var begin = System.DateTime.Now;
            System.Threading.Thread t = new System.Threading.Thread(() =>
             {
                 var sinfo = new System.Diagnostics.ProcessStartInfo("cmd");
                 sinfo.WorkingDirectory = System.IO.Path.GetFullPath(path);
                 sinfo.RedirectStandardInput = true;
                 sinfo.RedirectStandardOutput = true;
                 sinfo.UseShellExecute = false;
                 sinfo.CreateNoWindow = true;

                 System.Diagnostics.Process p = new System.Diagnostics.Process();
                 p.StartInfo = sinfo;
                 p.Start();
                 //p.WaitForInputIdle(10);
                 p.StandardInput.Write("echo off\r\n");
                 p.StandardInput.Write(text);
                 p.StandardInput.Write("\r\n");
                 p.StandardInput.Write("exit\r\n");

                 var line1 = p.StandardOutput.ReadLine();
                 var line2 = p.StandardOutput.ReadLine();
                 var line3 = p.StandardOutput.ReadLine();
                 var line4 = p.StandardOutput.ReadLine();

                 outstr = p.StandardOutput.ReadToEnd();
                 _end = 1;
             });
            t.Start();

            while (_end == 0)
            {
                System.Threading.Thread.Sleep(100);
                // await System.Threading.Tasks.Task.Delay(100);
                if((System.DateTime.Now -begin).TotalSeconds>10.0)
                {
                    try
                    {
                        t.Abort();
                    }
                    catch
                    {
                    }
                    break;
                }
            }
            var lines = outstr.Split(new string[] { "\r\n" },StringSplitOptions.None);
            StringBuilder sb = new StringBuilder();
            for (var i = 0; i < lines.Length - 2; i++)
                sb.Append(lines[i]+"\r\n");
            return sb.ToString();

        }
    }
}
