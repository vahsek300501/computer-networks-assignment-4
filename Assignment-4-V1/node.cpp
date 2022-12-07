//
// Created by kesha on 06-12-2022.
//

#include "node.h"

bool Comparator::operator()(const RoutingEntry &R1, const RoutingEntry &R2) {
    if (R1.cost == R2.cost) {
        return R1.dstip.compare(R2.dstip) < 0;
    } else if (R1.cost > R2.cost) {
        return false;
    } else {
        return true;
    }
}


// Functions of NetInterface
string NetInterface::getip() {
    return this->ip;
}

string NetInterface::getConnectedIp() {
    return this->connectedTo;
}

void NetInterface::setip(string p_ip) {
    this->ip = std::move(p_ip);
}

void NetInterface::setConnectedip(string p_ip) {
    this->connectedTo = std::move(p_ip);
}


// Functions of Node class
void Node::recvMsg(RouteMsg *msg) {
    cout << "Base" << endl;
}

bool Node::isMyInterface(const string &eth) {
    for (auto &interface: interfaces) {
        if (interface.first.getip() == eth)
            return true;
    }
    return false;
}

void Node::setName(string p_name) {
    this->name = std::move(p_name);
}

void Node::addInterface(string ip, string connip, Node *nextHop) {
    NetInterface eth;
    eth.setip(std::move(ip));
    eth.setConnectedip(std::move(connip));
    interfaces.emplace_back(eth, nextHop);
}

void Node::addTblEntry(const string &myip, int cost) {
    RoutingEntry entry;
    entry.dstip = myip;
    entry.nexthop = myip;
    entry.ip_interface = myip;
    entry.cost = cost;
    mytbl.tbl.push_back(entry);
}

string Node::getName() {
    return this->name;
}

void Node::printTable() {
    Comparator myobject;
    sort(mytbl.tbl.begin(), mytbl.tbl.end(), myobject);
    cout << this->getName() << ":" << endl;
    for (auto &i: mytbl.tbl) {
        cout << i.dstip << " | " << i.nexthop << " | " << i.ip_interface << " | "
             << i.cost << endl;
    }
}

void Node::sendMsg() {
    struct routingtbl ntbl;
    for (auto &i: mytbl.tbl) {
        ntbl.tbl.push_back(i);
    }

    for (auto &interface: interfaces) {
        RouteMsg msg;
        msg.from = interface.first.getip();
        msg.mytbl = &ntbl;
        msg.recvip = interface.first.getConnectedIp();
        interface.second->recvMsg(&msg);
    }
}

vector<pair<NetInterface, Node *>> &Node::getAdjacentNodeList() {
    return this->interfaces;
}

AdjacentNode::AdjacentNode(NetInterface &p_adjNetInterface, Node *p_adjNode, int p_cost) {
    this->adjNetInterface = p_adjNetInterface;
    this->adjNode = p_adjNode;
    this->cost = p_cost;
}