pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

SUBDIRS =

INCLUDES = -I$(top_srcdir)/include

AM_CXXFLAGS =

if COND_NCURSES
SUBDIRS += ncurses
endif

if COND_ORACLE
SUBDIRS += oracle
endif

if COND_MYSQL
SUBDIRS += mysql
endif

if COND_TIBCO
SUBDIRS += TIBCO
endif
