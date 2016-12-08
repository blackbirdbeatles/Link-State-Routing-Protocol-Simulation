#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>

#include <cstring>
#include <map>
#include <pthread.h>
#include <project3.h>



using namespace std;



struct ArPth{
	int32_t id;
	int32_t sock;
};

ofstream outfile;
int numberOfRouters;
vector<int> checkACK, checkOtherID;
map<int, int> nodeToPort;
vector<int> checkSource;


vector<vector<int>> buildSPT(const vector<vector<int>>& neighborTable, vector<vector<int>> connectionTable ) {
	//SPT will have the shortest path from A to B via C as <A, B, C>
	outfile << "Constructing Shortest Path Tree..." << endl;
	vector<vector<int>> availablePaths = neighborTable;
	vector<vector<int>> SPT;
	outfile << "Initial number of paths to choose from: " << availablePaths.size() << endl;
	while (availablePaths.size() != 0) {
		int chosenPath = -1;
		int smallestLength = -1;
		vector<int> shortestPath;
		//find the shortest available path
		outfile << "Choosing shortest path..." << endl;
		for (unsigned int i = 0; i < availablePaths.size(); i++) {
			int Length = availablePaths[i][2];
			if (Length < smallestLength || smallestLength == -1) {
				smallestLength = Length;
				chosenPath = i;
			}
		}
		outfile << "Successful!  Path is <" << availablePaths[chosenPath][0] << ", " << availablePaths[chosenPath][1] << ", " << availablePaths[chosenPath][2] << ">"
		<< endl;
		//check what the path must be going toward and via and grab length of path
		int goingToB;
		int goingViaC, originalC;
		int totalLengthD = availablePaths[chosenPath][2];
		//if a route contains A then A is the via route; otherwise, figure out which is which
		if (availablePaths[chosenPath][0] == routerID) {
			goingViaC = routerID;
			goingToB = availablePaths[chosenPath][1];
		} else if (availablePaths[chosenPath][1] == routerID) {
			goingViaC = routerID;
			goingToB = availablePaths[chosenPath][0];
		} else {
			for (unsigned int i = 0; i < SPT.size(); i++) {
				//the already taken value must be the new via
				if (SPT[i][0] == availablePaths[chosenPath][0]) {
					//2 is the length from C to B
					goingViaC = availablePaths[chosenPath][0];
					goingToB = availablePaths[chosenPath][1];
					break;
				} else {
					goingViaC = availablePaths[chosenPath][1];
					goingToB = availablePaths[chosenPath][0];
				}
			}
		}
		//correct C to be the first node in the path to B and theoretically totalLength to the total instead of just new
		outfile << "Finding via route for root to dest..." << endl;
		for (unsigned int i = 0; i < SPT.size(); i++) {
			if ((goingViaC == SPT[i][0]) && SPT[i][1] != routerID) {
				goingViaC = SPT[i][1];

				//totalLengthD = totalLengthD + SPT[i][2];
				break;
			}
		}
		if(goingViaC == routerID){
			goingViaC = goingToB;
		}
		outfile << "Found! To reach " << goingToB << " send to " << goingViaC << "." << endl;
		outfile << "Total cost to reach " << goingToB << " is " << totalLengthD << "." << endl;
		//create the shortest path node
		shortestPath.push_back(goingToB);
		shortestPath.push_back(goingViaC);
		shortestPath.push_back(totalLengthD);
		SPT.push_back(shortestPath);
		//grab the adjacency list for B and adds it to available paths, unless B and its path are already in there
		outfile << "Finding new available paths..." << endl;
		for (unsigned int i = 0; i < connectionTable.size(); i++) {
			bool containsA = false;
			bool containsB = false;
			bool containsC = false;
			//outfile << "Add " << connectionTable[i][0] << " " << connectionTable[i][1] << " " << connectionTable[i][2] << endl;
			for (unsigned int j = 0; j < SPT.size(); j++) {
				if (SPT[j][0] == connectionTable[i][0] || routerID == connectionTable[i][0])
					containsA = true;
				if (SPT[j][0] == connectionTable[i][1] || routerID == connectionTable[i][1])
					containsB = true;
			}
			if (containsA && containsB) {
				containsC = true;
			}

			if ((connectionTable[i][0] == goingToB || connectionTable[i][1] == goingToB) && !containsC) {
				//outfile << "Yes" << endl;
				connectionTable[i][2] = connectionTable[i][2] + shortestPath[2];
				availablePaths.push_back(connectionTable[i]);
			} else {
				//outfile << "No" << endl;
			}
		}
		//remove all available paths that are already reached by A in the SPT
		vector<vector<int>> newPaths;
		outfile << "Removing redundancies..." << endl;
		for (unsigned int j = 0; j < availablePaths.size(); j++) {
			bool containsB = false;
			bool containsA = false;
			bool containsSPTB1 = false;
			bool containsSPTB2 = false;
			if (availablePaths[j][0] == goingToB || availablePaths[j][1] == goingToB) {
				containsB = true;
			}
			if (availablePaths[j][0] == routerID || availablePaths[j][1] == routerID) {
				containsA = true;
			}
			for (unsigned int i = 0; i < SPT.size(); i++) {
				//we know what goingToB is and that that's now in the SPT, so if we check all the SPT Bs against 0 and 1
				//using goingToB as the other value, we know if we've already reached both nodes in the connection and can remove it
				//must also remove paths that contain both A and goingToB or A and SPT Bs
				int SPTB = SPT[i][0];
				if ((availablePaths[j][0] == SPTB || availablePaths[j][1] == SPTB) && containsSPTB1 == false) {
					containsSPTB1 = true;
					//outfile << "Erasing: " << availablePaths[j][0] << " " << availablePaths[j][1] << " " << availablePaths[j][2] << endl;
					//availablePaths.erase(availablePaths.begin() + j);
				} else if ((availablePaths[j][0] == SPTB || availablePaths[j][1] == SPTB) &&
						   containsSPTB1 == true) {
					containsSPTB2 = true;
				}
			}
			//add the path to new paths if either of its paths is not in the SPT
			//so if containsSPTB2 is false and if containsA or containsSPTB1 is false
			if (!containsSPTB2 && (!containsA || !containsSPTB1)) {
				newPaths.push_back(availablePaths[j]);
			}
		}
		availablePaths = newPaths;
		outfile << "Done! Number of paths to choose from: " << availablePaths.size() << endl;
	}
	outfile << "Shortest Path Tree construction complete. <Destination, Via, Distance> paths:" << endl;
	for (unsigned int i = 0; i < SPT.size(); i++) {
		outfile << "<" << SPT[i][0] << ", " << SPT[i][1] << ", " << SPT[i][2] << "> ";
	}
	outfile << endl;
	return SPT;
}



