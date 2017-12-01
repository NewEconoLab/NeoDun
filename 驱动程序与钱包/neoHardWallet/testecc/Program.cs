using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NEO.AllianceOfThinWallet.Cryptography;
namespace testecc
{
    class Program
    {
        public static string Bytes2HexString(byte[] data)
        {
            StringBuilder sb = new StringBuilder();
            foreach (var d in data)
            {
                sb.Append(d.ToString("x02"));
            }
            return sb.ToString();
        }
        public static byte[] HexString2Bytes(string str)
        {
            byte[] outd = new byte[str.Length / 2];
            for (var i = 0; i < str.Length / 2; i++)
            {
                outd[i] = byte.Parse(str.Substring(i * 2, 2), System.Globalization.NumberStyles.HexNumber);
            }
            return outd;
        }
        static System.Security.Cryptography.SHA256 sha256 = System.Security.Cryptography.SHA256.Create();
        static void Main(string[] args)
        {
            //neo 私钥
            string neoprivate = "f97b7434ffa981eb809ea6795e0cb3046850a65be2f9f67b92053f6f2838adca";
            Console.WriteLine("Neo 私钥=" + neoprivate);
            Console.WriteLine("    Neo私钥是预先生成的");

            //neo公钥
            byte[] prikey = HexString2Bytes(neoprivate);
            var PublicKey = NEO.AllianceOfThinWallet.Cryptography.ECC.ECCurve.Secp256r1.G * prikey;
            byte[] pubkey = PublicKey.EncodePoint(false).Skip(1).ToArray();
            Console.WriteLine("Neo 公钥=" + Bytes2HexString(pubkey));
            Console.WriteLine("    Neo公钥是用Neo私钥计算出来的");

            //签名的私钥
            byte[] first = { 0x45, 0x43, 0x53, 0x32, 0x20, 0x00, 0x00, 0x00 };
            byte[] signprikey = first.Concat(pubkey).Concat(prikey).ToArray();
            Console.WriteLine("签名算法用的私钥=" + Bytes2HexString(signprikey));
            Console.WriteLine("    签名算法用的私钥 是 用 neo 私钥 和 heo公钥 组合出来的");


            Console.WriteLine("");

            //随机数据
            Random r = new Random();
            byte[] srcdata = new byte[65536];
            for (var i = 0; i < srcdata.Length; i++)
            {
                srcdata[i] = (byte)(r.Next() % 256);
            }
            Console.WriteLine("填充64k数据用于签名");

            //签名
            byte[] signdata = null;

            using (System.Security.Cryptography.CngKey key = System.Security.Cryptography.CngKey.Import(signprikey, System.Security.Cryptography.CngKeyBlobFormat.EccPrivateBlob))
            {
                using (System.Security.Cryptography.ECDsaCng ecdsa = new System.Security.Cryptography.ECDsaCng(key))
                {
                    var hashsrc = sha256.ComputeHash(srcdata);
                    signdata = ecdsa.SignHash(hashsrc);
                }
            }
            Console.WriteLine("签名结果:" + Bytes2HexString(signdata));



            Console.WriteLine("");
            byte[] first2 = { 0x45, 0x43, 0x53, 0x31, 0x20, 0x00, 0x00, 0x00 };
            byte[] signpubkey = first2.Concat(pubkey).ToArray();
            Console.WriteLine("验证签名公钥=" + Bytes2HexString(signpubkey));
            Console.WriteLine("    验证签名公钥是用NEO公钥计算出来的");
            using (System.Security.Cryptography.CngKey key = System.Security.Cryptography.CngKey.Import(signpubkey, System.Security.Cryptography.CngKeyBlobFormat.EccPublicBlob))
            {
                using (System.Security.Cryptography.ECDsaCng ecdsa = new System.Security.Cryptography.ECDsaCng(key))
                {
                    Console.WriteLine("验证签名结果=" + ecdsa.VerifyData(srcdata, signdata));

                    var hashsrc = sha256.ComputeHash(srcdata);
                    Console.WriteLine("验证签名结果2=" + ecdsa.VerifyHash(hashsrc, signdata));
                }
            }
            Console.WriteLine("回车继续");
            Console.ReadLine();
        }
    }
}
