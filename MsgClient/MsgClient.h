#pragma once
#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <iostream>
#include <thread>


using Show = Logging::StaticLogger<1>;
using namespace Utilities;
using Utils = StringHelper;

/////////////////////////////////////////////////////////////////////
// ClientCounter creates a sequential number for each client
//
/*
class ClientCounter
{
public:
	ClientCounter() { ++clientCount; }
	size_t count() { return clientCount; }
private:
	static size_t clientCount;
};
size_t ClientCounter::clientCount = 0;
*/
/////////////////////////////////////////////////////////////////////
// MsgClient class
// - was created as a class so more than one instance could be 
//   run on child thread
//
class MsgClient
{
public:
	using EndPoint = std::string;
	void execute(const size_t TimeBetweenMessages, const size_t NumMessages);
	void fetchClientId(EndPoint clientId);
	void fetchEndPoint(EndPoint endPoint);
	void sendTextMessage(std::string messageType, std::string messageBody, Socket& socket);
	void sendFileMessage(std::string messageBody, Socket& socket);
private:
	EndPoint endPoint;
	EndPoint clientId;
	bool sendFile(const std::string& fqname, Socket& socket);
	HttpMessage makeMessage(size_t n, const std::string& messageBody, const EndPoint& endPoint);
	std::string searchAttributes(std::string messageBody, std::string attribute);
	void sendMessage(HttpMessage& message, Socket& socket);
	
};
