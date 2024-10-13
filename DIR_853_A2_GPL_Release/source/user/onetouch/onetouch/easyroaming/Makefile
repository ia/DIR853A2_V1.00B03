#
# Makefile for Easy Roaming version 2 daemon
#

NVDIR = $(ROOTDIR)/lib/libnvram
CFLAGS := -fPIC  -Os -Wall -D__BYTE_ORDER=__LITTLE_ENDIAN
CFLAGS += -Wall -D_GNU_SOURCE -I./include -laes -lm -lpthread -L.
CFLAGS += -I$(NVDIR) -I$(ROOTDIR)/$(LINUXDIR)/include
LDFLAG += -L$(NVDIR) -lnvram

PROG	= easyroaming
OBJS	= apcontrol.o cJSON.o dlink_nl.o iwlib.o mytool.o platform.o roaming.o

$(PROG): platform.o
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS) $(LDFLAG)

all: $(PROG)

install: 

clean: 
	rm -f $(PROG) platform.o 
	
.PHONY: all install clean 
