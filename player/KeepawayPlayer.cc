/*
Copyright (c) 2004 Gregory Kuhlmann, Peter Stone
University of Texas at Austin
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

#include "KeepawayPlayer.h"
#include "Parse.h"
#include "SayMsgEncoder.h"
#include <cstring>
#include "KeeperStateCopy.h"
#include "Analysis.h"
#include <chrono>

extern LoggerDraw LogDraw;

KeepawayPlayer::KeepawayPlayer( SMDPAgent* sa, ActHandler* act, WorldModel *wm, 
                                ServerSettings *ss, PlayerSettings *ps,
                                char* strTeamName, int iNumKeepers, int iNumTakers,
                                double dVersion, int iReconnect, string takerMode )

{
  char str[MAX_MSG];
  _takerMode    = takerMode;
  SA            = sa;
  ACT           = act;
  WM            = wm;
  SS            = ss;
  PS            = ps;
  bContLoop     = true;
  WM->setTeamName( strTeamName );
  WM->setNumKeepers( iNumKeepers );
  WM->setNumTakers( iNumTakers ); 
  WM->setNewEpisode( false );
  WM->setLastAction( UnknownIntValue );
  m_timeLastSay = -5;
  m_timeStartEpisode = -5;

  // create initialisation string
  if( iReconnect != -1 )
    sprintf( str, "(reconnect %s %d)", strTeamName, iReconnect );
  else
    sprintf( str, "(init %s (version %f))", strTeamName, dVersion );
  ACT->sendMessage( str );
  
  if ( WM->getSide() == SIDE_RIGHT ) {
    cout << ">>> Current takerMode: " << _takerMode << endl;
  }
}

/*! This is the main loop of the agent. This method calls the update methods
    of the world model after it is indicated that new information has arrived.
    After this, the correct main loop of the player type is called, which
    puts the best soccer command in the queue of the ActHandler. */
