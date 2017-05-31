#CXX = g++
CXX = mpicxx
CXXFLAGS = -Wall -W -s -lpthread
.PHONY: default run
default: run

run:
	${CXX} ${CXXFLAGS} Dining_Philosophers.cpp -o program 
	
clean:
	rm -f *.o program
