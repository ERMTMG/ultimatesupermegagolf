
INCLUDE_DIR="./include"
LIB_DIR="./lib"

all: bin/main

bin/main: src/main.cpp
	g++ src/main.cpp src/DS/*.cpp -g -o bin/main -I$(INCLUDE_DIR) -I. -L$(LIB_DIR) -std=c++14
