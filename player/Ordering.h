#ifndef Ordering_H
#define Ordering_H

#include <map> 
#include <string>

using namespace std;

class Ordering {
public:
    Ordering();
    Ordering (const Ordering &oldOrdering);
    int getValue(int argID);
    void setValue(pair<int, int> keyValuePair);
    void loadOrdering(string &file_path);
    void saveOrdering(string &output_filepath);
    map<int, int> values;

private:
    
};

#endif