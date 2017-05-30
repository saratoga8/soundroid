CC=gcc
CFLAGS=-Wall -c -O2
CFLAGS_GTK = $(CFLAGS) $(shell pkg-config --cflags gtk+-3.0)

LIBS_DIR=../lib
LIBS=-lMsgsQueue -lLog -lUtils
LIBS += $(shell pkg-config --libs gtk+-3.0)

HEADERS_DIR=../headers
MSGS_QUEUE_SRC_DIR=../MsgsQueue
COMMANDS_HEADERS_DIR=$(HEADERS_DIR)/commands
LOG_SRC_DIR=../Log
BUILD_DIR=../build

UTILS_SRC_DIR=../Utils

vpath %.h . $(UTILS_SRC_DIR) $(LOG_SRC_DIR) $(HEADERS_DIR) $(MSGS_QUEUE_SRC_DIR) $(COMMANDS_HEADERS_DIR)
vpath %.c .

install:	gui utils
	mkdir -p $(BUILD_DIR)	
	cp gui $(BUILD_DIR)

gui:	gtk_ui.o init.o port_ops.o
	$(CC) -L$(LIBS_DIR) -o $@ $^ $(LIBS)

utils:
	$(MAKE) --directory=$(UTILS_SRC_DIR) install;

gtk_ui.o:	gtk_ui.c init.h file_utils.h ConnectionTypes.h
	$(CC) $(CFLAGS_GTK) -I$(LOG_SRC_DIR) -I$(UTILS_SRC_DIR) -I$(HEADERS_DIR) $<

init.o:	init.c init.h port_ops.h file_utils.h 
	$(CC) $(CFLAGS_GTK) -I$(UTILS_SRC_DIR) $<

port_ops.o:	port_ops.c port_ops.h
	$(CC) $(CFLAGS) -I$(MSGS_QUEUE_SRC_DIR) -I$(COMMANDS_HEADERS_DIR) $<

clean: 
	rm -f *~ *.o  ./gui queue.msgs log*.txt

.PHONY:	gui install clean
