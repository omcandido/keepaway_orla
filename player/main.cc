/*
Copyright (c) 2004, Gregory Kuhlmann, Peter Stone 
University of Texas at Austin               
All right reserved

Based On:     
         
Copyright (c) 2000-2003, Jelle Kok, University of Amsterdam
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the University of Amsterdam nor the names of its
contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*! \file main.cpp
<pre>
<b>File:</b>          main.cpp
<b>Project:</b>       Robocup Soccer Simulation Team: UvA Trilearn
<b>Authors:</b>       Jelle Kok
<b>Created:</b>       28/11/2000
<b>Last Revision:</b> $ID$
<b>Contents:</b>      This file contains the main of the program that is used
               to start the agent. It creates all classes, starts the different
               threads and calls the mainloop. Furthermore it parses the
               specified arguments to adjust the variables.
<hr size=2>
<h2><b>Changes</b></h2>
<b>Date</b>             <b>Author</b>          <b>Comment</b>
28/11/2000       Jelle Kok       Initial version created
</pre>
*/


#include "SenseHandler.h"
#include "ActHandler.h"
#include "KeepawayPlayer.h"
#include "HandCodedAgent.h"
#include "LinearSarsaAgent.h"

#include "Parse.h"
#include <string.h>   // needed for strcpy
#ifdef WIN32
#include <windows.h>  // needed for CreateThread
#else
#include <dlfcn.h>    // needed for extension loading.
#include <pthread.h>  // needed for pthread_create
#endif
#include <stdlib.h>   // needed for exit
#include <vector>
#include "KeeperStateCopy.h"
#include "Analysis.h"
#include "map"

#include <memory>
#include "BasicTrajectoryData.h"
#include "BasicModelExtractor.h"
#include "BasicTeamModel.h"
#include "ArgumentFarKeeper.h"
#include "ArgumentOpenKeeper.h"
#include "ArgumentSmallestAngle.h"
#include "ArgumentSmallestDistance.h"
#include "ArgumentTackle.h"
#include <vector>
#include <string>
#include <iostream>
#include "ArgumentT1H1.h"
#include "ArgumentT1H2.h"
#include "ArgumentT1H3.h"
#include "ArgumentT2H1.h"
#include "ArgumentT2H2.h"
#include "ArgumentT2H3_T3H1.h"
#include "ArgumentT2H4.h"
#include "ArgumentT3H2.h"
#include "ArgumentT3H3.h"
				

extern Logger Log;     /*!< This is a reference to the normal Logger class   */
extern LoggerDraw LogDraw; /*!< This is a reference to the drawing Logger class  */

void printOptions( );
unique_ptr<BasicAgentModel> createAgent(int guid, int numTakers, int numKeepers, string mode);
void loadDefaultOrdering(int numTakers, int numKeepers, Ordering& default_ordering);

/*! This is the main function and creates and links all the different classes.
    First it reads in all the parameters from the command prompt
    (<program name> -help) and uses these values to create the classes. After
    all the classes are linked, the mainLoop in the Player class is called. */
