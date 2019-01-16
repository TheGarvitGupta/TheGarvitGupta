#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <chrono>
#include <regex>
#include "functions.h"
#include "structures.h"
#include "mutex"
#include <sys/socket.h>
#include <ctime>


// DEBUG: Mail Structs

struct header {
	std::string subject;
	std::string from;
	std::string date;
	bool unread;
	long long timestamp;	// used for operator (i.e. sorting)

	//TODO: add additional header fields we would like to display

	bool operator==(const header &o) const {
      return this->timestamp == o.timestamp;
	}
	// switched so we see most recent emails first!
	bool operator<(const header &o) const {
	  return this->timestamp > o.timestamp;
	}

	bool operator>(const header &o) const {
	  return this->timestamp < o.timestamp;
	}

	void clear() {
		subject = "";
		from = "";
		date = "";
		unread = true;
		timestamp = -1;
	}

	void assign(std::string subject, std::string from, std::string date, bool unread, int timestamp) {
		this->subject = subject;
		this->from = from;
		this->date = date;
		this->unread = unread;
		this->timestamp = timestamp;
	}
};

struct contents {
	std::string text;
	//TODO: add additional mail contents fxnality (e.g. attachments, images)
};


//TODO TO-CHECK should multiple users be able to login from the same browser???

//TO-INTEGRATE
// None is the default username. DB shouldn't be getting none. DB should check this!!

//	Forward Declarations

std::string read(std::string);
std::string read(int);
void write(std::string, std::string);
std::string findGroup(std::string);
std::string templatePrefix(std::string);
std::string templateSuffix();
void sendErrorPage(std::string username, std::string message);
std::vector<std::string> configLines;
std::string showMessage(std::string message);

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
serverStruct master_server, smtp_server;
std::mutex lock;

std::map <int, int> servertoload; // mapping from port numbers to individual load
std::map <int, bool> servertoactive; // mapping from port numbers to active state
std::map <int, std::string> servertotablet; // mapping from port numbers to individual tablets

int portNum = 8080;
bool verbose = false;
std::string okHeader("HTTP/1.1 200 OK\r\nServer: 505Team11\r\nContent-Type: text/html\r\nConnection: close\r\n");

//	Functions

// Function to populate the lines of the config file so that it's read only once
void populateConfigLines(){

	configLines.clear();
	std::ifstream file(config);
	std::string line;

	if (!file) {
		sendErrorPage("" , "Couldn't read file " + config);
		panic("Error: Could not read file <%s>", config.c_str());
	}

	while (getline(file, line)){
		configLines.push_back(line);
	}

	logSuccess("Length of the configLines %zu", configLines.size());
	file.close();
}


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

std::string readSMTP(int socket) {

//	Read from a particular server
	bool cr, lf = false;

	std::string buffer;
	while (1) {
		char character;
		printf("%c", character);
		int returnValue = read(socket, &character, sizeof(character));

		if (returnValue <= 0) {

			logVerbose("SMTP seems to have crashed!");
//			sleep(5);
			break;
		}

		buffer.push_back(character);
		if (character == '\r') cr = true;
		else if (character == '\n' && cr) break;
		else cr = false;
	}

	if (buffer.length() == 1) buffer = "-1";
	logReceive("%s", buffer.c_str());

	return buffer;
}

void sendNOOP(int sockfd) {
			std::string message = "NOOP\r\n";
			int length = message.size();
			logSend("%s (%d)", message.c_str(), (int)message.length());
			//write(smtp_server.socket, message.c_str(), message.length());
			printf("SMTP SERVER| IP: %s, PORT: %d\n", inet_ntoa(smtp_server.address.sin_addr), ntohs(smtp_server.address.sin_port));
			int bytes_written = dprintf(sockfd, "%s", message.c_str());
			printf("bytes written: %d | smtp_server socket: %d\n", bytes_written, sockfd);
			//if (sendto(smtp_server.socket, message.c_str(), message.length(), 0, (struct sockaddr*) &smtp_server.address, sizeof(smtp_server.address)) == -1) perror("send error");
			//send(sockfd, (char*) message.c_str(), message.length(), 0);
			// for (int j = 0; j < message.size(); j++) {
			// 	write(smtp_server.socket, &message[j], sizeof(message[j]));
			// }

			std::string response;
			response = readSMTP(sockfd);
			//printf("response: %s | smtp_server socket: %d\n", response.c_str(), sockfd);

			if (response == "-1") {

				int sock = socket(PF_INET, SOCK_STREAM, 0);


				if (connect(sock, (struct sockaddr*) &smtp_server.address, sizeof(struct sockaddr)) < 0) {
					logVerbose("Could not establish connection with SMTP server: %d", ntohs(smtp_server.address.sin_port));
					smtp_server.active = false;
					close(sock);
				}
				else {
					smtp_server.socket = sock;
					smtp_server.active = true;
					std::string response;
					response = readSMTP(sockfd);
					logReceive("%s", response.c_str());	// 220 localhost service ready
				}
			}
}

int sendHELO(int sockfd) {
	std::string message = "HELO PennCloud\r\n";
	int length = message.size();
	logSend("%s (%d)", message.c_str(), (int)message.length());
	printf("SMTP SERVER| IP: %s, PORT: %d\n", inet_ntoa(smtp_server.address.sin_addr), ntohs(smtp_server.address.sin_port));
	int bytes_written = dprintf(sockfd, "%s", message.c_str());
	printf("bytes written: %d | smtp_server socket: %d\n", bytes_written, sockfd);

	std::string response;
	response = readSMTP(sockfd);

	if (response.substr(0, 3).compare("250") != 0) return -1;
	else return 0;
}

int sendMAILFROM(int sockfd, std::string username) {
	std::string message = "MAIL FROM:<" + username + "@PennCloud>" + "\r\n";
	int length = message.size();
	logSend("%s (%d)", message.c_str(), (int)message.length());
	printf("SMTP SERVER| IP: %s, PORT: %d\n", inet_ntoa(smtp_server.address.sin_addr), ntohs(smtp_server.address.sin_port));
	int bytes_written = dprintf(sockfd, "%s", message.c_str());
	printf("bytes written: %d | smtp_server socket: %d\n", bytes_written, sockfd);

	std::string response;
	response = readSMTP(sockfd);

	if (response.substr(0, 3).compare("250") != 0) return -1;
	else return 0;
}

int sendRCPTTO(int sockfd, std::string recipient) {
	// POST req. converts '@' to '%40' so I will undo this conversion
	std::string recipient_with_at;
	int index_of_at = recipient.find("%40");
	printf("i_@: %d\n", index_of_at);
	if (index_of_at != std::string::npos) {
		recipient_with_at = recipient.substr(0, index_of_at);
		recipient_with_at += "@";
		recipient_with_at += recipient.substr(index_of_at + 3, recipient.size() - index_of_at - 3);
	} else recipient_with_at = recipient;

	std::string message = "RCPT TO:<" + recipient_with_at + ">\r\n";
	int length = message.size();
	logSend("%s (%d)", message.c_str(), (int)message.length());
	printf("SMTP SERVER| IP: %s, PORT: %d\n", inet_ntoa(smtp_server.address.sin_addr), ntohs(smtp_server.address.sin_port));
	int bytes_written = dprintf(sockfd, "%s", message.c_str());
	printf("bytes written: %d | smtp_server socket: %d\n", bytes_written, sockfd);

	std::string response;
	response = readSMTP(sockfd);
	printf("RCPT TO: response is %s\n", response.c_str());
	if (response.substr(0, 3).compare("250") != 0) return -1;
	else return 0;
}

int sendDATA(int sockfd) {
	std::string message = "DATA\r\n";
	int length = message.size();
	logSend("%s (%d)", message.c_str(), (int)message.length());
	printf("SMTP SERVER| IP: %s, PORT: %d\n", inet_ntoa(smtp_server.address.sin_addr), ntohs(smtp_server.address.sin_port));
	int bytes_written = dprintf(sockfd, "%s", message.c_str());
	printf("bytes written: %d | smtp_server socket: %d\n", bytes_written, sockfd);

	std::string response;
	response = readSMTP(sockfd);

	if (response.substr(0, 3).compare("354") != 0) return -1;
	else return 0;
}

int sendDATAInput(int sockfd, std::string input) {
	std::string message = "\r\n" + input + "\r\n.\r\n";	// append <CRLF>.<CRLF>
	int length = message.size();
	logSend("%s (%d)", message.c_str(), (int)message.length());
	printf("SMTP SERVER| IP: %s, PORT: %d\n", inet_ntoa(smtp_server.address.sin_addr), ntohs(smtp_server.address.sin_port));
	int bytes_written = dprintf(sockfd, "%s", message.c_str());
	printf("bytes written: %d | smtp_server socket: %d\n", bytes_written, sockfd);

	std::string response;
	response = readSMTP(sockfd);

	if (response.substr(0, 3).compare("250") != 0) return -1;
	else return 0;
}

int sendQUIT(int sockfd) {
	std::string message = "QUIT\r\n";
	int length = message.size();
	logSend("%s (%d)", message.c_str(), (int)message.length());
	printf("SMTP SERVER| IP: %s, PORT: %d\n", inet_ntoa(smtp_server.address.sin_addr), ntohs(smtp_server.address.sin_port));
	int bytes_written = dprintf(sockfd, "%s", message.c_str());
	printf("bytes written: %d | smtp_server socket: %d\n", bytes_written, sockfd);

	std::string response;
	response = readSMTP(sockfd);
	printf("exits quit\n");

	if (response.substr(0, 3).compare("250") != 0) return -1;
	else return 0;
}

// returns socket no.
int establishSMTPConnection() {
	int sock = socket(PF_INET, SOCK_STREAM, 0);

	if (smtp_server.active == false) {


		if (connect(sock, (struct sockaddr*) &smtp_server.address, sizeof(struct sockaddr)) < 0) {
			sendErrorPage("", "Could not establish connection with Email Server!");
			smtp_server.active = false;
			close(sock);
			panic("Could not establish connection with SMTP Server: %d", ntohs(smtp_server.address.sin_port));
		} else {
			smtp_server.socket = sock;
			smtp_server.active = true;
			logSuccess("Connected to SMTP Server: %d", ntohs(smtp_server.address.sin_port));
			printf("establishSMTPConnection on socket (%d == %d)\n", smtp_server.socket, sock);
			std::string response;
			response = readSMTP(sock);
			logReceive("%s", response.c_str());	// 220 localhost service ready
			sendNOOP(sock);
			return sock;
		}
	} else {
		sendNOOP(sock);
		return sock;
	}
}

void findSMTP() {

	for (int i = 0; i < configLines.size(); i++) {
		std::string line = configLines.at(i);

		if (line[0] != '#') {
			if (line.find("SMTP") != std::string::npos)
			{
				serverStruct server;

				bzero(&server.address, sizeof(server.address));
				server.address.sin_family = AF_INET;

				std::stringstream port(line.substr(line.find(':') + 1, line.find(",") - line.find(':') - 1));
				int portNumber = 0;
				port >> portNumber;
				printf("html-server writing to SMTP server on port: %d\n", portNumber);
				server.address.sin_port = htons(portNumber);

				inet_pton(AF_INET, line.substr(0, line.find(':')).c_str(), &(server.address.sin_addr));

				smtp_server = server;
			}
		}
	}

}


