#include <iostream>
#include <string>
#include <sstream>
#include "./Socket.h"

using namespace std;

const int MYPORT = 50001;

int done(const string message)
{
	Socket::Cleanup();
	cout << message;
	cin.get();
	exit(0);
}

// THE CLIENT: Connects to Server
int main()
{
	if (!Socket::Init()) {
		cerr << "Fail to initialize WinSock!\n";
		return -1;
	}

	// Step 1: Create a TCP socket
	Socket client("tcp");

	// 127.0.0.1 is the IP address for the local host
	cout << "Attempting to connect to 127.0.0.1" << MYPORT << endl;

	// Step 2: Connect to server
	if ( !client.sock_connect("127.0.0.1", MYPORT) )
	{
		done("could not connect to server");
	}

	cout << "Connected to 127.0.0.1:" << MYPORT << endl;

	// Step 3: Successfully connected to server.  We can now send (and receive) messages.
	string msg("Hello server");
	cout << "Sending message: " << msg << endl;
	client.msg_send(msg);							// C: Hello there

	msg = client.msg_recv();						// S: Nice to meet you!
	if (msg != "Hello client") {
		done("Incorrect server greeting");	
	}
	cout << "Received message: " << msg << endl;
	
	// Request for server file 
	string dataRequest("Give me data!");
	cout << "Sending message: " << dataRequest << endl;
	client.msg_send(dataRequest);

	// Recieve file size
	msg = client.msg_recv();
	cout << "The file will be " + msg + " bytes\n";

	// set up file size var to pass into msg_recvall()
	std::string::size_type sz;   // alias of size_t
	int fileSize = stoi (msg,&sz);

	// Tell the server it is ok to send
	msg = "I am ready";
	cout << "Sending message: " << msg << endl;
	client.msg_send(msg);

	// Recieve data file
	msg = client.msg_recvall(fileSize);
	cout << "File recieved:\n" << msg << endl;

	
	// Send many thanks
	msg = "Thank you";
	cout << "Sending message: " << msg << endl;
	client.msg_send(msg);

	// Revieve goodbye
	msg = client.msg_recv();
	cout << "Message recieved: " << msg << endl;
	
	done("Press enter to exit");
	return 0;
}
