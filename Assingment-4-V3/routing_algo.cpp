#include "node.h"
#include <iostream>

using namespace std;

#define triple pair<int,pair<string,string>>

void printRT(vector<RoutingNode *> &nd) {
/*Print routing table entries*/
    for (auto &i: nd) {
        i->printTable();
    }
}

void initializeInterfaceTable(vector<RoutingNode *> &routerArr,
                              unordered_map<string, unordered_map<string, pair<string, string>>> &interfaceTable) {
    for (RoutingNode *cntNode: routerArr) {
        for (pair<NetInterface, Node *> &cntInterface: cntNode->getInterfaces()) {
            interfaceTable[cntNode->getName()][cntInterface.second->getName()] = make_pair(cntInterface.first.getip(),
                                                                                           cntInterface.first.getConnectedIp());
        }
    }
}

void initializeNodeNameInterfaces(vector<RoutingNode *> &routerArr,
                                  unordered_map<string, vector<string>> &nameInterfaceMapping) {
    for (RoutingNode *cntNode: routerArr) {
        vector<string> cntNodeInterfaces;
        for (pair<NetInterface, Node *> tmpInterfacePair: cntNode->getInterfaces()) {
            cntNodeInterfaces.push_back(tmpInterfacePair.first.getip());
        }
        nameInterfaceMapping[cntNode->getName()] = cntNodeInterfaces;
    }
}

// Function to broadcast message
void RoutingNode::sendMsg() {
    for (pair<NetInterface, Node *> &adjNode: this->getAdjacentNodes()) {
        BroadcastTableEntry newBroadcastEntry;
        newBroadcastEntry.nodeName = adjNode.second->getName();
        newBroadcastEntry.node = adjNode.second;
        newBroadcastEntry.cost = adjNode.first.getCost();
        routingTableBroadcast.emplace_back(this->getName(), newBroadcastEntry);
    }
}

// Function to receive broadcast messages
void RoutingNode::recvMsg() {
    for (pair<string, BroadcastTableEntry> &tableEntry: routingTableBroadcast) {
        BroadcastTableEntry newEntry;
        newEntry.nodeName = tableEntry.second.nodeName;
        newEntry.cost = tableEntry.second.cost;
        newEntry.node = tableEntry.second.node;
        receivedRoutingTable.emplace_back(tableEntry.first, newEntry);
    }
}

// Initialize graph using the received routing tables
void RoutingNode::initializeGraph(unordered_map<string, vector<pair<Node *, int>>> &graph) {
    for (pair<string, BroadcastTableEntry> &cntEntry: this->receivedRoutingTable) {
        if (graph.find(cntEntry.first) != graph.end()) {
            graph[cntEntry.first].push_back(make_pair(cntEntry.second.node, cntEntry.second.cost));
        } else {
            vector<pair<Node *, int>> newGraphNode;
            newGraphNode.emplace_back(cntEntry.second.node, cntEntry.second.cost);
            graph[cntEntry.first] = newGraphNode;
        }
    }
}

// Dijkstra's algorithm to get the shortest path from each node
// Complexity O(nlogn)
void dijkstra(unordered_map<string, vector<pair<Node *, int>>> &graph, string &startNode,
              unordered_map<string, pair<string, int>> &distanceMap) {
    priority_queue<triple, vector<triple>, greater<>> minHeap;
    minHeap.push(make_pair(0, make_pair(startNode, startNode)));
    distanceMap[startNode].first = startNode;
    distanceMap[startNode].second = 0;

    while (!minHeap.empty()) {
        triple topNode = minHeap.top();
        minHeap.pop();
        string topNodeName = topNode.second.first;
        string topNodePath = topNode.second.second;
        vector<pair<Node *, int>> adjacentNodes = graph[topNodeName];

        for (pair<Node *, int> adjNode: adjacentNodes) {
            string adjNodeName = adjNode.first->getName();
            if (distanceMap[topNodeName].second + adjNode.second < distanceMap[adjNodeName].second) {
                distanceMap[adjNodeName].second = distanceMap[topNodeName].second + adjNode.second;
                distanceMap[adjNodeName].first = distanceMap[topNodeName].first + adjNode.first->getName();
                minHeap.push(make_pair(distanceMap[adjNodeName].second,
                                       make_pair(adjNodeName, distanceMap[adjNodeName].first)));
            }
        }
    }
}

// Based on results from dijkstra's algorithm this functions populates the routing table
void populateRoutingTable(RoutingNode *cntNode,
                          unordered_map<string, unordered_map<string, pair<string, string>>> &interfaceTable,
                          unordered_map<string, pair<string, int>> &distanceMap,
                          unordered_map<string, vector<string>> &nodeNameInterfaces) {
    auto itr = distanceMap.begin();
    while (itr != distanceMap.end()) {
        string traversal = itr->second.first;
        if (itr->second.second == 0) {
            itr++;
            continue;
        }
        string first = traversal.substr(0, 1);
        string second = traversal.substr(1, 1);
        string interface = interfaceTable[first][second].first;
        string nextHop = interfaceTable[first][second].second;

        for (string &routerInterfaceIPs: nodeNameInterfaces[itr->first]) {
            RoutingEntry newEntry;
            newEntry.dstip = routerInterfaceIPs;
            newEntry.ip_interface = interface;
            newEntry.nexthop = nextHop;
            newEntry.cost = itr->second.second;
            cntNode->addTblEntry(newEntry);
        }
        itr++;
    }
}


void testPrint(unordered_map<string, pair<string, int>> distanceMap) {
    auto itr = distanceMap.begin();
    while (itr != distanceMap.end()) {
        cout << itr->first << "    " << itr->second.first << "   " << itr->second.second << endl;
        itr++;
    }
    cout << endl << endl << endl;
}

void routingAlgo(vector<RoutingNode *> &nd) {
//  Each node initializing routing table with adjacent nodes
    unordered_map<string, unordered_map<string, pair<string, string>>> interfaceTable;
    initializeInterfaceTable(nd, interfaceTable);
    unordered_map<string, vector<string>> nodeNameInterfaces;
    initializeNodeNameInterfaces(nd, nodeNameInterfaces);
//  Broadcasting the routing table to whole network
    for (RoutingNode *cntNode: nd) {
        cntNode->sendMsg();
    }


    for (RoutingNode *tmp: nd) {
//      Receiving broadcast message from every node in the network
        tmp->recvMsg();

//      Initializing graph
        unordered_map<string, vector<pair<Node *, int>>> graph;
        tmp->initializeGraph(graph);

//      Running Dijkstra's algorithm for each node
        unordered_map<string, pair<string, int>> distanceMap;
        for (auto &i: nd) {
            distanceMap[i->getName()] = make_pair("", INT_MAX);
        }
        string startNode = tmp->getName();
        dijkstra(graph, startNode, distanceMap);

//      Populating routing table based on dijkstra's result
        populateRoutingTable(tmp, interfaceTable, distanceMap, nodeNameInterfaces);
        cout << "Routing table of: " << tmp->getName() << endl;
//        testPrint(distanceMap);
//        cout << endl;
    }

    cout << endl << endl << endl;
    printRT(nd);
}









