#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <malloc.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include <signal.h>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <chrono>
#include <map>
#include "functions.h"
#include "structures.h"

//	Forward Declarations

std::string read(std::string);
std::string read(int);
void write(std::string, std::string);
std::string findGroup(std::string);

//	Structures and data types

struct serverStruct {

	struct sockaddr_in address;
	int socket = -1;
	bool active = false;
};

struct message {

	uint64_t timestamp;
	std::string value;
};

//	Global variables

std::vector<serverStruct> servers;
int W;
int R;
std::vector<message> messages;
std::string config;

//	Functions

void closeConnections() {

	std::string message = "QUIT";
	logSend("%s (%d)", message.c_str(), (int)message.length());

	int length = message.size();

	for (int i = 0; i < servers.size(); i++) {

		write(servers[i].socket, &length, sizeof(length));

		for (int j = 0; j < message.size(); j++) {
			write(servers[i].socket, &message[j], sizeof(message[j]));
		}

		std::string response = read(servers[i].socket);
		logReceive("(%d) %s", ntohs(servers[i].address.sin_port), response.c_str());
	}
}

void establishConnections() {

	//	For all the servers in the list, tries to establish connection and refresh the socket.
	//	If a connection is already established, verifies by sending a NOOP

	for (int i = 0; i < servers.size(); i++) {

		if (servers[i].active == false) {

			int sock = socket(PF_INET, SOCK_STREAM, 0);
			servers[i].socket = sock;

			if(connect(sock, (struct sockaddr*) &servers[i].address, sizeof(servers[i].address)) < 0) {

				logVerbose("Could not establish connection with %d", ntohs(servers[i].address.sin_port));
				servers[i].active = false;
			}
			else {

				servers[i].active = true;
			}
		}
		else {

			std::string message = "NOOP";

			int length = message.size();

			logSend("%s (%d)", message.c_str(), (int)message.length());

			write(servers[i].socket, &length, sizeof(length));

			for (int j = 0; j < message.size(); j++) {
				write(servers[i].socket, &message[j], sizeof(message[j]));
			}

			std::string response;
			response = read(servers[i].socket);

			if (response == "-1") {

				int sock = socket(PF_INET, SOCK_STREAM, 0);
				servers[i].socket = sock;

				if(connect(sock, (struct sockaddr*) &servers[i].address, sizeof(servers[i].address)) < 0) {

					logVerbose("Could not establish connection with %d", ntohs(servers[i].address.sin_port));
					servers[i].active = false;
				}
				else {

					servers[i].active = true;
				}
			}
		}
	}
}

void terminate(int signal) {

//	Executed when client quits (handle CTRL + Q)

	closeConnections();
	panic("Exiting");
}

void bufferMessage(std::string buffer) {

	message current;

	std::stringstream timestamp(buffer.substr(1, buffer.find("]") - 2));
	timestamp >> current.timestamp;
	current.value = buffer.substr(buffer.find("]") + 1, std::string::npos);

	messages.push_back(current);
}

void parseServerList(std::string group) {

//	Finds server belonging to the given group in storage.md file and stores them in the vector servers

	servers.clear();

	std::ifstream file(config);
	std::string contents;

	if (!file) {
		panic("Error: Could not read file <%s>", config.c_str());
	}

	std::string line;

	while (getline(file, line)) {
		if (line[0] != '#') {
			if (line.substr(line.find("- ") + 2, std::string::npos) == group) {
				serverStruct server;
				bzero(&server.address, sizeof(server.address));
				server.address.sin_family = AF_INET;
				std::stringstream port(line.substr(line.find(':') + 1, line.find(",") - line.find(':') - 1));
				int portNumber = 0;
				port >> portNumber;
				server.address.sin_port = htons(portNumber);
				inet_pton(AF_INET, line.substr(0, line.find(':')).c_str(), &(server.address.sin_addr));
				servers.push_back(server);
			}
		}
	}

	if (servers.size() == 0) {
		panic("No storage servers found for group %s", group.c_str());
	}
}

void initializeQuorum() {

	W = servers.size()/2 + 1;
	R = servers.size() - W + 1;
}

std::string read(int socket) {

//	Read from a particular server

	int length;
	std::string buffer;

	if (read(socket, &length, sizeof(length)) <= 0) {
		return "-1";
	}

	for (int j = 0; j < length; j++) {

		char character;
		read(socket, &character, sizeof(character));
		buffer.push_back(character);
	}

	return buffer;
}