int main( int argc, char * argv[] )
{

#ifdef WIN32
  HANDLE         sense;
#else
  pthread_t      sense;
#endif

  ServerSettings ss;
  PlayerSettings cs;

  // define variables for command options and initialize with default values
  char     strTeamName[MAX_TEAM_NAME_LENGTH] = "UvA_Trilearn";
  int      iPort                             = ss.getPort();
  int      iMinLogLevel                      ;
  int      iMaxLogLevel                      ;
  char     strHost[128]                      = "127.0.0.1";
  // Changes recommended by celibertojr include changing dVersion to 12. See:
  // https://utlists.utexas.edu/sympa/arc/keepaway_orla/2011-05/msg00006.html
  double   dVersion                          = 12.0;
  int      iMode                             = 0;
  int      iNr                               = 2;
  int      iReconnect                        = -1;
  int      iNumKeepers                       = 3;
  int      iNumTakers                        = 2;
  char     strPolicy[128]                    = "random";
  char     takerMode[128]                    = "handcrafted";
  bool     bLearn                            = false;
  char     loadWeightsFile[256]              = "";
  char     saveWeightsFile[256]              = "";
  bool     hiveMind                          = false;
  bool     bInfo                             = false;
  bool     bSuppliedLogFile                  = false;
  bool     bSuppliedLogDrawFile              = false;
  int      iStopAfter                        = -1; //*met+1 8/16/05
  int      iStartLearningAfter               = -1;
  int      guid                              = -1;
  ofstream os;
  ofstream osDraw;

  // read in all the command options and change the associated variables
  // assume every two values supplied at prompt, form a duo
  char * str;
  for( int i = 1 ; i < argc ; i = i + 2  )
  {
    // help is only option that does not have to have an argument
    if( i + 1 >= argc && strncmp( argv[i], "-help", 3 ) != 0 )
    {
      cout << "Need argument for option: " << argv[i] << endl;
      exit( 0 );
    }
    // read a command option
    if( argv[i][0] == '-' && strlen( argv[i] ) > 1)
    {
      switch( argv[i][1] )
      {
        case '?':                                   // print help
          printOptions( );
          exit(0);
          break;
        case 'a':                                   // output file drawlog info
          osDraw.open( argv[i+1] );
          bSuppliedLogDrawFile = true;
          break;
        case 'c':                                   // clientconf file
          if( cs.readValues( argv[i+1], ":" ) == false )
            cerr << "Error in reading client file: " << argv[i+1] << endl;
          break;
        case 'd':
          str   = &argv[i+1][0];
          LogDraw.setActive( Parse::parseFirstInt( &str ) == 1 );
          break;
        case 'e': // enable learning 0/1
          str    = &argv[i+1][0];
          bLearn = (Parse::parseFirstInt( &str ) == 1 ) ? true : false ;
          break;
        case 'f':
          strcpy( saveWeightsFile, argv[i+1] );
          break;
        case 'g':
          str = &argv[i+1][0];
          guid = Parse::parseFirstInt( &str );
          break;  
        case 'h':                                   // host server or help
          if( strlen( argv [i]) > 2 && argv[i][2] == 'e' )
          {
            printOptions( );
            exit(0);
          }
          else if (strlen(argv[i]) > 2 && argv[i][2] == 'i')
          {
            str = &argv[i+1][0];
            hiveMind = (Parse::parseFirstInt( &str ) == 1 ) ? true : false;
          }
          else
            strcpy( strHost, argv[i+1] );
          break;
        case 'i':                                   // info 1 0
          str   = &argv[i+1][0];
          bInfo = (Parse::parseFirstInt( &str ) == 1 ) ? true : false ;
          break;
        case 'j':
          str   = &argv[i+1][0];
          iNumTakers = Parse::parseFirstInt( &str );
          break;
        case 'k':
          str   = &argv[i+1][0];
          iNumKeepers = Parse::parseFirstInt( &str );
          break;
        case 'l':                                   // loglevel int[..int]
          str = &argv[i+1][0];
          iMinLogLevel = Parse::parseFirstInt( &str );
          while( iMinLogLevel != 0 )
          {
            if( *str == '.' || *str == '-') // '.' or '-' indicates range
            {
              *str += 1 ;
              iMaxLogLevel = Parse::parseFirstInt( &str );
              if( iMaxLogLevel == 0 ) iMaxLogLevel = iMinLogLevel;
              Log.addLogRange( iMinLogLevel, iMaxLogLevel );
            }
            else
              Log.addLogLevel( iMinLogLevel );
            iMinLogLevel = Parse::parseFirstInt( &str );
          }
          break;
        case 'm':                                   // mode int
          str = &argv[i+1][0];
          iMode = Parse::parseFirstInt( &str );
          break;
        case 'n':                                   // number in formation int
          str = &argv[i+1][0];
          iNr = Parse::parseFirstInt( &str );
          break;
        case 'o':                                   // output file log info
          os.open( argv[i+1] );
          bSuppliedLogFile = true;
          break;
        case 'p':                                   // port
          str = &argv[i+1][0];
          iPort = Parse::parseFirstInt( &str );
          break;
        case 'q':
          strcpy( strPolicy, argv[i+1] );
          break;
        case 'r':                                   // reconnect 1 0
          str = &argv[i+1][0];
          iReconnect = Parse::parseFirstInt( &str );
          break;
        case 's':                                   // serverconf file
          if( ss.readValues( argv[i+1], ":" ) == false )
            cerr << "Error in reading server file: " << argv[i+1] << endl;
          break;
        case 't':                                   // teamname name
          strcpy( strTeamName, argv[i+1] );
          break;
        case 'v':                                   // version version
          str = &argv[i+1][0];
          dVersion = Parse::parseFirstDouble( &str );
          break;
        case 'w':
          strcpy( loadWeightsFile, argv[i+1] );
          break;
        case 'x':
          str   = &argv[i+1][0];
          iStopAfter = Parse::parseFirstInt( &str ); // exit after running for iStopAfter episodes
          break;
        case 'y':
          str   = &argv[i+1][0];
          iStartLearningAfter = Parse::parseFirstInt( &str ); // initially don't learn, but turn on learning after iStartLearningAfter episodes have passed
          break;
        case 'z':
          strcpy( takerMode, argv[i+1] ); // takerMode
          cout<< takerMode <<endl;
          break;
        default:
          cerr << "(main) Unknown command option: " << argv[i] << endl;
      }
    }
  }
  if( bInfo == true )
  {
    cout << "team         : "  << strTeamName    << endl <<
            "port         : "  << iPort          << endl <<
            "host         : "  << strHost        << endl <<
            "version      : "  << dVersion       << endl <<
            "mode         : "  << iMode          << endl <<
            "playernr     : "  << iNr            << endl <<
            "reconnect    : "  << iReconnect     << endl <<
            "takerMode    : "  << takerMode      << endl ;
    Log.showLogLevels( cout );
  }
  if( bSuppliedLogFile == true )
    Log.setOutputStream( os );                   // initialize logger
  else
    Log.setOutputStream( cout );
  if( bSuppliedLogDrawFile == true )
    LogDraw.setOutputStream( osDraw );          // initialize drawing logger
  else
    LogDraw.setOutputStream( cout );

  Log.restartTimer( );

  //Formations fs( strFormations, (FormationT)cs.getInitialFormation(), iNr-1 );

  if (takerMode == (string) "orla"){
    cout << "Increasing server timeout..."<< endl;
    cs.setServerTimeOut(99999);
  }

                                               // read formations file
  WorldModel wm( &ss, &cs, NULL );              // create worldmodel
  Connection c( strHost, iPort, MAX_MSG );     // make connection with server
  ActHandler a( &c, &wm, &ss );                // link actHandler and worldmodel
  SenseHandler s( &c, &wm, &ss, &cs );         // link senseHandler with wm

  SMDPAgent *sa = NULL;

  double keeper_ranges[ MAX_STATE_VARS ];
  double keeper_minValues[ MAX_STATE_VARS ];
  double keeper_resolutions[ MAX_STATE_VARS ];
  
  double taker_ranges[ MAX_STATE_VARS ];
  double taker_minValues[ MAX_STATE_VARS ];
  double taker_resolutions[ MAX_STATE_VARS ];
  
  int n_keeper_features = wm.keeperStateRangesAndResolutions( keeper_ranges, keeper_minValues, keeper_resolutions, 
                                                        iNumKeepers, iNumTakers );  
  
  int n_taker_features = wm.takerStateRangesAndResolutions( taker_ranges, taker_minValues, taker_resolutions, 
                                                        iNumKeepers, iNumTakers );


  int numActions = iNumKeepers;
  
  // cout << "# features: " << n_taker_features << endl;
  // cout << "# actions: " << numActions << endl;


  if ( strlen( strPolicy ) > 0 && strPolicy[0] == 'l' ) {
    // (l)earned
    // or "learned!" -> Don't explore at all.
      
    if (guid == -1) {
      cerr << "Missing agent identifier" << endl;
      return false;
    } 
     

    
    LinearSarsaAgent* linearSarsaAgent = new LinearSarsaAgent(
      guid, n_taker_features, numActions, bLearn, taker_resolutions,
      loadWeightsFile, saveWeightsFile, hiveMind
    );
    // Check for pure exploitation mode.
    size_t length = strlen(strPolicy);
    if (strPolicy[length - 1] == '!') {
      linearSarsaAgent->setExploitation();
    }
    // Done setting up.
    sa = linearSarsaAgent;  

    Analysis::agent = createAgent(guid, iNumTakers, iNumKeepers, (string) takerMode);
    Analysis::n_taker_features = n_taker_features;
    Analysis::agentID = guid;


  } else if (!strncmp(strPolicy, "ext=", 4)) {
    // Load extension.
    // Name should come after "ext=". Yes, this is hackish.
    char* extensionName = strPolicy + 4;
    // These parameters are based on the expected learning agent parameters
    // above.
    // Added WorldModel for agents needing richer/relational representation!
    typedef SMDPAgent* (*CreateAgent)(
      WorldModel&, int, int, bool, double*, char*, char*, bool
    );
    CreateAgent createAgent = NULL;
#ifdef WIN32
    // TODO
#else
    // Load the extension.
    void* extension = dlopen(extensionName, RTLD_NOW);
    if (!extension) {
      cerr << "Failed to load extension " << extensionName << endl;
      cerr << dlerror() << endl;
      return EXIT_FAILURE;
    }
    // Find the createAgent function.
    createAgent =
      reinterpret_cast<CreateAgent>(dlsym(extension, "createAgent"));
    if (!createAgent) {
      cerr << "Failed to find createAgent in " << extensionName << endl;
      cerr << dlerror() << endl;
      return EXIT_FAILURE;
    }
#endif
    sa = createAgent(
      wm, n_taker_features, numActions, bLearn, taker_resolutions,
      loadWeightsFile, saveWeightsFile, hiveMind
    );
  } else {
    // (ha)nd (ho)ld (r)andom
    sa = new HandCodedAgent( n_keeper_features, numActions,
                             strPolicy, &wm );
  }

  if (!sa) {
    cerr << "No agent!" << endl;
    return EXIT_FAILURE;
  }
  KeepawayPlayer bp( sa, &a, &wm, &ss, &cs, strTeamName, 
                     iNumKeepers, iNumTakers, dVersion, iReconnect, takerMode);

#ifdef WIN32
  DWORD id1;
  sense = CreateThread(NULL, 0, &sense_callback, &s, 0, &id1);
  if (sense == NULL)
  {
      cerr << "create thread error" << endl;
      return false;
  }
#else
  pthread_create( &sense, NULL, sense_callback  , &s); // start listening
#endif

  if( iMode == 0 )
    bp.mainLoop();

  c.disconnect();
  os.close();
}

