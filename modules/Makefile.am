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

if COND_TIBAE
SUBDIRS += tibae
endif

if COND_PGSQL
SUBDIRS += pgsql
endif

if COND_SYBASE_DIR
SUBDIRS += sybase
endif
