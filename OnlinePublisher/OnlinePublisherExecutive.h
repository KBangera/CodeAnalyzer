#pragma once
/////////////////////////////////////////////////////////////////////////
// OnlinePublisher.h - Handles all the operations on the server        //
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
#include "../CommunicationHandler/CommHandler.h"
#include "../Publisher/PublisherExecutive.h"
#include "../HttpMessage/HttpMessage.h"
#include <thread>
#include <iostream>
#include <string>
#include <time.h>
#include<vector>
#include<ctime>
#include<exception>

using namespace Logging;
using namespace CodeAnalysis;
class OnlinePublisherExec //like the windowmain class
{
	using File = std::string;
	using Files = std::vector<File>;
private:
	CommHandler shim;
	int receiverPort;
	std::thread receiverThread;
	std::thread processThread;
	std::string repository;
	std::string iisDirFolder;
	std::string addAttribute(std::string message, std::string v1, std::string v2);
	std::string fetchMessageAttribute(std::string message, std::string attribute);
	std::string fetchCategory();
	std::string fetchFiles(std::string category);
	void performAnalysis(std::string clientId, int argc, char* argv[]);
	void sendClientMessage(std::string destinationPort, std::string messageBody, int messageCategory);
	void deleteRepositoryFile(std::string message);
	bool searchDeleteRecursive(std::string fileName, std::string folder);
	void downloadFileMsg(Message msg, Message message, std::string destinationPort);
	void notifyFileDownload(Message msg, Message message, std::string destinationPort);
public:
	std::string fetchRepository();
	void prepRepository(std::string repositoryPath);
	CommHandler& fetchCommHandler();
	void processMessage(Message message, int argc, char* argv[]);
	void prepArguments(int argc, char* argv[]);
	void loadClass(int argc, char* argv[]);
	OnlinePublisherExec();
	~OnlinePublisherExec();
};

