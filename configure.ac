# Process this file with autoconf to produce a configure script.

# AC_PREREQ(2.59)
AC_INIT([qore], [0.7.0],
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
	AC_MSG_ERROR([incompatible $flexver found - you can download flex 2.5.35 at http://sourceforge.net/projects/flex])
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
   if test "$host_cpu" = "ia64"; then
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
		# add -ldl to libs if on Solaris 8
		if test "${host_os}" = "solaris2.8"; then
		   OTHER_LIBS="$OTHER_LIBS -ldl"
		fi
	        ;;
     *darwin*)  SHLIB_SUFFIX=dylib
		# libtool 1.5.* is creating modules on Darwin 8.* (OS/X 10.4.*) with a .so suffix for some reason
		MODULE_SUFFIX=so
		AC_DEFINE(DARWIN, 1, [if compiling on Darwin])
		darwin=true
		;;
     *hpux*)    if test "$host_cpu" = "ia64"; then
     		   SHLIB_SUFFIX=so
		else  # assume hppa (PA-RISC)
     		   SHLIB_SUFFIX=sl
		   AC_DEFINE(SHLIB_SUFFIX, "sl", shared library suffix)
		fi
		# add "-AA" and -D__STDC_EXT__ to aCC commandline to enable a clean compile
		if test -n "`echo $CXX|grep aCC`"; then
		   CXXFLAGS="$CXXFLAGS -AA -D__STDC_EXT__ -D_RWSTD_MULTI_THREAD"
		   AC_DEFINE(HPUX_ACC_SOCKLEN_HACK, 1, [if compiling on HP-UX with aCC and need to use int instead of socklen_t])
		fi
		AC_DEFINE(HPUX, 1, if compiling on HP-UX)
		AC_DEFINE(NEED_ENVIRON_LOCK, 1, [if the environment operations need explicit locking])
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

AC_ARG_ENABLE([scu],
  [AS_HELP_STRING([--enable-scu],
		  [short for --enable-single-compilation-unit: enable compilation as a single unit])],
  [case "${enable_scu}" in
       yes|no) enable_single_compilation_unit=$enable_scu;;
       *)      AC_MSG_ERROR(bad value ${enable_scu} for --enable-scu) ;;
      esac])

AC_ARG_ENABLE([single-compilation-unit-qt],
  [AS_HELP_STRING([--enable-single-compilation-unit-qt],
		  [enable compilation as a single unit for QT - only if you have at least 2 Gigs of RAM! (default: off)])],
  [case "${enable_single_compilation_unit_qt}" in
       yes|no) ;;
       *)      AC_MSG_ERROR(bad value ${enable_single_compilation_unit_qt} for --enable-single-compilation-unit-qt) ;;
      esac],
  [enable_single_compilation_unit_qt=no])

AC_ARG_ENABLE([scu-qt],
  [AS_HELP_STRING([--enable-scu-qt],
		  [short for --enable-single-compilation-unit-qt: enable compilation as a single unit for QT - only if you have at least 2 Gigs of RAM!])],
  [case "${enable_scu_qt}" in
       yes|no) enable_single_compilation_unit_qt=$enable_scu_qt;;
       *)      AC_MSG_ERROR(bad value ${enable_scu_qt} for --enable-scu-qt) ;;
      esac])

AC_ARG_ENABLE([64bit],
  [AS_HELP_STRING([--enable-64bit],
		  [enable 64bit support (default: auto)])],
  [case "${enable_64bit}" in
       yes|no) ;;
       *)      AC_MSG_ERROR(bad value ${enable_64bit} for --enable-64bit) ;;
      esac],
  [enable_64bit=auto])

if test "$enable_64bit" = "auto"; then
   # turn on 64-bit by default if compiling on itanium or x86_64
   if test "$host_cpu" = "ia64" -o "$host_cpu" = "x86_64"; then
      enable_64bit=yes
   else
      enable_64bit=no
   fi
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
   SYBLIBSUFFIX=64
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
   if test "$host_cpu" = "x86_64"; then
      cpu_family=i386
   fi
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
   bz2lib_static=yes
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
    CPPFLAGS="$CPPCLAGS $OPENSSL_CPPFLAGS"
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
    with_pcre_includes="$1"
}

find_pcre()
{
    lib=$1/lib${LIBSUFFIX}
    # if we only want the static pcre libraries
    if test -n "$pcre_static"; then
    	if test -f "$lib/libpcre.a"; then
	    PCRE_LDFLAGS="$lib/libpcre.a"
	    with_pcre_libs="$lib"
	    pcre_static=1
    	fi
    else
        if test -f "$lib/libpcre.${SHLIB_SUFFIX}"; then
	    if test "$lib" != "/lib" -a "$lib" != "/usr/lib"; then
       	        PCRE_LDFLAGS="-L$lib"
    	    fi
	    PCRE_LDFLAGS="$PCRE_LDFLAGS -lpcre"
	    with_pcre_libs="$lib"
    	elif test -f "$lib/libpcre.a"; then
	    if test "$lib" != "/lib" -a "$lib" != "/usr/lib"; then
       	        PCRE_LDFLAGS="-L$lib"
    	    fi
	    PCRE_LDFLAGS="$PCRE_LDFLAGS -lpcre"
	    with_pcre_libs="$lib"
	    pcre_static=1
    	fi
    fi
    if test -z "$PCRE_LDFLAGS"; then
        return
    fi
    # try to find include files
    if test "$1" = "/"; then
        inc=/usr/include
    else
        inc=$1/include
    fi
    if test -f "$inc/pcre.h"; then
        set_pcre_cppflags $inc
    elif test -f "$inc/pcre/pcre.h"; then
        set_pcre_cppflags $inc/pcre
    else
        unset PCRE_LDFLAGS
	unset with_pcre_libs
	return
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
if test -n "$pcre_static"; then
   AC_MSG_RESULT([includes $with_pcre_includes libs $with_pcre_libs (static)])
else
   AC_MSG_RESULT([includes $with_pcre_includes libs $with_pcre_libs (shared)])
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

set_bz2lib_cppflags()
{
    if test "$1" != "/usr/include"; then
        BZ2_CPPFLAGS=-I$1
    fi
}

find_bz2lib()
{
    lib=$1/lib${LIBSUFFIX}
    # if we only want the static bz2lib libraries
    if test -n "$bz2lib_static"; then
    	if test -f "$lib/libbz2.a"; then
	    BZ2_LDFLAGS="$lib/libbz2.a"
       	    AC_MSG_RESULT([$lib (forced static)])
    	fi
    else
        if test -f "$lib/libbz2.${SHLIB_SUFFIX}"; then
	    if test "$lib" != "/lib" -a "$lib" != "/usr/lib"; then
       	        BZ2_LDFLAGS="-L$lib"
    	    fi
	    BZ2_LDFLAGS="$BZ2_LDFLAGS -lbz2"
       	    AC_MSG_RESULT([$lib (shared)])
    	elif test -f "$lib/libbz2.a"; then
	    if test "$lib" != "/lib" -a "$lib" != "/usr/lib"; then
       	        BZ2_LDFLAGS="-L$lib"
    	    fi
	    BZ2_LDFLAGS="$BZ2_LDFLAGS -lbz2"
       	    AC_MSG_RESULT([$lib (static)])
    	fi
    fi
    if test -z "$BZ2_LDFLAGS"; then
        return
    fi
    AC_MSG_CHECKING([for bz2lib include files])
    # try to find include files
    if test "$1" = "/"; then
        inc=/usr/include
    else
        inc=$1/include
    fi
    if test -f "$inc/bzlib.h"; then
        AC_MSG_RESULT([found: $inc/bzlib.h])
        set_bz2lib_cppflags $inc
    else
	AC_MSG_ERROR([not found])
    fi
    AC_SUBST(BZ2_LDFLAGS)
    AC_SUBST(BZ2_CPPFLAGS)   
}

# see if we can figure out where the bz2lib library is
AC_ARG_WITH([bz2lib-dir],
  [AS_HELP_STRING([--with-bz2lib-dir@<:@=DIR@:>@],
                  [bz2lib directory])],
  [if test ! -d "${with_bz2lib_dir}"; then AC_MSG_ERROR(bad value ${with_bz2lib_dir} for --with-bz2lib-dir); unset with_bz2lib_dir; fi], 
  [with_bz2lib_dir="$BZ2_DIR"])

AC_MSG_CHECKING([for bz2lib libraries and header files])
for dir in "${with_bz2lib_dir}" /usr / /usr/local /opt/gnu /opt/bzlib /usr/local/bzlib /opt/local /sw /usr/sfw /opt/sfw; do
    find_bz2lib $dir
    if test -n "$BZ2_LDFLAGS"; then
        break
    fi
done
if test -z "$BZ2_LDFLAGS"; then
    AC_MSG_ERROR([no bzlib library found])
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

if test "${enable_profile}" = yes; then
   AC_DEFINE(PROFILE, 1, Define if profiling support should be included)
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
      # we no longer set visibility to hidden by default because it makes RTTI symbols invisible
      # all functions in the library are tagged anyway
      #CXXFLAGS="$CXXFLAGS -fvisibility=hidden"
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
   LDFLAGS="$LDFLAGS $TIBRV_LDFLAGS $TIBRVCPP_LDFLAGS"

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

# set TRA_ROOT to SDK_ROOT if SDK_ROOT is set and TRA_ROOT is not
if test -n "$SDK_ROOT" -a -z "$TRA_ROOT"; then TRA_ROOT=$SDK_ROOT; fi

AC_ARG_WITH([tibae],
  [AS_HELP_STRING([--with-tibae@<:@=DIR@:>@],
		  [TIBCO Adapters SDK directory (defaults to $TRA_ROOT)])],
  [if test ! -d "${with_tibae}"; then AC_MSG_ERROR(bad value ${with_tibae} for --with-tibae); unset with_tibae; fi])

AC_MSG_CHECKING([TIBCO Adapters SDK])
if test -n "$with_tibae"; then
   if test ! -e "$with_tibae/include/sdk/Maverick.h" -a ! -e "$with_tibae/include/Maverick.h"; then
      AC_MSG_ERROR([can't find Maverick.h in $with_tibae/include/sdk or in $with_tibae/include])
   else
      AC_MSG_RESULT([$with_tibae])
   fi
else
   # disable AE SDK on HP-UX PA-RISC, it requires compiling without the -AA flag which we don't support
   if test "$OS" = "HP-UX" -a $host_cpu != "ia64"; then
      AC_MSG_WARN([TIBCO Adapters SDK not supported on HP-UX])
      unset with_tibae
   elif test -n "$TRA_ROOT" -a \( -f "$TRA_ROOT/include/sdk/Maverick.h" -o -f "$TRA_ROOT/include/Maverick.h" \) 2>/dev/null; then
      AC_MSG_RESULT([$TRA_ROOT])
      with_tibae=$TRA_ROOT      
   else
      AC_MSG_RESULT([not found])
      unset with_tibae
   fi
fi

AC_ARG_WITH([tibco-ems],
  [AS_HELP_STRING([--with-tibco-ems@<:@=DIR@:>@],
		  [TIBCO EMS directory (defaults to $JMS_ROOT)])],
  [if test ! -d "${with_tibco_ems}"; then AC_MSG_ERROR(bad value ${with_tibco_ems} for --with-tibco-ems); unset with_tibco_ems; fi])

AC_MSG_CHECKING([TIBCO EMS])
if test -n "$with_tibco_ems"; then
   if test ! -e "$with_tibco_ems/clients/c/include/tibems/types.h"; then
      AC_MSG_ERROR([can't find clients/c/include/tibems/types.h in $with_tibco_ems])
   else
      AC_MSG_RESULT([$with_tibco_ems])
   fi
else
   if test -n "$JMS_ROOT" -a \( -f "$JMS_ROOT/clients/c/include/tibems/types.h" \) 2>/dev/null; then
      AC_MSG_RESULT([$JMS_ROOT])
      with_tibco_ems=$JMS_ROOT
   else
      AC_MSG_RESULT([not found])
      unset with_tibco_ems
   fi
fi

if test -n "$with_tibae"; then
   if test -z "$rv_found"; then
      AC_MSG_WARN([TIBCO Adapters SDK and TRA found, but no Rendezvous found, disabling tibae support])
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

if test -n "$with_tibae" -a -n "$with_:tibae_tra"; then
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
        5.4.*) TIBAE_LDFLAGS="-L${with_tibae}/lib -L${with_tibae_tpcl}/lib -lmaverick53 -lrepowww532 -lxerces-c2_1 -lssl -lcrypto -licuuc -lpthread"
               TIBAE_CPPFLAGS="-I${with_tibae}/include"
               AC_DEFINE(TIBCO_SDK, 5, [define for TIBCO SDK 5.*])
               ;;
        5.5.*) if test -z "${with_tibco_ems}"; then AC_MSG_ERROR([SDK 5.5 requires TIBCO EMS, but TIBCO EMS was not found]); fi
	       if test "$enable_64bit" = "yes"; then
	          TIBAE_LDFLAGS="-L${with_tibae}/lib -L${with_tibae}/lib/64 -L${with_tibae_tpcl}/lib -L${with_tibae_tpcl}/lib/64 -lmaverick55${LIBSUFFIX} -lrepowww550${LIBSUFFIX} -lxerces-c2_7 -lxerces-depdom2_7 -lssl -lcrypto -licuuc -licudata -lpthread"
	       else
	          TIBAE_LDFLAGS="-L${with_tibae}/lib -L${with_tibae_tpcl}/lib -lmaverick55${LIBSUFFIX} -lrepowww550${LIBSUFFIX} -lxerces-c2_7 -lssl -lcrypto -licuuc -licudata -lpthread"
	       fi
               TIBAE_CPPFLAGS="-I${with_tibae}/include -I${with_tibae}/include/xercesc -I${with_tibco_ems}/clients/c/include"
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
	]])], tibae_links=yes, tibae_links=no)

   CPPFLAGS="$SAVE_CPPFLAGS"
   LDFLAGS="$SAVE_LDFLAGS"

   if test "$tibae_links" = "no"; then
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
   TUXEDO_LIBS="-L${with_tuxedo}/lib -lbuft -lfml -lfml32 -lengine -ltux"   

   case "${host_os}" in
     *solaris*) TUXEDO_LIBS="${TUXEDO_LIBS} -lxmlfmlbase -ltux" ;;
   esac

   # check if C++ compiler can link with tuxedo libs
   SAVE_CPPFLAGS="$CPPFLAGS"
   SAVE_LDFLAGS="$LDFLAGS"
   CPPFLAGS="$CPPFLAGS $TUXEDO_CPPFLAGS"

   case "$tux_ver" in
      8.*) LDFLAGS="$LDFLAGS $TUXEDO_LIBS -lgiconv -lusort" 
	   ;;
      9.*|unknown|*)
           LDFLAGS="$LDFLAGS $TUXEDO_LIBS -lgiconv -lusort -lutrace"
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
  ora_ver_major=`echo $ORACLE_VERSION | cut -f1 -d.`
  if test $ora_ver_major -lt 10; then
     AC_DEFINE(NEED_ORACLE_LOB_WORKAROUND, 1, [define if we need a workaround for the Oracle streaming LOB callback bug])
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
   AC_CHECK_FUNC([OCIClientVersion], [have_ociclientversion=yes], [have_ociclientversion=no])
   if test "$have_ociclientversion" = "yes"; then
      AC_DEFINE(HAVE_OCICLIENTVERSION, 1, [define if OCIClientVersion is available])      
   fi
   CXXFLAGS="$SAVE_CXXFLAGS"
   LDFLAGS="$SAVE_LDFLAGS"
