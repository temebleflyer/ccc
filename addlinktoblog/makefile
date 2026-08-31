PWD=$(shell pwd)

NDEBUG_CXXFLAG:=-O2 -fPIC -Wall 
BASEFLAG:=
CFLAGS:=$(CXXFLAGS)
CXXFLAGS:= $(NDEBUG_CXXFLAG) $(BASEFLAG)
MYSQLPATH=/www/server/mysql/lib
MYSQLINCLUDE=/www/server/mysql/include
INC_BASE=-I$(PWD)/include -I$(PWD)/tinyxml/include -I$(MYSQLINCLUDE)
SRC_PATH=$(PWD)/src
OBJ_PATH=$(PWD)/obj
SRCS=$(wildcard $(SRC_PATH)/*.cpp)
OBJS=$(patsubst $(SRC_PATH)/%.cpp, $(OBJ_PATH)/%.o, $(SRCS))
LIBPATH=-L/usr/bin/mysql -L$(PWD)/lib -L/www/server/mysql/lib
LIBSLIST= -lpthread -lmysqlclient -ltinyxml -llogout
BIN=$(PWD)/classtest
all:$(BIN)
$(BIN):$(OBJS)
	g++ -m64 -w  --std=c++11 $(CFLAGS) -o $(BIN)  $(OBJS) $(LIBPATH) $(LIBSLIST)
$(OBJS):$(OBJ_PATH)/%.o:$(SRC_PATH)/%.cpp
	g++ -m64 -w --std=c++11 -c $(CFLAGS) $< -o $@ $(INC_BASE)
.PHONY:clean
clean:
	rm -rf obj/*.o $(BIN)


