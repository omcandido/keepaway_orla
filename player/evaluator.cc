#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "ActionArgument.h"
#include "ArgumentFarKeeper.h"
#include "ArgumentOpenKeeper.h"
#include "ArgumentSmallestAngle.h"
#include "ArgumentSmallestDistance.h"
#include "ArgumentTackle.h"
#include "BasicModelExtractor.h"
#include "BasicTrajectoryData.h"
#include "BasicValueExtractor.h"
#include "HeuristicEvaluator.h"

using namespace std;

void loadDefaultArguments(vector<unique_ptr<ActionArgument>>& allActionArguments);
void loadDefaultOrdering(Ordering& default_ordering);
bool readNextStep(vector<vector<double>> &agentStates,
                  vector<vector<double>> &agentActions,
                  vector<unique_ptr<ifstream>> &stateFileStreams,
                  vector<unique_ptr<ifstream>> &actionFileStreams,
                  int numStateVars,
                  int numOutputVars);
void processStates(HeuristicEvaluator &evaluator, int numAgents);

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

int main( int argc, char * argv[] )
{

    for (int i=1; i<numAgents+1; i++){
        teamCommState.push_back("/home/candido/robocup/keepaway_orla/player/stateComm/agent_" + to_string(i) + ".txt");
        teamCommArgs.push_back("/home/candido/robocup/keepaway_orla/player/argComm/agent_" + to_string(i) + ".txt");
        savedOrderings.push_back("/home/candido/robocup/keepaway_orla/player/savedOrderings/agent_" + to_string(i) + ".txt");
        stateFilePaths.push_back("/home/candido/robocup/savedStates/agent_" + to_string(i) + ".txt");
        actionFilePaths.push_back("/home/candido/robocup/savedActions/agent_" + to_string(i) + ".txt");
    }

    Ordering argOrdering;
    
    if (argc > 1 && (string)argv[1] == "default"){
        loadDefaultOrdering(argOrdering);
    } else {
        string pathOrd = "/home/candido//keepaway_orla/player/savedOrderings/agent_1.txt";
        argOrdering.loadOrdering(pathOrd);
    }

    // --- EVALUATION ---

    // Create all arguments.
    vector<unique_ptr<ActionArgument>> allActionArguments;
    loadDefaultArguments(allActionArguments);

    

    // Create evaluator.
    HeuristicEvaluator HE = HeuristicEvaluator(
        *make_unique<BasicValueExtractor>(numK),
        numT,
        allActionArguments
    );

    // Process states.

    processStates(HE, numT);
    HE.printStats();


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
}

// Reads the next set of states and actions for a group of agents
// from the given state/action filestreams
bool readNextStep(vector<vector<double>> &agentStates,
                  vector<vector<double>> &agentActions,
                  vector<unique_ptr<ifstream>> &stateFileStreams,
                  vector<unique_ptr<ifstream>> &actionFileStreams,
                  int numStateVars,
                  int numOutputVars) {
         
    double val = 0;

    for (int agentID = 0; agentID < stateFileStreams.size(); agentID++) {

        string state_line = "";

        // Stop reading when reached the end of at least one of the files
        if (!getline(*stateFileStreams.at(agentID), state_line)) {
            return false;
        }

        stringstream state_stream(state_line);

        for (int i = 0; i < numStateVars; i++) {
            state_stream >> val;
            agentStates.at(agentID).at(i) = val;
        }

        string action_line = "";

        if (!getline(*actionFileStreams.at(agentID), action_line)) {
            return false;
        }

        stringstream action_stream(action_line);

        for (int i = 0; i < numOutputVars; i++) {
            action_stream >> val;
            agentActions.at(agentID).at(i) = val;
        }
    }
    return true;
}

void processStates(HeuristicEvaluator &evaluator, int numAgents) {

    // Create streams for reading trajectories for every agent
    vector<unique_ptr<ifstream>> stateFileStreams;
    vector<unique_ptr<ifstream>> actionFileStreams;

    for (int i = 0; i < stateFilePaths.size(); i++) {
        stateFileStreams.push_back(make_unique<ifstream>(stateFilePaths.at(i), ios::in));
        actionFileStreams.push_back(make_unique<ifstream>(actionFilePaths.at(i), ios::in));
    }

    // Incrementally process trajectories 
    vector<vector<double>> agentStates(numAgents);
    vector<vector<double>> agentActions(numAgents);

    for (int i = 0; i < numAgents; i++) {
        agentStates.at(i).resize(numStateVars);
        agentActions.at(i).resize(numOutputVars);
    }

    while (readNextStep(agentStates, agentActions, stateFileStreams, 
    actionFileStreams, numStateVars, numOutputVars)) {
        
        evaluator.processStates(agentStates, agentActions);
    }
}