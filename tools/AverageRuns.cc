#include <string>
#include <ostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <string.h> 


using namespace std;

int main( int argc, char* argv[] )
{            
    // string data_path = "/home/dmitrykazhdan/Part-III-Project/robocup/keepaway/logs/";
    string data_path = "/home/candido/robocup/keepaway/logs/";

 
    // string output_filename = "/home/dmitrykazhdan/Part-III-Project/robocup/keepaway/tools/avg.out";
    string output_filename = "/home/candido/robocup/keepaway/tools/avg.out";

    // Load a set of arguments from files
    DIR* dirp = opendir(data_path.c_str());
    struct dirent *dp;
    int n_files = 0;
    bool is_parent_dir = false;
    bool is_current_dir = false;
    string line;

    // Count number of files in folder
    while ((dp = readdir(dirp)) != NULL) {  
      
        is_current_dir = strcmp(dp->d_name, ".");
        is_parent_dir = strcmp(dp->d_name, "..");
        string full_name = data_path + dp->d_name;
        
        if ((is_parent_dir != 0) && (is_current_dir != 0)) {
            n_files++;
        };       
    }
    closedir(dirp); 

    cout << "Retrieved " << n_files << " files..." << std::endl;

    // Create set of input file streams
    ifstream file_streams[n_files];
    
    // Open file streams of relevant files
    dirp = opendir(data_path.c_str());
    int offset = 0;
    
    while ((dp = readdir(dirp)) != NULL) {        
        is_current_dir = strcmp(dp->d_name, ".");
        is_parent_dir = strcmp(dp->d_name, "..");
        string full_name = data_path + dp->d_name;
        
        if ((is_parent_dir != 0) && (is_current_dir != 0)) {
	    std::cout << "Opening file: " << dp->d_name << "..." << std::endl;
            file_streams[offset++].open(full_name.c_str());
        };       
    }

    cout << "Retrieved input filestreams..." << std::endl;

    bool reached_eof = false;
    int iDummy = 0;
    char cDummy = 0;
    int episode_duration = 0;
    int duration_sum = 0;
    double start = 0;
    double avg_duration = 0;
    bool header_line = false;

    // Create output filestream
    ofstream ofs;

    ofs.open(output_filename.c_str(), ofstream::out | ofstream::trunc);
    
    while (!reached_eof) {
        
        duration_sum = 0;
        header_line = false;
        
	// Read next line from each file
        for (int i = 0; i < n_files; i++) {
            
            if (getline(file_streams[i], line)) {
                
		// Skip header of file
            	if (line.at(0) == '#') {
		    header_line = true;
                    continue;
            	}

                // Read in episode duration from line
                std::istringstream iss(line);
                iss >> iDummy;
                iss >> iDummy;
                iss >> iDummy;
                iss >> episode_duration;
                iss >> cDummy;

		// Add episode duration to current accumulating sum
                duration_sum += episode_duration;
                
            } else {
		// If reached the end of at least one file - quit
                reached_eof = true;
                break;
            }
        }

	if (!header_line && !reached_eof) {

		// Compute the average of episode durations
		avg_duration = duration_sum / n_files;
		start += avg_duration;

		//cout << "Computed sum: " << avg_duration << std::endl; // TODO: delete

		// Save data to output file
		ofs << start / 10.0 / 3600 << " " <<
		        avg_duration / 10.0  << std::endl;
       } 
    }
    closedir(dirp);

    return 0;
}
