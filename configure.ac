# Process this file with autoconf to produce a configure script.

# AC_PREREQ(2.59)
AC_INIT([qore], [0.5.2],
        [David Nichols <nicholsman@gmail.com>],
        [qore])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(include/qore/unix-config.h)

AC_PROG_LEX
##AC_PROG_MAKE_SET
AC_C_BIGENDIAN

# test for flex & flex version
AC_MSG_CHECKING([for flex 2.5.31 or greater])
if test -z "`echo $LEX | grep flex`"; then
        AC_MSG_ERROR([no flex version found])
elif test -z "`$LEX --version | grep -e ' 2\.5\.3[[1-9]]' -e '2\.5\.[[4-9]][[0-9]]' -e '2\.\[[6-9]]\.'`" ; then
        flexver=`$LEX --version`
	AC_MSG_ERROR([incompatible $flexver found - you can download flex 2.5.31 at http://sourceforge.net/projects/lex])
else
	AC_MSG_RESULT([yes])
fi

AC_PROG_YACC

# test for bison & bison version
AC_MSG_CHECKING([for bison 1.85 or greater])
if test -z "`echo $YACC | grep bison`"; then
	AC_MSG_ERROR([bison not found])
elif test -z "`$YACC --version | head -n 1 | grep -e ' 2\.' -e '1\.8[[5-9]]*' -e '1\.9'`"; then
        bisonver=`$YACC --version | head -n 1`
	AC_MSG_ERROR([incompatible $bisonver found])
else
	AC_MSG_RESULT([yes])
fi

AC_LANG(C++)
AC_PROG_CXX
AC_PROG_CC
AC_LIBTOOL_DLOPEN
AM_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_RANLIB

# check if socklen_t is needed
TYPE_SOCKLEN_T

# see if the iconv library is required
AC_CHECK_LIB(iconv, iconv)

# see if the socket library is required
AC_CHECK_LIB(socket, setsockopt)

# look for librt (needed by clock_gettime() on some platforms)
AC_CHECK_LIB(rt, clock_gettime)

#AC_CANONICAL_HOST
# get CPU family
case "${host_cpu}" in
   i[[3456]]86) cpu_family=i386 ;;
   *)           cpu_family=${host_cpu} ;;
esac

# do CPU specific actions
AC_MSG_CHECKING([if we have atomic operation support for ${cpu_family} cpus])
rm -f include/qore/macros.h
if test -e include/qore/macros-${cpu_family}.h; then
   (cd include/qore; ln -s macros-${cpu_family}.h macros.h)
   AC_MSG_RESULT([ok])
else
   (cd include/qore; ln -s macros-none.h macros.h)
   AC_MSG_RESULT([no])
fi

# set OS-specific variables
SHLIB_SUFFIX=so
#echo host_os=${host_os}
case "${host_os}" in
     *linux*)   AC_DEFINE(NEED_ICONV_TRANSLIT, 1, Define to 1 if //TRANSLIT should be added to the destination character set code) 
# set compile flags for Linux
CPPFLAGS="${CPPFLAGS} -D_GNU_SOURCE" 
		SHLIB_SUFFIX=so
		AC_DEFINE(LINUX, 1, if compiling on Linux)
		;;
     *solaris*) # set compile flags for Solaris
CPPFLAGS="${CPPFLAGS} -D_POSIX_C_SOURCE=199506L -D_XPG4_2 -D_XPG5 -D__EXTENSIONS__"
# for checking for some network functions on Solaris
LDFLAGS="-lnsl -norunpath"
		AC_DEFINE(SOLARIS, 1, if compiling on Solaris)
	        ;;
     *darwin*)  SHLIB_SUFFIX=dylib
		# libtool 1.5.10 is creating modules on Darwin 8.2.0 (OS/X 10.4.2) with a .so suffix for some reason
		MODULE_SUFFIX=so
		AC_DEFINE(DARWIN, 1, if compiling on Darwin)
		;;
     *hpux*)    SHLIB_SUFFIX=sl 
		AC_DEFINE(SHLIB_SUFFIX, "sl", shared library suffix)
		AC_DEFINE(HPUX, 1, if compiling on HP-UX)
		;;
esac
AC_SUBST(SHLIB_SUFFIX)
if test -z "$MODULE_SUFFIX" ; then
   MODULE_SUFFIX=$SHLIB_SUFFIX
fi
AC_SUBST(MODULE_SUFFIX)

AC_ARG_ENABLE([64bit],
  [AC_HELP_STRING([--enable-64bit],
		  [enable 64bit support (default: off)])],
  [case "${enable_64bit}" in
       yes|no) ;;
       *)      AC_MSG_ERROR(bad value ${enable_64bit} for --enable-64bit) ;;
      esac],
  [enable_64bit=no])

# see if we know how to set flags properly for different hosts and architectures
if test "$enable_64bit" = "yes"; then
   case "${host_os}" in
     *linux*)	if test "$ac_cv_prog_gxx" = "yes"; then
		CXXFLAGS="$CXXFLAGS -m64"
		LIBSUFFIX=64
		AC_SUBST(LIBSUFFIX)
		fi
		;;
   esac
else
   case "${host_os}" in
     *linux*)	if test "$ac_cv_prog_gxx" = "yes"; then
		CXXFLAGS="$CXXFLAGS -m32"
		fi
		;;
   esac
fi

# see if we can figure out where the openssl library is
AC_ARG_WITH([openssl-libs],
  [AC_HELP_STRING([--with-openssl-libs@<:@=DIR@:>@],
                  [openssl library directory])],
  [if test ! -d "${with_openssl_libs}"; then AC_MSG_ERROR(bad value ${with_openssl_libs} for --with-openssl-libs); unset with_openssl_libs; fi])