void establishConnections() {

	//	For all the servers in the list, tries to establish connection and refresh the socket.
	//	If a connection is already established, verifies by sending a NOOP

	for (int i = 0; i < servers.size(); i++) {

		if (servers[i].active == false) {

			int sock = socket(PF_INET, SOCK_STREAM, 0);


			if(connect(sock, (struct sockaddr*) &servers[i].address, sizeof(servers[i].address)) < 0) {

				logVerbose("Could not establish connection with %d", ntohs(servers[i].address.sin_port));
				servers[i].active = false;
				close(sock);
			}
			else {
				servers[i].socket = sock;
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


				if(connect(sock, (struct sockaddr*) &servers[i].address, sizeof(servers[i].address)) < 0) {

					logVerbose("Could not establish connection with %d", ntohs(servers[i].address.sin_port));
					servers[i].active = false;
					close(sock);
				}
				else {
					servers[i].socket = sock;
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

void findMaster() {

	for (int i = 0; i < configLines.size(); i++) {
		std::string line = configLines.at(i);

		if (line[0] != '#') {
			if (line.find("Master") != std::string::npos)
			{
				serverStruct server;

				bzero(&server.address, sizeof(server.address));
				server.address.sin_family = AF_INET;

				std::stringstream port(line.substr(line.find(':') + 1, line.find(",") - line.find(':') - 1));
				int portNumber = 0;
				port >> portNumber;
				server.address.sin_port = htons(portNumber);

				inet_pton(AF_INET, line.substr(0, line.find(':')).c_str(), &(server.address.sin_addr));

				master_server = server;
			}
		}
	}

}

void parseServerList(std::string group) {

//	Finds server belonging to the given group in storage.md file and stores them in the vector servers

	servers.clear();
	std::string line;

	for (int i = 0; i < configLines.size(); i++) {
		std::string line = configLines.at(i);

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
		sendErrorPage("", "Unable to find database servers");
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
		sendErrorPage("", "Couldn't contact all the backend servers");
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
		sendErrorPage("", "Couldn't contact all backend servers");
		panic("Could only write to %d/%d servers", count, W);
	}
}

std::string findGroup(std::string value) {

	//return "G1";

//	Parses server list in the storage.txt configuration file and find master and then ask the master for group

	std::stringstream masterIP;
	std::stringstream masterPort;

	bool masterFound = false;

	for (int i = 0; i < configLines.size(); i++) {
		std::string line = configLines.at(i);

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
		sendErrorPage("", "Unable to find Master");
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
		sendErrorPage("", "Could not establish connection with the Master Server");
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

	close(sock);
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

void handleAdmin(int comm_fd, std::string username, std::map<std::string, std::string> postParam) {

// Returns string with information for all servers
	//log("%s", "in handle admin function");
	//	Parses server list in the storage.txt configuration file and find master and then ask the master for group

	std::stringstream heartbeatIP;
	std::stringstream heartbeatPort;

	bool heartbeatFound = false;

	for (int i = 0; i < configLines.size(); i++) {
		std::string line = configLines.at(i);

		if (line[0] != '#') {

			if (line.substr(line.find(", ") + 2, std::string::npos) == "Heartbeat") {

				heartbeatIP << line.substr(0, line.find(":"));
				heartbeatPort << line.substr(line.find(":") + 1, line.find(",") - line.find(":") - 1);

				logVerbose("Heartbeat found at %s:%s", heartbeatIP.str().c_str(), heartbeatPort.str().c_str());
				heartbeatFound = true;
				break;
			}
		}
	}

	if (heartbeatFound == false) {
		sendErrorPage(username , "Heartbeat server was not found");
		panic("Heartbeat was not found in the configuration file (%s)", config.c_str());
	}

//	Convert the string to the address

	struct sockaddr_in heartbeat;
	bzero(&heartbeat, sizeof(heartbeat));
	heartbeat.sin_family = AF_INET;
	int port = 0;
	heartbeatPort >> port;
	heartbeat.sin_port = htons(port);
	inet_pton(AF_INET, heartbeatIP.str().c_str(), &(heartbeat.sin_addr));

//	Connect to the heartbeat via TCP

	int sock = socket(PF_INET, SOCK_STREAM, 0);

	if(connect(sock, (struct sockaddr*) &heartbeat, sizeof(heartbeat)) < 0) {
		sendErrorPage(username, "Could not establish connection with the heartbeat server");
		panic("Could not establish connection with the heartbeat at %d", ntohs(heartbeat.sin_port));
	}

	log("Established connection with the heartbeat");
	log("Read from the heartbeat: %s", read(sock).c_str());
	std::string response;

	std::string action;
	std::string server;
	if(postParam.find("action") != postParam.end()){
		action = postParam["action"];
	}

	if(action.compare("disable") == 0)
	{
		if(postParam.find("server") != postParam.end()){
			server = postParam["server"];
		}
		response = "DISABLE" + server;
		logVerbose("Sending: %s (%d)", response.c_str(), (int)response.length());

		int length = response.length();
		write(sock, &length, sizeof(length));

		for (int j = 0; j < response.length(); j++) {
			write(sock, &response[j], sizeof(response[j]));
		}

		// modify the tablet here
		servertoactive[stoi(server)] = false;
		servertoload[stoi(server)] = 0;
		servertotablet[stoi(server)] = "::empty::";

		std::stringstream tablebuilder;

		tablebuilder << "<table class='table table-striped'>";
		tablebuilder << "<tr><th>Server Port</th><th>Active</th><th>Current Load</th><th>Tablet Information</th><th>Disable/Enable</th></tr>";
		std::map<int, bool>::iterator it1 = servertoactive.begin();
		while(it1 != servertoactive.end())
		{
			tablebuilder << "<tr>";
			tablebuilder << "<td>";
			tablebuilder << it1->first;
			tablebuilder << "</td>";
			tablebuilder << "<td>";
			if(it1->second)
				tablebuilder << "YES";
			else
				tablebuilder << "NO";
			tablebuilder << "</td>";
			tablebuilder << "<td>";
			tablebuilder << float(float(servertoload[it1->first]) / 1024);
			tablebuilder << " kB";
			tablebuilder << "</td>";
			tablebuilder << "<td>";
			tablebuilder << "<a href='/admin?action=showTablet&server=";
			tablebuilder << it1->first;
			tablebuilder << "' class='myButton'>Show</a>";
			tablebuilder << "</td>";
			tablebuilder << "<td>";
			if(it1->second) {
				tablebuilder << "<a href='/admin?action=disable&server=";
				tablebuilder << it1->first;
				tablebuilder << "' class='myButton'>Stop</a>";
			}
			else {
				tablebuilder << "<a href='/admin?action=restart&server=";
				tablebuilder << it1->first;
				tablebuilder << "' class='myButton'>Start</a>";
			}
			tablebuilder << "</td>";
			tablebuilder << "</tr>";
			it1++;
		}
		tablebuilder << "</table>";

		std::string table_string = tablebuilder.str();

		std::stringstream body_stream;
		body_stream << templatePrefix(username);
		body_stream << "<link href='Views/css/bootstrap.min.css' rel='stylesheet'>";
		body_stream << table_string;
		body_stream << templateSuffix();

		std::string body = body_stream.str();
		std::string headers(okHeader);

		headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";

		if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
			fprintf(stderr, "%d Thread Couldn't Write Register Headers\n", comm_fd);
		}
		if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
			fprintf(stderr, "%d Thread Couldn't Write Register Body\n", comm_fd);
		}

		if(verbose){
			std::cout << "Sending--->" << headers << body << std::endl;
		}

	}

	if(action.compare("restart") == 0)
	{
		if(postParam.find("server") != postParam.end()){
			server = postParam["server"];
		}
		response = "RESTART" + server;
		logVerbose("Sending: %s (%d)", response.c_str(), (int)response.length());

		int length = response.length();
		write(sock, &length, sizeof(length));

		for (int j = 0; j < response.length(); j++) {
			write(sock, &response[j], sizeof(response[j]));
		}

		// modify the tablet here
		servertoactive[stoi(server)] = true;
		servertoload[stoi(server)] = 0;
		servertotablet[stoi(server)] = "::empty::";

		std::stringstream tablebuilder;

		tablebuilder << "<table class='table table-striped'>";
		tablebuilder << "<tr><th>Server Port</th><th>Active</th><th>Current Load</th><th>Tablet Information</th><th>Disable/Enable</th></tr>";
		std::map<int, bool>::iterator it1 = servertoactive.begin();
		while(it1 != servertoactive.end())
		{
			tablebuilder << "<tr>";
			tablebuilder << "<td>";
			tablebuilder << it1->first;
			tablebuilder << "</td>";
			tablebuilder << "<td>";
			if(it1->second)
				tablebuilder << "YES";
			else
				tablebuilder << "NO";
			tablebuilder << "</td>";
			tablebuilder << "<td>";
			tablebuilder << float(float(servertoload[it1->first]) / 1024);
			tablebuilder << " kB";
			tablebuilder << "</td>";
			tablebuilder << "<td>";
			tablebuilder << "<a href='/admin?action=showTablet&server=";
			tablebuilder << it1->first;
			tablebuilder << "' class='myButton'>Show</a>";
			tablebuilder << "</td>";
			tablebuilder << "<td>";
			if(it1->second) {
				tablebuilder << "<a href='/admin?action=disable&server=";
				tablebuilder << it1->first;
				tablebuilder << "' class='myButton'>Stop</a>";
			}
			else {
				tablebuilder << "<a href='/admin?action=restart&server=";
				tablebuilder << it1->first;
				tablebuilder << "' class='myButton'>Start</a>";
			}
			tablebuilder << "</td>";
			tablebuilder << "</tr>";
			it1++;
		}
		tablebuilder << "</table>";

		std::string table_string = tablebuilder.str();

		std::stringstream body_stream;
		body_stream << templatePrefix(username);
		body_stream << "<link href='Views/css/bootstrap.min.css' rel='stylesheet'>";
		body_stream << table_string;
		body_stream << templateSuffix();

		std::string body = body_stream.str();
		std::string headers(okHeader);

		headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";

		if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
			fprintf(stderr, "%d Thread Couldn't Write Register Headers\n", comm_fd);
		}
		if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
			fprintf(stderr, "%d Thread Couldn't Write Register Body\n", comm_fd);
		}

		if(verbose){
			std::cout << "Sending--->" << headers << body << std::endl;
		}

	}


	else if(action.compare("readTablet") == 0)
	{
		std::string server;
		std::string final_value;
		std::string row;
		std::string col;
		if(postParam.find("server") != postParam.end()){
			server = postParam["server"];
		}

		if(postParam.find("row") != postParam.end()){
			row = postParam["row"];
		}

		if(postParam.find("col") != postParam.end()){
			col = postParam["col"];
		}

		int primary_delim_pos;
		int delim_pos;
		logVerbose("server recds:  %s", server.c_str());
		logVerbose("row recds:  %s", row.c_str());
		logVerbose("col recds:  %s", col.c_str());
		std::string all_contents = servertotablet[stoi(server)];
		logVerbose("all contetns:  %s", all_contents.c_str());
		while(all_contents.find("|||") != std::string::npos)
		{
			primary_delim_pos = all_contents.find("|||");
			std::string line = all_contents.substr(0, primary_delim_pos);
			delim_pos = line.find(":::");
			std::string row_key = line.substr(0, delim_pos);
			line.erase(0, delim_pos+3);
			delim_pos = line.find(":::");
			std::string col_key = line.substr(0, delim_pos);
			line.erase(0, delim_pos+3);
			std::string value = line;
			if(row_key == row && col_key == col)
			{
				final_value = value;
				break;
			}
			all_contents.erase(0, primary_delim_pos + 3);
		}

		std::stringstream body_stream;
		body_stream << templatePrefix(username);
		body_stream << "<link href='Views/css/bootstrap.min.css' rel='stylesheet'>";
		body_stream << final_value;
		body_stream << templateSuffix();

		std::string body = body_stream.str();
		std::string headers(okHeader);

		headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";

		if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
			fprintf(stderr, "%d Thread Couldn't Write Register Headers\n", comm_fd);
		}
		if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
			fprintf(stderr, "%d Thread Couldn't Write Register Body\n", comm_fd);
		}

		if(verbose){
			std::cout << "Sending--->" << headers << body << std::endl;
		}
	}

	else if(action.compare("showTablet") == 0)
	{
		if(postParam.find("server") != postParam.end()){
			server = postParam["server"];
		}

		// build the table for displaying the tablet here
		std::stringstream tablebuilder;

		tablebuilder << "<table class='table table-striped'>";
		tablebuilder << "<tr><th>User</th><th>Column</th><th>Value</th></tr>";
		int primary_delim_pos;
		int delim_pos;
		std::string all_contents = servertotablet[stoi(server)];
		logVerbose("server in show tablet: %s", server.c_str());
		while(all_contents.find("|||") != std::string::npos)
		{
			primary_delim_pos = all_contents.find("|||");
			std::string line = all_contents.substr(0, primary_delim_pos);
			delim_pos = line.find(":::");
			std::string row_key = line.substr(0, delim_pos);
			line.erase(0, delim_pos+3);
			delim_pos = line.find(":::");
			std::string col_key = line.substr(0, delim_pos);
			line.erase(0, delim_pos+3);
			std::string value = line;
			tablebuilder << "<tr>";
			tablebuilder << "<td>";
			tablebuilder << row_key;
			tablebuilder << "</td>";
			tablebuilder << "<td>";
			tablebuilder << col_key;
			tablebuilder << "</td>";
			tablebuilder << "<td>";
			tablebuilder << "<a href='/admin?action=readTablet&server=";
			tablebuilder << server;
			tablebuilder << "&row=";
			tablebuilder << row_key;
			tablebuilder << "&col=";
			tablebuilder << col_key;
			tablebuilder << "' class='myButton'>Show contents</a>";
			tablebuilder << "</td>";
			tablebuilder << "</tr>";
			all_contents.erase(0, primary_delim_pos + 3);
		}
		tablebuilder << "</table>";

		std::string table_string = tablebuilder.str();

		std::stringstream body_stream;
		body_stream << templatePrefix(username);
		body_stream << "<link href='Views/css/bootstrap.min.css' rel='stylesheet'>";
		body_stream << table_string;
		body_stream << templateSuffix();

		std::string body = body_stream.str();
		std::string headers(okHeader);

		headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";

		if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
			fprintf(stderr, "%d Thread Couldn't Write Register Headers\n", comm_fd);
		}
		if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
			fprintf(stderr, "%d Thread Couldn't Write Register Body\n", comm_fd);
		}

		if(verbose){
			std::cout << "Sending--->" << headers << body << std::endl;
		}
	}

	else if(action.compare("mainAdmin") == 0) {

		response = "ADMININFO";

		logVerbose("Sending: %s (%d)", response.c_str(), (int)response.length());

		int length = response.length();
		write(sock, &length, sizeof(length));

		for (int j = 0; j < response.length(); j++) {
			write(sock, &response[j], sizeof(response[j]));
		}

		std::string value = read(sock);

		log("Read value: %s from the heartbeat", value.c_str());

		// SPLIT value into multiple components here and generate HTML

		int delim_pos = value.find("|*|*|");
		std::string active_information = value.substr(0, delim_pos);
		value.erase(0, delim_pos+5);
		delim_pos = value.find("|*|*|");
		std::string load_information = value.substr(0, delim_pos);
		value.erase(0, delim_pos+5);
		std::string tablet_information = value;

		logVerbose("tablet information: %s", tablet_information.c_str());

		std::vector <std::string> active_tokens;
		while(active_information.find("|:|:|") != std::string::npos)
		{
			delim_pos = active_information.find("|:|:|");
			active_tokens.push_back(active_information.substr(0, delim_pos));
			active_information.erase(0, delim_pos+5);
		}

		for (int i=0; i<active_tokens.size(); i++)
		{
			delim_pos = active_tokens[i].find("*:*");
			std::string server_index = active_tokens[i].substr(0, delim_pos);
			active_tokens[i].erase(0, delim_pos+3);
			servertoactive[stoi(server_index)] = stoi(active_tokens[i]);
		}

		std::vector <std::string> load_tokens;
		while(load_information.find("|:|:|") != std::string::npos)
		{
			delim_pos = load_information.find("|:|:|");
			load_tokens.push_back(load_information.substr(0, delim_pos));
			load_information.erase(0, delim_pos+5);
		}

		for (int i=0; i<load_tokens.size(); i++)
		{
			delim_pos = load_tokens[i].find("*:*");
			std::string server_index = load_tokens[i].substr(0, delim_pos);
			load_tokens[i].erase(0, delim_pos+3);
			servertoload[stoi(server_index)] = stoi(load_tokens[i]);
		}

		std::vector <std::string> tablet_tokens;
		while(tablet_information.find("|:|:|") != std::string::npos)
		{
			delim_pos = tablet_information.find("|:|:|");
			tablet_tokens.push_back(tablet_information.substr(0, delim_pos));
			tablet_information.erase(0, delim_pos+5);
		}

		for (int i=0; i<tablet_tokens.size(); i++)
		{
			delim_pos = tablet_tokens[i].find("*:*");
			std::string server_index = tablet_tokens[i].substr(0, delim_pos);
			tablet_tokens[i].erase(0, delim_pos+3);
			servertotablet[stoi(server_index)] = tablet_tokens[i];
		}

		std::stringstream tablebuilder;

		tablebuilder << "<table class='table table-striped'>";
		tablebuilder << "<tr><th>Server Port</th><th>Active</th><th>Current Load</th><th>Tablet Information</th><th>Disable/Enable</th></tr>";
		std::map<int, bool>::iterator it1 = servertoactive.begin();
		while(it1 != servertoactive.end())
		{
			tablebuilder << "<tr>";
			tablebuilder << "<td>";
			tablebuilder << it1->first;
			tablebuilder << "</td>";
			tablebuilder << "<td>";
			if(it1->second)
				tablebuilder << "YES";
			else
				tablebuilder << "NO";
			tablebuilder << "</td>";
			tablebuilder << "<td>";
			tablebuilder << float(float(servertoload[it1->first]) / 1024);
			tablebuilder << " kB";
			tablebuilder << "</td>";
			tablebuilder << "<td>";
			tablebuilder << "<a href='/admin?action=showTablet&server=";
			tablebuilder << it1->first;
			tablebuilder << "' class='myButton'>Show</a>";
			tablebuilder << "</td>";
			tablebuilder << "<td>";
			if(it1->second) {
				tablebuilder << "<a href='/admin?action=disable&server=";
				tablebuilder << it1->first;
				tablebuilder << "' class='myButton'>Stop</a>";
			}
			else {
				tablebuilder << "<a href='/admin?action=restart&server=";
				tablebuilder << it1->first;
				tablebuilder << "' class='myButton'>Start</a>";
			}
			tablebuilder << "</td>";
			tablebuilder << "</tr>";
			it1++;
		}
		tablebuilder << "</table>";

		std::string table_string = tablebuilder.str();

		std::stringstream body_stream;
		body_stream << templatePrefix(username);
		body_stream << "<link href='Views/css/bootstrap.min.css' rel='stylesheet'>";
		body_stream << table_string;
		body_stream << templateSuffix();

		std::string body = body_stream.str();
		std::string headers(okHeader);

		headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";

		if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
			fprintf(stderr, "%d Thread Couldn't Write Register Headers\n", comm_fd);
		}
		if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
			fprintf(stderr, "%d Thread Couldn't Write Register Body\n", comm_fd);
		}

		if(verbose){
			std::cout << "Sending--->" << headers << body << std::endl;
		}
	}

	close(sock);
}

