/* 
 * File:   PotentialGenerator.cc
 * Author: dmitrykazhdan
 * 
 * Created on 19 December 2018, 19:58
 */
#include "PotentialGenerator.h"
#include "VAFExtractor.h"
#include "RecommendationLogger.h"


//string PotentialGenerator::argumentsPath ="/home/dk525/Part-III-Project/agentArgs/";
// string PotentialGenerator::argumentsPath = "/home/dmitrykazhdan/Part-III-Project/agentArgs/";
string PotentialGenerator::argumentsPath = "/home/candido/agentArgs/";
vector<std::unique_ptr<ActionArgument>> PotentialGenerator::allArguments;
map<int, int> PotentialGenerator::valueOrdering;
double PotentialGenerator::potentialVal = 2;


void PotentialGenerator::updatePotentialVal() {
    double factor = 0.99;
    PotentialGenerator::potentialVal = PotentialGenerator::potentialVal * factor;
}


void PotentialGenerator::getApplicableArguments(double state[], ApplicableArguments &myAppArgs) {
    
    // Given a state, return arguments applicable in that state 
    for (unsigned int i = 0; i < allArguments.size(); i++) {
        if (allArguments.at(i)->isApplicable(state)) {
            myAppArgs.addArgID(i);
        }
    }
}

void PotentialGenerator::saveArguments(ApplicableArguments &applicableArgs, const string &filename) {
    // Write new file contents
    ofstream ofs;
    ofs.open(filename, ofstream::out | ofstream::trunc);
    
    for (int i = 0; i < applicableArgs.getNumArgs(); i++) {
        // Write argument to file 
        ofs << applicableArgs.getArgID(i) << endl;
    }    
    ofs.close();
}
    
void PotentialGenerator::loadArguments(int agentID, vector<ApplicableArguments> &applicableArgs) {
    
    // Load a set of arguments from files
    DIR* dirp = opendir(argumentsPath.c_str());
    struct dirent *dp;
    string line;
    int arg;
    string ignore_file = to_string(agentID) + ".txt";
    string full_path = "";
    bool is_parent_dir = false;
    bool is_current_dir = false;
    bool is_same_agent_file = false;
    
    while ((dp = readdir(dirp)) != NULL) {
        
        // Ignore the agent's own file, 
        // as well as the parent and current directory entries
        is_same_agent_file = strcmp(dp->d_name, ignore_file.c_str());
        is_current_dir = strcmp(dp->d_name, ".");
        is_parent_dir = strcmp(dp->d_name, "..");
        
        if ((!is_same_agent_file) && (!is_parent_dir) && (!is_current_dir)) {
            
            // Extract ID of agent from filename
            string curAgent(dp->d_name);            
            curAgent.erase(curAgent.length() - 4);
            int curAgentID = atoi(curAgent.c_str());
            
            // Read in agent arguments from file
            ApplicableArguments applicableArg(curAgentID);
            
            full_path = argumentsPath + dp->d_name;
            ifstream ifs(full_path);
            while (getline(ifs, line)) {
                std::istringstream iss(line);   
                iss >> arg;
                applicableArg.addArgID(arg);
            }
            applicableArgs.push_back(applicableArg);
        };       
    }
    closedir(dirp); 
}
   

// TODO: current implementation very inefficient
// Re-arrange/re-write to improve current approach (when you're confident the code works)   

void PotentialGenerator::getPotential(int agentID, double state[], double potentialTable[]) {        
 
    RecommendationLogger::numRequests++;
        
    // Initialize potential table to all zeros
    for (int i = 0; i < KeeperStateCopy::numKeepers; i++) {
        potentialTable[i] = 0;
    }


    // Tmp
    //return;

    // Exit if keeper state was not retreived 
    if (!KeeperStateCopy::validState) {
        return;
    }
    
    // Temporary Timing code
    // clock_t start, end;
    // start = clock();
    // if (agentID == 1) {
    //     std::cout << "Computing GP..." << std::endl;   
    // }

    // Retrieve applicable arguments of the other agents
    vector<ApplicableArguments> applicableArguments;
    loadArguments(agentID, applicableArguments);
    
    // Retrieve applicable arguments of the agent
    ApplicableArguments myArguments(agentID);
    getApplicableArguments(state, myArguments);

    // Save arguments to agent file
    string filename = PotentialGenerator::argumentsPath + to_string(agentID) + ".txt";
    saveArguments(myArguments, filename);
    
    // Combine all applicable arguments together
    applicableArguments.push_back(myArguments);
            
    // Fill the table with potential values
    computePotential(agentID, applicableArguments, potentialTable);
    
    // Temporary measure end time
    // end = clock();
    // double duration_sec = double(end-start)/CLOCKS_PER_SEC;
    // if (agentID == 1) {
    //     std::cout << "Elapsed time: " << duration_sec << std::endl;
    // }

    
    // Check if at least one recommendation has been made
    for (int i = 0; i < KeeperStateCopy::numKeepers; i++) {
        if (potentialTable[i] != 0) {
            RecommendationLogger::numRecommendations++;
            break;
        }
    }    
}



