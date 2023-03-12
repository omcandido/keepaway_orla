#include "ArgumentT2H4.h"

ArgumentT2H4::ArgumentT2H4(int _argID, int _agentID, int _action, int _numT, int _numK) 
: ActionArgument(_argID, _agentID, _action, _numT, _numK) {
}

// IF 2 < dist(K1, Me) < 10 

bool ArgumentT2H4::isApplicable(vector<double> &state) {

    double dist = state.at(0);

    if (dist >= 2.2 && dist <= 10.1) {
        return true;
    } else {
        return false;
    }
}


unique_ptr<ActionArgument> ArgumentT2H4::clone() const {
  return make_unique<ArgumentT2H4>(
      argID, agentID, action, numTakers, numKeepers); 
}




