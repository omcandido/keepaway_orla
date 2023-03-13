#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include "BasicAgentModel.h"
#include "BasicModelExtractor.h"
#include "ActionArgument.h"
#include "ArgumentationFramework.h"
#include "ActionArgumentAFConstructor.h"
#include "VAFConstructor.h"
#include "Ordering.h"
#include "GESolver.h"
#include "IOUtils.h"


BasicAgentModel::BasicAgentModel(
        int _agentID,
        vector<int> &_teamIDs,
        string &_agentStateFile,
        string &_agentArgFile,
        vector<string> &_teamStateFiles,
        vector<string> &_teamArgFiles,
        const vector<unique_ptr<ActionArgument>> &_actionArgsTable,
        unique_ptr<BasicValueExtractor> _myConverter,
        Ordering &_argOrdering) :

        agentID(_agentID), teamIDs(_teamIDs), agentStateFile(_agentStateFile),
        agentArgFile(_agentArgFile), teamStateFiles(_teamStateFiles),
        teamArgFiles(_teamArgFiles), actionArgsTable(), 
        myConverter(move(_myConverter)), argOrdering(_argOrdering) {


    for (int i = 0; i < _actionArgsTable.size(); i++) {
        unique_ptr<ActionArgument> argPtr = _actionArgsTable.at(i)->clone();
        actionArgsTable.push_back(move(argPtr));
    }
}


void BasicAgentModel::processState(vector<double> &old_state, vector<double> &output) {


    // std::cout << " -- START old_state: ";
    //     for (auto i: old_state)
    //       std::cout << i << ' ';
    //     std::cout << " FINISH -- ";

    vector<double> state(old_state);

    // Send agent state information to other agents
    writeData(state);

    // Receive agent state information from other agents
    readData(state);


    // std::cout << " -- START state: ";
    // for (auto i: state)
    //     std::cout << i << ' ';
    // std::cout << " FINISH -- \n";

    // Not all values read
    if (state.size() < 25) {
        // cout << agentID << " " << "didn't read data" << endl;
        output.push_back(-2);
        return;
    }

    // Compute applicable arguments for the agent
    set<int> applicableArgs;

    for (int i = 0; i < actionArgsTable.size(); i++) {
        if ((actionArgsTable.at(i)->getAgentID()==agentID) &&
            (actionArgsTable.at(i)->isApplicable(state))) { 

            applicableArgs.insert(i);
        }
    }   

    // Save arguments
    IOUtils::saveActionArguments(agentArgFile, applicableArgs);

    // Load other agent arguments
    for (int i = 0; i < teamIDs.size(); i++) {
        IOUtils::readActionArguments(teamArgFiles.at(i), applicableArgs);
    }

    ArgumentationFramework af;
    ActionArgumentAFConstructor::constructAF(applicableArgs, actionArgsTable, af);
    
    ArgumentationFramework vaf;
    VAFConstructor::constructVAF(af, argOrdering, vaf);

    // std::cout << " -- START ordering: ";
    // for (auto &i: argOrdering.values)
    //     std::cout << i.first << ":" << i.second << ", ";
    // std::cout << " FINISH -- \n";    

    GESolver solver;
    set<set<int>> extensions;
    int defaultAction = -1;
    solver.solve(vaf, extensions);


    for (auto groundedExtension : extensions) {
        output.push_back(getActionFromGE(defaultAction, groundedExtension));
        // cout << "T" << agentID << ": " << getActionFromGE(defaultAction, groundedExtension) << "\n";
        return;
    }    
}


int BasicAgentModel::getActionFromGE(int defaultAction,  set<int> &groundedExtension) {

    int recAction = defaultAction;


    // std::cout << " -- GE ";
    // for (auto& i: groundedExtension)
    //   std::cout << i << ' ';
    // std::cout << " GE --\n ";
    
    for (auto recArgID : groundedExtension) {
        if (actionArgsTable.at(recArgID)->getAgentID() == agentID) {

            // ORLA: log which argument is used during the matches.
            ofstream myFile("/home/candido/robocup/keepaway_orla/player/actionsTaken/actionsTaken.txt", std::ios_base::app);
            myFile << recArgID << "\n";
            myFile.close();

            recAction = actionArgsTable.at(recArgID)->getConclusion();
            break;
        }            
    } 
    return recAction;
}


void BasicAgentModel::writeData(vector<double> &state) {
    vector<double> values;
    myConverter->convert(state, values);
    IOUtils::saveValues(agentStateFile, values);
}

void BasicAgentModel::readData(vector<double> &values) {
    for (int i = 0; i < teamStateFiles.size(); i++) {
        IOUtils::loadValues(teamStateFiles.at(i), values);
    }
}

int BasicAgentModel::getNumArgs() {
    return actionArgsTable.size();
}



void BasicAgentModel::saveOrdering(string &outputPath) {
    argOrdering.saveOrdering(outputPath);
}