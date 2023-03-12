#ifndef BasicTeamModel_H
#define BasicTeamModel_H

#include <vector>
#include <memory>
#include "ExtractedTeamModel.h"
#include "BasicAgentModel.h"

using namespace std;

class BasicTeamModel : public ExtractedTeamModel {

public:
    void addAgent(unique_ptr<BasicAgentModel> agent);
    void processStates(vector<vector<double>> &states, vector<vector<double>> &outputs);
    void saveAgentOrderings(vector<string> &outputPaths);
    vector<unique_ptr<BasicAgentModel>> agents;
};


#endif