fi

AC_ARG_WITH([mysql],
    [AS_HELP_STRING([--with-mysql@<:@=DIR@:>@],
                    [MySQL base directory])],
    [if test ! -d "${with_mysql}"; then AC_MSG_ERROR(directory ${with_mysql} does not exist for --with-mysql); unset with_mysql; fi])

check_mysql()
{
    inc="$1/include"
    if test "$1" = "/"; then
       inc=/usr/include
    fi

    if test -f "$inc/mysql.h"; then
       with_mysql_includes=$inc
    elif test -f "$inc/mysql/mysql.h"; then
       with_mysql_includes=$inc/mysql
    fi
    if test -z "$with_mysql_includes"; then
       return
    fi

    lib="$1/lib${LIBSUFFIX}"
    if test -f "$lib/libmysqlclient_r.${SHLIB_SUFFIX}"; then
       with_mysql_libs=$lib
    elif test -f "$lib/mysql/libmysqlclient_r.${SHLIB_SUFFIX}"; then
       with_mysql_libs=$lib/mysql
    elif test -f "$lib/libmysqlclient_r.a"; then
       with_mysql_libs=$lib
       mysql_static=1
    elif test -f "$lib/mysql/libmysqlclient_r.a"; then
       with_mysql_libs=$lib/mysql
       mysql_static=1
    fi

    if test -z "$with_mysql_libs"; then
       unset with_mysql_includes
    else
	if test "$with_mysql_includes" != "/usr/include"; then
	   mysql_app_includes=true
        fi
	if test "$with_mysql_libs" != "/lib${LIBSUFFIX}"; then
	   mysql_app_libs=true
	fi
    fi
}

# see if we can figure out where the mysql include files and libraries are
if test "${with_mysql}" != "no"; then
   AC_MSG_CHECKING([for mysql includes and libraries])

   if test -n "$with_mysql"; then
      check_mysql "$with_mysql"
      if test -z "$with_mysql_includes"; then
	 AC_MSG_ERROR([not found in $with_mysql])
      fi
   fi

   for dir in $MYSQL_DIR / /usr /usr/local/mysql /opt/mysql /usr/mysql /sw /opt/local /opt/sfw /usr/sfw; do
      check_mysql $dir
      if test -n "$with_mysql_includes"; then
	 break;
      fi
   done

   if test -n "$with_mysql_includes"; then
      if test -n "$mysql_static"; then
	 AC_MSG_RESULT([includes=$with_mysql_includes, libs=$with_mysql_libs (static)])
      else
	 AC_MSG_RESULT([includes=$with_mysql_includes, libs=$with_mysql_libs (shared)])
      fi
      with_mysql=yes
   else
      AC_MSG_RESULT([not found])
   fi
fi

if test "${with_mysql}" = yes; then
  if test -n "$with_mysql_includes"; then
     # only set include path if necessary
     if test -n "$mysql_app_includes"; then
          MYSQL_INCLUDES="-I$with_mysql_includes"
     fi
     if test -n "$mysql_static"; then
          MYSQL_LIBS="$with_mysql_libs/libmysqlclient_r.a"
     else
	if test -n "$mysql_app_libs"; then
	   MYSQL_LIBS="-L$with_mysql_libs -lmysqlclient_r"
	else
	   MYSQL_LIBS="-lmysqlclient_r"
	fi
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
  else
     AC_MSG_WARN([no MySQL libraries or includes found, disabling MySQL support])
  fi
fi

check_freetds()
{
    d=`ls -d $1/freetds* 2>/dev/null | tail -1`;
    if test -n "$d"; then
       dir="$d"
    else
       dir="$1"
    fi

    if test -f "$dir/include/tds_sysdep_public.h" -a -f "$dir/include/ctpublic.h"; then
        with_freetds_includes=$dir/include
    elif test -f "$dir/include/freetds/tds_sysdep_public.h" -a -f "$dir/include/freetds/ctpublic.h"; then
        with_freetds_includes=$dir/include/freetds
    else
	return
    fi

    if test -f "$dir/lib${LIBSUFFIX}/libct.${SHLIB_SUFFIX}"; then
       with_freetds_libs=$dir/lib${LIBSUFFIX}
    elif test -f "$dir/lib${LIBSUFFIX}/freetds/libct.${SHLIB_SUFFIX}"; then
       with_freetds_libs=$dir/lib${LIBSUFFIX}/freetds
    else
       unset with_freetds_includes
    fi
}

check_sybase()
{
    d=`ls -d $1/OCS-* 2>/dev/null | tail -1`;
    if test -n "$d"; then
       dir="$d"
    else
       dir="$1"
    fi

    if test -f "$dir/include/ctpublic.h"; then
        with_sybase_includes=$dir/include
    else
	return
    fi

    if test -f "$dir/lib/libsybcs_r.${SHLIB_SUFFIX}"; then
       with_sybase_libs=$dir/lib
    else
       unset with_sybase_includes
    fi
}

# Sybase settings
AC_ARG_WITH([sybase],
    [AS_HELP_STRING([--with-sybase@<:@=DIR@:>@],
                    [Sybase or Sybase OCS base directory])],
    [if test ! -d "${with_sybase}"; then AC_MSG_ERROR(directory ${with_sybase} does not exist for --with-sybase); unset with_sybase; fi])

