#ifndef ArgPreferenceGraph_H
#define ArgPreferenceGraph_H

#include <vector>
#include <map>
#include <set>
#include "Argument.h"


using namespace std;

class ArgPreferenceGraph {

public:
    ArgPreferenceGraph();
    ArgPreferenceGraph (const ArgPreferenceGraph &oldAPG);
    void initialize(set<int> &_allNodes);
    void incrementPreference(int preferred, int non_preferred);
    void constructPreferenceGraph(map<int, set<int>> &incomingEdges, map<int, set<int>> &outgoingEdges);
    
private:
    vector<int> allNodes;
    map<pair<int, int>, int> edgeWeights;
};

#endif