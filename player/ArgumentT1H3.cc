#include "ArgumentT1H3.h"

ArgumentT1H3::ArgumentT1H3(int _argID, int _agentID, int _action, int _numT, int _numK) 
: ActionArgument(_argID, _agentID, _action, _numT, _numK) {
}


// IF min_ang(K3, T) < 20

bool ArgumentT1H3::isApplicable(vector<double> &state) {

    int offset = numKeepers + (numTakers - 1) + 
                (numKeepers - 1) + (numKeepers - 1) +
                    (numTakers - 1) + 1;

    double ang = state.at(offset);

    if (ang < 20) {
        return true;
    } else {
        return false;
    }
}


unique_ptr<ActionArgument> ArgumentT1H3::clone() const {
  return make_unique<ArgumentT1H3>(
      argID, agentID, action, numTakers, numKeepers); 
}




