# Figures out where slist is defined, and then writes out the
# location to a header file specified in $1.  The output file also
# #defines the namespace that list_map is in LIST_NAMESPACE.

AC_DEFUN([AC_CXX_MAKE_SLIST_H],
  [AC_REQUIRE([AC_CXX_STL_LIST])
   AC_MSG_CHECKING(writing a helper file for including slist)

   AS_MKDIR_P([`AS_DIRNAME([$1])`])
   cat >$1 <<EOF
#include $ac_cv_cxx_slist
#ifndef LIST_NAMESPACE
#define LIST_NAMESPACE $ac_cv_cxx_list_namespace
#endif
using LIST_NAMESPACE::slist;
EOF

   AC_MSG_RESULT([$1])
])

# We check two things: where the include file is for slist, and
# what namespace slist lives in within that include file.  We
# include AC_TRY_COMPILE for all the combinations we've seen in the
# wild.  We define one of HAVE_SLIST or HAVE_EXT_SLIST depending
# on location, and LIST_NAMESPACE to be the namespace slist is
# defined in.  If we can't find a slist that works, we die!

AC_DEFUN([AC_CXX_STL_LIST],
  [AC_CACHE_CHECK(
      location of STL slist,
      ac_cv_cxx_stl_list,
      [AC_REQUIRE([AC_CXX_NAMESPACES])
      AC_LANG_SAVE
      AC_LANG_CPLUSPLUS
      AC_TRY_COMPILE([#include <slist>],
                     [slist<int> t; return 0;],
                     ac_cv_cxx_stl_list=slist)
      AC_TRY_COMPILE([#include <ext/slist>],
                     [__gnu_cxx::slist<int> t; return 0;],
                     ac_cv_cxx_stl_list=ext_slist)
      AC_TRY_COMPILE([#include <slist>],
                     [std::slist<int> t; return 0;],
                     ac_cv_cxx_stl_list=std_slist)
      AC_TRY_COMPILE([#include <slist>],
                     [stdext::slist<int> t; return 0;],
                     ac_cv_cxx_stl_list=stdext_slist)
      if test "$with_tibae" != "yes" -a "$try_stlport" = "yes"; then
	SAVE_CXXFLAGS="$CXXFLAGS"
      	SAVE_LDFLAGS="$LDFLAGS"
      	CXXFLAGS="$CXXFLAGS -library=stlport4"
      	LDFLAGS="$LDFLAGS -library=stlport4"
      	AC_TRY_COMPILE([#include <slist>],
                       [std::slist<int> t; return 0;],
                       ac_cv_cxx_stl_list=solaris_std_slist)
      	CXXFLAGS="$SAVE_CXXFLAGS"
      	LDFLAGS="$SAVE_LDFLAGS"
      fi
      AC_LANG_RESTORE])
   if test "$ac_cv_cxx_stl_list" = slist; then
      AC_DEFINE(HAVE_SLIST, 1, [define if the compiler has slist])
      AC_DEFINE(HAVE_QORE_SLIST, 1, [define if slist is known])
      ac_cv_cxx_slist="<slist>"
      ac_cv_cxx_list_set="<list_set>"
      ac_cv_cxx_list_namespace=""
   fi
   if test "$ac_cv_cxx_stl_list" = ext_slist; then
      AC_DEFINE(HAVE_EXT_SLIST, 1, [define if the compiler has slist])
      AC_DEFINE(HAVE_QORE_SLIST, 1, [define if slist is known])
      ac_cv_cxx_slist="<ext/slist>"
      ac_cv_cxx_list_set="<ext/list_set>"
      ac_cv_cxx_list_namespace="__gnu_cxx"
   fi
   if test "$ac_cv_cxx_stl_list" = std_slist; then
      AC_DEFINE(HAVE_SLIST, 1, [define if the compiler has slist])
      AC_DEFINE(HAVE_QORE_SLIST, 1, [define if slist is known])
      ac_cv_cxx_slist="<slist>"
      ac_cv_cxx_list_set="<list_set>"
      ac_cv_cxx_list_namespace="std"
   fi
   if test "$ac_cv_cxx_stl_list" = stdext_slist; then
      AC_DEFINE(HAVE_SLIST, 1, [define if the compiler has slist])
      AC_DEFINE(HAVE_QORE_SLIST, 1, [define if slist is known])
      ac_cv_cxx_slist="<slist>"
      ac_cv_cxx_list_set="<list_set>"
      ac_cv_cxx_list_namespace="stdext"
   fi
   if test "$ac_cv_cxx_stl_list" = solaris_std_slist; then
      if test "$with_tibae" = "yes"; then
	AC_MSG_WARN(disabling stlport slist support because it conflicts with std iostream needed by the TIBCO module)
      else
        AC_DEFINE(HAVE_SLIST, 1, [define if the compiler has slist])
        AC_DEFINE(HAVE_QORE_SLIST, 1, [define if slist is known])
        ac_cv_cxx_slist="<slist>"
        ac_cv_cxx_list_set="<list_set>"
        ac_cv_cxx_list_namespace="std"
        CXXFLAGS="$CXXFLAGS -library=stlport4"
        LDFLAGS="$LDFLAGS -library=stlport4"
     fi
   fi
   AC_DEFINE_UNQUOTED(LIST_NAMESPACE,$ac_cv_cxx_list_namespace,
                      [the namespace of slist])

   if test x"$ac_cv_cxx_stl_list" = x; then
      AC_MSG_WARN(couldn't find an STL slist)
   fi
])

