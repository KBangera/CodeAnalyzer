#pragma once
#include "MsgServer.h"

//----< this defines processing to frame messages >------------------

// if the message is a file, it will be downloaded here itself. No need to send it up to Publisher
HttpMessage ClientHandler_MsgServer::readMessage(Socket& socket)
{
  connectionClosed_ = false;
  HttpMessage msg;

  // read message attributes

  while (true)
  {
    std::string attribString = socket.recvString('\n');
    if (attribString.size() > 1)
    {
      HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
      msg.addAttribute(attrib);
    }
    else
    {
      break;
    }
  }
  // If client is done, connection breaks and recvString returns empty string

  if (msg.attributes().size() == 0)
  {
    connectionClosed_ = true;
    return msg;
  }
  // read body if POST - all messages in this demo are POSTs

  if (msg.attributes()[0].first == "POST")
  {
    // is this a file message?

    std::string filename = msg.findValue("file");
    if (filename != "")
    {
      size_t contentSize;
      std::string sizeString = msg.findValue("content-length");
      if (sizeString != "")
        contentSize = Converter<size_t>::toValue(sizeString);
      else
        return msg;

      readFile(msg.findValue("catg"), filename, contentSize, socket);
    }

    if (filename != "")
    {
      // construct message body

      msg.removeAttribute("content-length");
      std::string bodyString = "<file>" + filename + "</file>";
      std::string sizeString = Converter<size_t>::toString(bodyString.size());
      msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
      msg.addBody(bodyString);
    }
    else
    {
      // read message body

      size_t numBytes = 0;
      size_t pos = msg.findAttribute("content-length");
      if (pos < msg.attributes().size())
      {
        numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
        Socket::byte* buffer = new Socket::byte[numBytes + 1];
        socket.recv(numBytes, buffer);
        buffer[numBytes] = '\0';
        std::string msgBody(buffer);
        msg.addBody(msgBody);
        delete[] buffer;
      }
    }
  }
  return msg;
}
//----< read a binary file from socket and save >--------------------
/*
 * This function expects the sender to have already send a file message, 
 * and when this function is running, continuosly send bytes until
 * fileSize bytes have been sent.
 */
bool ClientHandler_MsgServer::readFile(std::string category, const std::string& filename, size_t fileSize, Socket& socket)
{
	std::string dPath = downLoadFolder;
	if (category.length()>0)
	{
		dPath += "\\" + category;
	}
	size_t pos = dPath.find('\\');
	while (pos != std::string::npos) {
		if (!FileSystem::Directory::exists(dPath.substr(0, pos)))
			FileSystem::Directory::create(dPath.substr(0, pos));


		pos = dPath.find('\\', pos + 1);
	}
	if (dPath[dPath.size() - 1] != '\\')
	{
		if (!FileSystem::Directory::exists(dPath))
			FileSystem::Directory::create(dPath);
	}
	FileSystem::Directory::create(dPath);
	std::string shortFileName = FileSystem::Path::getName(filename);
	std::string fqname = dPath +"\\"+shortFileName;
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{
		/*
		 * This error handling is incomplete.  The client will continue
		 * to send bytes, but if the file can't be opened, then the server
		 * doesn't gracefully collect and dump them as it should.  That's
		 * an exercise left for students.
		 */
		Show::write("\n\n  can't open file " + fqname);
		return false;
	}
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	size_t bytesToRead;
	while (true)
	{
		if (fileSize > BlockSize)
			bytesToRead = BlockSize;
		else
			bytesToRead = fileSize;
		socket.recv(bytesToRead, buffer);
		FileSystem::Block blk;
		for (size_t i = 0; i < bytesToRead; ++i)
			blk.push_back(buffer[i]);
		file.putBlock(blk);
		if (fileSize < BlockSize)
			break;
		fileSize -= BlockSize;
	}
	file.close();
	return true;
}
//----< receiver functionality is defined by this function >---------

void ClientHandler_MsgServer::operator()(Socket socket)
{
  while (true)
  {
    HttpMessage msg = readMessage(socket); 
    if (connectionClosed_ || msg.bodyString() == "quit")
    {
      Show::write("\n\n  clienthandler thread is terminating");
      break;
    }
	//don't enque if it is a file. 
	if (msg.findValue("file")=="")
	{
		msgQ_.enQ(msg);
	}   
	std::cout << "\nmessage added:";
	std::cout << msg.bodyString();
  }
}

void ClientHandler_MsgServer::fetchDownloadFolder(std::string downloadFolder)
{
	this->downLoadFolder = FileSystem::Path::getFullFileSpec(downloadFolder);
}

//----< test stub >--------------------------------------------------

/*
int main()
{
  ::SetConsoleTitle(L"HttpMessage Server - Runs Forever");

  Show::attach(&std::cout);
  Show::start();
  Show::title("\n  HttpMessage Server started");

  BlockingQueue<HttpMessage> msgQ;

  try
  {
    SocketSystem ss;
    SocketListener sl(8080, Socket::IP6);
    ClientHandler_MsgServer cp(msgQ);
    sl.start(cp);
    
    while (true)
    {
      HttpMessage msg = msgQ.deQ();
      Show::write("\n\n  server recvd message with body contents:\n" + msg.bodyString());
    }
  }
  catch (std::exception& exc)
  {
    Show::write("\n  Exeception caught: ");
    std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
    Show::write(exMsg);
  }
  return 0;
}
*/