/*	Drive Functions	*/

/*	Drive assumptions:
 * 	1)	You can't have a folder and file with the same name in the same directory.
 */

std::string templatePrefix(std::string username) {

	std::stringstream HTML;
	std::string line;

	std::ifstream file_first("Views/index-first.html");
	std::ifstream file_second("Views/index-second.html");

	while(getline(file_first, line)) {
		HTML << line;
	}

	HTML << username;

	while(getline(file_second, line)) {
		HTML << line;
	}

	file_first.close();
	file_second.close();

	return HTML.str();
}

std::string templateSuffix() {

	return "</div></body></html>";
}

std::ifstream readDirectory(std::string username) {

	system("mkdir Drive/");

	lock.lock();

	std::string directoryContents = GET(username, "directory");

	if (directoryContents == "0") {
		directoryContents = "#" + time() + " \n+Folders\nroot";
	}
	else {
		logVerbose("FIX: Not updating %s's directory because it contains %s", username.c_str(), directoryContents.c_str());
	}

	std::ofstream out("Drive/directory-" + username + ".txt");
	out << directoryContents;
	out.close();

	std::ifstream file("Drive/directory-" + username + ".txt");

	lock.unlock();
	return file;
}

void pushDirectory(std::string username) {

	lock.lock();

	std::string directoryContents;

	std::ifstream file("Drive/directory-" + username + ".txt");

	if(!file.good()){
		logVerbose("Error: Could not read file %s", ("Drive/directory-" + username + ".txt").c_str());

		lock.unlock();
		return;
	}

	std::string line;

	while (getline(file, line)) {
		directoryContents += line + "\n";
	}

	PUT(username, "directory", directoryContents);

	remove(("Drive/directory-" + username + ".txt").c_str());
	file.close();
	lock.unlock();
}

std::string showMessage(std::string message) {

	if (message.length() == 0) {
		return "";
	}

	if (message.substr(0, 3) == "+OK") {
		return "<div class='alert-success'>" + message + "</div>";
	}
	else if (message.substr(0, 4) == "-ERR") {
		return "<div class='alert-danger'>" + message + "</div>";
	}
	else {
		return "<div class='alert-message'>" + message + "</div>";
	}
}

std::string showButtons(std::string directory) {

	/*	To be able to 'create a folder' in a directory, user should send the current directory to the server
	 * 	This function returns HTML for the create folder button along with the current directory the user is in.
	 * 	It also sends a textbox that user should fill the name of the new folder into.
	 * 	Should be called before any other drive function
	 */

	std::stringstream HTML;

	HTML << "<div class='header clearfix'>";

	/* Current Directory */

	HTML << "<p class='directory-name'>";
	HTML << "<a tabindex=0 class=\"home-icon\" href=\"drive?action=showDirectory&directory=root\">Home</a>";
	HTML << "Current directory: <i class='directory'>";
	HTML << directory;
	HTML << "</i></p>";

	/* Create Folder */
	HTML << "<div class='folder-parent'>";
	HTML << "<a tabindex=0 class='folder create-folder'>Create Folder</a>";
	HTML << "<div tabindex='0' class='context folder-context'>";
	HTML << "<div class='create'>";
	HTML << "<form class='create-form' action='createFolder' method='GET'>";
	HTML << "<input readonly='readonly' required name='directory' value='";
	HTML << directory;
	HTML << "' type='text'>";
	HTML << "<input class='user-input' required name='folderName' placeholder='Folder Name' type='text'>";
	HTML << "<input value='Go' class='myButton' type='submit'>";
	HTML << "</form>";
	HTML << "</div>";
	HTML << "</div>";
	HTML << "</div>";

	/* Upload File */
	HTML << "<div class='folder-parent'>";
	HTML << "<a tabindex=0 class='upload-file'>Upload File</a>";
	HTML << "<div tabindex='0' class='context folder-context'>";
	HTML << "<div class='upload'>";
	HTML << "<form class='upload-form' action='uploadFile' method='POST' enctype='multipart/form-data' accept-charset='utf-8'>";
	HTML << "<input readonly='readonly' name='directory' value='";
	HTML << directory;
	HTML << "' type='text'>";
	HTML << "<input tabindex=0 class='user-input' required name='fileName' type='file'>";
	HTML << "<input value='Go' class='myButton' type='submit'>";
	HTML << "</form>";
	HTML << "</div>";
	HTML << "</div>";
	HTML << "</div>";

	HTML << "</div>";

	return HTML.str();
}

std::string showDirectory(std::string username, std::string directory) {

	std::stringstream html;
	std::ifstream file = readDirectory(username);

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
				folder << "<div class='folder-parent'>";
				folder << "<a class='folder' href='drive?action=showDirectory&directory=";
				folder << directory;
				folder << "/";
				folder << line;
				folder << "'>";
				folder << line;
				folder << "</a>";
				folder << "<div tabindex='0' class='context folder-context'>";
				folder << "<div class='rename'>";
				folder << "<div class='rename-image'>Rename</div>";
				folder << "<form class='rename-form' action='renameFileorFolder' method=GET>";
				folder << "<input readonly='readonly' type='text' required name='directory' value='";
				folder << directory;
				folder << "'>";
				folder << "<input readonly='readonly' type='text' required name='oldName' value='";
				folder << line;
				folder << "'>";
				folder << "<input class='user-input' type='text' required name='newName' placeholder='New Name'>";
				folder << "<input type='submit' value='Go' class='myButton'>";
				folder << "</form>";
				folder << "</div>";
				folder << "<div class='delete'>";
				folder << "<div class='delete-image'>Delete</div>";
				folder << "<form class='delete-form' action='deleteFolder' method=GET>";
				folder << "<input readonly='readonly' type='text' required name='directory' value='";
				folder << directory;
				folder << "'>";
				folder << "<input readonly='readonly' type='text' required name='deleteFolder' value='";
				folder << line;
				folder << "'>";
				folder << "<p>Are you sure you want to delete this folder?</p>";
				folder << "<input type='submit' value='Delete' class='fill myButton'>";
				folder << "</form>";
				folder << "</div>";
				folder << "<div class='move'>";
				folder << "<div class='move-image'>Move</div>	";
				folder << "<form class='move-form' action='moveFileorFolder' method=GET>";
				folder << "<input readonly='readonly' type='text' required name='oldDirectory' value='";
				folder << directory;
				folder << "'>";
				folder << "<input readonly='readonly' type='text' required name='folderName' value='";
				folder << line;
				folder << "'>";
				folder << "<input type='text' class='user-input' required name='newDirectory' placeholder='Directory'>";
				folder << "<input type='submit' value='Go' class='myButton'>";
				folder << "</form>";
				folder << "</div>";
				folder << "</div>";
				folder << "</div>";

				//	logVerbose("Folder HTML: %s", folder.str().c_str());
				html << folder.str();
			}
			else if (mode == "files") {
				file << "<div class='file-parent'>";
				file << "<a class='file' href='drive?action=downloadFile&file=";
				file << directory;
				file << "/";
				file << line;
				file << "'>";
				file << line;
				file << "</a>";
				file << "<div tabindex='0' class='context file-context'>";
				file << "<div class='rename'>";
				file << "<div class='rename-image'>Rename</div>";
				file << "<form class='rename-form' action='renameFileorFolder' method=GET>";
				file << "<input readonly='readonly' type='text' required name='directory' value='";
				file << directory;
				file << "'>";
				file << "<input readonly='readonly' type='text' required name='oldName' value='";
				file << line;
				file << "'>";
				file << "<input type='text' class='user-input' required name='newName' placeholder='New Name'>";
				file << "<input type='submit' value='Go' class='myButton'>";
				file << "</form>";
				file << "</div>";
				file << "<div class='delete'>";
				file << "<div class='delete-image'>Delete</div>";
				file << "<form class='delete-form' action='deleteFile' method=GET>";
				file << "<input readonly='readonly' type='text' required name='directory' value='";
				file << directory;
				file << "'>";
				file << "<input readonly='readonly' type='text' required name='deleteFile' value='";
				file << line;
				file << "'>";
				file << "<p>Are you sure you want to delete this file?</p>";
				file << "<input type='submit' value='Delete' class='fill myButton'>";
				file << "</form>";
				file << "</div>";
				file << "<div class='move'>";
				file << "<div class='move-image'>Move</div>	";
				file << "<form class='move-form' action='moveFileorFolder' method=GET>";
				file << "<input type='text' readonly='readonly' required name='oldDirectory' value='";
				file << directory;
				file << "'>";
				file << "<input type='text' readonly='readonly' required name='folderName' value='";
				file << line;
				file << "'>";
				file << "<input type='text' class='user-input' required name='newDirectory' placeholder='Directory'>";
				file << "<input type='submit' value='Go' class='myButton'>";
				file << "</form>";
				file << "</div>";
				file << "</div>";
				file << "</div>";

				//	logVerbose("File HTML: %s", file.str().c_str());
				html << file.str();
			}
		}
	}

	file.close();
	pushDirectory(username);
	return "<div class='content-region'>" + html.str() + "</div>";
}

/*	NOTE: Mail from mailbox abides to the following format
		TODO: add more fields if necessary (e.g. unique message-id)
DEBUG: [HEADER]
From <xxx@yyy>											NOTE: Sender
Thu, 8 Mar 2018 17:01:06						NOTE: Date
U																		NOTE: U/R corresponding to Unread vs. Read
01230912942109											NOTE: Timestamp
9102432															NOTE: Unique Message ID
DEBUG: [CONTENTS]
Subject: Hello, World								NOTE: Subject is both part of header and contents
From: Shiva <xxx@yyy>
To: Priya <aaa@bbb>
Date: Thu, 8 Mar 2018 17:01:06
[Text]
<CRLF>.<CRLF>
*/

std::map<header, contents> getMailbox(std::string username) {
	std::map<header, contents> userInfo;
	printf("username:%s\n", username.c_str());

	/*
	// READ FROM shiva.mbox

	std::string filepath("Mail/mailboxes/" + username + ".mbox");
	char mboxpath[1024];	// we can assume 1024 is enough if we put char limit on username
	strcpy(mboxpath, filepath.c_str());
	logSuccess("mboxpath: %s", mboxpath);
	FILE* mbox = fopen(mboxpath, "r");
	*/

	std::string mbox_str = GET(username, "mail");
	if (mbox_str.size() == 0) return userInfo;
	if (mbox_str.at(0) == '0') {
		logSuccess("EMPTY MAILBOX");
		return userInfo;
	}

	std::istringstream f(mbox_str);
	std::string line_str;

	int counter = 0;
	std::string subject, from, date;
	subject = "";
	bool unread;
	long long timestamp;
	std::string tempContents = "";

	while (std::getline(f, line_str)) {
		char line[line_str.length() + 1];
		strcpy(line, line_str.c_str());
		printf("line: %s\n", line);
		if (line_str.length() >= 9) {
			if (line_str.substr(0,8).compare("Subject:") == 0 && subject.compare("") == 0) subject = line;
		}
		if (line_str.substr(0,5).compare("From ") != 0) {
			tempContents += line_str;//.substr(0,line_str.length()-2);
			tempContents += "<br>";
			printf("tempContents is %s\n", tempContents.c_str());
		}

		counter++;
		//logSend("line: %s", line);
		// beginning of new email, assume email lines do not begin with "From " as assumed in CIS 505 HW2
		if (strncmp(line, "From ", sizeof("From ")-1) == 0) {
			if (counter > 1) {
				// populate map
				header head;
				if (subject.compare("Subject: ") == 0) subject = "Subject: (no subject)";
				head.assign(subject, from, date, unread, timestamp);
				contents cont;
				cont.text = tempContents;
				logSuccess("cont.text: %s\n", cont.text.c_str());
				userInfo[head] = cont;
				//userInfo.insert(std::make_pair<header, contents>(head, cont));
				logSuccess("from: %s, date: %s, unread: %d, timestamp: %lld", from.c_str(), date.c_str(), unread, timestamp);
				// reset temp variables
				subject = from = date = "";
				//from = line;
				int ind_hash = line_str.find("#");
				int ind_star = line_str.find("*");
				int ind_and = line_str.find("&");
				from = line_str.substr(0, ind_hash);
				date = line_str.substr(ind_hash+1, ind_star - ind_hash - 1);
				unread = (line_str.substr(ind_star+1, 1).compare("U") == 0);
				timestamp = atoll(line_str.substr(ind_and+1, line_str.length() - ind_and).c_str());

				logSuccess("from: %s, date: %s, unread: %d, timestamp: %lld", from.c_str(), date.c_str(), unread, timestamp);
				tempContents.clear();
				counter = 1;
			} else {
				// first email
				header head;
				//logSuccess("from: %s", from.c_str());
				if (subject.compare("Subject: ") == 0) subject = "Subject: (no subject)";
				head.assign(subject, from, date, unread, timestamp);
				int ind_hash = line_str.find("#");
				int ind_star = line_str.find("*");
				int ind_and = line_str.find("&");
				from = line_str.substr(0, ind_hash);
				date = line_str.substr(ind_hash+1, ind_star - ind_hash - 1);
				unread = (line_str.substr(ind_star+1, 1).compare("U") == 0);
				timestamp = atoll(line_str.substr(ind_and+1, line_str.length() - ind_and).c_str());
			}
		}
		//free(line);
	}

	// add final email if there are >0 emails
	contents cont;
	cont.text = tempContents;
	printf("cont.text: %s\n", cont.text.c_str());
	if (counter != 0) {
		header head;
		if (subject.compare("Subject: ") == 0) subject = "Subject: (no subject)";
		logSuccess("from: %s, date: %s, unread: %d, timestamp: %lld", from.c_str(), date.c_str(), unread, timestamp);
		head.assign(subject, from, date, unread, timestamp);
		contents cont;
		cont.text = tempContents;
		userInfo.insert(std::pair<header, contents>(head, cont));
	}
	return userInfo;
}

