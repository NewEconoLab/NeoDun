using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace NeoDun
{
    public class Message
    {
        public byte tag1;
        public byte tag2;
        public UInt16 msgid;
        public byte[] data = new byte[58];
        public byte crc1;
        public byte crc2;
        public UInt32 readUInt32(int pos)
        {
            return BitConverter.ToUInt32(data, pos);
        }
        public void writeUInt32(int pos, UInt32 value)
        {
            var b = BitConverter.GetBytes(value);
            for (var i = 0; i < b.Length; i++)
            {
                data[pos + i] = b[i];
            }
        }
        public UInt32 readUInt16(int pos)
        {
            return BitConverter.ToUInt16(data, pos);
        }
        public void writeUInt16(int pos, UInt16 value)
        {
            var b = BitConverter.GetBytes(value);
            for (var i = 0; i < b.Length; i++)
            {
                data[pos + i] = b[i];
            }
        }
        public string readHash256(int pos,int len = 32)
        {
            return SignTool.Bytes2HexString(data, pos, len);
        }
        public void writeHash256(int pos, string hash)
        {
            var b = SignTool.HexString2Bytes(hash);
            for (var i = 0; i < b.Length; i++)
            {
                data[pos + i] = b[i];
            }
        }
        public void writeHash256(int pos, byte[] hash)
        {

            for (var i = 0; i < hash.Length; i++)
            {
                data[pos + i] = hash[i];
            }
        }
        public void ToData(byte[] _data)
        {
            _data[0] = tag1;
            _data[1] = tag2;
            var d = BitConverter.GetBytes(msgid);
            _data[2] = d[0];
            _data[3] = d[1];
            for (var i = 4; i < 62; i++)
            {
                var seek = i - 4;
                if (seek < this.data.Length)
                    _data[i] = this.data[seek];
                else
                    _data[i] = 0;
            }
            var crc = SignTool.ComputeCRC32(_data, 0, 62);
            _data[62] = crc[0];
            _data[63] = crc[1];


        }
        public void FromData(byte[] _data)
        {
            this.tag1 = _data[0];
            this.tag2 = _data[1];
            this.msgid = BitConverter.ToUInt16(_data, 2);
            for (var i = 4; i < 62; i++)
            {
                this.data[i - 4] = _data[i];
            }
            this.crc1 = _data[62];
            this.crc2 = _data[63];
            var crc = SignTool.ComputeCRC32(_data, 0, 62);
            if (this.crc1 != crc[0] || this.crc2 != crc[1])
            {
                throw new Error_MsgHashNotMatch("the message hash not match");
            }
        }
        public override string ToString()
        {
            return this.tag1.ToString("X02") + this.tag2.ToString("X02");
        }
    }

}
