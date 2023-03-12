#ifndef StateConverter_H
#define StateConverter_H


#include <vector>

using namespace std;

class StateConverter {

public:
    virtual void convert(vector<vector<double>> &states, vector<double> &values) = 0;
};

#endif