int getSMTPSocket() {
	printf("...sending mail\n");

	// connect to SMTP server
	findSMTP();
	return establishSMTPConnection();

	// SEND HELO <email>
	// SEND MAIL FROM: Name <email>

	// LOOP THRU RECIPIENTS
	// SEND RCPT TO:
	// SEND DATA (i.e. the contents)
	// QUIT
}

std::string plusToSpace(std::string s) {
	for (int i = 0; i < s.size(); i++) {
		if (s.at(i) == '+') s.at(i) = ' ';
	}
	return s;
}

void handleDeleteMail(int comm_fd, std::string username, std::map<std::string, std::string> postParam) {

		std::string timestampToDelete = postParam["timestamp"];
		logSuccess("timestamp to delete is: %s", timestampToDelete.c_str());
		long long ts = atoll(timestampToDelete.c_str());
		std::string updatedMbox = "";
		std::string oldMbox = GET(username, "mail");

		std::istringstream f(oldMbox);
		std::string line_str;
		bool toDelete = false;

		while (std::getline(f, line_str)) {
			// new email
			if (strncmp(line_str.c_str(), "From ", sizeof("From ")-1) == 0) {
				int ind_and = line_str.find("&");
				std::string timestamp = line_str.substr(ind_and+1, line_str.length() - ind_and);
				logSuccess("compare timestamp %s to %s", timestampToDelete.c_str(), timestamp.c_str());
				if (ts == atoll(timestamp.c_str())) {
					printf("to delete set to true\n");
					toDelete = true;	// set toDelete flag
				} else {
					printf("to delete set to false\n");
					toDelete = false;
				}
			}
			if (!toDelete) updatedMbox += line_str + "\n";
			printf("%s", updatedMbox.c_str());
		}
		if (updatedMbox.compare("") == 0) DELETE(username, "mail");
		else PUT(username, "mail", updatedMbox);

		std::string body("<meta http-equiv='refresh' content='0; url=mail' />");
		std::string headers(okHeader);

		headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";

		if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0) {
			fprintf(stderr, "%d Thread Couldn't Write Register Headers\n", comm_fd);
		}
		if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
			fprintf(stderr, "%d Thread Couldn't Write Register Body\n", comm_fd);
		}

		if(verbose){
			std::cout << "Sending--->" << headers << body << std::endl;
		}
}

void handleReply(int comm_fd, std::string username, std::map<std::string, std::string>postParam) {
	int smtp_fd = getSMTPSocket();

	//Panic Error handling
	if(smtp_fd < 0){
		return;
	}
	printf("forward recipients: %s, timestamp to query: %s\n", postParam["sender"].c_str(), postParam["email"].c_str());

	// -1 indicates send failure and abort
	// 0 indicates success
	// 1 indicates failed to send to some recipient
	int return_code;
	if (sendHELO(smtp_fd) != 0) return_code = -1;
	if (sendMAILFROM(smtp_fd, username) != 0) return_code = -1;

	// send to recipients [space-delimited]
	std::vector<std::string> recipients;
	std::string to_unformatted = postParam["sender"];
	printf("to_unformatted: %s length: %lu\n", to_unformatted.c_str(), to_unformatted.length());
	int start = to_unformatted.find("%3C") + 3;
	int end = to_unformatted.find("%3E");
	printf("start: %d | end: %d\n", start, end);
	std::string to = "";
	to += to_unformatted.substr(start, end-start);
	to += "\0";
	printf("forwarding to: %s", to.c_str());

	// commas are converted to %2C via post req. so I will undo the conversion here
	int left = 0;
	int right = 0;
	do {
		right = to.find("%2C");
		if (right == std::string::npos) right = to.size();
		recipients.push_back(to.substr(left, right-left));
		left = right+3;	//offset for %2C
    to[right] = 'A';
		to[right+1] = 'A';
		to[right+2] = 'A';
    std::cout << to;

	} while (left < to.size());

	printf("\nVector of Recipients (Size = %lu)\n", recipients.size());
	for (int i = 0; i < recipients.size(); i++) {
		logSuccess("Recipient: %s", recipients[i].c_str());
		if (sendRCPTTO(smtp_fd, recipients[i]) != 0) return_code = 1;
	}

	std::string data_copy = plusToSpace(postParam["email"]);
	std::string data_merged = urlDecode(data_copy);
	printf("reply is: %s\n", data_merged.c_str());

	printf("subject before I touch it: %s\n", postParam["subject"].c_str());
	std::string subject = plusToSpace(postParam["subject"]);
	printf("subject after plus2space: %s\n", subject.c_str());
	subject = urlDecode(subject);
	logSuccess("Here is the subject: %s", subject.c_str());
	std::string subj = subject.substr(0,9) + "RE: " + subject.substr(9, subject.size() - 9);

	std::string data = subj;
	data += "\n";
	std::string reply_copy = plusToSpace(postParam["reply"]);
	std::string reply = urlDecode(reply_copy) + "\n\n------------------\nPrevious Thread Content\n------------------\n";
	data += reply;
	data += data_merged;

	printf("data of email: %s\n", data.c_str());
	if (sendDATA(smtp_fd) == 0) {
		if (sendDATAInput(smtp_fd, data) != 0) return_code = -2;
	}
	sendQUIT(smtp_fd);

	std::string body("<meta http-equiv='refresh' content='0; url=mail' />");
	std::string headers(okHeader);

	headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";

	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0) {
		fprintf(stderr, "%d Thread Couldn't Write Register Headers\n", comm_fd);
	}
	if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Register Body\n", comm_fd);
	}

	if(verbose){
		std::cout << "Sending--->" << headers << body << std::endl;
	}

}

void handleForward(int comm_fd, std::string username, std::map<std::string, std::string>postParam) {
	int smtp_fd = getSMTPSocket();

	printf("forward recipients: %s, timestamp to query: %s\n", postParam["forward"].c_str(), postParam["email"].c_str());

	// -1 indicates send failure and abort
	// 0 indicates success
	// 1 indicates failed to send to some recipient
	int return_code;
	if (sendHELO(smtp_fd) != 0) return_code = -1;
	if (sendMAILFROM(smtp_fd, username) != 0) return_code = -1;

	// send to recipients [space-delimited]
	std::vector<std::string> recipients;
	std::string to = postParam["forward"];
	printf("forwarding to: %s\n", to.c_str());

	// commas are converted to %2C via post req. so I will undo the conversion here
	int left = 0;
	int right = 0;
	do {
		right = to.find("%2C");
		if (right == std::string::npos) right = to.size();
		recipients.push_back(to.substr(left, right-left));
		left = right+3;	//offset for %2C
    to[right] = 'A';
		to[right+1] = 'A';
		to[right+2] = 'A';
    std::cout << to;

	} while (left < to.size());

	printf("Vector of Recipients (Size = %lu)\n", recipients.size());
	for (int i = 0; i < recipients.size(); i++) {
		logSuccess("Recipient: %s", recipients[i].c_str());
		if (sendRCPTTO(smtp_fd, recipients[i]) != 0) return_code = 1;
	}

	std::string data_copy = plusToSpace(postParam["email"]);
	std::string data_merged = urlDecode(data_copy);

	std::string subject = plusToSpace(postParam["subject"]);
	std::string subj = subject.substr(0,11) + "FWD: " + subject.substr(11, subject.size() - 11);

	std::string data = urlDecode(subj);
	data += "\n";
	data += data_merged;

	printf("data of email: %s\n", data.c_str());
	if (sendDATA(smtp_fd) == 0) {
		if (sendDATAInput(smtp_fd, data) != 0) return_code = -2;
	}
	sendQUIT(smtp_fd);

	std::string body("<meta http-equiv='refresh' content='0; url=mail' />");
	std::string headers(okHeader);

	headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";

	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0) {
		fprintf(stderr, "%d Thread Couldn't Write Register Headers\n", comm_fd);
	}
	if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Register Body\n", comm_fd);
	}

	if(verbose){
		std::cout << "Sending--->" << headers << body << std::endl;
	}

}

void handleSend(int comm_fd, std::string username, std::map<std::string, std::string>postParam) {
	int smtp_fd = getSMTPSocket();

	// -1 indicates send failure and abort
	// 0 indicates success
	// 1 indicates failed to send to some recipient
	int return_code;
	printf("postParam[email]: %s\n", postParam["email"].c_str());
	if (sendHELO(smtp_fd) != 0) return_code = -1;
	if (sendMAILFROM(smtp_fd, username) != 0) return_code = -1;

	// send to recipients [space-delimited]
	std::vector<std::string> recipients;
	std::string to = postParam["to"];
	printf("TO: %s\n", to.c_str());

	// commas are converted to %2C via post req. so I will undo the conversion here
	int left = 0;
	int right = 0;
	do {
		right = to.find("%2C");
		if (right == std::string::npos) right = to.size();
		recipients.push_back(to.substr(left, right-left));
		left = right+3;	//offset for %2C
		to[right] = 'A';	// replace ','
		to[right+1] = 'A';
		to[right+2] = 'A';
		std::cout << to;

	} while (left < to.size());

	printf("Vector of Recipients (Size = %lu)\n", recipients.size());
	for (int i = 0; i < recipients.size(); i++) {
		logSuccess("Recipient: %s", recipients[i].c_str());
		if (sendRCPTTO(smtp_fd, recipients[i]) != 0) return_code = 1;
	}

	std::string data_copy = plusToSpace(postParam["email"]);
	std::string data_merged = urlDecode(data_copy);

	std::string subj = plusToSpace(postParam["subject"]);

	std::string data = "Subject: " + urlDecode(subj);
	data += "\n";
	data += "To: " + urlDecode(postParam["to"]) + "\n";
	data += data_merged;

	printf("data of email: %s\n", data.c_str());
	if (sendDATA(smtp_fd) == 0) {
		if (sendDATAInput(smtp_fd, data) != 0) return_code = -2;
	}
	sendQUIT(smtp_fd);

	logSuccess("handleSend force restart");
	std::string body("<meta http-equiv='refresh' content='0; url=mail' />");
	std::string headers(okHeader);

	headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";

	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0) {
		fprintf(stderr, "%d Thread Couldn't Write Register Headers\n", comm_fd);
	}
	if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0) {
		fprintf(stderr, "%d Thread Couldn't Write Register Body\n", comm_fd);
	}
	if(!verbose){
		std::cout << "Sending--->" << headers << body << std::endl;
	}
	logSuccess("last line of handlSend");
}

std::string composeMailHTML(std::string username) {
	std::string body = templatePrefix(username);
	body += "<link href='Views/css/bootstrap.min.css' rel='stylesheet'>";
	body +=
	"<div style='margin-top:10px'> \
	<form method='POST' action=\"send\" class='form-control' > \
	<br> \
	<p id=\"returnmessage\"></p> \
	<label>To: <span>*</span></label> \
	<input class='form-control' type=\"text\" name=\"to\" placeholder=\"Please separate recipients with spaces\"/> \
	<label>Subject: <span>*</span></label> \
	<input class='form-control' type=\"text\" name=\"subject\" placeholder=\"Subject\"/> \
	<label>Bcc:</label> \
	<input class='form-control' type=\"text\" name=\"bcc\" placeholder=\"Please separate Bccs with spaces\"/> \
	<label>Content:</label> \
	<textarea name=\"email\" placeholder=\"Make 'em smile :-)\"></textarea> \
	<input class='btn btn-primary form-control' style='margin-top:10px' type=\"submit\" id=\"send\" value=\"Send\"/> \
	</form> \
	</div>";

	body += templateSuffix();
	return body;
}

void sendComposePage(int comm_fd, std::string username) {
	std::string body = composeMailHTML(username);
	std::string headers(okHeader);
	headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";

	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Headers\n", comm_fd);
	}

	if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Body\n", comm_fd);
	}

	if(verbose){
		std::cout << "Sending--->" << headers << body << std::endl;
	}
}

void markUnread() {

}

void markRead() {

}

std::string escapeQuotes(std::string s) {
	std::string text_copy;
	for (int i=0; i<s.size(); i++) {
		if (s.at(i) == '\'' || s.at(i) == '\"') text_copy += "\\";
		text_copy += s.at(i);
	}
	return text_copy;
}

