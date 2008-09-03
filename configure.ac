# Process this file with autoconf to produce a configure script.

# AC_PREREQ(2.59)
AC_INIT([qore], [0.7.0],
        [David Nichols <nicholsman@gmail.com>],
        [qore])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADER(include/qore/intern/unix-config.h)

# fix default include dir
DEFAULT_INCLUDES="-I\${top_builddir}/include/qore"
AC_SUBST([DEFAULT_INCLUDES])

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

AC_ARG_ENABLE([static-common],
     [AS_HELP_STRING([--enable-static-common],
		     [force static linkage of openssl, libxml2, and pcre libraries (default=auto)])],
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

if test "$enable_static_common" = "yes"; then
   openssl_static=yes
   libxml2_static=yes
   pcre_static=yes
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
    LIBXML2_INC_DIR=$1
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
if test -n "$LIBXML2_LDFLAGS"; then
   # check for xmlTextReaderSetSchema()
   AC_MSG_CHECKING([for xmlTextReaderSetSchema()])
   file=`grep xmlTextReaderSetSchema $LIBXML2_INC_DIR/libxml/*.h`
   if test -n "$file"; then
      AC_DEFINE(HAVE_XMLTEXTREADERSETSCHEMA, 1, [Define if libxml2 defines xmlTextReaderSetSchema])
      AC_MSG_RESULT([found])
   else
      AC_MSG_RESULT([not found])
   fi
else
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

# checks for how to build threads
ACX_PTHREAD
if test "$acx_pthread_ok" = "no"; then
   AC_MSG_ERROR(POSIX threads do not seem to be supported on this platform, aborting)
fi
CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"

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

AC_ARG_WITH([doxygen],
    [AS_HELP_STRING([--with-doxygen@<:@=PATH@:>@],
                    [path to doxygen binary])],
    [if test "${with_doxygen}" != "no" -a ! -x "${with_doxygen}"; then AC_MSG_ERROR(${with_doxygen} is not an executable in --with-doxygen); unset with_doxygen; fi ],
    [with_doxygen=auto])

if test "${with_doxygen}" = "auto"; then
   AC_MSG_CHECKING([for doxygen to build API documentation])
   # check for doxygen
   with_doxygen=`which doxygen 2>/dev/null`
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

# get module API version to set pkglibdir
MODULE_API_MAJOR=`grep QORE_MODULE_API_MAJOR include/qore/ModuleManager.h|cut -f3 -d\ `
MODULE_API_MINOR=`grep QORE_MODULE_API_MINOR include/qore/ModuleManager.h|cut -f3 -d\ `
mypkglibdir=qore-module-api-$MODULE_API_MAJOR.$MODULE_API_MINOR
AC_SUBST([MODULE_API_MAJOR])
AC_SUBST([MODULE_API_MINOR])
AC_SUBST([mypkglibdir])

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

AM_CONDITIONAL([COND_DEBUG],      [test "$enable_debug" = yes])
AM_CONDITIONAL([COND_PROFILE],    [test "$enable_profile" = yes])
AM_CONDITIONAL([COND_ORACLE],     [test "$with_oracle" = yes])
AM_CONDITIONAL([COND_MYSQL],      [test "$with_mysql" = yes])
AM_CONDITIONAL([COND_SYBASE],     [test "$with_sybase" = yes])
AM_CONDITIONAL([COND_FREETDS],    [test "$with_freetds" = yes])
AM_CONDITIONAL([COND_SYBASE_DIR], [test "$with_sybase" = yes -o "$with_freetds" = yes]) 
AM_CONDITIONAL([COND_PGSQL],      [test "$with_pgsql" = yes])
AM_CONDITIONAL([COND_MONOLITHIC], [test "$enable_shared" = no -o "$enable_builtin_modules" = "yes"])
AM_CONDITIONAL([COND_SINGLE_COMPILATION_UNIT], [test "$enable_single_compilation_unit" = yes])
AM_CONDITIONAL([COND_MACOSX],     [test "$darwin" = true])
AM_CONDITIONAL([COND_DOXYGEN],    [test -n "$with_doxygen"])

AC_CONFIG_FILES([Makefile lib/Makefile modules/Makefile modules/mysql/Makefile modules/oracle/Makefile modules/sybase/Makefile modules/pgsql/Makefile])
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

show_library_feature oracle $with_oracle
show_library_feature sybase $with_sybase
show_library_feature mysql $with_mysql
show_library_feature pgsql $with_pgsql
show_library_feature mssql $with_freetds

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