void KeepawayPlayer::mainLoop( )
{
  Timing timer;

  // wait for new information from the server
  // cannot say bContLoop=WM->wait... since bContLoop can be changed elsewhere
  if(  WM->waitForNewInformation() == false )
    bContLoop =  false;

  while( bContLoop )                                 // as long as server alive
  // while( bContLoop and Analysis::numEpisodes <1000 )                                 // as long as server alive
  {
    Log.logWithTime( 3, "  start update_all" );
    Log.setHeader( WM->getCurrentCycle() );
    LogDraw.setTime( WM->getCurrentCycle() );

    if( WM->updateAll( ) == true )
    {
      timer.restartTime();
      SoccerCommand soc;

      if ( WM->getSide() == SIDE_LEFT ) {
        soc = keeper();
      } else {

        if (_takerMode == "handcrafted"){
          soc = taker();
        } else if (_takerMode == "sarsa"){
          soc = learning_taker();
        } else if (_takerMode == "extracted"){
          soc = argumentation_taker();
        } else if (_takerMode == "default"){
          soc = argumentation_taker();
        } else if (_takerMode == "orla"){
          soc = argumentation_taker();
        } else {
          cout << "Error: Invalid takerMode: " << _takerMode << endl;
        }
      }
            
      if( shallISaySomething() == true )           // shall I communicate
        {
          m_timeLastSay = WM->getCurrentTime();
          char strMsg[MAX_SAY_MSG];
          makeSayMessage( soc, strMsg );
          if( strlen( strMsg ) != 0 )
            Log.log( 600, "send communication string: %s", strMsg );
          WM->setCommunicationString( strMsg );
        }
      Log.logWithTime( 3, "  determined action; waiting for new info" );
      // directly after see message, will not get better info, so send commands
      if( WM->getTimeLastSeeMessage() == WM->getCurrentTime() ||
          (SS->getSynchMode() == true && WM->getRecvThink() == true ))
      {
        Log.logWithTime( 3, "  send messages directly" );
        ACT->sendCommands( );
        Log.logWithTime( 3, "  sent messages directly" );
        if( SS->getSynchMode() == true  )
        {
          WM->processRecvThink( false );
          ACT->sendMessageDirect( "(done)" );
        }
      }
    }  
    else
      Log.logWithTime( 3, "  HOLE no action determined; waiting for new info");

    int iIndex;
    double dConfThr = PS->getPlayerConfThr();
    char buffer[128];
    for( ObjectT o = WM->iterateObjectStart( iIndex, OBJECT_SET_TEAMMATES, dConfThr);
           o != OBJECT_ILLEGAL;
           o = WM->iterateObjectNext ( iIndex, OBJECT_SET_TEAMMATES, dConfThr ) ) {
      LogDraw.logCircle( "Players", WM->getGlobalPosition( o ), 1.6, 80,
                         false,
                         COLOR_ORANGE, WM->getConfidence( o ) );
      sprintf( buffer, "%d", SoccerTypes::getIndex( o ) + 1 );
      LogDraw.logText( "Players", WM->getGlobalPosition( o ),
                       buffer,
                       80, COLOR_ORANGE );
    }
    // Me
    ObjectT o = WM->getAgentObjectType();
    LogDraw.logCircle( "Players", WM->getGlobalPosition( o ), 1.6, 81,
                       false,
                       COLOR_PURPLE, WM->getConfidence( o ) );
    sprintf( buffer, "%d", SoccerTypes::getIndex( o ) + 1 );
    LogDraw.logText( "Players", WM->getGlobalPosition( o ),
                     buffer,
                     81, COLOR_PURPLE );
    for( ObjectT o = WM->iterateObjectStart( iIndex, OBJECT_SET_OPPONENTS, dConfThr);
           o != OBJECT_ILLEGAL;
           o = WM->iterateObjectNext ( iIndex, OBJECT_SET_OPPONENTS, dConfThr ) ) {
      LogDraw.logCircle( "Players", WM->getGlobalPosition( o ), 1.6, 80,
                         false,
                         COLOR_PINK, WM->getConfidence( o ) );
      sprintf( buffer, "%d", SoccerTypes::getIndex( o ) + 1 );
      LogDraw.logText( "Players", WM->getGlobalPosition( o ),
                       buffer,
                       80, COLOR_PINK );
    }

    Log.logWithTime( 604, "time for action: %f", timer.getElapsedTime()*1000 );
           
    // wait for new information from the server cannot say
    // bContLoop=WM->wait... since bContLoop can be changed elsewhere
    if(  WM->waitForNewInformation() == false )
        bContLoop =  false;
  }

  // shutdown, print hole and number of players seen statistics
  SA->shutDown();
  printf("Shutting down player %d\n", WM->getPlayerNumber() );
  // printf("   Number of holes: %d (%f)\n", WM->iNrHoles,
  //                        ((double)WM->iNrHoles/WM->getCurrentCycle())*100 );
  // printf("   Teammates seen: %d (%f)\n", WM->iNrTeammatesSeen,
  //                        ((double)WM->iNrTeammatesSeen/WM->getCurrentCycle()));
  // printf("   Opponents seen: %d (%f)\n", WM->iNrOpponentsSeen,
  //                        ((double)WM->iNrOpponentsSeen/WM->getCurrentCycle()));
  

  if (WM->getSide() == SIDE_RIGHT) {
    double matchRatio = Analysis::numMatches / (Analysis::numMatches + Analysis::numMismatches) * 100.0;
    cout << "Agent " << Analysis::agentID << " matches: " << matchRatio << 
    " episodes: " << Analysis::numEpisodes << endl;
  }
}


/********************** SAY **************************************************/

/*!This method determines whether a player should say something.
   \return bool indicating whether the agent should say a message */
bool KeepawayPlayer::shallISaySomething()
{
  bool        bReturn;

  bReturn  = ((WM->getCurrentTime() - m_timeLastSay) >= SS->getHearDecay());
  bReturn  &= (WM->getCurrentCycle() > 0 );

  return bReturn;
}

