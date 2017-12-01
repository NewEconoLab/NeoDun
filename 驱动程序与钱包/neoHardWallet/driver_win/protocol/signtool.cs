using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace NeoDun
{
    public class SignTool
    {
        static UInt32[] Crc32Table = {
0x00000000,0x77073096,0xEE0E612C,0x990951BA,
0x076DC419,0x706AF48F,0xE963A535,0x9E6495A3,
0x0EDB8832,0x79DCB8A4,0xE0D5E91E,0x97D2D988,
0x09B64C2B,0x7EB17CBD,0xE7B82D07,0x90BF1D91,
0x1DB71064,0x6AB020F2,0xF3B97148,0x84BE41DE,
0x1ADAD47D,0x6DDDE4EB,0xF4D4B551,0x83D385C7,
0x136C9856,0x646BA8C0,0xFD62F97A,0x8A65C9EC,
0x14015C4F,0x63066CD9,0xFA0F3D63,0x8D080DF5,
0x3B6E20C8,0x4C69105E,0xD56041E4,0xA2677172,
0x3C03E4D1,0x4B04D447,0xD20D85FD,0xA50AB56B,
0x35B5A8FA,0x42B2986C,0xDBBBC9D6,0xACBCF940,
0x32D86CE3,0x45DF5C75,0xDCD60DCF,0xABD13D59,
0x26D930AC,0x51DE003A,0xC8D75180,0xBFD06116,
0x21B4F4B5,0x56B3C423,0xCFBA9599,0xB8BDA50F,
0x2802B89E,0x5F058808,0xC60CD9B2,0xB10BE924,
0x2F6F7C87,0x58684C11,0xC1611DAB,0xB6662D3D,
0x76DC4190,0x01DB7106,0x98D220BC,0xEFD5102A,
0x71B18589,0x06B6B51F,0x9FBFE4A5,0xE8B8D433,
0x7807C9A2,0x0F00F934,0x9609A88E,0xE10E9818,
0x7F6A0DBB,0x086D3D2D,0x91646C97,0xE6635C01,
0x6B6B51F4,0x1C6C6162,0x856530D8,0xF262004E,
0x6C0695ED,0x1B01A57B,0x8208F4C1,0xF50FC457,
0x65B0D9C6,0x12B7E950,0x8BBEB8EA,0xFCB9887C,
0x62DD1DDF,0x15DA2D49,0x8CD37CF3,0xFBD44C65,
0x4DB26158,0x3AB551CE,0xA3BC0074,0xD4BB30E2,
0x4ADFA541,0x3DD895D7,0xA4D1C46D,0xD3D6F4FB,
0x4369E96A,0x346ED9FC,0xAD678846,0xDA60B8D0,
0x44042D73,0x33031DE5,0xAA0A4C5F,0xDD0D7CC9,
0x5005713C,0x270241AA,0xBE0B1010,0xC90C2086,
0x5768B525,0x206F85B3,0xB966D409,0xCE61E49F,
0x5EDEF90E,0x29D9C998,0xB0D09822,0xC7D7A8B4,
0x59B33D17,0x2EB40D81,0xB7BD5C3B,0xC0BA6CAD,
0xEDB88320,0x9ABFB3B6,0x03B6E20C,0x74B1D29A,
0xEAD54739,0x9DD277AF,0x04DB2615,0x73DC1683,
0xE3630B12,0x94643B84,0x0D6D6A3E,0x7A6A5AA8,
0xE40ECF0B,0x9309FF9D,0x0A00AE27,0x7D079EB1,
0xF00F9344,0x8708A3D2,0x1E01F268,0x6906C2FE,
0xF762575D,0x806567CB,0x196C3671,0x6E6B06E7,
0xFED41B76,0x89D32BE0,0x10DA7A5A,0x67DD4ACC,
0xF9B9DF6F,0x8EBEEFF9,0x17B7BE43,0x60B08ED5,
0xD6D6A3E8,0xA1D1937E,0x38D8C2C4,0x4FDFF252,
0xD1BB67F1,0xA6BC5767,0x3FB506DD,0x48B2364B,
0xD80D2BDA,0xAF0A1B4C,0x36034AF6,0x41047A60,
0xDF60EFC3,0xA867DF55,0x316E8EEF,0x4669BE79,
0xCB61B38C,0xBC66831A,0x256FD2A0,0x5268E236,
0xCC0C7795,0xBB0B4703,0x220216B9,0x5505262F,
0xC5BA3BBE,0xB2BD0B28,0x2BB45A92,0x5CB36A04,
0xC2D7FFA7,0xB5D0CF31,0x2CD99E8B,0x5BDEAE1D,
0x9B64C2B0,0xEC63F226,0x756AA39C,0x026D930A,
0x9C0906A9,0xEB0E363F,0x72076785,0x05005713,
0x95BF4A82,0xE2B87A14,0x7BB12BAE,0x0CB61B38,
0x92D28E9B,0xE5D5BE0D,0x7CDCEFB7,0x0BDBDF21,
0x86D3D2D4,0xF1D4E242,0x68DDB3F8,0x1FDA836E,
0x81BE16CD,0xF6B9265B,0x6FB077E1,0x18B74777,
0x88085AE6,0xFF0F6A70,0x66063BCA,0x11010B5C,
0x8F659EFF,0xF862AE69,0x616BFFD3,0x166CCF45,
0xA00AE278,0xD70DD2EE,0x4E048354,0x3903B3C2,
0xA7672661,0xD06016F7,0x4969474D,0x3E6E77DB,
0xAED16A4A,0xD9D65ADC,0x40DF0B66,0x37D83BF0,
0xA9BCAE53,0xDEBB9EC5,0x47B2CF7F,0x30B5FFE9,
0xBDBDF21C,0xCABAC28A,0x53B39330,0x24B4A3A6,
0xBAD03605,0xCDD70693,0x54DE5729,0x23D967BF,
0xB3667A2E,0xC4614AB8,0x5D681B02,0x2A6F2B94,
0xB40BBE37,0xC30C8EA1,0x5A05DF1B,0x2D02EF8D
        };
        static SHA256 sha256 = SHA256.Create();
        static Random random = new Random();

        public static byte[] ComputeCRC32(byte[] src, int begin, int length)
        {

            UInt32 crc = 0xFFFFFFFF;
            for (int i = 0; i < length; i++)
            {
                crc = ((crc >> 8) & 0x00FFFFFF) ^ Crc32Table[(crc ^ src[i + begin]) & 0xFF];
            }
            crc = crc ^ 0xFFFFFFFF;
            return BitConverter.GetBytes(crc);
        }
        public static byte[] ComputeSHA256(byte[] src, int begin, int length)
        {
            return sha256.ComputeHash(src, begin, length);
        }
        public static byte[] GetPrivateKeyFromWif(string _wif)
        {
            return NEO.AllianceOfThinWallet.Cryptography.Helper.GetPrivateKeyFromWIF(_wif);
        }

        public static byte[] GetPublicKeyFromPrivateKey(byte[] privateKey)
        {
            return NEO.AllianceOfThinWallet.Cryptography.Helper.GetPublicKeyFromPrivateKey(privateKey);
        }
        public static string GetAddressFromPublicKey(byte[] publickey)
        {
            return NEO.AllianceOfThinWallet.Cryptography.Helper.GetAddressFromPublicKey(publickey);

        }
        public static byte[] DecodeBase58(string txt)
        {
            return NEO.AllianceOfThinWallet.Cryptography.Base58.Decode(txt);
        }

        public static string EncodeBase58(byte[] bytes)
        {
            return NEO.AllianceOfThinWallet.Cryptography.Base58.Encode(bytes);
        }

        public static byte[] SignData(byte[] prikey,byte[] data)
        {
            var PublicKey = NEO.AllianceOfThinWallet.Cryptography.ECC.ECCurve.Secp256r1.G * prikey;
            byte[] pubkey = PublicKey.EncodePoint(false).Skip(1).ToArray();


            //签名的私钥
            byte[] first = { 0x45, 0x43, 0x53, 0x32, 0x20, 0x00, 0x00, 0x00 };
            byte[] signprikey = first.Concat(pubkey).Concat(prikey).ToArray();

            using (System.Security.Cryptography.CngKey key = System.Security.Cryptography.CngKey.Import(signprikey, System.Security.Cryptography.CngKeyBlobFormat.EccPrivateBlob))
            {
                using (System.Security.Cryptography.ECDsaCng ecdsa = new System.Security.Cryptography.ECDsaCng(key))
                {
                    var hashsrc = sha256.ComputeHash(data);
                    var signdata = ecdsa.SignHash(hashsrc);
                    return signdata;
                }
            }
        }
        public static  string EncodeBase58(byte[] data,int begin,int length)
        {
            byte[] input = new byte[length];
            Array.Copy(data, begin, input, 0, length);
            return NEO.AllianceOfThinWallet.Cryptography.Base58.Encode(input);
        }
        public static string Bytes2HexString(byte[] data, int begin, int length)
        {
            StringBuilder sb = new StringBuilder();
            for (var i = 0; i < length; i++)
            {
                sb.Append(data[i + begin].ToString("X02"));
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
        public static UInt16 RandomShort()
        {
            UInt16 v = (UInt16)random.Next();
            return v;
        }
        public static void RandomData(byte[] data)
        {
            random.NextBytes(data);
        }
        public static string GetAddressFromScriptHash(byte[] scripthash)
        {
            byte[] data = new byte[scripthash.Length + 1];
            data[0] = 0x17;
            Array.Copy(scripthash, 0, data, 1, scripthash.Length);
            var hash = sha256.ComputeHash(data);
            hash = sha256.ComputeHash(hash);

            var alldata = data.Concat(hash.Take(4)).ToArray();

            return NEO.AllianceOfThinWallet.Cryptography.Base58.Encode(alldata);
        }
        public struct TransactionOutput
        {
            public string assetId;
            public decimal assetCount;
            public string toAddress;
        }
        public class ContractTransaction
        {
            public TransactionOutput[] outputs;
        }
        public static object ParseTransforData(byte[] data)
        {
            ContractTransaction tdata = null;
            //参考源码来自
            //      https://github.com/neo-project/neo
            //      Transaction.cs
            //      源码采用c#序列化技术

            //参考源码2
            //      https://github.com/AntSharesSDK/antshares-ts
            //      Transaction.ts
            //      采用typescript开发

            System.IO.MemoryStream ms = new System.IO.MemoryStream(data);

            TransactionType type = (TransactionType)ms.ReadByte();//读一个字节，交易类型
            Console.WriteLine("datatype:" + type);
            byte version = (byte)ms.ReadByte();
            Console.WriteLine("dataver:" + version);

            if (type == TransactionType.ContractTransaction)//每个交易类型有一些自己独特的处理
            {
                //ContractTransaction 就是最常见的合约交易，
                //他没有自己的独特处理
                tdata = new ContractTransaction();
            }
            else
            {
                throw new Exception("未编写针对这个交易类型的代码");
            }

            //attributes
            var countAttributes = readVarInt(ms);
            Console.WriteLine("countAttributes:" + countAttributes);
            for (UInt64 i = 0; i < countAttributes; i++)
            {
                //读取attributes
                byte[] attributeData;
                var Usage = (TransactionAttributeUsage)ms.ReadByte();
                if (Usage == TransactionAttributeUsage.ContractHash || Usage == TransactionAttributeUsage.Vote || (Usage >= TransactionAttributeUsage.Hash1 && Usage <= TransactionAttributeUsage.Hash15))
                {
                    attributeData = new byte[32];
                    ms.Read(attributeData, 0, 32);
                }
                else if (Usage == TransactionAttributeUsage.ECDH02 || Usage == TransactionAttributeUsage.ECDH03)
                {
                    attributeData = new byte[33];
                    attributeData[0] = (byte)Usage;
                    ms.Read(attributeData, 1, 32);
                }
                else if (Usage == TransactionAttributeUsage.Script)
                {
                    attributeData = new byte[20];
                    ms.Read(attributeData, 0, 20);
                }
                else if (Usage == TransactionAttributeUsage.DescriptionUrl)
                {
                    var len = (byte)ms.ReadByte();
                    attributeData = new byte[len];
                    ms.Read(attributeData, 0, len);
                }
                else if (Usage == TransactionAttributeUsage.Description || Usage >= TransactionAttributeUsage.Remark)
                {
                    var len = (int)readVarInt(ms, 65535);
                    attributeData = new byte[len];
                    ms.Read(attributeData, 0, len);
                }
                else
                    throw new FormatException();
            }

            //inputs  输入表示基于哪些交易
            var countInputs = readVarInt(ms);
            Console.WriteLine("countInputs:" + countInputs);
            for (UInt64 i = 0; i < countInputs; i++)
            {
                byte[] hash = new byte[32];
                byte[] buf = new byte[2];
                ms.Read(hash, 0, 32);
                ms.Read(buf, 0, 2);
                UInt16 index = (UInt16)(buf[1] * 256 + buf[0]);
                hash = hash.Reverse().ToArray();//反转
                var strhash = Bytes2HexString(hash,0,hash.Length);
                Console.WriteLine("   input:" + strhash + "   index:" + index);
            }

            //outputes 输出表示最后有哪几个地址得到多少钱，肯定有一个是自己的地址,因为每笔交易都会把之前交易的余额清空,刨除自己,就是要转给谁多少钱

            //这个机制叫做UTXO
            var countOutputs = readVarInt(ms);
            Console.WriteLine("countOutputs:" + countOutputs);
            tdata.outputs = new TransactionOutput[countOutputs];
            for (UInt64 i = 0; i < countOutputs; i++)
            {
                TransactionOutput outp = new TransactionOutput();
                //资产种类
                var assetid = new byte[32];
                ms.Read(assetid, 0, 32);
                assetid = assetid.Reverse().ToArray();//反转

                Int64 value = 0; //钱的数字是一个定点数，乘以D 
                Int64 D = 100000000;
                byte[] buf = new byte[8];
                ms.Read(buf, 0, 8);
                value = BitConverter.ToInt64(buf, 0);

                decimal number = ((decimal)value / (decimal)D);
                if (number <= 0)
                    throw new FormatException();
                //资产数量

                var scripthash = new byte[20];

                ms.Read(scripthash, 0, 20);
                var address = GetAddressFromScriptHash(scripthash);
                outp.assetId = Bytes2HexString(assetid,0,assetid.Length);
                outp.assetCount = number;
                outp.toAddress = address;

                tdata.outputs[i] = outp;

                Console.WriteLine("   output" + i + ":" + Bytes2HexString(assetid,0,assetid.Length) + "=" + number);

                Console.WriteLine("       address:" + address);
            }
            return tdata;
        }
        public static UInt64 readVarInt(System.IO.Stream stream, UInt64 max = 9007199254740991)
        {
            var fb = (byte)stream.ReadByte();
            UInt64 value = 0;
            byte[] buf = new byte[8];
            if (fb == 0xfd)
            {
                stream.Read(buf, 0, 2);
                value = (UInt64)(buf[1] * 256 + buf[0]);
            }
            else if (fb == 0xfe)
            {
                stream.Read(buf, 0, 4);
                value = (UInt64)(buf[1] * 256 * 256 * 256 + buf[1] * 256 * 256 + buf[1] * 256 + buf[0]);
            }
            else if (fb == 0xff)
            {
                stream.Read(buf, 0, 8);
                //我懒得展开了，规则同上
                value = BitConverter.ToUInt64(buf, 0);// (UInt64)(buf[1] * 256 * 256 * 256 + buf[1] * 256 * 256 + buf[1] * 256 + buf[0]);
            }
            else
                value = fb;
            if (value > max) throw new Exception("to large.");
            return value;
        }
        public enum TransactionType : byte
        {
            /// <summary>
            /// 用于分配字节费的特殊交易
            /// </summary>
            MinerTransaction = 0x00,
            /// <summary>
            /// 用于分发资产的特殊交易
            /// </summary>
            IssueTransaction = 0x01,
            /// <summary>
            /// 用于分配小蚁币的特殊交易
            /// </summary>
            ClaimTransaction = 0x02,
            /// <summary>
            /// 用于报名成为记账候选人的特殊交易
            /// </summary>
            EnrollmentTransaction = 0x20,
            /// <summary>
            /// 用于资产登记的特殊交易
            /// </summary>
            RegisterTransaction = 0x40,
            /// <summary>
            /// 合约交易，这是最常用的一种交易
            /// </summary>
            ContractTransaction = 0x80,
            /// <summary>
            /// Publish scripts to the blockchain for being invoked later.
            /// </summary>
            PublishTransaction = 0xd0,
            InvocationTransaction = 0xd1
        }
        /// <summary>
        /// 表示交易特性的用途
        /// </summary>
        public enum TransactionAttributeUsage : byte
        {
            /// <summary>
            /// 外部合同的散列值
            /// </summary>
            ContractHash = 0x00,

            /// <summary>
            /// 用于ECDH密钥交换的公钥，该公钥的第一个字节为0x02
            /// </summary>
            ECDH02 = 0x02,
            /// <summary>
            /// 用于ECDH密钥交换的公钥，该公钥的第一个字节为0x03
            /// </summary>
            ECDH03 = 0x03,

            /// <summary>
            /// 用于对交易进行额外的验证
            /// </summary>
            Script = 0x20,

            Vote = 0x30,

            DescriptionUrl = 0x81,
            Description = 0x90,

            Hash1 = 0xa1,
            Hash2 = 0xa2,
            Hash3 = 0xa3,
            Hash4 = 0xa4,
            Hash5 = 0xa5,
            Hash6 = 0xa6,
            Hash7 = 0xa7,
            Hash8 = 0xa8,
            Hash9 = 0xa9,
            Hash10 = 0xaa,
            Hash11 = 0xab,
            Hash12 = 0xac,
            Hash13 = 0xad,
            Hash14 = 0xae,
            Hash15 = 0xaf,

            /// <summary>
            /// 备注
            /// </summary>
            Remark = 0xf0,
            Remark1 = 0xf1,
            Remark2 = 0xf2,
            Remark3 = 0xf3,
            Remark4 = 0xf4,
            Remark5 = 0xf5,
            Remark6 = 0xf6,
            Remark7 = 0xf7,
            Remark8 = 0xf8,
            Remark9 = 0xf9,
            Remark10 = 0xfa,
            Remark11 = 0xfb,
            Remark12 = 0xfc,
            Remark13 = 0xfd,
            Remark14 = 0xfe,
            Remark15 = 0xff
        }

    }
}
