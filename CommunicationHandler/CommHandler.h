#pragma once
///////////////////////////////////////////////////////////////////////////////////////
// CommHandler.h - Behaves like an intermediate between Online Publisher and         //
//                 communication channel                                             //
// Author: Karthik Bangera														     //
///////////////////////////////////////////////////////////////////////////////////////
//Maintenance History :
//ver 1.0 : 27 April 2017
#include "../MockChannel/MockChannel.h"
#include "../FileSystem/FileSystem.h"


class CommHandler
{
public:
	using Message = std::string;
	Message fetchMessage();
	CommHandler();
	~CommHandler();
	CommHandler(int receiverPort, std::string downLoadFolder);
	void postMessage(Message msg);

private:
	IMockChannel* pMockChannel;
	ISendr* pSender;
	IRecvr* pReceiver;
	void folderCreation(std::string folder);


};

