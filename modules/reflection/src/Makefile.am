MNAME=reflection

if COND_MINGWCC
REFLECTION_QPP=$(top_srcdir)/lib/qpp_native
else
REFLECTION_QPP=$(top_srcdir)/lib/qpp
endif

dummy:
	echo "Build started!"

.qpp.cpp:
	$(REFLECTION_QPP) -V $<

GENERATED_SOURCES = QC_AbstractClass.cpp \
    QC_AbstractClassMember.cpp \
    QC_AbstractConstant.cpp \
    QC_AbstractMember.cpp \
    QC_AbstractMethod.cpp \
    QC_AbstractMethodVariant.cpp \
    QC_AbstractReflectionFunction.cpp \
    QC_AbstractVariant.cpp \
    QC_Class.cpp \
    QC_ClassConstant.cpp \
    QC_Constant.cpp \
    QC_ConstructorMethod.cpp \
    QC_ConstructorMethodVariant.cpp \
    QC_CopyMethod.cpp \
    QC_CopyMethodVariant.cpp \
    QC_DestructorMethod.cpp \
    QC_DestructorMethodVariant.cpp \
    QC_Function.cpp \
    QC_FunctionVariant.cpp \
    QC_GlobalVar.cpp \
    QC_Namespace.cpp \
    QC_NormalMember.cpp \
    QC_NormalMethod.cpp \
    QC_NormalMethodVariant.cpp \
    QC_PseudoClass.cpp \
    QC_PseudoMethod.cpp \
    QC_PseudoMethodVariant.cpp \
    QC_StaticMember.cpp \
    QC_StaticMethod.cpp \
    QC_StaticMethodVariant.cpp \
    QC_Type.cpp \
    QC_TypedHash.cpp \
    QC_TypedHashMember.cpp

CLEANFILES = $(GENERATED_SOURCES)
if COND_SINGLE_COMPILATION_UNIT
REFLECTION_SOURCES = reflection_scu.cpp
reflection_scu.cpp: $(GENERATED_SOURCES)
else
REFLECTION_SOURCES = reflection-module.cpp
nodist_reflection_la_SOURCES = $(GENERATED_SOURCES)
endif

modexeclib_LTLIBRARIES = reflection.la
reflection_la_SOURCES = $(REFLECTION_SOURCES)
reflection_la_LDFLAGS = -module -avoid-version ${REFLECTION_LIBS} ${MODULE_LDFLAGS}

REFLECTION_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/modules/reflection/src $(QORE_LIB_CPPFLAGS)

AM_CPPFLAGS=$(QORE_CPPFLAGS) $(REFLECTION_CPPFLAGS)

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
