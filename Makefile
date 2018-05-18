CXX = g++
CXXFLAGS = -std=c++11 -g

main: project.o linkedlist.o action.o parse.o
	$(CXX) $(CXXFLAGS) -o main project.o linkedlist.o action.o parse.o

project.o: project.cpp parse.cpp project.hpp
	$(CXX) $(CXXFLAGS) -c project.cpp parse.cpp

linkedlist.o: linkedlist.cpp linkedlist.hpp project.hpp
	$(CXX) $(CXXFLAGS) -c linkedlist.cpp
	
parse.o: parse.cpp project.hpp 
	$(CXX) $(CXXFLAGS) -c parse.cpp
	
action.o: action.cpp linkedlist.cpp linkedlist.hpp project.hpp
	$(CXX) $(CXXFLAGS) -c action.cpp linkedlist.cpp

clean:
	rm *.o main

