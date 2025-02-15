# Copyright (c) 2004 WIDE Project. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the project nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

#
# $Id: Makefile.in,v 1.16 2007/02/27 14:47:11 suzsuz Exp $
# $KAME: Makefile.in,v 1.45 2005/10/16 16:25:38 suz Exp $
#

prefix=	
srcdir=	.
sysconfdir= ${prefix}/var/etc
localdbdir= /var/run
user= bin
group= bin

CFLAGS+= -O2 -I$(srcdir) -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DYYTEXT_POINTER=1 -DHAVE_GETADDRINFO=1 -DHAVE_GETNAMEINFO=1 -DHAVE_GETIFADDRS=1 -DHAVE_IF_NAMETOINDEX=1 -DHAVE_STRLCPY=1 -DHAVE_STRLCAT=1 -DHAVE_DAEMON=1 -DHAVE_WARNX=1 -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_FCNTL_H=1 -DHAVE_SYS_IOCTL_H=1 -DHAVE_SYS_TIME_H=1 -DHAVE_SYSLOG_H=1 -DHAVE_UNISTD_H=1 -DHAVE_IFADDRS_H=1 -DTIME_WITH_SYS_TIME=1 -DHAVE_STRUCT_TM_TM_ZONE=1 -DHAVE_TM_ZONE=1 -DHAVE_SIG_ATOMIC_T=1 -DGETPGRP_VOID=1 -DRETSIGTYPE=void -DHAVE_MKTIME=1 -DHAVE_SELECT=1 -DHAVE_SOCKET=1 -DHAVE_CLOCK_GETTIME=1 -DHAVE_ARC4RANDOM=1 -DHAVE_ANSI_FUNC=1 -DHAVE_TAILQ_FOREACH_REVERSE=1 -DHAVE_STDARG_H=1  -DSYSCONFDIR=\"${sysconfdir}\" \
	-DLOCALDBDIR=\"${localdbdir}\"

CFLAGS += -DDHCPV6_LOGO
CFLAGS += -DDHCPV6_IOL
ifeq ($(ELBOX_PROGS_PRIV_APMODULE_FOR_ZOOM_NETWORKS),y)
CFLAGS += -DELBOX_PROGS_PRIV_APMODULE_FOR_ZOOM_NETWORKS
endif

#for lost getifaddrs issue (tom, 20110930)
LIBOBJS += ifaddrs.o common.o if.o config.o timer.o lease.o base64.o auth.o dhcp6_ctl.o cfparse.o cftoken.o

APPLET=	dhcp6-multi #dhcp6c dhcp6s dhcp6relay dhcp6ctl

INSTALL=/usr/bin/install -c
INSTALL_PROGRAM=${INSTALL}
INSTALL_DATA=${INSTALL} -m 644
exec_prefix=	${prefix}
bindir=	${exec_prefix}/bin
sbindir=${exec_prefix}/sbin
mandir=	${prefix}/man

EXE = dhcp6-multi
USRSBINDIR=$(ROOTDIR)/romfs/usr/sbin

GENSRCS=cfparse.c cftoken.c
CLIENTOBJS=	dhcp6c.o prefixconf.o dhcp6c_ia.o \
	dhcp6c_script.o addrconf.o \
	arc4random.o  ll_map.o libnetlink.o
#SERVOBJS=	dhcp6s.o common.o if.o config.o timer.o lease.o \
#	base64.o auth.o dhcp6_ctl.o libnetlink.o ll_map.o $(GENSRCS:%.c=%.o)
SERVOBJS=	dhcp6s.o
RELAYOBJS =	dhcp6relay.o dhcp6relay_script.o common.o timer.o
CTLOBJS= dhcp6_ctlclient.o base64.o auth.o
CLEANFILES+=	y.tab.h

all: prompt $(APPLET)
dhcp6-multi: dhcp6-multi.o $(CLIENTOBJS) $(SERVOBJS) $(LIBOBJS)
	$(CC) $(LDFLAGS) -o dhcp6-multi dhcp6-multi.o $(CLIENTOBJS) $(SERVOBJS) $(LIBOBJS) $(LIBS)
