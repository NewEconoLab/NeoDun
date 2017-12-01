namespace driver
{
    partial class Form1
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.label1 = new System.Windows.Forms.Label();
            this.textWIF = new System.Windows.Forms.TextBox();
            this.labelWif = new System.Windows.Forms.Label();
            this.buttonWif = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.labelAccount = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("宋体", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label1.ForeColor = System.Drawing.Color.White;
            this.label1.Location = new System.Drawing.Point(123, 52);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(303, 33);
            this.label1.TabIndex = 0;
            this.label1.Text = "请假装我是硬件钱包";
            // 
            // textWIF
            // 
            this.textWIF.Font = new System.Drawing.Font("宋体", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.textWIF.Location = new System.Drawing.Point(159, 114);
            this.textWIF.Name = "textWIF";
            this.textWIF.Size = new System.Drawing.Size(283, 31);
            this.textWIF.TabIndex = 1;
            this.textWIF.Text = "Ky1X7GjA1y7UrUWhSWvAvNGdgWNvVJcL1t3AgRMApTcPkf85XS3i";
            this.textWIF.TextChanged += new System.EventHandler(this.textWIF_TextChanged);
            // 
            // labelWif
            // 
            this.labelWif.AutoSize = true;
            this.labelWif.Font = new System.Drawing.Font("宋体", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.labelWif.ForeColor = System.Drawing.Color.White;
            this.labelWif.Location = new System.Drawing.Point(68, 124);
            this.labelWif.Name = "labelWif";
            this.labelWif.Size = new System.Drawing.Size(85, 21);
            this.labelWif.TabIndex = 2;
            this.labelWif.Text = "导入WIF";
            // 
            // buttonWif
            // 
            this.buttonWif.Font = new System.Drawing.Font("宋体", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.buttonWif.Location = new System.Drawing.Point(448, 114);
            this.buttonWif.Name = "buttonWif";
            this.buttonWif.Size = new System.Drawing.Size(75, 31);
            this.buttonWif.TabIndex = 3;
            this.buttonWif.Text = "go";
            this.buttonWif.UseVisualStyleBackColor = true;
            this.buttonWif.Click += new System.EventHandler(this.buttonWif_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("宋体", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label3.ForeColor = System.Drawing.Color.White;
            this.label3.Location = new System.Drawing.Point(68, 354);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(94, 21);
            this.label3.TabIndex = 4;
            this.label3.Text = "咱的账户";
            // 
            // labelAccount
            // 
            this.labelAccount.AutoSize = true;
            this.labelAccount.Font = new System.Drawing.Font("宋体", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.labelAccount.ForeColor = System.Drawing.Color.White;
            this.labelAccount.Location = new System.Drawing.Point(168, 354);
            this.labelAccount.Name = "labelAccount";
            this.labelAccount.Size = new System.Drawing.Size(73, 21);
            this.labelAccount.TabIndex = 5;
            this.labelAccount.Text = "。。。";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Black;
            this.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("$this.BackgroundImage")));
            this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
            this.ClientSize = new System.Drawing.Size(545, 384);
            this.ControlBox = false;
            this.Controls.Add(this.labelAccount);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.buttonWif);
            this.Controls.Add(this.labelWif);
            this.Controls.Add(this.textWIF);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Form1";
            this.ShowIcon = false;
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox textWIF;
        private System.Windows.Forms.Label labelWif;
        private System.Windows.Forms.Button buttonWif;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label labelAccount;
    }
}

