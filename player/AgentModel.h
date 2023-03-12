#ifndef AgentModel_H
#define AgentModel_H

#include <vector>

using namespace std;


class AgentModel {
    virtual void processState(vector<double> &state, vector<double> &output) = 0;
};

#endif
