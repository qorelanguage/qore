## iconv.m4  -*- Autoconf -*-
##
## Written by Keith Marshall <keithmarshall@users.sourceforge.net>
##
## First Issue: 02-Sep-2006.
## Last Update: 04-Apr-2007, by Keith Marshall
## Public Domain.
##
##
## This file provides a lightweight alternative to Bruno Haible's
## autoconfigury, as provided with GNU libiconv.  This implementation
## avoids many of the dependencies introduced by Bruno's code.
##
## This file is provided `as is', in the hope that it may be useful,
## but WITHOUT WARRANTY OF ANY KIND, not even any implied warranty of
## MERCHANTABILITY, nor of FITNESS FOR ANY PARTICULAR PURPOSE.
##
##

# LIBICONV_AC_HEADER_ICONV
# ------------------------
# Check for existence and usability of an iconv.h header file,
# searching in any directory specified using `--with-libiconv-prefix',
# or if unspecified, in the directory specified by `--includedir',
# or otherwise the default compiler search path; set the AC_SUBST
# variable `INCICONV' to the appropriate compiler include flag,
# to identify this header file location.
#
AC_DEFUN([LIBICONV_AC_HEADER_ICONV],
[AC_REQUIRE([AC_CHECK_HEADERS])dnl
 AC_REQUIRE([ICONV_AC_LIBICONV_PREFIX])dnl
 AC_VAR_PUSHVAL([CPPFLAGS], ["$ac_cv_iconv_include $CPPFLAGS"])
 AC_CHECK_HEADERS([iconv.h])
 AC_VAR_POPVAL([CPPFLAGS])
 LIBICONV_AC_HEADER_INCICONV[]dnl
])# LIBICONV_AC_HEADER_ICONV

# LIBICONV_AC_FUNC_ICONV
# ----------------------
# Check if the `iconv' function is available,
# whether its prototype requires `const' in its declaration,
# and if it requires any compiler options for linking; set the
# AC_SUBST variable `LIBICONV' to any such compiler options,
# and define the `ICONV_CONST' preprocessor macro.
#
AC_DEFUN([LIBICONV_AC_FUNC_ICONV],
[AC_REQUIRE([LIBICONV_AC_HEADER_ICONV])dnl
 AC_REQUIRE([LIBICONV_AC_ICONV_CONST])dnl
 AC_MSG_CHECKING([for iconv])
 LIBICONV_AC_FUNC_ICONV_TRY_LINK([$ac_cv_iconv_include], [$ac_cv_libiconv])
  if test "$ac_val" = no
  then
    test -z "$ac_cv_libiconv" && ac_cv_libiconv="-liconv" || ac_cv_libiconv=""
    LIBICONV_AC_FUNC_ICONV_TRY_LINK([$ac_cv_iconv_include], [$ac_cv_libiconv])
  fi
 AC_MSG_RESULT([$ac_val])
  func_iconv=$ac_val
 AC_MSG_CHECKING([for $CC options to link with iconv])
  test "$func_iconv" = no && ac_cv_libiconv=""
  test -n "$ac_cv_libiconv" && ac_val=$ac_cv_libiconv || ac_val="none needed"
 AC_SUBST([LIBICONV],[$ac_cv_libiconv])
 AC_MSG_RESULT([$ac_val])
  if test "$func_iconv" = no
  then
    AC_MSG_WARN([])
    AC_MSG_WARN([no usable iconv function available])
    AC_MSG_WARN([])
    AC_MSG_WARN([This package requires a working iconv implementation;])
    AC_MSG_WARN([you should consider installing GNU libiconv.])
    AC_MSG_WARN([])
  fi[]dnl
])# LIBICONV_AC_FUNC_ICONV

# ICONV_AC_LIBICONV_PREFIX
# ------------------------
# Allow the user to specify `--with-libiconv-prefix=DIR';
# if he does so, and `DIR/include/iconv.h' is a readable file, then
# establish include path and lib path settings, so that libiconv
# from that path will be preferred to any system default.
#
AC_DEFUN([ICONV_AC_LIBICONV_PREFIX],
[AC_ARG_WITH([libiconv-prefix],
 [ICONV_AC_LIBICONV_PREFIX_HELP],
 [ICONV_AC_LIBICONV_PREFIX_PREFERRED],
 [ICONV_AC_LIBICONV_PREFIX_DEFAULT])dnl
])# ICONV_AC_LIBICONV_PREFIX

