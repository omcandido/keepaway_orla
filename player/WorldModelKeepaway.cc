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

#include <assert.h>

#include "WorldModel.h"

int WorldModel::getNumKeepers()
{
  return m_numKeepers;
}

void WorldModel::setNumKeepers( int iNum )
{
  m_numKeepers = iNum;
}

int WorldModel::getNumTakers()
{
  return m_numTakers;
}

void WorldModel::setNumTakers( int iNum )
{
  m_numTakers = iNum;
}

double WorldModel::congestion( VecPosition pos, bool considerMe ) 
{
  double congest = 0;
  if ( considerMe && pos != getAgentGlobalPosition() ) 
    congest = 1 / getAgentGlobalPosition().getDistanceTo( pos );

  VecPosition playerPos;

  int iIndex;
  for( ObjectT obj = iterateObjectStart( iIndex, OBJECT_SET_PLAYERS );
       obj != OBJECT_ILLEGAL;
       obj = iterateObjectNext ( iIndex, OBJECT_SET_PLAYERS ) ) {
    if ( ( playerPos = getGlobalPosition( obj ) ) != pos )
      if ( obj != getAgentObjectType() )
      /* Don't want to count a player in its own congestion measure */
      congest += 1/playerPos.getDistanceTo( pos );
  }

  return congest;
}

void WorldModel::resetEpisode()
{
  LogDraw.logText( "episode", VecPosition( 0, 0 ), "Reset", 
		      40, COLOR_WHITE );
  Ball.setTimeLastSeen( -1 );
  for ( int i = 0; i < MAX_TEAMMATES; i++ )
    Teammates[i].setTimeLastSeen( -1 );
  for ( int i = 0; i < MAX_OPPONENTS; i++ )
    Opponents[i].setTimeLastSeen( -1 );
  for ( int i = 0; i < MAX_TEAMMATES+MAX_OPPONENTS; i++ )
    UnknownPlayers[i].setTimeLastSeen( -1 );
  iNrUnknownPlayers = 0;
  for ( int i = 0; i < MAX_FLAGS; i++ )
    Flags[i].setTimeLastSeen( -1 );
  for ( int i = 0; i < MAX_LINES; i++ )
    Lines[i].setTimeLastSeen( -1 );

  setNewEpisode( true );
}

void WorldModel::setNewEpisode( bool bNewEp )
{
  m_newEpisode = bNewEp;
}

bool WorldModel::isNewEpisode()
{
  return m_newEpisode;
}

double WorldModel::keeperReward()
{
  double reward = getCurrentCycle() - getTimeLastAction();
  return reward;
}

void WorldModel::setLastAction( int iAction )
{
  m_lastAction = iAction;
  m_timeLastAction = 
    ( iAction == UnknownIntValue ) ? UnknownTime : getCurrentCycle();
}

int WorldModel::getLastAction()
{
  return m_lastAction;
}

int WorldModel::getTimeLastAction()
{
  return m_timeLastAction;
}