void sendMailPage(int comm_fd, std::string username) {
	logSuccess("sendMailPage Entered");
	// redirect to home page if not logged in
	// DEBUG: already checked beforehand -- this statement is unreachable!!!
	if (strcmp(username.c_str(), "none") == 0) {
		//TODO: send to main page
		return;
	}


	// DEBUG: dummy put this should be done upon account creation
	// send message with command

	// access username's mailbox at 'Mail/mailboxes/username.mbox'
	std::map<header, contents> userInfo = getMailbox(username);
	logSuccess("passed get mailbox");
	std::string body_str = templatePrefix(username);
	//body_str + "<b>Mail</b><br><br><i>Inbox</i><br><br>";
	body_str += "<link href='Views/css/bootstrap.min.css' rel='stylesheet'>";
	body_str += "<br>";
	body_str += "<div class=col-md-2 style='margin-top:10px'>";
	body_str += "<a href=\"/compose\" class='btn btn-primary'>Compose Mail</a><br><br>";	// TODO: implement send mail post
	body_str += "</div>";
	// iterate through map
	int index = 0;
	body_str += "<div class=col-md-10 style='margin-top:10px'>";
	for (auto const& i : userInfo) {
		header head = i.first;
		contents cont = i.second;
		bool unread = head.unread;
		body_str += "<ul>";
		//printf("index is %s\n", std::to_string(index).c_str());
		std::string toggler =
		"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> \
		<style> \
			#myDIV" + std::to_string(index) + " { \
				width: 100%%; \
				padding: 50px 0; \
				text-align: left; \
				background-color: #F2F2F2; \
				margin-top: 20px; \
			} \
		</style> \
		<button onclick=\"myFunction" + std::to_string(index) + "()\">";
		std::string from = head.from;
		from.at(head.from.find("<")) = ' ';
		from.at(head.from.find(">")) = ' ';
		if (head.subject.compare("") == 0) head.subject = "Subject: (no subject)";
		if (unread) toggler += "<b>" + head.subject + " " + from + "on<i> " + head.date + "</i></b></button>";
		else toggler += head.subject + " " + head.from + "on<i> " + head.date + "</i></button>";

		toggler += "<div id=\"myDIV" + std::to_string(index) + "\"><div>";
		toggler += cont.text;
		toggler +=
		"</div><br> \
		<form method='POST' action=\"forward\" name='forward' id=\"forward\" class='forwardMail' > \
		<input class='form-control' type=\"text\" name=\"forward\" placeholder=\"Enter email address(es) to forward to, comma-delimited\" /> \
			<input class='btn btn-default form-control' type=\"submit\" value='Forward'/> \
			<input readonly='readonly' required name='email' value='";
		toggler += escapeQuotes(cont.text) + "'/>";
		toggler += "<input readonly='readonly' required name='subject' value='";
		toggler += escapeQuotes(head.subject) + "'/>";
		toggler +=
		"</form>	\
		<form method='POST' action=\"reply\" id=\"reply\" class='replyMail' > \
		<textarea class='form-control' name='reply' id=\"reply\" placeholder=\"Reply with a kind message\"></textarea> \
			<input class='btn btn-default form-control' type=\"submit\" value=\"Reply\"/> \
			<input readonly='readonly' required name='email' value='";
		//printf("escape: %s\n", escapeQuotes(cont.text).c_str());
		toggler += escapeQuotes(cont.text) + "'/>";
		toggler += "<input readonly='readonly' required name='sender' value='";
		toggler += escapeQuotes(head.from) + "'/>";
		toggler += "<input readonly='readonly' required name='subject' value='";
		toggler += escapeQuotes(head.subject) + "'/>";
		toggler +=
		"</form> \
		<form method='POST' action='delete' class='deleteMail' > \
			<input class='btn btn-default form-control' type='submit' id='delete' value='Delete'/> \
			<input readonly='readonly' required name='timestamp' value='";
		toggler += std::to_string(head.timestamp) + "'/>";
		toggler += "</form>";
		toggler +=
		"</div> \
		<script> \
		function myFunction" + std::to_string(index) + "() { \
				var x = document.getElementById(\"myDIV" + std::to_string(index) + "\"); \
				if (x.style.display === \"none\") { \
						x.style.display = \"block\"; \
				} else { \
						x.style.display = \"none\"; \
				} \
		} \
		myFunction" + std::to_string(index) + "(); \
		</script></ul>";

		body_str += toggler;
		index++;
	}
	body_str += "</div>";
	body_str += templateSuffix();
	char body[body_str.length() + 1];
	strcpy(body, body_str.c_str());

	printf("index is: %d\n", index);
	std::string headers(okHeader);
	headers = headers + "Content-Length: " + std::to_string(body_str.size()) + "\r\n\r\n";
	//sprintf(headers, "Content-Length: %zu\r\n\r\n", strlen(body));
	if(write(comm_fd, headers.c_str(), headers.length()) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Content-Length Header\n", comm_fd);
	}
	if(write(comm_fd, body, strlen(body)) <= 0) {
		fprintf(stderr, "%d Thread Couldn't Write Body\n", comm_fd);
	}
	if(verbose){
		fprintf(stderr, "[%d] S: %s", comm_fd, headers.c_str());
		fprintf(stderr, "[%d] S: %s", comm_fd, body);
	}
	//free(body);
}

std::string downloadFile(std::string username, std::string filePath) {

	/*	Returns the contents of the file identified by the complete file path	*/

	std::string unique = GET(username, filePath);
	std::string contents = GET(username, unique);

	return contents;
}

std::string moveFileorFolder(std::string username, std::string oldDirectory, std::string folderName, std::string newDirectory) {

	/*	Convert ~A/B/C/X/... to ~A/X/...
	 *
	 * 	If it is a file, it will be of the form:
	 * 	~A/B/C/X.png to ~/X.png
	 */

	/*	1)	Check if the old path exists (sanity check)
	 * 	2)	Check if the new directory is actually an existing folder
	 * 	3)	Check if new directory is a subdirectory of the current directory
	 * 		For example: Move root/A/B/C to root/A/B/C/D/E should fail	*/

	/*	While we're referring it to as folderName, it might as well be a file
	 * 	Folder case is more generic and covers when it is a file	*/

	std::ifstream file = readDirectory(username);

	std::string line;
	std::string mode;
	std::stringstream message;
	bool oldPathValid = false;
	bool newPathValid = false;

	std::string oldPath = oldDirectory + "/" + folderName;
	std::string newPath = newDirectory + "/" + folderName;

	/*	Checking for condition 3	*/

	if (newPath.find(oldPath) != std::string::npos) {
		message << "-ERR: Move operation failed. Can't move " << folderName << " to " << newDirectory << ".";
		logVerbose("%s", message.str().c_str());
		return message.str();
	}

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
		message << "-ERR: Move operation failed. Either " << oldPath << " or " << newDirectory << " is not valid.";
		logVerbose("%s", message.str().c_str());
		return message.str();
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

	std::ofstream out("Drive/directory-" + username + ".txt");
	out << buffer.str();
	out.close();
	pushDirectory(username);

	message << "+OK: Moved " << folderName << " from " << oldDirectory << " to " << newDirectory << ".";
	logVerbose("%s", message.str().c_str());
	file.close();
	return message.str();
}

std::string renameFileorFolder(std::string username, std::string directory, std::string oldName, std::string newName) {

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
	std::ifstream file = readDirectory(username);
	bool foundFileorFolder = false;
	std::string line;
	std::string mode;

	std::stringstream buffer;
	std::stringstream message;

	if (newName.find('/') != std::string::npos) {
		message << "-ERR: Cannot rename " << oldName.c_str() << " to " << newName.c_str() << ", which is a directory";
		logVerbose("%s", message.str().c_str());
		return message.str();
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

	std::ofstream out("Drive/directory-" + username + ".txt");
	out << buffer.str();
	out.close();
	pushDirectory(username);

	if (foundFileorFolder == false) {
		message << "-ERR: Could not find " << oldPath.c_str() << ". Rename operation failed";
		logVerbose("%s", message.str().c_str());
		file.close();
		return message.str();
	}
	else {
		message << "+OK: Renamed " << oldName.c_str() << " to " << newName.c_str() << ".";
		logVerbose("%s", message.str().c_str());
		file.close();
		return message.str();
	}
	file.close();
}

std::string deleteFolder(std::string username, std::string directory, std::string folderName) {

	/*	Delete folder 'folderName' from 'directory' for user 'username'	*/

	/*	TODO:
	 * 	Recursively delete all the files and sub folders in that folder
	 * 	For deleting files:	DELETE(username, file)
	 * 	For deleting folders: Just update the directory file (GET(username, directory)
	 */

	/*	1)	Check if folder exists in the directory listing and delete it	*/

	std::ifstream file = readDirectory(username);
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
			else if (line.substr(0, path.length() + 1) == path + "/") {

				if (mode == "files") {
					logVerbose("Deleting file %s", line.c_str());

					std::string unique = GET(username, path);
					DELETE(username, unique);
					DELETE(username, path);
				}
				else if (mode == "folders") {
					logVerbose("Deleting folder %s because %s == %s", line.c_str(), line.substr(0, path.length()).c_str(), path.c_str());
				}
			}
			else if (line != "" && line != "\n"){

				/*	Empty lines are deleted	(Well, this block only keeps the non-empty lines	*/
				logSuccess("Saving %s", line.c_str());
				buffer << line << "\n";
			}
		}
	}

	std::ofstream out("Drive/directory-" + username + ".txt");
	out << buffer.str();
	out.close();
	pushDirectory(username);

	std::stringstream message;

	if (folderExists == false) {
		logVerbose("Could not find %s. Folder deletion failed.", (directory + "/" + folderName).c_str());

		message << "-ERR: Could not find " << directory << "/" << folderName << ". Folder deletion failed.";
		logVerbose("%s", message.str().c_str());
		file.close();
		return message.str();
	}
	else {
		message << "+OK: Deleted " << directory << "/" << folderName << ".";
		logVerbose("%s", message.str().c_str());
		file.close();
		return message.str();
	}
	file.close();
}

std::string deleteFile(std::string username, std::string directory, std::string fileName) {

	/*	1)	Check if file exists in the directory listing and delete it	*/

	std::ifstream file = readDirectory(username);
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

	std::ofstream out("Drive/directory-" + username + ".txt");
	out << buffer.str();
	out.close();
	pushDirectory(username);

	std::stringstream message;

	if (fileExists == false) {
		message << "-ERR: Could not find " << directory << "/" << fileName << ". File deletion failed.";
		logVerbose("%s", message.str().c_str());
		return message.str();
	}
	else {
		std::string unique = GET(username, (directory + "/" + fileName));
		DELETE(username, unique);
		DELETE(username, (directory + "/" + fileName));
	}

	message << "+OK: Deleted " << directory << "/" << fileName << ".";
	logVerbose("%s", message.str().c_str());
	file.close();
	return message.str();
}

std::string createFolder(std::string username, std::string directory, std::string folderName) {

	/*	Creates folder 'foldername' in 'directory' for user 'username'	*/
	/*	Instead of opening file, to be replaced with file = GET(username, "directory")	*/

	std::ifstream file = readDirectory(username);
	std::stringstream message;

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
			message << "-ERR: The folder " << line.c_str() << " already exists. Folder creation failed.";
			logVerbose("%s", message.str().c_str());
			return message.str();
		}
	}

	if (directoryExists == false) {
		message << "-ERR: The directory " << directory.c_str() << " does not exists. Folder creation failed.";
		logVerbose("%s", message.str().c_str());
		return message.str();
	}

	/*	The directory was found and the folder is not already there	*/

	std::ofstream append("Drive/directory-" + username + ".txt", std::ios_base::app | std::ios_base::out);
	append << "\n+Folders\n";
	append << directory + "/" + folderName;

	append.close();
	pushDirectory(username);

	logVerbose("Folder %s created in %s", folderName.c_str(), directory.c_str());

	message << "+OK: Folder " << folderName.c_str() << " created in " << directory.c_str() << ".";
	logVerbose("%s", message.str().c_str());
	file.close();
	return message.str();
}

std::string uploadFile(std::string username, std::string directory, std::string fileName, std::string fileContents) {

	/*	Uploads file 'fileName' in the 'directory', both in the directory file and the storage (key - value)
	 * 	Stores in the form of <FILENAME, Unique ID> and another record <Unique ID, File Contents>	*/

	/*	1)	Add the file to the directory	*/

	std::ifstream file = readDirectory(username);
	std::string line;
	std::stringstream message;
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
			message << "-ERR: The file " << line.c_str() << " already exists. File upload failed.";
			logVerbose("%s", message.str().c_str());
			return message.str();
		}
	}

	if (directoryExists == false) {
		message << "-ERR: The directory " << directory.c_str() << " does not exist. File upload failed.";
		logVerbose("%s", message.str().c_str());
		return message.str();
	}

	/*	The directory was found and the file is not already there	*/

	std::ofstream append("Drive/directory-" + username + ".txt", std::ios_base::app | std::ios_base::out);
	append << "\n*Files\n";
	append << directory + "/" + fileName;

	append.close();
	pushDirectory(username);

	/*	2)	Since Step 1) passed, add this as a key value pair	*/

	std::string unique = time();
	PUT(username, directory + "/" + fileName, unique);
	PUT(username, unique, fileContents);

	message << "+OK: File " << fileName.c_str() << " uploaded to " << directory.c_str() << ".";
	logVerbose("%s", message.str().c_str());
	file.close();
	return message.str();
}

/*	Drive functions end	*/

//Handle files differently????
void handleDrive(int comm_fd, std::string username, std::map<std::string, std::string> postParam){

	//Default is to show directory
	std::string action("showDirectory");
	if(postParam.find("action") != postParam.end()){
		action = postParam["action"];
	}

	if(action.compare("showDirectory") == 0){
		std::string body(templatePrefix(username));
		std::string headers(okHeader);

		//Default look for root directory, else look for the one specified in the params
		std::string directory("root");
		if(postParam.find("directory") != postParam.end()){
			directory = postParam["directory"];
		}

		//Add logout button
//		body = body + "<br><a id=\"id\" href=\"/logout\">logout</a><br>";
		body += showButtons(directory);

		std::string content = showDirectory(username, directory);
		body += content;
		body += templateSuffix();

		headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";
		//Send main headers, then content-length header, then the content

		if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
			fprintf(stderr, "%d Thread Couldn't Write Headers\n", comm_fd);
		}
		if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
			fprintf(stderr, "%d Thread Couldn't Write Body\n", comm_fd);
		}
		if(verbose){
			std::cout << "Sending --> " << headers << body << std::endl;
		}

	} else if(action.compare("downloadFile") == 0){
		std::string filePath = postParam["file"];
		std::string content = downloadFile(username, filePath);
		std::string downloadHeader("HTTP/1.1 200 OK\r\nServer: 505Team11\r\nConnection: close\r\n");

		//	Download instead of view
		downloadHeader += "Content-Disposition: attachment\r\n";

		//Add the content-type header
		if(filePath.find(".png") != std::string::npos){
			downloadHeader = downloadHeader + "Content-Type: image/png\r\n";
		} else if(filePath.find(".jpg") != std::string::npos){
			downloadHeader = downloadHeader + "Content-Type: image/jpeg\r\n";
		} else if(filePath.find(".txt") != std::string::npos){
			downloadHeader = downloadHeader + "Content-Type: text/plain\r\n";
		} else if(filePath.find(".pdf") != std::string::npos){
			downloadHeader = downloadHeader + "Content-Type: application/pdf\r\n";
		} else {
			//TODO verify the default content-type
			downloadHeader = downloadHeader + "Content-Type: text/plain\r\n";
		}
		//Add the content-length header
		downloadHeader = downloadHeader + "Content-Length: " + std::to_string(content.size()) + "\r\n\r\n";

		if(write(comm_fd, downloadHeader.c_str(), strlen(downloadHeader.c_str())) <= 0){
			fprintf(stderr, "%d Thread Couldn't Write Headers\n", comm_fd);
		}
		for(int i = 0; i < content.length(); i++){
			if(write(comm_fd, &content[i], sizeof(char)) <= 0){
				fprintf(stderr, "%d Thread Couldn't Write Body\n", comm_fd);
			}
		}

		if(verbose){
			std::cout << "Sending Header--> " << downloadHeader << std::endl;
		}

	} else {
		printf("######## SHOULDN'T BE HERE!!!!!\n");
		//TODO decide what to do here
	}
}

