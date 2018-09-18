using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace driver_win.helper
{
    class Mgr_Language
    {
        private static Mgr_Language ins;
        public static Mgr_Language Ins
        {
            get
            {
                if (ins == null)
                    ins = new Mgr_Language();
                return ins;
            }
        }

        public Mgr_Language()
        {
            Init();
        }


        private Dictionary<string, string> dic_cn = new Dictionary<string, string>();
        private Dictionary<string, string> dic_en = new Dictionary<string, string>();
        public EnumLanguage curEnumLanguage;
        private Dictionary<string, string> curDic;

        private void Init()
        {
            //读取语言配置包
            if (System.IO.File.Exists("language.txt"))
            {
                var lines = System.IO.File.ReadAllLines("language.txt");
                foreach (var line in lines)
                {
                    var strs = line.Split('\t');
                    var code = strs[0];
                    var word_cn = strs[2];
                    var word_en = strs[1];
                    dic_cn.Add(code, word_cn);
                    dic_en.Add(code, word_en);
                }
            }

            if (System.Threading.Thread.CurrentThread.CurrentCulture.Name == "zh-CN")
            {
                curDic = dic_cn;
            }
            else
            {
                curDic = dic_en;
            }

            /*
            //读取配置获取语言环境
            if (System.IO.File.Exists("appsetting.txt"))
            {
                var txt = System.IO.File.ReadAllText("appsetting.txt");
                var jsonConfig = MyJson.Parse(txt) as MyJson.JsonNode_Object;
                curEnumLanguage = (EnumLanguage)jsonConfig["language"].AsInt();
                switch (curEnumLanguage)
                {
                    case EnumLanguage.cn:
                        curDic = dic_cn;
                        break;
                    case EnumLanguage.en:
                        curDic = dic_en;
                        break;
                }

            }
            */

        }

        public void ChangeLanguage(EnumLanguage enumLanguage)
        {
            MyJson.JsonNode_Object jsonConfig = new MyJson.JsonNode_Object();
            //读取语言配置包
            if (System.IO.File.Exists("appsetting.txt"))
            {
                var txt = System.IO.File.ReadAllText("appsetting.txt");
                jsonConfig = MyJson.Parse(txt) as MyJson.JsonNode_Object;
                jsonConfig["language"] = new MyJson.JsonNode_ValueNumber((uint)enumLanguage);
            }
            System.IO.File.WriteAllText("appsetting.txt",jsonConfig.ToString());
        }


        public string Code2Word(string code)
        {
            if (!code.Contains("$"))
                return code;
            if (curDic.ContainsKey(code))
                return curDic[code];
            return "";
        }

        public string Code2Word(EnumMsgCode enumMsgCode, EnumErrorCode enumErrorCode)
        {
            return  Msg2Word(enumMsgCode) +Error2Word(enumErrorCode);
        }


        private string Error2Word(EnumErrorCode enumErrorCode)
        {
            string errorCode = "$error_"+((uint)enumErrorCode).ToString("x4");
            if (curDic.ContainsKey(errorCode))
                return curDic[errorCode];
            return "";
        }

        private string Msg2Word(EnumMsgCode enumMsgCode)
        {
            string msgCode = "$msg_" + ((uint)enumMsgCode).ToString("x4");
            if (curDic.ContainsKey(msgCode))
                return curDic[msgCode];
            return "";
        }
    }
}
