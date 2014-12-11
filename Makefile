# This is a make file for OMCBoost package
# Amir Saffari, amir@ymer.org
#-----------------------------------------

# Compiler options
CC = g++

INCLUDEPATH = -I/usr/local/include -I$(HOME)/local/include
LINKPATH = -L/usr/local/lib -L$(HOME)/local/lib

# OPTIMIZED
CFLAGS = -c -O3 -Wall -march=native -mtune=native -DNDEBUG -Wno-deprecated
LDFLAGS = -lconfig++

# Source directory and files
SOURCEDIR = src
HEADERS := $(wildcard $(SOURCEDIR)/*.h)
SOURCES := $(wildcard $(SOURCEDIR)/*.cpp)
OBJECTS := $(SOURCES:.cpp=.o)
LIBLARANKDIR := $(SOURCEDIR)/linear_larank

# Target output
BUILDTARGET = OMCBoost

# Build
all: $(BUILDTARGET)
$(BUILDTARGET): $(OBJECTS) $(SOURCES) $(HEADERS) $(LIBLARANKDIR)/LaRank.o $(LIBLARANKDIR)/vectors.o 
	$(CC) $(LINKPATH) $(LDFLAGS) $(OBJECTS) $(LIBLARANKDIR)/LaRank.o $(LIBLARANKDIR)/vectors.o -o $@

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDEPATH) $< -o $@

$(LIBLARANKDIR)/LaRank.o: $(LIBLARANKDIR)/LaRank.cpp  $(LIBLARANKDIR)/LaRank.h $(LIBLARANKDIR)/vectors.o
	$(CC) $(CFLAGS) $(INCLUDEPATH) -o $(LIBLARANKDIR)/LaRank.o $(LIBLARANKDIR)/LaRank.cpp

$(LIBLARANKDIR)/vectors.o: $(LIBLARANKDIR)/vectors.h $(LIBLARANKDIR)/wrapper.h
	$(CC) $(CFLAGS) $(INCLUDEPATH) -o $(LIBLARANKDIR)/vectors.o $(LIBLARANKDIR)/vectors.cpp

clean:
	rm -f $(SOURCEDIR)/*~ $(SOURCEDIR)/*.o
	rm -f $(LIBLARANKDIR)/*.o
	rm -f $(BUILDTARGET)
