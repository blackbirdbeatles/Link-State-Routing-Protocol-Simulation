#include "project3.h"

using namespace std;


int Router::routerPrint(string message) {
	ofstream routerFile;
	string fileName = "router";
	string ID = to_string(routerID);
	fileName.append(ID.c_str());
	fileName.append(".out");
	routerFile.open(fileName);
	routerFile << message << endl;
	return 0;
}

/*

int Router::buildSPT() {
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



//这一群在Ben哪里有 begin

void sendPortNum(int the_fd, long num){
	int sizeOfPacket = sizeof(uint8_t)+ sizeof(long);
	char buff[size];
	char* toSend;
	toSend = (char*)malloc(size);
	memcpy(toSend, &size, sizeof(int));
	memcpy(toSend + sizeof(int), &num, sizeof(long));
	if(send(the_fd, toSend, size, 0) == -1){
		cerr << "send error" << endl;
	}
}
void sendCommand(int the_fd, char c){
	char buff[sizeof(long)];
	char* toSend;
	toSend = (char*)malloc(sizeof(long));
	memcpy(toSend, &num, sizeof(long));
	if(send(the_fd, toSend, sizeof(long), 0) == -1){
		cerr << "send error" << endl;
	}
}
ResultUDPCreation Router::createUDPconnection(){
	ResultUDPCreation r;
	return r;

};
//说的就是这群 end






int Router::receiveFromOneUDP(int fdUDP, int& sourceNode, string& message){

}
int Router::sendToOneUDP(int fdUDP, int destNode, string message){

}


//This two are those key point for packing and unpacking
/*
int Router::sendToManager(const int fdTCP,string message, char flag){
	switch (flag){
		case 'P':
			packAndSendP(fdTCP,message);
			break;
		case 'C':
			packAndSendC(fdTCP,message);
			break;

	}

};
 */
//这要从Ben那里拿过来
void sendCommand(int fd, char c){

}

int Router::receiveFromManager(const int fdTCP, string message){


};




int Router::receiveFromAllNeighbors(int fdUDP){

};



void Router::LSP(vector<vector<int> >& neighborTable,vector<vector<int> >& connectionTable, map<int,int>& nodeToPort ){

}






void Router::breakTheMessageReceived(string message,int& NodeAddr,vector<vector<int> >& neighborTable, map<int,int>& nodeToPort){

}
void Router::breakTestMessage(string message,int& source,int& dest){

}

int Router::sendToAllNeighbors(int fdUDP, string message){

}


void Router::flowChartBuild(vector<vector<int>>& connectionTable, map<int, int>& flowChart){

}



void *waitMsg(void* p){
	Router r;
	string message;
	int fdTCP = (*(Args*)p).arg1;
	int fdUDP = (*(Args*)p).arg2;
	int NodeID = fdUDP = (*(Args*)p).arg3;

	message = "";
	r.receiveFromManager(fdTCP, message);
	while (message != "-1"){






		message = "";
		r.receiveFromManager(fdTCP, message);
	}
	r.sendToOneUDP(fdUDP,NodeID, "-1");
}



int Router::dosometing(){
	ResultUDPCreation r;
	int fdUDP,portUDP;
	r = createUDPconnection();
	fdUDP = r.fd;
	portUDP = r.port;
	//~~~~~~~~~~~~~~~~~~
	int NodeAddr;
	//Here are the variables may be transfered to Project3.h
	vector<vector<int> > connectionTable, neighborTable;
	map<int, int> flowChart;
	int fdTCP = connectToServer("127.0.0.1", to_string(serverPort).c_str());
	sendPortNum(fdTCP,portUDP);
	//clear message to avoid that it's original value affect the massage we just received
	string message = "";
	receiveFromManager(fdTCP, message);
	//We need to figure out the format of the packet
	breakTheMessageReceived(message, NodeAddr, neighborTable, nodeToPort);
	sendCommand(fdTCP, 'R');
	message = "";
	receiveFromManager(fdTCP, message);
	if (message == "It is safe to try to reach neighbors."){
		//Here we need to use UPD to send packets to neighbors

		sendToAllNeighbors(fdUDP, "#" + to_string(portUDP));
		receiveFromAllNeighbors(fdUDP);

		sendCommand(fdTCP, 'C');
		message = "";
		receiveFromManager(fdTCP, message);
		if (message == "Up"){
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
			while (message != "-1"){
				int source, dest;
				breakTestMessage(message, source, dest);
				if (dest == NodeAddr)
				{
					routerPrint("I'm destination. I've already got the packet.");
					sendToOneUDP(fdUDP,NodeAddr,"-1");
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
}
int main(){

	return 0;
}





