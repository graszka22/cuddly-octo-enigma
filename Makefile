TARGET = curve
LIBS = -lm
CC = gcc
CFLAGS = -g -Wall

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst src/%.c, build/%.o, $(wildcard src/*.c))
HEADERS = $(wildcard src/*.h)

build/%.o: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) `pkg-config --cflags cairo` -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) `pkg-config --cflags --libs cairo` -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)