int WorldModel::keeperStateVarsForTaker( double state[] )
{
  ObjectT PB = getClosestInSetTo( OBJECT_SET_OPPONENTS, OBJECT_BALL );
  if ( !SoccerTypes::isOpponent( PB ) )
    return 0;

  VecPosition C = getKeepawayRect().getPosCenter();

  double WB_dist_to_C = getGlobalPosition( PB ).getDistanceTo( C );
  
  int numK = getNumKeepers();
  int numT = getNumTakers();

  ObjectT K[ numK ];
  for ( int i = 0; i < numK; i++ )
    K[ i ] = SoccerTypes::getOpponentObjectFromIndex( i );

  ObjectT T[ numT ];
  for ( int i = 0; i < numT; i++ )
    T[ i ] = SoccerTypes::getTeammateObjectFromIndex( i );

  double WB_dist_to_K[ numK ];
  if ( !sortClosestTo( K, numK, PB, WB_dist_to_K ) )
    return 0;
  
  double WB_dist_to_T[ numT ];
  if ( !sortClosestTo( T, numT, PB, WB_dist_to_T ) )
    return 0;

  double dist_to_C_K[ numK ];
  for ( int i = 1; i < numK; i++ ) {
    dist_to_C_K[ i ] = getGlobalPosition( K[ i ] ).getDistanceTo( C );
  }

  double dist_to_C_T[ numT ];
  for ( int i = 0; i < numT; i++ ) {
    dist_to_C_T[ i ] = getGlobalPosition( T[ i ] ).getDistanceTo( C );
  }

  double nearest_Opp_dist_K[ numK ];
  VecPosition posPB = getGlobalPosition( PB );
  for ( int i = 1; i < numK; i++ ) {
    VecPosition pos = getGlobalPosition( K[ i ] );
    for ( int j = 0; j < numT; j++ ) {
      double tmp = getGlobalPosition( T[ j ] ).getDistanceTo( pos );
      if ( j == 0 || tmp < nearest_Opp_dist_K[ i ] ) {
        nearest_Opp_dist_K[ i ] = tmp;
      }
    }
  }

  double nearest_Opp_ang_K[ numK ];
  for ( int i = 1; i < numK; i++ ) {
    VecPosition pos = getGlobalPosition( K[ i ] );
    for ( int j = 0; j < numT; j++ ) {
      double tmp = posPB.getAngleBetweenPoints( pos, getGlobalPosition( T[ j ] ) );
      if ( j == 0 || tmp < nearest_Opp_ang_K[ i ] ) {
        nearest_Opp_ang_K[ i ] = tmp;
      }
    }
  }

  int j = 0;
  state[ j++ ] = WB_dist_to_C;
  for ( int i = 1; i < numK; i++ )
    state[ j++ ] = WB_dist_to_K[ i ];
  for ( int i = 0; i < numT; i++ )
    state[ j++ ] = WB_dist_to_T[ i ];
  for ( int i = 1; i < numK; i++ )
    state[ j++ ] = dist_to_C_K[ i ];
  for ( int i = 0; i < numT; i++ )
    state[ j++ ] = dist_to_C_T[ i ];
  for ( int i = 1; i < numK; i++ )
    state[ j++ ] = nearest_Opp_dist_K[ i ];
  for ( int i = 1; i < numK; i++ )
    state[ j++ ] = nearest_Opp_ang_K[ i ];

  return j;
}

