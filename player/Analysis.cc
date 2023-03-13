
#include "Analysis.h"
#include <math.h>

typedef std::pair<int, double> pair;

unique_ptr<BasicAgentModel> Analysis::agent;
int Analysis::n_taker_features = 0;
int Analysis::numEpisodes = 0;
double Analysis::numMatches = 0;
double Analysis::numMismatches = 0;
int Analysis::agentID = 0;
int Analysis::prevAction = 0;

bool Analysis::isNewEpisode(double state[]) {

    for (int i = 0; i < MAX_STATE_VARS; i++) {
        if (state[i] != -1) {
            return false;
        }
    }
    return true;
}

void Analysis::newEpisode(int agentID) {

    double defaultState[MAX_STATE_VARS];

    for (int i = 0; i < MAX_STATE_VARS; i++) {
        defaultState[i] = -1;
    }
    saveState(defaultState, agentID);
}

void Analysis::saveAction(int action, int agentID) {

    // std::string full_path = "/home/dmitrykazhdan/Part-III-Project/savedActions/agent_" +
    std::string full_path = "/home/candido/robocup/keepaway_orla/player/savedActions/agent_" +
    std::to_string(agentID) + ".txt";

    std::ofstream file(full_path, std::ios_base::app);

    if (file.is_open()) {
        file << action << std::endl;
        file.close();

    } else {
        std::cout << "Could not open file..." << std::endl;
    }

}

void Analysis::saveState(double state[], int agentID) {

    // std::string full_path = "/home/dmitrykazhdan/Part-III-Project/savedStates/agent_" +
    // std::string full_path = "/home/dmitrykazhdan/project/savedStates/agent_" +
    std::string full_path = "/home/candido/robocup/keepaway_orla/player/savedStates/agent_" +
    std::to_string(agentID) + ".txt";
    
    std::ofstream file(full_path, std::ios_base::app);

    if (file.is_open()) {
        for (int i = 0; i < MAX_STATE_VARS; i++) {
            file << state[i] << " ";
        }
        file << std::endl;
        file.close();

    } else {
        std::cout << "Could not open file..." << std::endl;
    }
}


// std::string Analysis::stateFilePath = "/home/dmitrykazhdan/Part-III-Project/savedStates/agent_";
// std::map<int, LinearSarsaAgent*> Analysis::agents;
// double Analysis::prevState[MAX_STATE_VARS];
// int Analysis::prevAction = 0;
// double Analysis::prevQ = 0;
// std::vector<int> Analysis::new_episodes;

// int Analysis::n_print_lines =  400;


// void Analysis::processFiles() {

//     for (int i = 0; i < MAX_STATE_VARS; i++) {
//         prevState[i] = 0;
//     }

//     std::vector<std::vector<int>> allFeatures;

//     for (int i = 1; i <= agents.size(); i++) {
//         std::vector<int> features;
//         processFile(agents.at(i)->getAgentID(), features);
//         allFeatures.push_back(features);
//     }


//     int factor = allFeatures.at(0).size() / n_print_lines;

//     std::cout << "Factor: " << factor << std::endl;


//     for (int i = 0; i < n_print_lines; i++) {
//         for (int j = 1; j < 2; j++) {

//             // if (allFeatures.at(j).at(factor*i + 1) < 20) {
//             //     continue;
//             // }

//             bool newEpisode = true;

//             for (int k = 0; k < factor; k++) {
//                 if (allFeatures.at(j).at(factor*i + k) != -1) {
//                     newEpisode = false;
//                     break;
//                 }
//             }

//             if (newEpisode) {
//                 std::cout << " ----------------------- "  << std::endl;
//                 continue;
//             }
            

//             for (int k = 0; k < factor; k++) {
//                 std::cout << allFeatures.at(j).at(factor*i + k) << ",";
//             }

//             std::cout << "    ";

//             // std::cout << allFeatures.at(j).at(3*i) << "   " 
//             // << "(" << allFeatures.at(j).at(3*i+1) << ", " <<
//             // allFeatures.at(j).at(3*i+2)
//             // << ")   ";
//         }
//         std::cout << std::endl;
//     }
//     std::cout << std::endl;

// }

// void Analysis::processFile(int agentID) {

//     std::vector<int> features;
//     processFile(agentID, features);

//     for (int i = 0; i < n_print_lines; i++) {
//         std::cout << features.at(3*i) 
//         << "    (" << features.at(3*i + 1)
//         << ", " << features.at(3*i + 2) << ")" <<
//         std::endl;
//     }
// }


// void Analysis::processFile(int agentID, std::vector<int> &features) {

//     std::cout << "Processing agent " << agentID << " ... " << std::endl << std::endl;

//     double state[MAX_STATE_VARS];
//     std::string full_path = Analysis::stateFilePath + std::to_string(agentID) + ".txt";
//     std::ifstream file(full_path, std::ios::in);
//     double val;
//     std::string line;
//     std::vector<double> differences;

//     int n_lines = n_print_lines;
//     int line_count = 0;

//     while (std::getline(file, line) && line_count < n_lines) {

//         line_count++;

//         std::stringstream line_stream(line);

//         for (int i = 0; i < MAX_STATE_VARS; i++) {
//             line_stream >> val;
//             state[i] = val;
//         }

//         if (isNewEpisode(state) && line_count > 2) {
//             int factor = features.size() / (line_count - 1);

//             for (int i = 0; i < factor; i++) {
//                 features.push_back(-1);
//             }
//             continue;
//         }