/*
 * Helper Methods
 */

std::string getCookieHeader(std::string username){
	std::string cookieHeader("Set-Cookie: ");
	cookieHeader = cookieHeader + username + "\r\n";
	return cookieHeader;
}

void redirectToLogin(int comm_fd){
	std::string headers("HTTP/1.1 302 Found\r\nLocation: /login\r\nServer: 505Team11\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n");

	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Headers\n", comm_fd);
	}
	if(verbose){
		std::cout << headers << std::endl;
	}
}

void send404(int comm_fd){
	std::string headers("HTTP/1.1 404 Not Found\r\nServer: 505Team11\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n");

	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Headers\n", comm_fd);
	}
	if(verbose){
		std::cout << headers << std::endl;
	}
}

/**
 * For each of the Main Pages, get the default headers, determine body, compute Content-length header and then send
 */
void sendLoginPage(int comm_fd, std::string errorMsg, std::string extraHeaders){
	std::string body;
	std::string headers(okHeader);

	body = "<style>.sidebar { display: none !important; } body {background-color: #2196F3;} .parent { padding-left: 0px !important; }</style>";
	body += templatePrefix("");
	body += showMessage(errorMsg);

	body += "<div class='logo logo-big'><i class='material-icons icon-size'>cloud</i>PennCloud</div><form class='login-form' action=\"login\" method = \"post\"><input type=\"text\" placeholder='Username' required name=\"username\"><input placeholder='Password' required type=\"password\" name=\"password\"><br><br><input type=\"submit\" value=\"Submit\"></form><div class='button-register'>New user? Register <a href=\"/register\">here</a></div></BODY></HTML>";
	body += templateSuffix();

	headers = headers + extraHeaders + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";

	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Register Headers\n", comm_fd);
	}
	if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Register Body\n", comm_fd);
	}

	if(verbose){
		std::cout << "Sending--->" << headers << body << std::endl;
	}
}

void sendRegisterPage(int comm_fd, std::string errorMsg){
	std::string body;
	std::string headers(okHeader);

	body = "<style>.sidebar { display: none !important; } body {background-color: #2196F3;} .parent { padding-left: 0px !important; }</style>";
	body += templatePrefix("");
	body += showMessage(errorMsg);

	body += "<div class='logo logo-big'><i class='material-icons icon-size'>cloud</i>PennCloud</div><form class='login-form' action=\"register\" method = \"post\"><input type=\"text\" placeholder='Username' required name=\"username\"><input placeholder='Password' required type=\"password\" name=\"password\"><br><br><input type=\"submit\" value=\"Submit\"></form>";
	body += templateSuffix();

	headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";

	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Register Headers\n", comm_fd);
	}
	if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Register Body\n", comm_fd);
	}

	if(verbose){
		std::cout << "Sending--->" << headers << body << std::endl;
	}
}


void sendMainPage(int comm_fd, std::string cookieHeader){

	std::string body(templatePrefix(""));
	std::string headers(okHeader);

	body += "<h1 align=\"center\"> Welcome to PennCloud!</h1>";
	body += templateSuffix();

	headers = headers + cookieHeader + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";
	//Send main headers, then content-length header, then the content

	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Headers\n", comm_fd);
	}
	if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Body\n", comm_fd);
	}

	if(verbose){
		std::cout << "Sending--->" << headers << body << std::endl;
	}
}

void sendErrorPage(std::string username, std::string message){
	return;
//	std::string body("<HTML><HEAD><TITLE>Error</TITLE></HEAD><BODY>");
//	std::string headers("HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nServer: 505Team11\r\nConnection: close\r\n");
//	body += "<h2 align=\"center\"> Oops! This is embarrassing!</h2><h3 align=\"center\">" + message +
//			"</h3><img style=\"margin:10px 0px 0px 450px\" src=\"https://media.giphy.com/media/Bi8W90LzIkXeM/giphy.gif\"><br>";
//	body += templateSuffix();
//
//	//https://i.gifer.com/UQ5.gif
//	//https://media.giphy.com/media/xT0GqtpF1NWd9VbstO/giphy.gif
//
//	headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";
//	//Send main headers, then content-length header, then the content
//
//	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
//		fprintf(stderr, "%d Thread Couldn't Write Headers\n", comm_fd);
//	}
//	if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
//		fprintf(stderr, "%d Thread Couldn't Write Body\n", comm_fd);
//	}
//	if(verbose){
//		std::cout << "Sending --> " << headers << body << std::endl;
//	}
}

//TODO need to add initial columns for directory, mbox
void handleRegisterPage(int comm_fd,  std::map<std::string, std::string> postParam){
	std::string username, password;
	username = postParam["username"];
	password = postParam["password"];

	std::string value = GET(username, "PWD");

	std::cout << "Password: " << password << " Value: " << value << std::endl;
	//If user name already exists, send error message
	if(value.compare("0") != 0){
		sendRegisterPage(comm_fd, "-ERR: User name already exists!");
		return;
	}

	//Add the username and password
	PUT(username, "PWD", password);

	//Send  Main Page with the Set-cookie header
	sendMainPage(comm_fd, getCookieHeader(username));
}

void handleLoginPage(int comm_fd,  std::map<std::string, std::string> postParam){
	std::string username, password;
	username = postParam["username"];
	password = postParam["password"];

	std::string value = GET(username, "PWD");
	//If user name already exists, send error message
	if(value.compare(password) != 0){
		sendLoginPage(comm_fd, "-ERR: Incorrect Username/password!", "");
		return;
	}

	//Send  Main Page with the Set-cookie header
	sendMainPage(comm_fd, getCookieHeader(username));
}

void handleLogout(int comm_fd, std::string username){
	//To the cookie header, set expires to an old date
	std::string cookieHeader = getCookieHeader(username + "; expires=Thu, 01 Jan 1920 00:00:00 GMT");
	sendLoginPage(comm_fd, "+OK You have now successfully logged out!", cookieHeader);
}

//TODO need to get a string from each of the drive methods

void handleDeleteFolder(int comm_fd, std::string username, std::map<std::string, std::string> postParam){
	std::string directory = postParam["directory"];
	std::string folder = postParam["deleteFolder"];
	std::string message;

	message = deleteFolder(username, directory, folder);

	//TODO this is duplicate. See if you can condense it all!
	std::string body(templatePrefix(username));
	std::string headers(okHeader);

	//Default look for root directory, else look for the one specified in the params
	body += showMessage(message);
//	body = body + "<br><a id=\"id\" href=\"/logout\">logout</a><br>";
	body += showButtons(directory); //TODO verify this!!

	std::string content = showDirectory(username, directory);
	body += content;
	body += templateSuffix();

	headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";
	//Send main headers, then content-length header, then the content

	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Headers\n", comm_fd);
	}
	if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Body\n", comm_fd);
	}
	if(verbose){
		std::cout << "Sending --> " << headers << body << std::endl;
	}

}

void handleDeleteFile(int comm_fd, std::string username, std::map<std::string, std::string> postParam){
	std::string directory = postParam["directory"];
	std::string file = postParam["deleteFile"];
	std::string message;

	message = deleteFile(username, directory, file);

	//TODO this is duplicate. See if you can condense it all!
	std::string body(templatePrefix(username));
	std::string headers(okHeader);

	//Default look for root directory, else look for the one specified in the params

	body += showMessage(message);
	body += showButtons(directory);
//	body = body + "<br><a id=\"id\" href=\"/logout\">logout</a><br>" + showButtons("root"); //TODO verify this!!

	std::string content = showDirectory(username, directory);
	body += content;
	body += templateSuffix();

	headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";
	//Send main headers, then content-length header, then the content

	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Headers\n", comm_fd);
	}
	if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Body\n", comm_fd);
	}
	if(verbose){
		std::cout << "Sending --> " << headers << body << std::endl;
	}
}

void handleMoveFileorFolder(int comm_fd, std::string username, std::map<std::string, std::string> postParam){
	std::string oldDirectory = postParam["oldDirectory"];
	std::string folderName = postParam["folderName"];
	std::string newDirectory = postParam["newDirectory"];
	std::string message;

	message = moveFileorFolder(username, oldDirectory, folderName, newDirectory);
	std::string body(templatePrefix(username));
	std::string headers(okHeader);

	body += showMessage(message);
	body += showButtons(oldDirectory); //TODO verify this!!
	//Default look for root directory, else look for the one specified in the params
//	body = body + "<br><a id=\"id\" href=\"/logout\">logout</a><br>" + showButtons("root"); //TODO verify this!!

	std::string content = showDirectory(username, oldDirectory);
	body += content;
	body += templateSuffix();

	headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";
	//Send main headers, then content-length header, then the content

	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Headers\n", comm_fd);
	}
	if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Body\n", comm_fd);
	}
	if(verbose){
		std::cout << "Sending --> " << headers << body << std::endl;
	}

}

void handleRenameFileorFolder(int comm_fd, std::string username, std::map<std::string, std::string> postParam){
	std::string directory = postParam["directory"];
	std::string oldName = postParam["oldName"];
	std::string newName = postParam["newName"];
	std::string message;

	message = renameFileorFolder(username, directory, oldName, newName);
	std::string body(templatePrefix(username));
	std::string headers(okHeader);

	//Default look for root directory, else look for the one specified in the params
	body += showMessage(message);
//	body = body + "<br><a id=\"id\" href=\"/logout\">logout</a><br>";
	body += showButtons(directory); //TODO verify this!!

	std::string content = showDirectory(username, directory);
	body += content;
	body += templateSuffix();

	headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";
	//Send main headers, then content-length header, then the content

	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Headers\n", comm_fd);
	}
	if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Body\n", comm_fd);
	}
	if(verbose){
		std::cout << "Sending --> " << headers << body << std::endl;
	}
}


void handleCreateFolder(int comm_fd, std::string username, std::map<std::string, std::string> postParam){
	std::string folderName = postParam["folderName"];
	std::string directory = postParam["directory"];
	std::string message;

	message = createFolder(username, directory, folderName);

	std::string body(templatePrefix(username));
	std::string headers(okHeader);

	//Default look for root directory, else look for the one specified in the params
	body += showMessage(message);
//	body = body + "<br><a id=\"id\" href=\"/logout\">logout</a><br>";
	body += showButtons(directory); //TODO verify this!!

	std::string content = showDirectory(username, directory);
	body += content;
	body += templateSuffix();

	headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";
	//Send main headers, then content-length header, then the content

	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Headers\n", comm_fd);
	}
	if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Body\n", comm_fd);
	}
	if(verbose){
		std::cout << "Sending --> " << headers << body << std::endl;
	}
}

void handleUploadFile(int comm_fd, std::string username, std::map<std::string, std::string> postParam){

	std::string directory = postParam["directory"];
	std::string fileName = postParam["fileName"];
	std::string contents = postParam["contents"];
	contents = contents.substr(0, contents.length()-2);
	std::string message;

	message = uploadFile(username, directory, fileName, contents);

	std::string body(templatePrefix(username));
	std::string headers(okHeader);

	//Default look for root directory, else look for the one specified in the params


	body += showMessage(message);
	body = body + showButtons(directory); //TODO verify this!!

//	body = body + "<br><a id=\"id\" href=\"/logout\">logout</a><br>" + showButtons("root"); //TODO verify this!!



	std::string content = showDirectory(username, directory);
	body += content;
	body += templateSuffix();


	headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";
	//Send main headers, then content-length header, then the content

	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Headers\n", comm_fd);
	}
	if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Body\n", comm_fd);
	}


	if(verbose){
		std::cout << "Sending --> " << headers << body << std::endl;
	}

	logSuccess("Completed sending\n");
}


/*
 * This method provides the css and svg files required for styling
 */
void handleViews(int comm_fd, std::string path){
	std::ifstream ifs(path);

	// If the file is not found then return a 404 error
	if(!ifs.good()){
		send404(comm_fd);
		return;
	}
	std::string body((std::istreambuf_iterator<char>(ifs)),
	                       std::istreambuf_iterator<char>());
	std::string headers("HTTP/1.1 200 OK\r\nServer: 505Team11\r\nConnection: close\r\n");

	if(path.find("svg") != std::string::npos){
		headers = headers + "Content-Type: image/svg+xml\r\n";
	} else {
		headers = headers + "Content-Type: text/css\r\n";
	}

	headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";

	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Register Headers\n", comm_fd);
	}
	if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Register Body\n", comm_fd);
	}

	if(verbose){
		std::cout << "Sending--->" << headers << body << std::endl;
	}
	ifs.close();
}

// Tic-Tac-Toe logic functions here

std::string gameHome(std::string username) {

	std::stringstream HTML;

	HTML << "<form class='game-home game-create' action='createGame' method='GET'>";
	HTML << "<input type='Submit' value='Create Game'>";
	HTML << "</form>";

	HTML << "<form class='game-home game-join' action='joinGame' method='GET'>";
	HTML << "<input type='text' name='gameID' placeholder='Unique Game ID' required>";
	HTML << "<input type='Submit' value='Join Game'>";
	HTML << "</form>";

	return HTML.str();
}

std::string createGame(std::string player_1) {

	/*	Create a game with a unique ID	*/
	/*	Stores in the form of <OWNER, 123, garvit@PennCloud if garvit has created the game>	*/

	std::string gameID = time();
	std::stringstream HTML;

	PUT("OWNER", gameID, player_1);

	HTML << "<p class='game-message'>Please share this unique ID with player two. Note: Please don't click start button before player two joins.</p>";
	HTML << "<div class='uid'>" + gameID + "</div>";
	HTML << "<a class='game-start' href='getGame?gameID=" + gameID + "'>Start Game</a>";

	return HTML.str();
}

