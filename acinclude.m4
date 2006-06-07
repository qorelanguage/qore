

AC_DEFUN([AC_ORACLE_VERSION],[
  AC_MSG_CHECKING([Oracle version])
  if test -s "$ORACLE_DIR/orainst/unix.rgs"; then
    ORACLE_VERSION=`grep '"ocommon"' $ORACLE_DIR/orainst/unix.rgs | sed "s/[ ][ ]*/:/g" | cut -d: -f 6 | cut -c 2-4`
    test -z "$ORACLE_VERSION" && ORACLE_VERSION=7.3
  elif test -f $ORACLE_DIR/lib/libclntsh.$SHLIB_SUFFIX.10.1; then
    ORACLE_VERSION=10.1
  elif test -f $ORACLE_DIR/lib/libclntsh.$SHLIB_SUFFIX.10.0; then
    ORACLE_VERSION=10.0
  elif test -f $ORACLE_DIR/lib/libclntsh.$SHLIB_SUFFIX.9.0; then
    ORACLE_VERSION=9.0
  elif test -f $ORACLE_DIR/lib/libclntsh.$SHLIB_SUFFIX.8.0; then
    ORACLE_VERSION=8.1
  elif test -f $ORACLE_DIR/lib/libclntsh.$SHLIB_SUFFIX.1.0; then
    ORACLE_VERSION=8.0
  elif test -f $ORACLE_DIR/lib/libclntsh.a; then
    if test -f $ORACLE_DIR/lib/libcore4.a; then
      ORACLE_VERSION=8.0
    else
      ORACLE_VERSION=8.1
    fi
  else
    AC_MSG_ERROR([Oracle needed libraries not found, unset ORACLE_HOME to build without Oracle support])
  fi
  AC_MSG_RESULT($ORACLE_VERSION)
])

