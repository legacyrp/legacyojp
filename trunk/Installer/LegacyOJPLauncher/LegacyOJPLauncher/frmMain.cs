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

namespace LegacyOJPLauncher
{
    public partial class frmMain : Form
    {
        //Variables
        private XmlDocument xmlFileList;
        string path = "http://legacyrp.com/test.xml";
        public bool gamePathSet = false;
        public string gamePath = "";

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
        public frmMain(bool gamePathSet, string gamepath)
        {
            InitializeComponent();
            this.gamePath = gamepath;
            this.gamePathSet = gamePathSet;
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
                    strLocal = new FileStream((this.filename + ".incomplete"), FileMode.Create, FileAccess.Write, FileShare.None);


                    // Loop through the buffer until the buffer is empty
                    while ((bytesSize = strResponse.Read(downBuffer, 0, downBuffer.Length)) > 0)
                    {
                        // Write the data from the buffer to the local hard drive
                        strLocal.Write(downBuffer, 0, bytesSize);
                        // Invoke the method that updates the form's label and progress bar
                        this.Invoke(new UpdateProgessCallback(this.UpdateProgress), new object[] { strLocal.Length, fileSize });
                    }
                }
                catch (WebException)
                {
                    MessageBox.Show("Error: Unable to access file source");
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
                    }
                    catch (SystemException)
                    {
                        //  Application.Exit();
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
            lblCurrent.Text = "Current File: (" + this.filename + ") - " + "Downloaded " + BytesRead + " out of " + TotalBytes + " (" + PercentProgress + "%)";
        }
    }
}