int WorldModel::keeperStateVars( double state[] )
{
  ObjectT PB = getClosestInSetTo( OBJECT_SET_TEAMMATES, OBJECT_BALL );
  if ( !SoccerTypes::isTeammate( PB ) ) {
      std::cout << "Ball holder is not a keeper" << std::endl;
     return 0; 
  }
    

  VecPosition C = getKeepawayRect().getPosCenter();

  double WB_dist_to_C = getGlobalPosition( PB ).getDistanceTo( C );
  
  int numK = getNumKeepers();
  int numT = getNumTakers();

  ObjectT K[ numK ];
  for ( int i = 0; i < numK; i++ )
    K[ i ] = SoccerTypes::getTeammateObjectFromIndex( i );

  ObjectT T[ numT ];
  for ( int i = 0; i < numT; i++ )
    T[ i ] = SoccerTypes::getOpponentObjectFromIndex( i );

  double WB_dist_to_K[ numK ];
  if ( !sortClosestTo( K, numK, PB, WB_dist_to_K ) ) {
//      std::cout << "Could not sort keepers (keeper)" << std::endl;
    return 0;
  }
  
  double WB_dist_to_T[ numT ];
  if ( !sortClosestTo( T, numT, PB, WB_dist_to_T ) ) {
//      std::cout << "Could not sort takers (keeper)" << std::endl;
    return 0;
  }

  double dist_to_C_K[ numK ];
  for ( int i = 1; i < numK; i++ ) {
    dist_to_C_K[ i ] = getGlobalPosition( K[ i ] ).getDistanceTo( C );
  }

  double dist_to_C_T[ numT ];
  for ( int i = 0; i < numT; i++ ) {
    dist_to_C_T[ i ] = getGlobalPosition( T[ i ] ).getDistanceTo( C );
  }

  double nearest_Opp_dist_K[ numK ];
  VecPosition posPB = getGlobalPosition( PB );
  for ( int i = 1; i < numK; i++ ) {
    VecPosition pos = getGlobalPosition( K[ i ] );
    for ( int j = 0; j < numT; j++ ) {
      double tmp = getGlobalPosition( T[ j ] ).getDistanceTo( pos );
      if ( j == 0 || tmp < nearest_Opp_dist_K[ i ] ) {
        nearest_Opp_dist_K[ i ] = tmp;
      }
    }
  }

  double nearest_Opp_ang_K[ numK ];
  for ( int i = 1; i < numK; i++ ) {
    VecPosition pos = getGlobalPosition( K[ i ] );
    for ( int j = 0; j < numT; j++ ) {
      double tmp = posPB.getAngleBetweenPoints( pos, getGlobalPosition( T[ j ] ) );
      if ( j == 0 || tmp < nearest_Opp_ang_K[ i ] ) {
        nearest_Opp_ang_K[ i ] = tmp;
      }
    }
  }

  int j = 0;
  state[ j++ ] = WB_dist_to_C;
  for ( int i = 1; i < numK; i++ )
    state[ j++ ] = WB_dist_to_K[ i ];
  for ( int i = 0; i < numT; i++ )
    state[ j++ ] = WB_dist_to_T[ i ];
  for ( int i = 1; i < numK; i++ )
    state[ j++ ] = dist_to_C_K[ i ];
  for ( int i = 0; i < numT; i++ )
    state[ j++ ] = dist_to_C_T[ i ];
  for ( int i = 1; i < numK; i++ )
    state[ j++ ] = nearest_Opp_dist_K[ i ];
  for ( int i = 1; i < numK; i++ )
    state[ j++ ] = nearest_Opp_ang_K[ i ];

  return j;
}

