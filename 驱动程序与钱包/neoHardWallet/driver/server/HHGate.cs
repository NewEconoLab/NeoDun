using AntShares.Compiler;
using Microsoft.Owin;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace hhgate
{
    public class HarddriveHttpGateway : CustomServer.IParser
    {


        public async Task HandleRequest(IOwinContext context, string rootpath, string relativePath)
        {
            var api = relativePath.ToLower();
            var formdata = await FormData.FromRequest(context.Request);
            if (formdata == null)
            {
                context.Response.StatusCode = 500;
                context.Response.ContentType = "text/plain";
                MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                json["msg"] = new MyJson.JsonNode_ValueString("formdata format error.");
                json["tag"] = new MyJson.JsonNode_ValueNumber(-1);
                await context.Response.WriteAsync(json.ToString());
                return;
            }

            if (relativePath == "ver")
            {
                await ver(context, formdata);
                return;
            }
            else if (relativePath == "filemap_list")
            {
                await filemap_list(context, formdata);
                return;
            }
            else if (relativePath == "filemap_add")
            {
                await filemap_add(context, formdata);
                return;
            }
            else if (relativePath == "filemap_remove")
            {
                await filemap_remove(context, formdata);
                return;
            }
            else if (relativePath == "file_list")
            {
                await file_list(context, formdata);
                return;
            }
            else if (relativePath == "file_save")
            {
                await file_save(context, formdata);
                return;
            }
            else if (relativePath == "file_remove")
            {
                await file_remove(context, formdata);
                return;
            }
            else if (relativePath == "file_cmd")
            {
                await file_cmd(context, formdata);
                return;
            }
            else
            {
                MyJson.JsonNode_Object jsonr = new MyJson.JsonNode_Object();
                jsonr["tag"] = new MyJson.JsonNode_ValueNumber(-1000);
                jsonr["msg"] = new MyJson.JsonNode_ValueString("unknown cmd.");
                await context.Response.WriteAsync(jsonr.ToString());
            }
            return;

        }
        private static async Task ver(IOwinContext context, FormData formdata)
        {
            MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
            json["tag"] = new MyJson.JsonNode_ValueNumber(0);
            MyJson.JsonNode_Array maps = new MyJson.JsonNode_Array();
            json.SetDictValue("msg", "HardDrive Http Gate By GD3D 0.01");
            await context.Response.WriteAsync(json.ToString());
            return;
        }
        private static async Task filemap_list(IOwinContext context, FormData formdata)
        {
            MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
            json["tag"] = new MyJson.JsonNode_ValueNumber(0);
            MyJson.JsonNode_Array maps = new MyJson.JsonNode_Array();
            json["maps"] = maps;
            foreach (var k in CustomServer.mapParser)
            {
                if (k.Value is StaticFileParser)
                {
                    StaticFileParser pv = k.Value as StaticFileParser;
                    MyJson.JsonNode_Object item = new MyJson.JsonNode_Object();
                    item.SetDictValue("url", "/" + k.Key);
                    item.SetDictValue("pathOnDrive", pv.pathOnDrive);
                    maps.Add(item);
                }
            }
            await context.Response.WriteAsync(json.ToString());
            return;
        }

        private static async Task filemap_add(IOwinContext context, FormData formdata)
        {

            if (formdata.mapParams.ContainsKey("url") && formdata.mapParams.ContainsKey("pathondrive"))
            {
                var b = CustomServer.AddStaticFileMap(formdata.mapParams["url"], formdata.mapParams["pathondrive"]);
                if (b)
                {
                    MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                    json["tag"] = new MyJson.JsonNode_ValueNumber(0);
                    await context.Response.WriteAsync(json.ToString());
                    return;
                }
                else
                {
                    MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                    json["tag"] = new MyJson.JsonNode_ValueNumber(-2);
                    json["msg"] = new MyJson.JsonNode_ValueString("add filemap fail.");
                    await context.Response.WriteAsync(json.ToString());
                    return;
                }

            }
            else
            {
                MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                json["tag"] = new MyJson.JsonNode_ValueNumber(-1);
                json["msg"] = new MyJson.JsonNode_ValueString("need param: url & pathondrive.");
                await context.Response.WriteAsync(json.ToString());
                return;
            }

        }
        private static async Task filemap_remove(IOwinContext context, FormData formdata)
        {

            if (formdata.mapParams.ContainsKey("url"))
            {
                var b = CustomServer.RemoveStaticFileMap(formdata.mapParams["url"]);
                if (b)
                {
                    MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                    json["tag"] = new MyJson.JsonNode_ValueNumber(0);
                    await context.Response.WriteAsync(json.ToString());
                    return;
                }
                else
                {
                    MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                    json["tag"] = new MyJson.JsonNode_ValueNumber(-2);
                    json["msg"] = new MyJson.JsonNode_ValueString("remove filemap fail.");
                    await context.Response.WriteAsync(json.ToString());
                    return;
                }

            }
            else
            {
                MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                json["tag"] = new MyJson.JsonNode_ValueNumber(-1);
                json["msg"] = new MyJson.JsonNode_ValueString("need param: url.");
                await context.Response.WriteAsync(json.ToString());
                return;
            }

        }

        private static async Task file_list(IOwinContext context, FormData formdata)
        {

            if (formdata.mapParams.ContainsKey("url") && formdata.mapParams.ContainsKey("path"))
            {
                var p = CustomServer.GetStaticFileParser(formdata.mapParams["url"]);
                if (p != null)
                {
                    var dirs = System.IO.Directory.GetDirectories(p.pathOnDrive);
                    var files = System.IO.Directory.GetFiles(p.pathOnDrive);
                    MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                    json["tag"] = new MyJson.JsonNode_ValueNumber(0);
                    MyJson.JsonNode_Array _files = new MyJson.JsonNode_Array();
                    MyJson.JsonNode_Array _dirs = new MyJson.JsonNode_Array();

                    json["files"] = _files;
                    json["subpaths"] = _dirs;
                    foreach (var d in dirs)
                    {
                        MyJson.JsonNode_Object item = new MyJson.JsonNode_Object();
                        item.SetDictValue("name", d);
                        _dirs.Add(item);
                    }
                    foreach (var f in files)
                    {
                        MyJson.JsonNode_Object item = new MyJson.JsonNode_Object();
                        item.SetDictValue("name", f);
                        try
                        {
                            System.IO.FileInfo finfo = new System.IO.FileInfo(f);
                            item.SetDictValue("length", finfo.Length);
                        }
                        catch
                        {

                        }
                        _files.Add(item);
                    }
                    await context.Response.WriteAsync(json.ToString());
                    return;

                }
                else
                {
                    MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                    json["tag"] = new MyJson.JsonNode_ValueNumber(-2);
                    json["msg"] = new MyJson.JsonNode_ValueString("file_list fail.");
                    await context.Response.WriteAsync(json.ToString());
                    return;
                }

            }
            else
            {
                MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                json["tag"] = new MyJson.JsonNode_ValueNumber(-1);
                json["msg"] = new MyJson.JsonNode_ValueString("need param: url & path.");
                await context.Response.WriteAsync(json.ToString());
                return;
            }

        }

        private static async Task file_save(IOwinContext context, FormData formdata)
        {

            if (formdata.mapParams.ContainsKey("url") && formdata.mapParams.ContainsKey("path") && formdata.mapFiles.ContainsKey("file"))
            {
                try
                {
                    var p = CustomServer.GetStaticFileParser(formdata.mapParams["url"]);
                    if (p != null)
                    {
                        var path = System.IO.Path.Combine(p.pathOnDrive, formdata.mapParams["path"]);
                        if (System.IO.File.Exists(path))
                        {
                            System.IO.File.Delete(path);
                        }
                        var pathpath = System.IO.Path.GetDirectoryName(path);
                        System.IO.Directory.CreateDirectory(pathpath);

                        var file = formdata.mapFiles["file"];
                        System.IO.File.WriteAllBytes(path, file);

                        MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                        json["tag"] = new MyJson.JsonNode_ValueNumber(0);
                        await context.Response.WriteAsync(json.ToString());
                        return;
                    }
                }
                catch
                {

                }

                {
                    MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                    json["tag"] = new MyJson.JsonNode_ValueNumber(-2);
                    json["msg"] = new MyJson.JsonNode_ValueString("file_save fail.");
                    await context.Response.WriteAsync(json.ToString());
                    return;
                }
            }
            else
            {
                MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                json["tag"] = new MyJson.JsonNode_ValueNumber(-1);
                json["msg"] = new MyJson.JsonNode_ValueString("need param: url & path & file.");
                await context.Response.WriteAsync(json.ToString());
                return;
            }

        }
        private static async Task file_remove(IOwinContext context, FormData formdata)
        {

            if (formdata.mapParams.ContainsKey("url") && formdata.mapParams.ContainsKey("path"))
            {
                try
                {
                    var p = CustomServer.GetStaticFileParser(formdata.mapParams["url"]);
                    if (p != null)
                    {
                        var path = System.IO.Path.Combine(p.pathOnDrive, formdata.mapParams["path"]);
                        if (System.IO.File.Exists(path))
                        {
                            System.IO.File.Delete(path);
                        }
                        MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                        json["tag"] = new MyJson.JsonNode_ValueNumber(0);
                        await context.Response.WriteAsync(json.ToString());
                        return;
                    }
                }
                catch
                {

                }

                {
                    MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                    json["tag"] = new MyJson.JsonNode_ValueNumber(-2);
                    json["msg"] = new MyJson.JsonNode_ValueString("file_remove fail.");
                    await context.Response.WriteAsync(json.ToString());
                    return;
                }
            }
            else
            {
                MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                json["tag"] = new MyJson.JsonNode_ValueNumber(-1);
                json["msg"] = new MyJson.JsonNode_ValueString("need param: url & path & file.");
                await context.Response.WriteAsync(json.ToString());
                return;
            }

        }

        private static async Task file_cmd(IOwinContext context, FormData formdata)
        {

            if (formdata.mapParams.ContainsKey("url") && formdata.mapParams.ContainsKey("path") && formdata.mapParams.ContainsKey("cmd"))
            {
                try
                {
                    var p = CustomServer.GetStaticFileParser(formdata.mapParams["url"]);
                    if (p != null)
                    {
                        var path = System.IO.Path.Combine(p.pathOnDrive, formdata.mapParams["path"]);
                        var cmd = formdata.mapParams["cmd"];

                        var text = await Cmd.Call(path, cmd);
                        await context.Response.WriteAsync(text );
                        return;
                    }
                }
                catch(Exception err)
                {

                }

                {
                    MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                    json["tag"] = new MyJson.JsonNode_ValueNumber(-2);
                    json["msg"] = new MyJson.JsonNode_ValueString("file_cmd fail.");
                    await context.Response.WriteAsync(json.ToString());
                    return;
                }
            }
            else
            {
                MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                json["tag"] = new MyJson.JsonNode_ValueNumber(-1);
                json["msg"] = new MyJson.JsonNode_ValueString("need param: url & path & cmd.");
                await context.Response.WriteAsync(json.ToString());
                return;
            }

        }

    }

}
