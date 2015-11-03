# Figures out where unordered_map or hash_map is defined, and then writes out the
# location to a header file specified in $1.  The output file also
# #defines the namespace that hash_map is in HASH_NAMESPACE.
#
# You can use this *instead of* stl_hash.ac.  stl_hash.ac will
# also define HASH_NAMESPACE, and vars indicating where hash_map
# lives, but won't actually make an include file for you.  This
# routine is a higher-level wrapper around hash_map.

AC_DEFUN([AC_CXX_MAKE_HASH_MAP_H],
  [AC_REQUIRE([AC_CXX_STL_HASH])
   AC_MSG_CHECKING(writing a helper file for including unordered_map)

   AS_MKDIR_P([`AS_DIRNAME([$1])`])
   cat >$1 <<EOF
#include $ac_cv_cxx_hash_map
#ifndef HASH_NAMESPACE
#define HASH_NAMESPACE $ac_cv_cxx_hash_namespace
#endif
#ifdef HAVE_UNORDERED_MAP
using HASH_NAMESPACE::unordered_map;
#else
using HASH_NAMESPACE::hash_map;
#endif
using HASH_NAMESPACE::hash;
EOF

   AC_MSG_RESULT([$1])
])

# We check two things: where the include file is for hash_map, and
# what namespace hash_map lives in within that include file.  We
# include AC_TRY_COMPILE for all the combinations we've seen in the
# wild.  We define one of HAVE_HASH_MAP or HAVE_EXT_HASH_MAP depending
# on location, and HASH_NAMESPACE to be the namespace hash_map is
# defined in.  If we can't find a hash_map that works, we die!

