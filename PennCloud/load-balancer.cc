#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <pthread.h>
#include "functions.h"


std::string config;
int portNum = 8080;
std::vector<std::string> fsServers;
bool verbose = false;
int numReq = 0;
std::string headers("HTTP/1.1 307 Temporary Redirect\r\nLocation: http://");
std::string end("\r\nServer: 505Team11\r\nHost: CIS505\r\nConnection: close\r\n\r\n\r\n");


//Parses the list of front-end servers
void parseServerList(std::string configPath) {
	fsServers.clear();
	std::ifstream file(configPath);
	std::string contents;

	if (!file) {
		panic("An error occurred when reading %s", configPath.c_str());
	}

	std::string line;

	while (getline(file, line)) {
		if(line.find("FE") != std::string::npos){
			std::string server = line.substr(0, line.find(','));
			fsServers.push_back(server);
		}
	}

	for(int i = 0; i < fsServers.size(); i++){
		std::cout << i << ": " << fsServers[i] << std::endl;
	}
	file.close();
}

void *worker(void *arg){
	int comm_fd = *(int*)arg;
	int num = numReq % fsServers.size();
	std::string newHeader = headers;
	newHeader += fsServers[num] + end;


	printf("Num: %d, %s\n", num, fsServers[num].c_str());
	if(write(comm_fd, newHeader.c_str(), strlen(newHeader.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Headers\n", comm_fd);
	}

	if(write(comm_fd, headers.c_str(), strlen(headers.c_str())) <= 0){
		fprintf(stderr, "%d Thread Couldn't Write Headers\n", comm_fd);
	}
	if(verbose){
		logSuccess("%s", newHeader.c_str());
	}

	numReq++;
	pthread_exit(NULL);

}

int main(int argc, char *argv[]) {
  	int c;

	config = std::string(argv[1]);

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

	parseServerList(config);

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
	while (true) {
		struct sockaddr_in clientaddr;
		socklen_t clientaddrlen = sizeof(clientaddr);
		int *fd = (int*) malloc(sizeof(int));
		*fd = accept(listen_fd, (struct sockaddr*) &clientaddr, &clientaddrlen);
		if (fd < 0) {
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



