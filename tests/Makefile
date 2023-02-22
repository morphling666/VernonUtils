
CC=gcc
CXX=g++

CXXFLAGS=-O2 -I../  -std=c++17  -L/usr/local/Cellar/jsoncpp/1.9.5/lib -ljsoncpp


TARGETS:= serialize_test
#gltf_test texture_test

all: $(TARGETS)

PHONY:

serialize_test: serialize_test.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm $(TARGETS)