/*! This function prints the command prompt options that can be supplied to the
    program. */
void printOptions( )
{
  cout << "Command options:"                                         << endl <<
   " a file                - write drawing log info to "             << endl <<
   " c(lientconf) file     - use file as client conf file"           << endl <<
   " d(rawloglevel) int[..int] - level(s) of drawing debug info"     << endl <<
   " e(nable) learning 0/1  - turn learning on/off"                  << endl << 
   " f save weights file   - use file to save weights"               << endl <<
   " h(ost) hostname       - host to connect with"                   << endl <<
   " he(lp)                - print this information"                 << endl <<
   " hi(ve) 0/1            - use mmap to hive mind the team"         << endl <<
   " i(nfo) 0/1            - print variables used to start"          << endl <<
   " j takers  int         - number of takers"                       << endl <<
   " k(eepers) int         - number of keepers"                      << endl <<
   " l(oglevel) int[..int] - level of debug info"                    << endl <<
   " m(ode) int            - which mode to start up with"            << endl <<
   " n(umber) int          - player number in formation"             << endl <<
   " o(utput) file         - write log info to (screen is default)"  << endl <<
   " p(ort)                - port number to connect with"            << endl <<
   " q policy name         - policy to play with"                    << endl <<
   " r(econnect) int       - reconnect as player nr"                 << endl <<
   " s(erverconf) file     - use file as server conf file"           << endl <<
   " t(eamname) name       - name of your team"                      << endl <<
   " w(eights) file        - use file to load weights"               << endl <<
   " x exit after running for this many episodes"                    << endl <<
   " y enable learning after not learning for this many episodes"    << endl;
}








