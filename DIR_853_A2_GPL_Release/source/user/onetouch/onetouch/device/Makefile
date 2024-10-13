NVDIR = $(ROOTDIR)/lib/libnvram
NRDIR = $(ROOTDIR)/user/apps/notify_rc
CFLAGS += -I$(NVDIR) -I$(ROOTDIR)/$(LINUXDIR)/include -I$(ROOTDIR)/user/apps/notify_rc
CFLAGS += -I../easyroaming/include
LDFLAG += -L$(NVDIR) -lnvram -L../ -ldxmlapi -L$(NRDIR) -lnotifyrc

ifeq ($(CONFIG_CUSTOM_PRODUCT), "DIR-878")
MODEL_DIR = dir878
else
MODEL_DIR = dir882
endif

all:
	$(CC) $(CFLAGS) -o starter starter.c $(LDFLAG)
		
clean: 
	rm -f starter starter.o

install:
	install -m 755 starter $(ROOTDIR)/romfs/usr/sbin
	install $(MODEL_DIR)/device.xml $(ROOTDIR)/romfs/etc_ro/onetouch	
	
.PHONY: all install clean 

	