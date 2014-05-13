FLAGS = -fPIC -g -c -Wall -Wextra -pedantic #-Werror
CC = g++
SOURCE = $(wildcard src/*.cpp)
INTERM = $(patsubst %.cpp,%.o,$(SOURCE))
OBJECTS = $(patsubst src/%,nbobj/%,$(INTERM))
RES = liblinalg.so
INCLUDE = -I../../libs
LINK = -L../../libs -lgstd

$(RES): $(OBJECTS)
	$(CC) -shared $(OBJECTS) -o $(RES) $(LINK)
	mv $(RES) ../
	
nbobj/%.o: src/%.cpp
	$(CC) $(FLAGS) $(INCLUDE) -o $@ -c $<

clean:
	rm -rf ../$(RES) $(OBJECTS) 