int WorldModel::takerStateVars( ObjectT me, double state[] )
{
  
  // Retrieve current ball-holder
  ObjectT ball_holder = getClosestInSetTo( OBJECT_SET_OPPONENTS, OBJECT_BALL );
  VecPosition ball_holder_pos = getGlobalPosition( ball_holder );
  
  // Ensure ball-holder is an opponent
  if ( !SoccerTypes::isOpponent( ball_holder ) ) {
    //std::cout << "(Taker) Ball-holder is not an opponent" << std::endl;
    return 0;
  }
  
  int numK = getNumKeepers();
  int numT = getNumTakers();
  
  // Retrieve position of current taker
  VecPosition my_pos = getGlobalPosition(me);

  // Retrieve keepers
  ObjectT K[ numK ];
  for ( int i = 0; i < numK; i++ )
    K[ i ] = SoccerTypes::getOpponentObjectFromIndex( i );

  // Retrieve takers
  ObjectT T[ numT ];
  for ( int i = 0; i < numT; i++ )
    T[ i ] = SoccerTypes::getTeammateObjectFromIndex( i );

  // Sort keepers by distance from the ball
  if ( !sortClosestTo( K, numK, OBJECT_BALL ) ) {
    //std::cout << "Could not sort keepers (taker)" << std::endl;
    return 0;
  }
  
  // Sort takers by distance from the ball
  if ( !sortClosestTo( T, numT, OBJECT_BALL ) ) {
      //std::cout << "Could not sort takers (taker)" << std::endl;
      return 0;
  }
    
  
  
  // State computation:
  
  // Distance between keepers and myself
  double me_dist_to_K[ numK ];
  for (int i = 0; i < numK; i++) {
    me_dist_to_K[i] = getGlobalPosition( K[ i ] ).getDistanceTo( my_pos );
  }
  
  // Distance between other takers and myself.
  double me_dist_to_T[ numT - 1 ];
  int skip = 0;
  for (int i = 0; i < numT; i++) {
      if (T[i] == me) {
          skip = 1;
      } else {
          me_dist_to_T[i - skip] = getGlobalPosition( T[ i ] ).getDistanceTo( my_pos );
      }
  }
  
   // The angle between the free keepers and myself, with vertex at K1.
  double me_ang_to_K[ numK - 1];
  for ( int i = 1; i < numK; i++ ) {
    me_ang_to_K[ i - 1 ] = ball_holder_pos.getAngleBetweenPoints(getGlobalPosition( K[ i ] ), my_pos);
  }
  
  // Distance between K1 and the other keepers.
  double k1_dist_to_k[ numK - 1];
  for ( int i = 1; i < numK; i++ ) {
    k1_dist_to_k[ i - 1 ] = getGlobalPosition(K[i]).getDistanceTo(ball_holder_pos);
  }

  // Distance between K1 and the other takers.
  double k1_dist_to_t[ numT - 1];
  skip = 0;
  for ( int i = 0; i < numT; i++ ) {
      if (T[i] == me) {
          skip = 1;
      } else {
          k1_dist_to_t[ i - skip ] = getGlobalPosition(T[i]).getDistanceTo(ball_holder_pos);
      }
  }
  
  // The smallest angle between Kp and the takers with vertex at K1.
  double nearest_Opp_ang_K[numK - 1];
  for ( int i = 1; i < numK; i++ ) {
    VecPosition pos = getGlobalPosition( K[ i ] );
    for ( int j = 0; j < numT; j++ ) {
      double tmp = ball_holder_pos.getAngleBetweenPoints( pos, getGlobalPosition( T[ j ] ) );
      if ( j == 0 || tmp < nearest_Opp_ang_K[ i - 1 ] ) {
        nearest_Opp_ang_K[ i - 1 ] = tmp;
      }
    }
  }

  
  // Now copy the values to the state
  int j = 0;
  
  for (int i = 0; i < numK; i++)
      state[j++] = me_dist_to_K[i];
  
  for (int i = 0; i < numT - 1; i++)
      state[j++] = me_dist_to_T[i];
  
  for (int i = 0; i < numK - 1; i++)
      state[j++] = me_ang_to_K[i];
              
  for (int i = 0; i < numK - 1; i++)
      state[j++] = k1_dist_to_k[i];

  for (int i = 0; i < numT - 1; i++)
      state[j++] = k1_dist_to_t[i];
  
  for (int i = 0; i < numK - 1; i++)
      state[j++] = nearest_Opp_ang_K[i];

  return j;
}

// Greg:
// This really doesn't belong here,
// because it is related to a specific 
// learner.  I don't know exactly
// where to put it, though because
// I want to keep the LinearSarsa
// class generic.

// Yaxin: changed from keeperTileWidths to keeperResolutions and keeperRanges,

