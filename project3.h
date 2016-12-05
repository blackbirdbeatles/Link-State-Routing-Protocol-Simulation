#ifndef PROJECT_THREE
#define PROJECT_THREE
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>

using namespace std;
class Manager {
public:
	Manager(ifstream& inFile);
	int makeRouters();
private:
	vector<vector<int>> routingTable;
	vector<vector<int>> routingCommands;
	int numberOfRouters;
	int managerPrint(string message);
	//0 9 40
	//0 4 60
	//if routingTable[1] == RouterID or routingTable[0] == RouterID
	//add routingTable[1] or [0] to passedRoutingTable
	//Router router(passedRoutingTable);
	//if RouterID == 1
	//passedRoutingTable.push_back(1 8 70)
	//Router router(passedRoutingTable)
};

class Router {
public:
	Router(const vector<vector<int>>& routingTable) {

		for (int j = 0; j < routingTable.size(); j++) {
			//routingTable[0][0] == 0 = sendRouter
			//routinngTable[0][1] == 9 = ReceiveRouter
			//routingTable[0][2] == 40 = cost

			//routingTable[0] == 1 8 70
			//routingTable[1] == NULL

			//hey manager, am I 1 or am I 8?
			//you're 1
			//okay cool I'll make my NEWroutingTable

			//hey 8 who are you by
			//8: I'm by 3 (70), 1 (70), and 9 (70) but I'm asking them, too
			//1: ok who are you by now
			//8: I'm by 3 (70), 1 (70), and 9(70), and I can reach 0 by 9, 3 by 9, 5 by 9, and 7 by 9
			
			/*0 9 40
				0 4 60
				0 2 40*/
			//we are router 0
			//broadcast to 9, 4, 2: 0 4 60, 0 2 40, 0 9 40
			//broadcast from 9: 3 9 20, 0 9 40, 5 9 70, 7 9 60, 8 9 70
			//discard 0 9 40 repeat
			//broadcast to 4 and 2: 3 9 20, 5 9 70, 7 9 60, 8 9 70
			//broadcast from 2: 0 2 40 
			//discard 0 2 40 repeat
			//broadcast from 4: 0 4 60, 4 7 70, 4 6 40
			//discard 0 4 60 repeat
			//broadcast to 2 and 9: 4 7 70, 4 6 40
			//broadcast from 9: 3 8 70, 3 5 70
		}
	}

	int sendToManager(const int fdTCP, string message);
	int connectToServer(const char* ip, const char* portNum);

	int receiveFromManager(const int fdTCP, string message);
	int receiveFromAllNeighbors(int fdUDP);
	int createUDPconnection(const string portUDP);

	void breakTheMessageReceived(string message, string& NodeAddr, map<string, int>& connectivityTable);
	int sendToAllNeighbors(int fdUDP, string message);
	int receiveFromAllNeighbors(int fdUDP);

	int sendToOneRouter(int fdUDP, string message, struct sockaddr_in dest);

	int routerPrint(string message);
	int buildSPT();

	//first contains its neighbors, then contains which neighbor it can send to to reach a specific node (created by Djikstra's
	//after receiving all connectionTables)
	//scratch the above, we're getting two tables
	vector<vector<int>> connectionTable;
	vector<vector<int>> neighborTable;
	int routerID;
	//SPT will have the shortest path from A to B via C with total length D from A to B as <B, C, D>.  A is the routerID.
	vector<vector<int>> SPT;

};


#endif
