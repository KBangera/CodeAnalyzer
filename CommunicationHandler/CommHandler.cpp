#pragma once
///////////////////////////////////////////////////////////////////////////////////////
// CommHandler.cpp - Behaves like an intermediate between Online Publisher and       //
//                 communication channel                                             //
// Author: Karthik Bangera														     //
///////////////////////////////////////////////////////////////////////////////////////
//Maintenance History :
//ver 1.0 : 27 April 2017
#include "CommHandler.h"


//Function fetches the message
Message CommHandler::fetchMessage()
{
	return pReceiver->getMessage();
}

CommHandler::CommHandler()
{
}

CommHandler::~CommHandler()
{
}

//Parameterized constructor
CommHandler::CommHandler(int receiverPort, std::string downLoadFolder)
{
	folderCreation(FileSystem::Path::getFullFileSpec(downLoadFolder));
	ObjectFactory factory;
	pSender = factory.createSendr();
	pReceiver = factory.createRecvr();
	pMockChannel = factory.createMockChannel(pSender, pReceiver, receiverPort, downLoadFolder);
	pMockChannel->start();
}

//Function posts message
void CommHandler::postMessage(Message message)
{
	pSender->postMessage(message);
}

//Function creates folder
void CommHandler::folderCreation(std::string destinationPath)
{
	size_t pos = destinationPath.find('\\');
	while (pos != std::string::npos)
	{
		if (!FileSystem::Directory::exists(destinationPath.substr(0, pos)))
			FileSystem::Directory::create(destinationPath.substr(0, pos));
		pos = destinationPath.find('\\', pos + 1);
	}
	if (destinationPath[destinationPath.size() - 1] != '\\')
	{
		if (!FileSystem::Directory::exists(destinationPath))
			FileSystem::Directory::create(destinationPath);
	}
}

#ifdef TEST_PUBCOMMHANDLER
int main()
{
	return 0;
}
#endif // TEST_PUBCOMMHANDLER


