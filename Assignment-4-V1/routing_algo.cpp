#include "routing_algo.h"

using namespace std;

//void printRT(vector<RoutingNode *> nd) {
///*Print routing table entries*/
//    for (auto &i: nd) {
//        i->printTable();
//    }
//}

unordered_map<string, vector<AdjacentNode>> RoutingAlgorithm::getGraph(const vector<RoutingNode *> &nodeList) {
    unordered_map<string, vector<AdjacentNode>> graph;
    for (RoutingNode *tmpNode: nodeList) {
        vector<AdjacentNode> listAdjNode;

        vector<pair<NetInterface, Node *>> &adjList = tmpNode->getAdjacentNodeList();
        for (pair<NetInterface, Node *> &adjListNode: adjList) {
            AdjacentNode tmp(adjListNode.first, adjListNode.second, 1);
            listAdjNode.push_back(tmp);
        }
        graph[tmpNode->getName()] = listAdjNode;
    }
    return graph;
}

void RoutingAlgorithm::dijkstraAlgorithm(unordered_map<string, vector<AdjacentNode>> &graph,
                                         string &startNode, int n,
                                         unordered_map<string, pair<int, string>> &finalDistance) {
    priority_queue<pair<int, pair<string, string>>, vector<pair<int, pair<string, string>>>, greater<>> minHeap;
    minHeap.push(make_pair(0, make_pair(startNode, "")));
    finalDistance[startNode].first = 0;
    finalDistance[startNode].second = "";

    while (!minHeap.empty()) {
        pair<int, pair<string, string>> topNode = minHeap.top();
        minHeap.pop();
        string topNodeName = topNode.second.first;
        string topNodePath = topNode.second.second;
        int topNodeCost = topNode.first;

        vector<AdjacentNode> adjNodes = graph[topNode.second.first];
        for (AdjacentNode adj: adjNodes) {
            string adjNodeName = adj.adjNode->getName();

            if (finalDistance[topNodeName].first + adj.cost < finalDistance[adjNodeName].first) {
                finalDistance[adjNodeName].first = finalDistance[topNodeName].first + adj.cost;
                finalDistance[adjNodeName].second = topNodePath + adjNodeName;
                pair<int, pair<string, string>> newNodeAdd = make_pair(finalDistance[adjNodeName].first,
                                                                       make_pair(adjNodeName,
                                                                                 finalDistance[adjNodeName].second));
                minHeap.push(newNodeAdd);
            }
        }
    }
}

void RoutingAlgorithm::routingAlgo(const vector<RoutingNode *> &nd) {
    unordered_map<string, vector<AdjacentNode>> graph = getGraph(nd);
    unordered_map<string, pair<int, string>> distanceMap;
    int n = (int) nd.size();
    for (RoutingNode *tmp: nd) {
        distanceMap[tmp->getName()] = make_pair(INT_MAX, "");
    }
    string x = "C";
    dijkstraAlgorithm(graph, x, n, distanceMap);

    auto itr = distanceMap.begin();
    while (itr != distanceMap.end()) {
        cout << itr->first << "   " << itr->second.second << "  " << itr->second.first << endl;
        itr++;
    }
    cout << endl;
}

void RoutingNode::recvMsg(RouteMsg *msg) {
    //your code here
}




