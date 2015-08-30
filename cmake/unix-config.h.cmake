

/* headers */

#cmakedefine HAVE_FCNTL_H
#cmakedefine HAVE_INTTYPES_H
#cmakedefine HAVE_NETDB_H
#cmakedefine HAVE_NETINET_IN_H
#cmakedefine HAVE_STDDEF_H
#cmakedefine HAVE_STDLIB_H
#cmakedefine HAVE_STRING_H
#cmakedefine HAVE_STRINGS_H
#cmakedefine HAVE_SYS_SOCKET_H
#cmakedefine HAVE_SYS_TIME_H
#cmakedefine HAVE_UNISTD_H
#cmakedefine HAVE_EXECINFO_H
#cmakedefine HAVE_CXXABI_H
#cmakedefine HAVE_ARPA_INET_H
#cmakedefine HAVE_SYS_SOCKET_H
#cmakedefine HAVE_SYS_STATVFS_H
#cmakedefine HAVE_WINSOCK2_H
#cmakedefine HAVE_WS2TCPIP_H
#cmakedefine HAVE_GLOB_H
#cmakedefine HAVE_SYS_UN_H
#cmakedefine HAVE_TERMIOS_H
#cmakedefine HAVE_NETINET_TCP_H
#cmakedefine HAVE_PWD_H
#cmakedefine HAVE_SYS_WAIT_H
#cmakedefine HAVE_GETOPT_H
#cmakedefine HAVE_STDINT_H
#cmakedefine HAVE_GRP_H


/* functions */
#cmakedefine HAVE_BZERO
#cmakedefine HAVE_FLOOR
#cmakedefine HAVE_GETHOSTBYADDR
#cmakedefine HAVE_GETHOSTBYNAME
#cmakedefine HAVE_GETHOSTNAME
#cmakedefine HAVE_GETTIMEOFDAY
#cmakedefine HAVE_MEMMOVE
#cmakedefine HAVE_MEMSET
#cmakedefine HAVE_MKFIFO
#cmakedefine HAVE_PUTENV
#cmakedefine HAVE_REGCOMP
#cmakedefine HAVE_SELECT
#cmakedefine HAVE_SOCKET
#cmakedefine HAVE_SETSOCKOPT
#cmakedefine HAVE_GETSOCKOPT
#cmakedefine HAVE_STRCASECMP
#cmakedefine HAVE_STRCHR
#cmakedefine HAVE_STRDUP
#cmakedefine HAVE_STRERROR
#cmakedefine HAVE_STRSPN
#cmakedefine HAVE_STRSTR
#cmakedefine HAVE_ATOLL
#cmakedefine HAVE_STRTOL
#cmakedefine HAVE_STRTOLL
#cmakedefine HAVE_ISBLANK
#cmakedefine HAVE_LOCALTIME_R
#cmakedefine HAVE_GMTIME_R
#cmakedefine HAVE_EXP2
#cmakedefine HAVE_CLOCK_GETTIME
#cmakedefine HAVE_REALLOC
#cmakedefine HAVE_TIMEGM
#cmakedefine HAVE_SETEUID
#cmakedefine HAVE_SETEGID
#cmakedefine HAVE_SETENV
#cmakedefine HAVE_UNSETENV
#cmakedefine HAVE_ROUND
#cmakedefine HAVE_PTHREAD_ATTR_GETSTACKSIZE
#cmakedefine HAVE_GETPWUID_R
#cmakedefine HAVE_GETPWNAM_R
#cmakedefine HAVE_GETGRGID_R
#cmakedefine HAVE_GETGRNAM_R
#cmakedefine HAVE_BACKTRACE
#cmakedefine HAVE_GLOB
#cmakedefine HAVE_SYSTEM
#cmakedefine HAVE_INET_NTOP
#cmakedefine HAVE_INET_PTON
#cmakedefine HAVE_LSTAT
#cmakedefine HAVE_FSYNC
#cmakedefine HAVE_LCHOWN
#cmakedefine HAVE_CHOWN
#cmakedefine HAVE_SETSID
#cmakedefine HAVE_SETUID
#cmakedefine HAVE_MKFIFO
#cmakedefine HAVE_RANDOM
#cmakedefine HAVE_KILL
#cmakedefine HAVE_GETPPID
#cmakedefine HAVE_GETGID
#cmakedefine HAVE_GETEGID
#cmakedefine HAVE_GETUID
#cmakedefine HAVE_GETEUID
#cmakedefine HAVE_SETUID
#cmakedefine HAVE_SETEUID
#cmakedefine HAVE_SETGID
#cmakedefine HAVE_SETEGID
#cmakedefine HAVE_SLEEP
#cmakedefine HAVE_USLEEP
#cmakedefine HAVE_NANOSLEEP
#cmakedefine HAVE_READLINK
#cmakedefine HAVE_SYMLINK
#cmakedefine HAVE_ACCESS
#cmakedefine HAVE_STRCASESTR
#cmakedefine HAVE_STRNCASECMP
#cmakedefine HAVE_SETGROUPS
#cmakedefine HAVE_GETGROUPS
#cmakedefine HAVE_REALPATH
#cmakedefine HAVE_MEMMEM
#cmakedefine HAVE_GETHOSTBYADDR_R
#cmakedefine HAVE_GETHOSTBYNAME_R
#cmakedefine HAVE_STRTOIMAX
#cmakedefine HAVE_STRERROR_R
#cmakedefine STRERROR_R_CHAR_P

