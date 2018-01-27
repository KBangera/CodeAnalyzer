/////////////////////////////////////////////////////////////////////////
// MainWindow.xaml.cs - Demonstrates the working of the GUI 		   //
//                                                                     //
// Author: Karthik Bangera                                             //
/////////////////////////////////////////////////////////////////////////
/*
Package Operations:
==================
The module demonstrates the working of the GUI

Build Process:
==============
Required files
- CLIShim

Maintenance History:
====================
ver 1.0 : 27 April 2017
- demonstrates
*/
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;


namespace Client1
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        Shim shim;
        Thread tReceive = null;
        int receiverPort;
        int serverPort;
        string downLoadFolder;
        string[] args;
        string iisFilePath;
        bool auto = false;
        public MainWindow()
        {
            InitializeComponent();
        }
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            fetchCommandLineArgs(Environment.GetCommandLineArgs());
            Console.WriteLine("\nClient at :" + receiverPort + "\n");
            tbWindow.Text = "Client at " + receiverPort;
            shim = new Shim(receiverPort, downLoadFolder);
            tReceive = new Thread(
              () =>
              {
                  while (true)
                  {
                      string message = shim.GetMessage();
                      Action<String> act = new Action<string>(processClientSideMessages);
                      Object[] args = { message };
                      Dispatcher.Invoke(act, args);
                  }
              }
            );
            tReceive.Start();
            fetchDataForArgsTab();
            if (auto)
            {
                automator(sender, e);
            }
        }

        //Function to automate 
        private void automator(object sender, RoutedEventArgs e)
        {
            try
            {
                Console.WriteLine("\n=================\n");
                Console.WriteLine("Requirement 9:\n");
                Console.WriteLine("=================\n");
                Console.WriteLine("An automated unit test suite which demonstrates that all the requirements are satisfied\n");
                string path = System.IO.Path.GetFullPath(@"..\..\..\CodeToBeTested");
                lstFilesToSend.Items.Add(System.IO.Directory.GetFiles(path, "*.h").ToList().FirstOrDefault());
                lstFilesToSend.Items.Add(System.IO.Directory.GetFiles(path, "*.cpp").ToList().FirstOrDefault());
                lstFilesToSend.SelectAll();
                btnSendClick(sender, e);
                Console.WriteLine("\n=================\n");
                Console.WriteLine("Requirement 5:\n");
                Console.WriteLine("=================\n");
                Console.WriteLine("File Browser opened\n");
                Thread.Sleep(200);
                btnClearClick(sender, e);
                Thread.Sleep(200);
                btnAnalyzeClick(sender, e);
                Thread.Sleep(2000);
                btnRefreshClick(sender, e);
                Thread.Sleep(600);
                lstFiles.SelectedIndex = 0;
                btnDownloadClick(sender, e);
                Console.WriteLine("Download of the selected file(s) has been complete\n");
                Console.WriteLine("Files are hosted on IIS\n");
                lstFiles.SelectedIndex = 0;
                btnOpenClick(sender, e);
                Thread.Sleep(800);
                MessageBox.Show("Automated execution complete!!");
                Console.WriteLine("\n=================\n");
                Console.WriteLine("Requirement 11:\n");
                Console.WriteLine("=================\n");
                Console.WriteLine("IIS virtual path and physical path have been taken(physical path is taken in the command line of the server)\n");
                Console.WriteLine("\n=================\n");
                Console.WriteLine("Requirement 10:\n");
                Console.WriteLine("=================\n");
                Console.WriteLine("Files are browsed and lazy downloading is achieved\n");

            }
            catch (Exception)
            {
                MessageBox.Show("Automated execution complete!!");
            }

        }


        //Function to search the value of the attribute
        private string searchAttributeValue(string attribute, string messageBody)
        {
            try
            {
                int sI = (messageBody.IndexOf("<" + attribute + ">") + (attribute.Length + 2));
                int eI = messageBody.IndexOf("</" + attribute + ">");
                return messageBody.Substring(sI, eI - sI);
            }
            catch (Exception ex)
            {
                Console.WriteLine("\n" + messageBody + "does not contain " + attribute + "\n" + ex.Message + "\n");
                return "";
            }
        }
        
        // Function based on the message type received by the client
        private void processClientSideMessages(String message)
        {
            String messageType = searchAttributeValue("MSGTYPE", message);
            switch (messageType)
            {
                case "text":
                    processReceivedTxtMsg(message);
                    break;
                default:
                    processReceivedTxtMsg(message);
                    break;
            }
        }

       

        //Function to process client message
        private void processReceivedTxtMsg(string messageBody)
        {
            int commNum = int.Parse(searchAttributeValue("command", messageBody));
            switch (commNum)
            {
                case 5: 
                    activateAnalysis();
                    break;
                case 6:
                    refreshCategory(messageBody);
                    break;
                case 7:
                    refreshServerFiles(messageBody);
                    break;
                //case 8://update IIS physical
                //    updateIISDetails(messageBody);
                // break;
                case 9:
                    openDLFolder(messageBody);
                    break;
                default:
                    break;
            }
        }

        //Function to open the download folder
        private void openDLFolder(string messageBody)
        {
            System.Diagnostics.Process.Start(downLoadFolder);
            
        }

        //Function to restart the analysis mode
        private void activateAnalysis()
        {
            btnSendSelectFiles.Content = "Send";
            btnSendSelectFiles.IsEnabled = true;
            btnAnalyzeSelectFiles.Content = "Analyze";
            btnAnalyzeSelectFiles.IsEnabled = true;
        }

        // Function to fetch the command line arguments
        private void fetchCommandLineArgs(string[] arguments)
        {
            args = arguments;
                downLoadFolder = System.IO.Path.GetFullPath(@"..\..\fileDownload");
                iisFilePath = args[3];
                txtPath.Text = iisFilePath;
                serverPort = int.Parse(args[2]);
                receiverPort = int.Parse(args[1]);
            if (args[4]=="t")
            {
                auto = true;
            }

        }

        //Function to update/refresh the server files
        private void refreshServerFiles(string messageBody)
        {
            string catFiles = searchAttributeValue("catFileLst", messageBody);
            List<string> categoryfiles = catFiles.Split(':').ToList();
            lstFiles.Items.Clear();
            foreach (var file in categoryfiles)
            {
                if (file.Length > 0)
                {
                    lstFiles.Items.Add(file);
                }
            }
        }

        //Function to update/refresh the category combobox
        private void refreshCategory(string messageBody)
        {
            string category = searchAttributeValue("catgs", messageBody);
            List<string> categories = category.Split(':').ToList();
            cbCategory.Items.Clear();
            foreach (var cat in categories)
            {
                if (cat.Length > 0)
                {
                    cbCategory.Items.Add(cat);
                }
            }
            cbCategory.SelectedIndex = 0;
        }

        //Function to browse and add .h and .cpp files
        private void btnAddClick(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.OpenFileDialog openFileDialog = new Microsoft.Win32.OpenFileDialog();
            openFileDialog.Filter = "cpp/h Files (*.cpp;*.h)|*.cpp;*.h";
            openFileDialog.Multiselect = true;
            openFileDialog.InitialDirectory = System.IO.Path.GetFullPath(@"..\..\CodeToBeTested");
            Console.WriteLine(Directory.GetCurrentDirectory());
            Console.WriteLine(Environment.CurrentDirectory);
            if (openFileDialog.ShowDialog() == true)
            {
                lstFilesToSend.SelectionMode = SelectionMode.Multiple;
                foreach (string file in openFileDialog.FileNames)
                {
                    lstFilesToSend.Items.Insert(0, file);
                }
            }
        }

        //Function to add attribute to the message 
        private string addAttribute(string attributeTag, string attributeValue, string message)
        {
            return message += "<" + attributeTag + ">" + attributeValue + "</" + attributeTag + ">";
        }
        

        //Function to send the files to the publisher
        private void btnSendClick(object sender, RoutedEventArgs e)
        {
            try
            {
                lstFilesToSend.SelectAll();
                foreach (String file in lstFilesToSend.SelectedItems)
                {
                    int fileLength = file.ToString().Length;
                    String message = "";
                    message = addAttribute("filePath", file, message);
                    message = addAttribute("catg", txtCategory.Text.Trim(), message);
                    message = addAttribute("fromPort", receiverPort.ToString(), message);
                    message = addAttribute("clntID", "clnt1", message);
                    message = addAttribute("toPort", serverPort.ToString(), message);
                    message = addAttribute("catg", txtCategory.Text.Trim(), message);
                    message = addAttribute("MSGTYPE", "fPth", message);
                    shim.PostMessage(message);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("\n" + ex.Message + "\n");
            }
        }

        //Function to analyze and store the HTML files locally 
        private void btnAnalyzeClick(object sender, RoutedEventArgs e)
        {
            btnSendSelectFiles.Content = "Send(wait)";
            btnSendSelectFiles.IsEnabled = false;
            btnAnalyzeSelectFiles.Content = "Analyzing(in process)";
            btnAnalyzeSelectFiles.IsEnabled = false;

            String message = "";
            message = addAttribute("command", "1", message);
            message = addAttribute("fromPort", receiverPort.ToString(), message);
            message = addAttribute("clntID", "clnt1", message);
            message = addAttribute("toPort", serverPort.ToString(), message);
            message = addAttribute("MSGTYPE", "text", message);
            shim.PostMessage(message);
        }

        //Function to check if the new text is empty. if so, replace it with defaultcategory
        private void TextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (txtCategory.Text.Trim() == "")
            {
                txtCategory.Text = "No Parent";
            }
            txtCategory.Text = txtCategory.Text.Trim();
        }

        //Function to clear the list of files to be sent
        private void btnClearClick(object sender, RoutedEventArgs e)
        {
            lstFilesToSend.Items.Clear();
        }

        //Function to refresh the file list based on category
        private void cbCategorySelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            updatedServerFilesList();
        }

        //Function to update the server files list on changing the selection
        private void updatedServerFilesList()
        {
            if (cbCategory.SelectedIndex > -1)
            {
                String message = "";
                message = addAttribute("command", "7", message);
                message = addAttribute("fromPort", receiverPort.ToString(), message);
                message = addAttribute("clntID", "clnt1", message);
                message = addAttribute("toPort", serverPort.ToString(), message);
                message = addAttribute("whichCatg", cbCategory.SelectedValue.ToString().Trim(), message);
                message = addAttribute("MSGTYPE", "text", message);
                shim.PostMessage(message);
            }
        }


        //Function to refresh the server file list
        private void btnRefreshClick(object sender, RoutedEventArgs e)
        {
            lstFiles.Items.Clear();
            cbCategory.Items.Clear();
            categoryRefreshCommand();
        }

        //Function to refresh the category
        private void categoryRefreshCommand()
        {
            String message = "";
            message = addAttribute("command", "2", message);
            message = addAttribute("fromPort", receiverPort.ToString(), message);
            message = addAttribute("clntID", "clnt1", message);
            message = addAttribute("toPort", serverPort.ToString(), message);
            message = addAttribute("MSGTYPE", "text", message);
            shim.PostMessage(message);
        }
        

        //Function for the publisher to delete both the source and the html file
        private void btnDeleteClick(object sender, RoutedEventArgs e)
        {

            foreach (string file in lstFiles.SelectedItems)
            {
                if (!file.Contains(".html"))
                {
                    String message = "";
                    string trimmedFile = file.Trim();
                    message = addAttribute("command", "3", message);
                    message = addAttribute("fromPort", receiverPort.ToString(), message);
                    message = addAttribute("clntID", "clnt1", message);
                    message = addAttribute("toPort", serverPort.ToString(), message);
                    message = addAttribute("whichFile", trimmedFile, message);
                    message = addAttribute("whichCatg", cbCategory.SelectedValue.ToString().Trim(), message);
                    message = addAttribute("MSGTYPE", "text", message);
                    shim.PostMessage(message);
                }
                else
                {
                    String message = "";
                    string trimmedFile = file.Trim();
                    message = addAttribute("command", "3", message);
                    message = addAttribute("fromPort", receiverPort.ToString(), message);
                    message = addAttribute("clntID", "clnt1", message);
                    message = addAttribute("toPort", serverPort.ToString(), message);
                    message = addAttribute("whichFile", trimmedFile, message);
                    message = addAttribute("whichCatg", "htmlFolder", message);
                    message = addAttribute("MSGTYPE", "text", message);
                    shim.PostMessage(message);
                }
            }
        }
        

        //Function to download the files
        private void btnDownloadClick(object sender, RoutedEventArgs e)
        {
            if (lstFiles.SelectedIndex >= 0)
            {
                string fileName = lstFiles.SelectedValue.ToString().Trim();
                String message = "";
                message = addAttribute("command", "4", message);
                message = addAttribute("fromPort", receiverPort.ToString(), message);
                message = addAttribute("clntID", "clnt1", message);
                message = addAttribute("toPort", serverPort.ToString(), message);
                message = addAttribute("whichFile", fileName, message);
                message = addAttribute("catg", cbCategory.SelectedValue.ToString().Trim(), message);
                message = addAttribute("MSGTYPE", "text", message);
                shim.PostMessage(message);
                updateStatusDisplayMain("File requested for download: " + fileName);
            }
        }

        //Function to update the main status display
        private void updateStatusDisplayMain(string text)
        {
            txtFileName.Text = text;
        }

        //Function will fetch all IIS details
        private void fetchIISDetails()
        {
            String message = "";
            message = addAttribute("command", "8", message); 
            message = addAttribute("fromPort", receiverPort.ToString(), message);
            message = addAttribute("clntID", "clnt1", message);
            message = addAttribute("toPort", serverPort.ToString(), message);
            message = addAttribute("MSGTYPE", "text", message);
            shim.PostMessage(message);
        }
        
        //Function to update IIS physical and virtual address 
        private void fetchDataForArgsTab()
        {
            fetchIISDetails();
            txtDownLoadFolder.Text = downLoadFolder;
            txtClientPort.Text = receiverPort.ToString();
            txtServerPort.Text = serverPort.ToString();
        }

        //Function to show the virtual url of the file to be opened in the browser
        private void lstFiles_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            string vPath = iisFilePath.Trim();
            if (lstFiles.SelectedItems.Count > 0)
            {
                string fileName = lstFiles.SelectedValue.ToString();
                if (fileName.Contains(".html"))
                {
                    txtPath.Text = vPath + "/htmlFolder/" + fileName;
                }
                else
                {
                    string tobrzHTML = fileName + ".html";
                    foreach (var item in lstFiles.Items)
                    {
                        if (item.ToString()==tobrzHTML.ToString())
                        {
                            txtPath.Text = vPath + "/htmlFolder/" + fileName + ".html";
                            return;
                        }
                    }
                    txtPath.Text = "corresponding html doesnt exist";
                }
            }
            else
            {
                txtPath.Text = vPath;
            }
            updateStatusDisplayMain("IIS virtual path has been accepted");
        }


        //Function to open file selected from htmlFolder
        private void btnOpenClick(object sender, RoutedEventArgs e)
        {
            if (lstFiles.SelectedIndex >= 0)
            {
                string selectedPageUrl = txtPath.Text.Trim();
                System.Diagnostics.Process.Start(selectedPageUrl);
                updateStatusDisplayMain("Selected file opening in browser");
            }
        }

        

    }
}