# see if we can figure out where the sybase include files and libraries are
if test "${with_sybase}" != "no"; then
   if test -n "$with_sybase"; then
      sybase_explicit=yes
   fi

   AC_MSG_CHECKING([for sybase includes and libraries])

   if test -n "$with_sybase"; then
      check_sybase "$with_sybase"
      if test -z "$with_sybase_includes"; then
	 AC_MSG_ERROR([not found in $with_sybase])
      fi
   fi

   for dir in $SYBASE $SYBASE/$SYBASE_OCS /usr/sybase /usr/local/sybase /opt/sybase /usr/local; do
      check_sybase "$dir"
      if test -n "$with_sybase_includes"; then
	 break;
      fi
   done

   if test -n "$with_sybase_includes"; then
      AC_MSG_RESULT([includes=$with_sybase_includes, libs=$with_sybase_libs (Sybase OCS)])
      with_sybase=yes
   else
      AC_MSG_RESULT([not found])
   fi
fi

if test "${with_sybase}" = yes; then
   SYBASE_INCLUDES="-I$with_sybase_includes"
   SYBASE_LIBS="-L$with_sybase_libs -lsybdb${SYBLIBSUFFIX} -lsybcs_r${SYBLIBSUFFIX} -lsybintl_r${SYBLIBSUFFIX} -lsybblk_r${SYBLIBSUFFIX} -lsybcomn_r${SYBLIBSUFFIX} -lsybct_r${SYBLIBSUFFIX} -lsybunic${SYBLIBSUFFIX} -lsybtcl_r${SYBLIBSUFFIX}"
   if test -f $with_sybase_libs/libsybunic${SYBLIBSUFFIX}.${SHLIB_SUFFIX} ; then
      SYBASE_LIBS="$SYBASE_LIBS -lsybunic${SYBLIBSUFFIX}"
   fi

   # check if C++ compiler can link with Sybase libs
   SAVE_CXXFLAGS="$CXXFLAGS"
   SAVE_LDFLAGS="$LDFLAGS"
   CXXFLAGS="$CXXFLAGS $SYBASE_INCLUDES"
   LDFLAGS="$LDFLAGS $SYBASE_LIBS"
   AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <ctpublic.h>
	 ]],[[
CS_CONTEXT *m_context; ct_init(m_context, CS_VERSION_100);
	]])], sybase_links=yes, sybase_links=no)

   CXXFLAGS="$SAVE_CXXFLAGS"
   LDFLAGS="$SAVE_LDFLAGS"

   if test "$sybase_links" = "no"; then
      if test -n "$sybase_explicit"; then
         AC_MSG_ERROR([could not compile and link a test Sybase program])
      else
         AC_MSG_WARN([could not compile and link a test Sybase program, disabling sybase support])
	 unset with_sybase
      fi
   else
      # without this "test" below, the shell will crap out
      test
      AC_SUBST(SYBASE_INCLUDES)
      AC_SUBST(SYBASE_LIBS)
   fi
fi

# freetds settings (mssql driver)
AC_ARG_WITH([freetds],
    [AS_HELP_STRING([--with-freetds@<:@=DIR@:>@],
                    [FreeTDS base directory])],
    [if test ! -d "${with_freetds}"; then AC_MSG_ERROR(directory ${with_freetds} does not exist for --with-freetds); unset with_freetds; fi])

# see if we can figure out where the freetds include files and libraries are
if test "${with_freetds}" != "no"; then
   if test -n "$with_freetds"; then
      freetds_explicit=yes
   fi

   AC_MSG_CHECKING([for freetds includes and libraries])

   if test -n "$with_freetds"; then
      check_freetds "$with_freetds"
      if test -z "$with_freetds_includes"; then
	 AC_MSG_ERROR([not found in $with_freetds])
      fi
   fi

   for dir in $FREETDS /usr /opt /usr/local /sw /opt/local /usr/local /opt/gnu; do
      check_freetds "$dir"
      if test -n "$with_freetds_includes"; then
	 break;
      fi
   done

   if test -n "$with_freetds_includes"; then
      AC_MSG_RESULT([includes=$with_freetds_includes, libs=$with_freetds_libs])
      with_freetds=yes
   else
      AC_MSG_RESULT([not found])
   fi
fi

if test "${with_freetds}" = yes; then
   FREETDS_INCLUDES="-I$with_freetds_includes"
   FREETDS_LIBS="-L$with_freetds_libs -lsybdb -lct"

   # check if C++ compiler can link with FreeTDS libs
   SAVE_CXXFLAGS="$CXXFLAGS"
   SAVE_LDFLAGS="$LDFLAGS"
   CXXFLAGS="$CXXFLAGS $FREETDS_INCLUDES"
   LDFLAGS="$LDFLAGS $FREETDS_LIBS"
   AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <ctpublic.h>
	 ]],[[
CS_CONTEXT *m_context; ct_init(m_context, CS_VERSION_100);
	]])], freetds_links=yes, freetds_links=no)

   if test "$freetds_links" = "no"; then
      if test -n "$freetds_explicit"; then
         AC_MSG_ERROR([could not compile and link a test FreeTDS program])
      else
         AC_MSG_WARN([could not compile and link a test FreeTDS program, disabling mssql support])
	 unset with_freetds
      fi
   else
      # without this "test" below, the shell will crap out
      test

      # check if freetds supports ct_loc_alloc, etc, if it has CS_SYB_LANG_CHARSET defined, 
      # then it will be OK
      AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <cspublic.h>
	 ]],[[
#ifndef CS_SYB_LANG_CHARSET
#error
#endif
	]])], freetds_locale=yes, freetds_locale=no)

      if test "$freetds_locale" = "yes"; then
         AC_DEFINE(FREETDS_LOCALE, 1, [define if freetds ct-lib cs_loc_alloc, cs_locale are (at least partially) implemented])
      fi

      AC_SUBST(FREETDS_INCLUDES)
      AC_SUBST(FREETDS_LIBS)
   fi

   CXXFLAGS="$SAVE_CXXFLAGS"
   LDFLAGS="$SAVE_LDFLAGS"
fi

AC_ARG_WITH([pgsql],
    [AS_HELP_STRING([--with-pgsql@<:@=DIR@:>@],
                    [PostgreSQL base directory])],
    [if test ! -d "${with_pgsql}"; then AC_MSG_ERROR(directory ${with_pgsql} does not exist for --with-pgsql); unset with_pgsql; fi])

check_pgsql()
{
    inc="$1/include"
    if test "$1" = "/"; then
       inc=/usr/include
    fi
    pdir=`ls -d $inc/postgres* 2>/dev/null | grep -v _ext.h | tail -1` 
    if test -n "$pdir"; then
       alt_inc=$pdir
    fi
    #echo DBG: checking $1 inc=$inc alt_inc=$alt_inc
    if test -f "$inc/libpq-fe.h"; then
       with_pgsql_includes=$inc
    elif test -n "$alt_inc" -a -f "$alt_inc/libpq-fe.h"; then
       with_pgsql_includes=$alt_inc
    elif test -f "$inc/postgresql/libpq-fe.h"; then
       with_pgsql_includes=$inc/postgresql
    elif test -f "$inc/pgsql/libpq-fe.h"; then
       with_pgsql_includes=$inc/pgsql
    fi
    if test -z "$with_pgsql_includes"; then
       #echo DBG: pgsql $1 $inc not found
       return
    else
       # check for server include files
       if test -d "$with_pgsql_includes/server"; then
	  with_pgsql_server_includes="$with_pgsql_includes/server"
       elif test -d "$with_pgsql_includes/pgsql/server"; then
	  with_pgsql_server_includes="$with_pgsql_includes/pgsql/server"
       elif test -d "$with_pgsql_includes/postgresql/server"; then
	  with_pgsql_server_includes="$with_pgsql_includes/postgresql/server"
       else
          #echo DBG: pgsql $1 $inc server includes not found
          unset with_pgsql_includes
          return
       fi
    fi

    lib="$1/lib${LIBSUFFIX}"

    pdir=`ls -d $lib/postgres* 2>/dev/null |tail -1` 
    if test -n "$pdir"; then
       alt_lib=$pdir
    fi

    if test -f "$lib/libpq.${SHLIB_SUFFIX}" -o -f "$lib/libpq.so"; then
       with_pgsql_libs=$lib
    elif test -f "$lib/postgresql/libpq.${SHLIB_SUFFIX}"; then
       with_pgsql_libs=$lib/postgresql
    elif test -f "$lib/pgsql/libpq.${SHLIB_SUFFIX}"; then
       with_pgsql_libs=$lib/pgsql
    elif test -f "$lib/libpq.a"; then
       with_pgsql_libs=$lib
       pgsql_static=1
    elif test -f "$lib/postgresql/libpq.a"; then
       with_pgsql_libs=$lib/postgresql
       pgsql_static=1
    elif test -f "$lib/pgsql/libpq.a"; then
       with_pgsql_libs=$lib/pgsql
       pgsql_static=1
    elif test -n "$alt_lib"; then
       if test -f "$alt_lib/libpq.${SHLIB_SUFFIX}" -o -f "$alt_lib/libpq.so"; then
          with_pgsql_libs=$alt_lib
       elif test -f "$alt_lib/postgresql/libpq.${SHLIB_SUFFIX}"; then
          with_pgsql_libs=$alt_lib/postgresql
       elif test -f "$alt_lib/pgsql/libpq.${SHLIB_SUFFIX}"; then
          with_pgsql_libs=$alt_lib/pgsql
       elif test -f "$alt_lib/libpq.a"; then
          with_pgsql_libs=$alt_lib
          pgsql_static=1
       elif test -f "$alt_lib/postgresql/libpq.a"; then
          with_pgsql_libs=$alt_lib/postgresql
          pgsql_static=1
       elif test -f "$alt_lib/pgsql/libpq.a"; then
          with_pgsql_libs=$alt_lib/pgsql
          pgsql_static=1
       fi
    fi

    if test -z "$with_pgsql_libs"; then
       #echo pgsql $1 libs not found
       unset with_pgsql_includes
       unset with_pgsql_server_includes
       return
    fi
    if test "$with_pgsql_includes" != "/usr/include"; then
       pgsql_app_includes=true
    fi
    if test "$with_pgsql_libs" != "/lib${LIBSUFFIX}"; then
       pgsql_app_libs=true
    fi
    
    # check postgresql version
    pg_config=`ls $with_pgsql_server_includes/pg_config*.h 2>/dev/null`
    if test -z "$pg_config"; then
       AC_MSG_ERROR([can't determine PostgreSQL version])
    fi
    version=`grep "PG_VERSION " $pg_config|cut -f2 -d\"`
    version_major=`echo $version|cut -f1 -d.`
    if test -z "$version_major"; then
       AC_MSG_ERROR([can't determine PostgreSQL version])
    fi
    if test $version_major -lt 7; then
       AC_MSG_ERROR([PostgreSQL version $version is not supported (must be >= 7)])
    fi
    AC_DEFINE_UNQUOTED([POSTGRES_VERSION_MAJOR], $version_major, [postgresql major version number])
}

# see if we can figure out where the pgsql include files and libraries are
if test "${with_pgsql}" != "no"; then
   AC_MSG_CHECKING([for PostgreSQL includes and libraries])

   if test -n "$with_pgsql"; then
      check_pgsql "$with_pgsql"
      if test -z "$with_pgsql_includes"; then
	 AC_MSG_ERROR([not found in $with_pgsql])
      fi
   fi

   for dir in $PGSQL_DIR / /usr /usr/local /opt/pgsql /usr/pgsql /sw /opt/local /opt/sfw /usr/sfw /opt/gnu; do
      check_pgsql $dir
      if test -n "$with_pgsql_includes"; then
	 break;
      fi
   done

   if test -n "$with_pgsql_includes"; then
      if test -n "$pgsql_static"; then
	 AC_MSG_RESULT([includes=$with_pgsql_includes, libs=$with_pgsql_libs (static)])
      else
	 AC_MSG_RESULT([includes=$with_pgsql_includes, libs=$with_pgsql_libs (shared)])
      fi
      with_pgsql=yes
   else
      AC_MSG_RESULT([not found])
   fi
fi

if test "${with_pgsql}" = yes; then
  if test -n "$with_pgsql_includes"; then
     # only set include path if necessary
     if test -n "$pgsql_app_includes"; then
          PGSQL_INCLUDES="-I$with_pgsql_includes -I$with_pgsql_server_includes"
     else
          PGSQL_INCLUDES="-I$with_pgsql_server_includes"
     fi
     if test -n "$pgsql_static"; then
          PGSQL_LIBS="$with_pgsql_libs/libpq.a"
     else
	if test -n "$pgsql_app_libs"; then
	   PGSQL_LIBS="-L$with_pgsql_libs -lpq"
	else
	   PGSQL_LIBS="-lpq"
	fi
     fi
     # check if C++ compiler can link with PGSQL libs
     SAVE_CXXFLAGS="$CXXFLAGS"
     SAVE_LDFLAGS="$LDFLAGS"
     CXXFLAGS="$CXXFLAGS $PGSQL_INCLUDES"
     LDFLAGS="$LDFLAGS $PGSQL_LIBS"
     AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <libpq-fe.h>
	 ]],[[
PQconnectdb(NULL);
	]])], pgsql_links=yes, pgsql_links=no)

     CXXFLAGS="$SAVE_CXXFLAGS"
     LDFLAGS="$SAVE_LDFLAGS"

     if test "$pgsql_links" = "no"; then
        AC_MSG_ERROR([could not compile a test PostgreSQL program])
     else
        AC_SUBST(PGSQL_INCLUDES)
	AC_SUBST(PGSQL_LIBS)
        with_pgsql=yes
     fi
  else
     AC_MSG_WARN([no PostgreSQL libraries or includes found, disabling PostgreSQL support])
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

