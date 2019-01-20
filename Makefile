TARGET = curve
DEPS = argparse
LIBS = -lm libs/argparse/libargparse.a
CC = gcc
CFLAGS = -O3 -Wall -Ilibs/argparse

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst src/%.c, build/%.o, $(wildcard src/*.c))
HEADERS = $(wildcard src/*.h)

argparse:
	$(MAKE) -C libs/argparse

build/%.o: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) `pkg-config --cflags cairo` -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS) $(DEPS)
	$(CC) $(OBJECTS) -Wall $(LIBS) `pkg-config --cflags --libs cairo` -o $@

clean:
	-rm -f build/*.o
	-rm -f $(TARGET)
