using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace NeoDun
{
    public enum DataBlockFrom
    {
        FromDriver,
        FromSigner,
    }
    public class DataBlock
    {
        public DataBlockFrom from;
        public DataBlock(string sha256, UInt32 length, DataBlockFrom from)
        {
            this.from = from;
            this.sha256 = sha256;
            this.data = new byte[length];
            UInt32 p = length / 50;
            var m = length % 50;
            if (m != 0) p++;
            this.piecetag = new byte[p];
        }
        public override string ToString()
        {
            return (from== DataBlockFrom.FromDriver?"[D]":"[S]")+ data.Length + "  id=" + dataid  +" id(S)="+ dataidRemote + "  match=" + match + "  hash=" + sha256;
        }
        public UInt32 dataid;
        public UInt32 dataidRemote;
        public string sha256;
        public byte[] data;
        public byte[] piecetag;
        bool match = false;

        public bool Check()
        {
            byte[] hashdata = SignTool.HexString2Bytes(sha256);
            var hash = SignTool.ComputeSHA256(data, 0, data.Length);
            var hashstr = SignTool.Bytes2HexString(hash, 0, hash.Length);
            match = (hashstr.ToUpper() == sha256.ToUpper());
            return match;
        }
        public void FillPieceMessage(Message msg, UInt16 msgid, UInt16 piece)
        {
            msg.tag1 = 0x01;
            msg.tag2 = 0xa2;
            msg.msgid = msgid;// 这个需要回复串
            var pd = BitConverter.GetBytes(piece);
            msg.data[0] = pd[0];
            msg.data[1] = pd[1];
            var begin = piece * 50;
            var end = (piece + 1) * 50;
            if (end > data.Length)
                end = data.Length;
            for (var i = 0; i < 50; i++)
            {
                var seek = begin + i;
                if (seek < end)
                    msg.data[i + 2] = data[seek];
                else
                    msg.data[i + 2] = 0;
            }
        }
        public void FromPieceMsg(Message msg)
        {
            if (msg.tag1 != 0x01 || msg.tag2 != 0xa2)
            {
                throw new Exception("tag error");
            }
            var piece = BitConverter.ToUInt16(msg.data, 0);
            piecetag[piece] = 1;
            var begin = piece * 50;
            var end = (piece + 1) * 50;
            if (end > data.Length)
                end = data.Length;
            for (var i = 0; i < 50; i++)
            {
                var seek = begin + i;
                if (seek < end)
                    data[seek] = msg.data[i + 2];
                else
                    continue;
                    //data[seek] = 0;
            }
        }

        public async Task<UInt32> GetRemoteid()
        {
            int time = 0;
            while (true)
            {
                await Task.Delay(10);
                time += 10;
                if (time >= 60000)
                    return 0;
                if (this.dataidRemote > 0 && this.Check())
                {
                    return this.dataidRemote;
                }
            }
        }
    }

    public class DataTable
    {
        public DataBlock newOrGet(string sha256, UInt32 length, DataBlockFrom from)
        {
            foreach (var b in blocks)
            {
                if (sha256.ToUpper() == b.sha256.ToUpper()&&b.from==from)
                {
                    return b;
                }
            }
            var nb= new DataBlock(sha256,(UInt32)length, from);
            this.AddBlock(nb);
            return nb;
        }
        UInt32 uid = 1;
        public System.Collections.Concurrent.ConcurrentBag<DataBlock> blocks = new System.Collections.Concurrent.ConcurrentBag<DataBlock>();
        public DataBlock getBlockByDataId(UInt32 dataid)
        {
            foreach (var b in blocks)
            {
                if (b.dataid == dataid)
                    return b;
            }
            return null;
        }
        public DataBlock getBlockBySha256(string sha256)
        {
            foreach (var b in blocks)
            {
                if (sha256.ToUpper() == b.sha256.ToUpper())
                {
                    return b;
                }
            }
            return null;
        }
        public void AddBlock(DataBlock block)
        {
            block.dataid = uid;
            while (getBlockByDataId(block.dataid) != null)
            {
                block.dataid = uid;
                uid++;
            }
            this.blocks.Add(block);
        }

    }


    public enum ESignerStateOfNew: byte
    {
        Unkonw = 0,
        New = 1,
        Old = 2
    }

    public enum Enum_DriverFun
    {
        连接钱包后是否自动弹出驱动界面 = 0,
        开机时是否自动检查更新 = 1,
        新增地址时是否要密码验证 = 2,
        删除地址是否要密码验证 = 3,
        备份地址是否要密码验证 = 4,
        备份钱包时进行是否要加密 = 5,
    }
}