std::string read(std::string row) {

//	Read from all the R servers

	int count = 0;

	for (int i = 0; i < servers.size(); i++) {

	//	Reads a message from the socket

		int length;
		std::string buffer;

		if (read(servers[i].socket, &length, sizeof(length)) <= 0) {
			logVerbose("Could not connect to %d", ntohs(servers[i].address.sin_port));
			servers[i].active = false;
			continue;
		}
		else {
			count++;
		}

		for (int j = 0; j < length; j++) {

			char character;
			read(servers[i].socket, &character, sizeof(character));
			buffer.push_back(character);
		}

		logReceive("(%d) %s (%d)", ntohs(servers[i].address.sin_port), buffer.c_str(), (int)buffer.length());

		bufferMessage(buffer);
	}

	if (count < R) {

		panic("Could only read from %d/%d servers", count, W);
	}

//	Go through all the messages and select the one with highest timestamp

	message final = messages[0];

	for (int i = 1; i < messages.size(); i++) {

		if (messages[i].timestamp > final.timestamp) {

			final = messages[i];
		}
	}

	messages.clear();

	return final.value;
}

void write(std::string message, std::string row) {

	logSend("%s (%d)", message.c_str(), (int)message.length());

	int count = 0;

	for (int i = 0; i < servers.size(); i++) {

		int length = message.size();

		write(servers[i].socket, &length, sizeof(length));

		for (int j = 0; j < message.size(); j++) {
			write(servers[i].socket, &message[j], sizeof(message[j]));
		}

		std::string response;
		response = read(servers[i].socket);

		if (response == "-1") {
			servers[i].active = false;
		}
		else {
			logReceive("(%d) %s", ntohs(servers[i].address.sin_port), response.c_str());
			count++;
		}
	}

	if (count < W) {

		panic("Could only write to %d/%d servers", count, W);
	}
}

std::string findGroup(std::string value) {

	return "G1";

//	Parses server list in the storage.txt configuration file and find master and then ask the master for group

	std::ifstream file(config);
	std::string contents;

	if (!file) {
		panic("An error occurred when reading %s", config.c_str());
	}

	std::string line;
	std::stringstream masterIP;
	std::stringstream masterPort;

	bool masterFound = false;

	while (getline(file, line)) {

		if (line[0] != '#') {

			if (line.substr(line.find(", ") + 2, std::string::npos) == "Master") {

				masterIP << line.substr(0, line.find(":"));
				masterPort << line.substr(line.find(":") + 1, line.find(",") - line.find(":") - 1);

				logVerbose("Master found at %s:%s", masterIP.str().c_str(), masterPort.str().c_str());
				masterFound = true;
				break;
			}
		}
	}

	if (masterFound == false) {
		panic("Master was not found in the configuration file (%s)", config.c_str());
	}

//	Convert the string to the address

	struct sockaddr_in master;
	bzero(&master, sizeof(master));
	master.sin_family = AF_INET;
	int port = 0;
	masterPort >> port;
	master.sin_port = htons(port);
	inet_pton(AF_INET, masterIP.str().c_str(), &(master.sin_addr));

//	Connect to the master via TCP

	int sock = socket(PF_INET, SOCK_STREAM, 0);

	if(connect(sock, (struct sockaddr*) &master, sizeof(master)) < 0) {

		panic("Could not establish connection with the master at %d", ntohs(master.sin_port));
	}

	log("Established connection with the master");
	log("Read from the master: %s", read(sock).c_str());

	value = "{ Search Key: " + value + " }";

	logVerbose("Sending: %s (%d)", value.c_str(), (int)value.length());

	int length = value.length();
	write(sock, &length, sizeof(length));

	for (int j = 0; j < value.length(); j++) {
		write(sock, &value[j], sizeof(value[j]));
	}

	std::string group = read(sock);

	log("Read group: %s from the master", group.c_str());

	return group;
}

/* Functions for interacting with the storage */

void PUT(std::string row, std::string column, std::string value) {

//	Stores a value v in column c of row r

	std::string group = findGroup(row);
	parseServerList(group);
	initializeQuorum();
	establishConnections();

	std::stringstream message;

	message << "PUT(";
	message << row << ",";
	message << column << ",";
	message << "[" << time() << "]";
	message << value << ")";

	write(message.str(), row);
	closeConnections();
}

