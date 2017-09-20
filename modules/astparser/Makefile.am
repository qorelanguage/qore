moddir = $(userlibdir)

AUTOMAKE_OPTIONS = foreign

SUBDIRS = src

ACLOCAL_AMFLAGS=-I m4

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
