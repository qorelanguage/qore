# Process this file with autoconf to produce a configure script.

# AC_PREREQ(2.59)
AC_INIT([qore], [0.6.0],
        [David Nichols <nicholsman@gmail.com>],
        [qore])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADER(include/qore/unix-config.h)

AC_PROG_LEX
##AC_PROG_MAKE_SET
AC_C_BIGENDIAN

# test for flex & flex version
AC_MSG_CHECKING([for flex 2.5.31 or greater])
if test -z "`echo $LEX | grep flex`"; then
        AC_MSG_ERROR([no flex version found])
elif test -z "`$LEX --version | grep -e ' 2\.5\.3[[1-9]]' -e '2\.5\.[[4-9]][[0-9]]' -e '2\.\[[6-9]]\.'`" ; then
        flexver=`$LEX --version`
	AC_MSG_ERROR([incompatible $flexver found - you can download flex 2.5.33 at http://sourceforge.net/projects/flex])
else
        flexver=`$LEX --version`
	AC_MSG_RESULT([yes ($flexver)])
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

# set system variables
OS=`uname -s`
if test "$OS" = "HP-UX"; then
   if "$host_cpu" = "ia64"; then
      ARCH=itanium
   else
      ARCH=PA-RISC
   fi
else
   ARCH=`uname -p`
   if test $ARCH = i686 -o $ARCH = i586 -o $ARCH = athlon ; then
      ARCH=i386
   fi
fi

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
		LDFLAGS="$LDFLAGS -lnsl"
		# assume we are using CC if not using g++, add -norunpath to link flags in this case
		if test "$GXX" != "yes"; then
		   LDFLAGS="$LDFLAGS -norunpath"
		fi
		AC_DEFINE(SOLARIS, 1, if compiling on Solaris)
	        ;;
     *darwin*)  SHLIB_SUFFIX=dylib
		# libtool 1.5.* is creating modules on Darwin 8.* (OS/X 10.4.*) with a .so suffix for some reason
		MODULE_SUFFIX=so
		AC_DEFINE(DARWIN, 1, if compiling on Darwin)
		;;
     *hpux*)    if test "$host_cpu" = "ia64"; then
     		   SHLIB_SUFFIX=so
		else  # assume hppa (PA-RISC)
     		   SHLIB_SUFFIX=sl
		   AC_DEFINE(SHLIB_SUFFIX, "sl", shared library suffix)
		fi
		# add "-AA" and -D__STDC_EXT__ to aCC commandline to enable a clean compile
		if test -n "`echo $CXX|grep aCC`"; then
		   CXXFLAGS="$CXXFLAGS -AA -D__STDC_EXT__"
		   AC_DEFINE(HPUX_ACC_SOCKLEN_HACK, 1, if compiling on HP-UX with aCC and need to use int instead of socklen_t)
		fi
		AC_DEFINE(HPUX, 1, if compiling on HP-UX)
		;;
esac
AC_SUBST(SHLIB_SUFFIX)
if test -z "$MODULE_SUFFIX" ; then
   MODULE_SUFFIX=$SHLIB_SUFFIX
fi
AC_SUBST(MODULE_SUFFIX)

AC_ARG_ENABLE([single-compilation-unit],
  [AS_HELP_STRING([--enable-single-compilation-unit],
		  [enable compilation as a single unit (default: on)])],
  [case "${enable_single_compilation_unit}" in
       yes|no) ;;
       *)      AC_MSG_ERROR(bad value ${enable_single_compilation_unit} for --enable-single-compilation-unit) ;;
      esac],
  [enable_single_compilation_unit=yes])

AC_ARG_ENABLE([64bit],
  [AS_HELP_STRING([--enable-64bit],
		  [enable 64bit support (default: off)])],
  [case "${enable_64bit}" in
       yes|no) ;;
       *)      AC_MSG_ERROR(bad value ${enable_64bit} for --enable-64bit) ;;
      esac],
  [enable_64bit=no])

# turn on 64-bit by default if compiling on itanium
if test "$host_cpu" = "ia64"; then
   enable_64bit=yes
fi

# see if we know how to set flags properly for different hosts and architectures
# FIXME: add test on HP-UX if the compiler can generate the appropriate binaries
if test "$enable_64bit" = "yes"; then
   case "${host_os}" in
      *linux*)	if test "$GXX" = "yes"; then
		   CXXFLAGS="$CXXFLAGS -m64"
		fi
		LIBSUFFIX=64
		AC_SUBST(LIBSUFFIX)
		;;
      *hpux*)	if test "$host_cpu" != "ia64"; then
		    if test -n "`echo $CXX|grep aCC`"; then
		        CXXFLAGS="$CXXFLAGS +DA2.0W"
		    elif test "$GXX" = "yes"; then
			CXXFLAGS="$CXXFLAGS -march=2.0"
		    fi
		fi
		;;
     *solaris*) if test "$GXX" = "yes"; then
		    if test "$host_cpu" = "sparc"; then
		        CXXFLAGS="$CXXFLAGS -mcpu=ultrasparc3 -m64"  # NOTE we compile for minimum ultrasparc 3 in 64-bit mode
		    else
		        CXXFLAGS="$CXXFLAGS -m64"
		    fi
		else
		    CXXFLAGS="$CXXFLAGS -xarch=generic64"
		fi
		;;
   esac
   RVLIBSUFFIX=64
   bits=64
else
   case "${host_os}" in
     *linux*)	if test "$GXX" = "yes"; then
		   CXXFLAGS="$CXXFLAGS -m32"
		fi
		;;
     *hpux*)	if test "$host_cpu" != "ia64"; then
		    if test "$GXX" = "yes"; then
		        CXXFLAGS="$CXXFLAGS -march=2.0"
		    elif test -n "`echo $CXX|grep aCC`" -a "$host_cpu" != "ia64"; then
		        CXXFLAGS="$CXXFLAGS +DA2.0N"   # NOTE we compile for PA-RISC 2.0 32-bit, not PA-RISC 1.1
		    fi
		fi
		;;
     *solaris*) if test "$GXX" = "yes"; then
		    if test "$host_cpu" = "sparc"; then
		        CXXFLAGS="$CXXFLAGS -mcpu=v8 -m32"
		    else
		        CXXFLAGS="$CXXFLAGS -m32"
		    fi
		else
		    CXXFLAGS="$CXXFLAGS -xarch=generic"
		fi
		;;
   esac
   bits=32
fi

AC_ARG_ENABLE([static-openssl],
     [AS_HELP_STRING([--enable-static-openssl],
		     [force static linkage of openssl libraries (default=auto)])],
     [case "${enable_static_openssl}" in
       yes|no) ;;
       *)      AC_MSG_ERROR(bad value ${enable_static_openssl} for --enable-static-openssl) ;;
      esac],
     [enable_static_openssl=no])

AC_ARG_ENABLE([static-libxml2],
     [AS_HELP_STRING([--enable-static-libxml2],
		     [force static linkage of libxml2 libraries (default=auto)])],
     [case "${enable_static_libxml2}" in
       yes|no) ;;
       *)      AC_MSG_ERROR(bad value ${enable_static_libxml2} for --enable-static-libxml2) ;;
      esac],
     [enable_static_libxml2=no])

AC_ARG_ENABLE([static-pcre],
     [AS_HELP_STRING([--enable-static-pcre],
		     [force static linkage of pcre libraries (default=auto)])],
     [case "${enable_static_pcre}" in
       yes|no) ;;
       *)      AC_MSG_ERROR(bad value ${enable_static_pcre} for --enable-static-pcre) ;;
      esac],
     [enable_static_pcre=no])

