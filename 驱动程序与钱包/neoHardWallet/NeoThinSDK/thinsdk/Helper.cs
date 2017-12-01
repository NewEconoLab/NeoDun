using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;


using NEO.AllianceOfThinWallet.Cryptography;

namespace NEO.AllianceOfThinWallet.Cryptography
{
    public static class Helper
    {
        static System.Security.Cryptography.SHA256 sha256 = System.Security.Cryptography.SHA256.Create();
        static RIPEMD160Managed ripemd160 = new RIPEMD160Managed();
        //static System.Security.Cryptography.RIPEMD160 ripemd160 = System.Security.Cryptography.RIPEMD160.Create();

        public static byte[] GetPrivateKeyFromWIF(string wif)
        {
            if (wif == null) throw new ArgumentNullException();
            byte[] data = Base58.Decode(wif);
            //检查标志位
            if (data.Length != 38 || data[0] != 0x80 || data[33] != 0x01)
                throw new Exception("wif length or tag is error");
            //取出检验字节
            var sum = data.Skip(data.Length - 4);
            byte[] realdata = data.Take(data.Length - 4).ToArray();

            //验证,对前34字节进行进行两次hash取前4个字节
            byte[] checksum = sha256.ComputeHash(realdata);
            checksum = sha256.ComputeHash(checksum);
            var sumcalc = checksum.Take(4);
            if (sum.SequenceEqual(sumcalc) == false)
                throw new Exception("the sum is not match.");

            byte[] privateKey = new byte[32];
            Buffer.BlockCopy(data, 1, privateKey, 0, privateKey.Length);
            Array.Clear(data, 0, data.Length);
            return privateKey;
        }

