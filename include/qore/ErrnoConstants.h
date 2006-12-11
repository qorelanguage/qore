/*
  ErrnoConstants.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _QORE_ERRNOCONSTANTS_H

#define _QORE_ERRNOCONSTANTS_H

// this function will set up the Qore system namespace
static inline class Namespace *get_errno_ns()
{
   tracein("get_errno_ns()");

   class Namespace *Err = new Namespace("Err");

#ifdef EPERM
   Err->addConstant("EPERM", new QoreNode((int64)EPERM));
#endif
#ifdef ENOENT
   Err->addConstant("ENOENT", new QoreNode((int64)ENOENT));
#endif
#ifdef ESRCH
   Err->addConstant("ESRCH", new QoreNode((int64)ESRCH));
#endif
#ifdef EINTR
   Err->addConstant("EINTR", new QoreNode((int64)EINTR));
#endif
#ifdef EIO
   Err->addConstant("EIO", new QoreNode((int64)EIO));
#endif
#ifdef ENXIO
   Err->addConstant("ENXIO", new QoreNode((int64)ENXIO));
#endif
#ifdef E2BIG
   Err->addConstant("E2BIG", new QoreNode((int64)E2BIG));
#endif
#ifdef ENOEXEC
   Err->addConstant("ENOEXEC", new QoreNode((int64)ENOEXEC));
#endif
#ifdef EBADF
   Err->addConstant("EBADF", new QoreNode((int64)EBADF));
#endif
#ifdef ECHILD
   Err->addConstant("ECHILD", new QoreNode((int64)ECHILD));
#endif
#ifdef EAGAIN
   Err->addConstant("EAGAIN", new QoreNode((int64)EAGAIN));
#endif
#ifdef ENOMEM
   Err->addConstant("ENOMEM", new QoreNode((int64)ENOMEM));
#endif
#ifdef EACCES
   Err->addConstant("EACCES", new QoreNode((int64)EACCES));
#endif
#ifdef EFAULT
   Err->addConstant("EFAULT", new QoreNode((int64)EFAULT));
#endif
#ifdef ENOTBLK
   Err->addConstant("ENOTBLK", new QoreNode((int64)ENOTBLK));
#endif
#ifdef EBUSY
   Err->addConstant("EBUSY", new QoreNode((int64)EBUSY));
#endif
#ifdef EEXIST
   Err->addConstant("EEXIST", new QoreNode((int64)EEXIST));
#endif
#ifdef EXDEV
   Err->addConstant("EXDEV", new QoreNode((int64)EXDEV));
#endif
#ifdef ENODEV
   Err->addConstant("ENODEV", new QoreNode((int64)ENODEV));
#endif
#ifdef ENOTDIR
   Err->addConstant("ENOTDIR", new QoreNode((int64)ENOTDIR));
#endif
#ifdef EISDIR
   Err->addConstant("EISDIR", new QoreNode((int64)EISDIR));
#endif
#ifdef EINVAL
   Err->addConstant("EINVAL", new QoreNode((int64)EINVAL));
#endif
#ifdef ENFILE
   Err->addConstant("ENFILE", new QoreNode((int64)ENFILE));
#endif
#ifdef EMFILE
   Err->addConstant("EMFILE", new QoreNode((int64)EMFILE));
#endif
#ifdef ENOTTY
   Err->addConstant("ENOTTY", new QoreNode((int64)ENOTTY));
#endif
#ifdef ETXTBSY
   Err->addConstant("ETXTBSY", new QoreNode((int64)ETXTBSY));
#endif
#ifdef EFBIG
   Err->addConstant("EFBIG", new QoreNode((int64)EFBIG));
#endif
#ifdef ENOSPC
   Err->addConstant("ENOSPC", new QoreNode((int64)ENOSPC));
#endif
#ifdef ESPIPE
   Err->addConstant("ESPIPE", new QoreNode((int64)ESPIPE));
#endif
#ifdef EROFS
   Err->addConstant("EROFS", new QoreNode((int64)EROFS));
#endif
#ifdef EMLINK
   Err->addConstant("EMLINK", new QoreNode((int64)EMLINK));
#endif
#ifdef EPIPE
   Err->addConstant("EPIPE", new QoreNode((int64)EPIPE));
#endif
#ifdef EDOM
   Err->addConstant("EDOM", new QoreNode((int64)EDOM));
#endif
#ifdef ERANGE
   Err->addConstant("ERANGE", new QoreNode((int64)ERANGE));
#endif
#ifdef EDEADLK
   Err->addConstant("EDEADLK", new QoreNode((int64)EDEADLK));
#endif
#ifdef ENAMETOOLONG
   Err->addConstant("ENAMETOOLONG", new QoreNode((int64)ENAMETOOLONG));
#endif
#ifdef ENOLCK
   Err->addConstant("ENOLCK", new QoreNode((int64)ENOLCK));
#endif
#ifdef ENOSYS
   Err->addConstant("ENOSYS", new QoreNode((int64)ENOSYS));
#endif
#ifdef ENOTEMPTY
   Err->addConstant("ENOTEMPTY", new QoreNode((int64)ENOTEMPTY));
#endif
#ifdef ELOOP
   Err->addConstant("ELOOP", new QoreNode((int64)ELOOP));
#endif
#ifdef EWOULDBLOCK
   Err->addConstant("EWOULDBLOCK", new QoreNode((int64)EWOULDBLOCK));
#endif
#ifdef ENOMSG
   Err->addConstant("ENOMSG", new QoreNode((int64)ENOMSG));
#endif
#ifdef EIDRM
   Err->addConstant("EIDRM", new QoreNode((int64)EIDRM));
#endif
#ifdef ECHRNG
   Err->addConstant("ECHRNG", new QoreNode((int64)ECHRNG));
#endif
#ifdef EL2NSYNC
   Err->addConstant("EL2NSYNC", new QoreNode((int64)EL2NSYNC));
#endif
#ifdef EL3HLT
   Err->addConstant("EL3HLT", new QoreNode((int64)EL3HLT));
#endif
#ifdef EL3RST
   Err->addConstant("EL3RST", new QoreNode((int64)EL3RST));
#endif
#ifdef ELNRNG
   Err->addConstant("ELNRNG", new QoreNode((int64)ELNRNG));
#endif
#ifdef EUNATCH
   Err->addConstant("EUNATCH", new QoreNode((int64)EUNATCH));
#endif
#ifdef ENOCSI
   Err->addConstant("ENOCSI", new QoreNode((int64)ENOCSI));
#endif
#ifdef EL2HLT
   Err->addConstant("EL2HLT", new QoreNode((int64)EL2HLT));
#endif
#ifdef EBADE
   Err->addConstant("EBADE", new QoreNode((int64)EBADE));
#endif
#ifdef EBADR
   Err->addConstant("EBADR", new QoreNode((int64)EBADR));
#endif
#ifdef EXFULL
   Err->addConstant("EXFULL", new QoreNode((int64)EXFULL));
#endif
#ifdef ENOANO
   Err->addConstant("ENOANO", new QoreNode((int64)ENOANO));
#endif
#ifdef EBADRQC
   Err->addConstant("EBADRQC", new QoreNode((int64)EBADRQC));
#endif
#ifdef EBADSLT
   Err->addConstant("EBADSLT", new QoreNode((int64)EBADSLT));
#endif
#ifdef EDEADLOCK
   Err->addConstant("EDEADLOCK", new QoreNode((int64)EDEADLOCK));
#endif
#ifdef EBFONT
   Err->addConstant("EBFONT", new QoreNode((int64)EBFONT));
#endif
#ifdef ENOSTR
   Err->addConstant("ENOSTR", new QoreNode((int64)ENOSTR));
#endif
#ifdef ENODATA
   Err->addConstant("ENODATA", new QoreNode((int64)ENODATA));
#endif
#ifdef ETIME
   Err->addConstant("ETIME", new QoreNode((int64)ETIME));
#endif
#ifdef ENOSR
   Err->addConstant("ENOSR", new QoreNode((int64)ENOSR));
#endif
#ifdef ENONET
   Err->addConstant("ENONET", new QoreNode((int64)ENONET));
#endif
#ifdef ENOPKG
   Err->addConstant("ENOPKG", new QoreNode((int64)ENOPKG));
#endif
#ifdef EREMOTE
   Err->addConstant("EREMOTE", new QoreNode((int64)EREMOTE));
#endif
#ifdef ENOLINK
   Err->addConstant("ENOLINK", new QoreNode((int64)ENOLINK));
#endif
#ifdef EADV
   Err->addConstant("EADV", new QoreNode((int64)EADV));
#endif
#ifdef ESRMNT
   Err->addConstant("ESRMNT", new QoreNode((int64)ESRMNT));
#endif
#ifdef ECOMM
   Err->addConstant("ECOMM", new QoreNode((int64)ECOMM));
#endif
#ifdef EPROTO
   Err->addConstant("EPROTO", new QoreNode((int64)EPROTO));
#endif
#ifdef EMULTIHOP
   Err->addConstant("EMULTIHOP", new QoreNode((int64)EMULTIHOP));
#endif
#ifdef EDOTDOT
   Err->addConstant("EDOTDOT", new QoreNode((int64)EDOTDOT));
#endif
#ifdef EBADMSG
   Err->addConstant("EBADMSG", new QoreNode((int64)EBADMSG));
#endif
#ifdef EOVERFLOW
   Err->addConstant("EOVERFLOW", new QoreNode((int64)EOVERFLOW));
#endif
#ifdef ENOTUNIQ
   Err->addConstant("ENOTUNIQ", new QoreNode((int64)ENOTUNIQ));
#endif
#ifdef EBADFD
   Err->addConstant("EBADFD", new QoreNode((int64)EBADFD));
#endif
#ifdef EREMCHG
   Err->addConstant("EREMCHG", new QoreNode((int64)EREMCHG));
#endif
#ifdef ELIBACC
   Err->addConstant("ELIBACC", new QoreNode((int64)ELIBACC));
#endif
#ifdef ELIBBAD
   Err->addConstant("ELIBBAD", new QoreNode((int64)ELIBBAD));
#endif
#ifdef ELIBSCN
   Err->addConstant("ELIBSCN", new QoreNode((int64)ELIBSCN));
#endif
#ifdef ELIBMAX
   Err->addConstant("ELIBMAX", new QoreNode((int64)ELIBMAX));
#endif
#ifdef ELIBEXEC
   Err->addConstant("ELIBEXEC", new QoreNode((int64)ELIBEXEC));
#endif
#ifdef EILSEQ
   Err->addConstant("EILSEQ", new QoreNode((int64)EILSEQ));
#endif
#ifdef ERESTART
   Err->addConstant("ERESTART", new QoreNode((int64)ERESTART));
#endif
#ifdef ESTRPIPE
   Err->addConstant("ESTRPIPE", new QoreNode((int64)ESTRPIPE));
#endif
#ifdef EUSERS
   Err->addConstant("EUSERS", new QoreNode((int64)EUSERS));
#endif
#ifdef ENOTSOCK
   Err->addConstant("ENOTSOCK", new QoreNode((int64)ENOTSOCK));
#endif
#ifdef EDESTADDRREQ
   Err->addConstant("EDESTADDRREQ", new QoreNode((int64)EDESTADDRREQ));
#endif
#ifdef EMSGSIZE
   Err->addConstant("EMSGSIZE", new QoreNode((int64)EMSGSIZE));
#endif
#ifdef EPROTOTYPE
   Err->addConstant("EPROTOTYPE", new QoreNode((int64)EPROTOTYPE));
#endif
#ifdef ENOPROTOOPT
   Err->addConstant("ENOPROTOOPT", new QoreNode((int64)ENOPROTOOPT));
#endif
#ifdef EPROTONOSUPPORT
   Err->addConstant("EPROTONOSUPPORT", new QoreNode((int64)EPROTONOSUPPORT));
#endif
#ifdef ESOCKTNOSUPPORT
   Err->addConstant("ESOCKTNOSUPPORT", new QoreNode((int64)ESOCKTNOSUPPORT));
#endif
#ifdef EOPNOTSUPP
   Err->addConstant("EOPNOTSUPP", new QoreNode((int64)EOPNOTSUPP));
#endif
#ifdef EPFNOSUPPORT
   Err->addConstant("EPFNOSUPPORT", new QoreNode((int64)EPFNOSUPPORT));
#endif
#ifdef EAFNOSUPPORT
   Err->addConstant("EAFNOSUPPORT", new QoreNode((int64)EAFNOSUPPORT));
#endif
#ifdef EADDRINUSE
   Err->addConstant("EADDRINUSE", new QoreNode((int64)EADDRINUSE));
#endif
#ifdef EADDRNOTAVAIL
   Err->addConstant("EADDRNOTAVAIL", new QoreNode((int64)EADDRNOTAVAIL));
#endif
#ifdef ENETDOWN
   Err->addConstant("ENETDOWN", new QoreNode((int64)ENETDOWN));
#endif
#ifdef ENETUNREACH
   Err->addConstant("ENETUNREACH", new QoreNode((int64)ENETUNREACH));
#endif
#ifdef ENETRESET
   Err->addConstant("ENETRESET", new QoreNode((int64)ENETRESET));
#endif
#ifdef ECONNABORTED
   Err->addConstant("ECONNABORTED", new QoreNode((int64)ECONNABORTED));
#endif
#ifdef ECONNRESET
   Err->addConstant("ECONNRESET", new QoreNode((int64)ECONNRESET));
#endif
#ifdef ENOBUFS
   Err->addConstant("ENOBUFS", new QoreNode((int64)ENOBUFS));
#endif
#ifdef EISCONN
   Err->addConstant("EISCONN", new QoreNode((int64)EISCONN));
#endif
#ifdef ENOTCONN
   Err->addConstant("ENOTCONN", new QoreNode((int64)ENOTCONN));
#endif
#ifdef ESHUTDOWN
   Err->addConstant("ESHUTDOWN", new QoreNode((int64)ESHUTDOWN));
#endif
#ifdef ETOOMANYREFS
   Err->addConstant("ETOOMANYREFS", new QoreNode((int64)ETOOMANYREFS));
#endif
#ifdef ETIMEDOUT
   Err->addConstant("ETIMEDOUT", new QoreNode((int64)ETIMEDOUT));
#endif
#ifdef ECONNREFUSED
   Err->addConstant("ECONNREFUSED", new QoreNode((int64)ECONNREFUSED));
#endif
#ifdef EHOSTDOWN
   Err->addConstant("EHOSTDOWN", new QoreNode((int64)EHOSTDOWN));
#endif
#ifdef EHOSTUNREACH
   Err->addConstant("EHOSTUNREACH", new QoreNode((int64)EHOSTUNREACH));
#endif
#ifdef EALREADY
   Err->addConstant("EALREADY", new QoreNode((int64)EALREADY));
#endif
#ifdef EINPROGRESS
   Err->addConstant("EINPROGRESS", new QoreNode((int64)EINPROGRESS));
#endif
#ifdef ESTALE
   Err->addConstant("ESTALE", new QoreNode((int64)ESTALE));
#endif
#ifdef EUCLEAN
   Err->addConstant("EUCLEAN", new QoreNode((int64)EUCLEAN));
#endif
#ifdef ENOTNAM
   Err->addConstant("ENOTNAM", new QoreNode((int64)ENOTNAM));
#endif
#ifdef ENAVAIL
   Err->addConstant("ENAVAIL", new QoreNode((int64)ENAVAIL));
#endif
#ifdef EISNAM
   Err->addConstant("EISNAM", new QoreNode((int64)EISNAM));
#endif
#ifdef EREMOTEIO
   Err->addConstant("EREMOTEIO", new QoreNode((int64)EREMOTEIO));
#endif
#ifdef EDQUOT
   Err->addConstant("EDQUOT", new QoreNode((int64)EDQUOT));
#endif
#ifdef ENOMEDIUM
   Err->addConstant("ENOMEDIUM", new QoreNode((int64)ENOMEDIUM));
#endif
#ifdef EMEDIUMTYPE
   Err->addConstant("EMEDIUMTYPE", new QoreNode((int64)EMEDIUMTYPE));
#endif

   traceout("get_errno_ns");
   return Err;
}

#endif