dhcp6c:	$(CLIENTOBJS) $(LIBOBJS)
	$(CC) $(LDFLAGS) -o dhcp6c $(CLIENTOBJS) $(LIBOBJS) $(LIBS)
dhcp6s:	$(SERVOBJS) $(LIBOBJS)
	$(CC) $(LDFLAGS) -o dhcp6s $(SERVOBJS) $(LIBOBJS) $(LIBS)
dhcp6relay: $(RELAYOBJS) $(LIBOBJS)
	$(CC) $(LDFLAGS) -o $@ $(RELAYOBJS) $(LIBOBJS) $(LIBS)
dhcp6ctl: $(CTLOBJS)
	$(CC) $(LDFLAGS) -o $@ $(CTLOBJS) $(LIBOBJS) $(LIBS)

prompt:
	@echo -e "\033[32mCompiling WIDE-DHCPV6 ...\033[0m"
.PHONY: prompt

cfparse.c y.tab.h: cfparse.y
	bison -y -d cfparse.y
	mv y.tab.c cfparse.c

cftoken.c: cftoken.l y.tab.h
	flex cftoken.l
	mv lex.yy.c $@	

getaddrinfo.o:	$(srcdir)/missing/getaddrinfo.c
	$(CC) -c $(srcdir)/missing/$*.c
getnameinfo.o:	$(srcdir)/missing/getnameinfo.c
	$(CC) -c $(srcdir)/missing/$*.c
strlcat.o:	$(srcdir)/missing/strlcat.c
	$(CC) -c $(srcdir)/missing/$*.c
strlcpy.o:	$(srcdir)/missing/strlcpy.c
	$(CC) -c $(srcdir)/missing/$*.c
arc4random.o:	$(srcdir)/missing/arc4random.c
	$(CC) $(CFLAGS) -c $(srcdir)/missing/$*.c

#for lost getifaddrs issue (tom, 20110930)
getifaddrs.o: $(TOPDIR)/comlib/getifaddrs.c
	$(CC) $(CFLAGS) -c $(TOPDIR)/comlib/getifaddrs.c

daemon.o:	$(srcdir)/missing/daemon.c
	$(CC) -c $(srcdir)/missing/$*.c
warnx.o:	$(srcdir)/missing/warnx.c
	$(CC) -c $(srcdir)/missing/$*.c

$(srcdir)/ianaopts.h: gentab.pl bootp-dhcp-parameters
	expand bootp-dhcp-parameters | perl gentab.pl > ianaopts.h

romfs:
	$(INSTALL) $(EXE) $(USRSBINDIR)
#	ln -s -f dhcp6-multi $(INSTALLDIR)/wide-dhcpv6/usr/sbin/dhcp6c
	ln -s -f $(EXE) $(USRSBINDIR)/dhcp6s
	  
#install::
#	-mkdir -p $(sbindir) $(mandir)/man5 $(mandir)/man8
#	$(INSTALL_PROGRAM) -s -o $(user) -g $(group) $(APPLET) $(sbindir)
#	$(INSTALL_DATA) -o $(user) -g $(group) dhcp6c.8 $(mandir)/man8
#	$(INSTALL_DATA) -o $(user) -g $(group) dhcp6s.8 $(mandir)/man8
#	$(INSTALL_DATA) -o $(user) -g $(group) dhcp6relay.8 $(mandir)/man8
#	$(INSTALL_DATA) -o $(user) -g $(group) dhcp6ctl.8 $(mandir)/man8
#	$(INSTALL_DATA) -o $(user) -g $(group) dhcp6c.conf.5 $(mandir)/man5
#	$(INSTALL_DATA) -o $(user) -g $(group) dhcp6s.conf.5 $(mandir)/man5