void KeepawayPlayer::makeSayMessage( SoccerCommand soc, char * strMsg )
{
  VecPosition posBall = WM->getGlobalPosition( OBJECT_BALL );
  VecPosition velBall = WM->getGlobalVelocity( OBJECT_BALL );
  int iDiff = 0;
  SayMsgEncoder myencoder;

  VecPosition posBallPred;
  WM->predictBallInfoAfterCommand( soc, &posBallPred );
  VecPosition posAgentPred = WM->predictAgentPosAfterCommand( soc );

  // If we have good information about the ball
  if( ( WM->getTimeChangeInformation(OBJECT_BALL) == WM->getCurrentTime() &&
        WM->getRelativeDistance( OBJECT_BALL ) < 20.0 &&
        WM->getTimeLastSeen( OBJECT_BALL ) == WM->getCurrentTime() )
      ||
      (
       WM->getRelativeDistance( OBJECT_BALL ) < SS->getVisibleDistance() &&
       WM->getTimeLastSeen( OBJECT_BALL ) == WM->getCurrentTime()  
       )
      ||
      (
       WM->getRelativeDistance( OBJECT_BALL ) < SS->getMaximalKickDist() &&
       posBallPred.getDistanceTo( posAgentPred ) > SS->getMaximalKickDist() 
       )
      ) 
  {
    // If we are kicking the ball 
    if( WM->getRelativeDistance( OBJECT_BALL ) < SS->getMaximalKickDist() )
    {
      // if kick and a pass
      if( soc.commandType == CMD_KICK )
      {
        WM->predictBallInfoAfterCommand( soc, &posBall, &velBall );
        VecPosition posAgent = WM->predictAgentPos( 1, 0 );
        if( posBall.getDistanceTo( posAgent ) > SS->getMaximalKickDist() + 0.2 )
          iDiff = 1;
      }
      
      if( iDiff == 0 )
      {
        posBall = WM->getGlobalPosition( OBJECT_BALL );
        velBall.setVecPosition( 0, 0 );
      }
    }

    LogDraw.logCircle( "ball sending", posBall,
                       1.1, 90, false, COLOR_BLUE );
    
    myencoder.add( new BallInfo( posBall.getX(), posBall.getY(),
                                 velBall.getX(), velBall.getY(), 1 - iDiff ) );
  }

  // Find closest opponent that was seen this cycle
  int numT = WM->getNumTakers();
  ObjectT T[ numT ];
  int numSeen = 0;
  for ( int i = 0; i < numT; i++ ) {
    T[ numSeen ] = SoccerTypes::getOpponentObjectFromIndex( i );
    if ( WM->getRelativeDistance( T[ numSeen ] ) < SS->getVisibleDistance() &&
         WM->getTimeLastSeen( T[ numSeen ] ) == WM->getCurrentTime() )
      numSeen++;  // store this opponent if we just saw him
  }
  WM->sortClosestTo( T, numSeen, WM->getAgentObjectType() ); 

  if ( numSeen > 0 ) { // add closest
    VecPosition posOpp = WM->getGlobalPosition( T[ 0 ] );
    myencoder.add( new OppPos( SoccerTypes::getIndex( T[ 0 ] ) + 1,
                               posOpp.getX(), posOpp.getY(), 1 ) );
  }

  if ( myencoder.getSize() <= 7 &&  // if there is room
       WM->getConfidence( WM->getAgentObjectType() ) > PS->getPlayerHighConfThr() ) {
    myencoder.add( new OurPos( posAgentPred.getX(), posAgentPred.getY() ) );
  }

  strcpy( strMsg, myencoder.getEncodedStr().c_str() );
  myencoder.clear();
}

/********************** Keepaway ***************************************/

