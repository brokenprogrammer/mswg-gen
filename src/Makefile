# Set compiler
CC = gcc

# Compiler flags
CFLAGS = -g -Wall -Wno-switch -fsanitize=address

# Object files
OBJFILES = mswg_gen.o json.o mswg_parser.o settings.o token.o common.o threadpool.o file.o

# Target executable
TARGET = mswg_gen

# Linker flags
LDFLAGS = -lpthread

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean: 
	$(RM) $(OBJFILES) $(TARGET)