AC_MSG_CHECKING([for openssl library])
if test -n "$with_openssl_libs"; then
   if test ! -f "$with_openssl_libs/libssl.a" -a ! -f "$with_openssl_libs/libssl.$SHLIB_SUFFIX"; then
      AC_MSG_ERROR([can't find openssl library in $with_openssl_libs])
   else
      AC_MSG_RESULT([$with_openssl_libs])
      if test "$with_openssl_libs" = "/usr/lib${LIBSUFFIX}"; then
         with_openssl_libs=
      elif test "$with_openssl_libs" = "/lib${LIBSUFFIX}"; then
         with_openssl_libs=
      fi
   fi
else
   if test -n "$OPENSSL_DIR" -a \( -f $OPENSSL_DIR/lib${LIBSUFFIX}/libssl.a -o -f "$OPENSSL_DIR/lib${LIBSUFFIX}/libssl.$SHLIB_SUFFIX" \); then
      AC_MSG_RESULT([$OPENSSL_DIR/lib${LIBSUFFIX}])
      with_openssl_libs=$OPENOPENSSL_DIR/lib${LIBSUFFIX}
   elif test -f /usr/lib${LIBSUFFIX}/libssl.a -o -f /usr/lib${LIBSUFFIX}/libssl.$SHLIB_SUFFIX; then
      AC_MSG_RESULT([/usr/lib${LIBSUFFIX}])
   elif test -f /lib${LIBSUFFIX}/libssl.a -o -f /lib${LIBSUFFIX}/libssl.$SHLIB_SUFFIX; then
      AC_MSG_RESULT([/lib${LIBSUFFIX}])
   elif test -f /opt/openssl/lib${LIBSUFFIX}/libssl.a -o -f /opt/openssl/lib${LIBSUFFIX}/libssl.$SHLIB_SUFFIX; then
      AC_MSG_RESULT([/opt/openssl/lib${LIBSUFFIX}])
      with_openssl_libs=/opt/openssl/lib${LIBSUFFIX}
   elif test -f /sw/lib/libssl.a -o -f /sw/lib/libssl.$SHLIB_SUFFIX; then
      AC_MSG_RESULT([/sw/lib])
      with_openssl_libs=/sw/lib
   elif test -f /opt/local/lib${LIBSUFFIX}/libssl.a -o -f /opt/local/lib${LIBSUFFIX}/libssl.$SHLIB_SUFFIX; then
      AC_MSG_RESULT([/opt/local/lib${LIBSUFFIX}])
      with_openssl_libs=/opt/local/lib${LIBSUFFIX}
   else
      AC_MSG_ERROR([no openssl library found])
   fi
fi

if test -n "$with_openssl_libs"; then
   OPENSSL_LDFLAGS="-L$with_openssl_libs -lssl -lcrypto"
else
   OPENSSL_LDFLAGS="-lssl -lcrypto"
fi
AC_SUBST(OPENSSL_LDFLAGS)

# see if we can figure out where the ssl header file is
AC_ARG_WITH([openssl-includes],
  [AC_HELP_STRING([--with-openssl-includes@<:@=DIR@:>@],
                  [SSL include file directory])],
  [if test ! -d "${with_openssl_includes}"; then AC_MSG_ERROR(bad value ${with_openssl_includes} for --with-openssl-includes); unset with_openssl_includes; fi])

AC_MSG_CHECKING([for openssl/ssl.h])
if test -n "$with_openssl_includes"; then
   if test ! -f "$with_openssl_includes/openssl/ssl.h"; then
      AC_MSG_ERROR([can't find openssl/ssl.h in $with_openssl_includes])
   else
      AC_MSG_RESULT([$with_openssl_includes])
      if test "$with_openssl_includes" = "/usr/include"; then
         with_openssl_includes=
      fi
   fi
else
   if test -n "$OPENSSL_DIR" -a -f $OPENSSL_DIR/include/openssl/ssl.h ; then
      AC_MSG_RESULT([$OPENSSL_DIR/include])
      with_openssl_includes=$OPENSSL_DIR/include
   elif test -f /usr/include/openssl/ssl.h ; then
      AC_MSG_RESULT([/usr/include])
   elif test -f /opt/openssl/include/openssl/ssl.h; then
      AC_MSG_RESULT([/opt/openssl/include])
      with_openssl_includes=/opt/openssl/include
   elif test -f /sw/include/openssl/ssl.h; then
      AC_MSG_RESULT([/sw/include])
      with_openssl_includes=/sw/include
   elif test -f /opt/local/include/openssl/ssl.h; then
      AC_MSG_RESULT([/opt/local/include])
      with_openssl_includes=/opt/local/include
   else
      AC_MSG_ERROR([no ssl.h header file found])
   fi
fi

if test -n "$with_openssl_includes"; then
   OPENSSL_CPPFLAGS=-I$with_openssl_includes
fi
AC_SUBST(OPENSSL_CPPFLAGS)

# see if we can figure out where the pcre library is
AC_ARG_WITH([pcre-libs],
  [AC_HELP_STRING([--with-pcre-libs@<:@=DIR@:>@],
                  [PCRE library directory])],
  [if test ! -d "${with_pcre_libs}"; then AC_MSG_ERROR(bad value ${with_pcre_libs} for --with-pcre-libs); unset with_pcre_libs; fi])

AC_MSG_CHECKING([for pcre library])
if test -n "$with_pcre_libs"; then
   if test ! -f "$with_pcre_libs/libpcre.a" -a ! -f "$with_pcre_libs/libpcre.$SHLIB_SUFFIX"; then
      AC_MSG_ERROR([can't find pcre library in $with_pcre_libs])
   else
      AC_MSG_RESULT([$with_pcre_libs])
      if test "$with_pcre_libs" = "/usr/lib${LIBSUFFIX}"; then
         with_pcre_libs=
      fi
   fi
else
   if test -n "$PCRE_DIR" -a \( -f $PCRE_DIR/lib${LIBSUFFIX}/libpcre.a -o -f "$PCRE_DIR/lib${LIBSUFFIX}/libpcre.$SHLIB_SUFFIX" \); then
      AC_MSG_RESULT([$PCRE_DIR/lib${LIBSUFFIX}])
      with_pcre_libs=$PCRE_DIR/lib${LIBSUFFIX}
   elif test -f /usr/lib${LIBSUFFIX}/libpcre.a -o -f /usr/lib${LIBSUFFIX}/libpcre.$SHLIB_SUFFIX; then
      AC_MSG_RESULT([/usr/lib${LIBSUFFIX}])
   elif test -f /opt/pcre/lib${LIBSUFFIX}/libpcre.a -o -f /opt/pcre/lib${LIBSUFFIX}/libpcre.$SHLIB_SUFFIX; then
      AC_MSG_RESULT([/opt/pcre/lib${LIBSUFFIX}])
      with_pcre_libs=/opt/pcre/lib${LIBSUFFIX}
   elif test -f /usr/pcre/lib${LIBSUFFIX}/libpcre.a -o -f /usr/pcre/lib${LIBSUFFIX}/libpcre.$SHLIB_SUFFIX; then
      AC_MSG_RESULT([/usr/pcre/lib${LIBSUFFIX}])
      with_pcre_libs=/usr/pcre/lib${LIBSUFFIX}
   elif test -f /sw/lib/libpcre.a -o -f /sw/lib/libpcre.$SHLIB_SUFFIX; then
      AC_MSG_RESULT([/sw/lib])
      with_pcre_libs=/sw/lib
   elif test -f /opt/local/lib${LIBSUFFIX}/libpcre.a -o -f /opt/local/lib${LIBSUFFIX}/libpcre.$SHLIB_SUFFIX; then
      AC_MSG_RESULT([/opt/local/lib${LIBSUFFIX}])
      with_pcre_libs=/opt/local/lib${LIBSUFFIX}
   else
      AC_MSG_ERROR([no PCRE library found])
   fi
fi

if test -n "$with_pcre_libs"; then
   PCRE_LDFLAGS="-L$with_pcre_libs -lpcre"
else
   PCRE_LDFLAGS=-lpcre
fi
AC_SUBST(PCRE_LDFLAGS)

# see if we can figure out where the pcre header file is
AC_ARG_WITH([pcre-includes],
  [AC_HELP_STRING([--with-pcre-includes@<:@=DIR@:>@],
                  [PCRE include file directory])],
  [if test ! -d "${with_pcre_includes}"; then AC_MSG_ERROR(bad value ${with_pcre_includes} for --with-pcre-includes); unset with_pcre_includes; fi])

AC_MSG_CHECKING([for pcre.h])
if test -n "$with_pcre_includes"; then
   if test ! -e "$with_pcre_includes/pcre.h"; then
      AC_MSG_ERROR([can't find pcre.h in $with_pcre_includes])
   else
      AC_MSG_RESULT([$with_pcre_includes])
      if test "$with_pcre_includes" = "/usr/include"; then
         with_pcre_includes=
      fi
   fi
else
   if test -n "$PCRE_DIR" -a -f $PCRE_DIR/include/pcre.h ; then
      AC_MSG_RESULT([$PCRE_DIR/include])
      with_pcre_includes=$PCRE_DIR/include
   elif test -f /usr/include/pcre.h ; then
      AC_MSG_RESULT([/usr/include])
   elif test -f /opt/pcre/include/pcre.h; then
      AC_MSG_RESULT([/opt/pcre/include])
      with_pcre_includes=/opt/pcre/include
   elif test -f /usr/pcre/include/pcre.h; then
      AC_MSG_RESULT([/usr/pcre/include])
      with_pcre_includes=/usr/pcre/include
   elif test -f /sw/include/pcre.h; then
      AC_MSG_RESULT([/sw/include])
      with_pcre_includes=/sw/include
   elif test -f /opt/local/include/pcre.h; then
      AC_MSG_RESULT([/opt/local/include])
      with_pcre_includes=/opt/local/include
   else
      AC_MSG_ERROR([no pcre.h header file found])
   fi
fi

if test -n "$with_pcre_includes"; then
   PCRE_CPPFLAGS=-I$with_pcre_includes
fi
AC_SUBST(PCRE_CPPFLAGS)

AC_ARG_ENABLE([profile],
     [AC_HELP_STRING([--enable-profile],
		     [turn on profiling support (default=no)])],
     [case "${enable_profile}" in
       yes|no) ;;
       *)      AC_MSG_ERROR(bad value ${enable_profile} for --enable-profile) ;;
      esac],
     [enable_profile=no])

AC_ARG_ENABLE([debug],
     [AC_HELP_STRING([--enable-debug],
		     [turn on debugging (default=no)])],
     [case "${enable_debug}" in
       yes|no) ;;
       *)      AC_MSG_ERROR(bad value ${enable_debug} for --enable-debug) ;;
      esac],
     [enable_debug=yes])

if test "${enable_debug}" = yes; then
   AC_DEFINE(DEBUG, 1, Define if debugging support should be included)
# remove -O2 from CXXFLAGS so valgrind can report properly
CXXFLAGS=`echo $CXXFLAGS | sed 's/\-O2//'`
fi

AC_ARG_ENABLE([builtin-modules],
     [AC_HELP_STRING([--enable-builtin-modules],
		     [link in module code directly (default=no)])],
     [case "${enable_builtin_modules}" in
       yes|no) ;;
       *)      AC_MSG_ERROR(bad value ${enable_builtin_modules} for --enable-builtin-modules) ;;
      esac],
     [enable_builtin_modules=no])

AC_ARG_ENABLE([threads],
  [AC_HELP_STRING([--enable-threads],
                  [include threading support (default=yes)])],
  [case "$enable_threads" in
      yes|no) ;;
      *)      AC_MSG_ERROR([bad value ${enable_threads} for threads option]) ;;
   esac],
   [enable_threads=yes])

