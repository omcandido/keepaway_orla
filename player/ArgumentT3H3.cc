#include "ArgumentT3H3.h"

ArgumentT3H3::ArgumentT3H3(int _argID, int _agentID, int _action, int _numT, int _numK) 
: ActionArgument(_argID, _agentID, _action, _numT, _numK) {
}

// IF dist(T2, Me) > 16

bool ArgumentT3H3::isApplicable(vector<double> &state) {

    int offset = numKeepers + 1;
    double dist = state.at(offset);

    if (dist >= 16) {
        return true;
    } else {
        return false;
    }
}


unique_ptr<ActionArgument> ArgumentT3H3::clone() const {
  return make_unique<ArgumentT3H3>(
      argID, agentID, action, numTakers, numKeepers); 
}




