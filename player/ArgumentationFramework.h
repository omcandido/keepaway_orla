#ifndef ArgumentationFramework_H
#define ArgumentationFramework_H

#include "Argument.h"
#include <set>
#include <map>


using namespace std;

class ArgumentationFramework {
public:
    ArgumentationFramework();
    ArgumentationFramework (const ArgumentationFramework &oldAF); 
    ArgumentationFramework(set<int> &_allArguments);
    set<int> allArguments;
    map<int,set<int>> attacks;
    map<int,set<int>> isAttackedBy;
};

#endif