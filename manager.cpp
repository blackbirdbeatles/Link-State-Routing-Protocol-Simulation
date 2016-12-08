#include "project3.h"

vector<vector<int>> connectionTable;
vector<vector<int>> routingCommands;
int numberOfRouters;
const char* ip;
vector<routerInfo> routers;
int sockfd;
ofstream outfile;
void sigHandler(int signum){

	outfile << "Interrupt signal (" << signum << ") received.\n";
	for (int i = 0; i < numberOfRouters;i++) {
		close(routers[i].sockfd);
		exit(signum);
	}
	close(sockfd);
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
					outfile << "Breaking -1" << endl;
					grouping++;
					moveOn = true;
					break;
				}
				routerConnection.push_back(nextNum);
			}
			if(!moveOn){
				outfile << "Router Connection: " << routerConnection[0] << " " << routerConnection[1] << " " << routerConnection[2] << endl;
				connectionTable.push_back(routerConnection);
			}
		}
		else if(grouping == 2){
			vector<int> command;
			bool moveOn = false;
			for (int i = 0; i < 2; i++) {
				int RouterNum;
				iss >> RouterNum;
				if (RouterNum == -1) {
					outfile << "Breaking -1" << endl;
					grouping++;
					moveOn = true;
					break;
				}
				command.push_back(RouterNum);
			}
			if(!moveOn){
				routingCommands.push_back(command);
			}
		}
		roundThrough++;
		outfile << "roundThrough: " << roundThrough << endl;
	}
	outfile << "read file done" << endl;
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

void sendCommand(int the_fd, char c){
	uint16_t sizeOfPacket = sizeof(uint16_t)+ sizeof(char);
	//char* toSend;
	//toSend = (char*)malloc(sizeOfPacket + 1);
	char toSend[4];
	memcpy(toSend, &sizeOfPacket,2);
	memcpy(toSend + 2, &c, 1);
	toSend[sizeOfPacket] = 0;
	outfile<<"send command "<<*(toSend+2)<<endl;
	if(send(the_fd, toSend, sizeOfPacket+1, 0) == -1){
		cerr << "send error" << endl;
		exit(-1);
	}
}


char receiveCommand(int the_fd){
	char* buff;
	long packageSize = sizeof(uint16_t)+sizeof(char);
	buff = (char*)malloc(packageSize+1);
	if((recv(the_fd, buff, packageSize+1, 0)) == -1){
		cerr << "recv error" << endl;
		exit(1);
	}
	buff[packageSize] = 0;

	outfile << "In ReceiveCommand: " << *(buff+sizeof(uint16_t)) <<endl;
	return *(buff+sizeof(uint16_t));
}

void sendTest(int the_fd, int source, int dest){
	//char* toSend;
	//toSend = (char*)malloc(sizeOfPacket + 1);
	char flag='T';
	char toSend[9];
	memcpy(toSend, &flag, sizeof(char));
	memcpy(toSend + sizeof(char), &source, sizeof(int));
	memcpy(toSend + sizeof(char)+sizeof(int), &dest, sizeof(int));
	outfile << "Send Test(flag, source, dest): " << *(char*)toSend << " "<<*(int*)(toSend+1) << " "<<*(int*)(toSend+5) <<endl;
	if(send(the_fd, toSend, 9, 0) == -1){
		cerr << "send error" << endl;
		exit(-1);
	}
}


void buildNeighborTableSet(vector<vector<int>>& connectionTable, int RouterNum, vector<vector<vector<int>>> & neighborTableSet, vector<routerInfo>& routers){

	//Go through the whole connectionTable read from input file, build the direct neighbor Table of each node

	for (int i=0; i <connectionTable.size(); i++){
		vector<int> edge = connectionTable[i];
		edge.push_back((routers[edge[1]]).portUDP);
		(neighborTableSet[connectionTable[i][0]]).push_back(edge);
		vector<int> reverseEdge = connectionTable[i];
		int t;
		t = reverseEdge[0];
		reverseEdge[0] = reverseEdge[1];
		reverseEdge[1] = t;
		reverseEdge.push_back((routers[reverseEdge[1]]).portUDP);
		(neighborTableSet[connectionTable[i][1]]).push_back(reverseEdge);
	}
}




//packet format: packetSIze + ID + numberOfRouters + s1 d1 c1 p1 + s2 d2 c2 p2 + s3 d3 c3 p3 + ...

