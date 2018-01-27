Server’s command line :
------------------------
- Ex: “F:\IIS\CodePublisher *.h *.cpp  /f /r ”
- Argv[1] is the physical path of the IIS web application’s virtual path 
- Argv[6] is the server's port, given in the client’s command line
- Argv[1] will also be used to store html files by the client, so if you try to delete anything in it manually(from outside the application), you may not be able to.
- The program will create some folders on the IIS folder given by you, which can’t be deleted unless the program is stopped.


Client’s command line :
------------------------
- Ex: “8080 8082 http://localhost:8090/codePublisher t”
- Ex: “8080 8082 http://localhost:8090/codePublisher f”
- Argv[1]== port of the client
- Argv[2]== the virtual path of the IIS server
- Argv[3]== will decide if the automated test executive needs to be run or not
- When set to ‘t’, it runs a test executive that shows most of the requirements(5-10 seconds of completion)
- When set to “f”, you will have to use the GUI as mentioned in the steps below
- Run one client in 't' mode


GUI :
-----
- Client:
- When running in "t", it will display most of the requirements in the console.
- When running in "f",  use the GUI.
- Server:
- It will run in console.

Manual Steps in Client GUI:
---------------------------
Steps in ‘f’ mode:(Manual mode)
- Click on ‘Add Selected File(s)’ and select the files (multi select)
- Click on ‘Send Selected File(s)’ -> sends files to server-> Click on ‘Analyze Selected File(s)’-> publishes it-> stores in IIS physical path
- Click on ‘Clear File(s) List’ to clear the list of files selected
- When the files are sent or the analysis is done, you will be notified with a text message on the bottom left side of the window.
- Click on 'Refresh Category files list'. It displays the files of the server below the category dropdown.
- Then click on any file and click ‘Download File’->file will be downloaded and the folder where it's been downloaded will be opened.
- You can click on any file and click ‘Open Page’->file will be opened in the browser without been downloaded.
- Then click ‘delete’ it will delete the selected file. Files other than ‘.html’ may not be deleted(might be due to an error in the helper code).
- The 'File Virtual Path' will display the virtual IIS server path of the html files selected(and if a .h or .cpp file is selected, it will display the path of the .html file of the page if present in the serber)
- The 'Download File' button will also create a fileDownload folder in the Client1 folder where the files would be downloaded, the path is displayed in the botton right corner as the Download Folder.
