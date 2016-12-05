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
				totalLengthD = totalLengthD + SPT[i][2]
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
		availablePaths.resize();
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

