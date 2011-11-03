CC = gcc
CFLAGS = -Wall
TARGET = libclisp.so

SRC_DIR = src
BIN_DIR = bin
INC_DIR = include
TRASH_DIR = trash

INCLUDE = -I$(INC_DIR)
SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:.c=.o)
LIBS = 

OPTIMIZATION = -O3
PEDANTIC = -ansi -pedantic

ifeq ($(DEBUG), yes)
	CFLAGS += -g
	OPTIMIZATION = -O0
endif

ifeq ($(PROFILE), yes)
	CFLAGS += -pg
endif


CFLAGS += $(PEDANTIC)
CFLAGS += $(INCLUDE)
CFLAGS += $(OPTIMIZATION)


all: $(TARGET) clisp clispi

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -shared -o $(BIN_DIR)/$@ $^ $(LIBS)

clisp: $(TARGET)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/clisp\
             tools/clisp/*.c -l$(BIN)/$(TARGET)

clispi: $(TARGET)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/clispi\
             tools/clispi/*.c -l$(BIN)/$(TARGET)


%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	mv $(OBJECTS) $(TRASH_DIR)
	mv $(BIN_DIR)/* $(TRASH_DIR)

.PHONEY : clean
