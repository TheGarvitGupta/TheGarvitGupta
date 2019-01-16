#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <malloc.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <string>
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <resolv.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <signal.h>
#include <algorithm>
#include <sys/stat.h>
#include <fstream>
#include <ctime>
#include <chrono>
#include <sstream>
#include <mutex>
#include "functions.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <ctype.h>
#include <map>
#include <regex>
#include "structures.h"

using namespace std;

/* SMTP Structs */

struct mylock {

	std::string mailbox;
	std::mutex mutex;
};

struct inputs {
	int p;
	int v;
	std::string directory;

	inputs() {
		p = 0;
		v = 0;
		directory.clear();
	}
};

struct serverStruct {

	struct sockaddr_in address;
	int socket = -1;
	bool active = false;
};

struct message {

	uint64_t timestamp;
	std::string value;
};


struct information {
	// Stores the information about message the client the delivering
	std::string from;
	std::vector<std::string> to;
	std::string data;
	bool init;

	information() {
		from.clear();
		to.clear();
		data.clear();
		init = false;
	}

	void reset() {
		from.clear();
		to.clear();
		data.clear();
	}

	void print() {

		fprintf(stderr, "\nMAIL FROM: %s\n", from.c_str());
		fprintf(stderr, "RCPT TO:\n");

		for (int i = 0; i < to.size(); i++) {

			fprintf(stderr, "\t%s\n", to[i].c_str());
		}

		fprintf(stderr, "DATA:\n%s", data.c_str());
		fprintf(stderr, "Initialized: %s\n", init ? "true" : "false");
	}

	void initialize() {

		//		Initializes the connection with the client
		init = true;
	}

	bool initialized() {

		//		Returns true if session has been established
		return init;
	}

};



/* GET/PUT Logic */

//	Forward Declarations

std::string read(std::string);
std::string read(int);
void write(std::string, std::string);
std::string findGroup(std::string);


std::vector<message> messages_smtp;
std::vector<serverStruct> servers;
int W;
int R;
std::vector<pthread_t> threads;
std::vector<mylock*> locks;
std::vector<int*> fds;
inputs in;
struct sockaddr_in smtp_address;
std::string config;
serverStruct master_server, smtp_server;

std::map <int, int> servertoload; // mapping from port numbers to individual load
std::map <int, bool> servertoactive; // mapping from port numbers to active state
std::map <int, std::string> servertotablet; // mapping from port numbers to individual tablets


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
	messages_smtp.push_back(current);

	logVerbose("############################");

	logVerbose("Message: %s", current.value.c_str());

	logVerbose("############################");

}