if test "${enable_threads}" = yes; then
    # checks for how to build threads
    ACX_PTHREAD
    if test "$acx_pthread_ok" = "no"; then
       AC_MSG_ERROR(POSIX threads do not seem to be supported on this platform, aborting)
    fi
fi

AC_ARG_WITH([tibrv],
  [AC_HELP_STRING([--with-tibrv@<:@=DIR@:>@],
		  [TIBCO Rendezvous directory (defaults to $RV_ROOT)])],
  [if test ! -d "${with_tibrv}"; then AC_MSG_ERROR(bad value ${with_tibrv} for --with-tibrv); unset with_tibrv; fi])

AC_MSG_CHECKING([TIBCO Rendezvous])
if test -n "$with_tibrv"; then
   if test ! -e "$with_tibrv/tibrv/tibrv.h"; then
      AC_MSG_ERROR([can't find tibrv.h in $with_tibrv/tibrv])
   elif test ! -e "$with_tibrv/lib/libtibrv.a"; then
      AC_MSG_ERROR([can't find libtibrv.a in $with_tibrv/lib])
   else
      AC_MSG_RESULT([$with_tibrv])
   fi
else
   if test -n "$RV_ROOT" -a -f "$RV_ROOT/include/tibrv/tibrv.h" -a -e "$RV_ROOT/lib/libtibrv.a"; then
      AC_MSG_RESULT([$RV_ROOT])
      with_tibrv=$RV_ROOT
   elif test -f /opt/tibrv/include/tibrv/tibrv.h -a -f /opt/tibrv/lib/libtibrv.a; then
      AC_MSG_RESULT([/opt/tibrv])
      with_tibrv=/opt/tibrv
   elif test -f /opt/tibco/tibrv/include/tibrv/tibrv.h -a -f /opt/tibco/tibrv/lib/libtibrv.a; then
      AC_MSG_RESULT([/opt/tibco/tibrv])
      with_tibrv=/opt/tibco/tibrv
   elif test -f /usr/tibrv/include/tibrv/tibrv.h -a -f /usr/tibrv/lib/libtibrv.a; then
      AC_MSG_RESULT([/usr/tibrv])
      with_tibrv=/usr/tibrv
   elif test -f /usr/tibco/tibrv/include/tibrv/tibrv.h -a -f /usr/tibco/tibrv/lib/libtibrv.a; then
      AC_MSG_RESULT([/usr/tibco/tibrv])
      with_tibrv=/usr/tibco/tibrv
   elif test -f /usr/local/tibco/tibrv/include/tibrv/tibrv.h -a -f /usr/local/tibco/tibrv/lib/libtibrv.a; then
      AC_MSG_RESULT([/usr/tibco/local/tibrv])
      with_tibrv=/usr/local/tibco/tibrv
   else
      AC_MSG_RESULT([not found])
      unset with_tibrv
   fi
fi

if test -n "$with_tibrv"; then

   # setup tibrv compile and link vars
   TIBRV_CPPFLAGS="-I${with_tibrv}/include"
   TIBRV_LIBS="-L${with_tibrv}/lib -ltibrv${LIBSUFFIX} -ltibrvcm${LIBSUFFIX} -ltibrvcmq${LIBSUFFIX} -ltibrvcpp${LIBSUFFIX} -ltibrvft${LIBSUFFIX} -ltibrvsd${LIBSUFFIX}"   

   # check if C++ compiler can link with TIBCO Rendezvous c++ libs
   SAVE_CPPFLAGS="$CPPFLAGS"
   SAVE_LDFLAGS="$LDFLAGS"
   CPPFLAGS="$CPPFLAGS $TIBRV_CPPFLAGS"
   LDFLAGS="$LDFLAGS $TIBRV_LIBS $OPENSSL_LDFLAGS"

   # hack to work around a bug in 64-bit g++ on linux? - if libtibrvcpp is not last on the compile line the link will fail for some reason
   SAVE_LIBS="$LIBS"
   LIBS="$LIBS -ltibrvcpp${LIBSUFFIX}"

   AC_MSG_CHECKING([if we can link to the TIBCO Rendezvous C++ library])
   AC_TRY_LINK([
#include <tibrv/tibrvcpp.h>
	 ],[
Tibrv::open();
Tibrv::close();
	],tibrv_links=yes, tibrv_links=no)

   CPPFLAGS="$SAVE_CPPFLAGS"
   LDFLAGS="$SAVE_LDFLAGS"
   LIBS="$SAVE_LIBS"

   if test ${tibrv_links} = "no"; then
      AC_MSG_RESULT([failed: please recompile libtibrvcpp using this c++ compiler ($CXX) and install in $with_tibrv/lib and run configure again if you want the tibrv module])
      unset with_tibrv
      unset TIBRV_CPPFLAGS
      unset TIBRV_LIBS
   else
      AC_MSG_RESULT([ok])
      # check if secure daemon support was enabled in libtibrvcpp
      SAVE_CPPFLAGS="$CPPFLAGS"
      SAVE_LDFLAGS="$LDFLAGS"
      CPPFLAGS="$CPPFLAGS $TIBRV_CPPFLAGS"
      LDFLAGS="$LDFLAGS $TIBRV_LIBS"

      AC_MSG_CHECKING([if secure daemon support was enabled in libtibrvcpp])
      AC_TRY_COMPILE([
#include <tibrv/tibrvcpp.h>
#include <tibrv/sdcpp.h>
	 ],[
TibrvSdContext::setDaemonCert(TIBRV_SECURE_DAEMON_ANY_NAME, TIBRV_SECURE_DAEMON_ANY_CERT);
	],tibrvsd_links=yes, tibrvsd_links=no)

      CPPFLAGS="$SAVE_CPPFLAGS"
      LDFLAGS="$SAVE_LDFLAGS"

      if test "${tibrvsd_links}" = "yes"; then
         AC_MSG_RESULT([ok])
	 TIBRV_LIBS="$TIBRV_LIBS -lssl -lcrypto"
	 AC_DEFINE(TIBRV_SD, 1, Define if TIBCO Rendezvous secure daemon support should be included)
      else
         AC_MSG_RESULT([failed: if you want secure daemon support in the tibrv module, rebuild libtibrvcpp with secure daemon support (remove comments from the SD_MODULE line in $with_tibrv/src/librvcpp/Makefile), rebuild the library and install in $with_tibrv/lib and rerun configure])
      fi
      AC_SUBST(TIBRV_CPPFLAGS)
      AC_SUBST(TIBRV_LIBS)
      AC_DEFINE(TIBRV, 1, Define if TIBCO Rendezvous support should be included)
      with_tibrv=yes
   fi
fi

AC_ARG_WITH([tibae],
  [AC_HELP_STRING([--with-tibae@<:@=DIR@:>@],
		  [TIBCO Active Enterprise SDK directory (defaults to $SDK_ROOT)])],
  [if test ! -d "${with_tibae}"; then AC_MSG_ERROR(bad value ${with_tibae} for --with-tibae); unset with_tibae; fi])

AC_MSG_CHECKING([TIBCO Active Enterprise SDK])
if test -n "$with_tibae"; then
   if test ! -e "$with_tibae/include/sdk/Maverick.h"; then
      AC_MSG_ERROR([can't find Maverick.h in $with_tibae/include/sdk])
   elif test -z "`ls $with_tibae/lib/libmaverick*.${SHLIB_SUFFIX} 2>/dev/null`"; then
      AC_MSG_ERROR([can't find libmaverick*.${SHLIB_SUFFIX} in $with_tibae/lib])
   else
      AC_MSG_RESULT([$with_tibae])
   fi
else
   if test -n "$SDK_ROOT" -a -f "$SDK_ROOT/include/sdk/Maverick.h" -a -n "`ls $SDK_ROOT/lib/libmaverick*.${SHLIB_SUFFIX} 2>/dev/null`"; then
      AC_MSG_RESULT([$SDK_ROOT])
      with_tibae=$SDK_ROOT
   else
      AC_MSG_RESULT([not found])
      unset with_tibae
   fi
fi

if test -n "$with_tibae"; then
   if test -z "$with_tibrv"; then
      AC_MSG_WARN([TIBCO Active Enterprise SDK found, but no Rendezvous found, disabling tibae support])
      unset with_tibae
   else
      AC_MSG_CHECKING([TIBCO Active Enterprise SDK version])
      if test -e "$with_tibae"/prodinfo; then
	 TIBCO_SDK_VERSION=`grep SDK "$with_tibae"/prodinfo|head -1|cut -f2`
      elif test -e "$with_tibae"/version.txt; then
         TIBCO_SDK_VERSION=`grep SDK "$with_tibae"/version.txt|head -1|cut -b25- | cut -f1 -d\ `
      else
         AC_MSG_RESULT([unable to determine TIBCO Active Enterprise SDK version, disabling tibae support])
         unset with_tibae
      fi

      if test -n "$TIBCO_SDK_VERSION"; then
         AC_MSG_RESULT($TIBCO_SDK_VERSION)
      fi
      # exit this structure otherwise the nesting gets too deep :-)
   fi
fi

if test -n "$with_tibae"; then
   # check if we need TPCL (third-party libraries)
   if test `echo $TIBCO_SDK_VERSION|cut -b1` -gt 4; then
      AC_ARG_WITH([tibae-tpcl],
        [AC_HELP_STRING([--with-tibae-tpcl@<:@=DIR@:>@],
		        [TIBCO Active Enterprise TPCL directory (defaults to $TPCL_ROOT)])],
			[if test ! -d "${with_tibae_tpcl}"; then AC_MSG_ERROR(bad value ${with_tibae_tpcl} for --with-tibae-tpcl); unset with_tibae_tpcl; fi])
      AC_MSG_CHECKING([TIBCO Active Enterprise TPCL])
      if test -n "$with_tibae_tpcl"; then
	 if test ! -e "$with_tibae_tpcl/lib/libssl.a"; then
	    AC_MSG_RESULT([can't find libssl.a in $with_tibae_tpcl/lib, disabling tibae support])
	    unset with_tibae_tpcl
	    unset with_tibae
	 else
	    AC_MSG_RESULT([$with_tibae_tpcl])
	 fi
      else
	 if test -n "$TPCL_ROOT" -a -f "$TPCL_ROOT/lib/libssl.a"; then
	    AC_MSG_RESULT([$TPCL_ROOT])
	    with_tibae_tpcl=$TPCL_ROOT
	 else
	    AC_MSG_RESULT([not found, disabling tibae support])
	    unset with_tibae_tpcl
	    unset with_tibae
	 fi
      fi
   fi
fi

if test -n "$with_tibae"; then
   case "$TIBCO_SDK_VERSION" in
	4.1.2) TIBAE_LIBS="-L${with_tibae}/lib -lmaverick41 -ldl -lpthread"
	       TIBAE_CPPFLAGS="-I${with_tibae}/include"
	       AC_DEFINE(TIBCO_SDK, 4, [define for TIBCO SDK 4.*])
	       ;;
	4.1.3) TIBAE_LIBS="-L${with_tibae}/lib -lmaverick41 -ldl -lpthread"
	       TIBAE_CPPFLAGS="-DLINUX24 -I${with_tibae}/include"
	       AC_DEFINE(TIBCO_SDK, 4, [define for TIBCO SDK 4.*])
	       ;;
        5.0.*) TIBAE_LIBS="-L${with_tibae}/lib -L${with_tibae_tpcl}/lib -lmaverick50 -lrepowww532 -lxerces-c2_1 -lssl -lcrypto -ldl -lpthread"
	       TIBAE_CPPFLAGS="-DLINUX24 -I${with_tibae}/include/sdk"
	       AC_DEFINE(TIBCO_SDK, 5, [define for TIBCO SDK 5.*])
	       ;;
        5.3.*) TIBAE_LIBS="-L${with_tibae}/lib -L${with_tibae_tpcl}/lib -lmaverick53 -lrepowww532 -lxerces-c2_1 -lssl -lcrypto -licuuc -ldl -lpthread"
	       TIBAE_CPPFLAGS="-DLINUX24 -I${with_tibae}/include/sdk"
	       AC_DEFINE(TIBCO_SDK, 5, [define for TIBCO SDK 5.*])
	       ;;
	*)     AC_MSG_ERROR(Unsupported TIBCO SDK Version) 
	       ;;
   esac

   # check if C++ compiler can link with TIBCO libs
   SAVE_CPPFLAGS="$CPPFLAGS"
   SAVE_LDFLAGS="$LDFLAGS"
   CPPFLAGS="$CPPFLAGS $TIBAE_CPPFLAGS $TIBRV_CPPFLAGS"
   LDFLAGS="$LDFLAGS $TIBAE_LIBS $TIBRV_LIBS"
   AC_MSG_CHECKING([if we can link to the TIBCO AE SDK])
   AC_TRY_LINK([
#include <Maverick.h>
class TApp : public MApp
{
      virtual void onTermination() throw (MException) {}
      virtual void onInitialization() throw (MException) {}
   public:
      inline TApp(MAppProperties *pm) : MApp(pm) {}
};

	 ],[
TApp ma(NULL);
	],tibae_sdk_links=yes, tibae_sdk_links=no)

   CPPFLAGS="$SAVE_CPPFLAGS"
   LDFLAGS="$SAVE_LDFLAGS"

   if test "$tibae_sdk_links" = "no"; then
      AC_MSG_RESULT([failed with this C++ compiler, try CXX=<comp> configure, disabling tibae support])
      unset with_tibae
   else
      AC_MSG_RESULT([ok])
      AC_SUBST(TIBAE_LIBS)
      AC_SUBST(TIBAE_CPPFLAGS)
      AC_DEFINE(TIBAE, 1, [Define if TIBCO Active Enterprise support should be included])
      with_tibae=yes
   fi
fi

AC_ARG_WITH([oracle],
  [AC_HELP_STRING([--with-oracle@<:@=DIR@:>@],
                  [directory for Oracle support (defaults to $ORACLE_HOME)])],
   , 
   [with_oracle=auto])

if test "$with_oracle" = "auto"; then
   if test -n "$ORACLE_HOME"; then
      with_oracle=yes
   else
      with_oracle=no
   fi
fi

case "$with_oracle" in
  yes) ORACLE_DIR=$ORACLE_HOME ;;
  no) ;;
  *) ORACLE_DIR=$with_oracle; with_oracle=yes ;;
esac

if test "$with_oracle" = "yes"; then
   if test -d "$ORACLE_DIR/rdbms/public"; then
      ORACLE_INCLUDES="-I$ORACLE_DIR/rdbms/public"
   fi
   if test -d "$ORACLE_DIR/rdbms/demo"; then
      ORACLE_INCLUDES="$ORACLE_INCLUDES -I$ORACLE_DIR/rdbms/demo"
   fi
   if test -d "$ORACLE_DIR/network/public"; then
      ORACLE_INCLUDES="$ORACLE_INCLUDES -I$ORACLE_DIR/network/public"
   fi
   if test -d "$ORACLE_DIR/plsql/public"; then
      ORACLE_INCLUDES="$ORACLE_INCLUDES -I$ORACLE_DIR/plsql/public"
   fi
   AC_SUBST(ORACLE_INCLUDES)

   AC_ORACLE_VERSION
   case "$ORACLE_VERSION" in
      9*|10*) ORACLE_LIBS="-L$ORACLE_DIR/lib -lclntsh" ;; 
      *)      AC_MSG_ERROR(Unsupported Oracle version) ;;
   esac
   AC_SUBST(ORACLE_LIBS)

   # check for 'oci.h'
   SAVE_CXXFLAGS="$CXXFLAGS"
   SAVE_LDFLAGS="$LDFLAGS"
   CXXFLAGS="$CXXFLAGS $ORACLE_INCLUDES"
   LDFLAGS="$LDFLAGS $ORACLE_LIBS"
   AC_TRY_COMPILE([
#include <oci.h>
         ],[
        ], with_oracle=yes, AC_MSG_ERROR(file 'oci.h' not found in '$ORACLE_INCLUDES'))
   CXXFLAGS="$SAVE_CXXFLAGS"
   LDFLAGS="$SAVE_LDFLAGS"