void CPUT(std::string row, std::string column, std::string value_1, std::string value_2) {

//	Stores a value v in column c of row r

	std::string group = findGroup(row);
	parseServerList(group);
	initializeQuorum();
	establishConnections();

	std::stringstream message;

	message << "CPUT(";
	message << row << ",";
	message << column << ",";
	message << "[" << time() << "]";
	message << value_1 << "-->|<--" << value_2 << ")";

	write(message.str(), row);
	closeConnections();
}

std::string GET(std::string row, std::string column) {

//	Returns the value stored in column c of row r

	std::string group = findGroup(row);
	parseServerList(group);
	initializeQuorum();
	establishConnections();

	std::stringstream message;

	message << "GET(";
	message << row << ",";
	message << column << ")";

	write(message.str(), row);
	std::string response = read(row);
	closeConnections();
	return response;
}

void DELETE(std::string row, std::string column) {

//	Returns the value stored in column c of row r

	std::string group = findGroup(row);
	parseServerList(group);
	initializeQuorum();
	establishConnections();

	std::stringstream message;

	message << "DELETE(";
	message << row << ",";
	message << column << ")";

	write(message.str(), row);
	closeConnections();
}

/*	Drive Functions	*/

/*	Drive assumptions:
 * 	1)	You can't have a folder and file with the same name in the same directory.
 */

std::string showButtons(std::string directory) {

	/*	To be able to 'create a folder' in a directory, user should send the current directory to the server
	 * 	This function returns HTML for the create folder button along with the current directory the user is in.
	 * 	It also sends a textbox that user should fill the name of the new folder into.
	 * 	Should be called before any other drive function
	 */

	std::stringstream HTML;

	HTML << "<div class='header'>";
	HTML << "<div class='createFolder'>Create Folder</div>";
	HTML << "<form action='createFolder'>";
	HTML << "<input type='text' required name='folderName'>";
	HTML << "<input type='text' hidden name='directory' value='";
	HTML << directory;
	HTML << "'>";
	HTML << "<input type='submit' value='Submit'>";
	HTML << "</form>";
	HTML << "<div class='uploadFile'>Upload File</div>";
	HTML << "<form action='uploadFile' method=POST enctype=multipart/form-data accept-charset='utf-8'>";
	HTML << "<input type='file' required name='fileName'>";
	HTML << "<input type='text' hidden name='directory' value='";
	HTML << directory;
	HTML << "'>";
	HTML << "<input type='submit' value='Submit'>";
	HTML << "</form>";
	HTML << "</div>";

	return HTML.str();
}

