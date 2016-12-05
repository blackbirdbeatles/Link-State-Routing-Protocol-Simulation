#include "project3.h"

Manager::Manager(ifstream& inFile) {
	//creates a manager using the inFile, reading in the number of routers,
	//the routing table, and the commands to run
	string ManagerLine = "";
	//int startVertex = 0;
	int grouping = 0;
	while (getline(inFile, ManagerLine)) {
		istringstream iss;
		iss.str(ManagerLine);
		if (grouping == 0) {
			grouping++;
			iss >> numberOfRouters;
		}
		else if (grouping == 1) {
			vector<int> routerConnection;
			for (int i = 0; i < 3; i++) {
				int nextNum;
				iss >> nextNum;
				if (nextNum == -1) {
					cout << "Breaking -1" << endl;
					grouping++;
					break;
				}
				routerConnection.push_back(nextNum);
			}
			cout << "Router Connection: " << routerConnection[0] << " " << routerConnection[1] << " " << routerConnection[2] << endl;
			routingTable.push_back(routerConnection);
		}
		else if(grouping == 2){
			vector<int> command;
			for (int i = 0; i < 2; i++) {
				int Router;
				iss >> Router;
				if (Router == -1) {
					cout << "Breaking -1" << endl;
					grouping++;
					break;
				}
				command.push_back(Router);
			}
			cout << "Command: " << command[0] << " " << command[1] << endl;
			routingCommands.push_back(command);
		}
	}
}

int Manager::makeRouters() {
	//creates routers as children branches
	for (int i = 0; i < numberOfRouters; i++) {
		//fork off routers with their individualized information
		routerID = fork();
		if (routerID == 0) {
			//I'm the child
			//create router with information from i
			vector<vector<int>> individualRoutingTable;
			for (unsigned int j = 0; j < routingTable.size(); j++) {
				if (routingTable[j][0] == i || routingTable[j][1] == i) {
					individualRoutingTable.push_back(routingTable[j]);
				}
			}
			//Router router(individualRoutingTable, numberOfRouters);
			//break?  run router methods? idk
		}
		else {
			//I'm the parent
			//wpid = wait(&status);
		}
	}
}

int Manager::managerPrint(string message) {
	ofstream manaFile;
	manaFile.open("manager.out");
	manaFile << message << endl;
	return 0;
}


int main(int argc, char *argv[]) {
	if (argc == 2) {
		ifstream inFile = argv[1];
		Manager man(inFile);
		man.makeRouters();
	}
	else {
		cerr << "Wrong args, need 2 total" << endl;
	}

}
