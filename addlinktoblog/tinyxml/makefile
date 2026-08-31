PWD=$(shell pwd)

NDEBUG_CXXFLAG:=-O2 -fPIC -Wall 
BASEFLAG:=
CFLAGS:=$(CXXFLAGS)
CXXFLAGS:= $(NDEBUG_CXXFLAG) $(BASEFLAG)

INC_BASE=-I$(PWD)/include 
SRC_PATH=$(PWD)/src
OBJ_PATH=$(PWD)/obj
SRCS=$(wildcard $(SRC_PATH)/*.cpp)
OBJS=$(patsubst $(SRC_PATH)/%.cpp, $(OBJ_PATH)/%.o, $(SRCS))
BIN=$(PWD)/libtinyxml.a
all:$(BIN)
$(BIN):$(OBJS)
	ar cr $(BIN) $(OBJS)
$(OBJS):$(OBJ_PATH)/%.o:$(SRC_PATH)/%.cpp
	g++ -m64 -w --std=c++11 -c $(CFLAGS) $< -o $@ $(INC_BASE)
.PHONY:clean
clean:
	rm -rf *.o libtinyxml.a