AC_ARG_ENABLE([static-ncurses],
     [AS_HELP_STRING([--enable-static-ncurses],
		     [force static linkage of ncurses libraries (default=auto)])],
     [case "${enable_static_ncurses}" in
       yes|no) ;;
       *)      AC_MSG_ERROR(bad value ${enable_static_ncurses} for --enable-static-ncurses) ;;
      esac],
     [enable_static_ncurses=no])

AC_ARG_ENABLE([static-common],
     [AS_HELP_STRING([--enable-static-common],
		     [force static linkage of openssl, libxml2, pcre, and ncurses libraries (default=auto)])],
     [case "${enable_static_common}" in
       yes|no) ;;
       *)      AC_MSG_ERROR(bad value ${enable_static_common} for --enable-static-common) ;;
      esac],
     [enable_static_common=no])

if test "$enable_static_openssl" = "yes"; then
   openssl_static=yes
fi

if test "$enable_static_libxml2" = "yes"; then
   libxml2_static=yes
fi

if test "$enable_static_pcre" = "yes"; then
   pcre_static=yes
fi

if test "$enable_static_ncurses" = "yes"; then
   ncurses_static=yes
fi

if test "$enable_static_common" = "yes"; then
   openssl_static=yes
   libxml2_static=yes
   pcre_static=yes
   ncurses_static=yes
fi

AC_ARG_WITH([static-libz-dir],
	    [AS_HELP_STRING([--with-static-libz-dir], [directory for libz.a library to force static linkage (default=auto)])],
	    [if test ! -d "${with_static_libz_dir}"; then AC_MSG_ERROR([bad value ${with_static_libz_dir} for --with-static_libz_dir]); fi])

if test -n "$with_static_libz_dir"; then
    if ! test -f "$with_static_libz_dir/libz.a"; then
        AC_MSG_ERROR([no libz.a found in $with_static_libz_dir])
    fi
    ZLIB_LDFLAGS="$with_static_libz_dir/libz.a"
else
    ZLIB_LDFLAGS="-lz"
fi
AC_SUBST(ZLIB_LDFLAGS)

set_openssl_cppflags()
{
    if test "$1" != "/usr/include"; then
        OPENSSL_CPPFLAGS=-I$1
    fi
}

find_openssl()
{
    lib=$1/lib${LIBSUFFIX}
    # if we only want the static openssl libraries
    if test -n "$openssl_static"; then
    	if test -f "$lib/libssl.a" -a -f "$lib/libcrypto.a"; then
	    OPENSSL_LDFLAGS="$lib/libssl.a $lib/libcrypto.a"
       	    AC_MSG_RESULT([$lib (forced static)])
    	fi
    else
        if test -f "$lib/libssl.${SHLIB_SUFFIX}" -a -f "$lib/libcrypto.${SHLIB_SUFFIX}"; then
	    if test "$lib" != "/lib" -a "$lib" != "/usr/lib"; then
       	        OPENSSL_LDFLAGS="-L$lib"
    	    fi
	    OPENSSL_LDFLAGS="$OPENSSL_LDFLAGS -lssl -lcrypto"
       	    AC_MSG_RESULT([$lib (shared)])
    	elif test -f "$lib/libssl.a" -a -f "$lib/libcrypto.a"; then
	    if test "$lib" != "/lib" -a "$lib" != "/usr/lib"; then
       	        OPENSSL_LDFLAGS="-L$lib"
    	    fi
	    OPENSSL_LDFLAGS="$OPENSSL_LDFLAGS -lssl -lcrypto"
       	    AC_MSG_RESULT([$lib (static)])
    	fi
    fi
    if test -z "$OPENSSL_LDFLAGS"; then
        return
    fi
    AC_MSG_CHECKING([for openssl include files])
    # try to find include files
    if test "$1" = "/"; then
        inc=/usr/include
    else
        inc=$1/include
    fi
    if test -f "$inc/openssl/ssl.h"; then
        AC_MSG_RESULT([found: $inc/openssl])
        set_openssl_cppflags $inc
    else
	AC_MSG_ERROR([not found])
    fi
    AC_SUBST(OPENSSL_LDFLAGS)
    AC_SUBST(OPENSSL_CPPFLAGS)   
}

# see if we can figure out where the openssl library is
AC_ARG_WITH([openssl-dir],
  [AS_HELP_STRING([--with-openssl-dir@<:@=DIR@:>@],
                  [openssl directory])],
  [if test ! -d "${with_openssl_dir}"; then AC_MSG_ERROR(bad value ${with_openssl_dir} for --with-openssl-dir); unset with_openssl_dir; fi], 
  [with_openssl_dir="$OPENSSL_DIR"])

AC_MSG_CHECKING([for openssl libraries and header files])
for dir in "${with_openssl_dir}" /usr / /usr/local /opt/gnu /opt/openssl /usr/local/openssl /opt/local /sw /usr/sfw /opt/sfw; do
    find_openssl $dir
    if test -n "$OPENSSL_LDFLAGS"; then
        break
    fi
done
if test -z "$OPENSSL_LDFLAGS"; then
    AC_MSG_ERROR([no openssl library found])
fi

set_pcre_cppflags()
{
    if test "$1" != "/usr/include"; then
        PCRE_CPPFLAGS=-I$1
    fi
}

find_pcre()
{
    lib=$1/lib${LIBSUFFIX}
    # if we only want the static pcre libraries
    if test -n "$pcre_static"; then
    	if test -f "$lib/libpcre.a"; then
	    PCRE_LDFLAGS="$lib/libpcre.a"
       	    AC_MSG_RESULT([$lib (forced static)])
    	fi
    else
        if test -f "$lib/libpcre.${SHLIB_SUFFIX}"; then
	    if test "$lib" != "/lib" -a "$lib" != "/usr/lib"; then
       	        PCRE_LDFLAGS="-L$lib"
    	    fi
	    PCRE_LDFLAGS="$PCRE_LDFLAGS -lpcre"
       	    AC_MSG_RESULT([$lib (shared)])
    	elif test -f "$lib/libpcre.a"; then
	    if test "$lib" != "/lib" -a "$lib" != "/usr/lib"; then
       	        PCRE_LDFLAGS="-L$lib"
    	    fi
	    PCRE_LDFLAGS="$PCRE_LDFLAGS -lpcre"
       	    AC_MSG_RESULT([$lib (static)])
    	fi
    fi
    if test -z "$PCRE_LDFLAGS"; then
        return
    fi
    AC_MSG_CHECKING([for pcre include files])
    # try to find include files
    if test "$1" = "/"; then
        inc=/usr/include
    else
        inc=$1/include
    fi
    if test -f "$inc/pcre.h"; then
        AC_MSG_RESULT([found: $inc])
        set_pcre_cppflags $inc
    elif test -f "$inc/pcre/pcre.h"; then
        AC_MSG_RESULT([found: $inc/pcre])
        set_pcre_cppflags $inc/pcre
    else
	AC_MSG_ERROR([not found])
    fi
    AC_SUBST(PCRE_LDFLAGS)
    AC_SUBST(PCRE_CPPFLAGS)   
}

# see if we can figure out where the pcre library is
AC_ARG_WITH([pcre-dir],
  [AS_HELP_STRING([--with-pcre-dir@<:@=DIR@:>@],
                  [PCRE directory])],
  [if test ! -d "${with_pcre_dir}"; then AC_MSG_ERROR(bad value ${with_pcre_dir} for --with-pcre-dir); unset with_pcre_dir; fi],
  [with_pcre_dir="$PCRE_DIR"])

AC_MSG_CHECKING([for pcre libraries and header files])
for dir in "${with_pcre_dir}" /usr / /usr/local /opt/gnu /opt/pcre /usr/local/pcre /opt/local /sw /usr/sfw /opt/sfw; do
    find_pcre $dir
    if test -n "$PCRE_LDFLAGS"; then
        break
    fi
