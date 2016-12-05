#include "project3.h"
#include <iostream>
#include <cstring>
#include <string>
#include <map>
using namespace std;



int sendToManager(const int fdTCP,string message);
int connectToServer(const char* ip, const char* portNum);
int receiveFromManager(const int fdTCP, string message);
int receiveFromAllNeighbors(int fdUDP);
int createUDPconnection(const string portUDP);

void breakTheMessageReceived(string message,string& NodeAddr,map<string, int>& connecticityTable){

}

int sendToAllNeighbors(int fdUDP, string message){

}

int receiveFromAllNeighbors(int fdUDP){

}



int main(){
    string portUDP = createUport();
    string serverPort = ServerP;
    string NodeAddr;
    map<string, int> connecticityTable;

    //Here is to make a TCP connection to manager
    int fdTCP = connectToServer("127.0.0.1", serverPort.c_str());
    //need to be implement
    sendToManager(fdTCP, portUDP);
    //clear message to avoid that it's original value affect the massage we just received
    string message = "";
    if (receiveFromManager(fdTCP, message)==0) {
        //We need to figure out the format of the packet
        breakTheMessageReceived(message, NodeAddr, connecticityTable);
        sendToManager(fdTCP, "Ready!");
        message = "";
        if (receiveFromManager(fdTCP, message)==0){
            if (message == "It is safe to try to reach neighbors."){
                //Here we need to use UPD to send packets to neighbors
                int fdUDP = createUDPconnection(portUDP);
                sendToAllNeighbors(fdUDP, "#" + portUDP);
                if (receiveFromAllNeighbors(fdUDP) == 0){
                    sendToManager(fdTCP, "Complete");
                }



            }
            else{
                cerr << "Error: The following message expected: safe!" <<endl;
                return -1;
            }
        }
        else{
            cerr << "Error: Received From Manager for safe!" <<endl;
            return -1;
        }
    }
    else{
        cerr << "Error: Received From Manager for Node Addr Error!" <<endl;
        return -1;
    }


}


