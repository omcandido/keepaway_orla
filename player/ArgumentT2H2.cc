#include "ArgumentT2H2.h"

ArgumentT2H2::ArgumentT2H2(int _argID, int _agentID, int _action, int _numT, int _numK)
: ActionArgument(_argID, _agentID, _action, _numT, _numK) {
}

// IF dist(T1, Me) > 11

bool ArgumentT2H2::isApplicable(vector<double> &state) {

    int offset = numKeepers;
    double dist = state.at(offset);

    if (dist >= 11 && dist <= 20) {
        return true;
    } else {
        return false;
    }
}


unique_ptr<ActionArgument> ArgumentT2H2::clone() const {
  return make_unique<ArgumentT2H2>(
      argID, agentID, action, numTakers, numKeepers);
}




