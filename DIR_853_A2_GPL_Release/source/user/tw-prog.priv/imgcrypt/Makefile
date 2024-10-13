#
SHELL:=/bin/bash
#

NAME	= imgdecrypt
#memwatch.o 
DEPEND_FILES	= crypt.o main.o
#				  cgi-bin/crc32.o

CERT_PEM = public.pem

CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include -I$(ROOTDIR)/config 
CFLAGS += -I$(ROOTDIR)/user/openssl-1.0.2j/include
CFLAGS += -I$(ROOTDIR)/user/curl/include

RM = -rm
#CFLAG = -Os -O0 -Wall -g  -s -Wl,-z,relro,-z,now -mfloat-abi=soft -Wall -fno-strict-aliasing
CFLAG = -O0 -Wall -g 
#CFLAG	+= -g -DDEBUG
#-Werror
UMSW	= -DUSER_MANAGEMENT_SUPPORT
#OTHERS	= -DB_STATS -DB_FILL 
#-DDEBUG
CFLAG_OPTION  += -D_NVRAM_ -D__CONFIG_IPV6__
CFLAG_OPTION  += -D__XML_FORMAT__
#CFLAG_OPTION  += -D_UCI_ -DMEMWATCH  -D_CSRF_
CFLAG_OPTION  += -DWEBS -DNO_BALLOC -DUEMF -DOS="LINUX" -DLINUX $(UMSW) $(DASW) $(SSLSW) $(IFMODSW) $(OTHERS)
CFLAG_INCLUDE	+= -I. -I./ixml -I./ixml/xmldb 
#CFLAG_INCLUDE	+= -I./xml2json/include
#CFLAG_INCLUDE	+= -I$(SRCBASE)/router/libcfgadpt
#CFLAG_INCLUDE    += -I$(ROOTDIR)/user/apps/include
#CFLAG_INCLUDE	+= -I$(ROOTDIR)/router/apps/notify_rc
CFLAG += $(CFLAG_OPTION) $(CFLAG_INCLUDE)
CFLAGS += $(CFLAG)

#LDFLAGS += -L$(SRCBASE)/router/libcfgadpt
LDFLAGS +=  $(EXTRA_LDFLAGS)
LDLIBS	+= -L$(ROOTDIR)/user/openssl-1.0.2j/ -lcrypto

# $(UPLOAD_SET) $(UPLOAD_IMG)

all:

romfs:
	$(ROMFSINST) $(NAME) /bin/$(NAME)
	$(ROMFSINST) ./$(CERT_PEM) /etc_ro/$(CERT_PEM)

