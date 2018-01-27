///////////////////////////////////////////////////////////////////////////////////////
// MockChannel.cpp - Demo for CSE687 Project #4, Spring 2015                         //
// - build as static library showing how C++\CLI client can use native code channel  //
// - MockChannel reads from sendQ and writes to recvQ                                //
// Author: Karthik Bangera                                                           //
// Source: Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015                 //
///////////////////////////////////////////////////////////////////////////////////////

#define IN_DLL
#include "MockChannel.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include <string>
#include <thread>
#include <iostream>
#include "../MsgClient/MsgClient.h"
#include "../MsgServer/MsgServer.h"

using namespace Async;
using BQueue = BlockingQueue < Message >;

/////////////////////////////////////////////////////////////////////////////
// Sender class
// - accepts messages from client for consumption by MockChannel
//
class Sender : public ISendr
{
public:
  void postMessage(const Message& msg);
  BQueue& queue();
private:
  BQueue sendQ_;
};

void Sender::postMessage(const Message& msg)
{
  sendQ_.enQ(msg);
}

BQueue& Sender::queue() { return sendQ_; }

/////////////////////////////////////////////////////////////////////////////
// Recvr class
// - accepts messages from MockChanel for consumption by client
//
class Receiver : public IRecvr
{
public:
  Message getMessage();
  BQueue& queue();
  Receiver();
  Receiver(int receiverPort);
private:
  BQueue recvQ_;
  int receiverPort_;
};

Message Receiver::getMessage()
{
  return recvQ_.deQ();
}

BQueue& Receiver::queue()
{
  return recvQ_;
}
Receiver::Receiver()
{
}
Receiver::Receiver(int rcvPort)
{
}
/////////////////////////////////////////////////////////////////////////////
// MockChannel class
// - reads messages from Sendr and writes messages to Recvr
//
class MockChannel : public IMockChannel
{
public:
  MockChannel(ISendr* pSendr, IRecvr* pRecvr,int recvAddr,std::string downLoadFolder);
  void start();
  void stop();
private:
  bool stop_ = false;
  ISendr* pISender_;
  IRecvr* pIReceiver_;	
  int  receiverPort;
  std::string downloadFolder;
  std::thread senderThread_;
  std::thread receiverThread_;
  void senderThread();
  void receiverThread();
  std::string fetchAttributes(std::string msg, std::string attribute);
};

//----< pass pointers to Sender and Receiver >-------------------------------

MockChannel::MockChannel(ISendr* pSender, IRecvr* pReceiver, int receiverPort, std::string downLoadFolder) : pISender_(pSender), pIReceiver_(pReceiver)
{
	this->receiverPort = receiverPort;
	this->downloadFolder = downLoadFolder;
}


