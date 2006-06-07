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
   Err->addConstant("EPERM", new QoreNode(NT_INT, EPERM));
#endif
#ifdef ENOENT
   Err->addConstant("ENOENT", new QoreNode(NT_INT, ENOENT));
#endif
#ifdef ESRCH
   Err->addConstant("ESRCH", new QoreNode(NT_INT, ESRCH));
#endif
#ifdef EINTR
   Err->addConstant("EINTR", new QoreNode(NT_INT, EINTR));
#endif
#ifdef EIO
   Err->addConstant("EIO", new QoreNode(NT_INT, EIO));
#endif
#ifdef ENXIO
   Err->addConstant("ENXIO", new QoreNode(NT_INT, ENXIO));
#endif
#ifdef E2BIG
   Err->addConstant("E2BIG", new QoreNode(NT_INT, E2BIG));
#endif
#ifdef ENOEXEC
   Err->addConstant("ENOEXEC", new QoreNode(NT_INT, ENOEXEC));
#endif
#ifdef EBADF
   Err->addConstant("EBADF", new QoreNode(NT_INT, EBADF));
#endif
#ifdef ECHILD
   Err->addConstant("ECHILD", new QoreNode(NT_INT, ECHILD));
#endif
#ifdef EAGAIN
   Err->addConstant("EAGAIN", new QoreNode(NT_INT, EAGAIN));
#endif
#ifdef ENOMEM
   Err->addConstant("ENOMEM", new QoreNode(NT_INT, ENOMEM));
#endif
#ifdef EACCES
   Err->addConstant("EACCES", new QoreNode(NT_INT, EACCES));
#endif
#ifdef EFAULT
   Err->addConstant("EFAULT", new QoreNode(NT_INT, EFAULT));
#endif
#ifdef ENOTBLK
   Err->addConstant("ENOTBLK", new QoreNode(NT_INT, ENOTBLK));
#endif
#ifdef EBUSY
   Err->addConstant("EBUSY", new QoreNode(NT_INT, EBUSY));
#endif
#ifdef EEXIST
   Err->addConstant("EEXIST", new QoreNode(NT_INT, EEXIST));
#endif
#ifdef EXDEV
   Err->addConstant("EXDEV", new QoreNode(NT_INT, EXDEV));
#endif
#ifdef ENODEV
   Err->addConstant("ENODEV", new QoreNode(NT_INT, ENODEV));
#endif
#ifdef ENOTDIR
   Err->addConstant("ENOTDIR", new QoreNode(NT_INT, ENOTDIR));
#endif
#ifdef EISDIR
   Err->addConstant("EISDIR", new QoreNode(NT_INT, EISDIR));
#endif
#ifdef EINVAL
   Err->addConstant("EINVAL", new QoreNode(NT_INT, EINVAL));
#endif
#ifdef ENFILE
   Err->addConstant("ENFILE", new QoreNode(NT_INT, ENFILE));
#endif
#ifdef EMFILE
   Err->addConstant("EMFILE", new QoreNode(NT_INT, EMFILE));
#endif
#ifdef ENOTTY
   Err->addConstant("ENOTTY", new QoreNode(NT_INT, ENOTTY));
#endif
#ifdef ETXTBSY
   Err->addConstant("ETXTBSY", new QoreNode(NT_INT, ETXTBSY));
#endif
#ifdef EFBIG
   Err->addConstant("EFBIG", new QoreNode(NT_INT, EFBIG));
#endif
#ifdef ENOSPC
   Err->addConstant("ENOSPC", new QoreNode(NT_INT, ENOSPC));
#endif
#ifdef ESPIPE
   Err->addConstant("ESPIPE", new QoreNode(NT_INT, ESPIPE));
#endif
#ifdef EROFS
   Err->addConstant("EROFS", new QoreNode(NT_INT, EROFS));
#endif
#ifdef EMLINK
   Err->addConstant("EMLINK", new QoreNode(NT_INT, EMLINK));
#endif
#ifdef EPIPE
   Err->addConstant("EPIPE", new QoreNode(NT_INT, EPIPE));
#endif
#ifdef EDOM
   Err->addConstant("EDOM", new QoreNode(NT_INT, EDOM));
#endif
#ifdef ERANGE
   Err->addConstant("ERANGE", new QoreNode(NT_INT, ERANGE));
