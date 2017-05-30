VER=2.0.0

PROG_NAME=vol_daemon

SRC_DIR=src
HEADERS_DIR=headers

SCRIPTS_DIR=scripts
STOP_SCRIPT=stop.sh
MEMCHK_DIR=memchk

LOCAL_LIBS_DIR=lib
SYS_LIBS_DIR=/usr/lib

UTILS_LIB_SRC_DIR=Utils
UTILS_LIB=libUtils.a

MSGS_QUEUE_LIB_SRC_DIR=MsgsQueue
MSGS_QUEUE_LIB=libMsgsQueue.a

SOCKETS_LIB_SRC_DIR=net/wifi
SOCKETS_LIB=libSockets.so

BT_LIB_SRC_DIR=net/bluetooth
BT_LIB=libBlueTooth.so

LOG_LIB_SRC_DIR=Log
LOG_LIB=libLog.so

SOUND_LIB_SRC_DIR=SoundLib
SOUND_LIB=libSound.a

GUI_SRC_DIR=Gui
GUI=gui

NET_DIR=net

TESTS_DIR=tests

LIBS_SRC_DIRS=$(MSGS_QUEUE_LIB_SRC_DIR) $(SOUND_LIB_SRC_DIR) $(LOG_LIB_SRC_DIR) $(UTILS_LIB_SRC_DIR) $(NET_DIR)

C=gcc
CPP=g++
CFLAGS=-Wall -O2 -c -std=c++0x

BUILD_DIR=build

PLATFORM=$(shell uname -i)

ARC_COMMAND=tar cfz
ARC_NAME=SoundroidDaemon
ARC_EXT=tar.gz
ARC_DIRS=$(LIBS_SRC_DIRS) $(GUI_SRC_DIR) $(SRC_DIR) $(HEADERS_DIR) $(SCRIPTS_DIR) imgs
HELP_FILES=INSTALL UNINSTALL README
ARC_FILES=config.file $(HELP_FILES) Makefile

TMP_DIR=$(ARC_NAME)$(VER)

vpath %.cpp src src/commands src/connectors src/dispatchers
vpath %.h headers headers/commands headers/connectors headers/dispatchers $(LIBS_SRC_DIRS) $(NET_DIR) $(SOCKETS_LIB_SRC_DIR) $(BT_LIB_SRC_DIR)

OBJS= Daemon.o SndConnector.o GuiConnector.o 

COMMANDS_OBJS=CommandChangePort.o CommandMute.o CommandIsMuted.o CommandUnMute.o CommandChangePort.o CommandGetPort.o \
	CommandChgVol.o CommandGetConnectedIP.o CommandGetLocalIP.o CommandHello.o CommandGetCurVol.o CommandsDispatcher.o

WIFI_OBJS=CommandsDispatcherWiFi.o ConnectorWiFi.o
BT_OBJS=CommandsDispatcherBT.o ConnectorBT.o

COMMANDS_SRC_FILES=CommandsNames.h Command.h CommandUnMute.h CommandMute.h CommandIsMuted.h CommandGetPort.h \
	 CommandChangePort.h CommandChgVol.h CommandGetConnectedIP.h CommandGetLocalIP.h CommandHello.h CommandGetCurVol.h

CONNECTORS_SRC_FILES=ConnectorBT.h GuiConnector.h SndConnector.h ConnectorWiFi.h



all:	libs $(PROG_NAME)

