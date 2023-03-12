#ifndef ActionArgumentAFConstructor_H
#define ActionArgumentAFConstructor_H

#include <vector>
#include <memory>
#include "ArgumentationFramework.h"
#include "ActionArgument.h"

using namespace std;

class ActionArgumentAFConstructor {

public:
  static void constructAF(set<int> &argIDs, const vector<unique_ptr<ActionArgument>> &actionArgsTable, ArgumentationFramework &af);

};

#endif