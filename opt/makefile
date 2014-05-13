LIBFLAGS = -fPIC -g -c -Wall -Wextra -Werror -pedantic -std=c++0x
CC = g++
OBJECTS = OptimizationScheme.o Opt_SteepestDescent.o OptimizableFunction.o
SO = data.so
INCLUDE = -I/usr0/home/gschoenh/Dropbox/Code/cpp/libs
LINK = /usr0/home/gschoenh/Dropbox/Code/cpp/libs/gstd/gstd.so


$(SO): $(OBJECTS)
	$(CC) -shared $(OBJECTS) -o $(SO) $(LINK)

Opt_SteepestDescent.o: Opt_SteepestDescent.cpp
	$(CC) $(LIBFLAGS) $(INCLUDE) Opt_SteepestDescent.cpp

OptimizationScheme.o: OptimizationScheme.cpp
	$(CC) $(LIBFLAGS) $(INCLUDE) OptimizationScheme.cpp

OptimizableFunction.o: OptimizableFunction.cpp
	$(CC) $(LIBFLAGS) $(INCLUDE) OptimizableFunction.cpp
	
clean:
	rm -rf $(SO) $(OBJECTS) 