done
if test -z "$PCRE_LDFLAGS"; then
    AC_MSG_ERROR([no pcre library found])
fi

set_libxml2_cppflags()
{
    if test "$1" != "/usr/include"; then
        LIBXML2_CPPFLAGS=-I$1
    fi
}

find_libxml2()
{
    lib=$1/lib${LIBSUFFIX}
    # if we only want the static libxml2 libraries
    if test -n "$libxml2_static"; then
    	if test -f "$lib/libxml2.a"; then
	    LIBXML2_LDFLAGS="$lib/libxml2.a"
       	    AC_MSG_RESULT([$lib (forced static)])
    	fi
    else
        if test -f "$lib/libxml2.${SHLIB_SUFFIX}"; then
	    if test "$lib" != "/lib" -a "$lib" != "/usr/lib"; then
       	        LIBXML2_LDFLAGS="-L$lib"
    	    fi
	    LIBXML2_LDFLAGS="$LIBXML2_LDFLAGS -lxml2"
       	    AC_MSG_RESULT([$lib (shared)])
    	elif test -f "$lib/libxml2.a"; then
	    if test "$lib" != "/lib" -a "$lib" != "/usr/lib"; then
       	        LIBXML2_LDFLAGS="-L$lib"
    	    fi
	    LIBXML2_LDFLAGS="$LIBXML2_LDFLAGS -lxml2"
       	    AC_MSG_RESULT([$lib (static)])
    	fi
    fi
    if test -z "$LIBXML2_LDFLAGS"; then
        return
    fi
    AC_MSG_CHECKING([for libxml2 include files])
    # try to find include files
    if test "$1" = "/"; then
        inc=/usr/include
    else
        inc=$1/include
    fi
    if test -f "$inc/libxml/xmlversion.h"; then
        AC_MSG_RESULT([found: $inc])
        set_libxml2_cppflags $inc
    elif test -f "$inc/libxml2/libxml/xmlversion.h"; then
        AC_MSG_RESULT([found: $inc/libxml2])
	inc=$inc/libxml2
        set_libxml2_cppflags $inc
    else
	AC_MSG_ERROR([not found])
    fi
    AC_MSG_CHECKING([libxml2 version])
    # check version
    full=`grep define.*LIBXML_DOTTED_VERSION $inc/libxml/xmlversion.h | cut -f2 -d\"`
    vers=`echo $full | awk 'BEGIN { FS = "."; } {printf("%04d%03d\n", $1 * 1000 + $2, $3);}'`
    if test "$vers" -ge 2004022; then
        AC_MSG_RESULT([$full (OK)])
    else
	AC_MSG_ERROR([libxml2 2.4.22 or higher is needed; found version $full])
    fi

    AC_SUBST(LIBXML2_LDFLAGS)
    AC_SUBST(LIBXML2_CPPFLAGS)
}

# see if we can figure out where the libxml2 library is
AC_ARG_WITH([libxml2-dir],
  [AS_HELP_STRING([--with-libxml2-dir@<:@=DIR@:>@],
                  [LIBXML2 directory])],
  [if test ! -d "${with_libxml2_dir}"; then AC_MSG_ERROR(bad value ${with_libxml2_dir} for --with-libxml2-dir); unset with_libxml2_dir; fi],
  [with_libxml2_dir="$LIBXML2_DIR"])

AC_MSG_CHECKING([for libxml2 libraries and header files])
for dir in "${with_libxml2_dir}" /usr / /usr/local /opt/gnu /opt/libxml2 /usr/local/libxml2 /opt/local /sw /usr/sfw /opt/sfw; do
    find_libxml2 $dir
    if test -n "$LIBXML2_LDFLAGS"; then
        break
    fi
done
if test -z "$LIBXML2_LDFLAGS"; then
    AC_MSG_ERROR([no libxml2 library found])
fi

AC_ARG_ENABLE([profile],
     [AS_HELP_STRING([--enable-profile],
		     [turn on profiling support (default=no)])],
     [case "${enable_profile}" in
       yes|no) ;;
       *)      AC_MSG_ERROR(bad value ${enable_profile} for --enable-profile) ;;
      esac],
     [enable_profile=no])

AC_ARG_ENABLE([debug],
     [AS_HELP_STRING([--enable-debug],
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
else
   AC_DEFINE(NDEBUG, 1, Define if assert() declarations should be suppressed)
fi

AC_ARG_ENABLE([optimization],
     [AS_HELP_STRING([--enable-optimization],
		     [turn on optimization (default=auto (yes unless debugging is enabled)])],
     [case "${enable_optimization}" in
       yes|no) ;;
       *)      AC_MSG_ERROR(bad value ${enable_optimization} for --enable-optimization) ;;
      esac],
     [enable_optimization=auto])

if test "$enable_optimization" = "auto"; then
   if test "${enable_debug}" = yes; then
      enable_optimization=no
   else
      enable_optimization=yes
   fi
fi

AC_ARG_ENABLE([builtin-modules],
     [AS_HELP_STRING([--enable-builtin-modules],
		     [link in module code directly (default=no)])],
     [case "${enable_builtin_modules}" in
       yes|no) ;;
       *)      AC_MSG_ERROR(bad value ${enable_builtin_modules} for --enable-builtin-modules) ;;
      esac],
     [enable_builtin_modules=no])

# check for gcc visibility support
AC_MSG_CHECKING([for gcc visibility support])
if test "$GXX" = "yes"; then
   gcc_version=`$CXX -dumpversion`
   if test "$gcc_version" \> "4"; then
      AC_DEFINE(HAVE_GCC_VISIBILITY, 1, if compiling with g++ and visibility support is available)
      AC_MSG_RESULT([yes, $CXX $gcc_version])
      CXXFLAGS="$CXXFLAGS -fvisibility=hidden"
   else
      AC_MSG_RESULT([no, gcc $gcc_version])
   fi
else
   AC_MSG_RESULT([no, $CXX])
fi

