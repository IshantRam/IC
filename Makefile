CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2

all:
	$(CXX) $(CXXFLAGS) IC.cpp -o bin/IC

run:
	bin/IC "images/David.bmp"

clean:
	rm bin/*

