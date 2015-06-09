.PHONY: all clean
CCC=g++
CCFLAGS=-Wl,--no-as-needed -std=c++11 -pthread 
OBJECTS=cpu 

all: $(OBJECTS)

run: all
	##./cpu 2

cpu: cpu.cpp
	$(CCC) $(CCFLAGS) cpu.cpp -o cpu