std::string showDirectory(std::string username, std::string directory) {

	std::stringstream html;
	std::ifstream file("Drive/directory.txt");

	/*	Instead of opening file, to be replaced with file = GET(username, "directory")	*/
	/*	Sends back the HTML which the user "username" should see when at a particular directory

	1)	Read GET("username", "directory")
	2)	Iteratively read all the folders in the root folder and add to the HTML
	3)	Iteratively read all the files in the root folder and add to the HTML */

	std::string line;
	std::string mode;

	while (getline(file, line)) {

		log("Line: %s", line.c_str());

		if (line[0] == '+') {
			mode = "folders";
		}
		else if (line[0] == '*') {
			mode = "files";
		}
		else {

			/*	Add file / folder in the HTML */

			std::stringstream folder;
			std::stringstream file;

			if (line == directory)
				/*	Current directory	*/
				continue;

			if (line.find(directory + "/") == std::string::npos)
				/*	Not a sub-folder of directory	*/
				continue;

			/*	Extract the remaining path	*/
			line = line.substr(line.find(directory + "/") + directory.length() + 1, std::string::npos);

			if (line.find("/") != std::string::npos)
				/*	Remaining path has further sub-folders	*/
				continue;

			if (mode == "folders") {
				folder << "<div class='file-parent'>";
				folder << "<a class='file' href='?action=downloadFile&file=";
				folder << directory;
				folder << "/";
				folder << line;
				folder << "'>";
				folder << line;
				folder << "</a>";
				folder << "<div class='context file-context'>";
				folder << "<div class='rename'>";
				folder << "<div class='rename-image'>Rename</div>";
				folder << "<form class='rename-form' action='renameFileorFolder' method=GET>";
				folder << "<input type='text' required name='directory' value='";
				folder << directory;
				folder << "'>";
				folder << "<input type='text' required name='oldName' value='";
				folder << line;
				folder << "'>";
				folder << "<input type='text' required name='newName' placeholder='New Name'>";
				folder << "<input type='submit' value='Rename' class='mdl-button mdl-js-button mdl-button--raised mdl-button--colored mybutton'>";
				folder << "</form>";
				folder << "</div>";
				folder << "<div class='delete'>";
				folder << "<div class='delete-image'>Delete</div>";
				folder << "<form class='delete-form' action='deleteFile' method=GET>";
				folder << "<input type='text' required name='directory' value='";
				folder << directory;
				folder << "'>";
				folder << "<input type='text' required name='deleteFolder' value='";
				folder << line;
				folder << "'>";
				folder << "<p> Are you sure you want to delete? </p>";
				folder << "<input type='submit' value='Delete' class='mdl-button mdl-js-button mdl-button--raised mdl-button--colored mybutton'>";
				folder << "</form>";
				folder << "</div>";
				folder << "<div class='move'>";
				folder << "<div class='move-image'>Move</div>	";
				folder << "<form class='move-form' action='moveFileorFolder' method=GET>";
				folder << "<input type='text' required name='oldDirectory' value='";
				folder << directory;
				folder << "'>";
				folder << "<input type='text' required name='folderName' value='";
				folder << line;
				folder << "'>";
				folder << "<input type='text' required name='newDirectory' placeholder='Directory'>";
				folder << "<input type='submit' value='Move' class='mdl-button mdl-js-button mdl-button--raised mdl-button--colored mybutton'>";
				folder << "</form>";
				folder << "</div>";
				folder << "</div>";
				folder << "</div>";

				//	logVerbose("Folder HTML: %s", folder.str().c_str());
				html << folder.str();
			}
			else if (mode == "files") {
				file << "<div class='file-parent'>";
				file << "<a class='file' href='?action=downloadFile&file=";
				file << directory;
				file << "/";
				file << line;
				file << "'>";
				file << line;
				file << "</a>";
				file << "<div class='context file-context'>";
				file << "<div class='rename'>";
				file << "<div class='rename-image'>Rename</div>";
				file << "<form class='rename-form' action='renameFileorFolder' method=GET>";
				file << "<input type='text' required name='directory' value='";
				file << directory;
				file << "'>";
				file << "<input type='text' required name='oldName' value='";
				file << line;
				file << "'>";
				file << "<input type='text' required name='newName' placeholder='New Name'>";
				file << "<input type='submit' value='Rename' class='mdl-button mdl-js-button mdl-button--raised mdl-button--colored mybutton'>";
				file << "</form>";
				file << "</div>";
				file << "<div class='delete'>";
				file << "<div class='delete-image'>Delete</div>";
				file << "<form class='delete-form' action='deleteFile' method=GET>";
				file << "<input type='text' required name='directory' value='";
				file << directory;
				file << "'>";
				file << "<input type='text' required name='deleteFile' value='";
				file << line;
				file << "'>";
				file << "<p> Are you sure you want to delete? </p>";
				file << "<input type='submit' value='Delete' class='mdl-button mdl-js-button mdl-button--raised mdl-button--colored mybutton'>";
				file << "</form>";
				file << "</div>";
				file << "<div class='move'>";
				file << "<div class='move-image'>Move</div>	";
				file << "<form class='move-form' action='moveFileorFolder' method=GET>";
				file << "<input type='text' required name='oldDirectory' value='";
				file << directory;
				file << "'>";
				file << "<input type='text' required name='folderName' value='";
				file << line;
				file << "'>";
				file << "<input type='text' required name='newDirectory' placeholder='Directory'>";
				file << "<input type='submit' value='Move' class='mdl-button mdl-js-button mdl-button--raised mdl-button--colored mybutton'>";
				file << "</form>";
				file << "</div>";
				file << "</div>";
				file << "</div>";

				//	logVerbose("File HTML: %s", file.str().c_str());
				html << file.str();
			}
		}
	}

	return html.str();
}