std::string joinGame(std::string player_2, std::string gameID) {

	/*	Joins the game identified by the gameID	*/
	/*	Stores in the form <GAME, 123, player_1|||player_2|||--------->	*/
	/* 	Where --------- is the default bitmap of the TTT game board	*/

	std::stringstream value;
	std::string player_1;
	std::stringstream HTML;

	player_1 = GET("OWNER", gameID);

	value << player_1;
	value << "|||";
	value << player_2;
	value << "|||";
	value << "---------";

	PUT("GAME", gameID, value.str());

	std::string message = "+OK Joined board " + gameID;
	logVerbose("%s", message.c_str());

	HTML << "<p class='game-message'>You have been invited by " + player_1 + " to join the game.</p>";
	HTML << "<div class='uid'>" + gameID + "</div>";
	HTML << "<a class='game-start' href='getGame?gameID=" + gameID + "'>Start Game</a>";

	return HTML.str();
}

std::string playMove(std::string player, std::string gameID, int row, int column) {

	std::string value = GET("GAME", gameID);

	std::string player_1;
	std::string player_2;
	std::string bitMap;
	std::string message;

	char operation = '-';

	/*	garvit@seas.upenn.edu|||garvit@seas.upenn.edu|||--------	*/

	player_1 = value.substr(0, value.find("|||"));
	value = value.substr(value.find("|||") + 3, std::string::npos);

	player_2 = value.substr(0, value.find("|||"));
	value = value.substr(value.find("|||") + 3, std::string::npos);

	bitMap = value;

	if (player == player_1) {
		operation ='O';
	}
	else if (player == player_2) {
		operation ='X';
	}
	else {
		message = "-ERR: " + player + " is not " + player_1 + " or " + player_2;
		logVerbose("%s", message.c_str());
		return message;
	}

	bitMap[row*3 + column] = operation;

	PUT("GAME", gameID, (player_1 + "|||" + player_2 + "|||" + bitMap));

	std::stringstream buffer;
	buffer << row;
	buffer << ", ";
	buffer << column;

	message = "+OK Modified bitmap to [" + bitMap + "]. ";
	logVerbose("%s", message.c_str());
	return message;
}

std::string isComplete(std::string gameID) {

	std::string value = GET("GAME", gameID);

	std::string player_1;
	std::string player_2;
	std::string bitMap;
	std::string message;

	std::string operation("-");

	/*	garvit@seas.upenn.edu|||garvit@seas.upenn.edu|||--------	*/

	player_1 = value.substr(0, value.find("|||"));
	value = value.substr(value.find("|||") + 3, std::string::npos);

	player_2 = value.substr(0, value.find("|||"));
	value = value.substr(value.find("|||") + 3, std::string::npos);

	bitMap = value;

	/*	Row 1	*/
	if (value.substr(0,3) == "OOO") {
		message = "+OK. " + player_1 + " won the game.";
		logVerbose("%s", message.c_str());
		return message;
	}
	if (value.substr(0,3) == "XXX") {
		message = "+OK. " + player_2 + " won the game.";
		logVerbose("%s", message.c_str());
		return message;
	}

	/*	Row 2	*/
	if (value.substr(3,6) == "OOO") {
		message = "+OK. " + player_1 + " won the game.";
		logVerbose("%s", message.c_str());
		return message;
	}
	if (value.substr(3,6) == "XXX") {
		message = "+OK. " + player_2 + " won the game.";
		logVerbose("%s", message.c_str());
		return message;
	}

	/*	Row 3	*/
	if (value.substr(6,9) == "OOO") {
		message = "+OK. " + player_1 + " won the game.";
		logVerbose("%s", message.c_str());
		return message;
	}
	if (value.substr(6,9) == "XXX") {
		message = "+OK. " + player_2 + " won the game.";
		logVerbose("%s", message.c_str());
		return message;
	}

	/*	Column 1	*/

	std::string pattern;
	pattern.push_back(value[0]);
	pattern.push_back(value[3]);
	pattern.push_back(value[6]);

	if (pattern == "OOO") {
		message = "+OK. " + player_1 + " won the game.";
		logVerbose("%s", message.c_str());
		return message;
	}
	if (pattern == "XXX") {
		message = "+OK. " + player_2 + " won the game.";
		logVerbose("%s", message.c_str());
		return message;
	}

	/*	Column 2	*/
	pattern.clear();
	pattern.push_back(value[1]);
	pattern.push_back(value[4]);
	pattern.push_back(value[7]);

	if (pattern == "OOO") {
		message = "+OK. " + player_1 + " won the game.";
		logVerbose("%s", message.c_str());
		return message;
	}
	if (pattern == "XXX") {
		message = "+OK. " + player_2 + " won the game.";
		logVerbose("%s", message.c_str());
		return message;
	}

	/*	Column 3	*/
	pattern.clear();
	pattern.push_back(value[2]);
	pattern.push_back(value[5]);
	pattern.push_back(value[8]);

	if (pattern == "OOO") {
		message = "+OK. " + player_1 + " won the game.";
		logVerbose("%s", message.c_str());
		return message;
	}
	if (pattern == "XXX") {
		message = "+OK. " + player_2 + " won the game.";
		logVerbose("%s", message.c_str());
		return message;
	}

	/*	Diagonal One	*/
	pattern.clear();
	pattern.push_back(value[0]);
	pattern.push_back(value[4]);
	pattern.push_back(value[8]);

	if (pattern == "OOO") {
		message = "+OK. " + player_1 + " won the game.";
		logVerbose("%s", message.c_str());
		return message;
	}
	if (pattern == "XXX") {
		message = "+OK. " + player_2 + " won the game.";
		logVerbose("%s", message.c_str());
		return message;
	}

	/*	Diagonal Two	*/
	pattern.clear();
	pattern.push_back(value[2]);
	pattern.push_back(value[4]);
	pattern.push_back(value[6]);

	if (pattern == "OOO") {
		message = "+OK. " + player_1 + " won the game.";
		logVerbose("%s", message.c_str());
		return message;
	}
	if (pattern == "XXX") {
		message = "+OK. " + player_2 + " won the game.";
		logVerbose("%s", message.c_str());
		return message;
	}

	/*	Check if it is a draw.	*/
	if (value.find("-") == std::string::npos) {
		message = "+OK. It's a draw!";
		logVerbose("%s", message.c_str());
		return message;
	}

	/*	Game is not complete	*/
	message = "Game is in progress. ";
	logVerbose("Game in progress.");
	return message;
}

std::string chance(std::string gameID) {

	std::string value = GET("GAME", gameID);

	std::string player_1;
	std::string player_2;
	std::string bitMap;
	std::string message;

	std::string operation("-");

	/*	garvit@seas.upenn.edu|||garvit@seas.upenn.edu|||--------	*/

	player_1 = value.substr(0, value.find("|||"));
	value = value.substr(value.find("|||") + 3, std::string::npos);

	player_2 = value.substr(0, value.find("|||"));
	value = value.substr(value.find("|||") + 3, std::string::npos);

	bitMap = value;

	if (isComplete(gameID) == "Game is in progress. ") {

		int X_Value = 0;
		int O_Value = 0;

		for (int i = 0; i < 9; i++) {
			if (bitMap[i] == 'X') {
				X_Value++;
			}
			else if (bitMap[i] == 'O') {
				O_Value++;
			}
		}

		if (O_Value > X_Value) {
			return player_2 + "|||" + bitMap;
		}
		else {
			return player_1 + "|||" + bitMap;
		}
	}
	else {
		return "none|||---------";
	}
}

std::string getGame(std::string username, std::string uid) {

	bool disabled = false;

	std::string value = chance(uid);

	std::string player = value.substr(0, value.find("|||"));
	std::string bitMap = value.substr(value.find("|||") + 3, std::string::npos);

	if (player != username) {
		disabled = true;
		logVerbose("# Chance returned %s while username is %s", chance(uid).c_str(), username.c_str());
	}

	std::stringstream tablebuilder;
	tablebuilder << "<table class='table table-striped game-box'>";
	for (int j=0;j<3;j++) {
		tablebuilder << "<tr>";

		for(int i=0;i<3;i++) {
			tablebuilder << "<td>";
			tablebuilder << "<form class='ttt-button form-control' action='playMove' method='POST'>";
			tablebuilder << "<input name='username' value='";
			tablebuilder << username;
			tablebuilder << "' type='text'>";
			tablebuilder << "<input name='gameID' value='";
			tablebuilder << uid;
			tablebuilder << "' type='text'>";
			tablebuilder << "<input name='row' value='";
			tablebuilder << j;
			tablebuilder << "' type='text'>";
			tablebuilder << "<input name='col' value='";
			tablebuilder << i;
			tablebuilder << "' type='text'>";
			tablebuilder << "<input value='' class='form-control ";

			if (bitMap[j*3 + i] == 'O') {
				tablebuilder << "zero";
			}
			else if (bitMap[j*3 + i] == 'X') {
				tablebuilder << "cross";
			}
			else {
				tablebuilder << "null";
			}

			tablebuilder << "' ";

			if (disabled == true) {
				tablebuilder << "disabled='disabled'";
			}
			else if (bitMap[j*3 + i] != '-') {
				tablebuilder << "disabled='disabled'";
			}

			tablebuilder << "type='submit'>";
			tablebuilder << "</form>";
			tablebuilder << "</td>";
		}
		tablebuilder << "</tr>";
	}

	std::string table_string = tablebuilder.str();

	return table_string;
}

// Tic-Tac-Toe HTTP functions here
void handleGetGame(int comm_fd, std::string username, std::map<std::string, std::string> postParam) {

	std::string table_string;
	std::string uid;

	if(postParam.find("gameID") != postParam.end()){
		uid = postParam["gameID"];
	}

	logVerbose("Game ID is %s", uid.c_str());

	std::string status;
	status = isComplete(uid);

	table_string = getGame(username, uid);

	std::string refreshButton;
	refreshButton = "<a href='getGame?gameID=" + uid + "' class='refresh-button'>Refresh</a>";

	std::stringstream body_stream;
	body_stream << templatePrefix(username);
	body_stream << table_string;
	body_stream << showMessage(status);
	body_stream << refreshButton;
	body_stream << templateSuffix();

	std::string body = body_stream.str();
	std::string headers(okHeader);

	headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";

	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Register Headers\n", comm_fd);
	}
	if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Register Body\n", comm_fd);
	}

	if(verbose){
		std::cout << "Sending--->" << headers << body << std::endl;
	}
}

void handleGameHome(int comm_fd, std::string username) {
	std::string HTML = gameHome(username);

	std::stringstream body_stream;
	body_stream << templatePrefix(username);
	body_stream << HTML;
	body_stream << templateSuffix();

	std::string body = body_stream.str();
	std::string headers(okHeader);

	headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";

	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Register Headers\n", comm_fd);
	}
	if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Register Body\n", comm_fd);
	}

	if(verbose){
		std::cout << "Sending--->" << headers << body << std::endl;
	}
}

void handleCreateGame(int comm_fd, std::string username){
	std::string HTML = createGame(username);

	std::stringstream body_stream;
	body_stream << templatePrefix(username);
	body_stream << HTML;
	body_stream << templateSuffix();

	std::string body = body_stream.str();
	std::string headers(okHeader);

	headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";

	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Register Headers\n", comm_fd);
	}
	if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Register Body\n", comm_fd);
	}

	if(verbose){
		std::cout << "Sending--->" << headers << body << std::endl;
	}
}

void handleJoinGame(int comm_fd, std::string username, std::map<std::string, std::string> postParam){

	std::string uid = "lol";
	if(postParam.find("gameID") != postParam.end()){
		uid = postParam["gameID"];
	}
	std::string HTML = joinGame(username, uid);

	std::stringstream body_stream;
	body_stream << templatePrefix(username);
	body_stream << HTML;
	body_stream << templateSuffix();

	std::string body = body_stream.str();
	std::string headers(okHeader);

	headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";

	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Register Headers\n", comm_fd);
	}
	if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Register Body\n", comm_fd);
	}

	if(verbose){
		std::cout << "Sending--->" << headers << body << std::endl;
	}
}