//---<creates and starts the sending thread>-------
void MockChannel::senderThread()
{
	std::cout <<"\nStarting up the Mock Channel Sender\n";
	senderThread_ = std::thread(
	[this] {
	Receiver* pReceiver = dynamic_cast<Receiver*>(pIReceiver_);
	Sender* pSender = dynamic_cast<Sender*>(pISender_);
	if (pReceiver == nullptr || pSender == nullptr)
	{
			std::cout <<"\nMock Channel failed to start\n";
			return;
	}
	BQueue& receiverQ = pReceiver->queue();
	BQueue& senderQ = pSender->queue();
	while (!stop_)
	{
			std::cout << "\n  Channel dequeing message";
			Message message = senderQ.deQ();
			MsgClient cMsg;
			std::string messageType = fetchAttributes(message, "MSGTYPE");
			std::string clientId = fetchAttributes(message, "clntID");
			std::string sourcePort = fetchAttributes(message, "fromPort");
			std::string destinationPort= fetchAttributes(message, "toPort");
			cMsg.fetchClientId(clientId);
			cMsg.fetchEndPoint(sourcePort);
			std::cout <<"Starting HttpMessage Client" + clientId +" on Thread with Id" + Utilities::Converter<std::thread::id>::toString(std::this_thread::get_id());
			try
			{
				SocketSystem sSys;
				SocketConnecter sConn;
				/*if (messageType == "fPath"|| messageType == "text")
				{
					while (!sConn.connect("localhost", std::stoi(destinationPort)))
					{
						std::cout << "\n Client is waiting to connect";
						::Sleep(100);
					}
					if (messageType == "fPath") {
						cMsg.sendFileMessage(message, sConn);
					}
					if (messageType == "text")
					{
						cMsg.sendTextMessage(messageType, message, sConn);
					}
				}	*/

				if (messageType == "text")
				{
					while (!sConn.connect("localhost", std::stoi(destinationPort))) //this(2nd parameter) is where  the message seems to go
					{
						std::cout << "\n client waiting to connect";
						::Sleep(100);
					}
					cMsg.sendTextMessage(messageType, message, sConn);
				}
				if (messageType == "fPth") // filePath
				{
					while (!sConn.connect("localhost", std::stoi(destinationPort))) //this(2nd parameter) is where  the message seems to go
					{
						std::cout << "\n client waiting to connect";
						::Sleep(100);
					}
					cMsg.sendFileMessage(message, sConn);
				}
			}
			catch (const std::exception& exc)
			{
				Show::write("\n Exception : ");
				std::string exMsg = "\n" + std::string(exc.what()) + "\n";
				Show::write(exMsg);
			}
		}
		std::cout << "\nMock Channel Sender\n";
	});

}
//---<creates and starts mockchannel reciever thread>--
void MockChannel::receiverThread()
{
	std::string downloadFolder = this->downloadFolder;
	std::cout << "\n  Starting up the Mock Channel Receiver\n";
	receiverThread_ = std::thread(
		[this, downloadFolder] {
		Receiver* pReceiver = dynamic_cast<Receiver*>(pIReceiver_);
		Sender* pSender = dynamic_cast<Sender*>(pISender_);
		if (pReceiver == nullptr || pSender == nullptr)
		{
			std::cout << "\nMock Channel failed to start\n";
			return;
		}
		BlockingQueue<HttpMessage> messageQ;
		BQueue& receiverQ = pReceiver->queue();
		try
		{
			SocketSystem sSys;
			SocketListener sListener(receiverPort, Socket::IP6);
			ClientHandler_MsgServer cMsg(messageQ);
			cMsg.fetchDownloadFolder(downloadFolder);
			sListener.start(cMsg);
			while (true)
			{
				HttpMessage message = messageQ.deQ();
				std::cout << "\nThe Server received the Message:\n" + message.bodyString() + "\n";
				receiverQ.enQ(message.bodyString());
			}
		}
		catch (std::exception& exc)
		{
			std::cout << "\n  Exception caught: ";
			std::string exMsg = "\n" + std::string(exc.what()) + "\n\n";
			std::cout << exMsg;
		}
	});

}
std::string MockChannel::fetchAttributes(std::string msg, std::string attribute)
{
	int sI = (msg.find("<" + attribute + ">") + (attribute.length() + 2));
	int eI = msg.find("</" + attribute + ">");
	return msg.substr(sI, eI - sI);
}
//----< creates thread to read from sendQ and echo back to the recvQ >-------

void MockChannel::start()
{
	std::cout<<  "\n  Starting up the MockChannel\n";
	this->senderThread();
	this->receiverThread();

	
}
//----< signal server thread to stop >---------------------------------------

void MockChannel::stop() { stop_ = true; }



//----< factory functions >--------------------------------------------------

ISendr* ObjectFactory::createSendr() { return new Sender; }

IRecvr* ObjectFactory::createRecvr() { return new Receiver; }

IMockChannel* ObjectFactory::createMockChannel(ISendr* pISendr, IRecvr* pIRecvr,int rcvPort, std::string downloadFolder) 
{ 
  return new MockChannel(pISendr, pIRecvr,rcvPort,downloadFolder); 
}

#ifdef TEST_MOCKCHANNEL

//----< test stub >----------------------------------------------------------

int main()
{
  ObjectFactory objFact;
  ISendr* pSendr = objFact.createSendr();
  IRecvr* pRecvr = objFact.createRecvr();
  IMockChannel* pMockChannel = objFact.createMockChannel(pSendr, pRecvr);
  pMockChannel->start();
  pSendr->postMessage("Hello World");
  pSendr->postMessage("CSE687 - Object Oriented Design");
  Message msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg << "\"";
  msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg << "\"";
  pSendr->postMessage("stopping");
  msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg << "\"";
  pMockChannel->stop();
  pSendr->postMessage("quit");
  std::cin.get();
}
#endif