std::string downloadFile(std::string username, std::string filePath) {

	/*	Returns the contents of the file identified by the complete file path	*/

	std::string unique = GET(username, filePath);
	std::string contents = GET(username, unique);

	return contents;
}

void moveFileorFolder(std::string username, std::string oldDirectory, std::string folderName, std::string newDirectory) {

	/*	Convert ~A/B/C/X/... to ~A/X/...
	 *
	 * 	If it is a file, it will be of the form:
	 * 	~A/B/C/X.png to ~/X.png
	 */

	/*	1)	Check if the old path exists (sanity check)
	 * 	2)	Check if the new directory is actually an existing folder	*/

	/*	While we're referring it to as folderName, it might as well be a file
	 * 	Folder case is more generic and covers when it is a file	*/

	std::ifstream file("Drive/directory.txt");

	std::string line;
	std::string mode;
	bool oldPathValid = false;
	bool newPathValid = false;

	std::string oldPath = oldDirectory + "/" + folderName;
	std::string newPath = newDirectory + "/" + folderName;

	/*	Check if the old path exists (sanity check)	*/

	while (getline(file, line)) {

		if (line == oldPath) {

			oldPathValid = true;
			break;
		}
	}

	file.clear();
	file.seekg(0, std::ios::beg);

	while (getline(file, line)) {

		if (line[0] == '+') {
			mode = "folders";
		}
		else if (line[0] == '*') {
			mode = "files";
		}
		else {
			if (mode == "folders" && line == newDirectory) {

				newPathValid = true;
				break;
			}
		}
	}

	if (!(oldPathValid && newPathValid)) {
		logVerbose("Move operation failed. Either %s or %s is not valid.", oldPath.c_str(), newDirectory.c_str());
	}

	file.clear();
	file.seekg(0, std::ios::beg);

	std::stringstream buffer;

	/*	Both of the paths are valid	*/

	while (getline(file, line)) {

		if (line[0] == '+') {
			mode = "folders";

			logVerbose("Saving %s", line.c_str());
			buffer << line << "\n";
		}
		else if (line[0] == '*') {
			mode = "files";

			logVerbose("Saving %s", line.c_str());
			buffer << line << "\n";
		}
		else {
			if (line.substr(0, oldPath.length()) == oldPath) {

				if (mode == "files") {
					std::string originalPath = line;
					std::string renamedPath = line.replace(0, oldPath.length(), newPath);
					buffer << renamedPath << "\n";

					logVerbose("Replaced %s with %s", originalPath.c_str(), renamedPath.c_str());

					/*	Change <USER, OLD FILEPATH, UNIQUE FILE ID> to
					 * 	<USER, NEW FILEPATH, UNIQUE FILE ID>
					 * 	No change to the actual file <USER, UNIQUE FILE ID, FILE CONTENTS>	*/

					std::string unique = GET(username, originalPath);
					PUT(username, renamedPath, unique);
					DELETE(username, originalPath);
				}
				else if (mode == "folders") {
					std::string originalPath = line;
					std::string renamedPath = line.replace(0, oldPath.length(), newPath);
					buffer << renamedPath << "\n";

					logVerbose("Replaced %s with %s", originalPath.c_str(), renamedPath.c_str());
				}
			}
			else {
				logVerbose("Saving %s", line.c_str());
				buffer << line << "\n";
			}
		}
	}

	std::ofstream out("Drive/directory.txt");
	out << buffer.str();
	out.close();
}

