/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RecommendationLogger.h
 * Author: dmitrykazhdan
 *
 * Created on 01 January 2019, 18:10
 */
#include <iostream>

#ifndef RECOMMENDATIONLOGGER_H
#define RECOMMENDATIONLOGGER_H

class RecommendationLogger {
public:
    static int numRecommendations;
    static int numRequests;
    static void print();
private:

};

#endif /* RECOMMENDATIONLOGGER_H */