void buildNodeToPort(map<int, int>& nodeToPort,vector<vector<int>>& neighborTable ){
	for (int i = 0; i < neighborTable.size(); i++) {
		nodeToPort[neighborTable[i][1]] = neighborTable[i][3];
	}
}


void sendPortNum(uint32_t the_fd, uint32_t port){
	uint32_t sizeOfPacket = sizeof(uint32_t)+ sizeof(uint32_t);
	//char* toSend;
	//toSend = (char*)malloc(sizeOfPacket + 1);
	char toSend[9];
	memcpy(toSend, &sizeOfPacket,sizeof(uint32_t));
	memcpy(toSend + sizeof(uint32_t), &port, sizeof(uint32_t));
	toSend[sizeOfPacket] = 0;
	if(send(the_fd, toSend, sizeOfPacket+1, 0) == -1){
		cerr << "send error" << endl;
		exit(-1);
	}
}

void sendCommand(int the_fd, char c){
	uint16_t sizeOfPacket = sizeof(uint16_t)+ sizeof(char);
	//char* toSend;
	//toSend = (char*)malloc(sizeOfPacket + 1);
	char toSend[4];
	memcpy(toSend, &sizeOfPacket,2);
	memcpy(toSend + 2, &c, 1);
	toSend[sizeOfPacket] = 0;
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
	return *(buff+sizeof(uint16_t));
}


