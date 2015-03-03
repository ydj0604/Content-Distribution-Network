CC=g++
CC_FLAGS=-std=c++11 -Wall -Werror -Wextra -Wno-unused-parameter
LD_FLAGS=-lcpprest 

EXEC=client
SOURCES=$(wildcard *.cpp) ../ipToLatLng/ipToLatLng.cpp
OBJECTS=$(SOURCES:.cpp=.o)

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC) $(LD_FLAGS)

%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@

clean:
	rm -f $(EXEC) $(OBJECTS)
