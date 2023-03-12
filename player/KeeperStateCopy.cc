/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   KeeperStateCopy.cc
 * Author: dmitrykazhdan
 * 
 * Created on 21 December 2018, 22:52
 */
#include <algorithm>
#include <iterator>
#include "KeeperStateCopy.h"

double KeeperStateCopy::state[MAX_STATE_VARS];
int KeeperStateCopy::numKeepers=-1;
int KeeperStateCopy::numTakers=-1;
bool KeeperStateCopy::validState=false;


//  WB_dist_to_C[1];
//  WB_dist_to_K[ numK - 1];
//  WB_dist_to_T[ numT ];
//  dist_to_C_K[ numK - 1 ];
//  dist_to_C_T[ numT ];
//  nearest_Opp_dist_K[ numK - 1 ];
//  nearest_Opp_ang_K[ numK - 1];
