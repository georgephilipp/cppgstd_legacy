###OSNAME###
OSNAME = 
ifeq ($(OS),Windows_NT)
    OSNAME = WIN32
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        OSNAME =  LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
        OSNAME = OSX
    endif
endif

###INPUT###
SOURCE = $(wildcard src/*.cpp)
INTERM = $(patsubst %.cpp,%.o,$(SOURCE))
OBJECTS = $(patsubst src/%,nbobj/%,$(INTERM))

###OUTPUT###
OUTNAME = dnn
ifeq ($(OSNAME),LINUX)
    OUTFILE = lib$(OUTNAME).so
else ifeq ($(OSNAME),OSX)
    OUTFILE = lib$(OUTNAME).dylib
endif

###COMPILER###
#Standard: 
CC = 
ifeq ($(OSNAME),LINUX)
    CC = g++ #use /opt/gcc/4.7.3/bin/g++ for cluster but not needed under genamap account
else ifeq ($(OSNAME),OSX)
    CC = clang++ -stdlib=libc++
endif

###FLAGS###
FLAGS = -fPIC -g -c -Wall -Wextra -Werror -pedantic -O3
LIBFLAGS = 
ifeq ($(OSNAME),LINUX)
    LIBFLAGS = -shared
else ifeq ($(OSNAME),OSX)
    LIBFLAGS = -dynamiclib -install_name '$(CURDIR)/../lib$(OUTNAME).dylib'
endif
ifeq ($(OSNAME),LINUX)
    FLAGS += -std=c++0x
else ifeq ($(OSNAME),OSX)
    FLAGS += -std=c++11
endif


###DEPS###

###
INCLUDE = -I../../libs
LINK =  -L../../libs -Wl,-rpath,./../../libs -lgstd -ldata

###TARGETS###

$(OUTFILE): $(OBJECTS)
	$(CC) $(LIBFLAGS) $(OBJECTS) -o $(OUTFILE) $(LINK)
	mv $(OUTFILE) ../

nbobj/%.o: src/%.cpp 
	$(CC) $(FLAGS) $(INCLUDE) -o $@ -c $<

clean:
	rm -rf ../$(OUTFILE) nbobj/*
