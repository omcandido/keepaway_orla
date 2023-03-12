#!/bin/bash


# keepaway_dir="$HOME/Part-III-Project/robocup/keepaway"
keepaway_dir="$HOME/robocup/keepaway"
client_dir=$keepaway_dir/player   # directory containing player binary
client=keepaway_player            # name of player binary
weight_dir="$keepaway_dir/weights"  # top-level weight directory

taker_load_dir="4v3_hand"           
taker_learn=0                    # should learning be turned on for takers?
taker_policy="learned"          # policy followed by takers

port=5800
num_keepers=4
num_takers=3
stop_after=-1                     # stop after num episodes
start_learning_after=-1           # start learning after num episodes

client_opts="-p $port -k $num_keepers -j $num_takers -x $stop_after -y $start_learning_after"

taker_opts="-e $taker_learn -q $taker_policy"

tweight_opts="-w $weight_dir/$taker_load_dir/"

tcmd_line="./$client $client_opts $taker_opts $tweight_opts -g 0 -m 1"

$tcmd_line