AC_ARG_WITH([ncurses],
  [AS_HELP_STRING([--without-ncurses],
                  [do not include ncurses support])],
  [case "$with_ncurses" in
      yes|no) ;;
      *)      AC_MSG_ERROR([bad value ${with_ncurses} for ncurses option]) ;;
   esac],
   [with_ncurses=yes])

if test "${with_ncurses}" = yes; then
   with_ncurses=no
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
      with_ncurses=yes
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

AC_ARG_WITH([opengl],
    [AS_HELP_STRING([--with-opengl@<:@=DIR@:>@],
                    [OpenGL base directory])],
    [if test "${with_opengl}" != "no" -a ! -d "${with_opengl}"; then AC_MSG_ERROR(directory ${with_opengl} does not exist for --with-opengl); unset with_opengl; fi])

AC_ARG_WITH([opengl-libs],
    [AS_HELP_STRING([--with-opengl-libs@<:@=DIR@:>@],
                    [OpenGL library directory])],
    [if test ! -d "${with_opengl_libs}"; then AC_MSG_ERROR(directory ${with_opengl_libs} does not exist for --with-opengl-libs); unset with_opengl_libs; fi])

AC_ARG_WITH([opengl-includes],
    [AS_HELP_STRING([--with-opengl-includes@<:@=DIR@:>@],
                    [OpenGL include file parent directory])],
    [if test ! -d "${with_opengl_includes}"; then AC_MSG_ERROR(directory ${with_opengl_includes} does not exist for --with-opengl-includes); unset with_opengl_includes; fi])

check_opengl_include()
{
    if test -f "$1/gl.h"; then
       found_opengl_includes=$1
    elif test -f "$1/GL/gl.h"; then
       found_opengl_includes=$1/GL
    elif test -f "$1/OpenGL/gl.h"; then
       found_opengl_includes=$1/OpenGL
    elif test -f "$1/mesa/gl.h"; then
       found_opengl_includes=$1/mesa
    elif test "$2" = "err"; then
       AC_MSG_ERROR([$1 is not a valid OpenGL parent include directory])
    fi
}

check_opengl_lib()
{
    if test -f "$1/libGL.${SHLIB_SUFFIX}" -a "$1/libGLU.${SHLIB_SUFFIX}"; then
       found_opengl_libs=$lib
    elif test "$2" = "err"; then
    	AC_MSG_ERROR([$1 is not a valid OpenGL library directory])
    fi
}

check_opengl_version()
{
    # check opengl version
    opengl_ver=`grep define.*GL_VERSION_1_3.*1 $with_opengl_includes/*.h`
    if test -z "$opengl_ver"; then
       AC_MSG_ERROR([can't find OpenGL API version 1.3])
    fi
}

check_opengl()
{
    if test -z "$with_opengl_includes"; then
        inc="$1/include"
        if test "$1" = "/"; then
       	    inc=/usr/include
        fi
	check_opengl_include $inc

	if test -z "$found_opengl_includes"; then
	    #echo DBG: opengl $1 $inc includes not found
       	    return
    	fi
	with_opengl_includes="$found_opengl_includes"
	unset found_opengl_includes
    fi

    if test -z "$with_opengl_libs"; then
        lib="$1/lib${LIBSUFFIX}"

	check_opengl_lib $lib
    	if test -z "$found_opengl_libs"; then
       	    #echo DBG: opengl $1 $lib libs not found
       	    unset with_opengl_includes
       	    return
    	fi
	with_opengl_libs="$found_opengl_libs"
    fi
    if test "$with_opengl_libs" != "/lib${LIBSUFFIX}"; then
        opengl_app_libs=true
    fi

    check_opengl_version
}

# see if we can figure out where the opengl include files and libraries are
if test "${with_opengl}" != "no"; then
   AC_MSG_CHECKING([for OpenGL includes and libraries])

   if test -n "$with_opengl_includes" -a -n "$with_opengl_libs"; then
      check_opengl_includes "$with_opengl_includes" err
      check_opengl_libs "$with_opengl_libs" err
   else
      if test -n "$with_opengl"; then
      	 check_opengl "$with_opengl"
      	 if test -z "$with_opengl_includes"; then
	    AC_MSG_ERROR([not found in $with_opengl])
      	 fi
      else
         # check for frameworks on darwin
	 if test -n "$darwin" -a -d /System/Library/Frameworks/OpenGL.framework; then
	    opengl_framework="/System/Library/Frameworks"
	    OPENGL_INCLUDES="-I /System/Library/Frameworks/OpenGL.framework/Headers"
	    OPENGL_LIBS="-framework OpenGL"
	 else
	    for dir in $OPENGL_DIR / /usr /usr/X11 /usr/X11R6 /usr/openwin /usr/local /opt/opengl /usr/opengl /sw /opt/local /opt/sfw /usr/sfw /opt/gnu; do
      	       check_opengl $dir
      	       if test -n "$with_opengl_includes"; then
	          break;
      	       fi
   	    done
         fi
      fi
   fi

   if test -n "$opengl_framework"; then
      AC_MSG_RESULT([using Darwin frameworks in $opengl_framework])
      with_opengl=yes
   else
      if test -n "$with_opengl_includes"; then
         if test -n "$opengl_static"; then
	    AC_MSG_RESULT([includes=$with_opengl_includes, libs=$with_opengl_libs (static)])
      	 else
	    AC_MSG_RESULT([includes=$with_opengl_includes, libs=$with_opengl_libs (shared)])
      	 fi
      	 with_opengl=yes
      else
	 AC_MSG_RESULT([not found])
         unset with_opengl
      fi
   fi
fi

if test "${with_opengl}" = yes; then
  if test -n "$with_opengl_includes" -o -n "$opengl_framework"; then
     if test -z "$OPENGL_INCLUDES"; then
        OPENGL_INCLUDES="-I$with_opengl_includes"
     fi
     if test -z "$OPENGL_LIBS"; then
	OPENGL_LIBS="-lGL -lGLU"
	if test -n "$opengl_app_libs"; then
	   OPENGL_LIBS="-L$with_opengl_libs $OPENGL_LIBS"
	fi
     fi

     # check if C++ compiler can link with OPENGL libs
     SAVE_CXXFLAGS="$CXXFLAGS"
     SAVE_LDFLAGS="$LDFLAGS"
     CXXFLAGS="$CXXFLAGS $OPENGL_INCLUDES"
     LDFLAGS="$LDFLAGS $OPENGL_LIBS"
     AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <gl.h>
	 ]],[[
   glBegin(GL_POINTS);
	]])], opengl_links=yes, opengl_links=no)

     CXXFLAGS="$SAVE_CXXFLAGS"
     LDFLAGS="$SAVE_LDFLAGS"

     if test "$opengl_links" = "no"; then
        AC_MSG_ERROR([could not compile a test OpenGL program])
     else
        AC_SUBST(OPENGL_INCLUDES)
	AC_SUBST(OPENGL_LIBS)
        with_opengl=yes
     fi
  else
     AC_MSG_WARN([no OpenGL libraries or includes found, disabling OpenGL support])
     unset with_opengl
  fi
fi

AC_ARG_WITH([glut],
    [AS_HELP_STRING([--with-glut@<:@=DIR@:>@],
                    [glut base directory])],
    [if test "${with_glut}" != "no" -a ! -d "${with_glut}"; then AC_MSG_ERROR(directory ${with_glut} does not exist for --with-glut); unset with_glut; fi])

AC_ARG_WITH([glut-libs],
    [AS_HELP_STRING([--with-glut-libs@<:@=DIR@:>@],
                    [glut library directory])],
    [if test ! -d "${with_glut_libs}"; then AC_MSG_ERROR(directory ${with_glut_libs} does not exist for --with-glut-libs); unset with_glut_libs; fi])