fi

AC_ARG_ENABLE([mysql],
     [AC_HELP_STRING([--enable-mysql],
		     [enable mysql support (default=auto)])],
     [case "${enable_mysql}" in
       yes|no|auto) ;;
       *)      AC_MSG_ERROR(bad value ${enable_mysql} for --enable-mysql) ;;
      esac],
     [enable_mysql=auto])

# see if we can figure out where the mysql files are
if test "${enable_mysql}" != "no"; then
   AC_MSG_CHECKING([for mysql files])
   if test -d /usr/local/mysql; then
      AC_MSG_RESULT([/usr/local/mysql])
      with_mysql_includes=/usr/local/mysql/include
      with_mysql_libs=/usr/local/mysql/lib${LIBSUFFIX}
      enable_mysql=yes
   elif test -d /opt/mysql; then
      AC_MSG_RESULT([/opt/mysql])
      with_mysql_includes=/opt/mysql/include
      with_mysql_libs=/opt/mysql/lib${LIBSUFFIX}
      enable_mysql=yes
   elif test -d /usr/mysql; then
      AC_MSG_RESULT([/usr/mysql])
      with_mysql_includes=/usr/mysql/include
      with_mysql_libs=/usr/mysql/lib${LIBSUFFIX}
      enable_mysql=yes
   elif test -d /usr/include/mysql -a -d /usr/lib${LIBSUFFIX}/mysql; then
      AC_MSG_RESULT([/usr/include/mysql and /usr/lib${LIBSUFFIX}/mysql])
      with_mysql_includes=/usr/include/mysql
      with_mysql_libs=/usr/lib${LIBSUFFIX}/mysql
      enable_mysql=yes
   elif test -d /sw/include/mysql -a -d /sw/lib/mysql; then
      AC_MSG_RESULT([/sw/include/mysql and /sw/lib/mysql])
      with_mysql_includes=/sw/include/mysql
      with_mysql_libs=/sw/lib/mysql
      enable_mysql=yes
   elif test -d /opt/local/include/mysql -a -d /opt/local/lib${LIBSUFFIX}/mysql; then
      AC_MSG_RESULT([/opt/local/include/mysql and /opt/local/lib${LIBSUFFIX}/mysql])
      with_mysql_includes=/opt/local/include/mysql
      with_mysql_libs=/opt/local/lib${LIBSUFFIX}/mysql
      enable_mysql=yes
   else
      AC_MSG_RESULT([no])
      enable_mysql=no
   fi