unique_ptr<BasicAgentModel> createAgent(int guid, int numTakers, int numKeepers, string mode) {

    // cout << "-- CREATE AGENT!--\n";
    // cout << "-guid: " << guid << "-\n";

    // string prefix = "/home/dmitrykazhdan/Desktop/";
    // prefix = "/home/dk525/Part-III-Project/keepaway_orla/player/";
    string prefix = "/home/candido/keepaway_orla/player/";

    vector<int> teamIDs;
    string agentStateFile;
    string agentArgFile;
    vector<string> teamStateFiles;
    vector<string> teamArgFiles;
    string argCommPath = prefix + "argComm/agent_";
    string stateCommPath = prefix + "stateComm/agent_";
    string orderingPath = prefix + "savedOrderings/agent_"+ to_string(guid) + ".txt";

    for (int i = 1; i <= numTakers; i++) {
        string nextCommPath = stateCommPath + to_string(i) + ".txt";
        string nextArgPath = argCommPath + to_string(i) + ".txt";

        if (i == guid) {
            agentStateFile.assign(nextCommPath);
            agentArgFile.assign(nextArgPath);
        } else {
            teamStateFiles.push_back(nextCommPath);
            teamArgFiles.push_back(nextArgPath);
            teamIDs.push_back(i);
        }
    }


    vector<unique_ptr<ActionArgument>> allActionArguments;

    int offset = 0;

    for (int agentID = 1; agentID <= numTakers; agentID++) {

        allActionArguments.push_back(make_unique<ArgumentTackle>(offset++, agentID, 0, numTakers, numKeepers));

        for (int i = 1; i < numKeepers; i++) {
            // Create arguments and their values
            allActionArguments.push_back(make_unique<ArgumentOpenKeeper>(offset++, agentID, i, numTakers, numKeepers));
            allActionArguments.push_back(make_unique<ArgumentFarKeeper>(offset++, agentID, i, numTakers, numKeepers));
            allActionArguments.push_back(make_unique<ArgumentSmallestAngle>(offset++, agentID, i, numTakers, numKeepers));
            allActionArguments.push_back(make_unique<ArgumentSmallestDistance>(offset++, agentID, i, numTakers, numKeepers));
        } 
    }

    // std::cout << " -- START allActionArguments";
    // for (auto& i: allActionArguments)
    //   std::cout << i->getArgID() << ' ';
    // std::cout << " FINISH --\n ";


    // Extra heuristics:

    /*
        Agent1:
        1) IF min_ang(K2, T) < 18 : Mark K1
        2) IF dist(T2, K1) > 26 : Mark K1
        3) IF min_ang(K3, T) < 23 : Mark K1

        Agent2:
        1) IF dist(K3, K1) > 26 : Mark K3
        2) IF dist(T1, Me) > 11 : Mark K1
        3) IF dist(K4, K1) > 30 : Mark K3 

        Agent3:
        1) IF dist(K4, K1) > 30 : Mark K4
        2) IF dist(K1, Me) > 27 : Mark K4
        3) IF dist(T2, Me) > 16 : Mark K4
    */

    //  // Agent 1:
    // allActionArguments.push_back(make_unique<ArgumentT1H1>(offset++, 1, 0, numTakers, numKeepers));
    // allActionArguments.push_back(make_unique<ArgumentT1H2>(offset++, 1, 0, numTakers, numKeepers));

     // Agent 2:
    //  allActionArguments.push_back(make_unique<ArgumentT2H1>(offset++, 2, 1, numTakers, numKeepers));
    //  allActionArguments.push_back(make_unique<ArgumentT2H3_T3H1>(offset++, 2, 2, numTakers, numKeepers));

     // Agent 3:
    // allActionArguments.push_back(make_unique<ArgumentT2H3_T3H1>(offset++, 3, 3, numTakers, numKeepers));
    // allActionArguments.push_back(make_unique<ArgumentT3H2>(offset++, 3, 3, numTakers, numKeepers));


    Ordering argOrdering;


    if (mode == "default"){
      cout << "Loading default ordering..." <<endl;
      loadDefaultOrdering(numTakers, numKeepers, argOrdering);
    } else if (mode=="extracted"){
      cout << "Loading extracted ordering" <<endl;
      argOrdering.loadOrdering(orderingPath);
    } else if (mode=="orla"){
      cout << "Loading ORLA ordering" <<endl;
      string pathOrdering = "/home/candido/keepaway_orla/orla/ordering.txt";
      argOrdering.loadOrdering(pathOrdering);
    }

    // std::cout << " -- START ordering (main): " << guid << " -> ";
    // for (auto i: argOrdering.values)
    //     std::cout << i.first << ":" << i.second << ", ";
    // std::cout << " FINISH -- \n";  

    auto myConverter = make_unique<BasicValueExtractor>(numKeepers);

    auto agent = make_unique<BasicAgentModel>(guid, teamIDs, agentStateFile,
                            agentArgFile, teamStateFiles, teamArgFiles,
                            allActionArguments, move(myConverter), argOrdering);

    return agent;
}

void loadDefaultOrdering(int numTakers, int numKeepers, Ordering& default_ordering) {
    int offset = 0;
    for (int j = 0; j < numTakers; j++) {
        default_ordering.setValue({offset++, 5});
        for (int i = 1; i < numKeepers; i++) {
            default_ordering.setValue({offset++, 2});
            default_ordering.setValue({offset++, 1});
            default_ordering.setValue({offset++, 4});
            default_ordering.setValue({offset++, 4});
        }
    }
}