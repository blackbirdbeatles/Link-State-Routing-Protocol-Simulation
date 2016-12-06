#include "project3.h"

vector<vector<int>> routingTable;
vector<vector<int>> routingCommands;
int numberOfRouters;
const char* ip;
int sockfd, new_fd;
vector<routerInfo> routers;

void sigHandler(int signum){
	cout << "Interrupt signal (" << signum << ") received.\n";

	close(sockfd);
	close(new_fd);
	exit(signum);

}

void readFile(ifstream& inFile){
	//creates a manager using the inFile, reading in the number of routers,
	//the routing table, and the commands to run
	string ManagerLine = "";
	//int startVertex = 0;
	int grouping = 0;
	int roundThrough = 0;
	while (getline(inFile, ManagerLine)) {
		istringstream iss;
		iss.str(ManagerLine);
		if (grouping == 0) {
			grouping++;
			iss >> numberOfRouters;
		}
		else if (grouping == 1) {
			bool moveOn = false;
			vector<int> routerConnection;
			for (int i = 0; i < 3; i++) {
				int nextNum;
				iss >> nextNum;
				if (nextNum == -1) {
					cout << "Breaking -1" << endl;
					grouping++;
					moveOn = true;
					break;
				}
				routerConnection.push_back(nextNum);
			}
			if(!moveOn){
				cout << "Router Connection: " << routerConnection[0] << " " << routerConnection[1] << " " << routerConnection[2] << endl;
				routingTable.push_back(routerConnection);
			}
		}
		else if(grouping == 2){
			vector<int> command;
			bool moveOn = false;
			for (int i = 0; i < 2; i++) {
				int RouterNum;
				iss >> RouterNum;
				if (RouterNum == -1) {
					cout << "Breaking -1" << endl;
					grouping++;
					moveOn = true;
					break;
				}
				cout << "Routernum " << RouterNum << endl;
				command.push_back(RouterNum);
			}
			if(!moveOn){
				cout << "Command: " << command[0] << " " << command[1] << endl;
				routingCommands.push_back(command);
			}
		}
		roundThrough++;
		cout << "roundThrough: " << roundThrough << endl;
	}
	cout << "done" << endl;
}

void sendMsg(int the_fd, long current){
		char buff[sizeof(long)];
		string check;
		char* toSend;
		toSend = (char*)malloc(sizeof(long));
		memcpy(toSend, &current, 4);
		if(send(the_fd, toSend, strlen(buff), 0) == -1){
			cerr << "send error" << endl;
			exit(1);
		}

}


uint32_t receivePortNum(int the_fd){
	char* buff;
	uint32_t packageSize = sizeof(uint32_t)+sizeof(uint32_t);
	buff = (char*)malloc(packageSize+1);
	if((recv(the_fd, buff, packageSize+1, 0)) == -1){
		cerr << "recv error" << endl;
		exit(1);
	}
	buff[packageSize] = 0;
	return *((uint32_t*)(buff+sizeof(uint32_t)));
}

char receiveCommand(int the_fd){
	char* buff;
	long packageSize = sizeof(char)+sizeof(char);
	buff = (char*)malloc(packageSize+1);
	if((recv(the_fd, buff, packageSize+1, 0)) == -1){
		cerr << "recv error" << endl;
		exit(1);
	}
	buff[packageSize] = 0;
	return *(buff+sizeof(char));
}



void *get_in_addr(struct sockaddr* sa){
	if(sa -> sa_family == AF_INET){
		return &(((struct sockaddr_in*)sa) -> sin_addr);
	}

	return &(((struct sockaddr_in6*)sa) -> sin6_addr);
}

int runServer(){
	char hostName[128];

	gethostname(hostName, sizeof hostName);
	hostent* hostInfo = gethostbyname(hostName);
	in_addr* hostIP = (in_addr*)hostInfo -> h_addr;
	cout << "Here is host IP " << hostIP <<endl;

	string res = inet_ntoa(*hostIP);

	struct addrinfo hints, *serverInfo, *p;
	//int sockfd, new_fd;
	struct sockaddr_storage their_addr;
	socklen_t sin_size;
	int yes = 1;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int retVal;
	if((retVal = getaddrinfo(NULL, PORT, &hints, &serverInfo)) != 0){
		cout << "getaddrinfo: " << stderr << gai_strerror(retVal);
		return -1;
	}

	for(p=serverInfo; p!=NULL; p=p->ai_next){
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			cerr << "server: socket" << endl;
			continue;
		}
		if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
			cerr << "server: bind" << endl;
			continue;
		}
		if(::bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
			close(sockfd);
			cerr << "server: bind" << endl;
			continue;
		}

		break;
	}

	freeaddrinfo(serverInfo);

	if(p == NULL){
		cout << "Server: failed to bind" << endl;
		exit(1);
	}

	if(listen(sockfd, 15) == -1){
		cerr << "listen" << endl;
		exit(1);
	}

	int count = 0;
	while(count < numberOfRouters){
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr*)&their_addr, &sin_size);
		if(new_fd == -1){
			continue;
		}

		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr*)&their_addr), s, sizeof s);
		//cout << "Accepted connection " << count << endl;

		sendMsg(new_fd, count);

		routerInfo currentRouter;
		currentRouter.routerID = (long)count;
		currentRouter.sockfd = (long)new_fd;
		uint32_t newPort = receivePortNum(new_fd);
		cout << "The UDP portNum for " << currentRouter.routerID << " is " << newPort<< endl;


		//~~~~From here it begins to just test the sending and receiving function
		for (int i=0;i<4;i++){
			 char c =receiveCommand(new_fd);
			 cout << "The manager receive " << c <<endl;
		}

		//~~~~From here it  ends to just test the sending and receiving function
		routers.push_back(currentRouter);
		count++;
	}



	return 0;
}

void * waitForConnections(void* a){
	runServer();

	return NULL;
}

int main(int argc, char *argv[]){
	if (argc == 2) {
		ifstream inFile(argv[1]);
		readFile(inFile);

		pthread_t t1;
		pthread_create(&t1, NULL, &waitForConnections, NULL);

		for(int i=0; i<numberOfRouters; i++){
			int status;
			pid_t childPID = fork();
			if(childPID >= 0){
				// If childPID is 0, the fork succeeded. Execute the child program
				if(childPID == 0){
					cout << "calling router" << endl;
					status = execv("router", argv);
				} else{
					// If childPID is not 0, wait for it to finish, then print the exit status
					wait(&status);
				}
			} else{
				// Error, failed to fork
				cerr << "Failed to fork()";
				exit(0);
			}
		}

		pthread_join(t1, NULL);
	}
	else {
		cerr << "Wrong args, need 2 total" << endl;
	}

	return 0;







	/*if(argc != 2){
		cout << "Put the right arguments, dumbass" << endl;
		return -1;
	}
	
	int test = atoi(argv[1]);
	numRouters = test;
	cout << "Starting Threads" << endl;
	
	pthread_t t1;
	string s = to_string(test);
	const char* temp = s.c_str();
	pthread_create(&t1, NULL, &waitForConnections, (void*)temp);
	
	for(int i=0; i<test; i++){
		int status;
		pid_t childPID = fork();
		if(childPID >= 0){
			// If childPID is 0, the fork succeeded. Execute the child program
			if(childPID == 0){
				status = execlp("router", NULL);
			} else{
				// If childPID is not 0, wait for it to finish, then print the exit status
				wait(&status);
			}
		} else{
			// Error, failed to fork
			cerr << "Failed to fork()";
			exit(0);
		}
	}
	
	pthread_join(t1, NULL);
	
	return 0;*/
}
