# This Makefile shows nicely how to compile and link applications
# using the ROOT libraries on all supported platforms.
#
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Fons Rademakers, 29/2/2000

# Extensions des fichiers
ObjSuf        = o
SrcSuf        = cxx
ExeSuf        =
DllSuf        = so
OutPutOpt     = -o 

# Compilateur et options
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC 
LD            = g++
SOFLAGS       = -shared 
LDFLAGS       = -O 

# Options de ROOT
ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)

#CXXFLAGS     += $(ROOTCFLAGS) $(OPTFLAGS)
CXXFLAGS     += $(ROOTCFLAGS)
LIBS          = $(ROOTLIBS) $(SYSLIBS) 
GLIBS         = $(ROOTGLIBS) $(SYSLIBS) 

# Specify Shared libraries
#-------------------------
SHARELIB      = libDAQ.so 

all:            $(SHARELIB)

# Create dictionaries
#-------------------------
libDAQDict.cxx:	libDAQ.h  Linkdef.h
			rootcint -f $@ -c $^
			
libDAQ.so:	libDAQ.o   libDAQDict.o
		$(LD) $(SOFLAGS) $^ $(OutPutOpt) $@



#------------------------------------------------------------------------------
clean:
	@rm -f core *~ *.o *Dict*

distclean:
	  make clean; rm -f *.so

.SUFFIXES: .$(SrcSuf)

.$(SrcSuf).$(ObjSuf):
	$(CXX) $(CXXFLAGS) -c $<


# Manage dependancies
#-------------------------
libDAQ.o:		libDAQ.cxx		libDAQ.h
