using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NeoDun
{
    public enum AddressType : UInt16
    {
        Neo = 0x0101,
    }
    public class Address
    {
        public string AddressText;
        public AddressType type;
        public byte[] privatekey;

        public override string ToString()
        {
            return type.ToString() + ":" + AddressText;
        }
        public byte[] GetAddbytes()
        {
            if (type == AddressType.Neo)
            {
                return SignTool.DecodeBase58(this.AddressText);
            }
            return null;
        }
    }
    public class AddressPool
    {
        public NeoDun.Address getAddress(AddressType type, string address)
        {
            foreach (var a in addresses)
            {
                if (a.type == type && a.AddressText == address)
                    return a;
            }
            return null;

        }
        /// <summary>
        /// 增加地址  如果增加成功返回true  已经存在返回false
        /// </summary>
        /// <param name="add"></param>
        /// <returns></returns>
        public bool AddAddress(NeoDun.Address add)
        {
            var addo = getAddress(add.type, add.AddressText);
            if (addo != null)
                return false;
            this.addresses.Add(add);
            this.Save();
            return true;
        }
        /// <summary>
        /// 删除地址 如果删除成功返回true 删除失败返回false
        /// </summary>
        public bool DelAddress(NeoDun.Address add)
        {
            var addo = getAddress(add.type, add.AddressText);
            if (addo == null)
                return false;
            ConcurrentBag<NeoDun.Address> _addresses = new ConcurrentBag<NeoDun.Address>();
            foreach (var _add in addresses)
            {
                if (_add != addo)
                {
                    _addresses.Add(_add);
                }
                else
                {
                }
            }
            addresses = _addresses;
            Save();
            return true;
        }
        public ConcurrentBag<NeoDun.Address> addresses = new ConcurrentBag<NeoDun.Address>();

        public void Save()
        {
            var str = "";
            foreach (var add in addresses)
            {
                str += add.type.ToString() + "||" + add.AddressText + "||" + NeoDun.SignTool.EncodeBase58(add.privatekey, 0, add.privatekey.Length) + "\n";
            }
            System.IO.File.WriteAllText("address.sim.save.txt", str);

        }
        public void Load()
        {
            if (System.IO.File.Exists("address.sim.save.txt"))
            {
                try
                {
                    var lines = System.IO.File.ReadAllText("address.sim.save.txt").Split('\n');
                    foreach (var line in lines)
                    {
                        var lp = line.Split(new string[] { "||" }, StringSplitOptions.None);
                        NeoDun.Address a = new Address();
                        a.type = (AddressType)Enum.Parse(typeof(AddressType), lp[0]);
                        a.AddressText = lp[1];
                        a.privatekey = SignTool.DecodeBase58(lp[2]);
                        this.addresses.Add(a);
                    }
                    Console.WriteLine("addresses isEmpty" + this.addresses.IsEmpty);
                }
                catch
                {

                }
            }
            else
            {
                Console.WriteLine("address.sim.save.txt is null");
            }
        }
    }

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
            return NEO.AllianceOfThinWallet.Cryptography.Base58.Encode(_bts);
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
                    json.TryGetValue("password",out js_pw);
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
                Console.WriteLine("personalInfo.sim.save.txt is null");
            }
        }
    }

}