void receiveTest(int the_fd, uint16_t& source, uint16_t& dest){
	char* buff;
	uint16_t packageSize = sizeof(uint16_t)+sizeof(uint16_t)+sizeof(uint16_t);
	buff = (char*)malloc(packageSize+1);
	if((recv(the_fd, buff, packageSize+1, 0)) == -1){
		cerr << "receiveTest error and s and d is: "<< source<<" "<< dest<<endl;
		exit(1);
	}
	buff[packageSize] = 0;

	//Give the source and dest
	memcpy(&source,buff+ sizeof(uint16_t), sizeof(uint16_t));
	memcpy(&dest, buff+sizeof(uint16_t)*2, sizeof(uint16_t));
	outfile << "The test received from manager is   source: "<< source<<"   dest:  "<<dest<<endl;

}

int receiveIDAndConnectionTable(int fdTCP, int &ID, vector<vector<int>>& neighborTable){

	char* buff;
	int bufferLen = 512;
	buff = (char*)malloc(bufferLen+1);
	if((recv(fdTCP, buff, bufferLen+1, 0)) == -1){
		cerr << " receiveIDAndConnectionTable error" << endl;
		exit(1);
	}
	uint16_t packetSize;
	memcpy(&packetSize,buff, sizeof(uint16_t));
	int neighborNum = (packetSize - (sizeof(uint16_t)+ sizeof(int32_t)+ sizeof(int)))/(4 * sizeof(int32_t));

	//dataLength and NodeAddr
	memcpy(&ID, buff + sizeof(uint16_t), sizeof(int32_t));
	memcpy(&numberOfRouters,buff + sizeof(uint16_t)+ sizeof(int32_t), sizeof(int));

	// extract neighborNum groups of "neighbor ,cost, port"
	uint16_t offset = sizeof(uint16_t)+ sizeof(int32_t)+ sizeof(int);
	for (int i = 0; i <neighborNum; i++){
		int32_t source, dest, cost, port;
		memcpy(&source,buff+offset, sizeof(int32_t) );
		memcpy(&dest,buff+offset+ sizeof(int32_t),  sizeof(int32_t) );
		memcpy(&cost,buff+offset+ sizeof(int32_t)*2,  sizeof(int32_t) );
		memcpy(&port,buff+offset+ sizeof(int32_t)*3,sizeof(int32_t) );
		offset += sizeof(int32_t)*4;
		vector<int> tempt;
		neighborTable.push_back(tempt);
		neighborTable.at(i).push_back(source);
		neighborTable.at(i).push_back(dest);
		neighborTable.at(i).push_back(cost);
		neighborTable.at(i).push_back(port);
	}
}






ResultUDPCreation createUDPConnection(){
	struct sockaddr_in addrinfo;
	socklen_t addrlen = sizeof(addrinfo);
	int sockfd;
	int portNum;
	while(1){
		if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
			cerr << "server: socket" << endl;
			continue;
		}

		memset((char *)&addrinfo, 0, sizeof(addrinfo));
		addrinfo.sin_family = AF_INET;
		addrinfo.sin_addr.s_addr = htons(INADDR_ANY);


		if (::bind(sockfd, (struct sockaddr *)&addrinfo, sizeof(addrinfo))==-1) {
			cerr << "bind failed" << endl;
			continue;
		}

		getsockname(sockfd, (struct sockaddr *)&addrinfo, &addrlen);
		portNum = ntohs(addrinfo.sin_port);
		outfile << "Router side UDP port num: " << portNum << endl;
		break;
	}

	ResultUDPCreation result;
	result.port = portNum;
	result.fd = sockfd;
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

		outfile << "Connecting... ";
		break;
	}

	if(p == NULL){
		cerr << "client: failed to connect" << endl;
		return -1;
	}

	outfile << "Connected!" << endl;


	freeaddrinfo(serverInfo);

	return sockfd;
}


void checkA(vector<int>& l){
	int sum = 0;
	for (int i = 0; i < checkACK.size(); i++){
		if (checkACK[i]==0)
			l.push_back(i);
	}
}

