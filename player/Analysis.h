
#include "LinearSarsaAgent.h"
#include <vector>
#include<map>
#include <iostream>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "BasicAgentModel.h"
#include <memory>

class Analysis {
public:
    // static void runAnalysis(double state[]);
    // static void pertrubState(double inputState[], double pertrubedState[]);
    // static void extractFeatures(double state[], std::vector<int> &features, LinearSarsaAgent &agent);
    // static bool pertrubState(double inputState[], int feature, int action);
    // static void sampleRun();
    // static void processFile(int agentID, std::vector<int> &features);
    // static void processFile(int agentID);
    // static void processFiles();
    // static std::map<int, LinearSarsaAgent*> agents;
    // static std::string stateFilePath;
    // static void loadState(double state[], int agentID);
    // static int n_print_lines;
    // static void stateDifference(double newState[], std::vector<double> &differences);
    // static double prevState[MAX_STATE_VARS];
    // static int prevAction;
    // static double prevQ;
    // static void updatePrevState(double newState[]);
    // static std::vector<int> new_episodes;


    static int agentID;
    static unique_ptr<BasicAgentModel> agent;
    static int n_taker_features;
    static void newEpisode(int agentID);
    static bool isNewEpisode(double state[]);
    static void saveState(double state[], int agentID);
    static void saveAction(int action, int agentID);
    static double numMatches;
    static double numMismatches;
    static int numEpisodes;
    static int prevAction;
};