#endif
#ifdef EDEADLK
   Err->addConstant("EDEADLK", new QoreNode(NT_INT, EDEADLK));
#endif
#ifdef ENAMETOOLONG
   Err->addConstant("ENAMETOOLONG", new QoreNode(NT_INT, ENAMETOOLONG));
#endif
#ifdef ENOLCK
   Err->addConstant("ENOLCK", new QoreNode(NT_INT, ENOLCK));
#endif
#ifdef ENOSYS
   Err->addConstant("ENOSYS", new QoreNode(NT_INT, ENOSYS));
#endif
#ifdef ENOTEMPTY
   Err->addConstant("ENOTEMPTY", new QoreNode(NT_INT, ENOTEMPTY));
#endif
#ifdef ELOOP
   Err->addConstant("ELOOP", new QoreNode(NT_INT, ELOOP));
#endif
#ifdef EWOULDBLOCK
   Err->addConstant("EWOULDBLOCK", new QoreNode(NT_INT, EWOULDBLOCK));
#endif
#ifdef ENOMSG
   Err->addConstant("ENOMSG", new QoreNode(NT_INT, ENOMSG));
#endif
#ifdef EIDRM
   Err->addConstant("EIDRM", new QoreNode(NT_INT, EIDRM));
#endif
#ifdef ECHRNG
   Err->addConstant("ECHRNG", new QoreNode(NT_INT, ECHRNG));
#endif
#ifdef EL2NSYNC
   Err->addConstant("EL2NSYNC", new QoreNode(NT_INT, EL2NSYNC));
#endif
#ifdef EL3HLT
   Err->addConstant("EL3HLT", new QoreNode(NT_INT, EL3HLT));
#endif
#ifdef EL3RST
   Err->addConstant("EL3RST", new QoreNode(NT_INT, EL3RST));
#endif
#ifdef ELNRNG
   Err->addConstant("ELNRNG", new QoreNode(NT_INT, ELNRNG));
#endif
#ifdef EUNATCH
   Err->addConstant("EUNATCH", new QoreNode(NT_INT, EUNATCH));
#endif
#ifdef ENOCSI
   Err->addConstant("ENOCSI", new QoreNode(NT_INT, ENOCSI));
#endif
#ifdef EL2HLT
   Err->addConstant("EL2HLT", new QoreNode(NT_INT, EL2HLT));
#endif
#ifdef EBADE
   Err->addConstant("EBADE", new QoreNode(NT_INT, EBADE));
#endif
#ifdef EBADR
   Err->addConstant("EBADR", new QoreNode(NT_INT, EBADR));
#endif
#ifdef EXFULL
   Err->addConstant("EXFULL", new QoreNode(NT_INT, EXFULL));
#endif
#ifdef ENOANO
   Err->addConstant("ENOANO", new QoreNode(NT_INT, ENOANO));
#endif
#ifdef EBADRQC
   Err->addConstant("EBADRQC", new QoreNode(NT_INT, EBADRQC));
#endif
#ifdef EBADSLT
   Err->addConstant("EBADSLT", new QoreNode(NT_INT, EBADSLT));
#endif
#ifdef EDEADLOCK
   Err->addConstant("EDEADLOCK", new QoreNode(NT_INT, EDEADLOCK));
#endif
#ifdef EBFONT
   Err->addConstant("EBFONT", new QoreNode(NT_INT, EBFONT));
#endif
#ifdef ENOSTR
   Err->addConstant("ENOSTR", new QoreNode(NT_INT, ENOSTR));
#endif
#ifdef ENODATA
   Err->addConstant("ENODATA", new QoreNode(NT_INT, ENODATA));
#endif
#ifdef ETIME
   Err->addConstant("ETIME", new QoreNode(NT_INT, ETIME));
#endif
#ifdef ENOSR
   Err->addConstant("ENOSR", new QoreNode(NT_INT, ENOSR));
#endif
#ifdef ENONET
   Err->addConstant("ENONET", new QoreNode(NT_INT, ENONET));
#endif
#ifdef ENOPKG
   Err->addConstant("ENOPKG", new QoreNode(NT_INT, ENOPKG));
#endif
#ifdef EREMOTE
   Err->addConstant("EREMOTE", new QoreNode(NT_INT, EREMOTE));
