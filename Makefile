DIR_BIN = .
DIR_OBJ = ./obj
DIR_TCP = ./NetServer
DIR_HTP = ./NetServer/http

SRC_TCP = $(wildcard $(DIR_TCP)/*.cpp) 
OBJ_TCP = $(patsubst %.cpp,$(DIR_OBJ)/%.o,$(notdir $(SRC_TCP)))

SRC_HTP = $(wildcard $(DIR_HTP)/*.cpp)
OBJ_HTP = $(patsubst %.cpp,$(DIR_OBJ)/%.o,$(notdir $(SRC_HTP)))

CPP = $(SRC_TCP) $(SRC_HTP)
OBJ = $(OBJ_TCP) $(OBJ_HTP)

CXX_FLAG = -g -std=c++11 -pthread
CC = g++

TARGET = httpserver

$(DIR_BIN)/$(TARGET) : $(OBJ)
	$(CC) $(CXX_FLAG) -o $@ $^

$(DIR_OBJ)/%.o : $(DIR_TCP)/%.cpp
	if [ ! -d $(DIR_OBJ) ];	then mkdir -p $(DIR_OBJ); fi;
	$(CC) $(CXX_FLAG) -c $< -o $@

$(DIR_OBJ)/%.o : $(DIR_HTP)/%.cpp
	if [ ! -d $(DIR_OBJ) ];	then mkdir -p $(DIR_OBJ); fi;
	$(CC) $(CXX_FLAG) -c $< -o $@

.PHONY : clean
clean : 
	-rm -rf $(DIR_OBJ)