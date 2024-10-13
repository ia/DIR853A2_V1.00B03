NVDIR = $(ROOTDIR)/lib/libnvram
CFLAGS += -I$(NVDIR) -I$(ROOTDIR)/lib/libevent-2.0.22-stable/include -I$(ROOTDIR)/user/openssl-1.0.2j/include
LDFLAGS += -lnvram -L./ -lmdb  -L$(ROOTDIR)/user/apps/notify_rc -lnotifyrc
LDEVENT = -L$(ROOTDIR)/user/openssl-1.0.2j/ -L$(ROOTDIR)/lib/libevent-2.0.22-stable/.libs -levent_core -lpthread
CFLAGS +=  -fPIC
SHARE_LIB = libmdb.so
EXE = mydlink
OBJS = mydlink.o


all: http  $(SHARE_LIB) $(EXE) event infocgi 


$(SHARE_LIB): mdb.o
	$(LD) -shared -o $@ $^
$(EXE): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)
http: http.o
	$(CC) -o $@ http.o 
infocgi: infocgi.o
	$(CC) -o $@ infocgi.o $(LDFLAGS)
event: event-client.o
	$(CC) -o $@ event-client.o $(LDFLAGS) $(LDEVENT) -lssl -lcrypto -levent_extra  -levent_pthreads -levent_openssl 
%.o: .%.depend
.%.depend: %.c
	$(CC) $(CFLAGS) -M $< > $@
-include $(OBJS:%.o=.%.depend)
romfs:
	$(ROMFSINST) /bin/infocgi
	$(ROMFSINST) /bin/http
	$(ROMFSINST) /bin/event
	$(ROMFSINST) /bin/$(EXE)
	$(ROMFSINST) /lib/$(SHARE_LIB)
	ln -sf /bin/$(EXE) $(ROOTDIR)/romfs/bin/factory_reset
	ln -sf /bin/$(EXE) $(ROOTDIR)/romfs/bin/fw_upgrade
	ln -sf /bin/$(EXE) $(ROOTDIR)/romfs/bin/mdb
	ln -sf /bin/$(EXE) $(ROOTDIR)/romfs/bin/mydlinknumber
	rm -rf $(ROOTDIR)/romfs/etc_ro/mydlink
ifeq ($(CONFIG_DEFAULTS_MEDIATEK_DIR_882H), y)
	cp -af agent/DIR-882/ $(ROOTDIR)/romfs/etc_ro/mydlink/
endif

ifeq ($(CONFIG_DEFAULTS_MEDIATEK_DIR_882), y)
	#cp -af agent/DIR-882/ $(ROOTDIR)/romfs/etc_ro/mydlink/
	cp -af agent/gen2/ $(ROOTDIR)/romfs/etc_ro/mydlink/
endif
	rm -rf $(ROOTDIR)/romfs/etc_ro/mydlink/config
	ln -sf /private/config $(ROOTDIR)/romfs/mydlink/config
	cp -f TZ_change_notify $(ROOTDIR)/romfs/etc/TZ_change_notify
	#mkdir $(ROOTDIR)/romfs/etc_ro/mydlink/config 2>&1 /dev/null
	#mkdir $(ROOTDIR)/romfs/etc_ro/mydlink/cert  2>&1 /dev/null
	#rm -rf $(ROOTDIR)/romfs/mydlink
	#cp -a agent/DIR-882/ $(ROOTDIR)/romfs/mydlink
	$(ROMFSINST) /etc_ro/lighttpd/shareport.conf
clean:
	rm -f *.o mydlink http .*.depend *.so event
	make -C shareport clean