SoccerCommand KeepawayPlayer::keeper()
{
  SoccerCommand soc;

  if ( WM->isNewEpisode() ) {
    SA->endEpisode( WM->keeperReward() );
    WM->setNewEpisode( false );
    WM->setLastAction( UnknownIntValue );
    m_timeStartEpisode = WM->getCurrentTime();
  }

  LogDraw.logCircle( "ball belief", WM->getBallPos(),
                     1.1, 11, false, COLOR_RED,
                     WM->getConfidence( OBJECT_BALL ) );
  char buffer[128];
  sprintf( buffer, "%.2f", WM->getConfidence( OBJECT_BALL ) );
  LogDraw.logText( "ball belief", WM->getBallPos(),
                   buffer,
                   11, COLOR_RED );

  // If we don't know where the ball is, search for it.
  if ( WM->getConfidence( OBJECT_BALL ) <
       PS->getBallConfThr() ) {
    ACT->putCommandInQueue( soc = searchBall() );
    ACT->putCommandInQueue( alignNeckWithBody() );
    LogDraw.logText( "state", VecPosition( 25, 25 ),
                     "lost ball",
                     1, COLOR_WHITE );
    return soc;
  }

  // If the ball is kickable,
  // call main action selection routine.
  if ( WM->isBallKickable() ) {
    Log.log( 100, "Ball is kickable for me." );
    return keeperWithBall();
  }

  // Get fastest to ball
  int iTmp;
  ObjectT fastest = WM->getFastestInSetTo( OBJECT_SET_TEAMMATES, 
                                           OBJECT_BALL, &iTmp );
  
  // If fastest, intercept the ball.
  if ( fastest == WM->getAgentObjectType() ) {
    Log.log( 100, "I am fastest to ball; can get there in %d cycles", iTmp );
    LogDraw.logText( "state", VecPosition( 25, 25 ),
                     "fastest",
                     1, COLOR_WHITE );

    ObjectT lookObject = chooseLookObject( 0.98 );

    char buffer[128];
    LogDraw.logText( "lookObject", VecPosition( 25, -25 ), 
                     SoccerTypes::getObjectStr( buffer, lookObject ), 100, COLOR_WHITE );

    ACT->putCommandInQueue( soc = intercept( false ) );
    //ACT->putCommandInQueue( turnNeckToObject( lookObject, soc ) );
    //ACT->putCommandInQueue( turnNeckToPoint( SS->getKeepawayRect().getPosCenter(), soc ) );
    ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
    return soc;
  }

  // Not fastest, get open
  Log.log( 100, "I am not fastest to ball" );
  LogDraw.logText( "state", VecPosition( 25, 25 ),
                   "support",
                   1, COLOR_WHITE );
  return keeperSupport( fastest );
}    

SoccerCommand KeepawayPlayer::keeperWithBall()
{
  double state[ MAX_STATE_VARS ];
  int action;

  if ( WM->keeperStateVars( state ) > 0 ) { // if we can calculate state vars
    // Call startEpisode() on the first SMDP step
    if ( WM->getTimeLastAction() == UnknownTime ) {
      action = SA->startEpisode( state );
    }
    else if ( WM->getTimeLastAction() == WM->getCurrentCycle() - 1 && 
              WM->getLastAction() > 0 ) {   // if we were in the middle of a pass last cycle
      action = WM->getLastAction();         // then we follow through with it
    }
    // Call step() on all but first SMDP step
    else {
      action = SA->step( WM->keeperReward(), state );
    }
    WM->setLastAction( action );
  }
  else { // if we don't have enough info to calculate state vars
    action = 0;  // hold ball 
    LogDraw.logText( "state", VecPosition( 35, 25 ),
                     "clueless",
                     1, COLOR_RED );
  }

  return interpretKeeperAction( action );
}

SoccerCommand KeepawayPlayer::interpretKeeperAction( int action )
{
  SoccerCommand soc;
  
  if ( action == 0 ) { // interpret HOLD action
    ACT->putCommandInQueue( soc = holdBall() );
    LogDraw.logText( "state", VecPosition( 25, 25 ),
                     "holding",
                     1, COLOR_WHITE );
  }
  else { // interpret PASS action
    int numK = WM->getNumKeepers();
    ObjectT K[ numK ];
    for ( int i = 0; i < numK; i++ )
      K[ i ] = SoccerTypes::getTeammateObjectFromIndex( i );
    WM->sortClosestTo( K, numK, WM->getAgentObjectType() );
    VecPosition tmPos = WM->getGlobalPosition( K[ action ] );
    // Normal Passing
    ACT->putCommandInQueue( soc = directPass( tmPos, PASS_NORMAL ) );
    // Or Fast Passing
    //ACT->putCommandInQueue( soc = directPass( tmPos, PASS_FAST ) );
    char buffer[128];
    LogDraw.logText( "state", VecPosition( 25, 25 ),
                     SoccerTypes::getObjectStr( buffer, K[ action ] ),
                     1, COLOR_WHITE );
  }
  
  return soc;
}

