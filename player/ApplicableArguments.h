/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ApplicableArguments.h
 * Author: dmitrykazhdan
 *
 * Created on 21 December 2018, 16:19
 */

#ifndef APPLICABLEARGUMENTS_H
#define APPLICABLEARGUMENTS_H

#include <vector>


class ApplicableArguments {
public:
    ApplicableArguments(const ApplicableArguments &old_obj);
    ApplicableArguments(int _agentID);
    void addArgID(int argID);
    int getAgentID();
    int getArgID(int index);
    int getNumArgs();
private:
    int agentID;
    std::vector<int> applicableArgIDs;
private:

};

#endif /* APPLICABLEARGUMENTS_H */

