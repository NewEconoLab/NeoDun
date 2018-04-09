using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace driver_win
{

    public class PersonalInfo
    {
        public string username;
        public string password;
        public string[] setting;

        public byte[] GetBytes(string _str)
        {
            return NEO.AllianceOfThinWallet.Cryptography.Base58.Decode(_str);
        }

        public string GetString(byte[] _bts)
        {
            return NeoDun.SignTool.Bytes2HexString(_bts, 0, _bts.Length);
            //return NEO.AllianceOfThinWallet.Cryptography.Base58.Encode(_bts);
        }

        public void SetUsername(string _username)
        {
            username = _username;
            Save();
        }

        public void SetPassword(string _password)
        {
            setting[0] = "0";
            password = _password;
            Save();
        }
        public void SetPassword(byte[] _password)
        {
            password = GetString(_password);
            Save();
        }

        public void SetSetting(string[] _setting)
        {
            setting = _setting;
            Save();
        }


        public bool ConfirmPassword(string _password)
        {
            return _password == password;
        }

        public void Save()
        {
            MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
            json["username"] = new MyJson.JsonNode_ValueString(username);
            json["password"] = new MyJson.JsonNode_ValueString(password);

            string save_setting = "";
            for (int i = 0; i < setting.Length; i++)
            {
                if (i != setting.Length - 1)
                {
                    save_setting += setting[i] + ",";
                }
                else
                {
                    save_setting += setting[i];
                }
            }
            json["setting"] = new MyJson.JsonNode_ValueString(save_setting);
            System.IO.File.WriteAllText("personalInfo.sim.save.txt", json.ToString());
        }

        public void Load()
        {
            if (System.IO.File.Exists("personalInfo.sim.save.txt"))
            {
                try
                {
                    string text = System.IO.File.ReadAllText("personalInfo.sim.save.txt");
                    MyJson.JsonNode_Object json = (MyJson.JsonNode_Object)MyJson.Parse(text);
                    MyJson.IJsonNode js_pw = new MyJson.JsonNode_ValueString();
                    MyJson.IJsonNode js_setting = new MyJson.JsonNode_ValueString();
                    json.TryGetValue("password", out js_pw);
                    password = js_pw.ToString();
                    json.TryGetValue("setting", out js_setting);
                    setting = js_setting.ToString().Split(',');
                }
                catch
                {

                }
            }
            else
            {
                MyJson.JsonNode_Object json = new MyJson.JsonNode_Object();
                json["username"] = new MyJson.JsonNode_ValueString("");
                json["password"] = new MyJson.JsonNode_ValueString("");
                json["setting"] = new MyJson.JsonNode_ValueString("1, 0, 0, 1, 1, 1, 0");
                System.IO.File.WriteAllText("personalInfo.sim.save.txt", json.ToString());
                Load();
            }
        }
    }
}