PWD=$(shell pwd)

NDEBUG_CXXFLAG:=-g -fPIC -Wall 
BASEFLAG:=
CXXFLAGS:= $(NDEBUG_CXXFLAG) $(BASEFLAG)
CFLAGS:=$(CXXFLAGS)
#MYSQLPATH=/usr/lib64
#MYSQLINCLUDE=/usr/include/mysql
INC_BASE=-I$(PWD)/include 
SRC_PATH=$(PWD)/src
OBJ_PATH=$(PWD)/obj
SRCS=$(wildcard $(SRC_PATH)/*.cpp)
OBJS=$(patsubst $(SRC_PATH)/%.cpp, $(OBJ_PATH)/%.o, $(SRCS))
LIBPATH=
LIBSLIST= -lpthread 
#BIN=$(PWD)/liblogout.so
DLIB = $(PWD)/liblogout.so
TLIB = $(PWD)/liblogout.a
all:$(TLIB)

$(TLIB): $(OBJS)
	ar r $@ $?
	ranlib $@

$(DLIB):$(OBJS)
	g++ -m64 -w  --std=c++11 $(CFLAGS) -shared -o $(DLIB)  $(OBJS) $(LIBPATH) $(LIBSLIST)
$(OBJS):$(OBJ_PATH)/%.o:$(SRC_PATH)/%.cpp
	g++ -m64 -w --std=c++11 -c $(CFLAGS) $< -shared -o $@ $(INC_BASE)
.PHONY:clean
clean:
	rm -rf obj/*.o $(BIN)


