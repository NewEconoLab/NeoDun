using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace driver
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            //this.WindowState = FormWindowState.Minimized;
            hhgate.CustomServer.BeginServer();
        }
        protected override void WndProc(ref Message m)
        {
            switch (m.Msg)
            {
                case 0x4e:
                case 0xd:
                case 0xe:
                case 0x14:
                    base.WndProc(ref m);
                    break;
                case 0x84://鼠标点任意位置后可以拖动窗体
                    this.DefWndProc(ref m);
                    if (m.Result.ToInt32() == 0x01)
                    {
                        m.Result = new IntPtr(0x02);
                    }
                    break;
                case 0xA3://禁止双击最大化
                    break;
                default:
                    base.WndProc(ref m);
                    break;
            }
        }


        public static string publicKey_NoComp ="";
        public static byte[] privateKey;
        public static byte[] publicKey;
        public static string address ="";
        private void buttonWif_Click(object sender, EventArgs e)
        {
            //导入WIF
            try
            {
                var txt = this.textWIF.Text;
                var bytes_PrivateKey = NEO.AllianceOfThinWallet.Cryptography.Helper.GetPrivateKeyFromWIF(txt);
                var bytes_PublicKey = NEO.AllianceOfThinWallet.Cryptography.Helper.GetPublicKeyFromPrivateKey(bytes_PrivateKey);
                publicKey_NoComp = NEO.AllianceOfThinWallet.Cryptography.Helper.Bytes2HexString(NEO.AllianceOfThinWallet.Cryptography.Helper.GetPublicKeyFromPrivateKey_NoComp(bytes_PrivateKey));
                var bytes_PublicKeyHash = NEO.AllianceOfThinWallet.Cryptography.Helper.GetPublicKeyHash(bytes_PublicKey);
                address = NEO.AllianceOfThinWallet.Cryptography.Helper.GetAddressFromPublicKey(bytes_PublicKey);
                privateKey = bytes_PrivateKey;
                publicKey = bytes_PublicKey;
            }
            catch
            {
                address = "";
                MessageBox.Show("导入WIF错误");
            }
            this.labelWif.Visible = false;
            this.textWIF.Visible = false;
            this.buttonWif.Visible = false;
            this.labelAccount.Text = address;
        }

        private void textWIF_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