void renameFileorFolder(std::string username, std::string directory, std::string oldName, std::string newName) {

	/*	Convert ~directory/X/path to ~directory/Y/path
	 *
	 * 	If it is a file, it will be of the form:
	 * 	~directory/X.png to ~directory/Y.png
	 * 	in which case, we'll just be replacing the occurrence of the former string with the latter string
	 *
	 * 	If it is a folder, just rename in the directory.
	 * 	If it is a file, also rename in the storage.
	 */

	std::string oldPath = directory + "/" + oldName;
	std::string newPath = directory + "/" + newName;
	std::ifstream file("Drive/directory.txt");
	bool foundFileorFolder = false;
	std::string line;
	std::string mode;

	std::stringstream buffer;

	if (newName.find('/') != std::string::npos) {
		logVerbose("Cannot rename %s to %s (which is a directory)", oldName.c_str(), newName.c_str());
		return;
	}

	while (getline(file, line)) {

		if (line[0] == '+') {
			mode = "folders";

			logVerbose("Saving %s", line.c_str());
			buffer << line << "\n";
		}
		else if (line[0] == '*') {
			mode = "files";

			logVerbose("Saving %s", line.c_str());
			buffer << line << "\n";
		}
		else {
			if (line.substr(0, oldPath.length()) == oldPath) {

				foundFileorFolder = true;

				if (mode == "files") {
					std::string originalPath = line;
					std::string renamedPath = line.replace(0, oldPath.length(), newPath);
					buffer << renamedPath << "\n";

					logVerbose("Replaced %s with %s", originalPath.c_str(), renamedPath.c_str());

					/*	Change <USER, OLD FILEPATH, UNIQUE FILE ID> to
					 * 	<USER, NEW FILEPATH, UNIQUE FILE ID>
					 * 	No change to the actual file <USER, UNIQUE FILE ID, FILE CONTENTS>	*/

					std::string unique = GET(username, originalPath);
					PUT(username, renamedPath, unique);
					DELETE(username, originalPath);
				}
				else if (mode == "folders") {
					std::string originalPath = line;
					std::string renamedPath = line.replace(0, oldPath.length(), newPath);
					buffer << renamedPath << "\n";

					logVerbose("Replaced %s with %s", originalPath.c_str(), renamedPath.c_str());
				}
			}
			else {
				logVerbose("Saving %s", line.c_str());
				buffer << line << "\n";
			}
		}
	}

	std::ofstream out("Drive/directory.txt");
	out << buffer.str();
	out.close();

	if (foundFileorFolder == false) {
		logVerbose("Could not find %s. Rename operation failed.", oldPath.c_str());
	}
}

void deleteFolder(std::string username, std::string directory, std::string folderName) {

	/*	Delete folder 'folderName' from 'directory' for user 'username'	*/

	/*	TODO:
	 * 	Recursively delete all the files and sub folders in that folder
	 * 	For deleting files:	DELETE(username, file)
	 * 	For deleting folders: Just update the directory file (GET(username, directory)
	 */

	/*	1)	Check if folder exists in the directory listing and delete it	*/

	std::ifstream file("Drive/directory.txt");
	std::stringstream buffer;

	std::string line;
	bool folderExists = false;

	std::string mode;

	std::string path = directory + "/" + folderName;

	while (getline(file, line)) {

		if (line[0] == '+') {
			mode = "folders";
			buffer << line << "\n";
		}
		else if (line[0] == '*') {
			mode = "files";
			buffer << line << "\n";
		}
		else {

			if (line == path) {
				folderExists = true;
				logVerbose("Deleting %s", line.c_str());
				continue;
			}
			else if (line.substr(0, path.length()) == path) {

				if (mode == "files") {
					logVerbose("Deleting file %s", line.c_str());

//					std::string unique = GET(username, path);
//					DELETE(username, unique);
//					DELETE(username, path);
				}
				else if (mode == "folders") {
					logVerbose("Deleting folder %s", line.c_str());
				}
			}
			else if (line != "" && line != "\n"){

				/*	Empty lines are deleted	(Well, this block only keeps the non-empty lines	*/
				logSuccess("Saving %s", line.c_str());
				buffer << line << "\n";
			}
		}
	}

	std::ofstream out("Drive/directory.txt");
	out << buffer.str();
	out.close();

	if (folderExists == false) {
		logVerbose("Could not find %s. Folder deletion failed.", (directory + "/" + folderName).c_str());
	}
}

void deleteFile(std::string username, std::string directory, std::string fileName) {

	/*	1)	Check if file exists in the directory listing and delete it	*/

	std::ifstream file("Drive/directory.txt");
	std::stringstream buffer;

	std::string line;
	bool fileExists = false;

	while (getline(file, line)) {

		if (line == (directory + "/" + fileName)) {
			fileExists = true;
		}
		else if (line != "" && line != "\n"){
			buffer << line << "\n";
		}
	}

	std::ofstream out("Drive/directory.txt");
	out << buffer.str();
	out.close();

	if (fileExists == false) {
		logVerbose("Could not find %s. File deletion failed.", (directory + "/" + fileName).c_str());
	}
	else {
		std::string unique = GET(username, (directory + "/" + fileName));
		DELETE(username, unique);
		DELETE(username, (directory + "/" + fileName));
	}

	return;
}

