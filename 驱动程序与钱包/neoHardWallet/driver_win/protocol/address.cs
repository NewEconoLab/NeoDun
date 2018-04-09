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

}
