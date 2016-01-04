#if !defined(GUARD_SOCKET_LIBRARY)
#define GUARD_SOCKET_LIBRARY



#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <vector>
#include <fstream>
#pragma comment (lib,"ws2_32.lib")

using namespace std;
using file_size_t = unsigned long;

class Socket {
protected:
	WSADATA wsadata;
	sockaddr_in serverAddress;
	u_short PORT = 20715; //Default port value (used only in no arg constructor)
	string address = "127.0.0.1"; //Default IP address, loopback address, used in default constructor and port specified constructor
	bool verbose = false;

public:
	void cleanup();

};

class HostSocket : public Socket {
	SOCKET hAcceptedSocket;//Accepted socket
	SOCKET hServerSocket;//base server socket

public:

	/*Brief: Default Constructor
	  No Params
	  Returns a a Host Socket using default IP and default port
	*/
	HostSocket() {
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
		if (iResult != NO_ERROR) {
			WSACleanup();
			throw new exception("Error - WSA startup failure");
		}
		hServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		serverAddress = { 0 };
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(PORT);
		serverAddress.sin_addr.s_addr = inet_addr(address.c_str());

		if (bind(hServerSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
			closesocket(hServerSocket);
			throw new std::exception("Error with Binding process");
		}

		if (listen(hServerSocket, 1) == SOCKET_ERROR) {
			closesocket(hServerSocket);
			throw new std::exception("Error Initializing listening");
		}
		cout << "\nServer Ready for connection";
		

		hAcceptedSocket = SOCKET_ERROR;
		while (hAcceptedSocket == SOCKET_ERROR) {
			hAcceptedSocket = accept(hServerSocket, NULL, NULL);
		}

		cout << "\nServer Received Connection";
		
	}
	/*
	Brief: Port Specified Constructor
	Params: USHORT port - the port value to be used by the Host socket
	Returns: A port specified HostSocket object
	*/
	HostSocket(USHORT port) {
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
		if (iResult != NO_ERROR) {
			WSACleanup();
			throw new exception("Error - WSA startup failure");
		}
		hServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		serverAddress = { 0 };
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(port);
		serverAddress.sin_addr.s_addr = inet_addr(address.c_str());

		if (bind(hServerSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
			closesocket(hServerSocket);
			throw new std::exception("Error with Binding process");
		}

		if (listen(hServerSocket, 1) == SOCKET_ERROR) {
			closesocket(hServerSocket);
			throw new std::exception("Error Initializing listening");
		}
		cout << "\nServer Ready for connection";

		hAcceptedSocket = SOCKET_ERROR;
		while (hAcceptedSocket == SOCKET_ERROR) {
			hAcceptedSocket = accept(hServerSocket, NULL, NULL);
		}

		cout << "\nServer Received Connection";
	}

	/*
	Brief: Port And IP Specified constructor
	Params:
		string customAddress - a string representing the custom IP address to use
		USHORT port - the port to be used
	Returns: A port and address specified HostSocket
	*/
	HostSocket(string customAddress, USHORT port) {
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
		if (iResult != NO_ERROR) {
			WSACleanup();
			throw new exception("Error - WSA startup failure");
		}
		hServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		serverAddress = { 0 };
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(port);
		serverAddress.sin_addr.s_addr = inet_addr(customAddress.c_str());

		if (bind(hServerSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
			closesocket(hServerSocket);
			throw new std::exception("Error with Binding process");
		}

		if (listen(hServerSocket, 1) == SOCKET_ERROR) {
			closesocket(hServerSocket);
			throw new std::exception("Error Initializing listening");
		}
		cout << "\nServer Ready for connection";

		hAcceptedSocket = SOCKET_ERROR;
		while (hAcceptedSocket == SOCKET_ERROR) {
			hAcceptedSocket = accept(hServerSocket, NULL, NULL);
		}

		cout << "\nServer Received Connection";
	}
	//Destructor - runs cleanup to clear up old port and WSAData information
	~HostSocket() {
		cleanup();
	}
	/*
	Brief: sendPing() - sends some data to confirm connection status
	Params: None
	Returns: Nothing, requested by client to confirm connection
	*/
	void sendPing();

	/*
	Brief: sendString()
	Params: string anyString - the string which the server is meant to send
	Returns: nothing, sends the string to a client socket
	*/
	void sendString(string anyString);
	/*
	Brief: sendVector
	Params: vector<string> vec - the vector of strings to send
	Returns: nothing, runs sendString for each item in the vector
	*/
	void sendVector(vector<string>vec);
	/*
	Brief: sendFile
	Params: string path - a string representing the local path of the file to send
	Returns: Nothing - sends the file (if it can)
	*/
	void sendFile(string path);
	/*
	Brief: sendNumber
	Params: long n - the number to send
	Returns: a boolean representing whether or not the correct amount of bytes were sent
	*/
	bool sendNumber(long n);

	/*
	Brief: receiveSelection
	Params: None
	Returns: a long representing the users file selection
	*/
	long receiveSelection();
	/*
	Brief: cleanup
	Params: none
	Returns: nothing, cleans up socket and WSAdata
	*/
	void cleanup();

};

class ClientSocket : public Socket {
	SOCKET hClientSocket;

public:
	/*
	Brief: Default constructor, defaults IP and port number to the values specified in class Socket
	*/
	ClientSocket() {
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
		if (iResult != 0) {
			WSACleanup();
			throw new exception("WSA startup has failed!");
		}
		try {
			hClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			serverAddress = { 0 };
			serverAddress.sin_family = AF_INET;
			serverAddress.sin_port = htons(PORT);
			serverAddress.sin_addr.s_addr = inet_addr(address.c_str());

			if (connect(hClientSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
				closesocket(hClientSocket);
				WSACleanup();
				throw new exception("Connection failed - check address and port information");
			}
			cout << "Connection looks good";
		}
		catch (exception e) {
			cout << "\nSomething went wrong, ensure server is available";
		}

	}
	/*
	Brief:  port specified ClientSocket constructor
	Params: USHORT port - the port value to use
	Returns: A port specified ClientSocket
	*/
	ClientSocket(USHORT port) {
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
		if (iResult != 0) {
			WSACleanup();
			throw new exception("WSA startup has failed!");
		}
		try {
			hClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			serverAddress = { 0 };
			serverAddress.sin_family = AF_INET;
			serverAddress.sin_port = htons(port);
			serverAddress.sin_addr.s_addr = inet_addr(address.c_str());

			if (connect(hClientSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
				closesocket(hClientSocket);
				WSACleanup();
				throw new exception("Connection failed - check address and port information");
			}
			cout << "Connection looks good";
		}
		catch (exception e) {
			cout << "\nSomething went wrong, ensure server is available";
		}

	}
	/*
	Brief: Address and Port Specified constructor
	Params: 
		string customAddress - the custom IP address to use
		USHORT port - the custom port to use
	Returns: An address and port specified ClientSocket
	*/
	ClientSocket(string customAddress, USHORT port) {
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
		if (iResult != 0) {
			WSACleanup();
			throw new exception("WSA startup has failed!");
		}
		try {
			hClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			serverAddress = { 0 };
			serverAddress.sin_family = AF_INET;
			serverAddress.sin_port = htons(port);
			serverAddress.sin_addr.s_addr = inet_addr(customAddress.c_str());

			if (connect(hClientSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
				closesocket(hClientSocket);
				WSACleanup();
				throw new exception("Connection failed - check address and port information");
			}
			cout << "Connection looks good";
		}
		catch (exception e) {
			cout << "\nSomething went wrong, ensure server is available";
		}

	}

	~ClientSocket() {
		cleanup();
	}
	/*
	Brief: receivePing
	Params: None
	Returns: nothing - receives some data to confirm connection
	*/
	void receivePing();
	/*Brief: receiveString
	PArams: None
	Returns: the string sent from the server
	*/
	string receiveString();
	/*Brief: receiveVector
	Params: Nothing
	Returns: the vector of strings sent by the server
	*/
	vector<string>receiveVector();
	/*Brief: receiveFile - receives a file from the server
	Params: nothing
	returns: Nothing, creates a file in a directory called ClientFiles
	*/
	void receiveFile();
	/*
	Brief: receiveNumber
	Params: None
	Returns: a long value holding the number sent by the server
	*/
	long receiveNumber();
	/*
	Brief: sendSelection
	Params: long n - a selection to send to the server
	Retuns: boolean representing whether the correct number of bytes was sent
	*/
	bool sendSelection(long n);
	/*
	Brief: cleanup
	Params: None
	Returns: nothing - cleans up WSA and socket data
	*/
	void cleanup();

};
#endif

