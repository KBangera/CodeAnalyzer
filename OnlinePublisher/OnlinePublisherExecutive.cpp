#include "OnlinePublisherExecutive.h"
/////////////////////////////////////////////////////////////////////////
// OnlinePublisher.cpp - Handles all the operations on the server      //
//                                                                     //
// Author: Karthik Bangera                                             //
/////////////////////////////////////////////////////////////////////////
/*
Package Operations:
==================
Handles all the operations on the server

Build Process:
==============
Required files
- CommunicationHandler
- Publisher
- HttpMessage
- MockChannel

Maintenance History:
====================
ver 1.1 : 29 April 2017
- added functions related to delete
ver 1.0 : 26 April 2017
*/

//Function which behaves like the main of OnlinePublisherExecutive
void OnlinePublisherExec::performAnalysis(std::string clientID, int argc, char* argv[])
{
	std::cout << "\n=================\n";
	std::cout << "Requirement 7:\n";
	std::cout << "=================\n";
	std::cout << "\nStarting publisher operations related to project 3\n";
	argv[0];
	PublisherExecutive exec;
	try {
		bool succeeded = exec.ProcessCommandLine(argc, argv);
		if (!succeeded) { return; }
		exec.setDisplayModes();
		exec.startLogger(std::cout);
		std::ostringstream tOut("CodeAnalysis - Version 2.0");
		Utilities::StringHelper::sTitle(tOut.str(), 3, 92, tOut, '=');
		Rslt::write(tOut.str());
		Rslt::write("\n     " + exec.systemTime());
		Rslt::flush();
		exec.showCommandLineArguments(argc, argv);
		Rslt::write("\n");
		exec.getSourceFiles();
		exec.complexityAnalysis();
		exec.dispatchOptionalDisplays();
		exec.flushLogger();
		Rslt::write("\n");
		std::ostringstream out;
		out << "\n  " << std::setw(10) << "searched" << std::setw(6) << exec.numDirs() << " dirs" << "\n  " << std::setw(10) << "processed" << std::setw(6) << exec.numFiles() << " files";
		Rslt::write(out.str() + "\n");
		exec.stopLogger();
		TypeTableAnal ta;
		Publisher publish;
		ta.doTypeTableAnal();
		ta.doGlobalFnAnal();
		Scanner::DepAnal da;
		da.setMapObject(ta.fetchBothTables());
		std::cout << "\n=================\n";
		std::cout << "Requirement 7:\n";
		std::cout << "=================\n";
		for each (File file in exec.cppImplemFiles())
			da.doDepAnal(file);
		for each (File file in exec.cppHeaderFiles())
			da.doDepAnal(file);
		da.insertInDb();
		std::string path = exec.getAnalysisPath() + "\\";
		std::unordered_map<Scanner::FileName, std::vector<Scanner::FileName >> depTable = da.fetchDepTable();
		std::vector<std::string> analysedFiles;
		std::string folder = argv[1] + std::string("\\htmlFolder");
		folder = FileSystem::Path::getFullFileSpec(folder);
		Files htmlFiles;
		for each (File file in exec.cppImplemFiles())
			analysedFiles.push_back(file);
		for each (File file in exec.cppHeaderFiles())
			analysedFiles.push_back(file);
		for each (File file in analysedFiles)
		{
			da.dependencyAnalysis(analysedFiles, file);
		}
		publish.setRepository(folder);
		publish.depTable(da.fetchDepTable());
		for each (File file in exec.cppImplemFiles())
		{
			htmlFiles.push_back(publish.htmlConvert("html", file));
		}
		for each (File file in exec.cppHeaderFiles())
		{
			htmlFiles.push_back(publish.htmlConvert("html", file));
		}
		publish.saveJavascriptCss();
		for each (File htmlFile in htmlFiles)
		{
			publish.replaceHTMLCharacters(htmlFile);
			publish.createHTML(htmlFile);
		}
		publish.generateHomePage();
	}
	catch (std::exception& except)
	{
		exec.flushLogger();
		std::cout << "\n\n  Exception caught: " + std::string(except.what()) + "\n\n";
		exec.stopLogger();
		return;
	}
	return;
}

//Function to add attribute
std::string OnlinePublisherExec::addAttribute(std::string tag, std::string value, std::string message)
{
	return message += "<" + tag + ">" + value + "</" + tag + ">";

}