fi

if test "${enable_mysql}" = yes; then
  AC_ARG_WITH([mysql-includes],
    [AC_HELP_STRING([--with-mysql-includes@<:@=DIR@:>@],
                    [MySQL include file directory])],
    [if test ! -d "${with_mysql_includes}"; then AC_MSG_ERROR(bad value ${with_mysql_includes} for --with-mysql-includes); unset with_mysql_includes; fi],
    [with_mysql_includes=/usr/include/mysql])

  AC_ARG_WITH([mysql-libs],
    [AC_HELP_STRING([--with-mysql-libs@<:@=DIR@:>@],
                    [directory for MySQL libraries (default: /usr/lib/mysql)])],
    [if test ! -d "${with_mysql_libs}"; then AC_MSG_ERROR(bad value ${with_mysql_libs} for --with-mysql-libs); unset with_mysql_libs; fi],
    [with_mysql_libs=/usr/lib/mysql])

  if test -n "$with_mysql_includes" -a -n "$with_mysql_libs"; then
     MYSQL_INCLUDES="-I$with_mysql_includes"
     MYSQL_LIBS="-L$with_mysql_libs -lmysqlclient_r"
     # check if C++ compiler can link with MYSQL libs
     SAVE_CXXFLAGS="$CXXFLAGS"
     SAVE_LDFLAGS="$LDFLAGS"
     CXXFLAGS="$CXXFLAGS $MYSQL_INCLUDES"
     LDFLAGS="$LDFLAGS $MYSQL_LIBS"   
     AC_TRY_COMPILE([
#include <mysql.h>
	 ],[
MYSQL mysql; mysql_init(&mysql);
	],mysql_links=yes, mysql_links=no)

     CXXFLAGS="$SAVE_CXXFLAGS"
     LDFLAGS="$SAVE_LDFLAGS"

     if test "$mysql_links" = "no"; then
        AC_MSG_ERROR([could not compile a test MySQL program])
     else
        AC_SUBST(MYSQL_INCLUDES)
	AC_SUBST(MYSQL_LIBS)
	# check for mysql_stmt_prepare
	if test -n "`grep mysql_stmt_prepare ${with_mysql_includes}/*.h`"; then
	   AC_DEFINE(HAVE_MYSQL_STMT, 1, Define if mysql_stmt_* functions are available)
	fi
	# check for mysql_commit
	if test -n "`grep mysql_commit ${with_mysql_includes}/*.h`"; then
	   AC_DEFINE(HAVE_MYSQL_COMMIT, 1, [Define if mysql_commit() and mysql_rollback() functions are available])
	fi
	# check for mysql_set_character_set
	if test -n "`grep mysql_set_character_set ${with_mysql_includes}/*.h`"; then
	   AC_DEFINE(HAVE_MYSQL_SET_CHARACTER_SET, 1, [Define if mysql_set_character_set() is available])
	fi
        with_mysql=yes
     fi
  elif test -n "$with_mysql_includes"; then
     AC_MSG_WARN([MySQL includes found, but libraries not, disabling MySQL support])
  elif test -n "$with_mysql_libs"; then
     AC_MSG_WARN([MySQL libraries found, but not includes, disabling MySQL support])
  else
     AC_MSG_WARN([no MySQL libraries or includes found, disabling MySQL support])
  fi
