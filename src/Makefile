# Set compiler
CC = gcc

# Compiler flags
CFLAGS = -Wno-switch -O0

CRFLAGS = -Wno-switch -Wno-unused-result -O3

# Object files
OBJFILES = mswg_gen.o json.o mswg_parser.o settings.o token.o common.o threadpool.o file.o

# Target executable
TARGET = mswg_gen

# Linker flags
LDFLAGS = -lpthread

all: $(TARGET)

release: CFLAGS = -Wno-switch -Wno-unused-result -O3
release: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean: 
	$(RM) $(OBJFILES) $(TARGET)