//Function to fetch the attribute from the message
std::string OnlinePublisherExec::fetchMessageAttribute(std::string messageBody, std::string attribute)
{
	int sI = (messageBody.find("<" + attribute + ">") + (attribute.length() + 2));
	int eI = messageBody.find("</" + attribute + ">");
	return messageBody.substr(sI, eI - sI);
}

//Function to send message to client
void OnlinePublisherExec::sendClientMessage(std::string destinationPort, std::string messageBody, int messageCategory)
{
	Message message;
	switch (messageCategory)
	{
	case 1: 
		message = addAttribute("toPort", destinationPort, message);
		message = addAttribute("fromPort", std::to_string(receiverPort), message);
		message = addAttribute("command", "5", message);
		message = addAttribute("MSGTYPE", "text", message); 
		this->fetchCommHandler().postMessage(message);
		break;
	case 4:	
		downloadFileMsg(message, messageBody, destinationPort);
		notifyFileDownload(message, messageBody, destinationPort);	
		break;
	case 6: 
		message = addAttribute("toPort", destinationPort, message);
		message = addAttribute("fromPort", std::to_string(receiverPort), message);
		message = addAttribute("command", "6", message);
		message = addAttribute("catgs", fetchCategory(), message);
		message = addAttribute("MSGTYPE", "text", message);  
		this->fetchCommHandler().postMessage(message);
		break;
	case 7:
		message = addAttribute("toPort", destinationPort, message);
		message = addAttribute("fromPort", std::to_string(receiverPort), message);
		message = addAttribute("command", "7", message);
		message = addAttribute("catFileLst", fetchFiles(fetchMessageAttribute(messageBody, "whichCatg")), message);
		message = addAttribute("MSGTYPE", "text", message);  
		this->fetchCommHandler().postMessage(message);
		break;
	case 8:
		message = addAttribute("toPort", destinationPort, message);
		message = addAttribute("fromPort", std::to_string(receiverPort), message);
		message = addAttribute("command", "8", message);
		message = addAttribute("IISPHY", this->iisDirFolder, message);
		message = addAttribute("MSGTYPE", "text", message);
		this->fetchCommHandler().postMessage(message);
		break;
	case  3:
		break;
	default:
		break;
	}
}

//Function to fetch category
std::string OnlinePublisherExec::fetchCategory()
{
	std::vector<std::string> categoryDirectory = FileSystem::Directory::getDirectories(this->repository);
	std::string categoryString = "";
	for each (auto dir in categoryDirectory)
	{
		if (dir == "." || dir == ".." || dir == "htmlFolder")
			continue;
		categoryString += ":" + dir;
	}
	return categoryString;
}

//Function to fetch files
std::string OnlinePublisherExec::fetchFiles(std::string cat)
{
	std::string separateFiles= "";
	std::vector<std::string> serializedfiles = FileSystem::Directory::getFiles(this->iisDirFolder + "\\" + cat);
	for each (std::string serialFile in serializedfiles)
	{
		separateFiles += ":" + serialFile;
		if (FileSystem::File::exists(this->repository + "\\htmlFolder\\" + serialFile + ".html"))
		{
			separateFiles += ":" + serialFile + ".html";
		}
	}
	return separateFiles;
}
 
//Function to fetch repository
std::string OnlinePublisherExec::fetchRepository()
{
	return repository;
}

//Function to set repository
void OnlinePublisherExec::prepRepository(std::string repositoryPath)
{
	std::cout << "\n=================\n";
	std::cout << "Requirement 3:\n";
	std::cout << "=================\n";
	std::cout << "\nRepository program being initialized\n";
	repository = FileSystem::Path::getFullFileSpec(repositoryPath);
	repository = iisDirFolder;
}

//Function deletes a file mentioned in the message
void OnlinePublisherExec::deleteRepositoryFile(std::string message)
{
	std::string whichFile = fetchMessageAttribute(message, "whichFile");
	std::string category = fetchMessageAttribute(message, "whichCatg");;
	std::cout << "\nFile being deleted : " << whichFile << "\n";
	if (!searchDeleteRecursive(whichFile, this->iisDirFolder + "\\" + category))
	{
		//File not found
		std::cout << "\nFile could not be found" << whichFile << "\n";
	}
}