fi

AC_ARG_ENABLE([ncurses],
  [AC_HELP_STRING([--disable-ncurses],
                  [do not include ncurses support])],
  [case "$enable_ncurses" in
      yes|no) ;;
      *)      AC_MSG_ERROR([bad value ${enable_ncurses} for ncurses option]) ;;
   esac],
   [enable_ncurses=yes])

if test "${enable_ncurses}" = yes; then
   enable_ncurses=no
   PKG_CHECK_MODULES(NCURSES, ncurses,,[
      AC_MSG_CHECKING([trying to find ncurses])
         AC_CHECK_LIB(ncurses, initscr, enable_ncurses=yes)
	 AC_CHECK_LIB(panel, update_panels, enable_panel=yes)
	 AC_CHECK_LIB(ncurses, wresize, AC_DEFINE(HAVE_WRESIZE, 1, [define if the wresize function is available]))
	 ])
fi

if test "${enable_ncurses}" = yes; then
   if test "${enable_panel}" = yes; then
      AC_DEFINE(NCURSES, 1, Define if ncurses support should be included)
   fi
fi

PKG_CHECK_MODULES(XML, libxml-2.0 >= 2.4.22,,[
  AC_MSG_CHECKING([trying xml2-config])
    if xml2-config --libs > /dev/null 2>&1; then
      full=`xml2-config --version`
      vers=`xml2-config --version | \
        awk 'BEGIN { FS = "."; } {print ($1 * 1000 + $2) * 1000 + $3;}'`
      if test "$vers" -ge 2004022; then
	AC_MSG_RESULT($full)
	XML_CFLAGS="`xml2-config --cflags`"
	XML_LIBS="`xml2-config --libs`"
      else
	AC_MSG_ERROR([libxml2 2.4.22 or higher is needed; found version $full])
      fi
    else
      AC_MSG_ERROR([libxml2 2.4.22 or higher is needed; xml2-config not found])      
    fi])

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h inttypes.h netdb.h netinet/in.h stddef.h stdlib.h string.h strings.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_CHECK_MEMBERS([struct stat.st_blksize])
AC_STRUCT_ST_BLOCKS
AC_CHECK_MEMBERS([struct stat.st_rdev])
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_VOLATILE

