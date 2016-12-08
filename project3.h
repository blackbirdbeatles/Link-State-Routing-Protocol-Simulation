#ifndef PROJECT_THREE
#define PROJECT_THREE
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>

#include <cstring>
#include <map>
#include <pthread.h>


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/syscall.h>

#define PORT "65003"





struct Args {
	int arg1;
	int arg2;
	int arg3;
};
struct ResultUDPCreation{
	int port;
	int fd;
};
struct routerInfo{
	int32_t routerID;
	int32_t portUDP;
	int32_t sockfd;
};

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


/*
class Router {
public:


	int dosometing();

	int  sendToManager(const int fdTCP,string message);
	int  connectToServer(const char* ip, const char* portNum);
	int  receiveFromManager(const int fdTCP, string message);
	int  receiveFromAllNeighbors(int fdUDP);
	ResultUDPCreation  createUDPconnection();


	void  LSP(vector<vector<int> >& neighborTable, vector<vector<int> >& connectionTable, map<int,int>& nodeToPort);
	int  receiveFromOneUDP(int fdUDP, int& sourceNode, string& message);
	int sendToOneUDP(int fdUDP, int destNode, string message);




	void  breakTheMessageReceived(string message,int& NodeAddr,vector<vector<int> >& neighborTable, map<int,int>& nodeToport);
	void  breakTestMessage(string message,int& source,int& dest);

	int  sendToAllNeighbors(int fdUDP, string message);
	void  flowChartBuild(vector<vector<int>>& connectionTable, map<int, int>& flowChart);



	//void*  waitMsg(void* p);

	
	
	
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
	map<int,int> nodeToPort;

};
*/


#endif
