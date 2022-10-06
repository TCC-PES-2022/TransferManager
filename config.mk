# version
VERSION = 0.1

# paths
DESTDIR 	?= /tmp
DEP_PATH 	?= $(DESTDIR)

DEPS 		:= atftp
LIB_DEPS	:= libtftp.a libtftpd.a

AR 			?= ar
ARFLAGS		:= rcs
CXX 		?=
CXXFLAGS 	:= -Wall -Werror -std=c++11
DBGFLAGS 	:= -g -ggdb
TESTFLAGS 	:= -fprofile-arcs -ftest-coverage --coverage

COBJFLAGS 	:= $(CXXFLAGS) -c
test: COBJFLAGS 	+= $(TESTFLAGS)
test: LINKFLAGS 	+= -fprofile-arcs -lgcov
debug: COBJFLAGS 	+= $(DBGFLAGS)
