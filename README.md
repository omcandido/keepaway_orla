# Keepaway for [ORLA](https://github.com/omcandido/ORLA)

This fork contains files from three different sources: 
1. the original Keepaway library: https://www.cs.utexas.edu/~AustinVilla/sim/keepaway/
2. the original repository: https://github.com/tjpalmer/keepaway
3. ad-hoc files for the MARLeME library: https://github.com/dmitrykazhdan/MARLeME

## Requirements
- rcssserver for ORLA: https://github.com/omcandido/rcssserver_orla
- WSL Ubuntu-16.04
- C++14

## Install
- Clone keepaway_orla in the home directory (i.e., "/home/username").
- Install as per the original readme file (at the end of this file)
- To compile the MARLeME model extractor do ``make -f Makeextractor`` from the player/ folder.

## Setup
The project is not plug-and-play. Many paths are hardcoded, connections are port-/IP-dependent and you may need to do your own adjustments if your setup differs from ours.
ORLA assumes keepaway and rcssserver are installed in the home directory of a WSL2 (Ubuntu-16.04) system. ORLA runs on the windows host (Windows 10). To maximize your chances of succeeding at getting it to work, it is advisable to use the same setup. After that, you should change the hardcoded home paths (I know, ugliness alert) to your own (i.e., /home/candido/.../ to /home/yourusername/...). Then you can compile the main.cc and extractor.cc and you should be ready to go.

