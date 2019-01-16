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
#include <iterator>
#include "functions.h"

using namespace std;

struct clientStruct {

	struct sockaddr_in address;
	int socket;
};

//	Global variables
std::map <std::string, std::map<std::string, std::string> > tablet;
int port;

// Recovery functions

void recovery_put(std::string row_key, std::string col_key, std::string value)
{

	logVerbose("**********Received %s, %s, %s", row_key.c_str(), col_key.c_str(), value.c_str());
	tablet[row_key][col_key] = value;
}

int recovery_delete(std::string row_key, std::string col_key)
{
	if(tablet.find(row_key) == tablet.end())
		return 0;
	if(tablet[row_key].find(col_key) == tablet[row_key].end())
		return 0;
	tablet[row_key].erase(col_key);
	return 1;
}

//	Big table functions

void put (std::string message) {

	logVerbose("PUT Function received %d bytes", (int)message.length());

//	PUT(row,column,value)

	message = message.substr(message.find("(") + 1, std::string::npos);
	std::string row_key = message.substr(0, message.find(","));
	message = message.substr(message.find(",") + 1, std::string::npos);
	std::string col_key = message.substr(0, message.find(","));
	message = message.substr(message.find(",") + 1, std::string::npos);
	std::string value = message.substr(0, message.length() - 1);

	tablet[row_key][col_key] = value;

	logVerbose("Stored, %d bytes to %s, %s", (int)value.length(), row_key.c_str(), col_key.c_str());

	// write the operation to log file
	std::stringstream log_path;
	log_path << "logs/";
	log_path << port;
	log_path << ".log";
	// PUT requests have identifier 1 in the log file
	std::string write_this = "1***" + row_key + "|||" + col_key + "|||" + value + "&:*:&";
	ofstream outfile(log_path.str(), ios_base::app);
	outfile << write_this;
	outfile.close();
	logVerbose("%d: logging complete", port);
}

string get (std::string message) {

//	GET(row,column);

	std::string row_key = message.substr(message.find("(") + 1, message.find(",") - message.find("(") - 1);
	std::string col_key = message.substr(message.find(",") + 1, message.find(")") - message.find(",") - 1);

	if (tablet.find(row_key) == tablet.end())
		return "[0]0";
	else if (tablet[row_key].find(col_key) == tablet[row_key].end())
		return "[0]0";
	else
		return tablet[row_key][col_key];
}

int delete_from_tablet (std::string message) {

	std::string row_key = message.substr(message.find("(") + 1, message.find(",") - message.find("(") - 1);
	std::string col_key = message.substr(message.find(",") + 1, message.find(")") - message.find(",") - 1);

	if(tablet.find(row_key) == tablet.end())
		return 0;
	if(tablet[row_key].find(col_key) == tablet[row_key].end())
		return 0;
	tablet[row_key].erase(col_key);

	// write the operation to log file
	std::stringstream log_path;
	log_path << "logs/";
	log_path << port;
	log_path << ".log";
	// DELETE requests have identifier 2 in the log file
	std::string write_this = "2***" + row_key + "|||" + col_key + "&:*:&";
	ofstream outfile(log_path.str(), ios_base::app);
	outfile << write_this;
	outfile.close();
	logVerbose("%d: Deletion Logging complete", port);
	return 1;
}

int cput(std::string message) {

//	CPUT(row,column,value_1--><--value_2)

	message = message.substr(message.find("(") + 1, std::string::npos);
	std::string row_key = message.substr(0, message.find(","));
	message = message.substr(message.find(",") + 1, std::string::npos);
	std::string col_key = message.substr(0, message.find(","));
	message = message.substr(message.find(",") + 1, std::string::npos);
	std::string value1 = message.substr(0, message.find("-->|<--"));
	message = message.substr(message.find("-->|<--") + 7, std::string::npos);
	std::string value2 = message.substr(0, message.length() - 1);

	if(tablet.find(row_key) == tablet.end())
		return 0;
	if (tablet[row_key].find(col_key) == tablet[row_key].end())
		return 0;
	if ((tablet[row_key][col_key] == value1) || (std::string(tablet[row_key][col_key]).substr(std::string(tablet[row_key][col_key]).find("]") + 1, std::string::npos) == value1.substr(value1.find("]") + 1, std::string::npos)))
	{
		tablet[row_key][col_key] = value2;

		// write the operation to log file
		std::stringstream log_path;
		log_path << "logs/";
		log_path << port;
		log_path << ".log";
		// CPUT requests have identifier 1 in the log file (if it happened, old value must have been value1; peace)
		std::string write_this = "1***" + row_key + "|||" + col_key + "|||" + value2 + "&:*:&";
		ofstream outfile(log_path.str(), ios_base::app);
		outfile << write_this;
		outfile.close();
		logVerbose("%d: CPUT Logging complete", port);

		return 1;
	}
	return 0;
}

