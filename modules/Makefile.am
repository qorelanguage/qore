libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/$(mypkglibdir)

SUBDIRS =

INCLUDES = -I$(top_srcdir)/include

AM_CXXFLAGS =

if COND_ORACLE
SUBDIRS += oracle
endif

if COND_MYSQL
SUBDIRS += mysql
endif
