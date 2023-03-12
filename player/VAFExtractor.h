/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VAFExtractor.h
 * Author: dmitrykazhdan
 *
 * Created on 02 February 2019, 21:25
 */
#include <vector>
#include "PotentialGenerator.h"
#include "KeeperStateCopy.h"
#include <set>

#ifndef VAFEXTRACTOR_H
#define VAFEXTRACTOR_H

class VAFExtractor {
public:
    static int getRecommendedAction(double state[]);
    static void processState(double state[], int agentAction);
    static int n_args;
    static std::vector<std::vector<int>> attackVals;
    static void initialize();
    static void print();
    static void extractAndSaveOrdering();
    static int agentID;
    static int numMismatch;
    static int numMatches;
    static void saveOrdering(std::vector<int> &ordering);
    static void loadOrdering(std::vector<int> &ordering);
    static string orderingPath;
    static int selectNextNode(std::set<int> &freeNodes);

private:
  
};

#endif /* VAFEXTRACTOR_H */

