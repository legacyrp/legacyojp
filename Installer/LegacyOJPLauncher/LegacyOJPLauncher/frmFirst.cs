using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace LegacyOJPLauncher
{
    public partial class frmFirst : Form
    {
        public frmFirst()
        {
            InitializeComponent();
        }

        private void btnBrowse_Click(object sender, EventArgs e)
        {
            diagBrowse.ShowDialog();
            txtGamePath.Text = diagBrowse.SelectedPath.ToString();
        }

        private void btnSubmit_Click(object sender, EventArgs e)
        {
            if (txtGamePath.Text == "")
            {
                MessageBox.Show("You must enter the path to your Gamedata Folder");
                return;
            }
            FileStream file = new FileStream("config.dat", FileMode.OpenOrCreate, FileAccess.Write);
            StreamWriter sw = new StreamWriter(file);
            sw.Write(txtGamePath.Text);
            sw.Close();
            file.Close();
            Form newMain = new frmMain(true, txtGamePath.Text,false);
            newMain.Show();
            this.Hide();
        }
    }
}
