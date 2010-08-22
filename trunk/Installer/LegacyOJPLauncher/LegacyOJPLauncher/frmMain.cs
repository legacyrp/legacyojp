using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using System.IO;
using System.Net;
using System.Threading;
using System.Security.Cryptography;
using System.Diagnostics;

namespace LegacyOJPLauncher
{
    public partial class frmMain : Form
    {
        //Variables
        private XmlDocument xmlFileList;
        string path = "http://legacyrp.com/LegacyOJP.xml";
        public bool gamePathSet = false;
        public string gamePath = "";
        public List<file> fileList = new List<file>();
        
        public Form gamedataChange = new frmFirst();

        #region Download required variables

        // The thread inside which the download happens
        private Thread thrDownload;
        // The stream of data retrieved from the web server
        Stream strResponse;
        // The stream of data that we write to the harddrive
        Stream strLocal;
        // The request to the web server for file information
        HttpWebRequest webRequest;
        // The response from the web server containing information about the file
        HttpWebResponse webResponse;
        // The progress of the download in percentage
        private static int PercentProgress;
        // The delegate which we will call from the thread to update the form
        private delegate void UpdateProgessCallback(Int64 BytesRead, Int64 TotalBytes);
        public string url;
        public string filename;
        public string folder;
        public bool fileDownloaded = false;
        #endregion

        //Constructor if we aren't configured
        public frmMain()
        {
            thrDownload = new Thread(Download);
            InitializeComponent();
        }
        //Constructor if we are configured
        public frmMain(bool gamePathSet, string gamepath, bool auto)
        {
            this.gamePath = gamepath;
            this.gamePathSet = gamePathSet;
            InitializeComponent();
            tmrStart.Enabled = true;
            if (auto)
                chkAuto.Checked = true;
        }
        ~frmMain()
        {
            if (thrDownload.IsAlive)
            {
                thrDownload.Abort();
            }
        }

