using NeoDun;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

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

        static System.Security.Cryptography.SHA256 sha256 = System.Security.Cryptography.SHA256.Create();
        static RIPEMD160Managed ripemd160 = new RIPEMD160Managed();

        public ECDH()
        {
            priKey = new byte[32];
            using (RandomNumberGenerator rng = RandomNumberGenerator.Create())
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

        public string GetPubHash(byte[] pub)
        {
            return "1234";
            byte[] data = SignTool.ComputeCRC32(pub,0,pub.Length);
            return ThinNeo.Helper.Bytes2HexString(data);
        }

        /// <summary>
        /// 检查m是否为null
        /// </summary>
        /// <returns>null返回true</returns>
        /// 
        public bool CheckM()
        {
            return M == null ? false : true;
        }

        public void UnInit()
        {
            priKey = null;
            pubKey = null;
            M = null;
        }

    }
}