AC_ARG_ENABLE([threads],
  [AS_HELP_STRING([--enable-threads],
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
  [AS_HELP_STRING([--with-tibrv@<:@=DIR@:>@],
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
   rv_found="$with_tibrv"

   # setup tibrv compile and link vars
   TIBRV_CPPFLAGS="-I${with_tibrv}/include"
   TIBRV_LDFLAGS="-L${with_tibrv}/lib -ltibrv${RVLIBSUFFIX} -ltibrvcm${RVLIBSUFFIX} -ltibrvcmq${RVLIBSUFFIX} -ltibrvft${RVLIBSUFFIX} -ltibrvsd${RVLIBSUFFIX}"
   # check for dynamic libtibrvcpp
   if test -f "${with_tibrv}/lib/libtibrvcpp${RVLIBSUFFIX}.${SHLIB_SUFFIX}"; then
      TIBRVCPP_LDFLAGS="-ltibrvcpp${RVLIBSUFFIX}"
   else
      TIBRVCPP_LDFLAGS="${with_tibrv}/lib/libtibrvcpp${RVLIBSUFFIX}.a"   
   fi

   # check if C++ compiler can link with TIBCO Rendezvous c++ libs
   SAVE_CPPFLAGS="$CPPFLAGS"
   SAVE_LDFLAGS="$LDFLAGS"
   CPPFLAGS="$CPPFLAGS $TIBRV_CPPFLAGS"
   LDFLAGS="$LDFLAGS $TIBRV_LDFLAGS $TIBRVCPP_LDFLAGS $OPENSSL_LDFLAGS"

   # hack to work around a bug in 64-bit g++ on linux? - if libtibrvcpp is not last on the compile line the link will fail for some reason
   SAVE_LIBS="$LIBS"
   LIBS="$LIBS -ltibrvcpp${RVLIBSUFFIX}"

   AC_MSG_CHECKING([if we can link to the TIBCO Rendezvous C++ library])
   AC_LINK_IFELSE([AC_LANG_PROGRAM(
[[#include <tibrv/tibrvcpp.h>
]],[[
Tibrv::open();
Tibrv::close();
	]])],tibrv_links=yes, tibrv_links=no)

   CPPFLAGS="$SAVE_CPPFLAGS"
   LDFLAGS="$SAVE_LDFLAGS"
   LIBS="$SAVE_LIBS"

   if test ${tibrv_links} = "no"; then
      AC_MSG_RESULT([failed: please recompile libtibrvcpp using this c++ compiler ($CXX) and install in $with_tibrv/lib and run configure again if you want the tibrv module])
      unset with_tibrv
      unset TIBRV_CPPFLAGS
      unset TIBRV_LDFLAGS
   else
      AC_MSG_RESULT([ok])
      # check if secure daemon support was enabled in libtibrvcpp
      SAVE_CPPFLAGS="$CPPFLAGS"
      SAVE_LDFLAGS="$LDFLAGS"
      CPPFLAGS="$CPPFLAGS $TIBRV_CPPFLAGS"
      LDFLAGS="$LDFLAGS $TIBRV_LDFLAGS"

      AC_MSG_CHECKING([if secure daemon support was enabled in libtibrvcpp])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <tibrv/tibrvcpp.h>
#include <tibrv/sdcpp.h>
	 ]],[[
TibrvSdContext::setDaemonCert(TIBRV_SECURE_DAEMON_ANY_NAME, TIBRV_SECURE_DAEMON_ANY_CERT);
	]])],tibrvsd_links=yes, tibrvsd_links=no)

      CPPFLAGS="$SAVE_CPPFLAGS"
      LDFLAGS="$SAVE_LDFLAGS"

      if test "${tibrvsd_links}" = "yes"; then
         AC_MSG_RESULT([ok])
	 TIBRV_LDFLAGS="$TIBRV_LDFLAGS -lssl -lcrypto"
	 AC_DEFINE(TIBRV_SD, 1, Define if TIBCO Rendezvous secure daemon support should be included)
      else
         AC_MSG_RESULT([failed: if you want secure daemon support in the tibrv module, rebuild libtibrvcpp with secure daemon support (remove comments from the SD_MODULE line in $with_tibrv/src/librvcpp/Makefile), rebuild the library and install in $with_tibrv/lib and rerun configure])
      fi
      AC_SUBST(TIBRV_CPPFLAGS)
      AC_SUBST(TIBRV_LDFLAGS)
      AC_SUBST(TIBRVCPP_LDFLAGS)
      AC_DEFINE(TIBRV, 1, Define if TIBCO Rendezvous support should be included)
      with_tibrv=yes
   fi
fi

AC_ARG_WITH([tibae],
  [AS_HELP_STRING([--with-tibae@<:@=DIR@:>@],
		  [TIBCO Adapters SDK directory (defaults to $SDK_ROOT)])],
  [if test ! -d "${with_tibae}"; then AC_MSG_ERROR(bad value ${with_tibae} for --with-tibae); unset with_tibae; fi])

AC_MSG_CHECKING([TIBCO Adapters SDK])
if test -n "$with_tibae"; then
   # disable AE SDK on HP-UX PA-RISC, it requires compiling without the -AA flag which we don't support
   if test "$OS" = "HP-UX" -a $host_cpu != "ia64"; then
      AC_MSG_WARN([TIBCO Adapters SDK not supported on HP-UX])
      unset with_tibae
   elif test ! -e "$with_tibae/include/sdk/Maverick.h"; then
      AC_MSG_ERROR([can't find Maverick.h in $with_tibae/include/sdk])
   elif test -z "`ls $with_tibae/lib/libmaverick*.${SHLIB_SUFFIX} 2>/dev/null`"; then
      AC_MSG_ERROR([can't find libmaverick*.${SHLIB_SUFFIX} in $with_tibae/lib])
   else
      AC_MSG_RESULT([$with_tibae])
   fi
else
   # disable AE SDK on HP-UX PA-RISC, it requires compiling without the -AA flag which we don't support
   if test "$OS" = "HP-UX" -a $host_cpu != "ia64"; then
      AC_MSG_WARN([TIBCO Adapters SDK not supported on HP-UX])
      unset with_tibae
   elif test -n "$SDK_ROOT" -a -f "$SDK_ROOT/include/sdk/Maverick.h" -a -n "`ls $SDK_ROOT/lib/libmaverick*.${SHLIB_SUFFIX} 2>/dev/null`"; then
      AC_MSG_RESULT([$SDK_ROOT])
      with_tibae=$SDK_ROOT
   else
      AC_MSG_RESULT([not found])
      unset with_tibae
   fi
fi

if test -n "$with_tibae"; then
   if test -z "$rv_found"; then
      AC_MSG_WARN([TIBCO Adapters SDK found, but no Rendezvous found, disabling tibae support])
      unset with_tibae
   else
      AC_MSG_CHECKING([TIBCO Adapters SDK version])
      if test -e "$with_tibae"/prodinfo; then
	 TIBCO_SDK_VERSION=`grep SDK "$with_tibae"/prodinfo|head -1|cut -f2`
      elif test -e "$with_tibae"/version.txt; then
         TIBCO_SDK_VERSION=`grep SDK "$with_tibae"/version.txt|head -1|cut -b25- | cut -f1 -d\ `
      else
         AC_MSG_RESULT([unable to determine TIBCO Adapters SDK version, disabling tibae support])
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
        [AS_HELP_STRING([--with-tibae-tpcl@<:@=DIR@:>@],
		        [TIBCO Adpaters TPCL directory (defaults to $TPCL_ROOT)])],
			[if test ! -d "${with_tibae_tpcl}"; then AC_MSG_ERROR(bad value ${with_tibae_tpcl} for --with-tibae-tpcl); unset with_tibae_tpcl; fi])
      AC_MSG_CHECKING([TIBCO Adapters TPCL])
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
	4.1.2) TIBAE_LDFLAGS="-L${with_tibae}/lib -lmaverick41 -lpthread"
	       TIBAE_CPPFLAGS="-I${with_tibae}/include"
	       AC_DEFINE(TIBCO_SDK, 4, [define for TIBCO SDK 4.*])
	       ;;
	4.1.3) TIBAE_LDFLAGS="-L${with_tibae}/lib -lmaverick41 -lpthread"
	       TIBAE_CPPFLAGS="-I${with_tibae}/include"
	       AC_DEFINE(TIBCO_SDK, 4, [define for TIBCO SDK 4.*])
	       ;;
        5.0.*) TIBAE_LDFLAGS="-L${with_tibae}/lib -L${with_tibae_tpcl}/lib -lmaverick50 -lrepowww532 -lxerces-c2_1 -lssl -lcrypto -lpthread"
	       TIBAE_CPPFLAGS="-I${with_tibae}/include/sdk"
	       AC_DEFINE(TIBCO_SDK, 5, [define for TIBCO SDK 5.*])
	       ;;
        5.3.*) TIBAE_LDFLAGS="-L${with_tibae}/lib -L${with_tibae_tpcl}/lib -lmaverick53 -lrepowww532 -lxerces-c2_1 -lssl -lcrypto -licuuc -lpthread"
	       TIBAE_CPPFLAGS="-I${with_tibae}/include/sdk"
	       AC_DEFINE(TIBCO_SDK, 5, [define for TIBCO SDK 5.*])
	       ;;
	*)     AC_MSG_ERROR(Unsupported TIBCO SDK Version) 
	       ;;
   esac

   case "${host_os}" in
     *linux*)	TIBAE_CPPFLAGS="$TIBAE_CPPFLAGS -DLINUX24 -ldl"
		;;
     *hpux*)    TIBAE_CPPFLAGS="$TIBAE_CPPFLAGS -DHP_UX +DAportable"
		;;
     *)		TIBAE_CPPFLAGS="$TIBAE_CPPFLAGS -ldl"
		;; 
   esac

   # check if C++ compiler can link with TIBCO libs
   SAVE_CPPFLAGS="$CPPFLAGS"
   SAVE_LDFLAGS="$LDFLAGS"
   CPPFLAGS="$CPPFLAGS $TIBAE_CPPFLAGS $TIBRV_CPPFLAGS"
   LDFLAGS="$LDFLAGS $TIBAE_LDFLAGS $TIBRV_LDFLAGS"
   AC_MSG_CHECKING([if we can link to the TIBCO AE SDK])
   AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <Maverick.h>
