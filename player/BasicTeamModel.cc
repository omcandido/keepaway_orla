#include "BasicTeamModel.h"

using namespace std;


void BasicTeamModel::addAgent(unique_ptr<BasicAgentModel> agent) {
    agents.push_back(move(agent));
}

void BasicTeamModel::processStates(vector<vector<double>> &states, 
                                   vector<vector<double>> &output) {

    for (int i = 0; i < states.size(); i++) {
        vector<double> agentOutput;
        agents.at(i)->processState(states.at(i), agentOutput);
        output.push_back(agentOutput);
    }
}

void BasicTeamModel::saveAgentOrderings(vector<string> &outputPaths) {

    for (int i = 0; i < agents.size(); i++) {
        agents.at(i)->saveOrdering(outputPaths.at(i));
    } 
}