void findMaster() {

	std::ifstream file(config);
	std::string contents;

	if (!file) {
		panic("Error: Could not read file <%s>", config.c_str());
	}

	std::string line;

	while (getline(file, line)) {
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
//			sleep(1);
			logVerbose("Could not connect to %d", ntohs(servers[i].address.sin_port));
//			sleep(5);
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

	message final = messages_smtp[0];

	for (int i = 1; i < messages_smtp.size(); i++) {

		if (messages_smtp[i].timestamp > final.timestamp) {

			final = messages_smtp[i];
		}
	}

	messages_smtp.clear();

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

	//return "G1";

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

void DELETE_ENTRY(std::string row, std::string column) {

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


void findSMTP() {

	std::ifstream file(config);
	std::string contents;

	if (!file) {
		panic("Error: Could not read file <%s>", config.c_str());
	}

	std::string line;

	while (getline(file, line)) {
		if (line[0] != '#') {
			if (line.find("SMTP") != std::string::npos)
			{

				bzero(&smtp_address, sizeof(smtp_address));
				smtp_address.sin_family = AF_INET;

				std::stringstream port(line.substr(line.find(':') + 1, line.find(",") - line.find(':') - 1));
				int portNumber = 0;
				port >> portNumber;
				smtp_address.sin_port = htons(portNumber);
				in.p = portNumber;
				inet_pton(AF_INET, line.substr(0, line.find(':')).c_str(), &(smtp_address.sin_addr));

			}
		}
	}
}



void shut_down_server(int signal) {

	//	Main thread calls SIGTERM on each thread when it receives SIGINT (CTRL + C)

	for (int i = 0; i < threads.size(); i++) {
		if (in.v == 1) {
			fprintf(stderr, "Sending SIGINT to thread: %lu\n", threads[i]);
		}
		pthread_kill(threads[i], SIGTERM);
	}

	//	By now threads would have started to cleanup and terminate
	//	Join all threads

	for (int i = 0; i < threads.size(); i++) {
		if (in.v == 1) {
			fprintf(stderr, "Joining thread: %lu\n", threads[i]);
		}
		void *status;
		pthread_join(threads[i], &status);
	}
	if (in.v == 1) {
		fprintf(stderr, "Shutting down server\n");
	}

	exit(0);
}

/* End GET/PUT Logic */

/* Start Relay Logic */

struct message_relay {

	string mailFrom;
	string recipientTo;
	string data;
	string allContents;

	void print(int index) {

		fprintf(stderr, "[%d] MAIL FROM:\t%s\n", index, mailFrom.c_str());
		fprintf(stderr, "[%d] RCPT TO:\t%s\n", index, recipientTo.c_str());
		fprintf(stderr, "[%d] DATA:\t%s", index, data.c_str());
	}
};

struct inputs_relay {
	int v;
	string username;

	inputs_relay() {
		v = 0;
		username.clear();
	}
};

inputs_relay in_relay;

inputs_relay parseRelayInput(int argc, char *argv[]) {

//	Parses user input and pushes to the structure inputs_relay

	inputs_relay in_relay;
	int c = 0;
	int error = 0;

	while ((c = getopt(argc, argv, "av")) != -1) {

		switch (c) {

		case 'a':
			fprintf(stderr, "%s\n", "Garvit Gupta");
			fprintf(stderr, "%s\n", "garvit@seas.upenn.edu");
			exit(0);
			break;

		case 'v':
			in_relay.v = 1;
			break;

		case '?':
			fprintf(stderr, "Error: Invalid argument, %c\n", optopt);
			error++;
			break;
		}
	}

	if (argc - optind == 1) {

//		in_relay.username = argv[optind];
//
//		if (in_relay.v == 1) {
//
//			fprintf(stdout, "Username set to: %s\n", in_relay.username.c_str());
//		}

		//string path = in.directory + "mqueue.mbox";
		//string pathSlash = in.directory + "/mqueue.mbox";

		//ifstream f(path.c_str());
		//ifstream g(pathSlash.c_str());

//		if (!f.good()) {
//			if (g.good()) {
//
//				in.directory = in.directory + "/";
//
//				if (in_relay.v == 1) {
//					fprintf(stderr, "Directory changed to: %s\n", in.directory.c_str());
//				}
//			}
//
//			else {
//				if (in_relay.v == 1) {
//					fprintf(stderr, "[ERROR] Could not find mqueue.mbox.\n");
//				}
//				error++;
//			}
//		}
	}
	else {

		fprintf(stderr, "Error: Ambiguous or no username provided.\n");
		error++;
	}

	if (error != 0) {
		printf("Usage: %s [-a][-v] <username>\n", argv[0]);
		exit(1);
	}

	if (in_relay.v == 1) {

		fprintf(stderr, "When executed, loads the mails via GET call from username argument and attempts to relay.\n");
		fprintf(stderr, "Note that you need to be connected to AirPennNet (or SSHed to SEAS) to send mails to that domain.\n");
		fprintf(stderr, "Please use SMTP+.cc to enqueue the messages to mqueue.mbox.\n");
	}

	return in_relay;
}

void sendToConnection(string buffer, int sockfd) {

	if (in_relay.v == 1) {
		fprintf(stderr, "[SENDING] %s", buffer.c_str());
	}

	for (int i = 0; i < buffer.size(); i++) {
		write(sockfd, &buffer[i], sizeof(char));
	}
}

int readFromConnection(int sockfd, string expected) {

	char character;
	std::string incoming;

	while (true) {
		read(sockfd, &character, sizeof(char));
		incoming.push_back(character);

		if (incoming.size() > 2 && incoming[incoming.size() - 2] == '\r' && incoming[incoming.size() - 1] == '\n') {

			if (in_relay.v == 1) {
				fprintf(stderr, "[RECEIVED] %s", incoming.c_str());
			}

			if (incoming.substr(0,3) == expected) {
				return 0;
			}
			else {
				return 1;
			}
		}
	}

	return 0;

}

int sendEmail(string domain, string mailFrom, string rcptTo, string data, int sockfd) {

	ostringstream command;
	int error = 0;
	logSuccess("FIRSTREAD");
	error += readFromConnection(sockfd, "220");
	if (error) {
		return 0;
	}
	logSuccess("HELO");
	command << "HELO ";
	command << domain;
	command << "\r\n";

	sendToConnection(command.str(), sockfd);
	error += readFromConnection(sockfd, "250");
	if (error) {
		return 0;
	}
	logSuccess("MAILFROM");
	command.str(std::string());
	command << "MAIL FROM:";
	command << mailFrom;
	command << "\r\n";

	sendToConnection(command.str(), sockfd);
	error += readFromConnection(sockfd, "250");
	if (error) {
		return 0;
	}
	printf("rcpt to: %s\n", rcptTo.c_str());
	command.str(std::string());
	command << "RCPT TO:";
	command << rcptTo;
	command << "\r\n";

	sendToConnection(command.str(), sockfd);
	error += readFromConnection(sockfd, "250");
	if (error) {
		return 0;
	}

	command.str(std::string());
	command << "DATA\r\n";

	sendToConnection(command.str(), sockfd);
	error += readFromConnection(sockfd, "354");
	if (error) {
		return 0;
	}

	command.str(std::string());
	command << data;
	command << "\r\n.\r\n";

	sendToConnection(command.str(), sockfd);
	error += readFromConnection(sockfd, "250");

	if (error == 0) {
		return 1;
	}
	else {
		return 0;
	}
}

string extractMX(char dispbuf[4096]) {

	ostringstream input;
	input << dispbuf;
	string record = input.str();
	return record.substr(record.find_last_of(" ") + 1, record.length() - record.find_last_of(" ") - 2);
}

string extractMXIP(char dispbuf[4096]) {

	ostringstream input;
	input << dispbuf;
	string record = input.str();
	return record.substr(record.find_last_of("\t") + 1, record.length() - record.find_last_of("\t") - 1);
}

void loadMessages(vector<message_relay> &relay_messages, string username) {

	if (in_relay.v == 1) {
		fprintf(stderr, "Loading messages...\n\n");
	}
	logSuccess("from FULL username to fetch for external: %s", username.c_str());
	std::string user = username.substr(1, username.find("@") - 1);
	logSuccess("from CLIPPED username to fetch for external: %s\n", user.c_str());
	//string path = directory + "mqueue.mbox";

//	ifstream f(path.c_str());
//	stringstream buffer;
//	string data;
//	buffer << f.rdbuf();
//	data = buffer.str();

	std::string data = GET(user, "mqueue");

	int position = data.find("From: ", 0);
	int index = 1;

	while (position != std::string::npos) {

		int secondPosition = data.find("From: ", position + 1);
		string contents = data.substr(position, secondPosition - position);

		message_relay m;
		m.allContents = contents;
		//m.mailFrom = contents.substr(contents.find("From: ") + 6, contents.find(" To: ") - contents.find("From: ") - 6);
		//m.recipientTo = contents.substr(contents.find("To ") + 3, contents.find(" ", contents.find("To") + 4) - contents.find("To ") - 3);
		int firstLess = contents.find("<");
		int firstGreater = contents.find(">");
		int secondLess = contents.find("<", firstLess+1);
		int secondGreater = contents.find(">", firstGreater+1);
		m.mailFrom = contents.substr(firstLess, firstGreater - firstLess + 1);
		m.recipientTo = contents.substr(secondLess, secondGreater - secondLess + 1);
		printf("m.recipientTo: %s\n", m.recipientTo.c_str());
		//m.data = contents.substr(contents.find(" ", contents.find("To ") + 3) + 1, std::string::npos);
		m.data = m.allContents;
		relay_messages.push_back(m);

		if (in_relay.v == 1) {
			m.print(index++);
		}
		position = secondPosition;
	}

	// Empty the file
//	std::ofstream file;
//	file.open(path.c_str(), std::ofstream::out | std::ofstream::trunc);
//	file.close();
	DELETE_ENTRY(user, "mqueue");
}

string extractDomain(message_relay msg) {

	string domain = msg.recipientTo.substr(msg.recipientTo.find("@") + 1, msg.recipientTo.find(">") - msg.recipientTo.find("@") - 1);
	return domain;
}

void saveMessage(message_relay m, string username) {

//	string path = directory + "mqueue.mbox";
//	std::fstream o;
//	o.open(path.c_str(), std::fstream::out | std::fstream::app);
//	o << m.allContents;
//	o.close();
	std::string clipped_name = username.substr(1, username.find("@")-1);
	PUT(username, "mqueue", m.allContents);

}

void relay_main(std::string username) {

	vector<message_relay> relay_messages;

	//in = parseRelayInput(argc, argv);
	//in_relay.username = username;
	in_relay.username = username;
	logSuccess("sending external from username...%s", username.c_str());
	in_relay.v = 1;
	loadMessages(relay_messages, username);

	cout << "\nTotal Messages: " << relay_messages.size() << endl;

	if (in_relay.v == 1) {
		fprintf(stderr, "\n%d messages loaded.\n\n", (int)relay_messages.size());
	}

	for (int y = 0; y < relay_messages.size(); y++) {

		if (in_relay.v == 1) {
			fprintf(stderr, "[%d] Attempting to deliver message #%d...\n", y+1, y+1);
		}

		vector<string> MXRecords;
		string extractedDomain = extractDomain(relay_messages[y]);
		char domain[4096];
		strcpy(domain, extractedDomain.c_str());
		u_char nsbuf[4096];
		char dispbuf[4096];
		ns_msg msg;
		ns_rr rr;
		int i, j, l;

		l = res_query(domain, C_IN, T_MX, nsbuf, sizeof(nsbuf));

		if (l > 0) {

			ns_initparse(nsbuf, l, &msg);
			l = ns_msg_count (msg, ns_s_an);

			if (in_relay.v == 1) {
				fprintf(stderr, "[%d] MX records found for %s:\n", y+1, extractedDomain.c_str());
			}

			for (j = 0; j < l; j++) {
				ns_parserr(&msg, ns_s_an, j, &rr);
				ns_sprintrr(&msg, &rr, NULL, NULL, dispbuf, sizeof(dispbuf));

				MXRecords.push_back(extractMX(dispbuf));
				if (in_relay.v == 1) {
					fprintf(stderr, "[%d] %s\n", y+1, dispbuf);
				}
			}
		}
		else {

			if (in_relay.v == 1) {
				fprintf(stderr, "[%d] No MX records found for %s.\n", y+1, extractedDomain.c_str());
				fprintf(stderr, "[%d] Failed to deliver message.\n\n", y+1);
			}
			saveMessage(relay_messages[y], username);
			continue;
		}

		logSuccess("domain prior to MXRecord loop: %s", domain);


//		u_char nsbuf_ip[4096];
//		char dispbuf_ip[4096];
//		ns_msg msg_ip;
//		ns_rr rr_ip;
//		int i_ip;
//		int l_ip;
//		l_ip = res_query(domain, ns_c_in, ns_t_a, nsbuf_ip, sizeof(nsbuf_ip));
//
//		if (l_ip < 0) {
//		   fprintf(stderr, "l_ip is less than 0\n");
//		} else {
//			ns_initparse(nsbuf_ip, l_ip, &msg_ip);
//			l_ip = ns_msg_count(msg_ip, ns_s_an);
//			for (i_ip = 0; i_ip < l_ip; i_ip++) {
//			  ns_parserr(&msg_ip, ns_s_an, 0, &rr_ip);
//			  ns_sprintrr(&msg_ip, &rr_ip, NULL, NULL, dispbuf_ip, sizeof(dispbuf_ip));
//			  printf("\t%s \n", dispbuf_ip);
//			}
//		}

		int delivered = 0;

		for (int m = 0; m < MXRecords.size(); m++) {

			if (delivered == 0) {

				std::string mx = MXRecords[m];
				if (in_relay.v == 1) {
					fprintf(stderr, "[%d] Delivering message to %s:25.\n", y+1, mx.c_str());
				}

				int sockfd = socket(PF_INET, SOCK_STREAM, 0);
				if (sockfd < 0) {
					fprintf(stderr, "[ERROR] Cannot open socket (%s)\n", strerror(errno));
					exit(1);
				}
				int ports[5] = { 5, 30, 20, 40, 10 };
				struct sockaddr_in servaddr;
				bzero(&servaddr, sizeof(servaddr));
				servaddr.sin_family = AF_INET;
				servaddr.sin_port = htons(25);

				std::string gmail_ip = "64.233.178.27";
				std::string seas_ip = "148.163.151.92";

				std::string smtp_ip = "";
				std::string domain_str(domain);
				logSuccess("detected domain: %s", domain_str.c_str());
				if (domain_str.compare("gmail.com") == 0) smtp_ip += gmail_ip;
				else if (domain_str.compare("seas.upenn.edu") == 0 || domain_str.compare("cis.upenn.edu") == 0) smtp_ip += seas_ip;

				logSuccess("Attempting to establish IP connection with %s:25\n", smtp_ip.c_str());


				inet_pton(AF_INET, smtp_ip.c_str(), &(servaddr.sin_addr));


				logSuccess("has Connected?");

				if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) != 0) {
					if (in_relay.v == 1) {
						fprintf(stderr, "[%d] Could not connect to %s:25.\n", y+1, mx.c_str());
						fprintf(stderr, "[%d] Could not connect to %s:25.\n", y+1, inet_ntoa(servaddr.sin_addr));
					}
					continue;
				}

				string mailFrom = relay_messages[y].mailFrom;
				string rcptTo = relay_messages[y].recipientTo;
				string data = relay_messages[y].data;

				logSuccess("connected, about to send");
				if (sendEmail(domain, mailFrom, rcptTo, data, sockfd)) {

					if (in_relay.v == 1) {
						fprintf(stderr, "[%d] Message accepted for delivery.\n", y+1);
					}
					delivered = 1;
				}

				close(sockfd);
			}
		}

		if (delivered == 0) {
			if (in_relay.v == 1) {
				fprintf(stderr, "[%d] Failed to deliver message.\n\n", y+1);
			}
			saveMessage(relay_messages[y], username);
		}
	}
}

void* relay_worker(void* args) {

	int argc;
	argc = 2;

//	char* uname = (char*) args;
//	char user_buf[2048];	// MAX USERNAME SIZE
//	strcpy(user_buf, uname);
//	printf("user buf: %s\n", user_buf);

	//std::string username(user_buf);

	std::string username = *reinterpret_cast<std::string*>(args);
	logSuccess("relay worker gets username: %s", username.c_str());
	relay_main(username);
	//relay_main(argc, argv);
}

/* End Relay Logic */






void shut_down_thread(int signal) {

	//	Worker Thread performs shutdown and cleanup on receiving the SIGTERM

	pthread_t thread;
	thread = pthread_self();
	int index = 0;
	while (threads[index] != thread) {
		index++;
	}

	int comm_fd = *fds[index];

	std::string writer = "421 localhost Service closing transmission channel\r\n";
	for (int i = 0; i < writer.length(); i++) {
		char item = writer[i];
		write(comm_fd, &item, sizeof(char));
	}
	close(comm_fd);
	pthread_exit(NULL);
}

int isEnd(std::string read) {

	//	Check if \r\n has been encountered

	if (read.size() < 2) {
		return 0;
	} else if (read[read.size() - 1] == '\n' && read[read.size() - 2] == '\r') {
		return 1;
	} else {
		return 0;
	}
}

int isCommand(std::string read) {

	//	Check if it is a potential command (echo/quit and return the code)

	// Just convert input string to lowercase for easy matching
	for (int i = 0; i < read.size(); i++) {

		read[i] = tolower(read[i]);
	}

	// Contains ECHO<SP> as the first five bytes
	if (read.substr(0, 5) == "echo ") {

		return 1;
	}

	// Contains QUIT as the first four bytes and immediately followed by \r\n
	else if (read.substr(0, 4) == "quit") {

		if (read.length() == 6) {
			return -1;
		} else {
			return 0;
		}
	}

	// Contains HELO<SP> as the first five bytes
	else if (read.substr(0, 5) == "helo ") {

		return 2;
	}

	// Contains MAIL FROM: as the first ten bytes
	else if (read.substr(0, 10) == "mail from:") {

		return 3;
	}

	// Contains RCPT TO: as the first eight bytes
	else if (read.substr(0, 8) == "rcpt to:") {

		return 4;
	}

	// Contains RESET as the first four bytes and immediately followed by \r\n
	else if (read.substr(0, 4) == "rset") {

		if (read.length() == 6) {
			return 5;
		} else {
			return 0;
		}
	}

	// Contains NOOP as the first four bytes and immediately followed by \r\n
	else if (read.substr(0, 4) == "noop") {

		if (read.length() == 6) {
			return 6;
		} else {
			return 0;
		}
	}

	// Contains DATA as the first four bytes and immediately followed by \r\n
	else if (read.substr(0, 4) == "data") {

		if (read.length() == 6) {
			return 7;
		} else {
			return 0;
		}
	}

	// Couldn't figure out what the command is
	else {

		return 0;
	}
}

std::string extractMessage(std::string read) {

	//	Extracts message between ECHO and \r\n
	return read.substr(5, read.length() - 7);
}

std::string extractFrom(std::string read) {

	//	Extracts message between MAIL FROM: and \r\n
	if (std::count(read.begin(), read.end(), '@') != 1) {
		return "0";

	} else if (read.find('<') != 0 + 10) {
		return "0";

	} else if (read.find('>') != read.length() - 3) {
		return "0";

	}
	return read.substr(10, read.length() - 12);
}

std::string extractRecipient(std::string read) {

	//	Extracts message between RCPT TO:< and >\r\n
	if (std::count(read.begin(), read.end(), '@') != 1) {
		return "0";
	}
	return read.substr(9, read.length() - 12);
}

std::string extractData(std::string read) {

	//	Extracts message between start of the message and end of the buffer (including \r\n)
	return read;
}

std::string extractUsername(std::string address) {
	return address.substr(0, address.length() - 10);
}

bool mailboxValid(std::string directory, std::string recipient) {
	logSuccess("recipient: %s || extractedUsername: %s", recipient.c_str(), extractUsername(recipient).c_str());


	logVerbose("Let's see what we read.");
//	sleep(5);

	std::string valid = GET(extractUsername(recipient), "PWD");

	logVerbose("Received %s", valid.c_str());
//	sleep(5);

	logSuccess("valid: %s", valid.c_str());
	return valid.at(0) != '0';
}

std::string domain(std::string address) {

	return address.substr(address.find("@") + 1,
			address.find(">") - address.find("@"));
}

int lockMailbox(std::string path) {

	for (int i = 0; i < locks.size(); i++) {

		if (locks[i]->mailbox == path) {

			if (in.v == 1) {
				fprintf(stderr, "Attempting to acquire the lock on %s...\n",
						path.c_str());
			}

			locks[i]->mutex.lock();

			if (in.v == 1) {
				fprintf(stderr, "Lock successfully acquired.\n");
			}

			return 1;
		}
	}

	if (in.v == 1) {
		fprintf(stderr,
				"No locks on %s. Attempting to create and acquire the lock...\n",
				path.c_str());
	}

	mylock *current = new mylock;
	current->mailbox = path;
	current->mutex.lock();

	locks.push_back(current);

	if (in.v == 1) {
		fprintf(stderr, "Lock successfully created and acquired.\n");
	}
	return 1;

}

int unlockMailbox(std::string path) {

	for (int i = 0; i < locks.size(); i++) {

		if (locks[i]->mailbox == path) {

			if (in.v == 1) {
				fprintf(stderr, "Found lock on %s. Attempting to release...\n",
						path.c_str());
			}

			locks[i]->mutex.unlock();

			if (in.v == 1) {
				fprintf(stderr, "Lock successfully released.\n");
			}

			return 1;
		}
	}

	if (in.v == 1) {
		fprintf(stderr, "Error: Could not find %s in the existing locks.\n",
				path.c_str());
	}

	return 1;
}

int finalizeMessage(information connection, std::string directory) {

	int external_counter = 0;
	std::string sender = "";
	for (int i = 0; i < connection.to.size(); i++) {

		std::string path;
		bool external;
		if (domain(connection.to[i]) == "PennCloud") {
			external = false;
			path = extractUsername(connection.to[i]) + ".mbox";
		} else {
			external = true;
			external_counter++;
			path = "Mail/forward/mqueue.mbox";

		}

		std::string mail;
		std::string datetime;

		std::time_t time = std::time(nullptr);
		datetime = std::asctime(std::localtime(&time));
		datetime[datetime.length() - 1] = 0;

		mail += "From";
		
		if (domain(connection.to[i]) == "PennCloud") {
			mail += " ";
			mail += connection.from;
			mail += "#";
			mail += datetime.c_str();
			mail += "*U";	// email begins as unread
			mail += "&" + std::to_string(time) + " ";

		} else {
			mail += ": ";
			// change domain from PennCloud to upenn.edu
			int ind_at = connection.from.find("@");
			sender = connection.from.substr(0, ind_at + 1) + "upenn.edu>";
			mail += sender;
			mail += "\n";
			mail += "To: <";
			mail += connection.to[i];
			mail += ">";
		}

		mail += connection.data;

		// write to mailbox
		lockMailbox(path);
		replaceAll(connection.to[i], "%40", "@");
		//std::ofstream o;
		//o.open(path, std::ios_base::app);
		//o << mail;
		if (external) {
			std::string currMail = GET(connection.from.substr(1, connection.from.find("@") - 1), "mqueue");
			std::string totExternal = "";
			if (currMail.at(0) == '0') totExternal += mail;
			else totExternal += currMail + mail;
			PUT(connection.from.substr(1, connection.from.find("@") - 1), "mqueue", totExternal);
			logSuccess("PUT into %s  mqueue: %s", connection.from.c_str(), totExternal.c_str());

//			std::ofstream o;
//			o.open(path, std::ios_base::app);
//			o << mail;
		} else {

			std::string mbox = "";
			mbox += GET(extractUsername(connection.to[i]), "mail");
//			char* argv[] = { (char*) connection.from.c_str(), NULL };
//			execve("Mail/relay", argv, NULL);
			// below print statement seg faults on empty mbox so be careful!!
			printf("the seg fault\n");
			printf("mbox is: %s\n", mbox.c_str());
			if (mbox.at(0) == '0') mbox = mail;	// empty case
			else mbox += mail;
			printf("not the seg fault\n");
			PUT(extractUsername(connection.to[i]), "mail", mbox);
		}
		unlockMailbox(path);
	}
	if (external_counter > 0) {
		//char usernam[sender.length()+1];
		//char* usernam = (char*) malloc(sender.length()+1);
		//strcpy(usernam, sender.c_str());
		//logSuccess("main thread sends %s username to worker", usernam);
		logSuccess("spawns relay worker thread....");
		pthread_t t;

		try {
			pthread_create(&t, NULL, relay_worker, (void*) &sender);
			pthread_join(t, NULL);
		}
		catch(...) {
			logVerbose("May not have relayed the mail.");
		}
	}
	return 1;
}

void *worker(void *arg) {

	//	Event handler when main thread is interrupted, it interrupts all the threads concurrently
	signal(SIGTERM, shut_down_thread);

	int comm_fd = *(int*) arg;

	std::string buffer;
	std::string writer;
	char character;
	bool shutdown = false;
	information connection;
	float command = 0;

	std::string initial = "220 localhost Service ready\r\n";

	for (int i = 0; i < initial.length(); i++) {
		char element = initial[i];
		write(comm_fd, &element, sizeof(char));
	}

	while (true) {

		//    	Check if client wants to terminate connection

		if (shutdown == true) {
			if (in.v == 1) {
				fprintf(stderr, "Terminating connection with the client\n");
			}
			close(comm_fd);
			if (in.v == 1) {
				fprintf(stderr, "[%d] Connection closed\n", comm_fd);
			}
			pthread_exit(NULL);
		}

		//		Keep waiting to read a byte

		if (read(comm_fd, &character, sizeof(char))) {

			buffer.push_back(character);
			std::string encoded = buffer;

		} else {
			if (in.v == 1) {
				fprintf(stderr, "The client died before saying goodbye\n");
			}
			close(comm_fd);
			if (in.v == 1) {
				fprintf(stderr, "[%d] Connection closed\n", comm_fd);
			}
			pthread_exit(NULL);
		}

		//		After reading each character, check if \r\n has been encountered, if that is the case parse input

		if (isEnd(buffer)) {

			if (in.v == 1) {
				fprintf(stderr, "[%d] C: %s\n", comm_fd, buffer.c_str());
			}

			if (in.v == 1) {
				fprintf(stderr, "Buffer: %s\n", buffer.c_str());
			}

			// 7.5: Extended Data Reading mode - Override command mode, append everything to data section until "."
			if (command != 7.5) {
				command = isCommand(buffer);
			}

			if (command == -1) { //QUIT
				writer = "221 localhost Service closing transmission channel";
				shutdown = true;
			} else if (command == 1) { //ECHO
				writer = "+OK " + extractMessage(buffer);
			} else if (command == 2) { //HELO
				connection.reset();
				connection.initialize();
				writer = "250 localhost";
			} else if (command == 3) { //Mail from:

				if (connection.from.length() != 0 && connection.from != "0") {

					writer = "503 Sender already specified";

				} else if (connection.initialized() == true) {

					connection.from = extractFrom(buffer);

					if (in.v == 1) {
						fprintf(stderr, "Connection From: %s\n",
								connection.from.c_str());
					}

					if (connection.from != "0") {
						writer = "250 OK";
					} else {
						writer =
								"550 Sender should be of the form <user@domain>";
					}
				} else {
					writer = "503 Bad sequence of commands";
				}
			} else if (command == 4) { //RCPT TO:

				if (connection.initialized() == true) {
					if (connection.from != "" && connection.from != "0") {
						if (extractRecipient(buffer) != "0") {
							if (domain(extractRecipient(buffer))
									== "PennCloud") {
								if (mailboxValid(in.directory,
										extractRecipient(buffer))) {
									connection.to.push_back(
											extractRecipient(buffer));
									writer = "250 OK";
								} else {
									writer = "550 No such user here";
								}
							} else {
								writer = "250 OK. Domain: "
										+ domain(extractRecipient(buffer))
										+ " detected.";
								connection.to.push_back(
										extractRecipient(buffer));
							}

						} else {
							writer =
									"550 Recipient should be of the form <user@domain>";
						}
					} else {
						writer = "503 Bad sequence of commands";
						//						If a RCPT command appears without a previous MAIL command, the server must return a 503 (Section 3.3 RFC 2821)
						//						One of the TAs pointed out to this RFC for HELO question, so assuming this can also be followed
					}
				} else {
					writer = "503 Bad sequence of commands";
				}
			} else if (command == 5) { //RSET
				connection.reset();
				writer = "250";
			} else if (command == 6) { //NOOP
				writer = "250";
			} else if (command == 7 || command == 7.5) { //DATA
				if (connection.initialized() == true) {
					if (connection.to.size() != 0) {
						if (connection.from != "") {
							if (extractData(buffer) == ".\r\n") {

								command = -2;
								if (finalizeMessage(connection, in.directory)
										== 1) {
									writer = "250 OK";
								} else {
									fprintf(stderr,
											"Something went wrong. Please deduct points.\n");
								}
								connection.reset();
							} else {

								if (command == 7) {
									command = 7.5;
									writer =
											"354 Start mail input; end with <CRLF>.<CRLF>";
								} else {
									connection.data += extractData(buffer);
								}
							}
						} else {
							writer = "503 Bad sequence of commands";
							command = -2;
						}
					} else {
						writer = "554 No valid recipients";
						command = -2;
					}
				} else {
					writer = "503 Bad sequence of commands";
					command = -2;
				}
			} else {
				writer = "500 Command unrecognized";
			}

			buffer.clear();

			//			Debug output
			if (in.v == 1) {
				fprintf(stderr, "Received command: %f", command);
				connection.print();
			}
		}

		//		There is something to send back to the client

		if (writer.length() > 0) {
			writer += "\r\n";
			char * push = new char[writer.length() + 1];
			strcpy(push, writer.c_str());

			if (in.v == 1) {
				fprintf(stderr, "[%d] S: %s\n", comm_fd, writer.c_str());
			}

			write(comm_fd, push, writer.length());

			//			Clear both the write and read buffers
			writer.clear();
		}
	}
	return 0;
}

inputs parseInput(int argc, char *argv[]) {

	//	Parses user input and pushes to the structure inputs

	inputs in;
	int c = 0;
	int error = 0;

	while ((c = getopt(argc, argv, ":p:av")) != -1) {

		switch (c) {
		case 'p':
			in.p = atoi(optarg);
			break;

		case 'a':
			fprintf(stderr, "%s\n", "Garvit Gupta");
			fprintf(stderr, "%s\n", "garvit@seas.upenn.edu");
			exit(0);
			break;

		case 'v':
			in.v = 1;
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

	if (argc - optind == 1) {
		in.directory = argv[optind];
	} else {

		fprintf(stderr, "Error: Ambiguous or no directory provided.\n");
		error++;
	}

	if (in.p == 0) {
		in.p = 30000;
	}

	if (error != 0) {
		printf("Usage: %s -p <portno> [-a][-v]\n", argv[0]);
		exit(1);
	}

	if (in.v == 1) {
		fprintf(stderr, "Directory set to %s\n", argv[optind]);
	}

	return in;
}

int main(int argc, char *argv[]) {

	/*	The following three lines are important
		 * 	Most of the stuff will break / not work if these are removed
		 */

//		config = std::string(argv[1]);
		signal(SIGPIPE, SIG_IGN);
//		signal(SIGINT, terminate);

		/*	Also important	*/
		logSuccess("Om Shree Ganeshaya Namaha!");

	/*	Important lines end	lol */

	if (argc < 2) {
		fprintf(stderr, "Please provide required arguments\n");
		return 1;
	}
	in = parseInput(argc, argv);
	if (in.v) config = std::string(argv[2]);
	else config = std::string(argv[1]);
	findSMTP();

	//	Signal handler when the user presses CTRL + C
	signal(SIGINT, shut_down_server);

	//	Open socket and set port reuse
	int enable = 1;
	int listen_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0) {
		if (in.v == 1) {
			fprintf(stderr, "Failed to open socket\n");
		}
	} else {
		if (in.v == 1) {
			fprintf(stderr, "Socket opened\n");
		}
	}
	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))
			< 0) {
		if (in.v == 1) {
			fprintf(stderr, "Failed to set socket for reuse\n");
		}
	} else {
		if (in.v == 1) {
			fprintf(stderr, "Socket set for reuse (SO_REUSEADDR)\n");
		}
	}

	//	Bind and listen
	bind(listen_fd, (struct sockaddr*) &smtp_address, sizeof(smtp_address));
	listen(listen_fd, 10);	// BACKLOG = 10

	int i = 0;
	if (in.v) printf ("Port: %d\n", in.p);
	//	Continue accepting connections - and send to thread for communication
	while (true) {
		struct sockaddr_in clientaddr;
		socklen_t clientaddrlen = sizeof(clientaddr);
		int* fd = (int*) malloc(sizeof(int));
		*fd = accept(listen_fd, (struct sockaddr*) &clientaddr, &clientaddrlen);
		printf("accepted conn. on socket: %d\n", *fd);
		if (in.v == 1) {

			std::string descriptor;
			std::ostringstream convert;
			convert << fd;
			descriptor = convert.str();

			fprintf(stderr, "[%s] %s\n", descriptor.c_str(), "New connection");
			fprintf(stderr, "From: %s\n", inet_ntoa(clientaddr.sin_addr));
		}

		pthread_t thread;
		pthread_create(&thread, NULL, worker, fd);
		threads.push_back(thread);
		fds.push_back(fd);

		if (in.v == 1) {
			fprintf(stderr, "Thread created: %lu\n", thread);
		}
	}
}
