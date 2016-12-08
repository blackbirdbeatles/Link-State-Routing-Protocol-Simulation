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


ofstream outfile;




/*

int buildSPT() {
	//SPT will have the shortest path from A to B via C as <A, B, C>
	vector<vector<int>> availablePaths = neighborTable;
	while (availablePaths.size() != 0) {
		int chosenPath = -1;
		int smallestLength = -1;
		vector<int> shortestPath;
		//find the shortest available path 
		for (unsigned int i = 0; i < availablePaths.size(); i++) {
			int Length = availablePaths[i][2];
			if (Length < smallestLength || smallestLength == -1) {
				smallestLength = Length;
				chosenPath = i;
			}
		}
		//check what the path must be going toward and via and grab length of path
		int goingToB;
		int goingViaC;
		int totalLengthD = availablePaths[chosenPath][2];
		//if a route contains A then A is the via route; otherwise, figure out which is which
		if (availablePaths[chosenPath][0] == routerID) {
			goingViaC = routerID;
			goingToB = availablePaths[chosenPath][1];
		}
		else if (availablePaths[chosenPath][1] == routerID) {
			goingViaC = routerID;
			goingToB = availablePaths[chosenPath][0];
		}
		else {
			for (unsigned int i = 0; i < SPT.size(); i++) {
				//the already taken value must be the new via
				if (SPT[i][0] == availablePaths[chosenPath][0]) {
					//2 is the length from C to B
					goingViaC = availablePaths[chosenPath][0];
					goingToB = availablePaths[chosenPath][1];
					break;
				}
				else {
					goingViaC = availablePaths[chosenPath][1];
					goingToB = availablePaths[chosenPath][0];
				}
			}
		}
		//correct C to be the first node in the path to B and theoretically totalLength to the total instead of just new
		for (unsigned int i = 0; i < SPT.size(); i++) {
			if (goingViaC == SPT[i][0]) {
				goingViaC = SPT[i][1];
				totalLengthD = totalLengthD + SPT[i][2];
				break;
			}
		}
		//create the shortest path node
		shortestPath.push_back(routerID); shortestPath.push_back(goingToB); shortestPath.push_back(goingViaC); shortestPath.push_back(totalLengthD);
		SPT.push_back(shortestPath);
		//grab the adjacency list for B and adds it to available paths
		for (unsigned int i = 0; i < connectionTable.size(); i++) {
			if (connectionTable[i][0] == goingToB || connectionTable[i][1] == goingToB) {
				availablePaths.push_back(connectionTable[i]);
			}
		}
		//remove all available paths that are already reached by A in the SPT
		//(if this code doesn't work, instead rebuild table using a temp table and clearing the original,
		//only adding a path back in if it does not match both a previous destination and B)
		for (unsigned int j = 0; j < availablePaths.size(); j++) {
			bool containsB;
			bool containsA;
			if (availablePaths[j][0] == goingToB || availablePaths[j][1] == goingToB) {
				containsB = true;
			}
			if (availablePaths[j][0] == routerID || availablePaths[j][1] == routerID) {
				containsA = true;
			}
			if (containsA && containsB) {
				availablePaths.erase(availablePaths.begin() + j);
			}
			for (unsigned int i = 0; i < SPT.size(); i++) {
				//we know what goingToB is and that that's now in the SPT, so if we check all the SPT Bs against 0 and 1
				//using goingToB as the other value, we know if we've already reached both nodes in the connection and can remove it
				//must also remove paths that contain both A and goingToB or A and SPT Bs
				if ((containsA && containsB) || (!containsA && !containsB)) {
					break;
				}
				else {
					bool containsSPTB;
					int SPTB = SPT[i][0];
					if (availablePaths[j][0] == SPTB || availablePaths[j][1] == SPTB) {
						containsSPTB = true;
						availablePaths.erase(availablePaths.begin() + j);
					}
				}
			}
		}
		//availablePaths.resize();
	}
}
*/


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
	int neighborNum = (packetSize - (sizeof(uint16_t)+ sizeof(int32_t)))/(4 * sizeof(int32_t));

	//dataLength and NodeAddr
	memcpy(&ID, buff + sizeof(uint16_t), sizeof(int32_t));


	// extract neighborNum groups of "neighbor ,cost, port"
	uint16_t offset = sizeof(uint16_t)+ sizeof(int32_t);
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