void createFolder(std::string username, std::string directory, std::string folderName) {

	/*	Creates folder 'foldername' in 'directory' for user 'username'	*/
	/*	Instead of opening file, to be replaced with file = GET(username, "directory")	*/

	std::ifstream file("Drive/directory.txt");

	std::string line;
	bool directoryExists = false;

	while (getline(file, line)) {

		if (line == directory) {

			/*	Check if the directory, that the user wants to create the folder in, exists	*/
			directoryExists = true;
			break;
		}
	}

	file.clear();
	file.seekg(0, std::ios::beg);

	while (getline(file, line)) {

		if (line == directory + "/" + folderName) {

			/*	Check in case the folder already exists	*/
			logVerbose("The folder already exists (%s). Folder creation failed", line.c_str());
			return;
		}
	}

	if (directoryExists == false) {
		logVerbose("Given directory, %s does not exist. Folder creation failed", directory.c_str());
		return;
	}

	/*	The directory was found and the folder is not already there	*/

	std::ofstream append("Drive/directory.txt", std::ios_base::app | std::ios_base::out);
	append << "\n+Folders\n";
	append << directory + "/" + folderName;

	append.close();
	logVerbose("Folder %s created in %s", folderName.c_str(), directory.c_str());
}

void uploadFile(std::string username, std::string directory, std::string fileName, std::string fileContents) {

	/*	Uploads file 'fileName' in the 'drectory', both in the directory file and the storage (key - value)
	 * 	Stores in the form of <FILENAME, Unique ID> and another record <Unique ID, File Contents>	*/

	/*	1)	Add the file to the directory	*/

	std::ifstream file("Drive/directory.txt");
	std::string line;
	bool directoryExists = false;

	while (getline(file, line)) {

		if (line == directory) {

			/*	Check if the directory, that the user wants to upload the file in, exists	*/
			directoryExists = true;
			break;
		}
	}

	file.clear();
	file.seekg(0, std::ios::beg);

	while (getline(file, line)) {

		if (line == directory + "/" + fileName) {

			/*	Check in case the file already exists	*/
			logVerbose("The file already exists (%s). File upload failed", line.c_str());
			return;
		}
	}

	if (directoryExists == false) {
		logVerbose("Given directory, %s does not exist. File upload failed", directory.c_str());
		return;
	}

	/*	The directory was found and the file is not already there	*/

	std::ofstream append("Drive/directory.txt", std::ios_base::app | std::ios_base::out);
	append << "\n*Files\n";
	append << directory + "/" + fileName;

	append.close();
	logVerbose("File %s uploaded in %s", fileName.c_str(), directory.c_str());

	/*	2)	Since Step 1) passed, add this as a key value pair	*/

	std::string unique = time();
	PUT(username, directory + "/" + fileName, unique);
	PUT(username, unique, fileContents);
}

/*	Drive functions end	*/

int main(int argc, char *argv[]) {

	config = std::string(argv[1]);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, terminate);

//	moveFileorFolder("garvit", "root", "bar", "root/foo/code");

//	renameFileorFolder("garvit", "root", "cat", "bar");
//	renameFileorFolder("garvit", "roots", "image.png", "stupid.png");

//	moveFileorFolder("garvit", "root", "cat", "root/foo");

//	log("%s", showButtons("root/foo/bar").c_str());

//	logVerbose("%s", showDirectory("garvit", "root/bar").c_str());

//	createFolder("garvit", "root/bar", "mySecrets");

//	deleteFile("garvit", "root/foo", "records.xlsx");
//	uploadFile("garvit", "root/bar", "coffee-beans.txt", "beans are so very amazing!");

//	std::string content = downloadFile("garvit", "root/bar/coffee-beans.txt");

//	logVerbose("Contents: %s", content.c_str());

//	deleteFolder("garvit", "root", "bar");

//	deleteFile("garvit", "root/bar", "coffee.txt");
//	logVerbose("My file AFTER: %s", downloadFile("garvit", "root/bar/coffee.txt").c_str());

	PUT("garvit", "bar", "data");
	logVerbose("Value read: %s", GET("foo", "bar").c_str());

// 	DELETE("garvit", "bar");
//	logVerbose("Value read: %s", GET("foo", "bar").c_str());

}
