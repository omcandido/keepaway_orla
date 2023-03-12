#include "ActionArgument.h"


#ifndef ArgumentT1H3_H
#define ArgumentT1H3_H

class ArgumentT1H3 : public ActionArgument {
public:
    ArgumentT1H3(int _argID, int _agentID, int _action, int _numT, int _numK);
    bool isApplicable(std::vector<double> &state);
    unique_ptr<ActionArgument> clone() const;
};


#endif