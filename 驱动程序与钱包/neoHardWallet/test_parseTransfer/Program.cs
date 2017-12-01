using NEO.AllianceOfThinWallet.Cryptography;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace test_parseTransfer
{
    class Program
    {
        static void Main(string[] args)
        {
            string src = "80000002235fc72a3372fa601d39017685c3a77ebb24f1369a16e80f218f5ff76880fb3800005d1ab60715d73e806cca1449fc089520211360ea526450803345e8987ad35e76010002e72d286979ee6cb1b7e65dfddfb2e384100b8d148e7758de42e4168b71792c60008b585a170000001b02c180df019e6113a985411cae62db80f90db4e72d286979ee6cb1b7e65dfddfb2e384100b8d148e7758de42e4168b71792c6070222f1817000000738679b1fd7dbc21fa7ebc1218e74f08e6afbdae";
            string src2 = "800000010c8fa242aaf7461f802bf0d6e0fc8694e3daed2d66de4dc70fe017012d075350010002e72d286979ee6cb1b7e65dfddfb2e384100b8d148e7758de42e4168b71792c6080969800000000001b02c180df019e6113a985411cae62db80f90db4e72d286979ee6cb1b7e65dfddfb2e384100b8d148e7758de42e4168b71792c607098df2817000000738679b1fd7dbc21fa7ebc1218e74f08e6afbdae";

            var bytes = HexString2Bytes(src);
            Console.WriteLine("=====parse bytes:" + src + "  len=" + bytes.Length);
            ParseTransforData(bytes);


            var bytes2 = HexString2Bytes(src2);
            Console.WriteLine("=====parse bytes:" + src2 + "  len=" + bytes.Length);
            ParseTransforData(bytes2);
            Console.ReadLine();
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
                var strhash = Bytes2HexString(hash);
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
                outp.assetId = Bytes2HexString(assetid);
                outp.assetCount = number;
                outp.toAddress = address;

                tdata.outputs[i] = outp;

                Console.WriteLine("   output" + i + ":" + Bytes2HexString(assetid) + "=" + number);

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
        public static byte[] HexString2Bytes(string str)
        {
            byte[] outd = new byte[str.Length / 2];
            for (var i = 0; i < str.Length / 2; i++)
            {
                outd[i] = byte.Parse(str.Substring(i * 2, 2), System.Globalization.NumberStyles.HexNumber);
            }
            return outd;
        }
        public static string Bytes2HexString(byte[] data)
        {
            StringBuilder sb = new StringBuilder();
            foreach (var d in data)
            {
                sb.Append(d.ToString("x02"));
            }
            return sb.ToString();
        }
        static System.Security.Cryptography.SHA256 sha256 = System.Security.Cryptography.SHA256.Create();

        public static string GetAddressFromScriptHash(byte[] scripthash)
        {
            byte[] data = new byte[scripthash.Length + 1];
            data[0] = 0x17;
            Array.Copy(scripthash, 0, data, 1, scripthash.Length);
            var hash = sha256.ComputeHash(data);
            hash = sha256.ComputeHash(hash);

            var alldata = data.Concat(hash.Take(4)).ToArray();

            return Base58.Encode(alldata);
        }
    }
}