SoccerCommand KeepawayPlayer::keeperSupport( ObjectT fastest )
{
  SoccerCommand soc;

  int iCycles = WM->predictNrCyclesToObject( fastest, OBJECT_BALL );
  VecPosition posPassFrom = 
    WM->predictPosAfterNrCycles( OBJECT_BALL, iCycles );
  LogDraw.logCircle( "BallPredict", posPassFrom, 1, 70, true, COLOR_BROWN );
  soc = getOpenForPassFromInRectangle( WM->getKeepawayRect(), posPassFrom );

  ObjectT lookObject = chooseLookObject( 0.97 );

  char buffer[128];
  LogDraw.logText( "lookObject", VecPosition( 25, -25 ), 
                   SoccerTypes::getObjectStr( buffer, lookObject ), 100, COLOR_WHITE );

  ACT->putCommandInQueue( soc );
  //ACT->putCommandInQueue( turnNeckToObject( lookObject, soc ) );
  ACT->putCommandInQueue( turnNeckToPoint( WM->getKeepawayRect().getPosCenter(), soc ) );

  return soc;
}

ObjectT KeepawayPlayer::chooseLookObject( double ballThr )
{
  if ( WM->getConfidence( OBJECT_BALL ) < ballThr )
    return OBJECT_BALL;

  ObjectT objLeast = OBJECT_ILLEGAL;
  double confLeast = 1.1;
  for ( int i = 0; i < WM->getNumKeepers(); i++ ) {
    ObjectT obj = SoccerTypes::getTeammateObjectFromIndex( i );
    if ( obj != WM->getAgentObjectType() ) {
      double conf = WM->getConfidence( obj );
      if ( conf < confLeast ) {
        confLeast = conf;
        objLeast = obj;
      }
    }
  }

  return objLeast;
}

SoccerCommand KeepawayPlayer::taker()
{

  // cout << "-- HANDCRAFTED TAKER! -- \n";

  SoccerCommand soc;

  LogDraw.logCircle( "ball pos", WM->getBallPos(),
                     1.1, 11, false, COLOR_RED, WM->getConfidence( OBJECT_BALL ) );

  // If we don't know where the ball is, search for it.
  if ( WM->getConfidence( OBJECT_BALL ) <
       PS->getBallConfThr() ) {
    ACT->putCommandInQueue( soc = searchBall() );
    ACT->putCommandInQueue( alignNeckWithBody() );
    return soc;
  }

  // Maintain possession if you have the ball.
  if ( WM->isBallKickable() &&
       WM->getClosestInSetTo( OBJECT_SET_TEAMMATES, OBJECT_BALL) ==
       WM->getAgentObjectType() ) {
    ACT->putCommandInQueue( soc = holdBall( 0.3 ) );
    return soc;
  }  

  // If not first or second closest, then mark open opponent
  int numT = WM->getNumTakers();
  ObjectT T[ numT ];
  for ( int i = 0; i < numT; i++ )
    T[ i ] = SoccerTypes::getTeammateObjectFromIndex( i );
  WM->sortClosestTo( T, numT, OBJECT_BALL );
  if ( numT > 2 && T[ 0 ] != WM->getAgentObjectType() &&
       T[ 1 ] != WM->getAgentObjectType() ) {
    ObjectT withBall = WM->getFastestInSetTo( OBJECT_SET_OPPONENTS, 
                                              OBJECT_BALL );
    ACT->putCommandInQueue( soc = markMostOpenOpponent( withBall ) );
    ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
    return soc;
  }

  // If teammate has it, don't mess with it
  double dDist;
  ObjectT closest = WM->getClosestInSetTo( OBJECT_SET_PLAYERS, 
                                           OBJECT_BALL, &dDist );
  if ( SoccerTypes::isTeammate( closest ) &&
       closest != WM->getAgentObjectType() &&
       dDist < SS->getMaximalKickDist() ) {
    ACT->putCommandInQueue( soc = turnBodyToObject( OBJECT_BALL ) );
    ACT->putCommandInQueue( alignNeckWithBody() );
    return soc;
  }
  

  // Otherwise try to intercept the ball
  ACT->putCommandInQueue( soc = intercept( false ) );
  ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
  return soc;
}



