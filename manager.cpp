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
	pid_t routerID;
	for (int i = 0; i < numberOfRouters; i++) {
		//fork off routers with their individualized information
		//if and else will run essentially simultaneously; might be good to fork off
		//all routers and then do parent code outside of for loop rather than waiting
		//for each router to complete its instantiation.  That said, not sure - it would
		//be easier if the routers were their own executable but whatever
		routerID = fork();
		if (routerID == 0) {
			//I'm the child
			//create router with no information
			//Router router();
			//break?  run router methods? idk, but be careful that they don't interfere with main.
			//in fact might want routerID to be a Manager global variable since routers will also
			//be part-manager (just not the parent part) I think
			//this could be done by essentially keeping the routers in an infinite loop though,
			//and when they break it they exit.  
		}
		else {
			//I'm the parent
			//make specific routing table to provide to router when it asks for its neighbors
			//since 
			vector<vector<int>> individualRoutingTable;
			for (unsigned int j = 0; j < routingTable.size(); j++) {
				if (routingTable[j][0] == i || routingTable[j][1] == i) {
					individualRoutingTable.push_back(routingTable[j]);
				}
			}
			//send routing table and other information to the router (this could be done in main or simply
			//after all routers are created (after the for loop), if we create a vector of the individualRoutingTables
			//which would allow us to essentially create all of the threads and deal with them simultaneously)
			//we'd need to keep track of which thread is which probably
			//wpid = wait(&status); (code from previous class that could potentially help)
		}
	}
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
