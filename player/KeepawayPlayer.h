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

#ifndef KEEPAWAY_PLAYER
#define KEEPAWAY_PLAYER

#include "BasicPlayer.h"
#include "SMDPAgent.h"

/*! This class is a superclass from BasicPlayer and contains a more
    sophisticated decision procedure to determine the next action. */
class KeepawayPlayer:public BasicPlayer
{
 protected:
  bool          bContLoop;               /*!< is server is alive             */

  Time          m_timeLastSay;           /*!< last time communicated         */
  Time          m_timeStartEpisode;
  ObjectT       m_lastTarget;
  SMDPAgent     *SA;

  string          _takerMode;

  // methods associated with saying (defined in KeepawayPlayer.cc)
  bool          shallISaySomething        (                                  );
  void          makeSayMessage            ( SoccerCommand  soc,
					    char *         str               );

 public:
  KeepawayPlayer                          ( SMDPAgent      *sa,
					    ActHandler     *a,
                                            WorldModel     *wm,
                                            ServerSettings *ss,
                                            PlayerSettings *cs,
                                            char           *strTeamName,
					    int            iNumKeepers,
					    int            iNumTakers,
                                            double         dVersion,
                                            int            iReconnect = -1,
                                            string         takerMode = "handcrafted" );

  void          mainLoop                  (                                  );

  // behaviors
  SoccerCommand keeper();
  SoccerCommand keeperWithBall();
  SoccerCommand keeperSupport( ObjectT fastest );
  SoccerCommand interpretKeeperAction( int action );

  ObjectT chooseLookObject( double ballThr );

  SoccerCommand taker();
  SoccerCommand learning_taker();
  SoccerCommand argumentation_taker();
  SoccerCommand interpretTakerAction( int action );
  SoccerCommand repeatLastTakerAction();
};

#endif

