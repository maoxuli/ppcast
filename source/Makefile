
#  
NAME = SourceServer
C++ = g++
CC = gcc
LINK = g++
MAKE = make

CORE_LINK_LIBS = -lpthread

CCFLAGS += $(COMPILER_FLAGS) -D__linux__  -g -Wall $(INCLUDE_FLAG) 
LIBS = $(CORE_LINK_LIBS) 

# OPTIMIZATION
CCFLAGS += -O3

# EACH DIRECTORY WITH HEADERS MUST BE APPENDED IN THIS MANNER TO THE CCFLAGS

CCFLAGS += -I.

CCFLAGS += -Icommon
CCFLAGS += -Iserver
CCFLAGS += -Imedia
CCFLAGS += -Imedia/asf

# EACH DIRECTORY WITH A STATIC LIBRARY MUST BE APPENDED IN THIS MANNER TO THE LINKOPTS

C++FLAGS = $(CCFLAGS)

CFILES = src/mediafile/rm/mb_file_io.c\
	 src/mediafile/rm/rmff.c
					
CPPFILES = src/stdafx.cpp\
	src/common/buffer.cpp\
	src/common/connection.cpp\
	src/common/ipendpoint.cpp\
	src/common/settings.cpp\
	src/common/tcplistener.cpp\
	src/common/tcplisteners.cpp\
	src/common/timeslot.cpp\
	src/common/tinystr.cpp\
	src/common/tinyxml.cpp\
	src/common/tinyxmlerror.cpp\
	src/common/tinyxmlparser.cpp\
	src/common/transfer.cpp\
	src/common/transfers.cpp\
	src/mediasource/mediasource.cpp\
	src/mediasource/mediasourcemgr.cpp\
	src/mediasource/parser.cpp\
	src/mediasource/parsermgr.cpp\
	src/os/os.cpp\
	src/os/osthread.cpp\
	src/os/osthreadlock.cpp\
	src/packet/datapacket.cpp\
	src/packet/packet.cpp\
	src/packet/pspacket.cpp\
	src/packet/rtppacket.cpp\
	src/peerclient/peerclient.cpp\
	src/peerclient/peermgr.cpp\
	src/mediafile/rm/rmmediasource.cpp\
	src/mediafile/rm/rmparser.cpp\
	src/mediafile/rm/rmreader.cpp\
	src/mediafile/rm/rtsputil.cpp\
	src/mediafile/wmv/asfheadobject.cpp\
	src/mediafile/wmv/asfpacket.cpp\
	src/mediafile/wmv/asfreader.cpp\
	src/mediafile/wmv/asfparser.cpp\
	src/mediafile/wmv/asfmediasource.cpp\
	src/app/source.cpp\
	src/app/sourceserver.cpp\
	src/app/sourceapp.cpp

#LIBFILES = 

all: LinuxSourceServer

LinuxSourceServer: $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)  $(LIBFILES)
	$(LINK) -o $@ $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(COMPILER_FLAGS) $(LIBS) 

install: LinuxSourceServer

clean:
	rm -f $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)

.SUFFIXES: .cpp .c .o

.cpp.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cpp

.c.o:
	$(CC) -c -o $*.o $(DEFINES) $(CCFLAGS) $*.c