//Function to search for a file in the given folder and deletes it, also the folder should be fully qualified
bool OnlinePublisherExec::searchDeleteRecursive(std::string fileName, std::string folder)
{
	if (FileSystem::File::exists(folder + "\\" + fileName))
	{
		std::cout << "\n Deleting " << folder + "\\" + fileName << "\n";
		FileSystem::File::remove(folder + "\\" + fileName);
		return true;
	}
	std::vector<std::string> folders = FileSystem::Directory::getDirectories(folder);
	for each (auto fold in folders)
	{
		if (fold != "." && fold != "..")
		{
			return searchDeleteRecursive(fileName, folder + "\\" + fold);
		}

	}
	return false;
}

//Function that downloads the mentioned file
void OnlinePublisherExec::downloadFileMsg(Message msg, Message message, std::string destinationPort)
{
	msg = addAttribute("toPort", destinationPort, msg);
	msg = addAttribute("fromPort", std::to_string(receiverPort), msg);
	msg = addAttribute("command", "4", msg);
	msg = addAttribute("catg", this->fetchMessageAttribute(message, "catg"), msg);
	std::string file = this->fetchMessageAttribute(message, "whichFile");
	std::string filePath;
	if (file.find(".html") != std::string::npos)
	{
		filePath = this->iisDirFolder + "\\" + "htmlFolder" + "\\" + file;
	}
	else
	{
		filePath = this->iisDirFolder + "\\" + fetchMessageAttribute(message, "catg") + "\\" + file;
	}
	if (!FileSystem::File::exists(filePath))
	{
		return;
	}
	msg = addAttribute("filePath", filePath, msg);
	msg = addAttribute("MSGTYPE", "fPth", msg);
	this->fetchCommHandler().postMessage(msg);
}

//Function to notify file download
void OnlinePublisherExec::notifyFileDownload(Message msg, Message message, std::string destinationPort)
{
	msg = addAttribute("toPort", destinationPort, msg);
	msg = addAttribute("fromPort", std::to_string(receiverPort), msg);
	msg = addAttribute("command", "9", msg);
	msg = addAttribute("MSGTYPE", "text", msg);
	this->fetchCommHandler().postMessage(msg);

}

//Function to fetch arguments
void OnlinePublisherExec::prepArguments(int argc, char* argv[])
{
	this->iisDirFolder = FileSystem::Path::getFullFileSpec(argv[1]);
	this->prepRepository("..\\New");
	this->receiverPort = std::stoi(argv[6]);
}

//Function to fetch the Communication handler
CommHandler& OnlinePublisherExec::fetchCommHandler()
{
	return this->shim;
}

//Function to check if message received in string format
void OnlinePublisherExec::processMessage(Message msg, int argc, char* argv[])
{
	std::string command = fetchMessageAttribute(msg, "command"); 
	std::string clientId = fetchMessageAttribute(msg, "clntID");
	std::string sourcePort = fetchMessageAttribute(msg, "fromPort");
	std::string destinationPort = fetchMessageAttribute(msg, "toPort");
	switch (std::stoi(command))
	{
	case 1:
		performAnalysis(clientId, argc, argv); 
		this->sendClientMessage(sourcePort, msg, 1); 
		break;
	case 2:
		this->sendClientMessage(sourcePort, msg, 6); 
		break;
	case 3:
		deleteRepositoryFile(msg);
		this->sendClientMessage(sourcePort, msg, 7);
		break;
	case 4:
		this->sendClientMessage(sourcePort, msg, 4);
		break;
	case 7:
		this->sendClientMessage(sourcePort, msg, 7);
		break;
	case 8:
		this->sendClientMessage(sourcePort, msg, 8);
		break;
	default: 
		break;
	}
}

//Function for the window loaded counterpart
void OnlinePublisherExec::loadClass(int argc, char* argv[]) 
{
	this->shim = CommHandler(std::stoi(argv[6]), argv[1]);
	receiverThread = std::thread(
		[this, argc, argv] {
		while (true)
		{
			Message receivedMessage = fetchCommHandler().fetchMessage(); 
			processMessage(receivedMessage, argc, argv);
		}
	});
}

OnlinePublisherExec::OnlinePublisherExec()
{

}

OnlinePublisherExec::~OnlinePublisherExec()
{
}

int main(int argc, char* argv[])
{
	OnlinePublisherExec exec;
	exec.prepArguments(argc, argv);
	exec.loadClass(argc, argv); 
	int a = 5;
	while (true)
	{

	}
	return 0;
}