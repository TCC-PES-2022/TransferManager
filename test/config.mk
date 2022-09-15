# version
VERSION = 0.1

GTEST_ROOT	:=./googletest
DEP_PATH	:= /opt/fls

CXX				?=
CXXFLAGS 		+= -Wall
CXXFLAGS 		+= -Wextra
CXXFLAGS		+= -pthread
CXXFLAGS 		+= -fprofile-arcs -ftest-coverage --coverage
#CXXFLAGS 		+= -g -ggdb
COBJFLAGS 		:= $(CXXFLAGS) -c
LDFLAGS  		:= -L$(GTEST_ROOT)/lib -L$(DEP_PATH)/lib
LDLIBS   		:= -ltransfer -ltftp -ltftpd -lgtest -lgcov -lpthread
INCFLAGS 		:= -I$(GTEST_ROOT)/include -I$(DEP_PATH)/include