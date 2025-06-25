
INCLUDE_DIR="./include"
LIB_DIR="./lib"

CPP_FILES := $(wildcard src/*.cpp)
MAIN := src/main.cpp
COLLISION_TEST := src/collision_test.cpp
BB_CALCULATE_TEST := src/bb_calculate_test.cpp
SOURCE_FILES := $(filter-out $(COLLISION_TEST) $(BB_CALCULATE_TEST) $(MAIN), $(CPP_FILES)) 
all: bin/main1

OBJ_FILES := $(patsubst src/%.cpp, obj/%.o, $(SOURCE_FILES))

obj/%.o: src/%.cpp
	g++ -c $< -o $@ -I$(INCLUDE_DIR) -I. -std=c++17 -O3

bin/main1: src/main.cpp $(OBJ_FILES)
	g++ $(OBJ_FILES) $(MAIN) $(INCLUDE_DIR)/entt.hpp -O3 -o bin/main1 -I$(INCLUDE_DIR) -I. -L$(LIB_DIR) -std=c++17 -lraylib -Wno-narrowing

collision_test:
	g++ $(SOURCE_FILES) $(COLLISION_TEST) -g -O3 -o bin/collision_test -I$(INCLUDE_DIR) -I. -L$(LIB_DIR) -std=c++17 -lraylib -Wno-narrowing

bb_calculate_test: $(BB_CALCULATE_TEST) $(OBJ_FILES)
	g++ $(OBJ_FILES) $(BB_CALCULATE_TEST) -g -O3 -o bin/bb_calculate_test -I$(INCLUDE_DIR) -I. -L$(LIB_DIR) -std=c++17 -lraylib -Wno-narrowing