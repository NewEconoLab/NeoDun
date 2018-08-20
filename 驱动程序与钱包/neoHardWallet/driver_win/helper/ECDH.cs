using NeoDun;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;
using ThinNeo.Cryptography.Cryptography;

namespace driver_win.helper
{
    class ECDH
    {
        private static ECDH ins;

        public static ECDH Ins
        {
            get
            {
                if (ins == null)
                    ins = new ECDH();
                return ins;
            }
        }

        public byte[] priKey { private set; get; } //a
        public byte[] pubKey { private set; get; } //A
        public byte[] M { private set; get; }
        public byte[] pubKey_B;//B

        public bool CheckResult { private set; get; }

        static System.Security.Cryptography.SHA256 sha256 = System.Security.Cryptography.SHA256.Create();
        static RIPEMD160Managed ripemd160 = new RIPEMD160Managed();

        public ECDH()
        {
            priKey = new byte[32];
            using (System.Security.Cryptography.RandomNumberGenerator rng = System.Security.Cryptography.RandomNumberGenerator.Create())
            {
                rng.GetBytes(priKey);
            }
            var PublicKey = ThinNeo.Cryptography.ECC.ECCurve.Secp256r1.G * priKey;
            pubKey = PublicKey.EncodePoint(false);
        }

        public void CalM(byte[] pubKey_B)
        {
            var EcpointB = ThinNeo.Cryptography.ECC.ECPoint.DecodePoint(pubKey_B, ThinNeo.Cryptography.ECC.ECCurve.Secp256r1);
            var EcpointM = EcpointB * priKey;
            M= EcpointM.EncodePoint(false);
        }

        public void CheckPubHash(string hash)
        {
            byte[] data = SignTool.ComputeSHA256(pubKey_B, 0, pubKey_B.Length);
            string str = SignTool.EncodeBase58(data, 0, 25);
            str = str.Substring(0,4);
            CheckResult =  hash == str;
        }

        /// <summary>
        /// null返回false
        /// </summary>
        /// <returns></returns>
        /// 
        public bool CheckM()
        {
            return M == null ? false : true;
        }

        public void Release()
        {
            priKey = null;
            pubKey = null;
            M = null;
        }

    }
}