# checks for an STL hash_map template
AC_CXX_MAKE_HASH_MAP_H([include/qore/hash_map_include.h])

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_LSTAT
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
##AC_FUNC_MALLOC
AC_FUNC_MKTIME
##AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_STRERROR_R
AC_FUNC_STRTOD
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([bzero floor gethostbyaddr gethostbyaddr_r gethostbyname gethostbyname_r gethostname gettimeofday memmove memset mkfifo putenv regcomp select socket setsockopt getsockopt strcasecmp strchr strdup strerror strspn strstr atoll strtol strtoll isblank localtime_r gmtime_r exp2 clock_gettime realloc timegm])

# try to find strtoimax on HPUX
AC_MSG_CHECKING([for strtoimax])
AC_EGREP_CPP([inttypes_h_defines_strtoimax], [
#include <inttypes.h>
#ifdef strtoimax
inttypes_h_defines_strtoimax
#endif
], [AC_MSG_RESULT(yes)
    AC_DEFINE(HAVE_STRTOIMAX, 1,
	     [Define to 1 if you have strtoimax])], 
   [AC_MSG_RESULT(no)])

# Check definition of gethostbyaddr_r (glibc2 defines this with 8 arguments)
ac_save_CXXFLAGS="$CXXFLAGS"
AC_CACHE_CHECK([style of gethost* routines], qore_gethost_style,
AC_LANG_SAVE
AC_LANG_CPLUSPLUS

# Do not treat warnings as errors if we are linking against other libc
# this is to work around gcc not being permissive on non-system includes
# with respect to ANSI C++
# We also remove the -fbranch-probabilities option as this will give warnings
# about not profiled code, which confuses configure
if test "$ac_cv_prog_gxx" = "yes" -a "$with_other_libc" = "no"
then
  CXXFLAGS=`echo "$CXXFLAGS -Werror" | sed 's/-fbranch-probabilities//'`
fi

AC_TRY_COMPILE(
[#undef inline
#if !defined(SCO) && !defined(__osf__) && !defined(_REENTRANT)
#define _REENTRANT
#endif
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>],
[int skr;
 struct hostent *foo = gethostbyaddr_r((const char *) 0,
  0, 0, (struct hostent *) 0, (char *) NULL,  0, &skr); return (foo == 0);],
qore_gethost_style=solaris, qore_gethost_style=other))
AC_LANG_RESTORE
CXXFLAGS="$ac_save_CXXFLAGS"
if test "$qore_gethost_style" = "solaris"
then
  AC_DEFINE([HAVE_SOLARIS_STYLE_GETHOST], [1],
            [Solaris defines gethostbyaddr_r with 7 arguments. glibc2 defines this with 8 arguments])
fi

# Check definition of gethostbyname_r (glibc2.0.100 is different from Solaris)
ac_save_CXXFLAGS="$CXXFLAGS"
AC_CACHE_CHECK([style of gethostname_r routines], qore_gethostname_style,
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
if test "$ac_cv_prog_gxx" = "yes" -a "$with_other_libc" = "no"
then
  CXXFLAGS=`echo "$CXXFLAGS -Werror" | sed 's/-fbranch-probabilities//'`
fi
AC_TRY_COMPILE(
[#undef inline
#if !defined(SCO) && !defined(__osf__) && !defined(_REENTRANT)
#define _REENTRANT
#endif
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>],
[int skr;

 skr = gethostbyname_r((const char *) 0,
  (struct hostent*) 0, (char*) 0, 0, (struct hostent **) 0, &skr);],
qore_gethostname_style=glibc2, qore_gethostname_style=other))
AC_LANG_RESTORE
CXXFLAGS="$ac_save_CXXFLAGS"
if test "$qore_gethostname_style" = "glibc2"
then
  AC_DEFINE([HAVE_GETHOSTBYNAME_R_GLIBC2_STYLE], [1],
            [Solaris define gethostbyname_r with 5 arguments. glibc2 defines this with 6 arguments])
