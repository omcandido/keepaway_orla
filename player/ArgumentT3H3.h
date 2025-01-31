#include "ActionArgument.h"


#ifndef ArgumentT3H3_H
#define ArgumentT3H3_H

class ArgumentT3H3 : public ActionArgument {
public:
    ArgumentT3H3(int _argID, int _agentID, int _action, int _numT, int _numK);
    bool isApplicable(std::vector<double> &state);
    unique_ptr<ActionArgument> clone() const;
};


#endif