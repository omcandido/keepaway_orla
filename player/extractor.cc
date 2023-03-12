#include <iostream>
#include <cstdlib>
#include <vector>
#include "BasicTrajectoryData.h"
#include "BasicModelExtractor.h"
#include "BasicValueExtractor.h"
#include "HeuristicEvaluator.h"
#include "ActionArgument.h"
#include "ArgumentFarKeeper.h"
#include "ArgumentOpenKeeper.h"
#include "ArgumentSmallestDistance.h"
#include "ArgumentSmallestAngle.h"
#include "ArgumentTackle.h"
#include "ArgumentT1H1.h"
#include "ArgumentT1H2.h"
#include "ArgumentT1H3.h"
#include "ArgumentT2H1.h"
#include "ArgumentT2H2.h"
#include "ArgumentT2H3_T3H1.h"
#include "ArgumentT2H4.h"
#include "ArgumentT3H2.h"
#include "ArgumentT3H3.h"

using namespace std;

void loadDefaultArguments(vector<unique_ptr<ActionArgument>>& allActionArguments);
void loadDefaultOrdering(Ordering& default_ordering);
void loadUniformOrdering(Ordering& default_ordering);

int numK = 4;
int numT = 3;
vector<string> teamCommState;
vector<string> teamCommArgs;
vector<string> savedOrderings;

vector<string> stateFilePaths;
vector<string> actionFilePaths;
int numAgents = 3;
int numStateVars = 17;
int numOutputVars = 4;

std::unique_ptr<BasicModelExtractor> modelExtractor;

int main( int argc, char * argv[] )
{
    for (int i=1; i<numAgents+1; i++){
        teamCommState.push_back("/home/candido/robocup/keepaway_orla/player/stateComm/agent_" + to_string(i) + ".txt");
        teamCommArgs.push_back("/home/candido/robocup/keepaway_orla/player/argComm/agent_" + to_string(i) + ".txt");
        savedOrderings.push_back("/home/candido/robocup/keepaway_orla/player/savedOrderings/agent_" + to_string(i) + ".txt");
        stateFilePaths.push_back("/home/candido/robocup/savedStates/sarsa_trained/agent_" + to_string(i) + ".txt");
        actionFilePaths.push_back("/home/candido/robocup/savedActions/sarsa_trained/agent_" + to_string(i) + ".txt");
    }

    // Create all arguments.
    vector<unique_ptr<ActionArgument>> allActionArguments;
    loadDefaultArguments(allActionArguments);

    // Create default ordering.
    Ordering default_ordering;

    if (argc > 1 && (string)argv[1] == "uniform"){
        cout << "Using uniform default ordering"<< endl;
        loadUniformOrdering(default_ordering);
        
    } else {
        cout << "Using default ordering given by domain expert (Gao et al.)"<< endl;
        loadDefaultOrdering(default_ordering);
    }

    cout << "Press any key to continue..."<< endl;
    cin.get();

    // Create a model extractor
    modelExtractor = std::make_unique<BasicModelExtractor>(
        numK, numT, teamCommState, teamCommArgs, allActionArguments, default_ordering
    );

    auto trajectoryData = std::make_unique<BasicTrajectoryData>(
        numAgents, numStateVars, numOutputVars, stateFilePaths, actionFilePaths
    );

    BasicTeamModel *teamModel = modelExtractor->extractModel(*trajectoryData);
    teamModel->saveAgentOrderings(savedOrderings);
    
    return 0;
}

void loadDefaultArguments(vector<unique_ptr<ActionArgument>>& allActionArguments) {
    int offset = 0;
    for (int agentID = 0; agentID < numT; agentID++) {
        allActionArguments.push_back(make_unique<ArgumentTackle>(offset++, agentID, 0, numT, numK));
        for (int i = 1; i < numK; i++) {
            // Create arguments and their values
            allActionArguments.push_back(make_unique<ArgumentOpenKeeper>(offset++, agentID, i, numT, numK));
            allActionArguments.push_back(make_unique<ArgumentFarKeeper>(offset++, agentID, i, numT, numK));
            allActionArguments.push_back(make_unique<ArgumentSmallestAngle>(offset++, agentID, i, numT, numK));
            allActionArguments.push_back(make_unique<ArgumentSmallestDistance>(offset++, agentID, i, numT, numK));
        }
    }

    // allActionArguments.push_back(make_unique<ArgumentT1H1>(offset++, 0, 0, numT, numK));
    // allActionArguments.push_back(make_unique<ArgumentT1H2>(offset++, 0, 0, numT, numK));
    // allActionArguments.push_back(make_unique<ArgumentT2H1>(offset++, 1, 2, numT, numK));
    // allActionArguments.push_back(make_unique<ArgumentT2H3_T3H1>(offset++, 1, 2, numT, numK));
    // allActionArguments.push_back(make_unique<ArgumentT2H3_T3H1>(offset++, 2, 3, numT, numK));
    // allActionArguments.push_back(make_unique<ArgumentT3H2>(offset++, 2, 3, numT, numK));
}


void loadDefaultOrdering(Ordering& default_ordering) {
    int offset = 0;
    for (int j = 0; j < numT; j++) {
        default_ordering.setValue({offset++, 5});
        for (int i = 1; i < numK; i++) {
            default_ordering.setValue({offset++, 2});
            default_ordering.setValue({offset++, 1});
            default_ordering.setValue({offset++, 4});
            default_ordering.setValue({offset++, 4});
        }
    }

    // default_ordering.setValue({offset++, 6});
    // default_ordering.setValue({offset++, 6});
    // default_ordering.setValue({offset++, 6});
    // default_ordering.setValue({offset++, 6});
    // default_ordering.setValue({offset++, 6});
    // default_ordering.setValue({offset++, 6});
}

void loadUniformOrdering(Ordering& default_ordering) {
    int offset = 0;
    for (int j = 0; j < numT; j++) {
        default_ordering.setValue({offset++, 0});
        for (int i = 1; i < numK; i++) {
            default_ordering.setValue({offset++, 0});
            default_ordering.setValue({offset++, 0});
            default_ordering.setValue({offset++, 0});
            default_ordering.setValue({offset++, 0});
        }
    }

    // default_ordering.setValue({offset++, 0});
    // default_ordering.setValue({offset++, 0});
    // default_ordering.setValue({offset++, 0});
    // default_ordering.setValue({offset++, 0});
    // default_ordering.setValue({offset++, 0});
    // default_ordering.setValue({offset++, 0});
}