void checkOther(vector<int>& l){
	int sum = 0;
	for (int i = 0; i < checkOtherID.size(); i++){
		if (checkOtherID[i]==0)
			l.push_back(i);
	}
}

void checkS(vector<int>& l){
	int sum = 0;
	for (int i = 0; i < checkSource.size(); i++){
		if (checkSource[i]==0)
			l.push_back(i);
	}
}



int sendToOneUDPCommand(int fdUDP, int32_t destNode, char flag, int32_t myID){

	//sleep to avoid congestion
	usleep(myID*1000);

	//Here is to define si_other
	int bufflen = 64;
	struct sockaddr_in si_other;
	socklen_t slen = sizeof(si_other);
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(nodeToPort[destNode]);
//	cout << "PORT: " << nodeToPort[destNode] << endl;
	if (inet_aton("127.0,0,1", &si_other.sin_addr)!=0) {
		cerr<<"inet_aton() failed\n";
		exit(1);
	}
	char* buff;
	buff = (char*) malloc(bufflen);
	memcpy(buff,&flag, sizeof(char));
	memcpy(buff+ sizeof(char), &myID, sizeof(int32_t));
//	cout << "Readytosend" << endl;
	if (sendto(fdUDP, buff, bufflen, 0, (struct sockaddr*)&si_other, slen)==-1)
	{
		cerr << "UDP command send fail"<<endl;
		perror("UDP SEND FAIL");

		cerr<<"Current Router: "<<myID <<"  "<<destNode<<endl;
		for (map<int,int>::iterator it=nodeToPort.begin(); it!=nodeToPort.end(); ++it)
			cout<< it->first << " => " << it->second << '\n';
		exit(1);
	}

}



int sendBuffTo(int fdUDP,  int destNode, char* buff,int NodeID){

//	cout << "A" << endl;
	//sleep to avoid congestion
	usleep(NodeID*1000);

	//Here is to define si_other
	int bufflen = 512;
	struct sockaddr_in si_other;
	socklen_t slen = sizeof(si_other);
	memset((char *) &si_other, 0, sizeof(si_other));
//	cout << "A" << endl;
//	outfile<<"  1   "<<endl;
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(nodeToPort[destNode]);
	if (inet_aton("127.0,0,1", &si_other.sin_addr)!=0) {
		cerr<<"inet_aton() failed\n";
		exit(1);
	}
//	outfile<<"  2   "<<endl;


	memcpy(buff+ sizeof(uint16_t)+ sizeof(int32_t), &NodeID, sizeof(int32_t)); //From who
//	outfile<<"  3  "<<endl;
	if (sendto(fdUDP, buff, bufflen, 0, (struct sockaddr*)&si_other, slen)==-1)
	{
		cerr << "UDP forwarding send fail"<<endl;
		exit(1);
	}
//	outfile<<"  4   "<<endl;
	return 0;
}

int sendToOneUDPTable(int fdUDP, int destNode, vector<vector<int>>& neighborTable, map<int,int>& nodeToPort, int NodeID, int fromID){
	//sleep to avoid congestion
	sleep(NodeID/10);

	//Here is to define si_other
	int bufflen = 512;
	struct sockaddr_in si_other;
	socklen_t slen = sizeof(si_other);
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(nodeToPort[destNode]);
	if (inet_aton("127.0,0,1", &si_other.sin_addr)!=0) {
		cerr<<"inet_aton() failed\n";
		exit(1);
	}

	char* buff;

	//dataLength and NodeAddr
	int neighborNum = neighborTable.size();
	uint16_t packetSize = sizeof(uint16_t)+sizeof(int32_t)+sizeof(int32_t)+ neighborNum * (4 * sizeof(int32_t));
	buff = (char*) malloc(bufflen);
	memcpy(buff,&packetSize, sizeof(uint16_t));
	memcpy(buff+ sizeof(uint16_t), &NodeID, sizeof(int32_t));
	memcpy(buff+ sizeof(uint16_t)+ sizeof(int32_t), &fromID, sizeof(int32_t)); //From who
	// neighborNum groups of "neighbor ,cost, port"
	uint16_t offset = sizeof(uint16_t)+ sizeof(int32_t)*2;
	for (int i = 0; i <neighborNum; i++){
		int32_t source = neighborTable[i][0];
		int32_t dest = neighborTable[i][1];
		int32_t cost = neighborTable[i][2];
		int32_t port = neighborTable[i][3];
		memcpy(buff+offset, &source, sizeof(int32_t) );
		memcpy(buff+offset+ sizeof(int32_t), &dest, sizeof(int32_t) );
		memcpy(buff+offset+ sizeof(int32_t)*2, &cost, sizeof(int32_t) );
		memcpy(buff+offset+ sizeof(int32_t)*3, &port, sizeof(int32_t) );
		offset += sizeof(int32_t)*4;

	}
	buff[packetSize]=0;


	if (sendto(fdUDP, buff, bufflen, 0, (struct sockaddr*)&si_other, slen)==-1)
	{
		cerr << "UDP send fail"<<endl;
		exit(1);
	}
	return 0;
}