#endif
#ifdef ENOLINK
   Err->addConstant("ENOLINK", new QoreNode(NT_INT, ENOLINK));
#endif
#ifdef EADV
   Err->addConstant("EADV", new QoreNode(NT_INT, EADV));
#endif
#ifdef ESRMNT
   Err->addConstant("ESRMNT", new QoreNode(NT_INT, ESRMNT));
#endif
#ifdef ECOMM
   Err->addConstant("ECOMM", new QoreNode(NT_INT, ECOMM));
#endif
#ifdef EPROTO
   Err->addConstant("EPROTO", new QoreNode(NT_INT, EPROTO));
#endif
#ifdef EMULTIHOP
   Err->addConstant("EMULTIHOP", new QoreNode(NT_INT, EMULTIHOP));
#endif
#ifdef EDOTDOT
   Err->addConstant("EDOTDOT", new QoreNode(NT_INT, EDOTDOT));
#endif
#ifdef EBADMSG
   Err->addConstant("EBADMSG", new QoreNode(NT_INT, EBADMSG));
#endif
#ifdef EOVERFLOW
   Err->addConstant("EOVERFLOW", new QoreNode(NT_INT, EOVERFLOW));
#endif
#ifdef ENOTUNIQ
   Err->addConstant("ENOTUNIQ", new QoreNode(NT_INT, ENOTUNIQ));
#endif
#ifdef EBADFD
   Err->addConstant("EBADFD", new QoreNode(NT_INT, EBADFD));
#endif
#ifdef EREMCHG
   Err->addConstant("EREMCHG", new QoreNode(NT_INT, EREMCHG));
#endif
#ifdef ELIBACC
   Err->addConstant("ELIBACC", new QoreNode(NT_INT, ELIBACC));
#endif
#ifdef ELIBBAD
   Err->addConstant("ELIBBAD", new QoreNode(NT_INT, ELIBBAD));
#endif
#ifdef ELIBSCN
   Err->addConstant("ELIBSCN", new QoreNode(NT_INT, ELIBSCN));
#endif
#ifdef ELIBMAX
   Err->addConstant("ELIBMAX", new QoreNode(NT_INT, ELIBMAX));
#endif
#ifdef ELIBEXEC
   Err->addConstant("ELIBEXEC", new QoreNode(NT_INT, ELIBEXEC));
#endif
#ifdef EILSEQ
   Err->addConstant("EILSEQ", new QoreNode(NT_INT, EILSEQ));
#endif
#ifdef ERESTART
   Err->addConstant("ERESTART", new QoreNode(NT_INT, ERESTART));
#endif
#ifdef ESTRPIPE
   Err->addConstant("ESTRPIPE", new QoreNode(NT_INT, ESTRPIPE));
#endif
#ifdef EUSERS
   Err->addConstant("EUSERS", new QoreNode(NT_INT, EUSERS));
#endif
#ifdef ENOTSOCK
   Err->addConstant("ENOTSOCK", new QoreNode(NT_INT, ENOTSOCK));
#endif
#ifdef EDESTADDRREQ
   Err->addConstant("EDESTADDRREQ", new QoreNode(NT_INT, EDESTADDRREQ));
#endif
#ifdef EMSGSIZE
   Err->addConstant("EMSGSIZE", new QoreNode(NT_INT, EMSGSIZE));
#endif
#ifdef EPROTOTYPE
   Err->addConstant("EPROTOTYPE", new QoreNode(NT_INT, EPROTOTYPE));
#endif
#ifdef ENOPROTOOPT
   Err->addConstant("ENOPROTOOPT", new QoreNode(NT_INT, ENOPROTOOPT));
#endif
#ifdef EPROTONOSUPPORT
   Err->addConstant("EPROTONOSUPPORT", new QoreNode(NT_INT, EPROTONOSUPPORT));
#endif
#ifdef ESOCKTNOSUPPORT
   Err->addConstant("ESOCKTNOSUPPORT", new QoreNode(NT_INT, ESOCKTNOSUPPORT));
#endif
#ifdef EOPNOTSUPP
   Err->addConstant("EOPNOTSUPP", new QoreNode(NT_INT, EOPNOTSUPP));
#endif
#ifdef EPFNOSUPPORT
   Err->addConstant("EPFNOSUPPORT", new QoreNode(NT_INT, EPFNOSUPPORT));