install::
#	@echo -e "\033[32mInstalling WIDE-DHCPV6 ...\033[0m"
#	@$(STRIP) $(APPLET)
#	@cp $(APPLET) $(TOPDIR)/rootfs/usr/sbin/.
#	@ln -s -f dhcp6-multi $(TOPDIR)/rootfs/usr/sbin/dhcp6c
#	@ln -s -f dhcp6-multi $(TOPDIR)/rootfs/usr/sbin/dhcp6s
ifeq ($(ELBOX_USE_IPV6),y)
#old config is for dhcpv6 stateless
#	@echo "interface br0 {" > $(TOPDIR)/rootfs/etc/templates/dhcpv6c.conf
#	@echo "	information-only;" >> $(TOPDIR)/rootfs/etc/templates/dhcpv6c.conf
#	@echo "	script \"/var/servd/WAN-4-dhcp6c.sh\";" >> $(TOPDIR)/rootfs/etc/templates/dhcpv6c.conf
#	@echo "};" >> $(TOPDIR)/rootfs/etc/templates/dhcpv6c.conf
#new config is for dhcpv6 stateful
	@echo "interface br0 {" > $(TOPDIR)/rootfs/etc/templates/dhcpv6c.conf
	@echo "	request domain-name-servers;" >> $(TOPDIR)/rootfs/etc/templates/dhcpv6c.conf
	@echo "	request domain-name;" >> $(TOPDIR)/rootfs/etc/templates/dhcpv6c.conf 
	@echo "	send ia-na 112215;" >> $(TOPDIR)/rootfs/etc/templates/dhcpv6c.conf
	@echo "	send ia-pd 888888;" >> $(TOPDIR)/rootfs/etc/templates/dhcpv6c.conf     
	@echo "	script \"/etc/templates/dhcp6c_script.sh\";" >> $(TOPDIR)/rootfs/etc/templates/dhcpv6c.conf
	@echo "};" >> $(TOPDIR)/rootfs/etc/templates/dhcpv6c.conf
	@echo "id-assoc na 112215 { #;" >> $(TOPDIR)/rootfs/etc/templates/dhcpv6c.conf
	@echo "};" >> $(TOPDIR)/rootfs/etc/templates/dhcpv6c.conf 
	@echo "id-assoc pd 888888{" >> $(TOPDIR)/rootfs/etc/templates/dhcpv6c.conf
	@echo "	prefix-interface br0 {" >> $(TOPDIR)/rootfs/etc/templates/dhcpv6c.conf
	@echo "		sla-id 1;" >> $(TOPDIR)/rootfs/etc/templates/dhcpv6c.conf
	@echo "	};" >> $(TOPDIR)/rootfs/etc/templates/dhcpv6c.conf	
	@echo "};" >> $(TOPDIR)/rootfs/etc/templates/dhcpv6c.conf 
endif

includes::

clean::
	@echo -e "\033[32mCleaning WIDE-DHCPV6 ...\033[0m"
	@/bin/rm -f *.o $(APPLET) $(CLEANFILES) $(GENSRCS)

distclean:: clean
#	/bin/rm -f Makefile config.cache config.log config.status .depend

depend:
	mkdep ${CFLAGS:M-[ID]*} $(srcdir)/*.c

package:
	tar -zcvf wide-dhcpv6.tar.gz $(srcdir)/*.[chyl1-8] $(srcdir)/Makefile.in \
		$(srcdir)/README $(srcdir)/COPYRIGHT $(srcdir)/CHANGES \
		$(srcdir)/configure $(srcdir)/configure.in \
		$(srcdir)/install-sh  $(srcdir)/*.sample \
		$(srcdir)/missing/arc4random.?  $(srcdir)/missing/strlcat.c \
		$(srcdir)/missing/strlcpy.c $(srcdir)/missing/daemon.c \
		$(srcdir)/missing/err.h $(srcdir)/missing/warnx.c \
		$(srcdir)/missing/ifaddrs.h $(srcdir)/missing/getifaddrs.c \
		$(srcdir)/missing/sys/queue.h