char* ReceiveFromOneUDPTable(int fdUDP, vector<vector<int>>& connectionTable, int32_t& fromID, int& r){
	outfile << "Here comes to receive an UDP packet from a direct neighbor" <<endl;
	//Here is to define si_other
	uint16_t packetSize;
	int sourceID;
	int bufflen = 512;
	struct sockaddr_in si_other;
	socklen_t fromlen;
	char* buff;
	buff = (char*) malloc(bufflen+1);
	recvfrom(fdUDP, buff, bufflen, 0, (struct sockaddr*)&si_other, &fromlen);
	//
	memcpy(&packetSize, buff, sizeof(uint16_t));
	memcpy(&sourceID, buff+ sizeof(uint16_t), sizeof(int32_t));
	memcpy(&fromID, buff+sizeof(uint16_t)+ sizeof(int32_t), sizeof(int32_t));
	if (checkSource[sourceID] == 1) {
		r = -1;
		return buff;
	}
	else
		checkSource[sourceID] = 1;

	outfile << "packet Size is: "<< packetSize <<endl;
	outfile << "From ID is: "<< fromID <<endl;
	outfile << "Source ID: " << sourceID << endl;
	int neighborNum = (packetSize - (sizeof(uint16_t)+ sizeof(int32_t)+ sizeof(int32_t)))/(4 * sizeof(int32_t));
	outfile << "neighborNum is " << neighborNum <<endl;


	// extract neighborNum groups of "neighbor ,cost, port"
	uint16_t offset = sizeof(uint16_t)+ sizeof(int32_t)+ sizeof(int32_t);
	for (int i = 0; i <neighborNum; i++){
		int32_t source, dest, cost, port;
		memcpy(&source,buff+offset, sizeof(int32_t) );
		memcpy(&dest,buff+offset+ sizeof(int32_t),  sizeof(int32_t) );
		memcpy(&cost,buff+offset+ sizeof(int32_t)*2,  sizeof(int32_t) );
		memcpy(&port,buff+offset+ sizeof(int32_t)*3,sizeof(int32_t) );
		offset += sizeof(int32_t)*4;
		vector<int> tempt;
		tempt.push_back(source);
		tempt.push_back(dest);
		tempt.push_back(cost);
		tempt.push_back(port);
		connectionTable.push_back(tempt);
	}
	r=0;
	return buff;
}









int receiveFromAllNeighbors(int fdUDP){

};



void LSP(vector<vector<int> >& neighborTable,vector<vector<int> >& connectionTable, map<int,int>& nodeToPort ){

}






void breakTheMessageReceived(string message,int& NodeAddr,vector<vector<int> >& neighborTable, map<int,int>& nodeToPort){

}




void flowChartBuild(vector<vector<int>>& connectionTable, map<int, int>& flowChart){

}