#endif
#ifdef EAFNOSUPPORT
   Err->addConstant("EAFNOSUPPORT", new QoreNode(NT_INT, EAFNOSUPPORT));
#endif
#ifdef EADDRINUSE
   Err->addConstant("EADDRINUSE", new QoreNode(NT_INT, EADDRINUSE));
#endif
#ifdef EADDRNOTAVAIL
   Err->addConstant("EADDRNOTAVAIL", new QoreNode(NT_INT, EADDRNOTAVAIL));
#endif
#ifdef ENETDOWN
   Err->addConstant("ENETDOWN", new QoreNode(NT_INT, ENETDOWN));
#endif
#ifdef ENETUNREACH
   Err->addConstant("ENETUNREACH", new QoreNode(NT_INT, ENETUNREACH));
#endif
#ifdef ENETRESET
   Err->addConstant("ENETRESET", new QoreNode(NT_INT, ENETRESET));
#endif
#ifdef ECONNABORTED
   Err->addConstant("ECONNABORTED", new QoreNode(NT_INT, ECONNABORTED));
#endif
#ifdef ECONNRESET
   Err->addConstant("ECONNRESET", new QoreNode(NT_INT, ECONNRESET));
#endif
#ifdef ENOBUFS
   Err->addConstant("ENOBUFS", new QoreNode(NT_INT, ENOBUFS));
#endif
#ifdef EISCONN
   Err->addConstant("EISCONN", new QoreNode(NT_INT, EISCONN));
#endif
#ifdef ENOTCONN
   Err->addConstant("ENOTCONN", new QoreNode(NT_INT, ENOTCONN));
#endif
#ifdef ESHUTDOWN
   Err->addConstant("ESHUTDOWN", new QoreNode(NT_INT, ESHUTDOWN));
#endif
#ifdef ETOOMANYREFS
   Err->addConstant("ETOOMANYREFS", new QoreNode(NT_INT, ETOOMANYREFS));
#endif
#ifdef ETIMEDOUT
   Err->addConstant("ETIMEDOUT", new QoreNode(NT_INT, ETIMEDOUT));
#endif
#ifdef ECONNREFUSED
   Err->addConstant("ECONNREFUSED", new QoreNode(NT_INT, ECONNREFUSED));
#endif
#ifdef EHOSTDOWN
   Err->addConstant("EHOSTDOWN", new QoreNode(NT_INT, EHOSTDOWN));
#endif
#ifdef EHOSTUNREACH
   Err->addConstant("EHOSTUNREACH", new QoreNode(NT_INT, EHOSTUNREACH));
#endif
#ifdef EALREADY
   Err->addConstant("EALREADY", new QoreNode(NT_INT, EALREADY));
#endif
#ifdef EINPROGRESS
   Err->addConstant("EINPROGRESS", new QoreNode(NT_INT, EINPROGRESS));
#endif
#ifdef ESTALE
   Err->addConstant("ESTALE", new QoreNode(NT_INT, ESTALE));
#endif
#ifdef EUCLEAN
   Err->addConstant("EUCLEAN", new QoreNode(NT_INT, EUCLEAN));
#endif
#ifdef ENOTNAM
   Err->addConstant("ENOTNAM", new QoreNode(NT_INT, ENOTNAM));
#endif
#ifdef ENAVAIL
   Err->addConstant("ENAVAIL", new QoreNode(NT_INT, ENAVAIL));
#endif
#ifdef EISNAM
   Err->addConstant("EISNAM", new QoreNode(NT_INT, EISNAM));
#endif
#ifdef EREMOTEIO
   Err->addConstant("EREMOTEIO", new QoreNode(NT_INT, EREMOTEIO));
#endif
#ifdef EDQUOT
   Err->addConstant("EDQUOT", new QoreNode(NT_INT, EDQUOT));
#endif
#ifdef ENOMEDIUM
   Err->addConstant("ENOMEDIUM", new QoreNode(NT_INT, ENOMEDIUM));
#endif
#ifdef EMEDIUMTYPE
   Err->addConstant("EMEDIUMTYPE", new QoreNode(NT_INT, EMEDIUMTYPE));
#endif

   traceout("get_errno_ns");
   return Err;
}

#endif
