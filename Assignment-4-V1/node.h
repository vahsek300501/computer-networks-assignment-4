#include <bits/stdc++.h>

using namespace std;

/*
  Each row in the table will have these fields
  dstip:	Destination IP address
  nexthop: 	Next hop on the path to reach dstip
  ip_interface: nexthop is reachable via this interface (a node can have multiple interfaces)
  cost: 	cost of reaching dstip (number of hops)
*/
class RoutingEntry {
public:
    string dstip, nexthop;
    string ip_interface;
    int cost;
    /* You will have to add a new variable say, share_cost.
    'share_cost' holds the cost that has to be shared with the neighbor.
    This change will be required to complete Q.3. of the assignment. */
};

/*
 * Class for specifying the sort order of Routing Table Entries
 * while printing the routing tables
 * 
*/
class Comparator {
public:
    bool operator()(const RoutingEntry &R1, const RoutingEntry &R2);
};

/*
  This is the routing table
*/
struct routingtbl {
    vector<RoutingEntry> tbl;
};

/*
  Message format to be sent by a sender
  from: 		Sender's ip
  mytbl: 		Senders routing table
  recvip:		Receiver's ip
*/
class RouteMsg {
public:
    string from;            // I am sending this message, so it must be me i.e. if A is sending mesg to B then it is A's ip address
    struct routingtbl *mytbl;    // This is routing table of A
    string recvip;        // B ip address that will receive this message
};

/*
  Emulation of network interface. Since we do not have a wire class, 
  we are showing the connection by the pair of IP's
  
  ip: 		Own ip
  connectedTo: 	An address to which above-mentioned ip is connected via ethernet.
*/
class NetInterface {
private:
    string ip;
    string connectedTo;    //this node is connected to this ip

public:
    string getip();

    string getConnectedIp();

    void setip(string p_ip);

    void setConnectedip(string p_ip);
};

/*
  Struct of each node
  name: 	It is just a label for a node
  interfaces: 	List of network interfaces a node have
  Node* is part of each interface, it easily allows to send message to another node
  mytbl: 		Node's routing table
*/
class Node {
private:
    string name;
    vector<pair<NetInterface, Node *> > interfaces;
protected:
    struct routingtbl mytbl;

    virtual void recvMsg(RouteMsg *msg);

    bool isMyInterface(const string &eth);

public:
    void setName(string name);

    void addInterface(string ip, string connip, Node *nextHop);

    void addTblEntry(const string &myip, int cost);

    string getName();

    struct routingtbl getTable();

    void printTable();

    void sendMsg();

    vector<pair<NetInterface, Node *>> &getAdjacentNodeList();

};

class RoutingNode : public Node {
public:
    void recvMsg(RouteMsg *msg);
};

class AdjacentNode {
public:
    NetInterface adjNetInterface;
    Node *adjNode;
    int cost;

    AdjacentNode(NetInterface &p_adjNetInterface, Node *p_adjNode, int p_cost);
};

class AdjacentNodeComparator {
public:
    int operator()(const AdjacentNode &node1, const AdjacentNode &node2) {
        return node1.cost > node2.cost;
    }
};
