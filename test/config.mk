# version
VERSION = 0.1

GTEST_ROOT	:=./googletest
DESTDIR 	?= /tmp
DEP_PATH 	?= $(DESTDIR)

CXX				?=
CXXFLAGS 		+= -Wall
CXXFLAGS 		+= -Wextra
CXXFLAGS		+= -pthread
CXXFLAGS 		+= -fprofile-arcs -ftest-coverage --coverage
COBJFLAGS 		:= $(CXXFLAGS) -c
LDFLAGS  		:= -L$(GTEST_ROOT)/lib -L$(DEP_PATH)/lib
LDLIBS   		:= -ltransfer -ltftp -ltftpd -lgtest -fprofile-arcs -lgcov -lpthread
INCFLAGS 		:= -I$(DEP_PATH)/include

debug: COBJFLAGS 		+= $(DBGFLAGS)
debugdeps: DEP_RULE    	:= debug
testdeps: DEP_RULE    	:= test
