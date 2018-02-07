using Microsoft.Owin;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace hhgate
{
    public class StaticFileParser : CustomServer.IParser
    {
        public string pathOnDrive;
        public StaticFileParser(string pathOnDrive)
        {
            this.pathOnDrive = pathOnDrive;

        }
        public void HandleRequest(IOwinContext context, string rootpath, string relativePath)
        {

            var filepath = pathOnDrive + "\\" + relativePath;
            if (System.IO.File.Exists(filepath))
            {
                staticFile(context, filepath);
                return;
            }
            else
            {
                context.Response.StatusCode = 404;
                context.Response.ContentType = "text/plain";
                context.Response.Write("miss file.");
                return;
            }
        }
        private static void staticFile(IOwinContext context, string path)
        {
            var bts = System.IO.File.ReadAllBytes(path);
            context.Response.ContentLength = bts.Length;
            var extname = System.IO.Path.GetExtension(path).ToLower();
            switch (extname)
            {
                case ".html":
                case ".htm":
                    context.Response.ContentType = "text/html; charset=UTF-8";
                    break;
                case ".txt":
                case ".json":
                case ".glsl":
                    context.Response.ContentType = "text/plain; charset=UTF-8";
                    break;
                case ".png":
                    context.Response.ContentType = "image/png";
                    break;
                case ".jpg":
                case ".jpeg":
                    context.Response.ContentType = "image/jpeg";
                    break;
                default:
                    context.Response.ContentType = "application/octet-stream";
                    break;

            }
            context.Response.Write(bts);
            return;
        }

    }

}
