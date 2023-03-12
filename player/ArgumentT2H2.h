#include "ActionArgument.h"


#ifndef ArgumentT2H2_H
#define ArgumentT2H2_H

class ArgumentT2H2 : public ActionArgument {
public:
    ArgumentT2H2(int _argID, int _agentID, int _action, int _numT, int _numK);
    bool isApplicable(std::vector<double> &state);
    unique_ptr<ActionArgument> clone() const;
};


#endif