class TApp : public MApp
{
      virtual void onTermination() throw (MException) {}
      virtual void onInitialization() throw (MException) {}
   public:
      inline TApp(MAppProperties *pm) : MApp(pm) {}
};

	 ]],[[
TApp ma(NULL);
	]])], tibae_sdk_links=yes, tibae_sdk_links=no)

   CPPFLAGS="$SAVE_CPPFLAGS"
   LDFLAGS="$SAVE_LDFLAGS"

   if test "$tibae_sdk_links" = "no"; then
      AC_MSG_RESULT([failed with this C++ compiler, try CXX=<comp> configure, disabling tibae support])
      unset with_tibae
   else
      AC_MSG_RESULT([ok])
      AC_SUBST(TIBAE_LDFLAGS)
      AC_SUBST(TIBAE_CPPFLAGS)
      AC_DEFINE(TIBAE, 1, [Define if TIBCO Adapters support should be included])
      with_tibae=yes
   fi
fi

# function to help find the tuxedo directory
find_tux()
{
    d=`ls -d $1/bea/tuxedo* 2>/dev/null | tail -1`;
    if test -n "$d" -a -d "$d"; then
        tux_dir="$d"
	return
    fi
    d=`ls -d $1/tuxedo* 2>/dev/null | tail -1`;
    if test -n "$d" -a -d "$d"; then
        tux_dir="$d"
    fi
}

AC_ARG_WITH([tuxedo],
  [AS_HELP_STRING([--with-tuxedo@<:@=DIR@:>@],
		  [BEA Tuxedo directory (defaults to $TUXDIR)])],
  [case "${with_tuxedo}" in 
     yes) with_tuxedo="" ;;
     no) ;;
     *) if test ! -d "${with_tuxedo}"; then AC_MSG_ERROR(bad value ${with_tuxedo} for --with-tuxedo); unset with_tuxedo; fi
   esac])

AC_MSG_CHECKING([BEA Tuxedo])
if test -n "$with_tuxedo"; then
   if test "$with_tuxedo" = "no"; then
      AC_MSG_RESULT([skipping check])
      unset with_tuxedo
   elif test ! -e "$with_tuxedo/include/atmi.h"; then
      AC_MSG_ERROR([can't find atmi.h in $with_tuxedo/include])
   elif test ! -e "$with_tuxedo/lib/libtux.a"; then
      AC_MSG_ERROR([can't find libtux.a in $with_tuxedo/lib])
   else
      AC_MSG_RESULT([$with_tuxedo])
   fi
else
   if test -n "$TUXDIR" -a -f "$TUXDIR/include/atmi.h" -a -e "$TUXDIR/lib/libtux.a"; then
      with_tuxedo=$TUXDIR
   else
      for dir in /opt /usr /usr/local /apps; do
         find_tux $dir
	 if test -n "$tux_dir" -a -f "$tux_dir/include/atmi.h" -a -e "$tux_dir/lib/libtux.a"; then
	    with_tuxedo="$tux_dir"
	    break;
	 fi
      done
   fi
   if test -n "$with_tuxedo"; then
      AC_MSG_RESULT([$with_tuxedo])
   else
      AC_MSG_RESULT([not found])
      unset with_tuxedo
   fi
fi

get_tux_version()
{
    # try to get tuxedo vesion from the directory name
    tux_ver=`echo $with_tuxedo | sed 's/.*tuxedo//'  | sed 's/\///'`
    if test -z "$tux_ver"; then
        tux_ver="unknown"
    fi
}

if test -n "$with_tuxedo"; then
   AC_MSG_CHECKING([BEA Tuxedo version])
   get_tux_version
   AC_MSG_RESULT([$tux_ver])

   TUX_MAJOR=`echo $tux_ver|cut -f1 -d.`
   TUX_MINOR=`echo $tux_ver|cut -f2- -d.`
   AC_DEFINE_UNQUOTED([TUXEDO_VERSION_MAJOR], $TUX_MAJOR, [Tuxedo major version])   
   AC_DEFINE_UNQUOTED([TUXEDO_VERSION_MINOR], $TUX_MAJOR, [Tuxedo major version])   

   # setup tuxedo compile and link vars
   TUXEDO_CPPFLAGS="-I${with_tuxedo}/include"
   TUXEDO_LIBS="-L${with_tuxedo}/lib -ltux -lbuft -lfml -lfml32 -lengine"   

   # check if C++ compiler can link with tuxedo libs
   SAVE_CPPFLAGS="$CPPFLAGS"
   SAVE_LDFLAGS="$LDFLAGS"
   CPPFLAGS="$CPPFLAGS $TUXEDO_CPPFLAGS"

   case "$tux_ver" in
      8.*) LDFLAGS="$LDFLAGS $TUXEDO_LIBS $OPENSSL_LDFLAGS -lgiconv -lusort" 
	   ;;
      9.*|unknown|*)
           LDFLAGS="$LDFLAGS $TUXEDO_LIBS $OPENSSL_LDFLAGS -lgiconv -lusort -lutrace"
	   ;;
   esac

   AC_MSG_CHECKING([if we can link to the Tuxedo libraries])
   AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <atmi.h>
	 ]],[[
tpinit((TPINIT*)0);
	]])], tuxedo_links=yes, tuxedo_links=no)

   CPPFLAGS="$SAVE_CPPFLAGS"
   LDFLAGS="$SAVE_LDFLAGS"

   if test ${tuxedo_links} = "no"; then
      AC_MSG_RESULT([failed: cannot link with the Tuxedo libraries in ${with_tuxedo}/lib, skipping support for the tuxedo module])
      unset with_tuxedo
      unset TUXEDO_CPPFLAGS
      unset TUXEDO_LIBS
   else
      AC_MSG_RESULT([ok])
      AC_SUBST(TUXEDO_CPPFLAGS)
      AC_SUBST(TUXEDO_LIBS)
      AC_DEFINE(TUXEDO, 1, Define if Tuxedo support should be included)
      with_tuxedo=yes
   fi
fi

AC_ARG_WITH([oracle],
  [AS_HELP_STRING([--with-oracle@<:@=DIR@:>@],
                  [directory for Oracle support (defaults to $ORACLE_HOME)])],
   , 
   [with_oracle=auto])

if test "$with_oracle" = "auto"; then
   if test -n "$ORACLE_HOME" -o -n "$ORACLE_INSTANT_CLIENT" -o "$with_oracle_instant_client"; then
      with_oracle=yes
   else
      with_oracle=no
   fi
fi

