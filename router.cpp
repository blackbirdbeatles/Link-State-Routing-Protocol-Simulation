#include "project3.h"
#include <iostream>
#include <cstring>
#include <string>
#include <map>
using namespace std;

int Router::sendToManager(const int fdTCP, string message);
int Router::connectToServer(const char* ip, const char* portNum);

int Router::receiveFromManager(const int fdTCP, string message) {
	return 0;
}
int Router::receiveFromAllNeighbors(int fdUDP);
int Router::createUDPconnection(const string portUDP);

void Router::breakTheMessageReceived(string message, string& NodeAddr, map<string, int>& connectivityTable) {

}

int Router::sendToAllNeighbors(int fdUDP, string message) {
	struct sockaddr_in dest;
	dest = 0; //I guess?  I don't know what to do with this.
	

}

int Router::receiveFromAllNeighbors(int fdUDP) {
	//check to see if received message destination ID == this router ID.  If not, send it to neighbor that
	//is next on the route to destination router
}

int Router::sendToOneRouter(int fdUDP, string message, struct sockaddr_in dest) {
	// now with UDP datagram sockets:
	//getaddrinfo(...
	//dest = ...  // assume "dest" holds the address of the destination
	//dgram_socket = socket(...

	// send secret message normally:
	//dest needs to be updated to be each neighbor
	sendto(fdUDP, message.c_str(), strlen(message.c_str()) + 1, 0,(struct sockaddr*)&dest, sizeof dest);
}

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
		int goingViaC, originalC;
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
			if ((goingViaC == SPT[i][0]) && SPT[i][1] != routerID) {
				goingViaC = SPT[i][1];
				//totalLengthD = totalLengthD + SPT[i][2];
				break;
			}
		}
		//create the shortest path node
		shortestPath.push_back(goingToB); shortestPath.push_back(goingViaC); shortestPath.push_back(totalLengthD);
		SPT.push_back(shortestPath);
		//grab the adjacency list for B and adds it to available paths, unless B and its path are already in there
		for (unsigned int i = 0; i < connectionTable.size(); i++) {
			bool containsA = false;
			bool containsB = false;
			bool containsC = false;
			//cout << "Add " << connectionTable[i][0] << " " << connectionTable[i][1] << " " << connectionTable[i][2] << endl;
			for(unsigned int j = 0; j < SPT.size(); j++){
				if(SPT[j][0] == connectionTable[i][0] || routerID == connectionTable[i][0])
					containsA = true;
				if(SPT[j][0] == connectionTable[i][1] || routerID == connectionTable[i][1])
					containsB = true;
			}
			if(containsA && containsB){
				containsC = true; 
			}

			if ((connectionTable[i][0] == goingToB || connectionTable[i][1] == goingToB) && !containsC) {
				//cout << "Yes" << endl;
				connectionTable[i][2] = connectionTable[i][2] + shortestPath[2];
				availablePaths.push_back(connectionTable[i]);
			}
			else{
				//cout << "No" << endl;}
		}
		//remove all available paths that are already reached by A in the SPT
		vector<vector<int>> newPaths;
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
						//cout << "Erasing: " << availablePaths[j][0] << " " << availablePaths[j][1] << " " << availablePaths[j][2] << endl;
						//availablePaths.erase(availablePaths.begin() + j);
					}
					else if((availablePaths[j][0] == SPTB || availablePaths[j][1] == SPTB) && containsSPTB1 == true){
						containsSPTB2 = true;
					}
			}
			//add the path to new paths if either of its paths is not in the SPT
			//so if containsSPTB2 is false and if containsA or containsSPTB1 is false
			if(!containsSPTB2 && (!containsA || !containsSPTB1)){
				newPaths.push_back(availablePaths[j]);
			}
		}
		availablePaths = newPaths;
	}
}

int main() {
	Router r;
	r.routerID = 0;
	vector<int> A{0, 1, 50};
	vector<int> B{0, 3, 20};
	vector<int> C{1, 2, 20};
	vector<int> D{1, 3, 40};
	vector<int> E{4, 3, 30};
	vector<int> F{5, 2, 20};
	r.connectionTable.push_back(A); r.connectionTable.push_back(B); r.connectionTable.push_back(C);
	r.connectionTable.push_back(D); r.connectionTable.push_back(E); r.connectionTable.push_back(F);
	r.neighborTable.push_back(A); r.neighborTable.push_back(B);
	r.buildSPT();
	for(int i = 0; i < r.SPT.size(); i++){
		cout << r.SPT[i][0] << " " << r.SPT[i][1] << " " << r.SPT[i][2] << endl; 
	}

}


int main() {
	string portUDP = createUport();
	string serverPort = ServerP;
	string NodeAddr;
	map<string, int> connectivityTable;

	//Here is to make a TCP connection to manager
	int fdTCP = connectToServer("127.0.0.1", serverPort.c_str());
	//need to be implement
	sendToManager(fdTCP, portUDP);
	//clear message to avoid that it's original value affect the massage we just received
	string message = "";
	if (receiveFromManager(fdTCP, message) == 0) {
		//We need to figure out the format of the packet
		breakTheMessageReceived(message, NodeAddr, connectivityTable);
		sendToManager(fdTCP, "Ready!");
		message = "";
		if (receiveFromManager(fdTCP, message) == 0) {
			if (message == "It is safe to try to reach neighbors.") {
				//Here we need to use UPD to send packets to neighbors
				int fdUDP = createUDPconnection(portUDP);
				sendToAllNeighbors(fdUDP, "#" + portUDP);
				if (receiveFromAllNeighbors(fdUDP) == 0) {
					sendToManager(fdTCP, "Complete");
				}
			}
			else {
				cerr << "Error: The following message expected: safe!" << endl;
				return -1;
			}
		}
		else {
			cerr << "Error: Received From Manager for safe!" << endl;
			return -1;
		}
	}
	else {
		cerr << "Error: Received From Manager for Node Addr Error!" << endl;
		return -1;
	}


}