        public static byte[] GetPublicKeyFromPrivateKey(byte[] privateKey)
        {
            var PublicKey = ECC.ECCurve.Secp256r1.G * privateKey;
            return PublicKey.EncodePoint(true);
        }
        public static byte[] GetPublicKeyFromPrivateKey_NoComp(byte[] privateKey)
        {
            var PublicKey = ECC.ECCurve.Secp256r1.G * privateKey;
            return PublicKey.EncodePoint(false);//.Skip(1).ToArray();
        }
        public static byte[] GetScriptFromPublicKey(byte[] publicKey)
        {
            byte[] script = new byte[publicKey.Length + 2];
            script[0] = (byte)publicKey.Length;
            Array.Copy(publicKey, 0, script, 1, publicKey.Length);
            script[script.Length - 1] = 172;//CHECKSIG
            return script;
        }
        public static byte[] GetScriptHashFromPublicKey(byte[] publicKey)
        {
            byte[] script = GetScriptFromPublicKey(publicKey);
            var scripthash = sha256.ComputeHash(script);
            scripthash = ripemd160.ComputeHash(scripthash);
            return scripthash;
        }
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
        public static string GetAddressFromPublicKey(byte[] publickey)
        {
            byte[] scriptHash = GetScriptHashFromPublicKey(publickey);
            return GetAddressFromScriptHash(scriptHash);
        }
        public static byte[] GetPublicKeyHash(byte[] publickey)
        {
            var hash1 = sha256.ComputeHash(publickey);
            var hash2 = ripemd160.ComputeHash(hash1);
            return hash2;
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
        public static byte[] HexString2Bytes(string str)
        {
            byte[] outd = new byte[str.Length / 2];
            for (var i = 0; i < str.Length / 2; i++)
            {
                outd[i] = byte.Parse(str.Substring(i * 2, 2), System.Globalization.NumberStyles.HexNumber);
            }
            return outd;
        }

        public static int BitLen(int w)
        {
            return (w < 1 << 15 ? (w < 1 << 7
                ? (w < 1 << 3 ? (w < 1 << 1
                ? (w < 1 << 0 ? (w < 0 ? 32 : 0) : 1)
                : (w < 1 << 2 ? 2 : 3)) : (w < 1 << 5
                ? (w < 1 << 4 ? 4 : 5)
                : (w < 1 << 6 ? 6 : 7)))
                : (w < 1 << 11
                ? (w < 1 << 9 ? (w < 1 << 8 ? 8 : 9) : (w < 1 << 10 ? 10 : 11))
                : (w < 1 << 13 ? (w < 1 << 12 ? 12 : 13) : (w < 1 << 14 ? 14 : 15)))) : (w < 1 << 23 ? (w < 1 << 19
                ? (w < 1 << 17 ? (w < 1 << 16 ? 16 : 17) : (w < 1 << 18 ? 18 : 19))
                : (w < 1 << 21 ? (w < 1 << 20 ? 20 : 21) : (w < 1 << 22 ? 22 : 23))) : (w < 1 << 27
                ? (w < 1 << 25 ? (w < 1 << 24 ? 24 : 25) : (w < 1 << 26 ? 26 : 27))
                : (w < 1 << 29 ? (w < 1 << 28 ? 28 : 29) : (w < 1 << 30 ? 30 : 31)))));
        }
        public static int GetBitLength(this BigInteger i)
        {
            byte[] b = i.ToByteArray();
            return (b.Length - 1) * 8 + BitLen(i.Sign > 0 ? b[b.Length - 1] : 255 - b[b.Length - 1]);
        }

        public static int GetLowestSetBit(this BigInteger i)
        {
            if (i.Sign == 0)
                return -1;
            byte[] b = i.ToByteArray();
            int w = 0;
            while (b[w] == 0)
                w++;
            for (int x = 0; x < 8; x++)
                if ((b[w] & 1 << x) > 0)
                    return x + w * 8;
            throw new Exception();
        }

        public static BigInteger Mod(this BigInteger x, BigInteger y)
        {
            x %= y;
            if (x.Sign < 0)
                x += y;
            return x;
        }

        public static bool TestBit(this BigInteger i, int index)
        {
            return (i & (BigInteger.One << index)) > BigInteger.Zero;
        }
        internal static BigInteger ModInverse(this BigInteger a, BigInteger n)
        {
            BigInteger i = n, v = 0, d = 1;
            while (a > 0)
            {
                BigInteger t = i / a, x = a;
                a = i % x;
                i = x;
                x = d;
                d = v - t * x;
                v = x;
            }
            v %= n;
            if (v < 0) v = (v + n) % n;
            return v;
        }

        internal static BigInteger NextBigInteger(this Random rand, int sizeInBits)
        {
            if (sizeInBits < 0)
                throw new ArgumentException("sizeInBits must be non-negative");
            if (sizeInBits == 0)
                return 0;
            byte[] b = new byte[sizeInBits / 8 + 1];
            rand.NextBytes(b);
            if (sizeInBits % 8 == 0)
                b[b.Length - 1] = 0;
            else
                b[b.Length - 1] &= (byte)((1 << sizeInBits % 8) - 1);
            return new BigInteger(b);
        }

        internal static BigInteger NextBigInteger(this System.Security.Cryptography.RandomNumberGenerator rng, int sizeInBits)
        {
            if (sizeInBits < 0)
                throw new ArgumentException("sizeInBits must be non-negative");
            if (sizeInBits == 0)
                return 0;
            byte[] b = new byte[sizeInBits / 8 + 1];
            rng.GetBytes(b);
            if (sizeInBits % 8 == 0)
                b[b.Length - 1] = 0;
            else
                b[b.Length - 1] &= (byte)((1 << sizeInBits % 8) - 1);
            return new BigInteger(b);
        }

        public static byte[] Sign(byte[] message, byte[] prikey)
        {
            var Secp256r1_G = NEO.AllianceOfThinWallet.Cryptography.Helper.HexString2Bytes("04" + "6B17D1F2E12C4247F8BCE6E563A440F277037D812DEB33A0F4A13945D898C296" + "4FE342E2FE1A7F9B8EE7EB4A7C0F9E162BCE33576B315ECECBB6406837BF51F5");

            var PublicKey = ECC.ECCurve.Secp256r1.G * prikey;
            var pubkey = PublicKey.EncodePoint(false).Skip(1).ToArray();
            //#if NET461
            const int ECDSA_PRIVATE_P256_MAGIC = 0x32534345;
            prikey = BitConverter.GetBytes(ECDSA_PRIVATE_P256_MAGIC).Concat(BitConverter.GetBytes(32)).Concat(pubkey).Concat(prikey).ToArray();
            using (System.Security.Cryptography.CngKey key = System.Security.Cryptography.CngKey.Import(prikey, System.Security.Cryptography.CngKeyBlobFormat.EccPrivateBlob))
            using (System.Security.Cryptography.ECDsaCng ecdsa = new System.Security.Cryptography.ECDsaCng(key))
            //#else
            //            using (var ecdsa = System.Security.Cryptography.ECDsa.Create(new System.Security.Cryptography.ECParameters
            //            {
            //                Curve = System.Security.Cryptography.ECCurve.NamedCurves.nistP256,
            //                D = prikey,
            //                Q = new System.Security.Cryptography.ECPoint
            //                {
            //                    X = pubkey.Take(32).ToArray(),
            //                    Y = pubkey.Skip(32).ToArray()
            //                }
            //            }))
            //#endif
            {
                var hash = sha256.ComputeHash(message);
                return ecdsa.SignHash(message);
            }
        }

        //public static bool VerifySignature(byte[] message, byte[] signature, byte[] pubkey)
        //{

        //}
    }
}
