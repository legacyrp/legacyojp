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
            this.grpNews = new System.Windows.Forms.GroupBox();
            this.prgTotal = new System.Windows.Forms.ProgressBar();
            this.prgCurrent = new System.Windows.Forms.ProgressBar();
            this.lblCurrent = new System.Windows.Forms.Label();
            this.lblTotal = new System.Windows.Forms.Label();
            this.tmrStart = new System.Windows.Forms.Timer(this.components);
            this.grpNews.SuspendLayout();
            this.SuspendLayout();
            // 
            // txtNews
            // 
            this.txtNews.Location = new System.Drawing.Point(7, 19);
            this.txtNews.Multiline = true;
            this.txtNews.Name = "txtNews";
            this.txtNews.ReadOnly = true;
            this.txtNews.Size = new System.Drawing.Size(619, 178);
            this.txtNews.TabIndex = 0;
            // 
            // grpNews
            // 
            this.grpNews.Controls.Add(this.txtNews);
            this.grpNews.Location = new System.Drawing.Point(5, 2);
            this.grpNews.Name = "grpNews";
            this.grpNews.Size = new System.Drawing.Size(635, 203);
            this.grpNews.TabIndex = 1;
            this.grpNews.TabStop = false;
            this.grpNews.Text = "News";
            // 
            // prgTotal
            // 
            this.prgTotal.Location = new System.Drawing.Point(12, 287);
            this.prgTotal.Name = "prgTotal";
            this.prgTotal.Size = new System.Drawing.Size(619, 23);
            this.prgTotal.TabIndex = 2;
            // 
            // prgCurrent
            // 
            this.prgCurrent.Location = new System.Drawing.Point(12, 244);
            this.prgCurrent.Name = "prgCurrent";
            this.prgCurrent.Size = new System.Drawing.Size(619, 23);
            this.prgCurrent.TabIndex = 3;
            // 
            // lblCurrent
            // 
            this.lblCurrent.AutoSize = true;
            this.lblCurrent.Location = new System.Drawing.Point(12, 228);
            this.lblCurrent.Name = "lblCurrent";
            this.lblCurrent.Size = new System.Drawing.Size(60, 13);
            this.lblCurrent.TabIndex = 4;
            this.lblCurrent.Text = "Current File";
            // 
            // lblTotal
            // 
            this.lblTotal.AutoSize = true;
            this.lblTotal.Location = new System.Drawing.Point(12, 271);
            this.lblTotal.Name = "lblTotal";
            this.lblTotal.Size = new System.Drawing.Size(75, 13);
            this.lblTotal.TabIndex = 5;
            this.lblTotal.Text = "Total Progress";
            // 
            // tmrStart
            // 
            this.tmrStart.Tick += new System.EventHandler(this.tmrStart_Tick);
            // 
            // frmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLight;
            this.ClientSize = new System.Drawing.Size(643, 335);
            this.ControlBox = false;
            this.Controls.Add(this.lblTotal);
            this.Controls.Add(this.lblCurrent);
            this.Controls.Add(this.prgCurrent);
            this.Controls.Add(this.prgTotal);
            this.Controls.Add(this.grpNews);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "frmMain";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Legacy OJP Launcher";
            this.Load += new System.EventHandler(this.frmMain_Load);
            this.grpNews.ResumeLayout(false);
            this.grpNews.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox txtNews;
        private System.Windows.Forms.GroupBox grpNews;
        private System.Windows.Forms.ProgressBar prgTotal;
        private System.Windows.Forms.ProgressBar prgCurrent;
        private System.Windows.Forms.Label lblCurrent;
        private System.Windows.Forms.Label lblTotal;
        private System.Windows.Forms.Timer tmrStart;
    }
}