void handlePlayMove(int comm_fd, std::string username, std::map<std::string, std::string> postParam) {
	std::string uid;
	std::string row;
	std::string col;

	if(postParam.find("gameID") != postParam.end()){
		uid = postParam["gameID"];
	}
	if(postParam.find("row") != postParam.end()){
		row = postParam["row"];
	}
	if(postParam.find("col") != postParam.end()){
		col = postParam["col"];
	}

	std::string message_1 = playMove(username, uid, stoi(row), stoi(col));
	std::string message_2 = isComplete(uid);
	std::string HTML = getGame(username, uid);
	std::string refreshButton;

	refreshButton = "<a href='getGame?gameID=" + uid + "' class='refresh-button'>Refresh</a>";

	std::stringstream body_stream;
	body_stream << templatePrefix(username);
	body_stream << showMessage(message_1 + message_2);
	body_stream << HTML;
	body_stream << refreshButton;
	body_stream << templateSuffix();

	std::string body = body_stream.str();
	std::string headers(okHeader);

	headers = headers + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";

	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Register Headers\n", comm_fd);
	}
	if(write(comm_fd, body.c_str(), strlen(body.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Register Body\n", comm_fd);
	}

	if(verbose){
		std::cout << "Sending--->" << headers << body << std::endl;
	}
}


void *worker(void *arg){
	int comm_fd;
	comm_fd = *(int*)arg;
	////char buf[1000000] = {'\0'};
	////int bytesRead = recv(comm_fd, buf, 1000000, 0); // TODO: check add as many zeroes as possible!!
	std::map<std::string, std::string> postParam; // TO-HANDLE if implementing persistent connection, then handle this

	char *inBuf = new char[1];
	int byteRead = 0, retval = 0;
	std::string input = "" ;

	do{
		retval =  read(comm_fd, inBuf, 1);
		if(retval <= 0){
			fprintf(stderr, "%d Thread Couldn't Read\n", comm_fd);
			lock.unlock();
			close(comm_fd);
			pthread_detach(pthread_self());
			pthread_exit(NULL);
		}

		inBuf[retval] = '\0';
		input = input + inBuf[0];
		byteRead++;
		//std::cout << "Input is now: " << input << std::endl;
	}while(input.find("\r\n\r\n") == std::string::npos);

	logSuccess("%d Out of while loop\n", comm_fd);


	/*

	buf[bytesRead] = '\0';
	if(bytesRead <= 0){
		fprintf(stderr, "%d Thread Couldn't Read\n", comm_fd);

//		Just in case the thread is exiting while still in Critical Section
		lock.unlock();
		pthread_exit(NULL);
	}



	std::string request = buf; //(bufPtr, strlen(buf));
	logSuccess("%s", request.c_str());
	std::cout << "$$END of request$$" << std::endl;
	*/

	logSuccess("%s", input.c_str());
	std::cout << "$$END of input$$" << std::endl;
	std::string headDelim = "\r\n\r\n";
	std::string lineDelim = "\r\n";
	std::string spaceDelim = " ";
	std::string quesDelim = "?";
	std::string cookieDelim = "Cookie: ";
	std::string contentLenDelim = "Content-Length: ";
	std::string username("none");
	int contentLength = 0;

////	int startOfBody = request.find(headDelim);
////	std::string headers = request.substr(0, startOfBody);

	std::string headers = input;

	int startOfLine = headers.find(lineDelim);
	std::string firstLine = headers.substr(0, startOfLine);

	int startOfSpace = firstLine.find(spaceDelim);
	std::string method = firstLine.substr(0, startOfSpace);
	std::string path = firstLine.substr(startOfSpace + 1);
	path = path.substr(0, path.find(spaceDelim));

	//Get the username from the cookie
	int startOfHeader = headers.find(cookieDelim);
	if(startOfHeader != -1){
		std::string cookieHeader = headers.substr(startOfHeader + 8); //Want to match after the word Cookie:
		//std::cout << "Cookie Header: " << cookieHeader << std::endl;
		startOfLine = cookieHeader.find(lineDelim);
		username = urlDecode(cookieHeader.substr(0, startOfLine));
		std::cout << "Got username = " << username << std::endl;
	}

	//Get the content-length. This is applicable only for POST messages
	startOfHeader = headers.find(contentLenDelim);
	if(startOfHeader != -1){
		std::string lenString = headers.substr(startOfHeader + contentLenDelim.size());
		startOfLine = lenString.find(lineDelim);
		contentLength = atoi(lenString.substr(0, startOfLine).c_str());
		std::cout << "Got Content-Length =  " << contentLength << std::endl;
	}

	////std::string body = request.substr(startOfBody + headDelim.size(), request.size());// (request.substr(startOfBody + headDelim.size(), request.size()).c_str(), contentLength);

	byteRead = 0;



	char *buffer = new char[contentLength]();

	byteRead = 0;
	std::string newBody = "";

	while (byteRead < contentLength) {

		retval = read(comm_fd, buffer, contentLength);
		byteRead += retval;

		logSuccess("****** Retrieved %d characters. Content length: %d", (int)retval, (int)contentLength);

		if(retval < 0){
			fprintf(stderr, "%d Thread Couldn't Read\n", comm_fd);
			lock.unlock();
			close(comm_fd);
			pthread_detach(pthread_self());
			pthread_exit(NULL);
		}

		buffer[retval] = '\0';
		std::string temp(buffer, retval);
		newBody += temp;

		logVerbose("New Body is %d", (int)newBody.length());
	}



	logSuccess("Out of body loop\n");
	logVerbose("%s", headers.c_str());
	std::cout << "*End of headers" << std::endl;
	std::cout << "***END of parsed body***" << std::endl;
	std::cout << "Method: " << method << std::endl;
	std::cout << "Path: " << path << std::endl;

	//Split the parameters in the GET request
	int startOfParam = path.find(quesDelim);
	if(startOfParam != -1){
		std::string params = path.substr(startOfParam + 1);
		path = path.substr(0, startOfParam);
		int pairStart = 0;
		std::string key, value;
		do{
			pairStart = params.find('&');
			std::string pair = params.substr(0, pairStart);
			params = params.substr(pairStart+1);

			int valStart = pair.find("=");
			key = pair.substr(0, valStart);
			value = pair.substr(valStart + 1);

			// Decode the URL
			// i)Replace %2F with /
			value = urlDecode(value);

			std::cout << "key: " << key << " value: " << value << std::endl;
			postParam[key] = value;
		} while (pairStart != -1);
	}

	std::cout << "New Path: " << path << std::endl;

	if(method.compare("GET") == 0){
		printf("It's a GET request\n");
		if(path.compare("/login") == 0 || path.compare("/login/") == 0){
			sendLoginPage(comm_fd, "", "");
		} else if(path.compare("/register") == 0 || path.compare("/register/") == 0){
			sendRegisterPage(comm_fd, "");
		} else if(path.compare("/logout") == 0 || path.compare("/admin/") == 0) {
			handleLogout(comm_fd, username);
		} else if(path.compare("/admin") == 0 || path.compare("/admin/") == 0) {
			handleAdmin(comm_fd, username, postParam);
		} else if(path.compare("/drive") == 0 || path.compare("/drive/") == 0){
			if(username.compare("none") == 0){
				redirectToLogin(comm_fd);
			} else {
				handleDrive(comm_fd, username, postParam);
			}
		} else if(path.compare("/mail") == 0 || path.compare("/mail/") == 0){
			if(username.compare("none") == 0){
				redirectToLogin(comm_fd);
			} else {
				sendMailPage(comm_fd, username);
			}
		} else if (path.compare("/compose") == 0 || path.compare("/compose/") == 0) {
			if(username.compare("none") == 0){
				redirectToLogin(comm_fd);
			} else {
				sendComposePage(comm_fd, username);
			}
		} else if(path.compare("/deleteFile") == 0 || path.compare("/deleteFile/") == 0){
			if(username.compare("none") == 0){
				redirectToLogin(comm_fd);
			} else {
				handleDeleteFile(comm_fd, username, postParam);
			}
		} else if(path.compare("/deleteFolder") == 0 || path.compare("/deleteFolder/") == 0){
			if(username.compare("none") == 0){
				redirectToLogin(comm_fd);
			} else {
				handleDeleteFolder(comm_fd, username, postParam);
			}
		} else if(path.compare("/moveFileorFolder") == 0 || path.compare("/moveFileorFolder/") == 0){
			if(username.compare("none") == 0){
				redirectToLogin(comm_fd);
			} else {
				handleMoveFileorFolder(comm_fd, username, postParam);
			}
		} else if(path.compare("/renameFileorFolder") == 0 || path.compare("/renameFileorFolder/") == 0){
			if(username.compare("none") == 0){
				redirectToLogin(comm_fd);
			} else {
				handleRenameFileorFolder(comm_fd, username, postParam);
			}
		} else if(path.compare("/createFolder") == 0 || path.compare("/createFolder/") == 0){
			if(username.compare("none") == 0){
				redirectToLogin(comm_fd);
			} else {
				handleCreateFolder(comm_fd, username, postParam);
			}
		} else if(path.compare("/getGame") == 0 || path.compare("/getGame/") == 0){
			handleGetGame(comm_fd, username, postParam);
		} else if(path.compare("/game") == 0 || path.compare("/game/") == 0){
			handleGameHome(comm_fd, username);
		} else if(path.compare("/createGame") == 0 || path.compare("/createGame/") == 0){
			handleCreateGame(comm_fd, username);
		} else if(path.compare("/joinGame") == 0 || path.compare("/joinGame/") == 0){
			handleJoinGame(comm_fd, username, postParam);
		} else if(path.find("/error") != std::string::npos){ //TODO remove later, just for TESTING!
		 	sendErrorPage(username, "It's not you, it's us!");
		} else if(path.find("/Views") != std::string::npos){
			 	handleViews(comm_fd, "." + path); //TODO commenting for now to reduce the number of requests
		} else if(path.compare("/") == 0){
			if(username.compare("none") == 0){
				redirectToLogin(comm_fd);
			} else {
				sendMainPage(comm_fd, "");
			}
		} else {
			//TODO decide what would be the default; Currently sending 404 message
			printf("######## Some unknown path for GET method!! \n");
			send404(comm_fd);
		}
	} else if (method.compare("POST") == 0){
		printf("It's a POST request\n");

		//Split the parameters in the body
		int pairStart = 0;
		std::string key, value;
		if(path.compare("/uploadFile") == 0){
			if(username.compare("none") == 0){
				redirectToLogin(comm_fd);
			} else {
				std::string boundaryDelim("boundary=");
				std::string boundary("none");

				//Get the boundary from the header
				int startOfBoundary = headers.find(boundaryDelim);
				if(startOfBoundary != -1){
					std::string boundaryHeader = headers.substr(startOfBoundary + 9);
					//std::cout << "Boundary Header: " << boundaryHeader << std::endl;
					startOfLine = boundaryHeader.find(lineDelim);
					boundary = "--" + boundaryHeader.substr(0, startOfLine); //TODO Verify with TA!!
					std::cout << "Got boundary: " << boundary << std::endl;
				} else {
					printf("ERROR! Boundary not found for Upload File\n");
				}
				//TODO Process the body for upload file
				size_t pos = 0;
				std::string bodyCopy(newBody.c_str(), contentLength);
				std::string part;
				std::string filenameDelim("filename=");

				int i = 1;
				while ((pos = bodyCopy.find(boundary)) != std::string::npos) {
					part = bodyCopy.substr(0, pos);
					//std::cout << "i: " << i++ << part << std::endl;
					bodyCopy.erase(0, pos + boundary.length());

					// Find the pos of filename and the contents
					if((pos = part.find(filenameDelim)) != std::string::npos){
						std::string fileStr = part.substr(pos + filenameDelim.length());
						std::string contentStr = part.substr(pos);

						//Get the filename
						startOfLine = fileStr.find(lineDelim);
						std::string fileName;
						fileName = fileStr.substr(0, startOfLine);
						//Remove the double quotes around the filename
						fileName.erase(remove( fileName.begin(), fileName.end(), '\"' ),fileName.end());
						postParam["fileName"] = fileName;
						std::cout << "FileName: " << postParam["fileName"] << std::endl;

						//Get the contents
						int startOfContents = contentStr.find(headDelim);
						std::string contents = contentStr.substr(startOfContents + headDelim.length());

						postParam["contents"] = contents;
						logSuccess("Contents Here (%d)",  (int)contents.length());
						logSuccess("End of contents");

					} else if( (pos = part.find("directory")) != std::string::npos){

						int startOfDir = part.find(headDelim);
						std::string mainDirectory = part.substr(startOfDir + headDelim.length());
						// Strip space from the directory name
						mainDirectory.erase(remove_if( mainDirectory.begin(), mainDirectory.end(), isspace),mainDirectory.end());
						postParam["directory"] = mainDirectory;
						std::cout << "Directory: " << postParam["directory"] << "End of directory" << std::endl;
					}

				}
				if(bodyCopy.length() > 0){ //TODO verify with TA
					std::cout << "End of body: " << bodyCopy << std::endl;
				}
				handleUploadFile(comm_fd, username, postParam);
			}
		} else {
			// Strip space from body
			////body.erase(remove_if(body.begin(), body.end(), isspace), body.end());
			//newBody.erase(remove_if(newBody.begin(), newBody.end(), isspace), newBody.end());
			do{
				/*
				pairStart = body.find('&');
				std::string pair = body.substr(0, pairStart);
				body = body.substr(pairStart+1);
				*/

				pairStart = newBody.find('&');
				std::string pair = newBody.substr(0, pairStart);
				newBody = newBody.substr(pairStart+1);

				int valStart = pair.find("=");
				key = pair.substr(0, valStart);
				value = pair.substr(valStart + 1);
				std::cout << "key: " << key << " value: " << value << std::endl;
				postParam[key] = value;
			} while (pairStart != -1);

			if(path.compare("/register") == 0){
				handleRegisterPage(comm_fd, postParam);
			} else if(path.compare("/login") == 0){
				handleLoginPage(comm_fd, postParam);
			} else if(path.compare("/send") == 0){
				if (username.compare("none") == 0) redirectToLogin(comm_fd);
				else {
					handleSend(comm_fd, username, postParam);
					logSuccess("handleSend finishes");
				}
			} else if(path.compare("/forward") == 0) {
				if (username.compare("none") == 0) redirectToLogin(comm_fd);
				else handleForward(comm_fd, username, postParam);
			} else if(path.compare("/reply") == 0) {
				if (username.compare("none") == 0) redirectToLogin(comm_fd);
				else handleReply(comm_fd, username, postParam);
			} else if(path.compare("/delete") == 0) {
				if (username.compare("none") == 0) redirectToLogin(comm_fd);
				else handleDeleteMail(comm_fd, username, postParam);
			}
			else if(path.compare("/playMove") == 0) {
				handlePlayMove(comm_fd, username, postParam);
			} else {
				//TODO decide how to handle
				printf("##### Some unknown path for POST!!! \n");
			}
		} // Not Upload file
	} else {
		printf("Unknown HTTP Method\n");
	}
	printf("EOFXN\n");

	close(comm_fd);
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	/*	The following three lines are important
	 * 	Most of the stuff will break / not work if these are removed
	 */

	config = std::string(argv[1]);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, terminate);

	/*	Also important	*/
	logSuccess("Om Shree Ganeshaya Namaha!");

	/*	Important lines end	lol */

  	int c;

	//Check for optional arguments
	while ((c = getopt(argc, argv, "p:v")) != -1) {
		switch (c) {
		case 'p':
			portNum = atoi(optarg);
			break;
		case 'v':
			verbose = true;
			break;
		case '?':
			break;
			printf("Usage -v for verbose and -p for port number\n");
			exit(1);
		}
	}

//	Create and bind the server socket
	int listen_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0) {
		fprintf(stderr, "Couldn't open socket\n");
		exit(1);
	}

//	Set socket for reuse
	int enable = 1;

	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		logVerbose("Failed to set socket for reuse");
	} else {
		logVerbose("Socket set for reuse");
	}

	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htons(INADDR_ANY );
	servaddr.sin_port = htons(portNum);
	if (bind(listen_fd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
		fprintf(stderr, "Couldn't Bind to %d\n", ntohs(portNum));
		exit(1);
	}
	if (listen(listen_fd, 100) < 0) {
		fprintf(stderr, "Couldn't Listen\n");
		exit(1);
	}

	//Populate the lines of the config file
	populateConfigLines();

	while (true) {
		struct sockaddr_in clientaddr;
		socklen_t clientaddrlen = sizeof(clientaddr);
		int *fd = (int*) malloc(sizeof(int));
		*fd = accept(listen_fd, (struct sockaddr*) &clientaddr, &clientaddrlen);
		if (*fd < 0) {
			fprintf(stderr, "Couldn't Accept\n");
			exit(1);
		}
		if (verbose)
			fprintf(stderr, "[%d] New Connection\n", *fd);

		pthread_t thread;
		pthread_create(&thread, NULL, worker, fd);
	}

	return 0;
}