        private void frmMain_Load(object sender, EventArgs e)
        {
            getNews();
            getFileList();
            tmrStart.Interval = 5000;
            tmrStart.Start();
        }
        public void MoveToCorrectFolder(string destination)
        {
            string filepath = "";
            try
            {
                if (this.folder == "Gamedata")
                {
                    filepath = gamePath;
                }
                else
                {
                    filepath = gamePath + "/" + this.folder;
                }
                if (!Directory.Exists(filepath))
                {
                    Directory.CreateDirectory(filepath);
                }
                File.Move(filename, destination);
            }
            catch (SystemException) { }
        }
        private int getDownloadTotal()
        {
            int total = 0;

            for (int i = 0; i < fileList.Count; i++)
            {
                    total++;
            }

            return total;
        }

       
        private void Download()
        {
            Int64 size = 0;
            using (WebClient wcDownload = new WebClient())
            {
                try
                {
                    wcDownload.Headers.Add("User-Agent", "Mozilla/4.0+");
                    // Create a request to the file we are downloading
                    webRequest = (HttpWebRequest)WebRequest.Create(this.url);
                    // Set default authentication for retrieving the file
                    webRequest.Credentials = CredentialCache.DefaultNetworkCredentials;
                    // Retrieve the response from the server
                    webResponse = (HttpWebResponse)webRequest.GetResponse();
                    // Ask the server for the file size and store it
                    Int64 fileSize = webResponse.ContentLength;
                    size = fileSize;
                    // It will store the current number of bytes we retrieved from the server
                    int bytesSize = 0;
                    // A buffer for storing and writing the data retrieved from the server
                    byte[] downBuffer = new byte[2048];

                    strResponse = wcDownload.OpenRead(this.url);
                    strLocal = new FileStream((this.filename + ".incomplete"), FileMode.OpenOrCreate, FileAccess.Write, FileShare.None);


                    // Loop through the buffer until the buffer is empty
                    while ((bytesSize = strResponse.Read(downBuffer, 0, downBuffer.Length)) > 0)
                    {
                        if (gamedataChange.Visible == true)
                        {
                            break;
                        }

                        else
                        {
                            // Write the data from the buffer to the local hard drive
                            strLocal.Write(downBuffer, 0, bytesSize);
                            // Invoke the method that updates the form's label and progress bar
                            this.Invoke(new UpdateProgessCallback(this.UpdateProgress), new object[] { strLocal.Length, fileSize });
                        }

                        /*while ((bytesSize = strResponse.Read(downBuffer, 0, downBuffer.Length)) > 0)
                        {
                            // Write the data from the buffer to the local hard drive
                            strLocal.Write(downBuffer, 0, bytesSize);
                            // Invoke the method that updates the form's label and progress bar
                            this.Invoke(new UpdateProgessCallback(this.UpdateProgress), new object[] { strLocal.Length, fileSize });
                        }*/


                    }
                }
                catch (WebException)
                {
                    Application.Exit();
                }
                finally
                {
                    long temp = 0;
                    try
                    {
                        // When the above code has ended, close the streams
                        strResponse.Close();
                        temp = strLocal.Length;
                        strLocal.Close();
                        webResponse.Close();
                    }
                    catch (SystemException)
                    {
                        Application.Exit();
                    }
                    //Rename the file to the correct name
                    if (size.CompareTo(temp) == 0 && size != 0)
                    {
                        File.Move((this.filename + ".incomplete"), this.filename);
                    }
                    else
                    {
                        File.Delete(this.filename + ".incomplete");
                    }
                    this.fileDownloaded = true;
                }
            }
        }
        string MD5SUM(byte[] FileOrText) //Output: String<-> Input: Byte[] //
        {
            return BitConverter.ToString(new MD5CryptoServiceProvider().ComputeHash(FileOrText)).Replace("-", "").ToLower();
        }
        // The progress of the download in percentage
        public void UpdateProgress(Int64 BytesRead, Int64 TotalBytes)
        {
            // Calculate the download progress in percentages
            PercentProgress = Convert.ToInt32((BytesRead * 100) / TotalBytes);
            // Make progress on the progress bar
            prgCurrent.Value = PercentProgress;
            // Display the current progress on the form
            lblCurrent.Text = "Current File: (" + this.filename + ") - " + "Downloaded " + BytesRead / 1048576 + " mb out of " + TotalBytes / 1048576 + " mb (" + PercentProgress + "%)";
        }
        //Gets the latest list of files and information
        public void getFileList()
        {
            try
            {

                WebClient Client = new WebClient();
                Stream strm = Client.OpenRead(path);
                xmlFileList = new XmlDocument();
                xmlFileList.Load(strm);

                //Create root Category node
                XmlNodeList xmlnode = xmlFileList.GetElementsByTagName("Category");
                for (int i = 0; i < xmlnode.Count; i++)
                {
                    for (int j = 0; j < xmlnode[i].ChildNodes.Count; j++)
                    {
                        XmlAttributeCollection fileInfo = xmlnode[i].ChildNodes[j].Attributes;
                        if (fileInfo[0].Value == "news")
                            continue;
                        file newFile = new file();
                        newFile.setSkin(fileInfo[0].Value); // is this file a skin?
                        newFile.setFileName(fileInfo[1].Value); //Set file name
                        newFile.setLocation(fileInfo[2].Value); //Set file location
                        newFile.setFolder(fileInfo[3].Value); //Set destination folder
                        newFile.setDescription(fileInfo[4].Value); //Set file description
                        newFile.setMd5sum(fileInfo[5].Value); //Set file md5sum

                        string pathToFile = "";
                        if (newFile.getFolder() == "Gamedata")
                        {
                            pathToFile = gamePath + "/" + newFile.getFileName(); ;
                        }
                        else
                        {
                            pathToFile = gamePath + "/" + newFile.getFolder() + "/" + newFile.getFileName();
                        }
                        this.fileList.Add(newFile);
                    }
                }
            }
            catch (SystemException) { }
        }
        public void getNews()
        {
            try
            {

                WebClient Client = new WebClient();
                Stream strm = Client.OpenRead(path);
                xmlFileList = new XmlDocument();
                xmlFileList.Load(strm);

                XmlNodeList xmlnode = xmlFileList.GetElementsByTagName("Category");
                for (int i = 0; i < xmlnode.Count; i++)
                {
                    for (int j = 0; j < xmlnode[i].ChildNodes.Count; j++)
                    {
                        XmlAttributeCollection newsInfo = xmlnode[i].ChildNodes[j].Attributes;
                        if (newsInfo[0].Value == "news")
                        {
                            for (int k = 0; k < xmlnode[i].ChildNodes[j].ChildNodes.Count; k++)
                            {
                                XmlAttributeCollection lineInfo = xmlnode[i].ChildNodes[j].ChildNodes[k].Attributes;
                                txtNews.AppendText(lineInfo[0].Value);
                                txtNews.Text = txtNews.Text + Environment.NewLine;
                            }
                        }
                    }
                }
            }
            catch (SystemException) { }
        }

