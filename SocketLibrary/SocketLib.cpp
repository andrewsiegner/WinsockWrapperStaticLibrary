#include "SocketLib.hpp"

void Socket::cleanup()
{
	WSACleanup();

}

void HostSocket::sendPing()
{
	int bytesSent;

	char sendBuff[32] = "Ping!";

	bytesSent = send(hAcceptedSocket, sendBuff, strlen(sendBuff) + 1, 0);
	if (verbose) {
		cout << "\nPinging client to confirm connection: Sent: " << bytesSent << " bytes";
	}
}

void HostSocket::sendString(string anyString)
{
	const char* sendable = anyString.c_str();

	int sizetoSend = strlen(sendable) + 1;

	int bytesSent = send(hAcceptedSocket, (char*)&sizetoSend, sizeof(sizetoSend), 0);

	bytesSent = send(hAcceptedSocket, sendable, sizetoSend, 0);

}


bool HostSocket::sendNumber(long n)
{
	int bytesSent = send(hAcceptedSocket, (char*)&n, sizeof(n), 0);

	return bytesSent == sizeof(n);
}

long HostSocket::receiveSelection()
{

	long value;
	int bytesReceived = recv(hAcceptedSocket, (char*)&value, 4, 0);

	return value;
}

void HostSocket::cleanup()
{
	closesocket(hAcceptedSocket);
	closesocket(hServerSocket);
	WSACleanup();
}


void HostSocket::sendVector(const vector<string> vec)
{
	int nItems = vec.size();

	int bytesSent = send(hAcceptedSocket, (char*)&nItems, sizeof(int), 0);

	for (int i = 0; i < nItems; i++) {
		sendString(vec[i]);
	}

}

void HostSocket::sendFile(string path)
{
	ifstream inputStream;

	ios::streampos fileSize;
	
	inputStream.open(path.c_str(), ios::in | ios::binary | ios::ate);

	if (!inputStream.is_open()) {
		throw new exception("Error opening file (server side)");
	}

	fileSize = inputStream.tellg();

	char *fileBuff = new char[(unsigned)fileSize];
	inputStream.seekg(0, ios::beg);
	inputStream.read(fileBuff, fileSize);
	inputStream.close();

	int progress = 0;

	if (!sendNumber((unsigned)fileSize)) {
		cout << "\nError sending header ";
		return;
	}

	sendString(path.substr(path.find_last_of("\\"), path.length() - 1));

	while (progress != fileSize) {
		progress = send(hAcceptedSocket, fileBuff, (int)fileSize, 0);
	}

	delete fileBuff;
}



string ClientSocket::receiveString()
{
	string receivedString;
	int bytesToReceive, bytesReceived;

	bytesReceived = recv(hClientSocket, (char*)&bytesToReceive, 4, 0);

	if (verbose) {
		cout << "\nPrepared to receive " << bytesToReceive << " byte long string";
	}
	const int bytes = bytesToReceive;

	char* recBuff;
	recBuff = new char[bytes];
	bytesReceived = recv(hClientSocket, recBuff, bytes, 0);
	

	string output = (string)recBuff;
	if (verbose) {
		cout << "\nReceived: " << recBuff << endl;
	}
	delete recBuff;
	return output;
}

vector<string> ClientSocket::receiveVector()
{
	int stringsInVector, bytesReceived;

	vector<string> received;

	bytesReceived = recv(hClientSocket, (char*)&stringsInVector, sizeof(int), 0);

	for (int i = 0; i < stringsInVector; i++) {
		received.push_back(receiveString());
	}
	return received;
}

void ClientSocket::receiveFile() {
	string clientDirectory = "ClientFiles\\";
	string clientFileLocation = clientDirectory;
	ofstream outputStream;
	int count = 0;
	
	wstring stemp = wstring(clientDirectory.begin(), clientDirectory.end());
	LPCWSTR lpCD = stemp.c_str();

	if (CreateDirectory(lpCD, NULL) || GetLastError()==ERROR_ALREADY_EXISTS) {

		int fileSize = receiveNumber();
		char* fileBuffer = new char[fileSize];

		long bytesReceived, totalBytesReceived = 0;
		clientFileLocation += receiveString().c_str();
		outputStream.open(clientFileLocation.c_str(), ios::out | ios::binary);

		while (totalBytesReceived < fileSize) {
			bytesReceived = recv(hClientSocket, fileBuffer, fileSize, 0);
			outputStream.write(fileBuffer, bytesReceived);
			totalBytesReceived += bytesReceived;
			count++;

		}

		outputStream.close();
		delete fileBuffer;
	}
	else {
		cout << "Error creating Directory - never gonna give you up";
	}
}

long ClientSocket::receiveNumber()
{
	long value;
	int bytesReceived = recv(hClientSocket, (char*)&value, 4, 0);

	return value;
}

bool ClientSocket::sendSelection(long n)
{
	int bytesSent = send(hClientSocket, (char*)&n, sizeof(n), 0);

	return bytesSent == sizeof(n);
}

void ClientSocket::cleanup()
{
	closesocket(hClientSocket);
	WSACleanup();
}

void ClientSocket::receivePing()
{
	char recBuff[32] = "";

	int bytesReceived = recv(hClientSocket, recBuff, 32, 0);
	if (verbose) {
		cout << endl << recBuff << " - connection confirmed";
	}

}



