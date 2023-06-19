TARGET  = QL6502
SRC_DIR = ./src
BIN_DIR = ./bin

CC = gcc

CFLAGS = -g
CPPFLAGS += -MMD -MP

LDFLAGS =
LDLIBS =

SRCS = $(wildcard $(SRC_DIR)/*.c)

OBJS = $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(SRCS))

DEPS = $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.d,$(SRCS))

all : $(BIN_DIR)/$(TARGET) ;

$(BIN_DIR)/$(TARGET) : $(OBJS) Makefile | $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

$(BIN_DIR)/%.o : $(SRC_DIR)/%.c Makefile | $(BIN_DIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

-include $(DEPS)

$(BIN_DIR) :
	@mkdir $(BIN_DIR)

clean :
	rm $(BIN_DIR)/*.*
	
.PHONY: all clean 
