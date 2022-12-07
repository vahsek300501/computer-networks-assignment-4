//
// Created by kesha on 06-12-2022.
//

#ifndef ASSIGNMENT_4_ROUTING_ALGO_H
#define ASSIGNMENT_4_ROUTING_ALGO_H

#include "node.h"

class RoutingAlgorithm {
    unordered_map<string, vector<AdjacentNode>> getGraph(const vector<RoutingNode *> &nodeList);

    void dijkstraAlgorithm(unordered_map<string, vector<AdjacentNode>> &graph,
                           string &startNode, int n, unordered_map<string, pair<int, string>> &finalDistance);

public:
    void routingAlgo(const vector<RoutingNode *> &nd);
};


#endif //ASSIGNMENT_4_ROUTING_ALGO_H