AC_ARG_WITH([glut-includes],
    [AS_HELP_STRING([--with-glut-includes@<:@=DIR@:>@],
                    [glut include file parent directory])],
    [if test ! -d "${with_glut_includes}"; then AC_MSG_ERROR(directory ${with_glut_includes} does not exist for --with-glut-includes); unset with_glut_includes; fi])

check_glut_include()
{
    if test -f "$1/glut.h"; then
       found_glut_includes=$1
    elif test -f "$1/GL/glut.h"; then
       found_glut_includes=$1/GL
    elif test -f "$1/glut/glut.h"; then
       found_glut_includes=$1/glut
    elif test "$2" = "err"; then
       AC_MSG_ERROR([$1 is not a valid glut parent include directory])
    fi
}

check_glut_lib()
{
    if test -f "$1/libglut.${SHLIB_SUFFIX}"; then
       found_glut_libs=$lib
    elif test "$2" = "err"; then
    	AC_MSG_ERROR([$1 is not a valid glut library directory])
    fi
}

check_glut()
{
    if test -z "$with_glut_includes"; then
        inc="$1/include"
        if test "$1" = "/"; then
       	    inc=/usr/include
        fi
	check_glut_include $inc

	if test -z "$found_glut_includes"; then
	    #echo DBG: glut $1 $inc includes not found
       	    return
    	fi
	with_glut_includes="$found_glut_includes"
	unset found_glut_includes
    fi

    if test -z "$with_glut_libs"; then
        lib="$1/lib${LIBSUFFIX}"

	check_glut_lib $lib
    	if test -z "$found_glut_libs"; then
       	    #echo DBG: glut $1 $lib libs not found
       	    unset with_glut_includes
       	    return
    	fi
	with_glut_libs="$found_glut_libs"
    fi
    if test "$with_glut_libs" != "/lib${LIBSUFFIX}"; then
        glut_app_libs=true
    fi
}

# see if we can figure out where the glut include files and libraries are
if test "${with_opengl}" = "yes" -a "${with_glut}" != "no"; then
   AC_MSG_CHECKING([for glut includes and libraries])

   if test -n "$with_glut_includes" -a -n "$with_glut_libs"; then
      check_glut_includes "$with_glut_includes" err
      check_glut_libs "$with_glut_libs" err
   else
      if test -n "$with_glut"; then
      	 check_glut "$with_glut"
      	 if test -z "$with_glut_includes"; then
	    AC_MSG_ERROR([not found in $with_glut])
      	 fi
      else
         # check for frameworks on darwin
	 if test -n "$darwin" -a -d /System/Library/Frameworks/GLUT.framework; then
	    glut_framework="/System/Library/Frameworks"
	    GLUT_INCLUDES="-I /System/Library/Frameworks/GLUT.framework/Headers"
	    GLUT_LIBS="-framework GLUT"
	 else
	    for dir in $GLUT_DIR / /usr /usr/X11 /usr/X11R6 /usr/openwin /usr/local /opt/glut /usr/glut /sw /opt/local /opt/sfw /usr/sfw /opt/gnu; do
      	       check_glut $dir
      	       if test -n "$with_glut_includes"; then
	          break;
      	       fi
   	    done
         fi
      fi
   fi

   if test -n "$glut_framework"; then
      AC_MSG_RESULT([using Darwin frameworks in $glut_framework])
      with_glut=yes
   else
      if test -n "$with_glut_includes"; then
         if test -n "$glut_static"; then
	    AC_MSG_RESULT([includes=$with_glut_includes, libs=$with_glut_libs (static)])
      	 else
	    AC_MSG_RESULT([includes=$with_glut_includes, libs=$with_glut_libs (shared)])
      	 fi
      	 with_glut=yes
      else
	 AC_MSG_RESULT([not found])
         unset with_glut
      fi
   fi
fi

if test "${with_glut}" = yes; then
  if test -n "$with_glut_includes" -o -n "$glut_framework"; then
     if test -z "$GLUT_INCLUDES"; then
        GLUT_INCLUDES="-I$with_glut_includes $OPENGL_INCLUDES"
     fi
     if test -z "$GLUT_LIBS"; then
	GLUT_LIBS="-lglut"
	if test -n "$glut_app_libs"; then
	   GLUT_LIBS="-L$with_glut_libs $GLUT_LIBS $OPENGL_LIBS"
	fi
     fi

     # check if C++ compiler can link with GLUT libs
     SAVE_CXXFLAGS="$CXXFLAGS"
     SAVE_LDFLAGS="$LDFLAGS"
     CXXFLAGS="$CXXFLAGS $GLUT_INCLUDES"
     LDFLAGS="$LDFLAGS $GLUT_LIBS"
     AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <glut.h>
	 ]],[[
   glutInit(0, 0);
	]])], glut_links=yes, glut_links=no)

     CXXFLAGS="$SAVE_CXXFLAGS"
     LDFLAGS="$SAVE_LDFLAGS"

     if test "$glut_links" = "no"; then
        AC_MSG_ERROR([could not compile a test glut program])
     else
        AC_SUBST(GLUT_INCLUDES)
	AC_SUBST(GLUT_LIBS)
        with_glut=yes
     fi
  else
     AC_MSG_WARN([no glut libraries or includes found, disabling glut support])
     unset with_glut
  fi
fi

AC_ARG_WITH([qt],
    [AS_HELP_STRING([--with-qt@<:@=DIR@:>@],
                    [QT base directory])],
    [if test "${with_qt}" != "no" -a ! -d "${with_qt}"; then AC_MSG_ERROR(directory ${with_qt} does not exist for --with-qt); unset with_qt; fi])

AC_ARG_WITH([qt-libs],
    [AS_HELP_STRING([--with-qt-libs@<:@=DIR@:>@],
                    [QT library directory])],
    [if test ! -d "${with_qt_libs}"; then AC_MSG_ERROR(directory ${with_qt_libs} does not exist for --with-qt-libs); unset with_qt_libs; fi])

AC_ARG_WITH([qt-includes],
    [AS_HELP_STRING([--with-qt-includes@<:@=DIR@:>@],
                    [QT include file parent directory])],
    [if test ! -d "${with_qt_includes}"; then AC_MSG_ERROR(directory ${with_qt_includes} does not exist for --with-qt-includes); unset with_qt_includes; fi])

check_qt_include()
{
    if test -f "$1/QtCore/qglobal.h" -a -f "$1/QtGui/QApplication"; then
       found_qt_includes=$1
    elif test "$2" = "err"; then
       AC_MSG_ERROR([$1 is not a valid QT parent include directory])
    fi
}

check_qt_lib()
{
    if test -f "$1/libQtCore.${SHLIB_SUFFIX}"; then
       found_qt_libs=$lib
    elif test "$2" = "err"; then
    	AC_MSG_ERROR([$1 is not a valid QT 4 library directory])
    fi
}

check_qt_version()
{
    # check qt version
    qt_maj=`grep QT_VERSION_STR $with_qt_includes/QtCore/qglobal.h|cut -f2 -d\"|cut -f1 -d.`
    if test -z "$qt_maj"; then
       AC_MSG_ERROR([can't determine QT version])
    fi
    if test $qt_maj -ne 4; then
       AC_MSG_ERROR([QT version $version is not supported (must be = 4)])
    fi
}

check_qt()
{
    if test -z "$with_qt_includes"; then
        inc="$1/include"
        if test "$1" = "/"; then
       	    inc=/usr/include
        fi
	check_qt_include $inc

	if test -z "$found_qt_includes"; then
	    #echo DBG: qt $1 $inc includes not found
       	    return
    	fi
	with_qt_includes="$found_qt_includes"
	unset found_qt_includes
    fi

    if test -z "$with_qt_libs"; then
        lib="$1/lib${LIBSUFFIX}"

	check_qt_lib $lib
    	if test -z "$found_qt_libs"; then
       	    #echo DBG: qt $1 $lib libs not found
       	    unset with_qt_includes
       	    return
    	fi
	with_qt_libs="$found_qt_libs"
    fi
    if test "$with_qt_libs" != "/lib${LIBSUFFIX}"; then
        qt_app_libs=true
    fi

    check_qt_version
}

# see if we can figure out where the qt include files and libraries are
if test "${with_qt}" != "no"; then
   AC_MSG_CHECKING([for QT 4 Core includes and libraries])

   if test -n "$with_qt_includes" -a -n "$with_qt_libs"; then
      check_qt_includes "$with_qt_includes" err
      check_qt_libs "$with_qt_libs" err
   else
      if test -n "$with_qt"; then
      	 check_qt "$with_qt"
      	 if test -z "$with_qt_includes"; then
	    AC_MSG_ERROR([not found in $with_qt])
      	 fi
      else
         # check for frameworks on darwin
	 if test -n "$darwin" -a -d /Library/Frameworks/QtCore.framework; then
	    qt_framework="/Library/Frameworks/QtCore.framework"
	 else
	    for dir in $QT_DIR / /usr /usr/local /opt/qt /opt/qt /usr/qt /usr/qt /sw /opt/local /opt/sfw /usr/sfw /opt/gnu; do
      	       check_qt $dir
      	       if test -n "$with_qt_includes"; then
	          break;
      	       fi
   	    done
         fi
      fi
   fi

   if test -n "$qt_framework"; then
      AC_MSG_RESULT([using Darwin frameworks in $qt_framework])
      QT_CORE_INCLUDES="-I $qt_framework/Headers"
      QT_CORE_LIBS="-framework QtCore"
      with_qt=yes
   else
      if test -n "$with_qt_includes"; then
         if test -n "$qt_static"; then
	    AC_MSG_RESULT([includes=$with_qt_includes, libs=$with_qt_libs (static)])
      	 else
	    AC_MSG_RESULT([includes=$with_qt_includes, libs=$with_qt_libs (shared)])
      	 fi
      	 with_qt=yes
      else
	 AC_MSG_RESULT([not found])
         unset with_qt
      fi
   fi
fi

if test "${with_qt}" = yes; then
  if test -n "$with_qt_includes" -o -n "$qt_framework"; then
     if test -z "$QT_CORE_INCLUDES"; then
        QT_CORE_INCLUDES="-I$with_qt_includes/QtCore -I$with_qt_includes/QtGui"
     fi
     if test -z "$QT_CORE_LIBS"; then
	QT_CORE_LIBS="-lQtCore"
	if test -n "$qt_app_libs"; then
	   QT_CORE_LIBS="-L$with_qt_libs $QT_CORE_LIBS"
	fi
     fi

     # check if C++ compiler can link with QT libs
     SAVE_CXXFLAGS="$CXXFLAGS"
     SAVE_LDFLAGS="$LDFLAGS"
     CXXFLAGS="$CXXFLAGS $QT_CORE_INCLUDES"
     LDFLAGS="$LDFLAGS $QT_CORE_LIBS"
     AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <QCoreApplication>
	 ]],[[
   int argc = 0;
   char **argv = 0;
   QCoreApplication app(argc, argv);
	]])], qt_core_links=yes, qt_core_links=no)

     CXXFLAGS="$SAVE_CXXFLAGS"
     LDFLAGS="$SAVE_LDFLAGS"

     if test "$qt_core_links" = "no"; then
        AC_MSG_ERROR([could not compile a test QT Core program])
     else
        AC_SUBST(QT_CORE_INCLUDES)
	AC_SUBST(QT_CORE_LIBS)
        with_qt=yes
     fi
  else
     AC_MSG_WARN([no QT Core libraries or includes found, disabling QT Core support])
     unset with_qt
  fi
