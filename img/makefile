FLAGS = -fPIC -g -c -Wall -Wextra -pedantic #-Werror
CC = g++
SOURCE = $(wildcard src/*.cpp)
INTERM = $(patsubst %.cpp,%.o,$(SOURCE))
OBJECTS = $(patsubst src/%,nbobj/%,$(INTERM))
RES = libimg.so
INCLUDE = -I../../libs
OPENCV = -L/usr/local/lib `pkg-config --cflags --libs opencv`
LINK = -L../../libs -lgstd $(OPENCV)

$(RES): $(OBJECTS)
	$(CC) -shared $(OBJECTS) -o $(RES) $(LINK)
	mv $(RES) ../
	
nbobj/%.o: src/%.cpp
	$(CC) $(FLAGS) $(INCLUDE) -o $@ -c $<

clean:
	rm -rf ../$(RES) $(OBJECTS) 
