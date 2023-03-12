#include <sstream>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include "IOUtils.h"


void IOUtils::saveValues(string &output_filepath, vector<double> &values) {

    string content = "";

    for (int i = 0; i < values.size(); i++) {
        content += to_string(values.at(i)) + " ";
    }

    ofstream file(output_filepath);
    if (file.is_open()) {
        file << content;
        file.close();
    } 
}


void IOUtils::loadValues(string &file_path, vector<double> &values) {

    ifstream file(file_path, ios::in);
    string line;
    double val;

    if (getline(file, line)) {
        stringstream file_stream(line);
        file.close();
        
        while (file_stream >> val){
            values.push_back(val);
        }
    }
}




void IOUtils::saveActionArguments(string &output_filepath, set<int> &argIDs) {

    // Write new file contents
    ofstream ofs;
    ofs.open(output_filepath, ofstream::out | ofstream::trunc);

    for (auto argID: argIDs) {
        // Write argument to file
        ofs << argID << endl;
    }
    ofs.close();
}

void IOUtils::readActionArguments(string &input_filepath, set<int> &argIDs) {

    ifstream file(input_filepath, ios::in);
    string line;
    int argID;

    while (getline(file, line)) {
        stringstream file_stream(line);
        file_stream >> argID;
        argIDs.insert(argID);
    }
    file.close();
}









