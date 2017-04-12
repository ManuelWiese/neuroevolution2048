SOURCE_FOLDER := src/
OBJECTS_FOLDER := obj/
BIN_NAME := neuronal2048

CPP_FILES := $(wildcard $(SOURCE_FOLDER)*.cpp)
OBJ_FILES := $(addprefix $(OBJECTS_FOLDER),$(notdir $(CPP_FILES:.cpp=.o)))
LD_FLAGS := -fopenmp

ifndef CXX
CXX := g++
endif

CC_FLAGS := -std=c++0x -O3 -fopenmp

all: main

debug: CC_FLAGS := -std=c++0x -O0 -g -fopenmp
debug: main

main: $(OBJ_FILES)
	$(CXX) $(LD_FLAGS) -o $(BIN_NAME) $^

obj/%.o: src/%.cpp folder
	$(CXX) $(CC_FLAGS) -c -o $@ $<
folder: 
	mkdir -p $(OBJECTS_FOLDER)
	
clean:
	rm -rf $(OBJECTS_FOLDER)
	rm $(BIN_NAME)