AC_DEFUN([AC_TIBCO_SDK_VERSION],[
    AC_MSG_CHECKING([TIBCO SDK version])
    if test -d "$SDK_ROOT"; then
    	if test -e "$SDK_ROOT"/prodinfo; then
	    TIBCO_SDK_VERSION=`grep SDK "$SDK_ROOT"/prodinfo|head -1|cut -f2`
	elif test -e "$SDK_ROOT"/version.txt; then
	    TIBCO_SDK_VERSION=`grep SDK "$SDK_ROOT"/version.txt|head -1|cut -b25- | cut -f1 -d\ `
	else
	    AC_MSG_ERROR([TIBCO SDK not found])
        fi
    fi
    AC_MSG_RESULT($TIBCO_SDK_VERSION)
])
dnl Available from the GNU Autoconf Macro Archive at:
dnl http://www.gnu.org/software/ac-archive/htmldoc/acx_pthread.html
dnl
AC_DEFUN([ACX_PTHREAD], [
AC_REQUIRE([AC_CANONICAL_HOST])
AC_LANG_SAVE
AC_LANG_C
acx_pthread_ok=no

# We used to check for pthread.h first, but this fails if pthread.h
# requires special compiler flags (e.g. on True64 or Sequent).
# It gets checked for in the link test anyway.

# First of all, check if the user has set any of the PTHREAD_LIBS,
# etcetera environment variables, and if threads linking works using
# them:
if test x"$PTHREAD_LIBS$PTHREAD_CFLAGS" != x; then
        save_CFLAGS="$CFLAGS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
        save_LIBS="$LIBS"
        LIBS="$PTHREAD_LIBS $LIBS"
        AC_MSG_CHECKING([for pthread_join in LIBS=$PTHREAD_LIBS with CFLAGS=$PTHREAD_CFLAGS])
        AC_TRY_LINK_FUNC(pthread_join, acx_pthread_ok=yes)
        AC_MSG_RESULT($acx_pthread_ok)
        if test x"$acx_pthread_ok" = xno; then
                PTHREAD_LIBS=""
                PTHREAD_CFLAGS=""
        fi
        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"
fi

# We must check for the threads library under a number of different
# names; the ordering is very important because some systems
# (e.g. DEC) have both -lpthread and -lpthreads, where one of the
# libraries is broken (non-POSIX).

# Create a list of thread flags to try.  Items starting with a "-" are
# C compiler flags, and other items are library names, except for "none"
# which indicates that we try without any flags at all, and "pthread-config"
# which is a program returning the flags for the Pth emulation library.

acx_pthread_flags="pthreads none -Kthread -kthread lthread -pthread -pthreads -mthreads pthread --thread-safe -mt pthread-config"

# The ordering *is* (sometimes) important.  Some notes on the
# individual items follow:

# pthreads: AIX (must check this before -lpthread)
# none: in case threads are in libc; should be tried before -Kthread and
#       other compiler flags to prevent continual compiler warnings
# -Kthread: Sequent (threads in libc, but -Kthread needed for pthread.h)
# -kthread: FreeBSD kernel threads (preferred to -pthread since SMP-able)
# lthread: LinuxThreads port on FreeBSD (also preferred to -pthread)
# -pthread: Linux/gcc (kernel threads), BSD/gcc (userland threads)
# -pthreads: Solaris/gcc
# -mthreads: Mingw32/gcc, Lynx/gcc
# -mt: Sun Workshop C (may only link SunOS threads [-lthread], but it
#      doesn't hurt to check since this sometimes defines pthreads too;
#      also defines -D_REENTRANT)
# pthread: Linux, etcetera
# --thread-safe: KAI C++
# pthread-config: use pthread-config program (for GNU Pth library)

case "${host_cpu}-${host_os}" in
        *solaris*)

        # On Solaris (at least, for some versions), libc contains stubbed
        # (non-functional) versions of the pthreads routines, so link-based
        # tests will erroneously succeed.  (We need to link with -pthread or
        # -lpthread.)  (The stubs are missing pthread_cleanup_push, or rather
        # a function called by this macro, so we could check for that, but
        # who knows whether they'll stub that too in a future libc.)  So,
        # we'll just look for -pthreads and -lpthread first:

        acx_pthread_flags="-pthread -pthreads pthread -mt $acx_pthread_flags"
        ;;
esac

if test x"$acx_pthread_ok" = xno; then
for flag in $acx_pthread_flags; do

        case $flag in
                none)
                AC_MSG_CHECKING([whether pthreads work without any flags])
                ;;

                -*)
                AC_MSG_CHECKING([whether pthreads work with $flag])
                PTHREAD_CFLAGS="$flag"
                ;;

		pthread-config)
		AC_CHECK_PROG(acx_pthread_config, pthread-config, yes, no)
		if test x"$acx_pthread_config" = xno; then continue; fi
		PTHREAD_CFLAGS="`pthread-config --cflags`"
		PTHREAD_LIBS="`pthread-config --ldflags` `pthread-config --libs`"
		;;

                *)
                AC_MSG_CHECKING([for the pthreads library -l$flag])
                PTHREAD_LIBS="-l$flag"
                ;;
        esac

        save_LIBS="$LIBS"
        save_CFLAGS="$CFLAGS"
        LIBS="$PTHREAD_LIBS $LIBS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

        # Check for various functions.  We must include pthread.h,
        # since some functions may be macros.  (On the Sequent, we
        # need a special flag -Kthread to make this header compile.)
        # We check for pthread_join because it is in -lpthread on IRIX
        # while pthread_create is in libc.  We check for pthread_attr_init
        # due to DEC craziness with -lpthreads.  We check for
        # pthread_cleanup_push because it is one of the few pthread
        # functions on Solaris that doesn't have a non-functional libc stub.
        # We try pthread_create on general principles.
        AC_TRY_LINK([#include <pthread.h>],
                    [pthread_t th; pthread_join(th, 0);
                     pthread_attr_init(0); pthread_cleanup_push(0, 0);
                     pthread_create(0,0,0,0); pthread_cleanup_pop(0); ],
                    [acx_pthread_ok=yes])

        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"

        AC_MSG_RESULT($acx_pthread_ok)
        if test "x$acx_pthread_ok" = xyes; then
                break;
        fi

        PTHREAD_LIBS=""
        PTHREAD_CFLAGS=""
done
fi

# Various other checks:
if test "x$acx_pthread_ok" = xyes; then
        save_LIBS="$LIBS"
        LIBS="$PTHREAD_LIBS $LIBS"
        save_CFLAGS="$CFLAGS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

        # Detect AIX lossage: threads are created detached by default
        # and the JOINABLE attribute has a nonstandard name (UNDETACHED).
        AC_MSG_CHECKING([for joinable pthread attribute])
        AC_TRY_LINK([#include <pthread.h>],
                    [int attr=PTHREAD_CREATE_JOINABLE;],
                    ok=PTHREAD_CREATE_JOINABLE, ok=unknown)
        if test x"$ok" = xunknown; then
                AC_TRY_LINK([#include <pthread.h>],
                            [int attr=PTHREAD_CREATE_UNDETACHED;],
                            ok=PTHREAD_CREATE_UNDETACHED, ok=unknown)
        fi
        if test x"$ok" != xPTHREAD_CREATE_JOINABLE; then
                AC_DEFINE(PTHREAD_CREATE_JOINABLE, $ok,
                          [Define to the necessary symbol if this constant
                           uses a non-standard name on your system.])
        fi
        AC_MSG_RESULT(${ok})
        if test x"$ok" = xunknown; then
                AC_MSG_WARN([we do not know how to create joinable pthreads])
        fi

        AC_MSG_CHECKING([if more special flags are required for pthreads])
        flag=no
        case "${host_cpu}-${host_os}" in
                *-aix* | *-freebsd* | *-darwin*) flag="-D_THREAD_SAFE";; 
	        *-linux*) flag="-D_THREAD_SAFE";;
                *solaris* | *-osf* | *-hpux*) flag="-D_REENTRANT";;
        esac
        AC_MSG_RESULT(${flag})
        if test "x$flag" != xno; then
                PTHREAD_CFLAGS="$flag $PTHREAD_CFLAGS"
        fi

        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"

        # More AIX lossage: must compile with cc_r
        AC_CHECK_PROG(PTHREAD_CC, cc_r, cc_r, ${CC})
