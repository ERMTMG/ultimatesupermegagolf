
INCLUDE_DIR="./include"
LIB_DIR="./lib"

CPP_FILES := $(wildcard src/*.cpp)
MAIN := src/main.cpp
COLLISION_TEST := src/tests/collision_test.cpp
BB_CALCULATE_TEST := src/tests/bb_calculate_test.cpp
SIMDJSON_TEST := src/tests/simdjson_test.cpp
SIMDJSON_SOURCE := src/simdjson.cpp
SOURCE_FILES := $(filter-out $(COLLISION_TEST) $(BB_CALCULATE_TEST) $(MAIN), $(CPP_FILES)) 

DEBUG_COMPILER_OPTIONS := -O0 -g -ftemplate-backtrace-limit=0 -Wno-narrowing
RELEASE_COMPILER_OPTIONS := -O3 -Wno-narrowing
COMPILER_OPTIONS := $(RELEASE_COMPILER_OPTIONS)

all: bin/main1

OBJ_FILES := $(patsubst src/%.cpp, obj/%.o, $(SOURCE_FILES))

obj/%.o: src/%.cpp
	g++ -c $< -o $@ -I$(INCLUDE_DIR) -I. -std=c++17 $(COMPILER_OPTIONS)

bin/main1: src/main.cpp $(OBJ_FILES) 
	g++ $(OBJ_FILES) $(MAIN) $(INCLUDE_DIR)/entt.hpp $(COMPILER_OPTIONS) -o bin/main1 -I$(INCLUDE_DIR) -I. -L$(LIB_DIR) -std=c++17 -lraylib

collision_test:
	g++ $(SOURCE_FILES) $(COLLISION_TEST) $(DEBUG_COMPILER_OPTIONS) -o bin/collision_test -I$(INCLUDE_DIR) -I. -L$(LIB_DIR) -std=c++17 -lraylib 

bb_calculate_test: $(BB_CALCULATE_TEST) $(OBJ_FILES)
	g++ $(OBJ_FILES) $(BB_CALCULATE_TEST) $(DEBUG_COMPILER_OPTIONS) -o bin/bb_calculate_test -I$(INCLUDE_DIR) -I. -L$(LIB_DIR) -std=c++17 -lraylib

simdjson_test:
	g++ $(SIMDJSON_TEST) $(SIMDJSON_SOURCE) -o bin/simdjson_test -I$(INCLUDE_DIR) -I. -L$(LIB_DIR) -std=c++17

clean:
	rm bin/*
	rm obj/*.o