fi

# Check 3rd argument of getthostbyname_r
ac_save_CXXFLAGS="$CXXFLAGS"
AC_CACHE_CHECK([3 argument to gethostname_r routines], qore_gethostname_arg,
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
if test "$ac_cv_prog_gxx" = "yes" -a "$with_other_libc" = "no"
then
  CXXFLAGS=`echo "$CXXFLAGS -Werror" | sed 's/-fbranch-probabilities//'`
fi
AC_TRY_COMPILE(
[#undef inline
#if !defined(SCO) && !defined(__osf__) && !defined(_REENTRANT)
#define _REENTRANT
#endif
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>],
[int skr;
 skr = gethostbyname_r((const char *) 0, (struct hostent*) 0, (struct hostent_data*) 0);],
qore_gethostname_arg=hostent_data, qore_gethostname_arg=char))
AC_LANG_RESTORE
CXXFLAGS="$ac_save_CXXFLAGS"
if test "$qore_gethostname_arg" = "hostent_data"
then
  AC_DEFINE([HAVE_GETHOSTBYNAME_R_RETURN_INT], [1],
            [In OSF 4.0f the 3'd argument to gethostname_r is hostent_data *])
fi

# check for DB driver support
if test "$with_oracle" = "yes"; then
   AC_DEFINE(ORACLE, 1, Define to 1 if Oracle support should be included)
fi
if test "$with_mysql" = "yes"; then
   AC_DEFINE(QORE_MYSQL, 1, Define to 1 if MySQL support should be included)
fi

# turn on some build flags for non-debugging, non-profiling builds
if test "$enable_debug" = "no" -a "$enable_profile" = "no"; then
   # turn on optimizations if we know how
   if test "$ac_cv_prog_gxx" = "yes"; then
      CXXFLAGS="$CXXFLAGS -O3"  # gcc
   elif test "$CXX" = "CC" -a "`echo $host_os|cut -b-7`" = "solaris"; then
      CXXFLAGS="$CXXFLAGS -xO5" # Solaris CC
   fi
   # strip binaries and libraries
   if test "`echo $host_os|cut -b-6`" = "darwin"; then
      LDFLAGS="$LDFLAGS -Wl,-x"
   else
      LDFLAGS="$LDFLAGS -s"
   fi	
fi

# set special compiler flags for SunPro C++ compiler to turn off annoying "String literal converted to char* ..." warnings
if test "$CXX" = "CC" -a "`echo $host_os|cut -b-7`" = "solaris"; then
   CXXFLAGS="$CXXFLAGS -erroff=badargtypel2w"
fi

# turn on warnings if we know how
if test "$ac_cv_prog_gxx" = "yes"; then
   CXXFLAGS="$CXXFLAGS -Wall"  # gcc
fi

# set flag for monolithic builds if necessary
if test "$enable_shared" = "no" -o "$enable_builtin_modules" = "yes"; then
   AC_DEFINE(QORE_MONOLITHIC, 1, Define to 1 if all modules should be statically linked into the libqore.a library)
fi

# save more version information in config.h
VERSION_MAJOR=`echo $PACKAGE_VERSION | cut -f3 -d\  | sed s/\"//g | cut -f1 -d.`
VERSION_MINOR=`echo $PACKAGE_VERSION | cut -f3 -d\  | sed s/\"//g | cut -f2 -d.`
VERSION_SUB=`echo $PACKAGE_VERSION | cut -f3 -d\  | sed s/\"//g | cut -f3 -d.`

# generate next build number
AC_DEFINE_UNQUOTED([VERSION_MAJOR], $VERSION_MAJOR, [major version number])
AC_DEFINE_UNQUOTED([VERSION_MINOR], $VERSION_MINOR, [minor version number])
AC_DEFINE_UNQUOTED([VERSION_SUB], $VERSION_SUB, [sub version number])
AC_DEFINE_UNQUOTED([TARGET_ARCH], "`uname -m`", [host type])
AC_DEFINE_UNQUOTED([TARGET_OS], "`uname -s`", [host type])
AC_DEFINE_UNQUOTED([BUILD], `./next_build.sh`, [build number])

VERSION_STRING=$PACKAGE_VERSION-`cat .build`
AC_DEFINE_UNQUOTED([VERSION_STRING], "$VERSION_STRING", [qore version string])

AM_CONDITIONAL([COND_NCURSES],    [test "$enable_ncurses" = yes])
AM_CONDITIONAL([COND_DEBUG],      [test "$enable_debug" = yes])
AM_CONDITIONAL([COND_PROFILE],    [test "$enable_profile" = yes])
AM_CONDITIONAL([COND_ORACLE],     [test "$with_oracle" = yes])
AM_CONDITIONAL([COND_MYSQL],      [test "$with_mysql" = yes])
AM_CONDITIONAL([COND_TIBRV],      [test "$with_tibrv" = yes])
AM_CONDITIONAL([COND_TIBAE],      [test "$with_tibae" = yes])
AM_CONDITIONAL([COND_TUXEDO],     [test "$with_tuxedo"=yes])
AM_CONDITIONAL([COND_MONOLITHIC], [test "$enable_shared" = no -o "$enable_builtin_modules" = "yes"])

AC_CONFIG_FILES([Makefile lib/Makefile modules/Makefile modules/ncurses/Makefile modules/TIBCO/Makefile modules/mysql/Makefile modules/oracle/Makefile modules/Tuxedo/Makefile])
AC_OUTPUT

echo "*** BUILTIN FEATURES ***"
printf "debug    : "
if test "$enable_debug" = "yes"; then
   echo yes
else
   echo disabled
fi

printf "profile  : "
if test "$enable_profile" = "yes"; then
   echo yes
else
   echo disabled
fi

if test "$enable_shared" = "yes"; then
   echo "*** MODULES ***"
fi

printf "ncurses  : "
if test "$enable_ncurses" = "yes"; then
   echo yes
else
   echo disabled
fi

printf "oracle   : "
if test "$with_oracle" = "yes"; then
   echo yes
else
   echo disabled
fi

printf "mysql    : "
if test "$with_mysql" = "yes"; then
   echo yes
else
   echo disabled
fi

printf "tibrv    : "
if test "$with_tibrv" = "yes"; then
   echo yes
else
   echo disabled
fi

printf "tibae    : "
if test "$with_tibae" = "yes"; then
   echo yes
else
   echo disabled
fi
