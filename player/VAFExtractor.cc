/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VAFExtractor.cc
 * Author: dmitrykazhdan
 * 
 * Created on 02 February 2019, 21:25
 */

#include "VAFExtractor.h"
#include "ApplicableArguments.h"


int VAFExtractor::n_args = 0;
int VAFExtractor::agentID = -1;
int VAFExtractor::numMismatch=0;
int VAFExtractor::numMatches=0;
std::vector<std::vector<int>> VAFExtractor::attackVals;

//string VAFExtractor::orderingPath = "/home/dk525/Part-III-Project/robocup/ordering_";
// string VAFExtractor::orderingPath = "/home/dmitrykazhdan/Part-III-Project/robocup/ordering_";
string VAFExtractor::orderingPath = "/home/candido/robocup/ordering_";

// Initialize inequality counts to 0
void VAFExtractor::initialize() {

    n_args = PotentialGenerator::allArguments.size();

    for (int i = 0; i < n_args; i++) {
        std::vector<int> new_vec;
        attackVals.push_back(new_vec);
        attackVals[i].resize(n_args, 0);
    }
}

int VAFExtractor::getRecommendedAction(double state[]){ 

    int action = -1;
    double potentialTable[KeeperStateCopy::numKeepers]; 

    PotentialGenerator::getPotential(VAFExtractor::agentID, state, potentialTable);

    for (int i = 0; i < KeeperStateCopy::numKeepers; i++){
        if (potentialTable[i] != 0) {
            action = i;
            break;
        }
    } 
    return action;
}

void VAFExtractor::processState(double state[], int agentAction) {
    
    // if (!KeeperStateCopy::validState) {
    //     return;
    // }

    int n_actions;
        
    ApplicableArguments applicableArgs(VAFExtractor::agentID);
    PotentialGenerator::getApplicableArguments(state, applicableArgs);
    int argID = 0;
 
 //   double potentialTable[KeeperStateCopy::numKeepers];
    // PotentialGenerator::getPotential(VAFExtractor::agentID, state, potentialTable);
    // int recAction = -1;

    // for (int i = 0; i < KeeperStateCopy::numKeepers; i++) {
    //     if (potentialTable[i] > 0) {
    //         recAction = i;
    //         break;
    //     }
    // }

    // if (recAction != -1) {
    //     if (recAction != agentAction){
    //         VAFExtractor::numMismatch++;
    //     } else {
    //         VAFExtractor::numMatches++;
    //     }
    // }


    std::vector<int> relevantArgIDs;
    std::vector<int> irrelevantArgIDs;

    for (int i = 0; i < applicableArgs.getNumArgs(); i++) {
        if (PotentialGenerator::allArguments.at(applicableArgs.getArgID(i))->getConclusion() == agentAction) {
            relevantArgIDs.push_back(applicableArgs.getArgID(i));
        } else {
            irrelevantArgIDs.push_back(applicableArgs.getArgID(i));
        }
    }
    

    for (int i = 0; i < relevantArgIDs.size(); i++) {
        for (int j = 0; j < irrelevantArgIDs.size(); j++) {
            VAFExtractor::attackVals.at(relevantArgIDs.at(i)).at(irrelevantArgIDs.at(j))++;
        }
    }
    
}

void VAFExtractor::print() {

    std::cout << "No. matches: " << VAFExtractor::numMatches << std::endl <<
    "No. mismatches: " << VAFExtractor::numMismatch << std::endl <<
    " \% Match: " << 
    ((float) VAFExtractor::numMatches) / (VAFExtractor::numMatches + VAFExtractor::numMismatch) << std::endl <<
    "Agent ID: " << VAFExtractor::agentID << std::endl << std::endl;

}



void VAFExtractor::extractAndSaveOrdering() {

    std::map<int, std::set<int>> incomingEdges;
    std::map<int, std::set<int>> outgoingEdges;

    for (int i = 0; i < n_args; i++) {
        std::set<int> empty;
        incomingEdges.insert({i, empty});
        outgoingEdges.insert({i, empty});
    }

    int v1 = 0;
    int v2 = 0;
    int preference = 0;

    for (int i = 0; i < n_args; i++) {
        for (int j = i+1; j < n_args; j++) {
        
            v1 = VAFExtractor::attackVals.at(i).at(j);
            v2 = VAFExtractor::attackVals.at(j).at(i);
            preference = v1 - v2;

            if (preference > 100) {

                incomingEdges[j].insert(i);
                outgoingEdges[i].insert(j);

            } else if (preference < -100) {

                incomingEdges[i].insert(j);
                outgoingEdges[j].insert(i);
            }

        }
    }


    std::vector<int> orderedList;
    std::set<int> freeNodes;

    for (int i = 0; i < n_args; i++) {
        if (incomingEdges[i].size() == 0) {
            freeNodes.insert(i);
        }
    }

    int nextNode;

    while (freeNodes.size() != 0 ) {

        nextNode = VAFExtractor::selectNextNode(freeNodes);
        freeNodes.erase(nextNode);
        orderedList.push_back(nextNode);

        for (auto dstNode : outgoingEdges[nextNode]) {
            
            incomingEdges[dstNode].erase(nextNode);

            if (incomingEdges[dstNode].size() == 0) {
                freeNodes.insert(dstNode);
            }
        }
    }


    if (orderedList.size() < n_args) {
        std::cout << "Graph contains cycles..." << std::endl;
    } else {
        std::cout << "Sorted successfully..." << std::endl;
        VAFExtractor::saveOrdering(orderedList);
        std::cout << "Ordering saved successfully..." << std::endl;
    }
}



int VAFExtractor::selectNextNode(std::set<int> &freeNodes) {

    int maxVal = -1;
    int maxIndex = -1;

    for (auto argID : freeNodes) {

        if (PotentialGenerator::valueOrdering[argID] > maxVal) {
            maxIndex = argID;
            maxVal = PotentialGenerator::valueOrdering[argID];
        }

    }
    return maxIndex;
}





void VAFExtractor::saveOrdering(std::vector<int> &ordering) {

    std::string full_path = VAFExtractor::orderingPath + std::to_string(VAFExtractor::agentID) + ".txt";

    ofstream file(full_path);

    if (file.is_open()) {
        for (int i = 0; i < ordering.size(); i++) {
            file << ordering.at(i) << std::endl;
        }
        file.close();

    } else {
        std::cout << "Could not open file..." << std::endl;
    }
}





void VAFExtractor::loadOrdering(std::vector<int> &ordering) {

   std::string full_path = VAFExtractor::orderingPath + std::to_string(VAFExtractor::agentID) + ".txt";

    ifstream file(full_path, ios::in);
    int val;

    while (file >> val) {
        ordering.push_back(val);
    }
    file.close();
}