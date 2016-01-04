#include <iostream>
#include "SocketLib.hpp"
#include <string>
#include <vector>
#include "boost/filesystem.hpp"

using namespace std;
namespace fs = boost::filesystem;

/*
Brief: getFilePaths - gets paths for each file in a directory called ServerFiles, creates the directory if it does not exist
Params: None
Returns: a vector of strings, one for each file in the directory
*/
vector<string>getFilePaths() {

	string serverDirectory = "ServerFiles\\";
	wstring stemp = wstring(serverDirectory.begin(), serverDirectory.end());
	LPCWSTR lpSD = stemp.c_str();
	vector<string> allPaths;
	if (CreateDirectory(lpSD, NULL) || GetLastError() == ERROR_ALREADY_EXISTS) {
		fs::path full_path = fs::system_complete("ServerFiles");

		if (!fs::exists(full_path)) {
			return allPaths;
		}
		fs::directory_iterator end_iterator;


		for (auto i = fs::directory_iterator(full_path); i != end_iterator; i++) {
			if (fs::is_regular_file(i->status())) {
				allPaths.push_back(i->path().string());
			}
		}
	}

	return allPaths;
}




int main() {
	cout << "Enter Custom ip or enter 0 to use default (127.0.0.1): ";
	string ip;
	cin >> ip;

	if (ip == "0") {
		ip = "127.0.0.1";
	}

	USHORT customPort;
	while (true) {
		cout << "\nEnter Custom port or enter 0 to use default: ";
		
		cin >> customPort;
		if (cin.fail()) {
			cout << "Enter a number\n";
			continue;
		}
		else {
			if (customPort == 0) {
				customPort = 27015;
			}
			else if (customPort < 1 || customPort > 65535) {
				cout << "Must be between 0 and 65535\n";
				continue;
			}
			break;
		}
	}

	//Create a HostSocket object using custom information
	HostSocket server(ip, customPort);
	vector<string> filePaths = getFilePaths();
	//Send a ping to confirm connection
	server.sendPing();
	//Connection is good, send the file paths
	server.sendVector(filePaths);
	while (true) {
		//Get the users selection
		long selection = server.receiveSelection();
		//If the user wants to close the client applicaiton only, destruct the HostSocket object server, recreate it, and allow new connections
		if (selection == filePaths.size()) {
			(&server)->~HostSocket();
			new (&server) HostSocket(ip, customPort);
			server.sendPing();
			filePaths = getFilePaths();
			server.sendVector(filePaths);
			continue;
		}
		//IF the user wants to close both applications, destruct the HostSocket and close everything
		else if (selection == filePaths.size() + 1) {
			(&server)->~HostSocket();
			break;
		}
		//if the user selected an appropriate choice, send the file the chose
		else {
			server.sendFile(filePaths[selection]);
		}
	}

	//server.cleanup();

	_CrtDumpMemoryLeaks();

}