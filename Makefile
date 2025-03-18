
INCLUDE_DIR="./include"
LIB_DIR="./lib"

CPP_FILES := $(wildcard src/*.cpp)
MAIN := src/main.cpp
COLLISION_TEST := src/collision_test.cpp
SOURCE_FILES := $(filter-out $(COLLISION_TEST) $(MAIN), $(CPP_FILES)) src/DS/*.cpp

all: bin/main1



bin/main1: src/main.cpp
	g++ $(SOURCE_FILES) $(MAIN) $(INCLUDE_DIR)/entt.hpp -g -O3 -o bin/main1 -I$(INCLUDE_DIR) -I. -L$(LIB_DIR) -std=c++17 -lraylib -Wno-narrowing

collision_test:
	g++ $(SOURCE_FILES) $(COLLISION_TEST) -g -O3 -o bin/collision_test -I$(INCLUDE_DIR) -I. -L$(LIB_DIR) -std=c++17 -lraylib -Wno-narrowing