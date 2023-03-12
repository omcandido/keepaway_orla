# Makefile for Keepaway tools
# Gregory Kuhlmann, 2002

TARGET = hist winsum kwyzipper average_runs winsum_2
CC = g++

all:: $(TARGET)

hist: hist.cc
	$(CC) hist.cc -o hist

winsum: winsum.cc
	$(CC) winsum.cc -o winsum

average_runs: AverageRuns.cc
	$(CC) AverageRuns.cc -o AverageRuns

winsum_2: winsum_2.cc
	  $(CC) winsum_2.cc -o winsum_2

kwyzipper: kwyzipper.cc
	$(CC) kwyzipper.cc -o kwyzipper

clean:
	rm -f $(TARGET) *~ 1.out 1.hist graph.eps hist.eps