void PotentialGenerator::computePotential(int agentID, vector<ApplicableArguments> &applicableArguments,double potentialTable[]) {

    // Flatten set of applicable arguments
    set<Argument> arguments;
    for (ApplicableArguments appArg : applicableArguments) {
        for (int i = 0; i < appArg.getNumArgs(); i++) {
            Argument arg(appArg.getAgentID(), appArg.getArgID(i));
            arguments.insert(arg);
        }
    } 

    // Compute VSCAF
    std::map<Argument, set<Argument>> VSCAF;
    computeVSCAF(arguments, VSCAF);

    // Compute reverse of VSCAF
    std::map<Argument, set<Argument>> isAttackedBy;
    computeIsAttackedBy(VSCAF, isAttackedBy);
    


    // if (agentID == 1) {
    //     std::map<Argument, set<Argument>>::iterator it;
    //     std::cout << "Printing VSCAF..." << std::endl;
    //     for (it = VSCAF.begin(); it != VSCAF.end(); it++) {

    //         std::cout << "Argument " << (it->first).getArgumentID() << "  attacks: ";

    //         for (auto it2 : it->second) {
    //             std::cout << it2.getArgumentID() << " ";
    //         }
    //         std::cout << std::endl;
    //     }

    //     std::cout << "Printing isAttackedBy..." << std::endl;
    //     for (it = isAttackedBy.begin(); it != isAttackedBy.end(); it++) {

    //         std::cout << "Argument " << (it->first).getArgumentID() << "  attacks: ";

    //         for (auto it2 : it->second) {
    //             std::cout << it2.getArgumentID() << " ";
    //         }
    //         std::cout << std::endl;
    //     }


    // }




    // Compute the grounded extension from the mapping
    set<Argument> groundedExtension;
    computeGroundedExtension(VSCAF, isAttackedBy, groundedExtension);
    int recAction = 0;
    
    // Set potential values of recommended actions
    for (Argument recArg : groundedExtension) {
        if (recArg.getAgentID() == agentID) {

            recAction = allArguments.at(recArg.getArgumentID())->getConclusion();
            potentialTable[recAction] = PotentialGenerator::potentialVal;

            //if (agentID == 1) {
                // std::cout << "Recommended argument: " << recArg.getArgumentID() 
                
                // << " (id: " << agentID << " )" << std::endl;
                
                // std::cout << "Recommended action: " << recAction << " (id: " 
                // << agentID << " )" << std::endl;
            //}
        }            
    } 

    // if (groundedExtension.size() == 0) {
    //     std::cout << " No action recommended  (id: " << agentID << " )" << std::endl;
    // }
    // std::cout << std::endl;   
}


void PotentialGenerator::computeVSCAF(set<Argument> &arguments, std::map<Argument, set<Argument>> &VSCAF) {
    
    for (Argument arg : arguments) {
        set<Argument> argSet;
        VSCAF.insert({arg, argSet});
    }
    
    
    // Fill VSCAF with appropriate edges
    int ID1 = 0;
    int ID2 = 0;
    int action1 = 0;
    int action2 = 0;
    bool sameAgent = false;
    bool sameAction = false;
    
    for (Argument arg1 : arguments) {
        for (Argument arg2 : arguments) {

            ID1 = arg1.getAgentID();
            ID2 = arg2.getAgentID();
            action1 = allArguments.at(arg1.getArgumentID())->getConclusion();
            action2 = allArguments.at(arg2.getArgumentID())->getConclusion();
            sameAgent = (ID1 == ID2);
            sameAction = (action1 == action2);
            
            if ((sameAgent && !sameAction) || (!sameAgent && sameAction)) {
                if (valueOrdering[arg1.getArgumentID()] >= valueOrdering[arg2.getArgumentID()]) {
                    VSCAF[arg1].insert(arg2);
                }
                
                if (valueOrdering[arg2.getArgumentID()] >= valueOrdering[arg1.getArgumentID()]) {
                    VSCAF[arg2].insert(arg1);
                }                              
            }
        }
    }
}


