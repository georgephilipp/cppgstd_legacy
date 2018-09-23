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
OUTNAME = gstd_matlab
OUTFILE = 
ifeq ($(OSNAME),LINUX)
    OUTFILE = lib$(OUTNAME).so
else ifeq ($(OSNAME),OSX)
    OUTFILE = lib$(OUTNAME).dylib
endif

###COMPILER###
CC = 
ifeq ($(OSNAME),LINUX)
    CC = g++
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
MATLABINCLUDELINUX = -I/usr0/home/gschoenh/Installs/matlab/extern/include
MATLABINCLUDECOGITO = -I/opt/matlab/8.1/extern/include
MATLABINCLUDEMAC = -I/Applications/MATLAB_R2014a.app/extern/include
MATLABLINKSYSLINUX = -L/usr0/home/gschoenh/Installs/matlab/sys/os/glnxa64 -Wl,-rpath,/usr0/home/gschoenh/Installs/matlab/sys/os/glnxa64
MATLABLINKSYSCOGITO = -L/opt/matlab/8.1/sys/os/glnxa64 -Wl,-rpath,/opt/matlab/8.1/sys/os/glnxa64
MATLABLINKLINUX = -L/usr0/home/gschoenh/Installs/matlab/bin -L/usr0/home/gschoenh/Installs/matlab/bin/glnxa64 -L/usr0/home/gschoenh/Installs/matlab/extern/lib/glnxa64 -Wl,-rpath,/usr0/home/gschoenh/Installs/matlab/bin/glnxa64,-rpath,/usr0/home/gschoenh/Installs/matlab/bin/glnxa64,-rpath,/usr0/home/gschoenh/Installs/matlab/bin,-rpath,/usr0/home/gschoenh/Installs/matlab/extern/lib/glnxa64
MATLABLINKCOGITO = -L/opt/matlab/8.1/bin -L/opt/matlab/8.1/bin/glnxa64 -L/opt/matlab/8.1/extern/lib/glnxa64 -Wl,-rpath,/opt/matlab/8.1/bin,-rpath,/opt/matlab/8.1/bin/glnxa64,-rpath,/opt/matlab/8.1/extern/lib/glnxa64
MATLABLINKMAC = -L/Applications/MATLAB_R2014a.app/bin -L/Applications/MATLAB_R2014a.app/bin/maci64 -L/Applications/MATLAB_R2014a.app/extern/lib/maci64 -Wl,-rpath,/Applications/MATLAB_R2014a.app/bin,-rpath,/Applications/MATLAB_R2014a.app/bin/maci64,-rpath,/Applications/MATLAB_R2014a.app/extern/lib/maci64
MATLABLIBS = -leng -lmat -lmx

GLIBLINK = -Wl,-rpath,/opt/gcc/4.7.3/lib64/
INCLUDE = $(MATLABINCLUDELINUX) $(MATLABINCLUDECOGITO) $(MATLABINCLUDEMAC) -I../../libs
LINK = $(MATLABLINKLINUX) $(MATLABLINKCOGITO) $(MATLABLINKMAC) $(MATLABLIBS) -L../../libs -Wl,-rpath,./../../libs -lgstd $(GLIBLINK)

###TARGETS###
$(OUTFILE): $(OBJECTS)
	$(CC) $(LIBFLAGS) -shared $(OBJECTS) -o $(OUTFILE) $(LINK)
	mv $(OUTFILE) ../

nbobj/%.o: src/%.cpp 
	$(CC) $(FLAGS) $(INCLUDE) -o $@ -c $<
	
clean:
	rm -rf nbobj/* ../$(OUTFILE)