//         extractFeatures(state, features, *agents[agentID]);

//         // stateDifference(state, differences);
//         updatePrevState(state);

//         // for (int i = 0; i < 17; i++) {
//         //     std::cout << differences.at(i) << " ";
//         // }
//         // std::cout << std::endl;
//         // differences.clear();

//     }
//     file.close();
// }


// void Analysis::runAnalysis(double state[]) {

//     std::vector<int> features;

//     int action = agent.step(0.0, state);

//     extractFeatures(state, features);

//     std::cout << std::endl;

//     for (int i = 0; i < features.size(); i++) {
//         std::cout << features.at(i) << " " ;
//     }
//     std::cout << std::endl << "Printed features..." << std::endl;
// }


// Try Q-value entropy?
// Try "multi-agent" petrurbation (perturb features for several agents)
// Try seeing Q-value for a distribution of feature values
// Try accident: use trajectory of agent 1 to evaluate agent 2

// void Analysis::extractFeatures(double state[], std::vector<int> &features, LinearSarsaAgent &agent) {

//     int action = agent.step(0.0, state);

//     for (int i = 0; i < 4; i++) {
//         double q = agent.getQ(i);
//         features.push_back(q);
//     }

//  //   std::cout << action << std::endl;

//     action = 0;

//     double oldQ = agent.getQ(action);
//     double oldQOldA = agent.getQ(prevAction);
//     double newQ = 0;
//     double newQOldA = 0;

//     double pertrubedState[MAX_STATE_VARS];
//     int newAction = 0;
//     double maxDiff = -1;
//     double diff = 0;
//     int topFeature = -1;
//     int topAction = -1;

//     std::vector<pair> differences;
    

// //    std::cout << "True action: " << action << std::endl;

//     for (int i = 0; i < MAX_STATE_VARS; i++) {

//         for (int j = 0; j < MAX_STATE_VARS; j++) {
//             pertrubedState[j] = state[j];
//         }

//         pertrubedState[i] = prevState[i];

//         newAction = agent.step(0.0, pertrubedState);
//         newQ = agent.getQ(action);
// //        newQOldA = agent.getQ(prevAction);

//         diff = fabs(oldQ - newQ);
//        // diff = fabs(oldQ - newQ) + fabs(oldQOldA - newQOldA);
//        // diff = fabs(oldQOldA - oldQ) + fabs(newQ - newQOldA);

//         differences.push_back({i+1, diff});


//         // if (diff > maxDiff) {
//         //     maxDiff = diff;
//         //     topFeature = i + 1 ;
//         //     topAction = newAction;
//         // } 
//     }

// 	std::sort(differences.begin(), differences.end(),
// 			[](const pair& l, const pair& r) {
// 				if (l.second != r.second)
// 					return l.second > r.second;

// 				return l.first > r.first;
// 			});


//     features.push_back(fabs(oldQ - prevQ));

// //    if (prevAction == 0 && action == 2) {

//         for (int k = 0; k < 1; k++) {
//             features.push_back(differences.at(k).second);
//             features.push_back(differences.at(k).first);
// //            features.push_back(state[differences.at(k).first]);
//         }

//        // features.push_back(topFeature);

// //        features.push_back(prevAction);
// //        features.push_back(action);
//  //   }

//     prevAction = action;
//     prevQ = oldQ;




//     // for (int i = 0; i < MAX_STATE_VARS; i++) {
//     //     pertrubedState[i] = state[i];
//     // }

//     // bool atLeastOnce = false;

//     // for (int i = 0; i < MAX_STATE_VARS; i++) {
        
//     //     pertrubedState[i] = prevState[i];
//     //     newAction = agent.step(0.0, pertrubedState);

//     //     // if (newAction != action) {
//     //     if (newAction == 0 && action == 2) {
//     //         std::cout << i + 1 << " ";
//     //         atLeastOnce = true;
//     //     }
//     //     pertrubedState[i] = state[i];
//     // }

//     // for (int i = 0; i < MAX_STATE_VARS; i++) {
//     //     if (pertrubedState[i] == state[i]) {
//     //         std::cout << i << " ";
//     //     }
//     // }

//     // if (atLeastOnce) {
//     //     std::cout << std::endl;
//     // }
// }

// bool Analysis::pertrubState(double inputState[], int feature, int action) {


//     agent.step(0.0, inputState);
//     return false;

//     // Copy state





//     // if (newAction != action) {
//     //     std::cout << newAction << " ";
//     //     return true;
//     // } else {
//     //     return false;
//     // }
// }


// void Analysis::stateDifference(double newState[], std::vector<double> &differences) {

//     double diff;

//     for (int i = 0; i < MAX_STATE_VARS; i++) {
//         diff = fabs(prevState[i] - newState[i]) / newState[i];
//         differences.push_back(diff);
//     }
// }

// void Analysis::updatePrevState(double newState[]) {
//     for (int i = 0; i < MAX_STATE_VARS; i++) {
//         prevState[i] = newState[i];
//     }
// }


/*
States:


me dist to k1, k2, ..., kN      [1-5]    [1-4]

me dist to t2, ..., tN          [6-7]    [5-6]

me ang to k2, ..., kN           [8-11]   [7-9]

k1 dist to k2, ..., kN          [12-15]  [10-12]

k1 dist to t2, ..., tN          [16-17]  [13-14]

min ang k2, ..., kN to takers   [18-21]  [15-17]



*/




