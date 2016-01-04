#include "SocketLib.hpp"
#include <iostream>
#include <string>
#pragma comment(lib, "SocketLibrary.lib")
/*
brief: displayVector - for displaying selection options
Params: vector<string>files - a vector of files available for download
returns: nothing - displays the data
*/
void displayVector(vector<string>files) {
	int count = 0;
	if (files.size() > 0) {
		cout << "\n\nFiles available for download from host:";

		for (string s : files) {
			cout << endl << count << ". " << s.substr(s.find_last_of("\\") + 1, s.length() - 1);
			count++;
		}
	}
	else {
		cout << "\n\nNo Files In Directory ServerFiles, transfer files to directory to share, then restart application\n";
	}

	cout << "\n\n" << files.size() << ". CLOSE CLIENT APPLICATION";
	cout << "\n" << files.size() + 1 << ". CLOSE CLIENT AND SERVER APPLICATIONS";
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
			else if (customPort < 0 || customPort > 65535) {
				cout << "Must be between 0 and 65536(exclusive)\n";
				continue;
			}
			

			break;
		}
	}
	//Declare and construct a custom ClientSocket object
	ClientSocket client(ip, customPort);
	//Receive a ping from the server to confirm connection
	client.receivePing();
	//get the files from the client
	vector<string> files = client.receiveVector();

	//loop through allowing file selection until the user closes the application(s)
	while (true) {
		//Show them the selections
		displayVector(files);
		int selection = -1;
		//Confirm that the user made a usable selection
		while (selection < 0 || selection >(int)files.size()) {
			cout << "\n\nPlease enter the number corresponding to the file you would like to download: ";

			cin >> selection;


			if (selection == files.size() || selection == files.size()+1) {
				client.sendSelection(selection);
				return 0;
			}
			else if (selection < 0 || selection >(int)files.size()) {
				cout << "\nSelection failed, please choose an number that is listed";
			}

		}
		//tell the user they're a good person
		cout << "\nGreat Choice!  Sending " << files[selection].substr(files[selection].find_last_of("\\"), files[selection].length() - 1);
		//Send the selection
		client.sendSelection(selection);
	
		//Receive the selected File from the server
		client.receiveFile();
		cout << "\nFile Received Succesfully\n\n";
	}

	

	_CrtDumpMemoryLeaks();
}