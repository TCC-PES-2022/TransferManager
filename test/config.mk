# version
VERSION = 0.1

GTEST_ROOT	:=./googletest/googletest
MODULE_ROOT	:= ..
DEP_PATH	:= /opt/fls

CXX				?=
CXXFLAGS 		+= -Wall
CXXFLAGS 		+= -Wextra
CXXFLAGS		+= -pthread
CXXFLAGS 		+= -fprofile-arcs -ftest-coverage --coverage
#CXXFLAGS 		+= -g -ggdb
COBJFLAGS 		:= $(CXXFLAGS) -c
LDFLAGS  		:= -L$(GTEST_ROOT)/lib -lgtest -L$(DEP_PATH)/lib -ltftp -ltftpd
LDLIBS   		:= -lgcov -lpthread
