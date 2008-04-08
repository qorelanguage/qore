libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

if COND_SINGLE_COMPILATION_UNIT
GLUT_SOURCES = single-compilation-unit.cc
else
GLUT_SOURCES = glut.cc glut-constants.cc
endif

if COND_MONOLITHIC
noinst_LTLIBRARIES = libglut.la
libglut_la_SOURCES = $(GLUT_SOURCES)
libglut_la_LDFLAGS = ${GLUT_LIBS} -static
else
pkglib_LTLIBRARIES = glut.la
glut_la_SOURCES = $(GLUT_SOURCES)
glut_la_LDFLAGS = -module -avoid-version ${GLUT_LIBS}
endif

INCLUDES = -I$(top_srcdir)/include $(GLUT_INCLUDES) $(OPENSSL_CPPFLAGS)

AM_CXXFLAGS = ${PTHREAD_CFLAGS}

if COND_PROFILE
AM_CXXFLAGS += -g -pg
else
if COND_DEBUG
AM_CXXFLAGS += -g
endif
endif

if !COND_MONOLITHIC
install-exec-hook:
	mv $(pkglibdir)/glut.$(MODULE_SUFFIX) $(pkglibdir)/glut.qmod
	rm -f $(pkglibdir)/glut.la $(pkglibdir)/glut.a
endif