# LIBICONV_AC_HEADER_INCICONV
# ---------------------------
# Local helper macro, invoked by LIBICONV_AC_HEADER_ICONV,
# to set the AC_SUBST variable, INCICONV, to an appropriate value.
#
AC_DEFUN([LIBICONV_AC_HEADER_INCICONV],
[AC_MSG_CHECKING([for $CC option to locate iconv.h])
  if test -n "$ac_cv_iconv_include" && test "$ac_cv_header_iconv_h" = yes \
  && test "$iconv_include_default" = yes
  then
    AC_COMPILE_IFELSE([#include <iconv.h>],[ac_cv_iconv_include=""])
  fi
  if test -z "$ac_cv_iconv_include"
  then
    AC_MSG_RESULT([none needed])
  else
    AC_MSG_RESULT([$ac_cv_iconv_include])
set -x
    if test -z "$ac_dir"
    then
      AC_MSG_WARN([cannot find iconv.h in specified path])
      ac_cv_iconv_include=""
      ac_cv_libiconv=""
    fi
set +x
  fi
 AC_SUBST([INCICONV],[$ac_cv_iconv_include])dnl
])# LIBICONV_AC_HEADER_INCICONV

# ICONV_AC_LIBICONV_PREFIX_HELP
# -----------------------------
# Specify the `configure --help' text for `--with-libiconv-prefix'.
#
AC_DEFUN([ICONV_AC_LIBICONV_PREFIX_HELP], [dnl
AS_HELP_STRING([--with-libiconv-prefix@<:@=DIR@:>@],
 [search for libiconv in DIR/include and DIR/lib;])
AS_HELP_STRING([],
 [if DIR not specified, force search in INCLUDEDIR and LIBDIR])
AS_HELP_STRING([--without-libiconv-prefix],
 [don't search for libiconv in INCLUDEDIR and LIBDIR])dnl
])# ICONV_AC_LIBICONV_PREFIX_HELP

# ICONV_AC_LIBICONV_PREFIX_PREFERRED
# ----------------------------------
# Invoked when user specifies `--with-libiconv-prefix=DIR',
# sets up the appropriate cache variables to locate iconv in DIR.
# Alternatively, if user specifies `--without-libiconv-prefix',
# disable searching for iconv in `libdir' and `includedir'.
#
AC_DEFUN([ICONV_AC_LIBICONV_PREFIX_PREFERRED],
[dnl
  iconv_include_default=no
  if test x$withval = xno
  then
    ac_cv_iconv_include=""
    ac_cv_libiconv=""
  else
    ICONV_AC_LIBICONV_SEARCH([ac_dir], [include/iconv.h], ["$withval"])
    if test -n "$ac_dir"
    then
      ac_cv_iconv_include="-I$ac_dir/include"
      ac_cv_libiconv="-L$ac_dir/lib -liconv"
    else
      ac_cv_iconv_include="-I$withval/include"
      ac_cv_libiconv="-I$withval/lib -liconv"
    fi
  fi[]dnl
])# ICONV_AC_LIBICONV_PREFIX_PREFERRED

# ICONV_AC_LIBICONV_PREFIX_DEFAULT
# --------------------------------
# Invoked if user does not specify `--with-libiconv-prefix'.
# Check if libiconv has been installed to `includedir' and `libdir';
# if yes, assume that user will prefer this implementation over
# any system supplied default.
#
AC_DEFUN([ICONV_AC_LIBICONV_PREFIX_DEFAULT],
[AC_CACHE_VAL([ac_cv_iconv_include],
 [ICONV_AC_LIBICONV_SEARCH([ac_dir], [iconv.h], ["$includedir"])
  if test -n "$ac_dir"
  then
    iconv_include_default=yes
    ac_cv_iconv_include="-I$ac_dir"
    ac_cv_iconv_lib="-L@&t@AC_PREFIX_RESOLVE([echo $libdir])"
  fi[]dnl
 ])dnl
])# ICONV_AC_LIBICONV_PREFIX_DEFAULT

# ICONV_AC_LIBICONV_SEARCH( VARNAME, FILEPATH, SEARCHPATHLIST )
# -------------------------------------------------------------
# Used to search from each root path specified in SEARCHPATHLIST,
# for a file name matching FILEPATH, (which may include a relative
# path component).  Set the variable specified by VARNAME to the
# first matching entry in SEARCHPATHLIST, which results in a
# successful file name match.
#
AC_DEFUN([ICONV_AC_LIBICONV_SEARCH],
[$1=AC_PREFIX_RESOLVE([dnl
  for ac_dir in $3
  do
    eval ac_dir="'$ac_dir'"
    test -r "$ac_dir/$2" && echo "$ac_dir" && exit
  done])dnl
])# ICONV_AC_LIBICONV_SEARCH

# LIBICONV_AC_FUNC_ICONV_TRY_LINK( INCLUDES, LIBNAME )
# ----------------------------------------------------
# Check if the specified INCLUDES and LIBNAME allow us to link
# a simple iconv dependent application.
#
AC_DEFUN([LIBICONV_AC_FUNC_ICONV_TRY_LINK],
[AC_VAR_PUSHVAL([CPPFLAGS], [`echo $1 $CPPFLAGS`])
 AC_VAR_PUSHVAL([LIBS], [`echo $2 $LIBS`])
 AC_LANG_PUSH(C)
 AC_LINK_IFELSE(
  [AC_LANG_PROGRAM(
   [
#include <iconv.h>
#define ac_NULL (void *)(0)
   ],[
iconv_t cd = iconv_open( "", "" );
iconv( cd, ac_NULL, ac_NULL, ac_NULL, ac_NULL );
iconv_close( cd );
   ])
  ],
  [ac_val=yes],
  [ac_val=no])
 AC_LANG_POP(C)
 AC_VAR_POPVAL([CPPFLAGS]) AC_VAR_POPVAL([LIBS])dnl
])# LIBICONV_AC_FUNC_ICONV_TRY_LINK

# LIBICONV_AC_ICONV_CONST
# -----------------------
# Check if the declaration for the iconv function requires the
# const qualifier on its input string argument, and define the
# `ICONV_CONST' preprocessor macro accordingly.
#
AC_DEFUN([LIBICONV_AC_ICONV_CONST],
[AC_CACHE_CHECK([whether iconv declaration requires const],
 [ac_cv_iconv_const],
 [AC_LANG_PUSH(C)
  AC_VAR_PUSHVAL([CPPFLAGS], [$ac_cv_iconv_include])
  AC_COMPILE_IFELSE(
  [AC_LANG_PROGRAM(
   [
#include <iconv.h>
extern size_t iconv(iconv_t, const char**, size_t*, char**, size_t*);
   ],[])
  ],
  [ac_cv_iconv_const=yes],
  [ac_cv_iconv_const=no])
  AC_VAR_POPVAL([CPPFLAGS])
  AC_LANG_POP(C)dnl
 ])
 test x$ac_cv_iconv_const = xyes && ICONV_CONST=const || ICONV_CONST=""
 AC_DEFINE_UNQUOTED([ICONV_CONST],[$ICONV_CONST],
 [Define to `const' if iconv declaration requires const.])
])# LIBICONV_AC_ICONV_CONST

##
## The following macros are general purpose, and in no way associated
## specifically with `libiconv'; they are reproduced here, in order to
## keep `iconv.m4' self-contained.
##

# AC_VAR_PUSHVAL( VARIABLE, VALUE )
# ---------------------------------
# Save a backup copy of VARIABLE, and then assign VARIABLE=VALUE.
#
AC_DEFUN([AC_VAR_PUSHVAL],
[ac_popval_$1=${$1} $1=[$2]dnl
])# AC_VAR_PUSHVAL

# AC_VAR_POPVAL( VARIABLE )
# -------------------------
# Restore VARIABLE to the value saved by the most recently
# preceding expansion of AC_VAR_PUSHVAL.
#
AC_DEFUN([AC_VAR_POPVAL],
[$1=${ac_popval_$1}dnl
])# AC_VAR_POPVAL

# AC_PREFIX_RESOLVE( COMMAND )
# ----------------------------
# Invoke COMMAND as a back-quoted expression, with ${prefix} set to
# its ultimate value, as it will be at `config.status' invocation.
#
AC_DEFUN([AC_PREFIX_RESOLVE],
[`test "$prefix" = NONE && prefix="$ac_default_prefix"
  $1`dnl
])# AC_PREFIX_RESOLVE

## iconv.m4: $RCSfile$: end of file: vim: ft=config