install:
	cp $(LOCAL_LIBS_DIR)/*.so $(SYS_LIBS_DIR)

$(PROG_NAME):	$(OBJS) $(COMMANDS_OBJS) $(BT_OBJS) $(WIFI_OBJS)
	mkdir -p $(BUILD_DIR)
	$(CPP) -L$(LOCAL_LIBS_DIR) -rdynamic -o $(BUILD_DIR)/$@ $^ -lpthread -lSockets -lLog -lSound -lasound -lMsgsQueue -lBlueTooth -lbluetooth -ldl
	cp $(SCRIPTS_DIR)/*.sh $(BUILD_DIR)

CommandChgVol.o:	CommandChgVol.cpp CommandChgVol.h Command.h SndConnector.h  Log.h
	$(CPP) $(CFLAGS) -I$(HEADERS_DIR)/commands  -I$(HEADERS_DIR)/connectors -I$(LOG_LIB_SRC_DIR) $< 

CommandGetCurVol.o:	CommandGetCurVol.cpp CommandGetCurVol.h Command.h SndConnector.h  Log.h
	$(CPP) $(CFLAGS) -I$(HEADERS_DIR)/commands  -I$(HEADERS_DIR)/connectors -I$(LOG_LIB_SRC_DIR) $< 

CommandGetConnectedIP.o:	CommandGetConnectedIP.cpp  CommandGetConnectedIP.h Command.h NetConnector.h  PortException.h
	$(CPP) $(CFLAGS) -I$(HEADERS_DIR)/connectors -I$(HEADERS_DIR)/commands -I$(HEADERS_DIR) $< 

CommandGetLocalIP.o:	 CommandGetLocalIP.cpp  CommandGetLocalIP.h Command.h Connector.h
	$(CPP) $(CFLAGS) -I$(HEADERS_DIR)/connectors -I$(HEADERS_DIR) -I$(HEADERS_DIR)/commands  $< 

CommandGetPort.o: CommandGetPort.cpp CommandGetPort.h Command.h ConnectorWiFi.h PortException.h
	$(CPP) $(CFLAGS) -I$(HEADERS_DIR)/connectors -I$(HEADERS_DIR) -I$(HEADERS_DIR)/commands  $< 

CommandChangePort.o:	CommandChangePort.cpp CommandChangePort.h Command.h CommandsDispatcherWiFi.h PortException.h
	$(CPP) $(CFLAGS) -I$(HEADERS_DIR)/connectors -I$(HEADERS_DIR)/dispatchers -I$(HEADERS_DIR)/commands -I$(HEADERS_DIR) -I$(LOG_LIB_SRC_DIR) $< 

CommandMute.o:	CommandMute.cpp CommandMute.h Command.h SndConnector.h
	$(CPP) $(CFLAGS) -I$(HEADERS_DIR)/commands -I$(HEADERS_DIR)/connectors $< 

CommandUnMute.o:	CommandUnMute.cpp CommandUnMute.h Command.h SndConnector.h
	$(CPP) $(CFLAGS) -I$(HEADERS_DIR)/commands -I$(HEADERS_DIR)/connectors $< 

CommandIsMuted.o:	CommandIsMuted.cpp CommandIsMuted.h Command.h SndConnector.h
	$(CPP) $(CFLAGS) -I$(HEADERS_DIR)/commands -I$(HEADERS_DIR)/connectors $< 

CommandHello.o:	CommandHello.cpp CommandHello.h Command.h NetConnector.h
	$(CPP) $(CFLAGS) -I$(HEADERS_DIR)/commands -I$(HEADERS_DIR)/connectors -I$(HEADERS_DIR) $< 

CommandsDispatcherWiFi.o:	CommandsDispatcherWiFi.cpp CommandsDispatcher.h Log.h CommandsDispatcherWiFi.h PortException.h GuiException.h CommandChangePort.h CommandGetPort.h
	$(CPP) $(CFLAGS) -pthread -I$(HEADERS_DIR) -I$(HEADERS_DIR)/connectors -I$(HEADERS_DIR)/commands -I$(LOG_LIB_SRC_DIR) -I$(HEADERS_DIR)/dispatchers $<

CommandsDispatcherBT.o:	CommandsDispatcherBT.cpp CommandsDispatcher.h Log.h CommandsDispatcherBT.h GuiConnector.h SndConnector.h
	$(CPP) $(CFLAGS) -pthread -I$(HEADERS_DIR) -I$(HEADERS_DIR)/connectors -I$(HEADERS_DIR)/dispatchers -I$(HEADERS_DIR)/commands -I$(LOG_LIB_SRC_DIR) $< 

CommandsDispatcher.o:	CommandsDispatcher.cpp CommandsDispatcher.h Log.h CommandsNames.h GuiException.h
	$(CPP) $(CFLAGS) -I$(HEADERS_DIR)/connectors -I$(HEADERS_DIR)/dispatchers -I$(LOG_LIB_SRC_DIR) -I$(HEADERS_DIR)/commands -I$(HEADERS_DIR) $< 

Daemon.o:	Daemon.cpp CommandsDispatcher.h CommandsDispatcherBT.h CommandsDispatcherWiFi.h GuiException.h PortException.h ConnectionTypes.h
	$(CPP) $(CFLAGS) -pthread -I$(HEADERS_DIR) -I$(HEADERS_DIR)/commands -I$(HEADERS_DIR)/connectors -I$(HEADERS_DIR)/dispatchers $<

ConnectorBT.o:	ConnectorBT.cpp ConnectorBT.h BlueToothLib.h NetConnector.h Log.h synchronise.h addr.h
	$(CPP) $(CFLAGS) -I$(HEADERS_DIR) -I$(BT_LIB_SRC_DIR) -I$(HEADERS_DIR)/connectors -I$(LOG_LIB_SRC_DIR) -I$(NET_DIR) $<

ConnectorWiFi.o:	ConnectorWiFi.cpp ConnectorWiFi.h SocketsLib.h NetConnector.h Log.h synchronise.h ClosingClient.h addr.h
	$(CPP) $(CFLAGS) -I$(HEADERS_DIR) -I$(SOCKETS_LIB_SRC_DIR) -I$(HEADERS_DIR)/connectors -I$(LOG_LIB_SRC_DIR) -I$(NET_DIR) $<

SndConnector.o:	SndConnector.cpp SndConnector.h  Connector.h Log.h SoundLib.h CommandsNames.h
	$(CPP) $(CFLAGS) -I$(HEADERS_DIR) -I$(SOUND_LIB_SRC_DIR) -I$(HEADERS_DIR)/commands -I$(HEADERS_DIR)/connectors -I$(LOG_LIB_SRC_DIR) $< 

GuiConnector.o:	GuiConnector.cpp GuiConnector.h  Connector.h CommandsNames.h Log.h MsgsQueueServer.h
	$(CPP) $(CFLAGS) -I$(HEADERS_DIR) -I$(HEADERS_DIR)/commands -I$(HEADERS_DIR)/connectors  -I$(LOG_LIB_SRC_DIR) -I$(MSGS_QUEUE_LIB_SRC_DIR) $< 

sound:
	mkdir -p $(LOCAL_LIBS_DIR)
	$(MAKE) --directory=$(SOUND_LIB_SRC_DIR) $(SOUND_LIB);
	$(MAKE) --directory=$(SOUND_LIB_SRC_DIR) install;

log:
	$(MAKE) --directory=$(LOG_LIB_SRC_DIR) $(LOG_LIB);
	$(MAKE) --directory=$(LOG_LIB_SRC_DIR) install;

sockets:
	mkdir -p $(LOCAL_LIBS_DIR)
	$(MAKE) --directory=$(SOCKETS_LIB_SRC_DIR) $(SOCKETS_LIB);
	$(MAKE) --directory=$(SOCKETS_LIB_SRC_DIR) install;

bluetooth:
	mkdir -p $(LOCAL_LIBS_DIR)
	$(MAKE) --directory=$(BT_LIB_SRC_DIR) $(BT_LIB);
	$(MAKE) --directory=$(BT_LIB_SRC_DIR) install;

gui:
	$(MAKE) --directory=$(GUI_SRC_DIR);
	cp $(GUI_SRC_DIR)/$(GUI) $(BUILD_DIR)

msgs_queue:
	mkdir -p $(LOCAL_LIBS_DIR)
	$(MAKE) --directory=$(MSGS_QUEUE_LIB_SRC_DIR) $(MSGS_QUEUE_LIB)
	$(MAKE) --directory=$(MSGS_QUEUE_LIB_SRC_DIR) install;

utils:
	mkdir -p $(LOCAL_LIBS_DIR)
	$(MAKE) --directory=$(UTILS_LIB_SRC_DIR) $(UTILS_LIB);
	$(MAKE) --directory=$(UTILS_LIB_SRC_DIR) install;

clean:
	$(MAKE) --directory=$(SOCKETS_LIB_SRC_DIR) clean;
	$(MAKE) --directory=$(BT_LIB_SRC_DIR) clean;
	$(MAKE) --directory=$(LOG_LIB_SRC_DIR) clean;
	$(MAKE) --directory=$(SOUND_LIB_SRC_DIR) clean;
	$(MAKE) --directory=$(GUI_SRC_DIR) clean;
	$(MAKE) --directory=$(MSGS_QUEUE_LIB_SRC_DIR) clean;
	$(MAKE) --directory=$(UTILS_LIB_SRC_DIR) clean;
	rm -f *.o 
	find . -name *~ | xargs rm -f
	find . -name log.txt ! -path "./build/*" | xargs rm -f

uninstall:
	rm -f $(LOCAL_LIBS_DIR)/*.a
	rm -rf $(BUILD_DIR)
	rm -f $(SYS_LIBS_DIR)/$(LOG_LIB) $(SYS_LIBS_DIR)/$(SOCKETS_LIB) $(SYS_LIBS_DIR)/$(BT_LIB)

libs:	sound sockets msgs_queue log utils bluetooth

libs_test:
	$(MAKE) --directory=$(SOUND_LIB_SRC_DIR)      -s test;
	$(MAKE) --directory=$(LOG_LIB_SRC_DIR)        -s test;
	$(MAKE) --directory=$(UTILS_LIB_SRC_DIR)      -s test;
	$(MAKE) --directory=$(SOCKETS_LIB_SRC_DIR)    -s test;
	$(MAKE) --directory=$(MSGS_QUEUE_LIB_SRC_DIR) -s test;
	$(MAKE) --directory=$(BT_LIB_SRC_DIR)         -s test;

libs_mem_leak:
	$(MAKE) --directory=$(SOUND_LIB_SRC_DIR)      mem_leak_chk;
	$(MAKE) --directory=$(LOG_LIB_SRC_DIR)        mem_leak_chk;
	$(MAKE) --directory=$(UTILS_LIB_SRC_DIR)      mem_leak_chk;
	$(MAKE) --directory=$(SOCKETS_LIB_SRC_DIR)    mem_leak_chk;
	$(MAKE) --directory=$(MSGS_QUEUE_LIB_SRC_DIR) mem_leak_chk;
	$(MAKE) --directory=$(BT_LIB_SRC_DIR)         mem_leak_chk;

dist_src:	$(ARC_NAME)_src_$(VER).$(ARC_EXT)

$(ARC_NAME)_src_$(VER).$(ARC_EXT):	src_tmp_dir
	$(ARC_COMMAND) $@ $(TMP_DIR)
	rm -rf $(TMP_DIR)

src_tmp_dir:	build_tmp
	cp -r $(ARC_DIRS) $(ARC_FILES) $(TMP_DIR)
	find $(TMP_DIR) -name Makefile -exec sed -i "s/=/=/" {} ';'

dist_bin:	$(ARC_NAME)_bin_$(PLATFORM)_$(VER).$(ARC_EXT)

$(ARC_NAME)_bin_$(PLATFORM)_$(VER).$(ARC_EXT):	bin_tmp_dir
	$(ARC_COMMAND) $@ --exclude=*.txt --exclude=*.a --exclude=*.msgs $(TMP_DIR)
	rm -rf $(TMP_DIR)

bin_tmp_dir:	build_tmp
	cp -r $(BUILD_DIR) $(LOCAL_LIBS_DIR) $(HELP_FILES) $(TMP_DIR)
	rm -f $(TMP_DIR)/$(BUILD_DIR)/*.*
	cp $(BUILD_DIR)/$(STOP_SCRIPT) $(TMP_DIR)/$(BUILD_DIR)

build_tmp:	clean
	mkdir -p $(TMP_DIR)

distchk:	dist_src
	$(SCRIPTS_DIR)/distchk.sh $(ARC_NAME)_src_$(VER).$(ARC_EXT) $(TMP_DIR) $(SYS_LIBS_DIR)/$(LOG_LIB)

tests:	libs_test
	$(MAKE) --directory=$(MSGS_QUEUE_LIB_SRC_DIR) -s testSndMsgs;
	mv $(MSGS_QUEUE_LIB_SRC_DIR)/testSndMsgs $(BUILD_DIR) 
	cd $(TESTS_DIR) && ./test.sh msgs

mem_leak:	libs_mem_leak
	cd $(MEMCHK_DIR) && ./memchk.sh

.PHONY:	tests build_tmp bin_tmp_dir dist_bin src_tmp_dir dist_src libs sound sockets msgs_queue log utils uninstall clean all install distchk libs_mem_leak mem_leak
