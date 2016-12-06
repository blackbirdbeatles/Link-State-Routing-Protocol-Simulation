# include "project3.h"

int portNum;
int routerID;
int managerfd;

struct ResultUPDCreation{
	int port;
	int fd;
};

ResultUPDCreation myInfo;

void sendPortNum(int the_fd, long num){
	char buff[sizeof(long)];
	string check;
	char* toSend;
	toSend = (char*)malloc(sizeof(long));
	memcpy(toSend, &portNum, 4);
	if(send(the_fd, toSend, strlen(buff), 0) == -1){
		cerr << "send error" << endl;
		exit(1);
	}
}

void recieveID(int the_fd){
	char buff[2];

	if((recv(the_fd, buff, sizeof buff, 0)) == -1){
			cerr << "recv error" << endl;
			exit(1);
		}
		
		short ID;
		memcpy(&ID, buff, 2);
		routerID = ID;

		cout << "My ID: " << ID << endl;
}

ResultUPDCreation createUDPConnection(){
	struct sockaddr_in addrinfo;
	socklen_t addrlen = sizeof(addrinfo); 
	int sockfd;
	
	while(1){
		if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
			cerr << "server: socket" << endl;
			continue;
		}
		
		memset((char *)&addrinfo, 0, sizeof(addrinfo)); 
		addrinfo.sin_family = AF_INET; 
		addrinfo.sin_addr.s_addr = htons(INADDR_ANY); 


		if (bind(sockfd, (struct sockaddr *)&addrinfo, sizeof(addrinfo)) == -1) { 
			cerr << "bind failed" << endl;
			continue; 
		}

		getsockname(sockfd, (struct sockaddr *)&addrinfo, &addrlen);
		portNum = ntohs(addrinfo.sin_port);
		cout << "Router side UDP port num: " << portNum << endl;
		break;
	}
	
	ResultUPDCreation result;
	result.port = portNum;
	result.fd = sockfd;
	
	myInfo = result;
	
	return result;
}

int connectToServer(){
	struct addrinfo hints, *serverInfo, *p;
	int retVal;
	int sockfd;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if((retVal = getaddrinfo("127.0.0.1", PORT, &hints, &serverInfo)) != 0){
		cerr << "getaddrinfo: " << gai_strerror(retVal) << endl;
		return -1;
	}

	for(p=serverInfo; p!=NULL; p=p->ai_next){
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			cerr << "client: socket" << endl;
			continue;
		}

		if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
			close(sockfd);
			perror("client: connect");
			continue;
		}

		cout << "Connecting... ";
		break;
	}

	if(p == NULL){
		cerr << "client: failed to connect" << endl;
		return -1;
	}

	cout << "Connected!" << endl;
	
	managerfd = sockfd;

	freeaddrinfo(serverInfo);
	
	createUDPConnection();
	
	sendPortNum(sockfd, (long)portNum);
	
	recieveID(sockfd);

	return 0;
}


int main(int argc, char* argv[]){
	
	//int mynum = atoi(argv[1]);
	
	//cout << "I am child number " << 0 << endl;
	
	connectToServer();
	
	/*createUDPConnection();
	
	sendPortNum(managerfd, (long)portNum);

	recieveID(managerfd);*/
	
	return 0;
}