get_oracle_version()
{
  AC_MSG_CHECKING([Oracle version])
  if test -n "$2" -a -s "$1/orainst/unix.rgs"; then
    ORACLE_VERSION=`grep '"ocommon"' $1/orainst/unix.rgs | sed "s/[ ][ ]*/:/g" | cut -d: -f 6 | cut -c 2-4`
    test -z "$ORACLE_VERSION" && ORACLE_VERSION=7.3
  elif test -f $1/libclntsh.$SHLIB_SUFFIX.10.1; then
    ORACLE_VERSION=10.1
  elif test -f $1/libclntsh.$SHLIB_SUFFIX.10.0; then
    ORACLE_VERSION=10.0
  elif test -f $1/libclntsh.$SHLIB_SUFFIX.9.0; then
    ORACLE_VERSION=9.0
  elif test -f $1/libclntsh.$SHLIB_SUFFIX.8.0; then
    ORACLE_VERSION=8.1
  elif test -f $1/libclntsh.$SHLIB_SUFFIX.1.0; then
    ORACLE_VERSION=8.0
  elif test -f $1/libclntsh.a; then
    if test -f $1/libcore4.a; then
      ORACLE_VERSION=8.0
    else
      ORACLE_VERSION=8.1
    fi
  else
    AC_MSG_ERROR([Oracle libraries not found, use --without-oracle to build without Oracle support])
  fi
  AC_MSG_RESULT($ORACLE_VERSION)
}

case "$with_oracle" in
  yes) ORACLE_DIR=$ORACLE_HOME ;;
  no) ;;
  *) ORACLE_DIR=$with_oracle; with_oracle=yes ;;
esac

if test "$with_oracle" = "yes"; then
   AC_ARG_WITH([oracle-instant-client],
  [AS_HELP_STRING([--with-oracle-instant-client@<:@=DIR@:>@],
                  [directory for Oracle instant client files])],
   ,
  [with_oracle_instant_client=$ORACLE_INSTANT_CLIENT])

   if test -n "$with_oracle_instant_client"; then
      AC_MSG_CHECKING([for Oracle instant client SDK])
      if test -f "$with_oracle_instant_client/sdk/include/oci.h"; then
	 AC_MSG_RESULT([ok])
	 ORACLE_INCLUDES="-I$with_oracle_instant_client/sdk/include"
	 oracle_lib_dir=$with_oracle_instant_client
	 get_oracle_version $with_oracle_instant_client
      else
	 AC_MSG_ERROR([instant client SDK include files not found, use --without-oracle to build without Oracle support or install the instant client SDK])
      fi
   else
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
      if test "$enable_64bit" != "yes" -a -d "$ORACLE_DIR/lib32"; then
         oracle_lib_dir="$ORACLE_DIR/lib32"
      else
         oracle_lib_dir="$ORACLE_DIR/lib"
      fi
      get_oracle_version $oracle_lib_dir $ORACLE_DIR
   fi

   AC_SUBST(ORACLE_INCLUDES)

   case "$ORACLE_VERSION" in
      9*|10*) ORACLE_LIBS="-L$oracle_lib_dir -lclntsh" ;; 
      *)      AC_MSG_ERROR(Unsupported Oracle version) ;;
   esac
   AC_SUBST(ORACLE_LIBS)

   # check for 'oci.h'
   SAVE_CXXFLAGS="$CXXFLAGS"
   SAVE_LDFLAGS="$LDFLAGS"
   CXXFLAGS="$CXXFLAGS $ORACLE_INCLUDES"
   LDFLAGS="$LDFLAGS $ORACLE_LIBS"
   AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <oci.h>
         ]],[[
        ]])], with_oracle=yes, AC_MSG_ERROR(file 'oci.h' not found in '$ORACLE_INCLUDES'))
   CXXFLAGS="$SAVE_CXXFLAGS"
   LDFLAGS="$SAVE_LDFLAGS"
fi

AC_ARG_ENABLE([mysql],
     [AS_HELP_STRING([--enable-mysql],
		     [enable mysql support (default=auto)])],
     [case "${enable_mysql}" in
       yes|no|auto) ;;
       *)      AC_MSG_ERROR(bad value ${enable_mysql} for --enable-mysql) ;;
      esac],
     [enable_mysql=auto])

AC_ARG_WITH([mysql-includes],
    [AS_HELP_STRING([--with-mysql-includes@<:@=DIR@:>@],
                    [MySQL include file directory])],
    [if test ! -d "${with_mysql_includes}"; then AC_MSG_ERROR(bad value ${with_mysql_includes} for --with-mysql-includes); unset with_mysql_includes; fi])

AC_ARG_WITH([mysql-libs],
    [AS_HELP_STRING([--with-mysql-libs@<:@=DIR@:>@],
                    [directory for MySQL libraries (default: /usr/lib/mysql)])],
    [if test ! -d "${with_mysql_libs}"; then AC_MSG_ERROR(bad value ${with_mysql_libs} for --with-mysql-libs); unset with_mysql_libs; fi])

check_mysql_include()
{
    if test -f "$1/mysql.h"; then
       mysql_include_dir=$1
    fi
}

# see if we can figure out where the mysql files are
if test "${enable_mysql}" != "no"; then
   AC_MSG_CHECKING([for mysql includes])
   if test -n "$with_mysql_includes"; then 
      if test -f "$with_mysql_includes/mysql.h"; then
      	 AC_MSG_RESULT([$with_mysql_includes])
	 enable_mysql=yes
      else
	 AC_MSG_RESULT([error: $with_mysql_includes does not contain mysql.h])
	 unset with_mysql_includes
	 enable_mysql=no
      fi
   else
      for dir in "$MYSQL_DIR/include" /usr/local/mysql/include /opt/mysql/include /usr/mysql/include /usr/include/mysql /sw/include/mysql /opt/local/include/mysql /opt/sfw/include/mysql /usr/sfw/include/mysql; do
      	  check_mysql_include $dir
      	  if test -n "$mysql_include_dir"; then
	     AC_MSG_RESULT([$mysql_include_dir])
	     with_mysql_includes="$mysql_include_dir"
	     enable_mysql=yes
	     break;
	  fi
      done
      if test -z "$mysql_include_dir"; then
          AC_MSG_RESULT([not found])
	  unset with_mysql_includes
      fi
   fi
fi

check_mysql_lib()
{
    if test -f "$1/libmysqlclient_r.${SHLIB_SUFFIX}"; then
       mysql_lib_dir=$1
       AC_MSG_RESULT([$1 (shared)])
    elif test -f "$1/libmysqlclient_r.a"; then
       mysql_lib_dir=$1
       mysql_static=1
       AC_MSG_RESULT([$1 (static)])
    fi
}

if test "${enable_mysql}" != "no"; then
   AC_MSG_CHECKING([for mysql libraries])
   if test -n "$with_mysql_libs"; then
      check_mysql_lib "$with_mysql_libs"

      if test -n "$mysql_lib_dir"; then
	 enable_mysql=yes
      else
	 AC_MSG_RESULT([error: $with_mysql_libs does not contain libmysqlclient])
	 unset with_mysql_libs
      fi
   else
      for dir in "$MYSQL_DIR/lib${LIBSUFFIX}" /usr/local/mysql/lib${LIBSUFFIX} /opt/mysql/lib${LIBSUFFIX} /usr/mysql/lib${LIBSUFFIX} /usr/lib${LIBSUFFIX}/mysql /sw/lib/mysql /opt/local/lib${LIBSUFFIX} /usr/sfw/lib${LIBSUFFIX} /opt/sfw/lib${LIBSUFFIX} ;do
      	  check_mysql_lib $dir
      	  if test -n "$mysql_lib_dir"; then
	     with_mysql_libs="$mysql_lib_dir"
	     enable_mysql=yes
	     break;
	  fi
      done
      if test -z "$mysql_lib_dir"; then
          AC_MSG_RESULT([not found])
	  unset with_mysql_libs
      fi
   fi
fi

