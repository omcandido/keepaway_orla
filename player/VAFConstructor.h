#ifndef VAFConstructor_H
#define VAFConstructor_H

#include <map>
#include "ArgumentationFramework.h"
#include "Ordering.h"

class VAFConstructor {
public:
    static void constructVAF(ArgumentationFramework &af, Ordering &argOrdering, ArgumentationFramework &vaf);
};

#endif