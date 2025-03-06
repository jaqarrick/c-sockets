# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -pthread

# Object files
OBJ = proxy.o http_parser.o

# Header files (dependencies)
DEPS = http_parser.h

# Target executable
TARGET = cproxy

# Pattern rule to compile .c files to .o files
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Rule to link object files into the final executable
$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

# Phony target to clean up object files and the executable
.PHONY: clean

clean:
	rm -f *.o $(TARGET)