fi

AC_ARG_WITH([qt-gui],
    [AS_HELP_STRING([--with-qt-gui@<:@=DIR@:>@],
                    [QT 4 Gui base directory])],
    [if test ! -d "${with_qt_gui}"; then AC_MSG_ERROR(directory ${with_qt_gui} does not exist for --with-qt-gui); unset with_qt_gui; fi])

AC_ARG_WITH([qt-gui-libs],
    [AS_HELP_STRING([--with-qt-gui-libs@<:@=DIR@:>@],
                    [QT 4 Gui library directory])],
    [if test ! -d "${with_qt_gui_libs}"; then AC_MSG_ERROR(directory ${with_qt_gui_libs} does not exist for --with-qt-gui-libs); unset with_qt_gui_libs; fi])

AC_ARG_WITH([qt-gui-includes],
    [AS_HELP_STRING([--with-qt-gui-includes@<:@=DIR@:>@],
                    [QT 4 Gui include file parent directory])],
    [if test ! -d "${with_qt_gui_includes}"; then AC_MSG_ERROR(directory ${with_qt_gui_includes} does not exist for --with-qt-gui-includes); unset with_qt_gui_includes; fi])

check_qt_gui_include()
{
    if test -f "$1/QtGui/QApplication"; then
       found_qt_gui_includes=$1
    elif test "$2" = "err"; then
       AC_MSG_ERROR([$1 is not a valid QT Gui parent include directory])
    fi
}

check_qt_gui_lib()
{
    if test -f "$1/libQtGui.${SHLIB_SUFFIX}"; then
       found_qt_gui_libs=$lib
    elif test "$2" = "err"; then
    	AC_MSG_ERROR([$1 is not a valid QT 4 Gui library directory])
    fi
}

check_qt_gui()
{
    if test -z "$with_qt_gui_includes"; then
        inc="$1/include"
        if test "$1" = "/"; then
       	    inc=/usr/include
        fi
	check_qt_gui_include $inc

	if test -z "$found_qt_gui_includes"; then
	    #echo DBG: qt $1 $inc includes not found
       	    return
    	fi
	with_qt_gui_includes="$found_qt_gui_includes"
	unset found_qt_gui_includes
    fi

    if test -z "$with_qt_gui_libs"; then
        lib="$1/lib${LIBSUFFIX}"

	check_qt_gui_lib $lib
    	if test -z "$found_qt_gui_libs"; then
       	    #echo DBG: qt $1 $lib libs not found
       	    unset with_qt_gui_includes
       	    return
    	fi
	with_qt_gui_libs="$found_qt_libs"
    fi
    if test "$with_qt_gui_libs" != "/lib${LIBSUFFIX}"; then
        qt_app_gui_libs=true
    fi
}

# see if we can figure out where the qt include files and libraries are
if test "${with_qt}" = "yes" -a "${with_qt_gui}" != "no"; then
   AC_MSG_CHECKING([for QT 4 Gui includes and libraries])

   if test -n "$with_qt_gui_includes" -a -n "$with_qt_gui_libs"; then
      check_qt_gui_includes "$with_qt_gui_includes" err
      check_qt_gui_libs "$with_qt_gui_libs" err
   else
      if test -n "$with_qt_gui"; then
      	 check_qt "$with_qt_gui"
      	 if test -z "$with_qt_gui_includes"; then
	    AC_MSG_ERROR([not found in $with_qt_gui])
      	 fi
      else
         # check for frameworks on darwin
	 if test -n "$darwin" -a -d /Library/Frameworks/QtGui.framework; then
	    qt_gui_framework="/Library/Frameworks/QtGui.framework"
	 else
	    for dir in $QT_DIR / /usr /usr/local /opt/qt /opt/qt /usr/qt /usr/qt /sw /opt/local /opt/sfw /usr/sfw /opt/gnu; do
      	       check_qt_gui $dir
      	       if test -n "$with_qt_gui_includes"; then
	          break;
      	       fi
   	    done
         fi
      fi
   fi

   if test -n "$qt_gui_framework"; then
      AC_MSG_RESULT([using Darwin frameworks in $qt_gui_framework])
      QT_GUI_INCLUDES="-I $qt_gui_framework/Headers"
      QT_GUI_LIBS="-framework QtGui"
      with_qt_gui=yes
   else
      if test -n "$with_qt_gui_includes"; then
         if test -n "$qt_gui_static"; then
	    AC_MSG_RESULT([includes=$with_qt_gui_includes, libs=$with_qt_gui_libs (static)])
      	 else
	    AC_MSG_RESULT([includes=$with_qt_gui_includes, libs=$with_qt_gui_libs (shared)])
      	 fi
      	 with_qt_gui=yes
      else
	 AC_MSG_RESULT([not found])
         unset with_qt_gui
      fi
   fi
fi

if test "${with_qt_gui}" = yes; then
  if test -n "$with_qt_gui_includes" -o -n "$qt_gui_framework"; then
     if test -z "$QT_GUI_INCLUDES"; then
        QT_GUI_INCLUDES="-I$with_qt_includes/QtGui"
     fi
     if test -z "$QT_GUI_LIBS"; then
	QT_GUI_LIBS="-lQtGui"
	if test -n "$qt_gui_app_libs"; then
	   QT_GUI_LIBS="-L$with_qt_libs $QT_GUI_LIBS"
	fi
     fi

     # check if C++ compiler can link with QT libs
     SAVE_CXXFLAGS="$CXXFLAGS"
     SAVE_LDFLAGS="$LDFLAGS"
     CXXFLAGS="$CXXFLAGS $QT_CORE_INCLUDES $QT_GUI_INCLUDES"
     LDFLAGS="$LDFLAGS $QT_CORE_LIBS $QT_GUI_LIBS"
     AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <QApplication>
#include <QPushButton>
	 ]],[[
   int argc = 0;
   char **argv = 0;
   QApplication app(argc, argv);

   QPushButton hello("Hello world!");
   hello.resize(100, 30);

   hello.show();
   return app.exec();
	]])], qt_gui_links=yes, qt_gui_links=no)

     CXXFLAGS="$SAVE_CXXFLAGS"
     LDFLAGS="$SAVE_LDFLAGS"

     if test "$qt_gui_links" = "no"; then
        AC_MSG_ERROR([could not compile a test QT 4 Gui program])
     else
        AC_SUBST(QT_GUI_INCLUDES)
	AC_SUBST(QT_GUI_LIBS)
        with_qt_gui=yes
     fi
  else
     AC_MSG_WARN([no QT 4 Gui libraries or includes found, disabling QT 4 Gui support])
     unset with_qt_gui
  fi
fi

AC_ARG_WITH([qt-opengl],
    [AS_HELP_STRING([--with-qt-opengl@<:@=DIR@:>@],
                    [QT OpenGL base directory])],
    [if test ! -d "${with_qt_opengl}"; then AC_MSG_ERROR(directory ${with_qt_opengl} does not exist for --with-qt-opengl); unset with_qt_opengl; fi])

AC_ARG_WITH([qt-opengl-libs],
    [AS_HELP_STRING([--with-qt-opengl-libs@<:@=DIR@:>@],
                    [QT OpenGL library directory])],
    [if test ! -d "${with_qt_opengl_libs}"; then AC_MSG_ERROR(directory ${with_qt_opengl_libs} does not exist for --with-qt-opengl-libs); unset with_qt_opengl_libs; fi])

AC_ARG_WITH([qt-opengl-includes],
    [AS_HELP_STRING([--with-qt-opengl-includes@<:@=DIR@:>@],
                    [QT OpenGL include file parent directory])],
    [if test ! -d "${with_qt_opengl_includes}"; then AC_MSG_ERROR(directory ${with_qt_opengl_includes} does not exist for --with-qt-opengl-includes); unset with_qt_opengl_includes; fi])

check_qt_opengl_include()
{
    if test -f "$1/QtOpenGL/QtOpenGL"; then
       found_qt_opengl_includes=$1
    elif test "$2" = "err"; then
       AC_MSG_ERROR([$1 is not a valid QT OpenGL parent include directory])
    fi
}

check_qt_opengl_lib()
{
    if test -f $1/libQtOpenGL.${SHLIB_SUFFIX}; then
       found_qt_opengl_libs=$lib
    elif test "$2" = "err"; then
    	AC_MSG_ERROR([$1 is not a valid QT OpenGL library directory])
    fi
}

check_qt_opengl()
{
    if test -z "$with_qt_opengl_includes"; then
        inc="$1/include"
        if test "$1" = "/"; then
       	    inc=/usr/include
        fi
	check_qt_opengl_include $inc

	if test -z "$found_qt_opengl_includes"; then
	    #echo DBG: qt $1 $inc includes not found
       	    return
    	fi
	with_qt_opengl_includes="$found_qt_opengl_includes"
	unset found_qt_opengl_includes
    fi

    if test -z "$with_qt_opengl_libs"; then
        lib="$1/lib${LIBSUFFIX}"

	check_qt_opengl_lib $lib
    	if test -z "$found_qt_opengl_libs"; then
       	    #echo DBG: qt $1 $lib libs not found
       	    unset with_qt_opengl_includes
       	    return
    	fi
	with_qt_opengl_libs="$found_qt_opengl_libs"
    fi
    if test "$with_qt_opengl_libs" != "$with_qt_libs"; then
        qt_app_opengl_libs=true
    fi
}

