libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

if COND_SINGLE_COMPILATION_UNIT
OPENGL_SOURCES = single-compilation-unit.cc
else
OPENGL_SOURCES = opengl.cc opengl-constants.cc opengl-ext.cc opengl-glu.cc opengl-ext-constants.cc
endif

if COND_MONOLITHIC
noinst_LTLIBRARIES = libopengl.la
libopengl_la_SOURCES = $(OPENGL_SOURCES)
libopengl_la_LDFLAGS = ${OPENGL_LIBS} -static
else
pkglib_LTLIBRARIES = opengl.la
opengl_la_SOURCES = $(OPENGL_SOURCES)
opengl_la_LDFLAGS = -module -avoid-version ${OPENGL_LIBS}
endif

INCLUDES = -I$(top_srcdir)/include $(OPENGL_INCLUDES)

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
	mv $(pkglibdir)/opengl.$(MODULE_SUFFIX) $(pkglibdir)/opengl.qmod
	rm -f $(pkglibdir)/opengl.la $(pkglibdir)/opengl.a
endif

