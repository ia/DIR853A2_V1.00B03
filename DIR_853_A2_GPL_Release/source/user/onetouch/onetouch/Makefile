#
# Makefile for StarterAPI library
#
TARGET   = obj


CFLAGS := -Wall -D_GNU_SOURCE $(CFLAGS)
CFLAGS += $(EXTRA_CFLAGS)
CFLAGS += -I. -I$(SRCBASE)/include -Wall
#CFLAGS += -I./easyroaming/include


LIB_NAME	= libstarter.so
CFLAGS		+= -I./include 

SRCS_y   =  platform.c
BASEOBJS   = obj/auth.o obj/xml_parser.o obj/crc32.o obj/debug_log.o obj/md5.o obj/dhttpd.o obj/rss_sub.o obj/rss_encry.o obj/base64.o

SRCS     = $(SRCS_y) $(SRCS_m)
OBJS     = $(shell for i in $(SRCS); do n=`basename $$i`; echo -n $(TARGET)/$${n%%.*}.o ""; done)

$(LIB_NAME): $(OBJS) $(BASEOBJS)	
	$(CC) -shared -Wl,-soname,$(LIB_NAME).0 -o $(LIB_NAME).0.0 $(OBJS) $(BASEOBJS)
	ln -sf $(LIB_NAME).0.0 $(LIB_NAME).0
	ln -sf $(LIB_NAME).0 $(LIB_NAME)

$(TARGET)/%.o: %.c
	$(CC) $(INCLUDE) $(CFLAGS_EXTRA) $(CFLAGS) -fPIC -c -o $@ $<

all: $(LIB_NAME)
	
install: 
	install -d $(INSTALLDIR)/usr/sbin
	install -m 755 onetouch $(INSTALLDIR)/usr/sbin/onetouch
	install -m 755 xmldb $(INSTALLDIR)/usr/sbin/dxml
	ln -sf ./dxml $(INSTALLDIR)/usr/sbin/dxmlc
	ln -sf ./dxml $(INSTALLDIR)/usr/sbin/xmldb
	ln -sf ./dxml $(INSTALLDIR)/usr/sbin/xmldbc
	install -m 755 goahead $(INSTALLDIR)/usr/sbin/dhttpd
	install -d $(INSTALLDIR)/lib
	install -m 755 $(LIB_NAME) $(INSTALLDIR)/lib
	install -m 755 libdxmlapi.so $(INSTALLDIR)/lib

clean: 
	rm -f $(LIB_NAME) $(LIB_NAME).0.0 $(LIB_NAME).0 $(TARGET)/platform.o 
	
.PHONY: all install clean 