# see if we can figure out where the qt include files and libraries are
if test "${with_qt_gui}" = "yes" -a "${with_qt_opengl}" != "no"; then
   AC_MSG_CHECKING([for QT 4 OpenGL includes and libraries])

   if test -n "$with_qt_opengl_includes" -a -n "$with_qt_opengl_libs"; then
      check_qt_opengl_includes "$with_qt_opengl_includes" err
      check_qt_opengl_libs "$with_qt_opengl_libs" err
   else
      if test -n "$with_qt_opengl"; then
      	 check_qt_opengl "$with_qt_opengl"
      	 if test -z "$with_qt_opengl_includes"; then
	    AC_MSG_ERROR([not found in $with_qt_opengl])
      	 fi
      else
         # check for frameworks on darwin
	 if test -n "$darwin" -a -d /Library/Frameworks/QtOpenGL.framework; then
	    qt_opengl_framework="/Library/Frameworks/QtOpenGL.framework"
	 else
	    for dir in $QT_DIR / /usr /usr/local /opt/qt /opt/qt /usr/qt /usr/qt /sw /opt/local /opt/sfw /usr/sfw /opt/gnu; do
      	       check_qt_opengl $dir
      	       if test -n "$with_qt_opengl_includes"; then
	          break;
      	       fi
   	    done
         fi
      fi
   fi

   if test -n "$qt_opengl_framework"; then
      QT_OPENGL_INCLUDES="-I $qt_opengl_framework/Headers"
      QT_OPENGL_LIBS="-framework QtOpenGL"
      AC_MSG_RESULT([using Darwin frameworks in $qt_opengl_framework])
      with_qt_opengl=yes
   else
      if test -n "$with_qt_opengl_includes"; then
         if test -n "$qt_static"; then
	    AC_MSG_RESULT([includes=$with_qt_opengl_includes, libs=$with_qt_opengl_libs (static)])
      	 else
	    AC_MSG_RESULT([includes=$with_qt_opengl_includes, libs=$with_qt_opengl_libs (shared)])
      	 fi
      	 with_qt_opengl=yes
      else
	 AC_MSG_RESULT([not found])
         unset with_qt_opengl
      fi
   fi
fi

if test "${with_qt_opengl}" = yes; then
  if test -n "$with_qt_opengl_includes" -o -n "$qt_opengl_framework"; then
     if test -z "$QT_OPENGL_INCLUDES"; then
        QT_OPENGL_INCLUDES="-I$with_qt_includes/QtOpenGL"
     fi
     if test -z "$QT_OPENGL_LIBS"; then
        QT_OPENGL_LIBS="-lQtOpenGL"
        if test -n "$qt_app_opengl_libs"; then
           QT_OPENGL_LIBS="-L$with_qt_opengl_libs $QT_OPENGL_LIBS"
        fi
     fi

     # check if C++ compiler can link with QT opengl libs
     SAVE_CXXFLAGS="$CXXFLAGS"
     SAVE_LDFLAGS="$LDFLAGS"
     CXXFLAGS="$CXXFLAGS $QT_CORE_INCLUDES $QT_GUI_INCLUDES $QT_OPENGL_INCLUDES"
     LDFLAGS="$LDFLAGS $QT_CORE_LIBS $QT_GUI_LIBS $QT_OPENGL_LIBS"
     AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <QApplication>
#include <QPushButton>
#include <QGLWidget>
	 ]],[[
   int argc = 0;
   char **argv = 0;
   QApplication app(argc, argv);

   QPushButton hello("Hello world!");
   hello.resize(100, 30);

   QGLWidget test();

   hello.show();
   return app.exec();
	]])], qt_opengl_links=yes, qt_opengl_links=no)

     CXXFLAGS="$SAVE_CXXFLAGS"
     LDFLAGS="$SAVE_LDFLAGS"

     if test "$qt_opengl_links" = "no"; then
        AC_MSG_ERROR([could not compile a test QT OpenGL program])
     else
        AC_SUBST(QT_OPENGL_INCLUDES)
	AC_SUBST(QT_OPENGL_LIBS)
        with_qt_opengl=yes
     fi
  else
     AC_MSG_WARN([no QT OpenGL libraries or includes found, disabling QT OpenGL support])
     unset with_qt_opengl
  fi
fi

AC_ARG_WITH([qt-svg],
    [AS_HELP_STRING([--with-qt-svg@<:@=DIR@:>@],
                    [QT SVG base directory])],
    [if test ! -d "${with_qt_svg}"; then AC_MSG_ERROR(directory ${with_qt_svg} does not exist for --with-qt-svg); unset with_qt_svg; fi])

AC_ARG_WITH([qt-svg-libs],
    [AS_HELP_STRING([--with-qt-svg-libs@<:@=DIR@:>@],
                    [QT SVG library directory])],
    [if test ! -d "${with_qt_svg_libs}"; then AC_MSG_ERROR(directory ${with_qt_svg_libs} does not exist for --with-qt-svg-libs); unset with_qt_svg_libs; fi])

AC_ARG_WITH([qt-svg-includes],
    [AS_HELP_STRING([--with-qt-svg-includes@<:@=DIR@:>@],
                    [QT SVG include file parent directory])],
    [if test ! -d "${with_qt_svg_includes}"; then AC_MSG_ERROR(directory ${with_qt_svg_includes} does not exist for --with-qt-svg-includes); unset with_qt_svg_includes; fi])

check_qt_svg_include()
{
    if test -f "$1/QtSvg/QtSvg" -a -f "$1/QtXml/QtXml"; then
       found_qt_svg_includes=$1
    elif test "$2" = "err"; then
       AC_MSG_ERROR([$1 is not a valid QT SVG parent include directory])
    fi
}

check_qt_svg_lib()
{
    if test -f $1/libQtSvg.${SHLIB_SUFFIX} -a -f $1/libQtXml.${SHLIB_SUFFIX}; then
       found_qt_svg_libs=$lib
    elif test "$2" = "err"; then
    	AC_MSG_ERROR([$1 is not a valid QT Svg library directory])
    fi
}

check_qt_svg()
{
    if test -z "$with_qt_svg_includes"; then
        inc="$1/include"
        if test "$1" = "/"; then
       	    inc=/usr/include
        fi
	check_qt_svg_include $inc

	if test -z "$found_qt_svg_includes"; then
	    #echo DBG: qt $1 $inc includes not found
       	    return
    	fi
	with_qt_svg_includes="$found_qt_svg_includes"
	unset found_qt_svg_includes
    fi

    if test -z "$with_qt_svg_libs"; then
        lib="$1/lib${LIBSUFFIX}"

	check_qt_svg_lib $lib
    	if test -z "$found_qt_svg_libs"; then
       	    #echo DBG: qt $1 $lib libs not found
       	    unset with_qt_svg_includes
       	    return
    	fi
	with_qt_svg_libs="$found_qt_svg_libs"
    fi
    if test "$with_qt_svg_libs" != "$with_qt_libs"; then
        qt_app_svg_libs=true
    fi
}

# see if we can figure out where the qt include files and libraries are
if test "${with_qt_gui}" = "yes" -a "${with_qt_svg}" != "no"; then
   AC_MSG_CHECKING([for QT 4 SVG includes and libraries])

   if test -n "$with_qt_svg_includes" -a -n "$with_qt_svg_libs"; then
      check_qt_svg_includes "$with_qt_svg_includes" err
      check_qt_svg_libs "$with_qt_svg_libs" err
   else
      if test -n "$with_qt_svg"; then
      	 check_qt_svg "$with_qt_svg"
      	 if test -z "$with_qt_svg_includes"; then
	    AC_MSG_ERROR([not found in $with_qt_svg])
      	 fi
      else
         # check for frameworks on darwin
	 if test -n "$darwin" -a -d /Library/Frameworks/QtSvg.framework -a -d /Library/Frameworks/QtXml.framework; then
	    qt_svg_framework="/Library/Frameworks/QtSvg.framework"
	    qt_xml_framework="/Library/Frameworks/QtXml.framework"
	 else
	    for dir in $QT_DIR / /usr /usr/local /opt/qt /opt/qt /usr/qt /usr/qt /sw /opt/local /opt/sfw /usr/sfw /opt/gnu; do
      	       check_qt_svg $dir
      	       if test -n "$with_qt_svg_includes"; then
	          break;
      	       fi
   	    done
         fi
      fi
   fi

   if test -n "$qt_svg_framework"; then
      QT_SVG_INCLUDES="-I $qt_svg_framework/Headers -I $qt_xml_framework/Headers"
      QT_SVG_LIBS="-framework QtSvg -framework QtXml"
      AC_MSG_RESULT([using Darwin frameworks in $qt_svg_framework])
      with_qt_svg=yes
   else
      if test -n "$with_qt_svg_includes"; then
         if test -n "$qt_static"; then
	    AC_MSG_RESULT([includes=$with_qt_svg_includes, libs=$with_qt_svg_libs (static)])
      	 else
	    AC_MSG_RESULT([includes=$with_qt_svg_includes, libs=$with_qt_svg_libs (shared)])
      	 fi
      	 with_qt_svg=yes
      else
	 AC_MSG_RESULT([not found])
         unset with_qt_svg
      fi
   fi
fi

if test "${with_qt_svg}" = yes; then
  if test -n "$with_qt_svg_includes" -o -n "$qt_svg_framework"; then
     if test -z "$QT_SVG_INCLUDES"; then
        QT_SVG_INCLUDES="-I$with_qt_includes/QtSvg -I$with_qt_includes/QtXml"
     fi
     if test -z "$QT_SVG_LIBS"; then
        QT_SVG_LIBS="-lQtSvg -lQtXml"
        if test -n "$qt_app_svg_libs"; then
           QT_SVG_LIBS="-L$with_qt_svg_libs $QT_SVG_LIBS"
        fi
     fi

     # check if C++ compiler can link with QT svg libs
     SAVE_CXXFLAGS="$CXXFLAGS"
     SAVE_LDFLAGS="$LDFLAGS"
     CXXFLAGS="$CXXFLAGS $QT_CORE_INCLUDES $QT_GUI_INCLUDES $QT_SVG_INCLUDES"
     LDFLAGS="$LDFLAGS $QT_CORE_LIBS $QT_GUI_LIBS $QT_SVG_LIBS"
     AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <QApplication>
#include <QPushButton>
#include <QSvgWidget>
	 ]],[[
   int argc = 0;
   char **argv = 0;
   QApplication app(argc, argv);

   QPushButton hello("Hello world!");
   hello.resize(100, 30);

   QSvgWidget test();

   hello.show();
   return app.exec();
	]])], qt_svg_links=yes, qt_svg_links=no)

     CXXFLAGS="$SAVE_CXXFLAGS"
     LDFLAGS="$SAVE_LDFLAGS"

     if test "$qt_svg_links" = "no"; then
        AC_MSG_ERROR([could not compile a test QT SVG program])
     else
        AC_SUBST(QT_SVG_INCLUDES)
	AC_SUBST(QT_SVG_LIBS)
        with_qt_svg=yes
     fi
  else
     AC_MSG_WARN([no QT SVG libraries or includes found, disabling QT SVG support])
     unset with_qt_svg
  fi
