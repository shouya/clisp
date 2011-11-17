CC = gcc
RM = /bin/rm -f
CFLAGS = -Wall

LIBCLISP_OUT = libclisp.so

DIR = `pwd`
SRC_DIR = libclisp
BIN_DIR = bin
INC_DIR = include
TRASH_DIR = trash
TOOL_DIR = tools
OBJ_DIR = obj
LIB_DIR = lib

INCLUDE = -I$(INC_DIR)
LIBS = 

LIB_SOURCES = $(wildcard $(SRC_DIR)/*.c)
LIB_OBJECTS = $(LIB_SOURCES:.c=.o)

CLISPI_SOURCES = $(wildcard $(TOOL_DIR)/clispi/*.c)
CLISPI_OBJECTS = $(CLISPI_SOURCES:.c=.o)

CLISP_SOURCES = $(wildcard $(TOOL_DIR)/clisp/*.c)
CLISP_OBJECTS = $(CLISP_SOURCES:.c=.o)

OPTIMIZATION = -O3
PEDANTIC = -ansi -pedantic
DEBUG = yes
PROFILE = yes

ifeq ($(DEBUG), yes)
	CFLAGS += -g
	OPTIMIZATION = -O0
endif

ifeq ($(PROFILE), yes)
	CFLAGS += -pg
endif


#CFLAGS += $(PEDANTIC)
CFLAGS += $(INCLUDE)
CFLAGS += $(OPTIMIZATION)


all: libclisp clisp #clispi

libclisp: $(LIB_OBJECTS)
	cd $(SRC_DIR)
	$(CC) $(CFLAGS) -shared -o $(DIR)/$(LIB_DIR)/$(LIBCLISP_OUT) $^ $(LIBS)

clisp: $(CLISP_OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $^ \
	   -L$(LIB_DIR) -lclisp

clispi: $(CLISPI_OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $^ -l$(BIN_DIR)/$(LIBCLISP_OUT)


%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(LIB_DIR)/*
	$(RM) $(BIN_DIR)/*
	$(RM) $(LIB_OBJECTS)
	$(RM) $(CLISP_OBJECTS)


rebuild: clean all

.PHONEY : clean