int sendIDAndConnectionTable(int fdTCP, int32_t ID, vector<vector<int>> neighborTable){

	char toSend[513];
	outfile << "Sending NodeID and Connection table to Router "<<ID<<endl;
	//dataLength and NodeAddr
	int neighborNum = neighborTable.size();

	uint16_t packetSize = sizeof(uint16_t)+ sizeof(int32_t)+ sizeof(int)+neighborNum * (4 * sizeof(int32_t));
	memcpy(toSend,&packetSize, sizeof(uint16_t));
	memcpy(toSend+ sizeof(uint16_t),&ID, sizeof(int32_t));
	memcpy(toSend+ sizeof(uint16_t)+ sizeof(int32_t), &numberOfRouters, sizeof(int));
	outfile << "packSize: "<<packetSize<<"  ID is: "<<ID<<endl<<"     numberOfRouters: "<<numberOfRouters<<endl;

	// neighborNum groups of "neighbor ,cost, port"
	uint16_t offset = sizeof(int16_t)+ sizeof(int32_t)+ sizeof(int);
	for (int i = 0; i <neighborNum; i++){
		int32_t source = neighborTable[i][0];
		int32_t dest = neighborTable[i][1];
		int32_t cost = neighborTable[i][2];
		int32_t port = neighborTable[i][3];
		outfile <<"s,d,c,p is: " <<source<<" "<<dest<<" "<<cost<<" "<<port<<endl;
		memcpy(toSend+offset, &source, sizeof(int32_t) );
		memcpy(toSend+offset+ sizeof(int32_t), &dest, sizeof(int32_t) );
		memcpy(toSend+offset+ sizeof(int32_t)*2, &cost, sizeof(int32_t) );
		memcpy(toSend+offset+ sizeof(int32_t)*3, &port, sizeof(int32_t) );
		offset += sizeof(int32_t)*4;
	}
	toSend[packetSize]=0;
	if (send(fdTCP, toSend, 512+1, 0)==-1)
	{
		cerr << "TCP send table fail"<<endl;
		exit(1);
	}
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

	string res = inet_ntoa(*hostIP);

	struct addrinfo hints, *serverInfo, *p;
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
		outfile << "getaddrinfo: " << stderr << gai_strerror(retVal);
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
		outfile << "Server: failed to bind" << endl;
		exit(1);
	}

	if(listen(sockfd, 15) == -1){
		cerr << "listen" << endl;
		exit(1);
	}

	int count = 0;
	//collect the port number
	while(count < numberOfRouters) {
		int new_fd;
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
		routerInfo currentRouter;
		currentRouter.sockfd = new_fd;
		if (new_fd == -1) {
			continue;
		}

		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *) &their_addr), s, sizeof s);
		//outfile << "Accepted connection " << count << endl;
		int nodeID = count;
		uint32_t nodePort = receivePortNum(new_fd);
		currentRouter.portUDP = nodePort;
		currentRouter.routerID = nodeID;
		outfile << "The UDP portNum for " << nodeID << " is " << nodePort << endl;

		routers.push_back(currentRouter);
		count++;

	}

	//test of vector: routers
	outfile<< "The routers already got: "<<endl;
	for (int i = 0; i < numberOfRouters; i++){
		outfile << "ID: " << (routers[i]).routerID << " portUDP: " << (routers[i]).portUDP<< " TCP fd: " << (routers[i]).sockfd<<endl;
	}




			//initialize the neighborTableSet

		vector<vector<vector<int>>> neighborTableSet;
		vector<int> tmp;
		vector<vector<int> > tmp2;
		for (int i = 0; i < numberOfRouters; i++)
			neighborTableSet.push_back(tmp2);

			//build neighbor table and print it out to see
		buildNeighborTableSet(connectionTable, numberOfRouters, neighborTableSet,routers);
		for (int i = 0; i<numberOfRouters; i++){
			outfile << "neighborTableSet for R " << i <<":"<<endl;
			for (int j = 0; j < neighborTableSet[i].size(); j++){
				outfile <<"    "<< neighborTableSet[i][j][0]<< " "<<neighborTableSet[i][j][1]<< " "<<neighborTableSet[i][j][2]<<endl;
			}
		}



		for (int i = 0; i < numberOfRouters; i++) {
			int fdTCP = (routers[i]).sockfd;
			int32_t nodeID = (routers[i]).routerID;
			sendIDAndConnectionTable(fdTCP, nodeID, neighborTableSet[nodeID]);
			char c = receiveCommand(fdTCP);
			if (c == 'R') {
				outfile << "Got READY from router " << nodeID << endl;
				sendCommand(fdTCP, 'S');
			}
		}

		for (int i = 0; i < numberOfRouters; i++) {
			int fdTCP = (routers[i]).sockfd;
			int32_t nodeID = (routers[i]).routerID;
			int32_t portUDP = (routers[i]).portUDP;
			char c = receiveCommand(fdTCP);
			if (c == 'C') {
				outfile << "Got COMPLETE from router " << nodeID << endl;
				if (i==numberOfRouters-1){
					for (int j = 0; j <numberOfRouters;j++){
						sendCommand(routers[j].sockfd, 'U');
					}
				}
			}

		}
		for (int i=0; i < routingCommands.size();i++){
			sendTest(routers[routingCommands[i][0]].sockfd, routingCommands[i][0],routingCommands[i][1]);
//			outfile << "Sent test from " << routingCommands[i][0] << " to " << routingCommands[i][1] << endl;
			usleep(500000);
		}
		for (int i=0; i < routers.size(); i++){
			char c = 'E';
			send(routers[i].sockfd,&c,1,0 );
		}
		outfile << "Sent quit messages to routers. Manager quitting." << endl;
		usleep(500000);
		cout << "Sent quit messages to routers. Manager quitting." << endl;





	return 0;
}

void * waitForConnections(void* a){
	runServer();

	return NULL;
}

int main(int argc, char *argv[]){
	if (argc == 2) {
		outfile.open("manager.out");
		outfile<<"Manager output."<<endl;

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
					outfile << "calling router" << endl;
					status = execv("router", argv);
					exit(0);
				} else{
					// If childPID is not 0, wait for it to finish, then print the exit status
				//	wait(&status);
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
	outfile.close();
	return 0;


	/*if(argc != 2){
		outfile << "Put the right arguments, dumbass" << endl;
		return -1;
	}
	
	int test = atoi(argv[1]);
	numRouters = test;
	outfile << "Starting Threads" << endl;
	
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