AC_DEFUN([AC_CXX_STL_HASH],
  [AC_CACHE_CHECK(
      location of STL unordered_map,
      ac_cv_cxx_stl_hash,
      [AC_REQUIRE([AC_CXX_NAMESPACES])
      AC_LANG_SAVE
      AC_LANG_CPLUSPLUS
      AC_TRY_COMPILE([#include <unordered_map>],
                     [unordered_map<int, int> t; const unordered_map<int, int> &tr = t; tr.find(1); return 0;],
                     ac_cv_cxx_stl_hash=unordered_map)
      if test -z "$ac_cv_cxx_stl_hash"; then
            AC_TRY_COMPILE([#include <unordered_map>],
      	                   [std::unordered_map<int, int> t; const std::unordered_map<int, int> &tr = t; tr.find(1); return 0;],
                           ac_cv_cxx_stl_hash=std_unordered_map)
      fi
      if test -z "$ac_cv_cxx_stl_hash"; then
            AC_TRY_COMPILE([#include <tr1/unordered_map>],
                          [std::tr1::unordered_map<int, int> t; const std::tr1::unordered_map<int, int> &tr = t; tr.find(1); return 0;],
                          ac_cv_cxx_stl_hash=tr1_unordered_map)
      fi
      if test -z "$ac_cv_cxx_stl_hash"; then
           AC_TRY_COMPILE([#include <hash_map>],
                          [hash_map<int, int> t; return 0;],
                          ac_cv_cxx_stl_hash=hash_map)
      fi
      if test -z "$ac_cv_cxx_stl_hash"; then
           AC_TRY_COMPILE([#include <ext/hash_map>],
                          [__gnu_cxx::hash_map<int, int> t; return 0;],
                          ac_cv_cxx_stl_hash=ext_hash_map)
      fi
      if test -z "$ac_cv_cxx_stl_hash"; then
           AC_TRY_COMPILE([#include <hash_map>],
                          [std::hash_map<int, int> t; return 0;],
                          ac_cv_cxx_stl_hash=std_hash_map)
      fi
      if test -z "$ac_cv_cxx_stl_hash"; then
           AC_TRY_COMPILE([#include <hash_map>],
                          [stdext::hash_map<int, int> t; return 0;],
                          ac_cv_cxx_stl_hash=stdext_hash_map)
      fi
      if test -z "$ac_cv_cxx_stl_hash" -a "$with_tibae" != "yes" -a "$try_stlport" = "yes"; then
	SAVE_CXXFLAGS="$CXXFLAGS"
      	SAVE_LDFLAGS="$LDFLAGS"
      	CXXFLAGS="$CXXFLAGS -library=stlport4"
      	LDFLAGS="$LDFLAGS -library=stlport4"
      	AC_TRY_COMPILE([#include <hash_map>],
                       [std::hash_map<int, int> t; return 0;],
                       ac_cv_cxx_stl_hash=solaris_std_hash_map)
      	CXXFLAGS="$SAVE_CXXFLAGS"
      	LDFLAGS="$SAVE_LDFLAGS"
      fi
      AC_LANG_RESTORE])
   if test "$ac_cv_cxx_stl_hash" = unordered_map; then
      AC_DEFINE(HAVE_UNORDERED_MAP, 1, [define if the compiler has unordered_map])
      AC_DEFINE(HAVE_QORE_HASH_MAP, 1, [define if hash_map or unordered_map is known])
      ac_cv_cxx_hash_map="<unordered_map>"
      ac_cv_cxx_hash_namespace=""
   fi
   if test "$ac_cv_cxx_stl_hash" = std_unordered_map; then
      AC_DEFINE(HAVE_UNORDERED_MAP, 1, [define if the compiler has unordered_map])
      AC_DEFINE(HAVE_QORE_HASH_MAP, 1, [define if hash_map or unordered_map is known])
      ac_cv_cxx_hash_map="<unordered_map>"
      ac_cv_cxx_hash_namespace="std"
   fi
   if test "$ac_cv_cxx_stl_hash" = tr1_unordered_map; then
      AC_DEFINE(HAVE_UNORDERED_MAP, 1, [define if the compiler has unordered_map])
      AC_DEFINE(HAVE_QORE_HASH_MAP, 1, [define if hash_map or unordered_map is known])
      ac_cv_cxx_hash_map="<tr1/unordered_map>"
      ac_cv_cxx_hash_namespace="std::tr1"
   fi
   if test "$ac_cv_cxx_stl_hash" = hash_map; then
      AC_DEFINE(HAVE_HASH_MAP, 1, [define if the compiler has hash_map])
      AC_DEFINE(HAVE_HASH_SET, 1, [define if the compiler has hash_set])
      AC_DEFINE(HAVE_QORE_HASH_MAP, 1, [define if hash_map or unordered_map is known])
      ac_cv_cxx_hash_map="<hash_map>"
      ac_cv_cxx_hash_set="<hash_set>"
      ac_cv_cxx_hash_namespace=""
   fi
   if test "$ac_cv_cxx_stl_hash" = ext_hash_map; then
      AC_DEFINE(HAVE_EXT_HASH_MAP, 1, [define if the compiler has hash_map])
      AC_DEFINE(HAVE_EXT_HASH_SET, 1, [define if the compiler has hash_set])
      AC_DEFINE(HAVE_QORE_HASH_MAP, 1, [define if hash_map or unordered_map is known])
      ac_cv_cxx_hash_map="<ext/hash_map>"
      ac_cv_cxx_hash_set="<ext/hash_set>"
      ac_cv_cxx_hash_namespace="__gnu_cxx"
   fi
   if test "$ac_cv_cxx_stl_hash" = std_hash_map; then
      AC_DEFINE(HAVE_HASH_MAP, 1, [define if the compiler has hash_map])
      AC_DEFINE(HAVE_HASH_SET, 1, [define if the compiler has hash_set])
      AC_DEFINE(HAVE_QORE_HASH_MAP, 1, [define if hash_map or unordered_map is known])
      ac_cv_cxx_hash_map="<hash_map>"
      ac_cv_cxx_hash_set="<hash_set>"
      ac_cv_cxx_hash_namespace="std"
   fi
   if test "$ac_cv_cxx_stl_hash" = stdext_hash_map; then
      AC_DEFINE(HAVE_HASH_MAP, 1, [define if the compiler has hash_map])
      AC_DEFINE(HAVE_HASH_SET, 1, [define if the compiler has hash_set])
      AC_DEFINE(HAVE_QORE_HASH_MAP, 1, [define if hash_map or unordered_map is known])
      ac_cv_cxx_hash_map="<hash_map>"
      ac_cv_cxx_hash_set="<hash_set>"
      ac_cv_cxx_hash_namespace="stdext"
   fi
   if test "$ac_cv_cxx_stl_hash" = solaris_std_hash_map; then
      if test "$with_tibae" = "yes"; then
	AC_MSG_WARN(disabling stlport hash_map support because it conflicts with std iostream needed by the TIBCO module)
      else
        AC_DEFINE(HAVE_HASH_MAP, 1, [define if the compiler has hash_map])
        AC_DEFINE(HAVE_HASH_SET, 1, [define if the compiler has hash_set])
	AC_DEFINE(HAVE_QORE_HASH_MAP, 1, [define if hash_map or unordered_map is known])
        ac_cv_cxx_hash_map="<hash_map>"
        ac_cv_cxx_hash_set="<hash_set>"
        ac_cv_cxx_hash_namespace="std"
        CXXFLAGS="$CXXFLAGS -library=stlport4"
        LDFLAGS="$LDFLAGS -library=stlport4"
     fi
   fi
   AC_DEFINE_UNQUOTED(HASH_NAMESPACE,$ac_cv_cxx_hash_namespace,
                      [the namespace of unordered_map])

   if test x"$ac_cv_cxx_stl_hash" = x; then
      AC_MSG_WARN(couldn't find an STL unordered_map)
   fi
])

# Checks whether the compiler implements namespaces
AC_DEFUN([AC_CXX_NAMESPACES],
 [AC_CACHE_CHECK(whether the compiler implements namespaces,
                 ac_cv_cxx_namespaces,
                 [AC_LANG_SAVE
                  AC_LANG_CPLUSPLUS
                  AC_TRY_COMPILE([namespace Outer {
                                    namespace Inner { int i = 0; }}],
                                 [using namespace Outer::Inner; return i;],
                                 ac_cv_cxx_namespaces=yes,
                                 ac_cv_cxx_namespaces=no)
                  AC_LANG_RESTORE])
  if test "$ac_cv_cxx_namespaces" = yes; then
    AC_DEFINE(HAVE_NAMESPACES, 1, [define if the compiler implements namespaces])
  fi])

