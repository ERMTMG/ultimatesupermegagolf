
INCLUDE_DIR="./include"
LIB_DIR="./lib"

all: bin/main1



bin/main1: src/main.cpp
	g++ src/*.cpp src/DS/*.cpp $(INCLUDE_DIR)/entt.hpp -o bin/main1 -I$(INCLUDE_DIR) -I. -L$(LIB_DIR) -std=c++17 -lraylib -Wno-narrowing
