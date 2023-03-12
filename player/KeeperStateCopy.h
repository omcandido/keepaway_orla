/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   KeeperStateCopy.h
 * Author: dmitrykazhdan
 *
 * Created on 21 December 2018, 22:52
 */
#include "SMDPAgent.h"

#ifndef KEEPERSTATECOPY_H
#define KEEPERSTATECOPY_H

class KeeperStateCopy {
public:  
   static bool validState;
   static int numKeepers;
   static int numTakers;
   static double state[MAX_STATE_VARS];
private:

};

#endif /* KEEPERSTATECOPY_H */