        private void tmrStart_Tick(object sender, EventArgs e)
        {
            tmrStart.Stop();
            UpdateDownload();

            //Check for skins
            DialogResult dr = MessageBox.Show("Would you like to check for new skins now?",
                      "Skin Download", MessageBoxButtons.YesNo);
            switch (dr)
            {
                case DialogResult.Yes:
                    checkNewSkins();
                    break;
                case DialogResult.No:
                    break;
            }

            prgCurrent.Visible = false;
            prgTotal.Visible = false;
            lblCurrent.Visible = false;
            lblTotal.Visible = false;
            if (chkAuto.Checked == true)
            {
                btnLaunch.Visible = true;
                btnLaunch.Text = "Launching Legacy OJP...";
                btnLaunch.Enabled = false;

                //Start game
                Process p = new Process();
                p.StartInfo.WorkingDirectory = this.gamePath + @"\jke\";
                p.StartInfo.FileName = @"Play_JKE.bat";
                p.Start();
                this.Close();
            }
            btnLaunch.Visible = true;
        }

        public void checkNewSkins()
        {
            //Loop through the files
            int count = 0;
            for (int j = 0; j < fileList.Count(); j++)
            {
                //get skins here
                if (!fileList[j].isSkin())
                    continue;
                //Update total progress
                count++;
                prgTotal.Value = ((count * 100) / getDownloadTotal());
                lblTotal.Text = "Total Progress: " + ((count * 100) / getDownloadTotal()) + "%";
                Application.DoEvents();
                this.url = fileList[j].getLocation();
                this.filename = fileList[j].getFileName();
                this.folder = fileList[j].getFolder();
                string pathToFile = "";
                if (this.folder == "Gamedata")
                {
                    pathToFile = gamePath + "/" + this.filename;
                }
                else
                {
                    pathToFile = gamePath + "/" + this.folder + "/" + this.filename;
                }
                fileDownloaded = false;
                //If this file already exists don't download it again
                lblCurrent.Text = "Current File: (" + this.filename + ") Checking to server";
                Application.DoEvents();
                if (File.Exists(pathToFile))
                {
                    //Check if the file is the same as the one on the server
                    if (MD5SUM(File.ReadAllBytes(pathToFile)) == fileList[j].getMd5sum())
                    {//Ok, we have the right file, move along
                        continue;
                    }
                    //Get rid of the old/bad file so it can be updated
                    else
                    {
                        File.Delete(pathToFile);
                    }
                }
                //Open a thread for processing the download
                thrDownload = new Thread(Download);
                thrDownload.Name = this.filename + "Download";

                // Start the thread, and thus call Download()
                thrDownload.Start();
                //Wait until the file is downloaded
                while (!fileDownloaded) { Application.DoEvents(); }
                MoveToCorrectFolder(pathToFile);
            }
            //Reset everything
            prgTotal.Value = 0;
            prgCurrent.Value = 0;
            lblTotal.Text = "Total Progress";
            lblCurrent.Text = "Current File: Done";
        }

