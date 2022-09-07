# version
VERSION = 0.1

# paths
DEST 	:= /opt/fls
DEPS 	:= atftp

INSTALL_PATH 	:= $(DEST)
DEP_PATH 		:= $(DEST)

CXX 		?=
CXXFLAGS 	:= -Wall -Werror -std=c++11
DBGFLAGS 	:= -g -ggdb
TESTFLAGS 	:= -fprofile-arcs -ftest-coverage --coverage -lgcov
#LINKFLAGS 	:= -shared
#LDFLAGS  	:= $(addprefix -L, $(basename $(DEP_PATH)))
#LDLIBS   	:= -ltftp -ltftpd

COBJFLAGS 	:= $(CXXFLAGS) -c
test: COBJFLAGS 	+= $(TESTFLAGS)
debug: COBJFLAGS 	+= $(DBGFLAGS)