SoccerCommand KeepawayPlayer::argumentation_taker() 
{

  // cout << "-- ARGUMENTATION TAKER! -- \n";

  if ( WM->isNewEpisode() ) {
    SA->endEpisode(0);
    WM->setNewEpisode( false );
    WM->setLastAction( UnknownIntValue );
    m_timeStartEpisode = WM->getCurrentTime();
    m_lastTarget = OBJECT_BALL;

    Analysis::numEpisodes++;
    cout << "Episode count: " << Analysis::numEpisodes << endl;
    double matchRatio = Analysis::numMatches / (Analysis::numMatches + Analysis::numMismatches) * 100.0;
    cout << "Agent " << Analysis::agentID << " matchRatio: " << matchRatio << 
    " episodes: " << Analysis::numEpisodes << endl;

    // cout <<"numMatches: " << Analysis::numMatches <<endl;
    // cout <<"numMissmatches: " << Analysis::numMismatches <<endl;

    if (_takerMode == "orla"){
      // update here the ordering.
      string pathOrdering = "/home/candido/keepaway_orla/player/orla/ordering.txt";
      Ordering newOrdering;
      newOrdering.loadOrdering(pathOrdering);
      Analysis::agent->argOrdering = newOrdering;
    }
  }

  SoccerCommand soc;


  // If we don't know where the ball is, search for it.
  if ( WM->getConfidence( OBJECT_BALL ) <
       PS->getBallConfThr() ) {
    ACT->putCommandInQueue( soc = searchBall() );
    ACT->putCommandInQueue( alignNeckWithBody() );
    return soc;
  }

  // Maintain possession if you have the ball.
  if ( WM->isBallKickable() &&
       WM->getClosestInSetTo( OBJECT_SET_TEAMMATES, OBJECT_BALL) ==
       WM->getAgentObjectType() ) {
    ACT->putCommandInQueue( soc = holdBall( 0.3 ) );
    return soc;
  } 

  int n_cycles = 5; 
  int action = -1;
  double state[ MAX_STATE_VARS ];


  ObjectT me = WM->getAgentObjectType();


  if (WM->takerStateVars(me, state) > 0) {

        vector<double> outputs;
        vector<double> inputState;

        for (int i = 0; i < Analysis::n_taker_features; i++) {
          inputState.push_back(state[i]);
        }


        // auto start = std::chrono::high_resolution_clock::now();

        Analysis::agent->processState(inputState, outputs);

        // std::cout << " -- START ";
        // for (auto i: outputs)
        //   std::cout << i << ' ';
        // std::cout << " FINISH -- ";

      // auto finish = std::chrono::high_resolution_clock::now();
      // std::chrono::duration<double> elapsed = finish - start;
      // std::cout << "Elapsed time: " << elapsed.count() * 1000.0 << " s\n";

        action = outputs.at(0);

        // Check matches (for some reason, the MARLeME implementation ignores the initial action, and so do I):
        if (WM->getTimeLastAction() <= WM->getCurrentCycle() - n_cycles) {
          int original_action = SA->step(0, state); 

          int arg_action = (action < 0) ? Analysis::prevAction : action;

          if (arg_action == original_action){
            Analysis::numMatches++;
          } else {
            Analysis::numMismatches++;
          }

          Analysis::prevAction = arg_action;
        }

        WM->setLastAction(action);

        if (action < 0) {
          return repeatLastTakerAction();
        }  
    } else {
        return repeatLastTakerAction();
    }
  return interpretTakerAction(action);
}



