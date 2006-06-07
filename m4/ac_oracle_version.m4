

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
