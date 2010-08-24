namespace LegacyOJPLauncher
{
    partial class frmMain
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmMain));
            this.txtNews = new System.Windows.Forms.TextBox();
            this.prgTotal = new System.Windows.Forms.ProgressBar();
            this.prgCurrent = new System.Windows.Forms.ProgressBar();
            this.lblCurrent = new System.Windows.Forms.Label();
            this.lblTotal = new System.Windows.Forms.Label();
            this.tmrStart = new System.Windows.Forms.Timer(this.components);
            this.btnLaunch = new System.Windows.Forms.Button();
            this.chkAuto = new System.Windows.Forms.CheckBox();
            this.btnSettings = new System.Windows.Forms.Button();
            this.btnQuit = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // txtNews
            // 
            this.txtNews.BackColor = System.Drawing.SystemColors.Control;
            this.txtNews.Location = new System.Drawing.Point(66, 169);
            this.txtNews.Multiline = true;
            this.txtNews.Name = "txtNews";
            this.txtNews.ReadOnly = true;
            this.txtNews.Size = new System.Drawing.Size(306, 239);
            this.txtNews.TabIndex = 0;
            // 
            // prgTotal
            // 
            this.prgTotal.Location = new System.Drawing.Point(382, 428);
            this.prgTotal.Name = "prgTotal";
            this.prgTotal.Size = new System.Drawing.Size(321, 23);
            this.prgTotal.TabIndex = 2;
            // 
            // prgCurrent
            // 
            this.prgCurrent.Location = new System.Drawing.Point(382, 457);
            this.prgCurrent.Name = "prgCurrent";
            this.prgCurrent.Size = new System.Drawing.Size(321, 23);
            this.prgCurrent.TabIndex = 3;
            // 
            // lblCurrent
            // 
            this.lblCurrent.AutoSize = true;
            this.lblCurrent.BackColor = System.Drawing.Color.Transparent;
            this.lblCurrent.ForeColor = System.Drawing.Color.Lime;
            this.lblCurrent.Location = new System.Drawing.Point(395, 412);
            this.lblCurrent.Name = "lblCurrent";
            this.lblCurrent.Size = new System.Drawing.Size(60, 13);
            this.lblCurrent.TabIndex = 4;
            this.lblCurrent.Text = "Current File";
            // 
            // lblTotal
            // 
            this.lblTotal.AutoSize = true;
            this.lblTotal.BackColor = System.Drawing.Color.Transparent;
            this.lblTotal.ForeColor = System.Drawing.Color.Lime;
            this.lblTotal.Location = new System.Drawing.Point(246, 467);
            this.lblTotal.Name = "lblTotal";
            this.lblTotal.Size = new System.Drawing.Size(75, 13);
            this.lblTotal.TabIndex = 5;
            this.lblTotal.Text = "Total Progress";
            // 
            // tmrStart
            // 
            this.tmrStart.Tick += new System.EventHandler(this.tmrStart_Tick);
            // 
            // btnLaunch
            // 
            this.btnLaunch.BackColor = System.Drawing.SystemColors.ControlLight;
            this.btnLaunch.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("btnLaunch.BackgroundImage")));
            this.btnLaunch.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.btnLaunch.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.btnLaunch.Font = new System.Drawing.Font("Microsoft Sans Serif", 14F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnLaunch.Location = new System.Drawing.Point(481, 279);
            this.btnLaunch.Name = "btnLaunch";
            this.btnLaunch.Size = new System.Drawing.Size(138, 48);
            this.btnLaunch.TabIndex = 6;
            this.btnLaunch.UseVisualStyleBackColor = false;
            this.btnLaunch.Click += new System.EventHandler(this.btnLaunch_Click);
            // 
            // chkAuto
            // 
            this.chkAuto.AutoSize = true;
            this.chkAuto.BackColor = System.Drawing.Color.Transparent;
            this.chkAuto.ForeColor = System.Drawing.Color.Lime;
            this.chkAuto.Location = new System.Drawing.Point(28, 457);
            this.chkAuto.Name = "chkAuto";
            this.chkAuto.Size = new System.Drawing.Size(147, 17);
            this.chkAuto.TabIndex = 7;
            this.chkAuto.Text = "Auto-Launch after update";
            this.chkAuto.UseVisualStyleBackColor = false;
            this.chkAuto.CheckedChanged += new System.EventHandler(this.chkAuto_CheckedChanged);
            // 
            // btnSettings
            // 
            this.btnSettings.AccessibleName = "Settings";
            this.btnSettings.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("btnSettings.BackgroundImage")));
            this.btnSettings.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.btnSettings.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.btnSettings.Font = new System.Drawing.Font("Microsoft Sans Serif", 14F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnSettings.Location = new System.Drawing.Point(467, 198);
            this.btnSettings.Name = "btnSettings";
            this.btnSettings.Size = new System.Drawing.Size(166, 51);
            this.btnSettings.TabIndex = 8;
            this.btnSettings.UseVisualStyleBackColor = true;
            this.btnSettings.Click += new System.EventHandler(this.btnSettings_Click);
            // 
            // btnQuit
            // 
            this.btnQuit.BackColor = System.Drawing.Color.Transparent;
            this.btnQuit.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("btnQuit.BackgroundImage")));
            this.btnQuit.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.btnQuit.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.btnQuit.Location = new System.Drawing.Point(617, 63);
            this.btnQuit.Name = "btnQuit";
            this.btnQuit.Size = new System.Drawing.Size(86, 33);
            this.btnQuit.TabIndex = 9;
            this.btnQuit.UseVisualStyleBackColor = false;
            this.btnQuit.Click += new System.EventHandler(this.btnQuit_Click);
            // 
            // frmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlDark;
            this.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("$this.BackgroundImage")));
            this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.ClientSize = new System.Drawing.Size(721, 513);
            this.Controls.Add(this.btnQuit);
            this.Controls.Add(this.txtNews);
            this.Controls.Add(this.btnSettings);
            this.Controls.Add(this.chkAuto);
            this.Controls.Add(this.btnLaunch);
            this.Controls.Add(this.lblTotal);
            this.Controls.Add(this.lblCurrent);
            this.Controls.Add(this.prgCurrent);
            this.Controls.Add(this.prgTotal);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(721, 513);
            this.MinimumSize = new System.Drawing.Size(721, 513);
            this.Name = "frmMain";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Legacy OJP Launcher";
            this.Load += new System.EventHandler(this.frmMain_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox txtNews;
        private System.Windows.Forms.ProgressBar prgTotal;
        private System.Windows.Forms.ProgressBar prgCurrent;
        private System.Windows.Forms.Label lblCurrent;
        private System.Windows.Forms.Label lblTotal;
        private System.Windows.Forms.Timer tmrStart;
        private System.Windows.Forms.Button btnLaunch;
        private System.Windows.Forms.CheckBox chkAuto;
        private System.Windows.Forms.Button btnSettings;
        private System.Windows.Forms.Button btnQuit;
    }
}