fi

AC_ARG_WITH([doxygen],
    [AS_HELP_STRING([--with-doxygen@<:@=PATH@:>@],
                    [path to doxygen binary])],
    [if test "${with_doxygen}" != "no" -a ! -x "${with_doxygen}"; then AC_MSG_ERROR(${with_doxygen} is not an executable in --with-doxygen); unset with_doxygen; fi ],
    [with_doxygen=auto])

if test "${with_doxygen}" = "auto"; then
   AC_MSG_CHECKING([for doxygen to build API documentation])
   # check for doxygen
   with_doxygen=`which doxygen`
   if test -n "$with_doxygen"; then
      doxy_version=`$with_doxygen --version`
      AC_MSG_RESULT([found $with_doxygen $doxy_version])
   else
      AC_MSG_RESULT([not found])
   fi
fi

if test -n "${with_doxygen}"; then
   AC_SUBST(DOXYGEN_CMD, $with_doxygen)
fi

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h inttypes.h netdb.h netinet/in.h stddef.h stdlib.h string.h strings.h sys/socket.h sys/time.h unistd.h])

# check for umem.h
AC_CHECK_HEADER([umem.h], have_umem_h=yes, have_umem_h=no)
if test "$have_umem_h" = "yes"; then
   AC_DEFINE(HAVE_UMEM_H, 1, [define if umem.h is available])
   OTHER_LIBS="$OTHER_LIBS -lumem"
fi

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
AC_CHECK_FUNCS([bzero floor gethostbyaddr gethostbyaddr_r gethostbyname gethostbyname_r gethostname gettimeofday memmove memset mkfifo putenv regcomp select socket setsockopt getsockopt strcasecmp strchr strdup strerror strspn strstr atoll strtol strtoll isblank localtime_r gmtime_r exp2 clock_gettime realloc timegm seteuid setegid setenv unsetenv])

# try to find strtoimax on HPUX
AC_MSG_CHECKING([for strtoimax])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
[[#include <inttypes.h>]], [[strtoimax("10", NULL, 10);]])], 
   [AC_MSG_RESULT(yes)
    AC_DEFINE(HAVE_STRTOIMAX, 1, [Define to 1 if you have strtoimax])], 
   [AC_MSG_RESULT(no)])

# Check definition of gethostbyaddr_r (glibc2 defines this with 8 arguments)
ac_save_CXXFLAGS="$CXXFLAGS"
AC_CACHE_CHECK([style of gethost* routines], qore_cv_gethost_style,
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
qore_cv_gethost_style=solaris, qore_cv_gethost_style=other))
AC_LANG_POP([C++])
CXXFLAGS="$ac_save_CXXFLAGS"
if test "$qore_cv_gethost_style" = "solaris"
then
  AC_DEFINE([HAVE_SOLARIS_STYLE_GETHOST], [1],
            [Solaris defines gethostbyaddr_r with 7 arguments. glibc2 defines this with 8 arguments])
fi

# Check definition of gethostbyname_r (glibc2.0.100 is different from Solaris)
ac_save_CXXFLAGS="$CXXFLAGS"
AC_CACHE_CHECK([style of gethostname_r routines], qore_cv_gethostname_style,
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
qore_cv_gethostname_style=glibc2, qore_cv_gethostname_style=other))
AC_LANG_POP([C++])
CXXFLAGS="$ac_save_CXXFLAGS"
if test "$qore_cv_gethostname_style" = "glibc2"
then
  AC_DEFINE([HAVE_GETHOSTBYNAME_R_GLIBC2_STYLE], [1],
            [Solaris define gethostbyname_r with 5 arguments. glibc2 defines this with 6 arguments])
fi

# Check 3rd argument of getthostbyname_r
ac_save_CXXFLAGS="$CXXFLAGS"
AC_CACHE_CHECK([3 argument to gethostname_r routines], qore_cv_gethostname_arg,
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
qore_cv_gethostname_arg=hostent_data, qore_cv_gethostname_arg=char))
AC_LANG_POP([C++])
CXXFLAGS="$ac_save_CXXFLAGS"
if test "$qore_cv_gethostname_arg" = "hostent_data"
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
        # remove -g with Solaris CC as it reduces optimization levels
	CXXFLAGS="`echo $CXXFLAGS| sed s/-g//` -xO5" # Solaris CC
    elif test -n "`echo $CXX|grep aCC`"; then 
        # remove -g with HP-UX aCC as it reduces optimization levels
	CXXFLAGS="`echo $CXXFLAGS| sed s/-g//` +O4" # HP-UX aCC
    fi
else
    if test "$GXX" = "yes"; then
        CXXFLAGS="`echo $CXXFLAGS| sed s/-O2//`"  # gcc
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

AC_SUBST(OTHER_LIBS)

# set flag for monolithic builds if necessary
if test "$enable_shared" = "no" -o "$enable_builtin_modules" = "yes"; then
   AC_DEFINE(QORE_MONOLITHIC, 1, Define to 1 if all modules should be statically linked into the libqore.a library)
fi

# save more version information in config.h
VERSION_MAJOR=`echo $PACKAGE_VERSION | cut -f3 -d\  | sed s/\"//g | cut -f1 -d.`
VERSION_MINOR=`echo $PACKAGE_VERSION | cut -f3 -d\  | sed s/\"//g | cut -f2 -d.`
VERSION_SUB=`echo $PACKAGE_VERSION | cut -f3 -d\  | sed s/\"//g | cut -f3 -d.`

# set version information
AC_DEFINE_UNQUOTED([VERSION_MAJOR], $VERSION_MAJOR, [major version number])
AC_DEFINE_UNQUOTED([VERSION_MINOR], $VERSION_MINOR, [minor version number])
AC_DEFINE_UNQUOTED([VERSION_SUB], $VERSION_SUB, [sub version number])
AC_DEFINE_UNQUOTED([TARGET_ARCH], "$ARCH", [host type])
AC_DEFINE_UNQUOTED([TARGET_OS], "$OS", [host type])
AC_DEFINE_UNQUOTED([TARGET_BITS], $bits, [32 or 64 bit build])

AM_CONDITIONAL([COND_NCURSES],    [test "$with_ncurses" = yes])
AM_CONDITIONAL([COND_DEBUG],      [test "$enable_debug" = yes])
AM_CONDITIONAL([COND_PROFILE],    [test "$enable_profile" = yes])
AM_CONDITIONAL([COND_ORACLE],     [test "$with_oracle" = yes])
AM_CONDITIONAL([COND_MYSQL],      [test "$with_mysql" = yes])
AM_CONDITIONAL([COND_SYBASE],     [test "$with_sybase" = yes])
AM_CONDITIONAL([COND_FREETDS],    [test "$with_freetds" = yes])
AM_CONDITIONAL([COND_SYBASE_DIR], [test "$with_sybase" = yes -o "$with_freetds" = yes]) 
AM_CONDITIONAL([COND_PGSQL],      [test "$with_pgsql" = yes])
AM_CONDITIONAL([COND_TIBRV],      [test "$with_tibrv" = yes])
AM_CONDITIONAL([COND_TIBAE],      [test "$with_tibae" = yes])
AM_CONDITIONAL([COND_TUXEDO],     [test "$with_tuxedo" = yes])
AM_CONDITIONAL([COND_QT_CORE],    [test "$with_qt" = yes])
AM_CONDITIONAL([COND_QT_GUI],     [test "$with_qt_gui" = yes])
AM_CONDITIONAL([COND_QT_OPENGL],  [test "$with_qt_opengl" = yes])
AM_CONDITIONAL([COND_QT_SVG],     [test "$with_qt_svg" = yes])
AM_CONDITIONAL([COND_OPENGL],     [test "$with_opengl" = yes])
AM_CONDITIONAL([COND_GLUT],       [test "$with_glut" = yes])
AM_CONDITIONAL([COND_MONOLITHIC], [test "$enable_shared" = no -o "$enable_builtin_modules" = "yes"])
AM_CONDITIONAL([COND_SINGLE_COMPILATION_UNIT], [test "$enable_single_compilation_unit" = yes])
AM_CONDITIONAL([COND_SINGLE_COMPILATION_UNIT_QT], [test "$enable_single_compilation_unit_qt" = yes])
AM_CONDITIONAL([COND_MACOSX],     [test "$darwin" = true])
AM_CONDITIONAL([COND_DOXYGEN],    [test -n "$with_doxygen"])

AC_CONFIG_FILES([Makefile lib/Makefile modules/Makefile modules/ncurses/Makefile modules/tibrv/Makefile modules/tibae/Makefile modules/mysql/Makefile modules/oracle/Makefile modules/Tuxedo/Makefile modules/sybase/Makefile modules/pgsql/Makefile modules/qt-core/Makefile modules/qt-gui/Makefile modules/qt-opengl/Makefile modules/qt-svg/Makefile modules/opengl/Makefile modules/glut/Makefile])
AC_OUTPUT

echo "*** BUILTIN FEATURES ***"
printf "%-24s: %s %s" library "$OS" "$cpu_family"
if test "$enable_64bit" = "yes"; then
   printf " (64-bit)\n"
else
   printf " (32-bit)\n"
fi

show_library_feature()
{
   printf "%-24s: " "$1"
   if test "$2" = "yes"; then
      printf "%-8s" yes
   else
      printf disabled
   fi
   if test -n "$3"; then
      printf " (%s)" $3
   fi
   echo
}

show_library_feature optimizations $enable_optimization
show_library_feature "single compilation unit" $enable_single_compilation_unit
show_library_feature debug $enable_debug
show_library_feature profiling $enable_profile

if test "$enable_shared" = "yes"; then
   echo "*** MODULES ***"
fi

show_library_feature ncurses $with_ncurses
show_library_feature oracle $with_oracle
show_library_feature sybase $with_sybase
show_library_feature mysql $with_mysql
show_library_feature pgsql $with_pgsql
show_library_feature mssql $with_freetds
show_library_feature tibrv $with_tibrv
show_library_feature tibae $with_tibae
show_library_feature tuxedo $with_tuxedo
show_library_feature qt-core $with_qt
show_library_feature qt-gui $with_qt_gui scu=$enable_single_compilation_unit_qt
show_library_feature qt-opengl $with_qt_opengl
show_library_feature qt-svg $with_qt_svg
show_library_feature opengl $with_opengl
show_library_feature glut $with_glut

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

