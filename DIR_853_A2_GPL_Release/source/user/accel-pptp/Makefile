# $Id: //WIFI_SOC/MP/SDK_5_0_0_0/RT288x_SDK/source/user/accel-pptp/Makefile#1 $
VERSION=1.3.3
RELEASE=

RM	= rm -f
OPTIMIZE= -O2
#DEBUG	= -g
INCLUDE =
CFLAGS  += -Wall -DHAVE_CONFIG_H -DSBINDIR='"/sbin"' $(OPTIMIZE) $(DEBUG) $(INCLUDE)
CFLAGS  += -I$(ROOTDIR)/$(LINUXDIR)/include
LIBS	= -lutil
LDFLAGS	=


PPTP_OBJS = compat.o  configfile.o  \
	    getopt1.o  getopt.o  inststr.o  pptpd.o \
	    pptpmanager.o

PPTP_DEPS = compat.h configfile.h inststr.h \
	    our_syslog.h  pptpdefs.h config.embed.h config.h \
	    defaults.h our_getopt.h pptpmanager.h 

PPTP_BIN = pptpd
PPTPCTRL_BIN = pptpctrl
BCRELAY = bcrelay

#all: $(PPTP_BIN) $(PPTPCTRL_BIN) $(BCRELAY)
all:

$(PPTP_BIN): $(PPTP_OBJS) $(PPTP_DEPS)
	$(CC) -o $@ $(PPTP_OBJS) $(LDFLAGS) $(LIBS)

$(PPTPCTRL_BIN): compat.o inststr.o pptpctrl.o ctrlpacket.o ctrlpacket.h
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	$(RM) *.o $(PPTP_BIN) $(PPTPCTRL_BIN) $(BCRELAY)

romfs:
#	$(ROMFSINST) /bin/$(PPTP_BIN)
#	$(ROMFSINST) /bin/$(PPTPCTRL_BIN)
#	$(ROMFSINST) /bin/$(BCRELAY)
	$(ROMFSINST) /sbin/accel-pptp.sh


