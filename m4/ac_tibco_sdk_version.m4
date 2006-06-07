
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
