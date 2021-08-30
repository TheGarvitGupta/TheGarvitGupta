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
#include <errno.h>

//	Forward Declarations

int read();
std::string read(int);
void write(std::string);
void readGroups(std::string);
std::string getServerStatistics(std::string, int);

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

std::vector<serverStruct> servers;
std::vector<message> messages;
std::vector<std::string> groups;
serverStruct master_server;
std::string config;

std::map <std::string, int> grouptoload;
std::map <int, int> servertoload; // mapping from port numbers to individual load
std::map <int, bool> servertoactive; // mapping from port numbers to active state
std::map <int, std::string> servertotablet; // mapping from port numbers to individual tablets
std::vector<std::string> configLines; // lines of the config file

// Function to populate the lines of the config file so that it's read only once
void populateConfigLines(){

	configLines.clear();
	std::ifstream file(config);
	std::string line;

	if (!file) {
		panic("Error: Could not read file <%s>", config.c_str());
	}

	while (getline(file, line)){
		configLines.push_back(line);
	}

	log("Length of the configLines %zu", configLines.size());
	file.close();
}

void readGroups(std::string configPath) {
	// reads all groups present and writes as string in the vector

	for (int i = 0; i < configLines.size(); i++) {
		std::string line = configLines.at(i);

		if (line[0] != '#') {

			if(line.find("- ") != std::string::npos)
			{
				std::string new_group = line.substr(line.find("- ") + 2, std::string::npos);
				if(std::find(groups.begin(), groups.end(), new_group) == groups.end())
					groups.push_back(new_group);
			}
		}
	}

	for (int i=0;i<groups.size(); i++)
	{
		grouptoload[groups[i]] = 0;
	}

	log("Num of groups %zu", groups.size());
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
		close(servers[i].socket);
		logReceive("(%d) %s", ntohs(servers[i].address.sin_port), response.c_str());
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

std::string bufferMessage(std::string buffer) {

	message current;

	std::stringstream timestamp(buffer.substr(1, buffer.find("]") - 2));
	std::stringstream value(buffer.substr(buffer.find("]") + 1, std::string::npos));
	timestamp >> current.timestamp;
	value >> current.value;

	messages.push_back(current);
	return current.value;
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

void sendMessage(int comm_fd, std::string message) {

	int length = message.length();
	write(comm_fd, &length, sizeof(length));

	for (int i = 0; i < message.length(); i++) {
		char element = message[i];
		write(comm_fd, &element, sizeof(char));
	}
}

std::string readMessage(int comm_fd) {

	int length;
	std::string buffer;

	log("Starting the read");

	if (read(comm_fd, &length, sizeof(length)) <= 0) {
		log("Connection : %d returning -1", comm_fd);
		return "-1";
	}
	else {
		log("Read fine");
	}

	log("Buffer before %s", buffer.c_str());

	for (int j = 0; j < length; j++) {

		char character;
		read(comm_fd, &character, sizeof(character));
		buffer.push_back(character);
	}

	log("Buffer after %s", buffer.c_str());

	return buffer;
}

int read() {

//	Read from all the R servers

	int count = 0;

	for (int i = 0; i < servers.size(); i++) {

	//	Reads a message from the socket

		int length;
		std::string buffer;

		if (read(servers[i].socket, &length, sizeof(length)) <= 0) {
			logVerbose("Could not connect to %d", ntohs(servers[i].address.sin_port));
			servers[i].active = false;
			servertoload[ntohs(servers[i].address.sin_port)] = 0;
			servertoactive[ntohs(servers[i].address.sin_port)] = servers[i].active;
			servertotablet[ntohs(servers[i].address.sin_port)] = "::empty::";
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

		std::string just_message = bufferMessage(buffer);

		servertoload[ntohs(servers[i].address.sin_port)] = stoi(just_message);
		servertoactive[ntohs(servers[i].address.sin_port)] = servers[i].active;
		servertotablet[ntohs(servers[i].address.sin_port)] = getServerStatistics(config, ntohs(servers[i].address.sin_port));
	}

//	Go through all the messages and add the load values returned

	int final = 0;

	for (int i = 1; i < messages.size(); i++) {
		final = final + stoi(messages[i].value);
	}

	messages.clear();

	return final;
}

void write(std::string message) {

	logSend("%s (%d)", message.c_str(), (int)message.length());

	int count = 0;

	for (int i = 0; i < servers.size(); i++) {

		int length = message.size();
		log("current server: %d", ntohs(servers[i].address.sin_port));
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
}

void parseServerList(std::string configPath, std::string group) {

//	Parses server list in the storage.txt configuration file and stores in a vector: servers

	servers.clear();


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

			if (line.substr(line.find("- ") + 2, std::string::npos) == group) {

				serverStruct server;

				bzero(&server.address, sizeof(server.address));
				server.address.sin_family = AF_INET;

				std::stringstream port(line.substr(line.find(':') + 1, line.find(",") - line.find(':') - 1));
				int portNumber = 0;
				port >> portNumber;
				server.address.sin_port = htons(portNumber);

				inet_pton(AF_INET, line.substr(0, line.find(':')).c_str(), &(server.address.sin_addr));

				log("Added server: %s:%d", inet_ntoa(server.address.sin_addr), ntohs(server.address.sin_port));

				servers.push_back(server);
			}
		}
	}

}

void disableServer(std::string configPath, int this_port) {
	// disable the server running at a given port
	std::string value;

	for (int i = 0; i < configLines.size(); i++) {
		std::string line = configLines.at(i);

		if (line[0] != '#') {

			if (line.find("- ") != std::string::npos) {

				serverStruct server;

				bzero(&server.address, sizeof(server.address));
				server.address.sin_family = AF_INET;

				std::stringstream serverIP(line.substr(0, line.find(":")));
				std::stringstream serverport(line.substr(line.find(':') + 1, line.find(",") - line.find(':') - 1));
				int portNumber = 0;
				serverport >> portNumber;

				if(portNumber == this_port)
				{
					struct sockaddr_in heartbeat;
					bzero(&heartbeat, sizeof(heartbeat));
					heartbeat.sin_family = AF_INET;
					heartbeat.sin_port = htons(portNumber);
					inet_pton(AF_INET, serverIP.str().c_str(), &(heartbeat.sin_addr));

				//	Connect to the master via TCP

					int sock = socket(PF_INET, SOCK_STREAM, 0);

					if(connect(sock, (struct sockaddr*) &heartbeat, sizeof(heartbeat)) < 0) {

						logVerbose("Could not establish connection with the server at %d", ntohs(heartbeat.sin_port));
					}

					log("Established connection with the server");

					std::string response = "DISABLE";

					logVerbose("Sending: %s (%d)", response.c_str(), (int)response.length());

					int length = response.length();
					write(sock, &length, sizeof(length));

					for (int j = 0; j < response.length(); j++) {
						write(sock, &response[j], sizeof(response[j]));
					}
					log("Read ACK: %s", read(sock).c_str());

					value = read(sock);

					log("Received: %s", value.c_str());
					close(sock);
					break;
				}
			}
		}
	}
}

std::string getServerStatistics(std::string configPath, int this_port) {
	// get the entire tablet of server running at this port

	std::string value;

	for (int i = 0; i < configLines.size(); i++) {
		std::string line = configLines.at(i);

		if (line[0] != '#') {

			if (line.find("- ") != std::string::npos) {

				serverStruct server;

				bzero(&server.address, sizeof(server.address));
				server.address.sin_family = AF_INET;

				std::stringstream serverIP(line.substr(0, line.find(":")));
				std::stringstream serverport(line.substr(line.find(':') + 1, line.find(",") - line.find(':') - 1));
				int portNumber = 0;
				serverport >> portNumber;

				if(portNumber == this_port)
				{
					struct sockaddr_in heartbeat;
					bzero(&heartbeat, sizeof(heartbeat));
					heartbeat.sin_family = AF_INET;
					heartbeat.sin_port = htons(portNumber);
					inet_pton(AF_INET, serverIP.str().c_str(), &(heartbeat.sin_addr));

				//	Connect to the master via TCP

					int sock = socket(PF_INET, SOCK_STREAM, 0);

					if(connect(sock, (struct sockaddr*) &heartbeat, sizeof(heartbeat)) < 0) {

						logVerbose("Could not establish connection with the server at %d", ntohs(heartbeat.sin_port));
					}

					log("Established connection with the server");

					std::string response = "STATS";

					logVerbose("Sending: %s (%d)", response.c_str(), (int)response.length());

					int length = response.length();
					write(sock, &length, sizeof(length));

					for (int j = 0; j < response.length(); j++) {
						write(sock, &response[j], sizeof(response[j]));
					}
					log("Read ACK: %s", read(sock).c_str());

					value = read(sock);

					log("Received: %s", value.c_str());
					close(sock);
					break;
				}
			}
		}
	}
	log("returning this: %s", value.c_str());
	return value;
}

void restartServer(std::string this_port) {

	pid_t pid = fork();

	if (pid < 0)
	{
		log("%s", "Error in forking process to restart the server.");
	}
	if(pid == 0)
	{
		std::string command = "xterm -e ./quorum-server " + this_port;
		char* command_char = (char*) malloc(command.length() + 1);
		strcpy(command_char, command.c_str());
		logVerbose("%s", command_char);
		system(command_char);
		free(command_char);

		exit(1);
	}
	else
	{
		logVerbose("I am Chuck Norris");
		return;
	}
}

void *worker(void *arg) {

	int enable = 1;
	int listen_fd = socket(PF_INET, SOCK_STREAM, 0);

	if (listen_fd < 0) {
		panic("Failed to open socket");
	}
	else {
		logVerbose("Socket opened");
	}

	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {

		panic("Failed to set socket for reuse");
	}
	else {

		logVerbose("Socket set for reuse");
	}

//	Bind and listen
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
	servaddr.sin_port = htons(15000);
	bind(listen_fd, (struct sockaddr*) &servaddr, sizeof(servaddr));
	logVerbose("Server bound to %s:%d", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));
	listen(listen_fd, 10);

//	Continue accepting connections - and send redirection

	while (true) {
		struct sockaddr_in clientaddr;
		socklen_t clientaddrlen = sizeof(clientaddr);
		logVerbose("Waiting for client to join...");
		int comm_fd = accept(listen_fd, (struct sockaddr*) &clientaddr, &clientaddrlen);
		if(comm_fd <= 0){
			panic("Comm_fd value was %d", comm_fd);
		}
		logVerbose("New connection from: %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		std::string initial = "+OK You have connected with the heartbeat server.";
		sendMessage(comm_fd, initial);
		std::string buffer = readMessage(comm_fd);

		logVerbose("Received: %s", buffer.c_str());
		std::stringstream builder;
		std::string response;

		if (buffer.substr(0,7) == "DISABLE")
		{
			buffer.erase(0,7);
			int this_port = stoi(buffer);
			disableServer(config, this_port);
			logVerbose("Disabled server running at port %d", this_port);
		}

		else if (buffer.substr(0, 7) == "RESTART")
		{
			buffer.erase(0,7);
			restartServer(buffer);
			logVerbose("Restarted server at port %s", buffer.c_str());
		}

		else if (buffer.substr(0,9) == "ADMININFO")
		{
			// append the active information
			std::map<int, bool>::iterator it1 = servertoactive.begin();
			builder.str(""); // empty the contents of the string builder
			while(it1 != servertoactive.end())
			{
				builder << it1->first;
				builder << "*:*";
				builder << it1->second;
				builder << "|:|:|";
				it1++;
			}
			response = response + builder.str();

			// append primary delimiter
			response = response + "|*|*|";

			// append the load information
			std::map<int, int>::iterator it2 = servertoload.begin();
			builder.str(""); // empty the contents of the string builder
			while(it2 != servertoload.end())
			{
				builder << it2->first;
				builder << "*:*";
				builder << it2->second;
				builder << "|:|:|";
				it2++;
			}
			response = response + builder.str();

			// append primary delimiter
			response = response + "|*|*|";

			// append the tablet information
			std::map<int, std::string>::iterator it = servertotablet.begin();
			builder.str(""); // empty the contents of the string builder
			while(it != servertotablet.end())
			{
				builder << it->first;
				builder << "*:*";
				builder << it->second;
				builder << "|:|:|";
				it++;
			}
			response = response + builder.str();


			// send the final message
			sendMessage(comm_fd, response);
		}
		close(comm_fd);
	}

	return NULL;
}


int main(int argc, char *argv[]) {

	int value;

//	Signal handlers for SIGPIPE (write to a pipe) and CTRL + C
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, terminate);

	config = std::string(argv[1]);

	//Populate the lines of the config file
	populateConfigLines();

	readGroups(argv[1]);
	//disableServer(argv[1], 5000);
	//restartServer("5000");

	pthread_t thread;
	pthread_create(&thread, NULL, worker, NULL);

	while(true) {
		std::string message;

		message += "LOAD: ";
		message += time();

	    //	Redirection and establish connections

		for (int i=0; i<groups.size(); i++)
		{
			value = 0;
			std::string current_group = groups[i];
			parseServerList(argv[1], current_group);
			establishConnections();

			logVerbose("Sending %s (%d bytes)", message.c_str(), (int)message.length());
			write(message);
			value = read();
			closeConnections();

			// update the group to load map
			grouptoload[current_group] = value;
		}

		// send the load values to master node
		std::string response = "{ Search Key: heartbeat";
		for (int i=0; i<groups.size(); i++)
		{
			response = response + groups[i] + "-" + std::to_string(grouptoload[groups[i]]) + ":";
		}
		response.erase(response.length() - 1, 1);
		response = response + " }";

		servers.erase(servers.begin(), servers.end());
		servers.push_back(master_server);
		establishConnections();
		write(response);

		//closeConnections();
		logVerbose("Sleeping");
		sleep(5);
		logVerbose("Waking up");
	}

}
