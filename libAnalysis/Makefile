# This Makefile shows nicely how to compile and link applications
# using the ROOT libraries on all supported platforms.
#
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Original Author: Fons Rademakers, 29/2/2000
# Modified       : Mohamad Moukaddam // moukaddam@triumf.ca


# Extensions des fichiers
ObjSuf        = o
SrcSuf        = cpp
ExeSuf        =
DllSuf        = so
OutPutOpt     = -o 

# Compilateur et options
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC -std=c++0x
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
SHARELIB      = libMielData.so libMielHit.so libMielEvent.so
all:            $(SHARELIB)

# Create dictionaries
#-------------------------
TMielDataDict.cpp:	TMielData.h 
			rootcint -f $@ -c $^		
libMielData.so:	TMielData.o   TMielDataDict.o
		$(LD) $(SOFLAGS) $^ $(OutPutOpt) $@
#---
TMielHitDict.cpp:	TMielHit.h 
			rootcint -f $@ -c $^	
libMielHit.so:	TMielHit.o   TMielHitDict.o
		$(LD) $(SOFLAGS) $^ $(OutPutOpt) $@
#---		
TMielEventDict.cpp:	TMielEvent.h 
			rootcint -f $@ -c $^	
libMielEvent.so:	TMielEvent.o   TMielEventDict.o   TMielHit.o   TMielHitDict.o
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
TMielData.o:		TMielData.cpp		TMielData.h
TMielHit.o:			TMielHit.cpp		TMielHit.h
TMielEvent.o:		TMielEvent.cpp		TMielEvent.h     TMielHit.h
