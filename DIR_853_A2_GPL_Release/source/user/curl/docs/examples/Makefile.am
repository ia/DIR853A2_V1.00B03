#
# $Id: //WIFI_SOC/MP/SDK_5_0_0_0/RT288x_SDK/source/user/curl/docs/examples/Makefile.am#1 $
#

AUTOMAKE_OPTIONS = foreign nostdinc

EXTRA_DIST = README Makefile.example Makefile.inc Makefile.m32 \
	makefile.dj $(COMPLICATED_EXAMPLES)

# Specify our include paths here, and do it relative to $(top_srcdir) and
# $(top_builddir), to ensure that these paths which belong to the library
# being currently built and tested are searched before the library which
# might possibly already be installed in the system.
#
# $(top_builddir)/include is for libcurl's generated curl/curlbuild.h file
# $(top_srcdir)/include is for libcurl's external include files

INCLUDES = -I$(top_builddir)/include \
           -I$(top_srcdir)/include

LIBDIR = $(top_builddir)/lib

if STATICLIB
# we need this define when building with a static lib on Windows
STATICCPPFLAGS = -DCURL_STATICLIB
endif

CPPFLAGS = -DCURL_NO_OLDIES $(STATICCPPFLAGS)

# Dependencies
LDADD = $(LIBDIR)/libcurl.la

# Makefile.inc provides the check_PROGRAMS and COMPLICATED_EXAMPLES defines
include Makefile.inc