else
        PTHREAD_CC="$CC"
fi

AC_SUBST(PTHREAD_LIBS)
AC_SUBST(PTHREAD_CFLAGS)
AC_SUBST(PTHREAD_CC)

##echo PTHREAD_CFLAGS are ${PTHREAD_CFLAGS}

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test x"$acx_pthread_ok" = xyes; then
        ifelse([$1],,AC_DEFINE(HAVE_PTHREAD,1,[Define if you have POSIX threads libraries and header files.]),[$1])
        :
else
        acx_pthread_ok=no
        $2
fi
AC_LANG_RESTORE
])dnl ACX_PTHREAD
# Figures out where hash_map is defined, and then writes out the
# location to a header file specified in $1.  The output file also
# #defines the namespace that hash_map is in HASH_NAMESPACE.
#
# You can use this *instead of* stl_hash.ac.  stl_hash.ac will
# also define HASH_NAMESPACE, and vars indicating where hash_map
# lives, but won't actually make an include file for you.  This
# routine is a higher-level wrapper around hash_map.

AC_DEFUN([AC_CXX_MAKE_HASH_MAP_H],
  [AC_REQUIRE([AC_CXX_STL_HASH])
   AC_MSG_CHECKING(writing a helper file for including hash_map)

   AS_MKDIR_P([`AS_DIRNAME([$1])`])
   cat >$1 <<EOF
#include $ac_cv_cxx_hash_map
#ifndef HASH_NAMESPACE
#define HASH_NAMESPACE $ac_cv_cxx_hash_namespace
#endif
using HASH_NAMESPACE::hash_map;
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
      location of STL hash_map,
      ac_cv_cxx_stl_hash,
      [AC_REQUIRE([AC_CXX_NAMESPACES])
      AC_LANG_SAVE
      AC_LANG_CPLUSPLUS
      AC_TRY_COMPILE([#include <hash_map>],
                     [hash_map<int, int> t; return 0;],
                     ac_cv_cxx_stl_hash=hash_map)
      AC_TRY_COMPILE([#include <ext/hash_map>],
                     [__gnu_cxx::hash_map<int, int> t; return 0;],
                     ac_cv_cxx_stl_hash=ext_hash_map)
      AC_TRY_COMPILE([#include <hash_map>],
                     [std::hash_map<int, int> t; return 0;],
                     ac_cv_cxx_stl_hash=std_hash_map)
      AC_TRY_COMPILE([#include <hash_map>],
                     [stdext::hash_map<int, int> t; return 0;],
                     ac_cv_cxx_stl_hash=stdext_hash_map)
      if test "$with_tibco" != "yes"; then
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
   if test "$ac_cv_cxx_stl_hash" = hash_map; then
      AC_DEFINE(HAVE_HASH_MAP, 1, [define if the compiler has hash_map])
      AC_DEFINE(HAVE_HASH_SET, 1, [define if the compiler has hash_set])
      AC_DEFINE(HAVE_QORE_HASH_MAP, 1, [define if hash_map is known])
      ac_cv_cxx_hash_map="<hash_map>"
      ac_cv_cxx_hash_set="<hash_set>"
      ac_cv_cxx_hash_namespace=""
   fi
   if test "$ac_cv_cxx_stl_hash" = ext_hash_map; then
      AC_DEFINE(HAVE_EXT_HASH_MAP, 1, [define if the compiler has hash_map])
      AC_DEFINE(HAVE_EXT_HASH_SET, 1, [define if the compiler has hash_set])
      AC_DEFINE(HAVE_QORE_HASH_MAP, 1, [define if hash_map is known])
      ac_cv_cxx_hash_map="<ext/hash_map>"
      ac_cv_cxx_hash_set="<ext/hash_set>"
      ac_cv_cxx_hash_namespace="__gnu_cxx"
   fi
   if test "$ac_cv_cxx_stl_hash" = std_hash_map; then
      AC_DEFINE(HAVE_HASH_MAP, 1, [define if the compiler has hash_map])
      AC_DEFINE(HAVE_HASH_SET, 1, [define if the compiler has hash_set])
      AC_DEFINE(HAVE_QORE_HASH_MAP, 1, [define if hash_map is known])
      ac_cv_cxx_hash_map="<hash_map>"
      ac_cv_cxx_hash_set="<hash_set>"
      ac_cv_cxx_hash_namespace="std"
   fi
   if test "$ac_cv_cxx_stl_hash" = stdext_hash_map; then
      AC_DEFINE(HAVE_HASH_MAP, 1, [define if the compiler has hash_map])
      AC_DEFINE(HAVE_HASH_SET, 1, [define if the compiler has hash_set])
      AC_DEFINE(HAVE_QORE_HASH_MAP, 1, [define if hash_map is known])
      ac_cv_cxx_hash_map="<hash_map>"
      ac_cv_cxx_hash_set="<hash_set>"
      ac_cv_cxx_hash_namespace="stdext"
   fi
   if test "$ac_cv_cxx_stl_hash" = solaris_std_hash_map; then
      if test "$with_tibco" = "yes"; then
	AC_MSG_WARN(disabling stlport hash_map support because it conflicts with std iostream needed by the TIBCO module)
      else
        AC_DEFINE(HAVE_HASH_MAP, 1, [define if the compiler has hash_map])
        AC_DEFINE(HAVE_HASH_SET, 1, [define if the compiler has hash_set])
        AC_DEFINE(HAVE_QORE_HASH_MAP, 1, [define if hash_map is known])
        ac_cv_cxx_hash_map="<hash_map>"
        ac_cv_cxx_hash_set="<hash_set>"
        ac_cv_cxx_hash_namespace="std"
        CXXFLAGS="$CXXFLAGS -library=stlport4"
        LDFLAGS="$LDFLAGS -library=stlport4"
     fi
   fi
   AC_DEFINE_UNQUOTED(HASH_NAMESPACE,$ac_cv_cxx_hash_namespace,
                      [the namespace of hash_map])

   if test x"$ac_cv_cxx_stl_hash" = x; then
      AC_MSG_WARN(couldn't find an STL hash_map)
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

AC_DEFUN([TYPE_SOCKLEN_T],
[AC_CACHE_CHECK([for socklen_t], ac_cv_type_socklen_t,
[
  AC_TRY_COMPILE(
  [#include <sys/types.h>
   #include <sys/socket.h>],
  [socklen_t len = 42; return 0;],
  ac_cv_type_socklen_t=yes,
  ac_cv_type_socklen_t=no)
])
  if test $ac_cv_type_socklen_t != yes; then
    AC_DEFINE(socklen_t, int, [Substitute for socklen_t])
  fi
])