// Note: actions are zero-based (i.e. action 0 means 'tackle ball holder'
SoccerCommand KeepawayPlayer::learning_taker()
{    

  // cout << "-- LEARNING TAKER! -- \n";

  if ( WM->isNewEpisode() ) {
    SA->endEpisode(0);
    WM->setNewEpisode( false );
    WM->setLastAction( UnknownIntValue );
    m_timeStartEpisode = WM->getCurrentTime();
    m_lastTarget = OBJECT_BALL;

    Analysis::numEpisodes++;
    cout << "Episode count: " << Analysis::numEpisodes << endl;
    double matchRatio = Analysis::numMatches / (Analysis::numMatches + Analysis::numMismatches) * 100.0;
    cout << "Agent " << Analysis::agentID << " matches: " << matchRatio << 
    " episodes: " << Analysis::numEpisodes << endl;
  }  
    

  SoccerCommand soc;
  
  // If we don't know where the ball is, search for it.
  if ( WM->getConfidence( OBJECT_BALL ) <
       PS->getBallConfThr() ) {
    ACT->putCommandInQueue( soc = searchBall() );
    ACT->putCommandInQueue( alignNeckWithBody() );
    return soc;
  }

  // Maintain possession if you have the ball.
  if ( WM->isBallKickable() &&
       WM->getClosestInSetTo( OBJECT_SET_TEAMMATES, OBJECT_BALL) ==
       WM->getAgentObjectType() ) {
    ACT->putCommandInQueue( soc = holdBall( 0.3 ) );
    return soc;
  } 

  
  int n_cycles = 5; 
  double taker_reward = -5;
  double state[ MAX_STATE_VARS ];
  int action = 0;
  
  ObjectT me = WM->getAgentObjectType();
  
    if ((WM->takerStateVars(me, state) > 0)) {

        vector<double> outputs;
        vector<double> inputState;

        for (int i = 0; i < Analysis::n_taker_features; i++) {
          inputState.push_back(state[i]);
        }

        Analysis::agent->processState(inputState, outputs);

        int chosenAction = outputs.at(0);


        if (WM->getTimeLastAction() == UnknownTime) { // If the episode just started
            action = SA->startEpisode(state);
            
        } else if (WM->getTimeLastAction() <= WM->getCurrentCycle() - n_cycles) {
            action = SA->step(taker_reward, state); 

            Analysis::saveState(state, Analysis::agentID);
            Analysis::saveAction(action, Analysis::agentID);

            // if (Analysis::agentID == 2 && chosenAction != action) {
            //   cout << "True action: " << action << 
            //   " Chosen action: " << chosenAction << endl;
            // }

            if (chosenAction < 0) {
              chosenAction = Analysis::prevAction;
            }

            if (chosenAction == action){
              Analysis::numMatches++;
            } else {
              Analysis::numMismatches++;
            }

            Analysis::prevAction = chosenAction;
                         
        } else {
            return repeatLastTakerAction();    
        }
        WM->setLastAction(action);
        
    } else {
        return repeatLastTakerAction();
    }
    return interpretTakerAction(action);
}


SoccerCommand KeepawayPlayer::repeatLastTakerAction()
{
   SoccerCommand soc;

   if (m_lastTarget == OBJECT_BALL) {
      ACT->putCommandInQueue( soc = intercept( false ) );
   } else {
      ACT->putCommandInQueue( soc = mark( m_lastTarget, 4.0,  MARK_BALL) );
   }
   ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );

   return soc;
}


SoccerCommand KeepawayPlayer::interpretTakerAction( int action )
{
  SoccerCommand soc;
  
  if ( action == 0 ) { // interpret TackleBall() action
    m_lastTarget = OBJECT_BALL;
    ACT->putCommandInQueue( soc = intercept( false ) );
    ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );

  } else { // interpret MarkKeeper() (aka BlockPass()) action
      
    // Order keepers by their distance to the ball 
    int numK = WM->getNumKeepers();
    ObjectT K[ numK ];

    for ( int i = 0; i < numK; i++ ) {
      K[ i ] = SoccerTypes::getOpponentObjectFromIndex( i );
    }
    WM->sortClosestTo( K, numK, OBJECT_BALL );  
    
    // Issue corresponding command
    m_lastTarget = K[action];
    ACT->putCommandInQueue( soc = mark( K[action], 4.0,  MARK_BALL) );
    ACT->putCommandInQueue( turnNeckToObject( OBJECT_BALL, soc ) );
  }
  return soc;
}