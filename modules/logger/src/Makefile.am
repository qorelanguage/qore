# issue #3069: make sure to install correctly when cross compiling
modexeclibdir=${libdir}/${mymodverdir}

MNAME=logger

if COND_MINGWCC
LOGGER_QPP=$(top_srcdir)/lib/qpp_native
else
LOGGER_QPP=$(top_srcdir)/lib/qpp
endif

dummy:
	echo "Build started!"

.qpp.cpp:
	$(LOGGER_QPP) -V $<

GENERATED_SOURCES = QC_LoggerPattern.cpp \
    QC_LoggerLevelBase.cpp \
    QC_LoggerEvent.cpp \
    QC_LoggerLayout.cpp

CLEANFILES = $(GENERATED_SOURCES)
if COND_SINGLE_COMPILATION_UNIT
LOGGER_SOURCES = logger_scu.cpp
logger_scu.cpp: $(GENERATED_SOURCES)
else
LOGGER_SOURCES = logger-module.cpp \
    QoreLoggerLevelBase.cpp \
    QoreLoggerPattern.cpp \
    QoreLoggerEvent.cpp
nodist_logger_la_SOURCES = $(GENERATED_SOURCES)
endif

modexeclib_LTLIBRARIES = logger.la
logger_la_SOURCES = $(LOGGER_SOURCES)
logger_la_LDFLAGS = -module -avoid-version ${LOGGER_LIBS} ${MODULE_LDFLAGS}

LOGGER_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/modules/logger/src $(QORE_LIB_CPPFLAGS)

AM_CPPFLAGS=$(QORE_CPPFLAGS) $(LOGGER_CPPFLAGS)

if COND_PROFILE
AM_CXXFLAGS=-g -pg
else
if COND_DEBUG
AM_CXXFLAGS=-g
endif
endif

if COND_MINGWCC
RM_OTHER=$(DESTDIR)$(modexeclibdir)/$(MNAME).dll.a
endif

install-exec-hook:
	mv $(DESTDIR)$(modexeclibdir)/$(MNAME).$(MODULE_SUFFIX) $(DESTDIR)$(modexeclibdir)/$(MNAME)-api-$(MODULE_API).qmod
	$(RM) $(DESTDIR)$(modexeclibdir)/$(MNAME).la $(DESTDIR)$(modexeclibdir)/$(MNAME).a $(RM_OTHER)

clean-local:
	$(RM) -r $(GENERATED_SOURCES)
	$(RM) *.qtest

uninstall-local:
	$(RM) $(DESTDIR)$(modexeclibdir)/$(MNAME)-api-$(MODULE_API).qmod
