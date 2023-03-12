#include <set>
#include "Argument.h"
#include "ArgumentationFramework.h"
#include <memory>

using namespace std;

#ifndef ExtensionSolver_H
#define ExtensionSolver_H

/*
Abstract class for computing a set of extensions from a given argumentation framework
Potential examples include: grounded extensions, preferred extensions, etc.
*/
class ExtensionSolver {

public:
    virtual void solve(ArgumentationFramework af, set<set<int>> &extensions) = 0;
};


#endif