int sendToOneUDPCommand(int fdUDP, int NodeID, char c ){

}
int sendToOneUDPTable(int fdUDP, int destNode, vector<vector<int>>& neighborTable, map<int,int>& nodeToPort, int NodeID){

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
	uint16_t packetSize = sizeof(uint16_t)+ neighborNum * (4 * sizeof(uint16_t));
	buff = (char*) malloc(bufflen);
	memcpy(buff,&packetSize, sizeof(uint16_t));

	// neighborNum groups of "neighbor ,cost, port"
	uint16_t offset = sizeof(uint16_t);
	for (int i = 0; i <neighborNum; i++){
		int source = neighborTable[i][0];
		int dest = neighborTable[i][1];
		int cost = neighborTable[i][2];
		int port = neighborTable[i][3];
		memcpy(buff+offset, &source, sizeof(int) );
		memcpy(buff+offset+ sizeof(int), &dest, sizeof(int) );
		memcpy(buff+offset+ sizeof(int)*2, &cost, sizeof(int) );
		memcpy(buff+offset+ sizeof(int)*3, &port, sizeof(int) );
		offset += sizeof(int)*4;
	}
	buff[packetSize]=0;

	//sleep to avoid congestion
	sleep(1.5/NodeID);
	if (sendto(fdUDP, buff, bufflen, 0, (struct sockaddr*)&si_other, slen)!=0)
	{
		cerr << "UDP send fail"<<endl;
		exit(1);
	}
}


int ReceiveUDPTableFromOne(int fdUDP, vector<vector<int>>& connectionTable){

	//Here is to define si_other
	uint16_t packetSize;
	int bufflen = 512;
	struct sockaddr_in si_other;
	socklen_t fromlen;
	char* buff;
	buff = (char*) malloc(bufflen);

	recvfrom(fdUDP, buff, bufflen, 0, (struct sockaddr*)&si_other, &fromlen);
	//
	memcpy(&packetSize, buff, sizeof(uint16_t));
	outfile << "packetSize is: "<< packetSize <<endl;
	int neighborNum = (packetSize - (sizeof(uint16_t)+ sizeof(int)))/(4 * sizeof(uint16_t));
	outfile << "neighborNum is" << neighborNum <<endl;


	// extract neighborNum groups of "neighbor ,cost, port"
	uint16_t offset = sizeof(uint16_t);
	for (int i = 0; i <neighborNum; i++){
		int source, dest, cost, port;
		memcpy(&source,buff+offset, sizeof(int) );
		memcpy(&dest,buff+offset+ sizeof(int),  sizeof(int) );
		memcpy(&cost,buff+offset+ sizeof(int)*2,  sizeof(int) );
		memcpy(&port,buff+offset+ sizeof(int)*3,sizeof(int) );
		offset += sizeof(int)*4;
		vector<int> tempt;
		connectionTable.push_back(tempt);
		connectionTable.at(i).push_back(source);
		connectionTable.at(i).push_back(dest);
		connectionTable.at(i).push_back(cost);
		connectionTable.at(i).push_back(port);
	}

}









int receiveFromAllNeighbors(int fdUDP){

};



void LSP(vector<vector<int> >& neighborTable,vector<vector<int> >& connectionTable, map<int,int>& nodeToPort ){

}






void breakTheMessageReceived(string message,int& NodeAddr,vector<vector<int> >& neighborTable, map<int,int>& nodeToPort){

}


int sendToAllNeighbors(int fdUDP, string message){

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
		outfile << neighborTable[i][0] << " " << neighborTable[i][1] << " " << neighborTable[i][2] << endl;
	}


	//PART OF(DIRECT) node to port table built and print out
	map<int, int> nodeToPort;
	outfile << "NodeToPort table print out: " <<endl;
	buildNodeToPort(nodeToPort,neighborTable);
	for (map<int,int>::iterator it=nodeToPort.begin(); it!=nodeToPort.end(); ++it)
		outfile << it->first << " => " << it->second << '\n';



	map<int, int> flowChart;

	sendCommand(fdTCP, 'R');
	char c;
	c = receiveCommand(fdTCP);

	if (c == 'S') {                                                // MEANS:  It is safe to try to reach neighbors.


		sendToAllNeighbors(fdUDP, "#" + to_string(portUDP));
		receiveFromAllNeighbors(fdUDP);
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

	*/
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