int initialize(int port) {

	struct sockaddr_in server;

	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &(server.sin_addr));
	server.sin_port = htons(port);

	int listen_fd = socket(PF_INET, SOCK_STREAM, 0);
	int enable = 1;

	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	bind(listen_fd, (struct sockaddr*) &server, sizeof(server));

	logVerbose("Server started at %s:%d.", inet_ntoa(server.sin_addr), ntohs(server.sin_port));
	return listen_fd;
}

clientStruct acceptConnection(int socket) {

	struct sockaddr_in client;
	socklen_t clientaddrlen = sizeof(client);
	int * comm_fd = (int*) malloc(sizeof(int));
	*comm_fd = accept(socket, (struct sockaddr*) &client, &clientaddrlen);
	logVerbose("New connection from %s:%d", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

	clientStruct myClient;
	myClient.address = client;
	myClient.socket = *comm_fd;

	return myClient;
}

void send(clientStruct client, std::string message) {

//	Writes a message to the socket

	signal(SIGPIPE, SIG_IGN);

	logSend("%s (%d)", message.c_str(), (int)message.length());

	int length = message.size();
	write(client.socket, &length, sizeof(length));

	for (int i = 0; i < message.length(); i++) {
		write(client.socket, &message[i], sizeof(message[i]));
	}
}

void sendACK(clientStruct client) {

	send(client, "+ACK");
}

std::string receive(clientStruct client) {

//	Reads a message from the socket and sends an ACK

	int length;
	std::string buffer;

	read(client.socket, &length, sizeof(length));

	for (int j = 0; j < length; j++) {

		char character;
		read(client.socket, &character, sizeof(character));
		buffer.push_back(character);
	}

	logReceive("%s (%d)", buffer.c_str(), length);
	sendACK(client);

	return buffer;
}

int parseCommand(std::string message) {

	/*
	Identifies commands and returns the code.
	Supported commands:

	1) GET
	2) PUT <integer value>
	3) CPUT
	4) DELETE
	5) NOOP
	6) QUIT
	7) LOAD
	8) DISABLE
	9) STATS

	*/

	for (int i = 0; i < message.size(); i++) {
		message[i] = tolower(message[i]);
	}

	if (message.substr(0,3) == "get") {
		return 1;
	}
	else if(message.substr(0,3) == "put") {
		return 2;
	}
	else if(message.substr(0,4) == "cput") {
		return 3;
	}
	else if(message.substr(0,6) == "delete") {
		return 4;
	}
	else if(message.substr(0,4) == "noop") {
		return 5;
	}
	else if(message.substr(0,4) == "quit") {
		return 6;
	}
	else if(message.substr(0,4) == "load") {
		return 7;
	}
	else if(message.substr(0,7) == "disable") {
		return 8;
	}
	else if(message.substr(0,5) == "stats") {
		return 9;
	}
	else {

		logVerbose("Message was: %s", message.c_str());
		return 0;
	}
}

void *worker(void *arg) {

	clientStruct client = *(clientStruct*) arg;

	while(true) {

		pthread_t id = pthread_self();

		std::string message = receive(client);

		int command = parseCommand(message);

		if (command == 1) {

		//	GET(row, column)
			send(client, get(message));
		}
		else if (command == 2) {

			put(message);
		}
		else if (command == 3) {

			if (cput(message)) {
				logSuccess("CPUT Successful");
			}
			else {
				logVerbose("CPUT Failed");
			}
		}
		else if (command == 4) {

			if (delete_from_tablet(message)) {
				logSuccess("DELETE Successful");
			}
			else {
				logVerbose("DELETE Failed");
			}
		}
		else if (command == 5) {
			continue;
		}
		else if (command == 6) {

		//	Quit
			logVerbose("Closing connection with the client");
			close(client.socket);
			pthread_exit(NULL);
		}
		else if (command == 7) {

		//  LOAD
			std::stringstream response;
			int final_size;
			map <std::string, std::map<std::string, std::string> >::iterator it1 = tablet.begin();

			while(it1 != tablet.end())
			{
				string row = it1->first;
				map <std::string, std::string> col_val = it1->second;
				map <std::string, std::string>::iterator it2 = col_val.begin();

				while(it2 != col_val.end())
				{
					string val = it2->second;
					final_size = final_size + val.size();
					it2++;
				}
				it1++;
			}
			response << "[" << time() << "]" << final_size;
			send(client, response.str());
		}
		else if (command == 8) {
			// kill this server
			exit(1);
		}
		else if (command == 9) {
			std::string result = "";
			map <std::string, std::map<std::string, std::string> >::iterator it1 = tablet.begin();

			if (it1 == tablet.end())
			{
				result = "::empty::";
			}

			else
			{
				int danger_flag = 0;
				while(it1 != tablet.end())
				{
					string row = it1->first;
					map <std::string, std::string> col_val = it1->second;
					map <std::string, std::string>::iterator it2 = col_val.begin();

					while(it2 != col_val.end())
					{
						string col = it2->first;
						string val = it2->second;

						result = result + row + ":::" + col + ":::" + val + "|||";
						it2++;
					}
					it1++;
				}
			}

			send(client, result);
		}
		else {

			logVerbose("Command couldn't be recognized");
			logVerbose("Closing connection with the client");
			close(client.socket);
			pthread_exit(NULL);
		}
	}

	return NULL;
}

void *ckpt_worker(void *arg)
{
	// takes a checkpoint every 30 seconds
	// deletes old checkpoint
	// flushes log

	// write the tablet to the checkpoint file
	while(true)
	{
		std::stringstream ckpt_path;
		ckpt_path << "ckpts/";
		ckpt_path << port;
	    ckpt_path << ".ckpt";

	    std::string result = "";
		map <std::string, std::map<std::string, std::string> >::iterator it1 = tablet.begin();
		ofstream outfile(ckpt_path.str());
		if (it1 == tablet.end())
		{
			result = "::empty::";
		}

		else
		{
			while(it1 != tablet.end())
			{
				string row = it1->first;
				map <std::string, std::string> col_val = it1->second;
				map <std::string, std::string>::iterator it2 = col_val.begin();

				while(it2 != col_val.end())
				{
					string col = it2->first;
					string val = it2->second;

					result = row + ":::" + col + ":::" + val + "&:*:&";
					outfile << result;
					it2++;
				}
				it1++;
			}
		}

		outfile.close();

		// flush the log file
		std::stringstream log_path;
		log_path << "logs/";
		log_path << port;
		log_path << ".log";

		ofstream logfile(log_path.str());
		logfile.close();

		sleep(30);
	}

	return NULL;
}

int main(int argc, char *argv[]) {

	system("mkdir ckpts/");
	system("mkdir logs/");

	std::stringstream stringPort(argv[1]);
	stringPort >> port;

	int socket = initialize(port);
	listen(socket, 10);

	// Recovery from checkpoint
	ifstream checkfile;
	std::stringstream ckpt_path;
	ckpt_path << "ckpts/";
	ckpt_path << port;
    ckpt_path << ".ckpt";

	checkfile.open(ckpt_path.str());
	// apparently using this syntax reads the entire file into the string file_contents
	std::string file_contents { istreambuf_iterator<char>(checkfile), istreambuf_iterator<char>() };
	int delim_pos;
	int primary_delim_pos;
	while (file_contents.find("&:*:&") != std::string::npos)
	{
		primary_delim_pos = file_contents.find("&:*:&");
		std::string line = file_contents.substr(0, primary_delim_pos);
		delim_pos = line.find(":::");
		std::string row_key = line.substr(0, delim_pos);
		line.erase(0, delim_pos+3);
		delim_pos = line.find(":::");
		std::string col_key = line.substr(0, delim_pos);
		line.erase(0, delim_pos+3);
		std::string value = line;
		tablet[row_key][col_key] = value;
		file_contents.erase(0, primary_delim_pos + 5);
	}
	checkfile.close();

	//Recovery from log
	ifstream logfile;
	std::stringstream log_path;
	log_path << "logs/";
	log_path << port;
	log_path << ".log";

	logfile.open(log_path.str());
	std::string log_file_contents { istreambuf_iterator<char>(logfile), istreambuf_iterator<char>() };

	while (log_file_contents.find("&:*:&") != std::string::npos) {
		primary_delim_pos = log_file_contents.find("&:*:&");
		std::string line = log_file_contents.substr(0, primary_delim_pos);
		if (line.substr(0,1) == "1")
		{
			line.erase(0,4);
			delim_pos = line.find("|||");
			std::string row_key = line.substr(0, delim_pos);
			line.erase(0, delim_pos+3);
			delim_pos = line.find("|||");
			std::string col_key = line.substr(0, delim_pos);
			line.erase(0, delim_pos+3);
			std::string value = line;
			recovery_put(row_key, col_key, value);
			logVerbose("Recovery: PUT operation successful");
		}
		else
		{
			line.erase(0,4);
			delim_pos = line.find("|||");
			std::string row_key = line.substr(0, delim_pos);
			line.erase(0, delim_pos+3);
			delim_pos = line.find("|||");
			std::string col_key = line.substr(0, delim_pos);
			line.erase(0, delim_pos+3);
			if(recovery_delete(row_key, col_key)) {
				logVerbose("Recovery: Delete operation successful");
			}
			else {
				logVerbose("Recovery: Delete operation failed");
			}
		}
		log_file_contents.erase(0, primary_delim_pos + 5);
	}
	logfile.close();

	pthread_t ckpt_thread;
	pthread_create(&ckpt_thread, NULL, ckpt_worker, NULL);

	while (true) {

		clientStruct client = acceptConnection(socket);

		pthread_t thread;
		pthread_create(&thread, NULL, worker, &client);
	}

	return 0;
}
