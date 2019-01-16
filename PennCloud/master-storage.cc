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
#include <map>
#include <iterator>
#include "functions.h"

using namespace std;

std::map <string, string> keytonode;
std::map <string, int> grouptoload;

struct inputs {
	int p;
	bool verbose;

	inputs() {
		p = 0;
		verbose = false;
	}
};

inputs in;

inputs parseInput(int argc, char *argv[]) {

	//	Parses user input and pushes to the structure inputs

	inputs in;
	int c = 0;
	int error = 0;

	while ((c = getopt(argc, argv, ":p:v")) != -1) {

		switch (c) {
		case 'p':
			in.p = atoi(optarg);
			break;

		case 'v':
			in.verbose = true;
			break;

		case ':':
			fprintf(stderr, "Error: -%c expects an argument\n", optopt);
			error++;
			break;

		case '?':
			fprintf(stderr, "Error: Invalid argument, %c\n", optopt);
			error++;
			break;
		}
	}

	if (in.p == 0) {
		in.p = 10000;
	}

	if (error == 1) {
		printf("Usage: %s -p <portno> [-a][-v]\n", argv[0]);
		exit(1);
	}

	return in;
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

std::string extractSearchKey(std::string buffer) {

	std::string extracted = buffer.substr(buffer.find("Search Key: ") + 12, std::string::npos);
	extracted = extracted.substr(0, extracted.length() - 2);
	return extracted;
}

int hashed(std::string key, int nodes) {

	return key[0] % nodes;
}

int redirect(std::string key, int nodes) {

//	Check if this key was already redirected



//	If not, just return the has and log it
	return hashed(key, nodes);
}

int main(int argc, char *argv[]) {

	in = parseInput(argc, argv);

	//	Open socket and set port reuse
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
	servaddr.sin_port = htons(in.p);
	bind(listen_fd, (struct sockaddr*) &servaddr, sizeof(servaddr));
	logVerbose("Server bound to %s:%d", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));
	listen(listen_fd, 10);

//	Continue accepting connections - and send redirection

	while (true) {
		struct sockaddr_in clientaddr;
		socklen_t clientaddrlen = sizeof(clientaddr);
		logVerbose("Waiting for client to join...");
		int comm_fd = accept(listen_fd, (struct sockaddr*) &clientaddr, &clientaddrlen);
		logVerbose("New connection from: %s\n", inet_ntoa(clientaddr.sin_addr));

		string initial = "+OK You have connected with the master storage. Please ping the search key to be redirected to the local node. Expected format { Search Key: <Alphabet> }";
		sendMessage(comm_fd, initial);
		string buffer = readMessage(comm_fd);

		printf("Received: %s", buffer.c_str());

		string key = extractSearchKey(buffer);

		logVerbose("Search Key: %s", key.c_str());
		string new_key_string;

		string node_index;
		// this means that the message is from a client

		// check if message is from the heartbeat server
		if (key.substr(0,9) == "heartbeat")
		{
			cout << "message from heartbeat" << endl;
			new_key_string = key.substr(key.find("heartbeat") + 9);
			vector< string > tokens;
			split(new_key_string, ":", tokens);
			for (int i=0; i<tokens.size(); i++)
			{
				cout << tokens[i] << endl;
				vector <string> load_tokens;
				split(tokens[i], "-", load_tokens);
				grouptoload[load_tokens[0]] = stoi(load_tokens[1]);
			}
		}
		else if (key.substr(0,9) == "ADMININFO")
		{
			std::string response = "Master's reply to admininfo";
			//log("%s", "in admin if condtion");
			sendMessage(comm_fd, response);
		}
		else
		{
			// if key is present in the keytonode mapping
			if (keytonode.find(key) != keytonode.end())
			{
				node_index = keytonode[key];
			}
			// if key is not present in the keytonode mapping, then find the minimum load group
			else
			{
				logVerbose("%s", "message from client");
				map<string, int>::iterator it = grouptoload.begin();
				int min_load = it->second;
				node_index = it->first;
				while(it != grouptoload.end())
				{
					if(it->second < min_load)
					{
						min_load = it->second;
						node_index = it->first;
					}
					it++;
				}
				keytonode[key] = node_index;
			}
			std::stringstream response;
			response << node_index;

			sendMessage(comm_fd, response.str());
		}

		close(comm_fd);
	}
}