/* OpenSSL */

#cmakedefine HAVE_OPENSSL_SHA512
#cmakedefine HAVE_OPENSSL_CONST
#cmakedefine NEED_SSL_CTX_NEW_CONST
#cmakedefine HAVE_OPENSSL_HMAC_RV

/* mpfr */
#cmakedefine HAVE_MPFR_DIVBY0
#cmakedefine HAVE_MPFR_REGULAR
#cmakedefine HAVE_MPFR_SPRINTF
#cmakedefine HAVE_MPFR_BUILDOPT_TLS_P
#cmakedefine HAVE_MPFR_EXP_T
#cmakedefine HAVE_RNDN

/* gethost* */
#cmakedefine HAVE_SOLARIS_STYLE_GETHOST
#cmakedefine HAVE_GETHOSTBYNAME_R_GLIBC2_STYLE
#cmakedefine HAVE_GETHOSTBYNAME_R_RETURN_INT

/* stl hash_map and list */
#cmakedefine HAVE_EXT_HASH_MAP
#cmakedefine HAVE_EXT_HASH_SET
#cmakedefine HAVE_EXT_SLIST
#cmakedefine HAVE_HASH_MAP
#cmakedefine HAVE_HASH_SET
#cmakedefine HAVE_SLIST
#cmakedefine HAVE_QORE_HASH_MAP
#cmakedefine HAVE_QORE_SLIST
#cmakedefine HAVE_UNORDERED_MAP

/* the rest */
#cmakedefine HAVE_LOCAL_VARIADIC_ARRAYS
#cmakedefine HAVE_NAMESPACES
#cmakedefine HAVE_STRUCT_FLOCK
#cmakedefine HAVE_GCC_VISIBILITY
#cmakedefine HAVE_SIGNAL_HANDLING

#define PACKAGE_VERSION "@VERSION_MAJOR@.@VERSION_MINOR@.@VERSION_SUB@"
#define VERSION "@VERSION_MAJOR@.@VERSION_MINOR@.@VERSION_SUB@"
#define VERSION_MAJOR @VERSION_MAJOR@
#define VERSION_MINOR @VERSION_MINOR@
#define VERSION_SUB @VERSION_SUB@


#define TARGET_OS "@CMAKE_SYSTEM_NAME@"
#define TARGET_ARCH "@CMAKE_SYSTEM_PROCESSOR@"
#define TARGET_BITS @TARGET_BITS@
#define QORE_BUILD_HOST "@QORE_BUILD_HOST@"

#define QORE_LIB_CXX "@CMAKE_CXX_COMPILER_ID@"

/* to be set later */
#define QORE_LIB_CFLAGS ""
#define QORE_LIB_LDFLAGS ""
#define MODULE_DIR "@MODULE_DIR@"
#define MODULE_VER_DIR "@MODULE_VER_DIR@"
#define USER_MODULE_DIR "@USER_MODULE_DIR@"
#define USER_MODULE_VER_DIR "@USER_MODULE_VER_DIR@"
