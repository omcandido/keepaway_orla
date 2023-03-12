#include "ActionArgument.h"


#ifndef ArgumentT2H4_H
#define ArgumentT2H4_H

class ArgumentT2H4 : public ActionArgument {
public:
    ArgumentT2H4(int _argID, int _agentID, int _action, int _numT, int _numK);
    bool isApplicable(std::vector<double> &state);
    unique_ptr<ActionArgument> clone() const;
};


#endif