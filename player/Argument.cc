/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Argument.cc
 * Author: dmitrykazhdan
 * 
 * Created on 21 December 2018, 19:42
 */

#include "Argument.h"

Argument::Argument(int _agentID, int _argumentID) : 
agentID(_agentID), argumentID(_argumentID) {
}

int Argument::getAgentID() {
    return agentID;
}

int Argument::getArgumentID() const {
    return argumentID;
}

bool Argument::operator <(const Argument& right) const {
    
    if (argumentID == right.argumentID) {
        return agentID < right.agentID;
    } else {
        return argumentID < right.argumentID;
    }  
}

bool Argument::operator ==(const Argument& other) const {
    return (argumentID == other.argumentID) and (other.agentID == agentID);
}