/*
void *waitMsg(void* p){
	string message;
	int fdTCP = (*(Args*)p).arg1;
	int fdUDP = (*(Args*)p).arg2;
	int NodeID = fdUDP = (*(Args*)p).arg3;

	message = "";
	uint16_t source,dest;
	//其实这里需要一个模糊的消息接受
	receiveTest(fdTCP, source, dest);
	while (message != "-1"){
		sendToOneUDPTable();






		message = "";
		receiveTest(fdTCP, source, dest);
	}
	sendToOneUDPCommand(fdUDP, NodeID,'E');
}
*/

void *checkFullStatus(void* p){

	ArPth a = *(ArPth*)p;
	int myID = a.id;
	int fdUDP = a.sock;
	while (1){
		usleep(50000);
		vector<int> emptyListA;
		vector<int> emptyListI;
		checkA(emptyListA);
		checkOther(emptyListI);
		if (emptyListA.empty() && emptyListI.empty())
			break;
		else{
			for (int i=0; i <emptyListA.size(); i++)
			{
				sendToOneUDPCommand(fdUDP,emptyListA[i],'I',myID);

			}
		}
	}
}

int main() {


	sleep(1);

	//Hardcode area  #begin#


	//Hardcode area  #end#




	int fdTCP = connectToServer();
	ResultUDPCreation r;
	int fdUDP, portUDP;
	r = createUDPConnection();
	fdUDP = r.fd;
	portUDP = r.port;


	sendPortNum(fdTCP, portUDP);

	int NodeAddr;
	vector <vector<int>> connectionTable, neighborTable;
	receiveIDAndConnectionTable(fdTCP, NodeAddr, neighborTable);
	string filename = "Router" + to_string(NodeAddr) + ".out";
	outfile.open(filename);
	outfile << "Test of neighbor table of " << NodeAddr << endl;
	outfile << "SIZE:" << neighborTable.size() << endl;
	for (int i = 0; i < neighborTable.size(); i++) {
		outfile <<neighborTable[i][0] << "  " << neighborTable[i][1] << "  " << neighborTable[i][2] << "  " << neighborTable[i][3] << endl;
	}


	//PART OF(DIRECT) node to port table built and print out

	outfile << "NodeToPort table print out: " <<endl;
	buildNodeToPort(nodeToPort,neighborTable);
	for (map<int,int>::iterator it=nodeToPort.begin(); it!=nodeToPort.end(); ++it)
		outfile << it->first << " => " << it->second << '\n';



	map<int, int> flowChart;

	sendCommand(fdTCP, 'R');
	char c;
	c = receiveCommand(fdTCP);

	if (c == 'S') {                                                // MEANS:  It is safe to try to reach neighbors.

		//Here to do the ACK thing
		for (int i = 0; i < neighborTable.size();i++)
			sendToOneUDPCommand(fdUDP, neighborTable[i][1], 'I', NodeAddr);

		//build up the recvfrom thing
		uint16_t packetSize;
		int bufflen = 512;
		struct sockaddr_in si_other;
		socklen_t fromlen;
		char* buff;
		buff = (char*) malloc(bufflen+1);

		//initialize the checking ACK and checking ID tables to 0
		for (int i = 0; i<numberOfRouters;i++) {
			checkACK.push_back(1);
			checkOtherID.push_back(1);
		}
		for (int i = 0; i < neighborTable.size(); i++){
			checkACK[neighborTable[i][1]] = 0;
			checkOtherID[neighborTable[i][1]] = 0;
		}


		ArPth ar;
		ar.sock=fdUDP;
		ar.id = NodeAddr;
		pthread_t checkFull;
		pthread_create(&checkFull, NULL, &checkFullStatus, &ar);
		//begin to listen
		//packet explainanation:      												char + fromID
		vector<int> emptyA, emptyI;
		checkA(emptyA);
		checkOther(emptyI);
		while ((!emptyA.empty()) || (!emptyI.empty()) ){
			recvfrom(fdUDP, buff, bufflen, 0, (struct sockaddr*)&si_other, &fromlen);

			int32_t fromID;
			memcpy(&fromID, buff+1, sizeof(int32_t));

			switch (*buff){
				case 'A':
					checkACK[fromID] = 1;
					break;
				case 'I':
					checkOtherID[fromID]=1;
					sendToOneUDPCommand(fdUDP, fromID, 'A', NodeAddr);
					break;
			}
			emptyA.clear();
			emptyI.clear();
			checkA(emptyA);
			checkOther(emptyI);
		}
		//A thread to send extra packet to prevent packet loss to some extend
		pthread_join(checkFull,NULL);
		sendCommand(fdTCP, 'C');     //"Complete!"
		c = receiveCommand(fdTCP);
		if (c == 'U'){				//"Network is up now!"

			//initialize source neighbor whose table already get
			for (int i = 0; i<numberOfRouters;i++)
				checkSource.push_back(0);
			checkSource[NodeAddr]=1;



			//Perform the flooding algorithm
			for (int i = 0; i < neighborTable.size();i++)
				sendToOneUDPTable(fdUDP, neighborTable[i][1], neighborTable,nodeToPort,neighborTable[i][0],neighborTable[i][0]);
			vector<int> emptyS;
			checkS(emptyS);
			connectionTable = neighborTable;
			while(!emptyS.empty()){
				int32_t fromID;
				char* pbuff;
				int r;
				pbuff = ReceiveFromOneUDPTable(fdUDP, connectionTable, fromID,r);
				if (r==0) {
					//forwarding

					for (int j = 0; j < neighborTable.size(); j++) {
						if (neighborTable[j][1] != fromID) {
							sendBuffTo(fdUDP, neighborTable[j][1], pbuff, NodeAddr);
						}
					}

				}
				else
					//discard
					outfile << "Discard table from Router " << fromID <<endl;
				emptyS.clear();
				checkS(emptyS);
			}
			outfile<<"For router "<<NodeAddr<<", connection size = "<< connectionTable.size()<<endl;
			//print out the connectionTable
			for (int j = 0; j < connectionTable.size(); j++) {
				outfile << connectionTable[j][0] << "  " << connectionTable[j][1] << "  " <<
				connectionTable[j][2] << "  " << connectionTable[j][3] << endl;
			}
		}
		else
			cout << "unexpected message" <<endl;








/*









		/*
		sendCommand(fdTCP, 'C');
		c = receiveCommand(fdTCP);
		if (message == 'U'){
			LSP(neighborTable, connectionTable,nodeToPort);
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			//BuildSPT(connectionTable);
			flowChartBuild(connectionTable, flowChart);
			//Here we begin to test our flowChart
			Args A;
			A.arg1 = fdTCP;
			A.arg2 = fdUDP;
			A.arg3 = NodeAddr;
			pthread_t listenTCP;
			//~~~~~~
			pthread_create(&listenTCP, NULL, &waitMsg, &A);
			pthread_join(listenTCP, NULL);
			message = "";
			int sourceNode = -1;
			receiveFromOneUDP(fdUDP, sourceNode, message);
			while (message != 'E'){
				int source, dest;
				breakTestMessage(message, source, dest);
				if (dest == NodeAddr)
				{
					routerPrint("I'm destination. I've already got the packet.");
					sendToOneUDP(fdUDP,NodeAddr,'E');
				}
				else{
					routerPrint("I will forward this to " + to_string(flowChart[dest])+" now");
					sendToOneUDP(fdUDP,flowChart[dest],message);
				}
				message = "";
				receiveFromOneUDP(fdUDP, sourceNode, message);
			}
			return 0;
		};


	}
	else{
		cerr << "Error: The following message expected: safe!" <<endl;
		return -1;
	}

		outfile << "Yes! I receive the command S" << endl;
		outfile.close();
		return 0;


		//只是UDP send 的test
		/*sendToOneUDPTable(fdUDP, NodeAddr, neighborTable, nodeToPort, NodeAddr);
        ReceiveUDPTableFromOne(fdUDP, connectionTable);


        outfile <<"  12   " <<endl;
        for (int i = 0; i < connectionTable.size(); i++){
            for (int j =0; j <connectionTable[i].size();j++)
                outfile << connectionTable[i][j];
            outfile<<endl;
        }
        outfile <<"  13   " <<endl;
        */



		//~~~~~~~~~~~~~

	}

}



