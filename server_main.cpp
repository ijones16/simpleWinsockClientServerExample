#include <windows.h>
#include <winsock.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
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

std::ifstream::pos_type countFileSize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

// THE SERVER: serves clients
int main()
{
	if (!Socket::Init()) {
		cerr << "Fail to initialize WinSock!\n";
		return -1;
	}

	// Step 1: Create a TCP socket
	Socket server("tcp");

	// Step 2: Bind socket to a port (MYPORT)	
	if (!server.sock_bind("", MYPORT))
	{
		stringstream str;
		str << "Could not bind to port " << MYPORT;
		done(str.str());
	}

	// Step 3: Ask my socket to "listen"
	if (!server.sock_listen(1)) 
	{
		done("Could not get socket to listen");
	}

	cout << "Server is now listening on port " << MYPORT << endl;
	
	// Step 4:	Wait to accept a connection from a client.  
	//			The variable conn is the "connected" socket.  This is the socket that is actually connected to the server.
	//			The socket variable server can be used to accept another connection if you want.
	Socket conn = server.sock_accept();
	
	cout << "A client just connected to me!\n";

	// Step 5: Receive/send a message from/to the client. 
	string msg = conn.msg_recv();			// C: Hello there
	if (msg != "Hello server") {
		done("Incorrect client greeting");
	}
	cout << "Received message: " << msg << endl;

	cout << "Sending my server greeting ...\n";
	conn.msg_send("Hello client");		// S: Nice to meet you!

	// Client asks for server to send data
	msg = conn.msg_recv();
	if (msg != "Give me data!"){
		done("Incorrect data request");
	}
	cout << "Received message: " << msg << endl;

	// Server sends file size to prepare client
	int fileSize = countFileSize("server_data.txt");
	string fileSizeString = to_string(fileSize);
	cout << "Sending file size of " + fileSizeString + "\n";
	conn.msg_send(fileSizeString);

	// Reveive approval
	msg = conn.msg_recv();
	if (msg != "I am ready"){
		done("Client not ready");
	}
	cout << "Recieved message: " << msg << endl;

	// Read file and send data to client
	string fileToSend;
	string fileLine;
	ifstream myfile ("server_data.txt");
	if (myfile.is_open()){
		while ( getline (myfile,fileLine) ){
			fileToSend += fileLine + "\r\n";
		}
		myfile.close();
	}	
	else done("Unable to open file"); 

	conn.msg_send(fileToSend);
	cout << "File is sent.\n";

	// Recieve clients thank
	msg = conn.msg_recv();
	if (msg != "Thank you"){
		done("Client had an error or wasn't thankful");
	}
	cout << "Recieved message: "<< msg << endl;

	// Send goodbye
	msg = "Goodbye";
	cout << "Sending message: " << msg << endl;
	conn.msg_send(msg);

	done("Press enter to exit");
	return 0;

}