        public void UpdateDownload()
        {
            //Loop through the files
            int count = 0;
            for (int j = 0; j < fileList.Count(); j++)
            {
                //Don't get skins here
                if (fileList[j].isSkin())
                    continue;
                //Update total progress
                count++;
                prgTotal.Value = ((count * 100) / getDownloadTotal());
                lblTotal.Text = "Total Progress: " + ((count * 100) / getDownloadTotal()) + "%";
                Application.DoEvents();
                this.url = fileList[j].getLocation();
                this.filename = fileList[j].getFileName();
                this.folder = fileList[j].getFolder();
                string pathToFile = "";
                if (this.folder == "Gamedata")
                {
                    pathToFile = gamePath + "/" + this.filename;
                }
                else
                {
                    pathToFile = gamePath + "/" + this.folder + "/" + this.filename;
                }
                fileDownloaded = false;
                //If this file already exists don't download it again
                lblCurrent.Text = "Current File: (" + this.filename + ") Checking to server";
                Application.DoEvents();
                if (File.Exists(pathToFile))
                {
                    //Check if the file is the same as the one on the server
                    if (MD5SUM(File.ReadAllBytes(pathToFile)) == fileList[j].getMd5sum())
                    {//Ok, we have the right file, move along
                        continue;
                    }
                    //Get rid of the old/bad file so it can be updated
                    else
                    {
                        File.Delete(pathToFile);
                    }
                }
                //Open a thread for processing the download
                thrDownload = new Thread(Download);
                thrDownload.Name = this.filename + "Download";

                // Start the thread, and thus call Download()
                thrDownload.Start();
                //Wait until the file is downloaded
                while (!fileDownloaded) { Application.DoEvents(); }
                MoveToCorrectFolder(pathToFile);
            }
            //Reset everything
            prgTotal.Value = 0;
            prgCurrent.Value = 0;
            lblTotal.Text = "Total Progress";
            lblCurrent.Text = "Current File: Done";
        }

        private void chkAuto_CheckedChanged(object sender, EventArgs e)
        {
            if (chkAuto.Checked == true)
            {
                FileStream file = new FileStream("config.dat", FileMode.OpenOrCreate, FileAccess.Write);
                StreamWriter sw = new StreamWriter(file);
                sw.WriteLine(gamePath);
                sw.WriteLine("1");
                sw.Close();
                file.Close();
            }
            else
            {
                FileStream file = new FileStream("config.dat", FileMode.OpenOrCreate, FileAccess.Write);
                StreamWriter sw = new StreamWriter(file);
                sw.WriteLine(gamePath);
                sw.WriteLine("1");
                sw.Close();
                file.Close();
            }
        }

        private void btnLaunch_Click(object sender, EventArgs e)
        {
            //Start game
            Process p = new Process();
            p.StartInfo.WorkingDirectory = this.gamePath + @"\jke\";
            p.StartInfo.FileName = @"Play_JKE.bat";
            p.Start();
            this.Close();
        }

        private void Settings_Click(object sender, EventArgs e)
        {
            gamedataChange.Show();
        }


    }
    public class file
    {
        private string fileName;
        private string location;
        private string description;
        private string md5sum;
        private string destinationFolder;
        private bool skin;

        public file()
        {
            this.fileName = "";
            this.location = "";
            this.description = "";
            this.md5sum = "";
            this.destinationFolder = "";
            this.skin = false;
        }

        public string getFileName() { return this.fileName; }
        public string getLocation() { return this.location; }
        public string getDescription() { return this.description; }
        public string getMd5sum() { return this.md5sum; }
        public string getFolder() { return this.destinationFolder; }
        public bool isSkin() { return this.skin; }

        public void setFileName(string str) { this.fileName = str; }
        public void setLocation(string str) { this.location = str; }
        public void setDescription(string str) { this.description = str; }
        public void setMd5sum(string str) { this.md5sum = str; }
        public void setFolder(string str) { this.destinationFolder = str; }
        public void setSkin(string str) 
        {
            if (str == "true")
                this.skin = true;
            else
                this.skin = false;
        }
    };
}
