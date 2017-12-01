using Microsoft.Owin;
using Microsoft.Owin.Hosting;
using Owin;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace hhgate
{
    class CustomServer
    {
        public interface IParser
        {
            Task HandleRequest(IOwinContext context, string rootpath, string relativePath);
        }


        static IDisposable handleServer;
        public static void BeginServer()
        {
            var url = "http://127.0.0.1:50288/";
            var startOpts = new StartOptions(url)
            {

            };
            mapParser.Clear();
            handleServer = WebApp.Start<CustomServer>(startOpts);
            AddStaticFileMap("www", "www");
            AddCustomFileParser("_api", new SignMachine());
            //AddCustomFileParser("_api", new HarddriveHttpGateway());
            Console.WriteLine("open server on:" + url);
        }
        public static void CloseServer()
        {
            handleServer.Dispose();
        }
        public static ConcurrentDictionary<string, IParser> mapParser = new ConcurrentDictionary<string, IParser>();
        public static bool AddStaticFileMap(string pathOnUrl, string pathOnDrive)
        {
            var key = pathOnUrl.ToLower();
            if (mapParser.ContainsKey(key))
                return false;


            mapParser[pathOnUrl.ToLower()] = new StaticFileParser(pathOnDrive);
            return true;
        }
        public static StaticFileParser GetStaticFileParser(string pathOnUrl)
        {
            var key = pathOnUrl.ToLower();
            if ((mapParser[key] is StaticFileParser) == false)
                return null;

            return mapParser[key] as StaticFileParser;
        }
        public static bool RemoveStaticFileMap(string pathOnUrl)
        {
            var key = pathOnUrl.ToLower();
            if (key == "www" || key == "_api")
                return false;
            if (mapParser.ContainsKey(key) == false)
                return false;
            if ((mapParser[key] is StaticFileParser) == false)
                return false;

            IParser p = null;
            mapParser.TryRemove(key, out p);
            return true;
        }

        public static void AddCustomFileParser(string pathOUrl, IParser parser)
        {
            mapParser[pathOUrl.ToLower()] = parser;
        }
        public void Configuration(IAppBuilder appBuilder)
        {
            appBuilder.Run(CustomServer.HandleRequest);
        }
        static async Task HandleRequest(IOwinContext context)
        {
            context.Response.Headers["Access-Control-Allow-Origin"] = "*";
            context.Response.ContentType = "text/plain; charset=UTF-8";

            var info = context.Request.QueryString;


            string path = context.Request.Path.Value;

            string spath = path.Substring(1);
            var i = spath.IndexOf("/");
            if (i > 0)//根请求
            {
                var rootpath = spath.Substring(0, i).ToLower();
                var relativePath = spath.Substring(i + 1);
                if (mapParser.ContainsKey(rootpath))
                {
                    await mapParser[rootpath].HandleRequest(context, rootpath, relativePath);
                    return;
                }
                else
                {
                    context.Response.StatusCode = 404;
                    await context.Response.WriteAsync("私有服务器,请询问管理员使用方法.");
                    return;
                }
            }
            else
            {
                context.Response.StatusCode = 404;
                await context.Response.WriteAsync("私有服务器,请询问管理员使用方法.");
                return;
            }
        }

    }


}
