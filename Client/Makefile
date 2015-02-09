CC=g++
CC_FLAGS=-Wall -Werror -Wextra -Wno-unused-parameter

EXEC=client
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC)

%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@

clean:
	rm -f $(EXEC) $(OBJECTS)
