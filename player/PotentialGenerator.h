/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PotentialGenerator.h
 * Author: dmitrykazhdan
 *
 * Created on 19 December 2018, 19:58
 */
#include <string>
#include <vector>
#include <algorithm>
#include "ActionArgument.h"
#include <ostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <string.h>
#include <memory>
#include <map>
#include <set> 
#include <time.h>
#include "Argument.h"
#include "ApplicableArguments.h"
#include "ArgumentTackle.h"
#include "ArgumentOpenKeeper.h"
#include "ArgumentFarKeeper.h"
#include "ArgumentSmallestAngle.h"
#include "ArgumentSmallestDistance.h"
#include "KeeperStateCopy.h"
#include <iostream>

#ifndef POTENTIALGENERATOR_H
#define POTENTIALGENERATOR_H

#define MAX_AGENT_ARGS 10

using namespace std;

class PotentialGenerator {
public:
    static void initialize();
    static void custom_initialize();
    static void getPotential(int agentID, double state[], double potentialTable[]);
    static void getApplicableArguments(double state[], ApplicableArguments &myAppArgs);
    static vector<std::unique_ptr<ActionArgument>> allArguments;
    static map<int, int> valueOrdering;
    static void updatePotentialVal();
private:
    static void computeGroundedExtension(std::map<Argument, set<Argument>> &VSCAF, std::map<Argument, set<Argument>> &isAttackedBy, set<Argument> &groundedExtension);
    static void saveArguments(ApplicableArguments &applicableArgs, const string &filename);
    static void loadArguments(int agentID, vector<ApplicableArguments> &arguments);
    static void computePotential(int agentID, vector<ApplicableArguments> &applicableArguments, double potentialTable[]);
    static string argumentsPath;
    static double potentialVal;
    static void computeVSCAF(set<Argument> &arguments, std::map<Argument, set<Argument>> &VSCAF);
    static void computeIsAttackedBy(std::map<Argument, set<Argument>> &VSCAF, std::map<Argument, set<Argument>> &isAttackedBy);
        
    
};

#endif /* POTENTIALGENERATOR_H */