if test "${enable_mysql}" = yes; then
  if test -n "$with_mysql_includes" -a -n "$with_mysql_libs"; then
     MYSQL_INCLUDES="-I$with_mysql_includes"
     if test -n "$mysql_static"; then
          MYSQL_LIBS="$with_mysql_libs/libmysqlclient_r.a"
     else
          MYSQL_LIBS="-L$with_mysql_libs -lmysqlclient_r"
     fi
     # check if C++ compiler can link with MYSQL libs
     SAVE_CXXFLAGS="$CXXFLAGS"
     SAVE_LDFLAGS="$LDFLAGS"
     CXXFLAGS="$CXXFLAGS $MYSQL_INCLUDES"
     LDFLAGS="$LDFLAGS $MYSQL_LIBS"
     AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <mysql.h>
	 ]],[[
MYSQL mysql; mysql_init(&mysql);
	]])], mysql_links=yes, mysql_links=no)

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

# don't add system directories to library path
set_ncurses_lib()
{
   if test "$1" = "/usr" -a "$1" = "/"; then
      NCURSES_LIBS="-l$2 -lpanel"
   else
      NCURSES_LIBS="-L$lib -l$2 -lpanel"
   fi
   AC_SUBST(NCURSES_LIBS)
}

# checks for ncurses (or curses) and panel libraries and include files and sets NCURSES_LIBS and NCURSES_CPPFLAGS appropriately
check_ncurses()
{
    lib=$1/lib${LIBSUFFIX}

    if test -n "$ncurses_static"; then
    	if test -f "$lib/libncurses.a" -a -f "$lib/libpanel.a"; then
	    ncurses_dir=$1
       	    AC_MSG_RESULT([$lib (ncurses forced static)])
	    NCURSES_LIBS="$lib/libncurses.a $lib/libpanel.a"
	elif test -f "$lib/libcurses.a" -a -f "$lib/libpanel.a"; then
	    ncurses_dir=$1
       	    AC_MSG_RESULT([$lib (curses forced static)])
	    NCURSES_LIBS="$lib/libcurses.a $lib/libpanel.a"
    	fi
    elif test -f "$lib/libncurses.${SHLIB_SUFFIX}" -a -f "$lib/libpanel.${SHLIB_SUFFIX}"; then
       ncurses_dir=$1
       AC_MSG_RESULT([$lib (ncurses shared)])
       set_ncurses_lib "$1" ncurses
    elif test -f "$lib/libncurses.a" -a -f "$lib/libpanel.a"; then
       ncurses_dir=$1
       AC_MSG_RESULT([$lib (ncurses static)])
       set_ncurses_lib "$1" ncurses
    elif test -f "$lib/libcurses.${SHLIB_SUFFIX}" -a -f "$lib/libpanel.${SHLIB_SUFFIX}"; then
       ncurses_dir=$1
       AC_MSG_RESULT([$lib (curses shared)])
       NCURSES_CPPFLAGS="-DNOMACROS"
       set_ncurses_lib "$1" curses
    elif test -f "$lib/libcurses.a" -a -f "$lib/libpanel.a"; then
       ncurses_dir=$1
       AC_MSG_RESULT([$lib (curses static)])
       NCURSES_CPPFLAGS="-DNOMACROS"
       set_ncurses_lib "$1" curses
    fi

    # make sure include files are in the expected place as well
    if test -n "$ncurses_dir"; then
       AC_MSG_CHECKING([for ncurses and panel include files])
       if test -f "$1/include/curses.h" -a "$1/include/panel.h"; then
          AC_MSG_RESULT([found in $ncurses_dir])
	  if test "$1" = "/usr"; then
	     NCURSES_CPPFLAGS="$NCURSES_CPPFLAGS"
	  else
	     NCURSES_CPPFLAGS="$NCURSES_CPPFLAGS -I$1/include"
	  fi
	  AC_SUBST(NCURSES_CPPFLAGS)
       elif test -f "$1/include/ncurses/curses.h" -a "$1/include/ncurses/panel.h"; then
          AC_MSG_RESULT([found in $1/include/ncurses])
	  if test "$1" = "/usr"; then
	     NCURSES_CPPFLAGS="$NCURSES_CPPFLAGS -I$1/include/ncurses"
	  else
	     NCURSES_CPPFLAGS="$NCURSES_CPPFLAGS -I$1/include -I$1/include/ncurses"
	  fi
	  AC_SUBST(NCURSES_CPPFLAGS)
       else
          AC_MSG_ERROR([not found!  use configure --disable-ncurses to skip ncurses checks])
       fi
    fi
}

AC_ARG_ENABLE([ncurses],
  [AS_HELP_STRING([--disable-ncurses],
                  [do not include ncurses support])],
  [case "$enable_ncurses" in
      yes|no) ;;
      *)      AC_MSG_ERROR([bad value ${enable_ncurses} for ncurses option]) ;;
   esac],
   [enable_ncurses=yes])

if test "${enable_ncurses}" = yes; then
   enable_ncurses=no
   AC_MSG_CHECKING([for ncurses and panel libraries])

   for dir in "$NCURSES_DIR" /usr /usr/local /usr/local/ncurses /opt/gnu /opt/ncurses /usr/ncurses /sw /opt/local /usr/sfw /opt/sfw; do
       check_ncurses $dir
       if test -n "$ncurses_dir"; then
	  break;
       fi
   done
   if test -z "$ncurses_dir"; then
      AC_MSG_RESULT([not found])
   else
      enable_ncurses=yes
      AC_DEFINE(NCURSES, 1, Define if ncurses support should be included)

      # check for wresize function in library
      SAVE_CPPFLAGS="$CPPFLAGS"
      SAVE_LDFLAGS="$LDFLAGS"
      CPPFLAGS="$CPPFLAGS $NCURSES_CPPFLAGS"
      LDFLAGS="$LDFLAGS $NCURSES_LIBS"

      AC_MSG_CHECKING([for wresize function in ncurses library])
      AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <curses.h>
	 ]],[[
wresize((WINDOW *)0, 0, 0);
	 ]])], have_wresize=yes, have_wresize=no)

      CPPFLAGS="$SAVE_CPPFLAGS"
      LDFLAGS="$SAVE_LDFLAGS"

      if test "$have_wresize" = "yes"; then
	 AC_MSG_RESULT([yes])
	 AC_DEFINE(HAVE_WRESIZE, 1, [define if the wresize function is available])
      else
	 AC_MSG_RESULT([no])
      fi
   fi
fi

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

# checks for an STL slist template
AC_CXX_MAKE_SLIST_H([include/qore/slist_include.h])

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
AC_CHECK_FUNCS([bzero floor gethostbyaddr gethostbyaddr_r gethostbyname gethostbyname_r gethostname gettimeofday memmove memset mkfifo putenv regcomp select socket setsockopt getsockopt strcasecmp strchr strdup strerror strspn strstr atoll strtol strtoll isblank localtime_r gmtime_r exp2 clock_gettime realloc timegm seteuid setegid])

# try to find strtoimax on HPUX
AC_MSG_CHECKING([for strtoimax])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
[[#include <inttypes.h>]], [[strtoimax("10", NULL, 10);]])], 
   [AC_MSG_RESULT(yes)
    AC_DEFINE(HAVE_STRTOIMAX, 1, [Define to 1 if you have strtoimax])], 
   [AC_MSG_RESULT(no)])

# Check definition of gethostbyaddr_r (glibc2 defines this with 8 arguments)
ac_save_CXXFLAGS="$CXXFLAGS"
AC_CACHE_CHECK([style of gethost* routines], qore_gethost_style,
AC_LANG_PUSH([C++])

# Do not treat warnings as errors if we are linking against other libc
# this is to work around gcc not being permissive on non-system includes
# with respect to ANSI C++
# We also remove the -fbranch-probabilities option as this will give warnings
# about not profiled code, which confuses configure
if test "$GXX" = "yes" -a "$with_other_libc" = "no"
then
  CXXFLAGS=`echo "$CXXFLAGS -Werror" | sed 's/-fbranch-probabilities//'`
fi

AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
[[#undef inline
#if !defined(SCO) && !defined(__osf__) && !defined(_REENTRANT)
#define _REENTRANT
#endif
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>]],
[[int skr;
 struct hostent *foo = gethostbyaddr_r((const char *) 0,
  0, 0, (struct hostent *) 0, (char *) NULL,  0, &skr); return (foo == 0);]])],
