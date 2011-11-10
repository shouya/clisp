CC = gcc
RM = mv -f --backup=t
CFLAGS = -Wall

LIBCLISP_OUT = libclisp.so

SRC_DIR = libclisp
BIN_DIR = bin
INC_DIR = include
TRASH_DIR = trash
TOOL_DIR = tools

RM += -t $(TRASH_DIR)

INCLUDE = -I$(INC_DIR)
LIBS = 

LIB_SOURCES = $(wildcard $(SRC_DIR)/*.c)
LIB_OBJECTS = $(SOURCES:.c=.o)

CLISPI_SOURCES = $(wildcard $(TOOL_DIR)/clispi/*.c)
CLISPI_OBJECTS = $(CLISPI_SOURCES:.c=.o)

CLISP_SOURCES = $(wildcard $(TOOL_DIR)/clisp/*.c)
CLISP_OBJECTS = $(CLISP_SOURCES:.c=.o)

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


all: clisp #clispi

libclisp: $(LIB_OBJECTS)
	$(CC) $(CFLAGS) -shared -o $(BIN_DIR)/$(LIBCLISP_OUT) $^ $(LIBS)

clisp: libclisp $(CLISP_OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $^ -l$(BIN)/$(LIBCLISP_OUT)

clispi: libclisp $(CLISPI_OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $^ -l$(BIN)/$(LIBCLISP_OUT)


%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) $(LIB_OJECTS)
	$(RM) $(CLISP_OBJECTS)
	$(RM) $(CLISPI_OBJECTS)
	$(RM) $(BIN_DIR)/*


rebuild: clean all

.PHONEY : clean
