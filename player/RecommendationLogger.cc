/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RecommendationLogger.cc
 * Author: dmitrykazhdan
 * 
 * Created on 01 January 2019, 18:10
 */

#include "RecommendationLogger.h"

int RecommendationLogger::numRecommendations = 0;
int RecommendationLogger::numRequests = 0;


void RecommendationLogger::print() {

    std::cout << "No. Requests: " << RecommendationLogger::numRequests
    << "  No. Recommendations: " << RecommendationLogger::numRecommendations << std::endl;


}