qore_gethost_style=solaris, qore_gethost_style=other))
AC_LANG_POP([C++])
CXXFLAGS="$ac_save_CXXFLAGS"
if test "$qore_gethost_style" = "solaris"
then
  AC_DEFINE([HAVE_SOLARIS_STYLE_GETHOST], [1],
            [Solaris defines gethostbyaddr_r with 7 arguments. glibc2 defines this with 8 arguments])
fi

# Check definition of gethostbyname_r (glibc2.0.100 is different from Solaris)
ac_save_CXXFLAGS="$CXXFLAGS"
AC_CACHE_CHECK([style of gethostname_r routines], qore_gethostname_style,
AC_LANG_PUSH([C++])
if test "$GXX" = "yes" -a "$with_other_libc" = "no"
then
  CXXFLAGS=`echo "$CXXFLAGS -Werror" | sed 's/-fbranch-probabilities//'`
fi
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
[[#undef inline
#if !defined(SCO) && !defined(__osf__) && !defined(_REENTRANT)
#define _REENTRANT
#endif
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>]],
[[int skr;

 skr = gethostbyname_r((const char *) 0,
  (struct hostent*) 0, (char*) 0, 0, (struct hostent **) 0, &skr);]])],
qore_gethostname_style=glibc2, qore_gethostname_style=other))
AC_LANG_POP([C++])
CXXFLAGS="$ac_save_CXXFLAGS"
if test "$qore_gethostname_style" = "glibc2"
then
  AC_DEFINE([HAVE_GETHOSTBYNAME_R_GLIBC2_STYLE], [1],
            [Solaris define gethostbyname_r with 5 arguments. glibc2 defines this with 6 arguments])
fi

# Check 3rd argument of getthostbyname_r
ac_save_CXXFLAGS="$CXXFLAGS"
AC_CACHE_CHECK([3 argument to gethostname_r routines], qore_gethostname_arg,
AC_LANG_PUSH([C++])
if test "$GXX" = "yes" -a "$with_other_libc" = "no"
then
  CXXFLAGS=`echo "$CXXFLAGS -Werror" | sed 's/-fbranch-probabilities//'`
fi
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
[[#undef inline
#if !defined(SCO) && !defined(__osf__) && !defined(_REENTRANT)
#define _REENTRANT
#endif
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>]],
[[int skr;
 skr = gethostbyname_r((const char *) 0, (struct hostent*) 0, (struct hostent_data*) 0);]])],
qore_gethostname_arg=hostent_data, qore_gethostname_arg=char))
AC_LANG_POP([C++])
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

# turn on optimizations if we know how
if test "$enable_optimization" = "yes"; then
    if test "$GXX" = "yes"; then
        CXXFLAGS="$CXXFLAGS -O3"  # gcc
    elif test "$CXX" = "CC" -a "`echo $host_os|cut -b-7`" = "solaris"; then
	CXXFLAGS="$CXXFLAGS -xO5" # Solaris CC
    elif test -n "`echo $CXX|grep aCC`"; then 
	CXXFLAGS="$CXXFLAGS +O2" # HP-UX aCC
    fi
fi

# turn on some build flags for non-debugging, non-profiling builds
if test "$enable_debug" = "no" -a "$enable_profile" = "no"; then
    # strip binaries and libraries on darwin with -x
    if test "`echo $host_os|cut -b-6`" = "darwin"; then
       LDFLAGS="$LDFLAGS -Wl,-x"
    fi	
fi

# set special compiler flags for C++ compilers to turn off annoying "String literal converted to char* ..." warnings
if test -n "`echo $CXX|grep CC`" -a "`echo $host_os|cut -b-7`" = "solaris"; then
   CXXFLAGS="$CXXFLAGS -erroff=badargtypel2w"
elif test -n "`echo $CXX|grep aCC`"; then 
   CXXFLAGS="$CXXFLAGS +W829"
fi

# turn on warnings if we know how
if test "$GXX" = "yes"; then
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
AC_DEFINE_UNQUOTED([TARGET_ARCH], "$ARCH", [host type])
AC_DEFINE_UNQUOTED([TARGET_OS], "$OS", [host type])
AC_DEFINE_UNQUOTED([TARGET_BITS], $bits, [32 or 64 bit build])
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
AM_CONDITIONAL([COND_TUXEDO],     [test "$with_tuxedo" = yes])
AM_CONDITIONAL([COND_MONOLITHIC], [test "$enable_shared" = no -o "$enable_builtin_modules" = "yes"])
AM_CONDITIONAL([COND_SINGLE_COMPILATION_UNIT], [test "$enable_single_compilation_unit" = yes])

AC_CONFIG_FILES([Makefile lib/Makefile modules/Makefile modules/ncurses/Makefile modules/tibrv/Makefile modules/tibae/Makefile modules/mysql/Makefile modules/oracle/Makefile modules/Tuxedo/Makefile])
AC_OUTPUT

echo "*** BUILTIN FEATURES ***"
printf "library                 : %s %s" "$OS" "$ARCH"
if test "$enable_64bit" = "yes"; then
   printf " (64-bit)\n"
else
   printf " (32-bit)\n"
fi

printf "optimizations           : "
if test "$enable_optimization" = "yes"; then
   echo yes
else
   echo disabled
fi

printf "single compilation unit : "
if test "$enable_single_compilation_unit" = "yes"; then
   echo yes
else
   echo disabled
fi

printf "debug                   : "
if test "$enable_debug" = "yes"; then
   echo yes
else
   echo disabled
fi

printf "profile                 : "
if test "$enable_profile" = "yes"; then
   echo yes
else
   echo disabled
fi

if test "$enable_shared" = "yes"; then
   echo "*** MODULES ***"
fi

printf "ncurses                 : "
if test "$enable_ncurses" = "yes"; then
   echo yes
else
   echo disabled
fi

printf "oracle                  : "
if test "$with_oracle" = "yes"; then
   echo yes
else
   echo disabled
fi

printf "mysql                   : "
if test "$with_mysql" = "yes"; then
   echo yes
else
   echo disabled
fi

printf "tibrv                   : "
if test "$with_tibrv" = "yes"; then
   echo yes
else
   echo disabled
fi

printf "tibae                   : "
if test "$with_tibae" = "yes"; then
   echo yes
else
   echo disabled
fi

printf "tuxedo                  : "
if test "$with_tuxedo" = "yes"; then
   echo yes
else
   echo disabled
fi

# hack to use our libtool if on PA-RISC HP-UX to enable static libs to be linked in to modules
# also to allow 64-bit PA-RISC libraries to be linked to modules
if test "$OS" = "HP-UX" -a "$host_os" != "ia64"; then
   # backup generated libtool
   if test ! -e libtool.orig ; then
      cp libtool libtool.orig
   fi
   # remove references to -ldl in libtool
   sed "s/-ldl//g" libtool > libtool.tmp
   # ensure static libs can be linked to modules
   sed "s/valid_a_lib=no/valid_a_lib=yes/g" libtool.tmp > libtool.tmp.1
   # ensure that valid 64-bit libraries are not recognized as invalid
   sed "s/deplibs_check_method=.*/deplibs_check_method=pass_all/g" libtool.tmp.1 > libtool
   # delete temporary files
   rm -f libtool.tmp libtool.tmp.1
fi