int WorldModel::keeperStateRangesAndResolutions( double ranges[], 
						 double minValues[], 
						 double resolutions[], 
						 int numK, int numT )
{
  if ( numK < 3 ) {
    cerr << "keeperTileWidths: num keepers must be at least 3, found: " 
	 << numK << endl;
    return 0;
  }
  
  if ( numT < 2 ) {
    cerr << "keeperTileWidths: num takers must be at least 2, found: " 
	 << numT << endl;
    return 0;
  }

  int j = 0;

  double maxRange = hypot( 40, 40 );
			  
  ranges[ j ] = maxRange / 2.0;        // WB_dist_to_center           
  minValues[ j ] = 0;
  resolutions[ j++ ] = 2.0; 
  for ( int i = 1; i < numK; i++ ) {     // WB_dist_to_T          
    ranges[ j ] = maxRange;
    minValues[ j ] = 0;
    resolutions[ j++ ] = 2.0 + ( i - 1 ) / ( numK - 2 );
  }
  for ( int i = 0; i < numT; i++ ) {     // WB_dist_to_O
    ranges[ j ] = maxRange;
    minValues[ j ] = 0;
    resolutions[ j++ ] = 3.0 + ( i - 1 ) / ( numT - 1 );
  }
  for ( int i = 1; i < numK; i++ ) {     // dist_to_center_T    
    ranges[ j ] = maxRange / 2.0;
    minValues[ j ] = 0;
    resolutions[ j++ ] = 2.0 + ( i - 1 ) / ( numK - 2 );
  }
  for ( int i = 0; i < numT; i++ ) {     // dist_to_center_O  
    ranges[ j ] = maxRange / 2.0;
    minValues[ j ] = 0;
    resolutions[ j++ ] = 3.0;
  }
  for ( int i = 1; i < numK; i++ ) {     // nearest_Opp_dist_T 
    ranges[ j ] = maxRange / 2.0;
    minValues[ j ] = 0;
    resolutions[ j++ ] = 4.0;
  }
  for ( int i = 1; i < numK; i++ ) {     // nearest_Opp_ang_T  
    ranges[ j ] = 180;
    minValues[ j ] = 0;
    resolutions[ j++ ] = 10.0;
  }

  return j;
}



int WorldModel::takerStateRangesAndResolutions( double ranges[], 
						 double minValues[], 
						 double resolutions[], 
						 int numK, int numT)
{
  if ( numK < 3 ) {
    cerr << "takerTileWidths: num keepers must be at least 3, found: " 
	 << numK << endl;
    return 0;
  }
  
  if ( numT < 2 ) {
    cerr << "takerTileWidths: num takers must be at least 2, found: " 
	 << numT << endl;
    return 0;
  }

  /*
   Note: Tile widths set somewhat arbitrarily
   */
  
  
  // Hard-coded field size
  int field_size = 40;
  
  int j = 0;
  double maxRange = hypot( field_size, field_size );

  // Distance between keepers and myself.
  for (int i = 0; i < numK; i++) {  
    ranges[ j ] = maxRange;
    minValues[ j ] = 0;
    resolutions[ j++ ] = 4.0;       
  }
  
  // Distance between other takers and myself.
  for (int i = 0; i < numT - 1; i++) {  
    ranges[ j ] = maxRange;
    minValues[ j ] = 0;
    resolutions[ j++ ] = 4.0;       
  }  
  
  // The angle between the free keepers and myself, with vertex at K1.
  for (int i = 0; i < numK - 1; i++) {  
    ranges[ j ] = 180;
    minValues[ j ] = 0;
    resolutions[ j++ ] = 10.0;      
  }   
  
  // Distance between K1 and the other keepers.
  for (int i = 1; i < numK; i++) {
    ranges[ j ] = maxRange;
    minValues[ j ] = 0;
    resolutions[ j++ ] = 4.0;    
  } 
  
  // Distance between K1 and the other takers.
  for (int i = 1; i < numT; i++) {  
    ranges[ j ] = maxRange;
    minValues[ j ] = 0;
    resolutions[ j++ ] = 4.0;    
  }
  
  // The smallest angle between Kp and the takers with vertex at K1.
  for (int i = 0; i < numK - 1; i++) {  
    ranges[ j ] = 180;
    minValues[ j ] = 0;
    resolutions[ j++ ] = 10.0;
  }
  
  return j;
}


void WorldModel::setMoveSpeed( double speed )
{
  m_moveSpeed = speed;
}

double WorldModel::getMoveSpeed()
{
  return m_moveSpeed;
}

void WorldModel::setKeepawayRect( VecPosition pos1, VecPosition pos2 )
{
  m_keepawayRect.setRectanglePoints( pos1, pos2 );
}

Rect WorldModel::getKeepawayRect()
{
  return m_keepawayRect;
}