## Run modes
You can run the keepaway library in one of the 7 runMode.sh files ``./runMode.sh`` from the root folder. To execute one runMode.sh file you first have to make them executable: ``chmod +x runMode.sh``. The different run modes are:
- handcrafted.sh: takers follow a handcrafted strategy
- sLearning.sh: takers learn using linear SARSA
- sTrained.sh: takers follow the learned SARSA policy (by default, the weights are in ~/keepaway_orla/weights/good_weights)
- mDefault.sh: takers use the default VAF given by the expert (see [Gao & Toni 2014](https://link.springer.com/chapter/10.1007/978-3-642-54373-9_6))
- mExtracted.sh: takers use the VAF extracted by MARLeME (by default the values are in folder ~/keepaway_orla/player/savedOrderings/agent_N.txt)
- oLearning.sh: takers are being trained via ORLA (requires ORLA to connect via sockets to send the start signal. The takers read ~/keepaway_orla/orla/ordering.txt containing the values corresponding to the current episode)
- oTrained.sh: takers follow the values learned by ORLA (in ~/keepaway_orla/orla/ordering.txt) but it does not wait for the start signal and it does not terminate the match early if the same keeper has been holding the ball for more than 10s. 

## Learning a model with SARSA
By running ``./sLearning.sh``, a set of weights will be created by default under ``keepaway_orla/weights/``. When you are happy with the learned models, save the weights under the ``good_weights`` folder to evaluate the learned SARSA models.

## Extracting a model with MARLeME
Once a list of trajectories has been sampled, you should have a list of actions and states in the folders ``player/savedActions`` and ``player/savedStates``, respectively.
You can extract a VAF from those trajectories by moving to the player folder and running ``./extractor``. The extracted ordering will be output to player/savedOrderings by default.

## Learning a model with ORLA
By running ``./oLearning.sh``, the keepaway referee will wait for ORLA to send a start signal. Upon receiving the start signal, the argumentation-based takers read the values of their arguments from ``keepaway_orla/orla/ordering.txt``. When you are happy with the learned values, run ``./oTrained`` to evaluate them without the need for running ORLA.

## Permission issues
Note that you might need to run some of the commands above as sudo, depending on the file permissions you have.

##


_____
### ORIGINAL README:
This project is intended as a public but unofficial repository for updates to
the Keepaway benchmark player framework created at UT Austin by Gregory
Kuhlmann and Peter Stone.

The intention is that this project can serve as a source for updates and
compatibility with ongoing changes to the RoboCup Soccer Server. However, no
promises are made as to the fitness of this software for any purpose
whatsoever.

I have also modified the launch scripts to expect working with in-place builds
of rcss tools rather than system installed ones. The latter might still work,
but I haven't checked on that. I expect sibling dirs to keepaway called
rcssmonitor_qt4 and rcssserver. Use the keepaway.py launch script for most
convenience. I haven't done much to maintain the original keepaway.sh. As an
example, to kick off with default settings and see the action, run like so (if
your system has python and knows shebangs):

  ./keepaway.py --monitor

The original README from Gregory Kuhlmann and Peter Stone follows. Following
that is the original README for the learning code, including the list of
contributors to that code base. See these READMEs also for papers to reference
when using this code.

Also included in this combined code base is the Tile Coding Software version 2
from Richard S. Sutton, included with permission from him. This software, as
well as more recent versions, is also available directly from the author
(http://webdocs.cs.ualberta.ca/~sutton/tiles2.html).

The combined, updated code base has also included assistance from Stephen Kelly
(initial integration of learning code) and Luiz Celiberto (my updates in support
of latest rcss and gcc checked against his), among others.

I have also made some minimal changes to support dynamic loading of shared
library code for agent behavior, thus avoiding the need to change the original
code. See the "ext=" hackery in main.cc for more details.

- Tom Palmer


-------------------
README for Keepaway
-------------------

This package contains the source code for the Keepaway benchmark 
player framework and the source code for some keepaway utility
programs.  The framework includes all low- and mid-level keepaway
behaviors.  A few example high-level policies are included.  Not
included is any learning code.  However, the framework was designed
to make it easy to insert your own learning code.  Also, not included
is the soccer simulator (see REQUIREMENTS).

The framework was created by

 Gregory Kuhlmann
 and Peter Stone

in the Department of Computer Sciences
at the University of Texas at Austin:
http://www.cs.utexas.edu/~AustinVilla/sim/keepaway/

Copyright 2004 Gregory Kuhlmann, Peter Stone
All Rights Reserved

This package was created as part of our research using
the keepaway domain.  The following is a list of selected 
publications:

Keepaway Soccer: From Machine Learning Testbed to Benchmark.
Peter Stone, Gregory Kuhlmann, Matthew E. Taylor, and Yaxin Liu.
In Itsuki Noda, Adam Jacoff, Ansgar Bredenfeld, and Yasutake Takahashi, editors, RoboCup-2005: Robot Soccer World Cup IX, Springer Verlag, Berlin, 2006. To appear.

Reinforcement Learning for RoboCup-Soccer Keepaway.
Peter Stone, Richard S. Sutton, and Gregory Kuhlmann.
Adaptive Behavior, 2005. 

These and other keepaway papers can be found here:
http://www.cs.utexas.edu/~pstone/Papers/bib2html/

We ask that you please cite these papers if you publish work that builds
off of this framework.

The players are built upon version 3.3 of the UvA Basic team that is 
publicly available from:
http://staff.science.uva.nl/~jellekok/robocup/2003/index_en.html

The communication code is based on:
saymsglib - a library to generate and parse messages in the Coachable agent
communication standard created by Carnegie Mellon University.  It is 
described here:
http://www-2.cs.cmu.edu/~robosoccer/simulator/comm_standard.html

We provide this code as-is with no support implied.
It has been tested under Debian Linux 3.1 (testing)

REQUIREMENTS:

The RoboCup Soccer Server, available from:
http://sserver.sf.net

The following package versions are strongly recommended:
  rcssbase    9.4.5
  rcssserver  9.4.5
  rcssmonitor 9.3.7

Gnuplot (optional)


FILES and DIRECTORIES:

README        - This file
COPYING       - GNU Public License
keepaway.sh   - script to start players
player/       - keepaway player source code    
logs/         - dir to store generated log files (initially empty)
weights/      - dir to store learned weights (initially empty)
tools/        - some useful utilities for keepaway experiments


INSTALLATION:

In player/:
make depend
make

In tools/:
make
(copy all binaries to some directory in your PATH)


USAGE:

(Modify "keepaway_dir" in keepaway.sh to match installation dir)
./keepaway.sh

To view players:
monitor

To stop:
killserver


---------------------------------
README for Keepaway learning code
---------------------------------

This package contains the learning code to accompany the Keepaway
benchmark player framework.

This is a particular snapshot of the code, and different published
results have used different learning algorithms.  We don't guarantee
that this particular instantiation will line up with all of the
published papers using keepaway.  Variations on the learning algorithm
are described in the papers indexed here:
http://www.cs.utexas.edu/~pstone/Papers/bib2html/

We provide this code as-is with no support implied. This code has not
been tested and we make no guarantees that it will even compile.

The learning code was created by

 Peter Stone
 Gregory Kuhlmann
 Matthew E. Taylor
 Yaxin Liu
 and Shivaram Kalyanakrishnan

in the Department of Computer Sciences
at the University of Texas at Austin:
http://www.cs.utexas.edu/~AustinVilla/sim/keepaway/

This package was created as part of our research using
the keepaway domain.  The following is a list of selected 
publications:

Keepaway Soccer: From Machine Learning Testbed to Benchmark.
Peter Stone, Gregory Kuhlmann, Matthew E. Taylor, and Yaxin Liu.
In Itsuki Noda, Adam Jacoff, Ansgar Bredenfeld, and Yasutake Takahashi, editors,
RoboCup-2005: Robot Soccer World Cup IX, Springer Verlag, Berlin, 2006. To
appear.

Reinforcement Learning for RoboCup-Soccer Keepaway.
Peter Stone, Richard S. Sutton, and Gregory Kuhlmann.
Adaptive Behavior, 2005. 

We ask that you please cite these papers if you publish work that builds
off of this code.

The players are built upon version 3.3 of the UvA Basic team that is 
publicly available from:
http://staff.science.uva.nl/~jellekok/robocup/2003/index_en.html

The communication code is based on:
saymsglib - a library to generate and parse messages in the Coachable agent
communication standard created by Carnegie Mellon University.  It is 
described here:
http://www-2.cs.cmu.edu/~robosoccer/simulator/comm_standard.html

REQUIREMENTS:

The Keepaway Player Framework
http://www.cs.utexas.edu/~AustinVilla/sim/keepaway/

The RoboCup Soccer Server, available from:
http://sserver.sf.net