void PotentialGenerator::computeIsAttackedBy(std::map<Argument, set<Argument>> &VSCAF, std::map<Argument, set<Argument>> &isAttackedBy) {
    
    // Create empty map
    for(map<Argument,set<Argument>>::iterator it = VSCAF.begin(); it != VSCAF.end(); it++) {
        set<Argument> argSet;
        isAttackedBy.insert({it->first, argSet});
    }

    // Fill in mapping
    for(map<Argument,set<Argument>>::iterator it = VSCAF.begin(); it != VSCAF.end(); it++) {
        Argument attackingArg = it->first;
        for (Argument attackedArg : it->second) {
            isAttackedBy[attackedArg].insert(attackingArg);
        }
    }
}



// Potential optimisation: know that as soon as at least one argument
// of agent is in GE - can recommend the action.

void PotentialGenerator::computeGroundedExtension(std::map<Argument, set<Argument>> &attacks, std::map<Argument, set<Argument>> &isAttackedBy, set<Argument> &groundedExtension) {
    
    set<Argument> unmarkedArgs;
    set<Argument> deletedArgs;

    // Set all arguments as unmarked
    for(map<Argument,set<Argument>>::iterator it = attacks.begin(); it != attacks.end(); it++) {
        unmarkedArgs.insert(it->first);
    }


    bool changed = true;

    while (changed) {
        
        changed = false;
        deletedArgs.clear();
        
        for(auto itr = unmarkedArgs.begin(); itr != unmarkedArgs.end();) {
            if (isAttackedBy[*itr].size() == 0) {
                
                groundedExtension.insert(*itr);
               
                for (Argument arg : attacks[*itr]) {
                    deletedArgs.insert(arg);
                }
                
                itr = unmarkedArgs.erase(itr);
                changed = true;
            } else {
                ++itr;
            }
        }
        
        if (deletedArgs.size() > 0) {
            for (Argument deletedArg : deletedArgs) {
                unmarkedArgs.erase(deletedArg);            
            }

            for (auto it = isAttackedBy.begin(); it != isAttackedBy.end(); it++) {
                for (Argument deletedArg : deletedArgs) {
                    it->second.erase(deletedArg);                    
                }
            }  
        }
    }  
}


void PotentialGenerator::initialize() {
    

    allArguments.clear();
    valueOrdering.clear();

    int numK = KeeperStateCopy::numKeepers;
    int offset = 0;
    
    allArguments.push_back(std::make_unique<ArgumentTackle>(0));
    valueOrdering.insert({offset++, 5});

    for (int i = 1; i < numK; i++) {
        // Create arguments and their values
        allArguments.push_back(std::make_unique<ArgumentOpenKeeper>(i));
        valueOrdering.insert({offset++, 4});
         
        allArguments.push_back(std::make_unique<ArgumentFarKeeper>(i));
        valueOrdering.insert({offset++, 2});
        
        allArguments.push_back(std::make_unique<ArgumentSmallestAngle>(i));
        valueOrdering.insert({offset++, 1});
        
        allArguments.push_back(std::make_unique<ArgumentSmallestDistance>(i));
        valueOrdering.insert({offset++, 3});        
    }  
}


void PotentialGenerator::custom_initialize() {
    
    allArguments.clear();
    valueOrdering.clear();

    int numK = KeeperStateCopy::numKeepers;
    int offset = 0;

    std::vector<int> ordering;
    VAFExtractor::loadOrdering(ordering);

    // Note ordering is in reverse:
    std::reverse(ordering.begin(), ordering.end());

    allArguments.push_back(std::make_unique<ArgumentTackle>(0));
    valueOrdering.insert({offset++, 0});

    for (int i = 1; i < numK; i++) {
        // Create arguments and their values
        allArguments.push_back(std::make_unique<ArgumentOpenKeeper>(i));
        valueOrdering.insert({offset++, 0});
         
        allArguments.push_back(std::make_unique<ArgumentFarKeeper>(i));
        valueOrdering.insert({offset++,0});
        
        allArguments.push_back(std::make_unique<ArgumentSmallestAngle>(i));
        valueOrdering.insert({offset++, 0});
        
        allArguments.push_back(std::make_unique<ArgumentSmallestDistance>(i));
        valueOrdering.insert({offset++, 0});        
    }  


    for (int i = 0; i < ordering.size(); i++) {
        valueOrdering[ordering.at(i)] = i;
    }
}

