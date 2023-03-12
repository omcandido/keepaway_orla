/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ApplicableArguments.cpp
 * Author: dmitrykazhdan
 * 
 * Created on 21 December 2018, 14:28
 */

#include "ApplicableArguments.h"

ApplicableArguments::ApplicableArguments(int _agentID) {
    agentID = _agentID;
}

ApplicableArguments::ApplicableArguments(const ApplicableArguments& old_obj) {
    agentID = old_obj.agentID;
    applicableArgIDs = old_obj.applicableArgIDs; 
}

void ApplicableArguments::addArgID(int argID) {
    applicableArgIDs.push_back(argID);
}

int ApplicableArguments::getAgentID() {
    return agentID;
}

int ApplicableArguments::getArgID(int index) {
    return applicableArgIDs.at(index);
}

int ApplicableArguments::getNumArgs() {
    return applicableArgIDs.size();
}


