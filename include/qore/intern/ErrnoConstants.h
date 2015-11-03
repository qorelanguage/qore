/*
  ErrnoConstants.h

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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
static inline class QoreNamespace *get_errno_ns()
{
   QoreNamespace *Err = new QoreNamespace("Err");

#ifdef EPERM
   Err->addConstant("EPERM", new QoreBigIntNode(EPERM));
#endif
#ifdef ENOENT
   Err->addConstant("ENOENT", new QoreBigIntNode(ENOENT));
#endif
#ifdef ESRCH
   Err->addConstant("ESRCH", new QoreBigIntNode(ESRCH));
#endif
#ifdef EINTR
   Err->addConstant("EINTR", new QoreBigIntNode(EINTR));
#endif
#ifdef EIO
   Err->addConstant("EIO", new QoreBigIntNode(EIO));
#endif
#ifdef ENXIO
   Err->addConstant("ENXIO", new QoreBigIntNode(ENXIO));
#endif
#ifdef E2BIG
   Err->addConstant("E2BIG", new QoreBigIntNode(E2BIG));
#endif
#ifdef ENOEXEC
   Err->addConstant("ENOEXEC", new QoreBigIntNode(ENOEXEC));
#endif
#ifdef EBADF
   Err->addConstant("EBADF", new QoreBigIntNode(EBADF));
#endif
#ifdef ECHILD
   Err->addConstant("ECHILD", new QoreBigIntNode(ECHILD));
#endif
#ifdef EAGAIN
   Err->addConstant("EAGAIN", new QoreBigIntNode(EAGAIN));
#endif
#ifdef ENOMEM
   Err->addConstant("ENOMEM", new QoreBigIntNode(ENOMEM));
#endif
#ifdef EACCES
   Err->addConstant("EACCES", new QoreBigIntNode(EACCES));
#endif
#ifdef EFAULT
   Err->addConstant("EFAULT", new QoreBigIntNode(EFAULT));
#endif
#ifdef ENOTBLK
   Err->addConstant("ENOTBLK", new QoreBigIntNode(ENOTBLK));
#endif
#ifdef EBUSY
   Err->addConstant("EBUSY", new QoreBigIntNode(EBUSY));
#endif
#ifdef EEXIST
   Err->addConstant("EEXIST", new QoreBigIntNode(EEXIST));
#endif
#ifdef EXDEV
   Err->addConstant("EXDEV", new QoreBigIntNode(EXDEV));
#endif
#ifdef ENODEV
   Err->addConstant("ENODEV", new QoreBigIntNode(ENODEV));
#endif
#ifdef ENOTDIR
   Err->addConstant("ENOTDIR", new QoreBigIntNode(ENOTDIR));
#endif
#ifdef EISDIR
   Err->addConstant("EISDIR", new QoreBigIntNode(EISDIR));
#endif
#ifdef EINVAL
   Err->addConstant("EINVAL", new QoreBigIntNode(EINVAL));
#endif
#ifdef ENFILE
   Err->addConstant("ENFILE", new QoreBigIntNode(ENFILE));
#endif
#ifdef EMFILE
   Err->addConstant("EMFILE", new QoreBigIntNode(EMFILE));
#endif
#ifdef ENOTTY
   Err->addConstant("ENOTTY", new QoreBigIntNode(ENOTTY));
#endif
#ifdef ETXTBSY
   Err->addConstant("ETXTBSY", new QoreBigIntNode(ETXTBSY));
#endif
#ifdef EFBIG
   Err->addConstant("EFBIG", new QoreBigIntNode(EFBIG));
#endif
#ifdef ENOSPC
   Err->addConstant("ENOSPC", new QoreBigIntNode(ENOSPC));
#endif
#ifdef ESPIPE
   Err->addConstant("ESPIPE", new QoreBigIntNode(ESPIPE));
#endif
#ifdef EROFS
   Err->addConstant("EROFS", new QoreBigIntNode(EROFS));
#endif
#ifdef EMLINK
   Err->addConstant("EMLINK", new QoreBigIntNode(EMLINK));
#endif
#ifdef EPIPE
   Err->addConstant("EPIPE", new QoreBigIntNode(EPIPE));
#endif
#ifdef EDOM
   Err->addConstant("EDOM", new QoreBigIntNode(EDOM));
#endif
#ifdef ERANGE
   Err->addConstant("ERANGE", new QoreBigIntNode(ERANGE));
#endif
#ifdef EDEADLK
   Err->addConstant("EDEADLK", new QoreBigIntNode(EDEADLK));
#endif
#ifdef ENAMETOOLONG
   Err->addConstant("ENAMETOOLONG", new QoreBigIntNode(ENAMETOOLONG));
#endif
#ifdef ENOLCK
   Err->addConstant("ENOLCK", new QoreBigIntNode(ENOLCK));
#endif
#ifdef ENOSYS
   Err->addConstant("ENOSYS", new QoreBigIntNode(ENOSYS));
#endif
#ifdef ENOTEMPTY
   Err->addConstant("ENOTEMPTY", new QoreBigIntNode(ENOTEMPTY));
#endif
#ifdef ELOOP
   Err->addConstant("ELOOP", new QoreBigIntNode(ELOOP));
#endif
#ifdef EWOULDBLOCK
   Err->addConstant("EWOULDBLOCK", new QoreBigIntNode(EWOULDBLOCK));
#endif
#ifdef ENOMSG
   Err->addConstant("ENOMSG", new QoreBigIntNode(ENOMSG));
#endif
#ifdef EIDRM
   Err->addConstant("EIDRM", new QoreBigIntNode(EIDRM));
#endif
#ifdef ECHRNG
   Err->addConstant("ECHRNG", new QoreBigIntNode(ECHRNG));
#endif
#ifdef EL2NSYNC
   Err->addConstant("EL2NSYNC", new QoreBigIntNode(EL2NSYNC));
#endif
#ifdef EL3HLT
   Err->addConstant("EL3HLT", new QoreBigIntNode(EL3HLT));
#endif
#ifdef EL3RST
   Err->addConstant("EL3RST", new QoreBigIntNode(EL3RST));
#endif
#ifdef ELNRNG
   Err->addConstant("ELNRNG", new QoreBigIntNode(ELNRNG));
#endif
#ifdef EUNATCH
   Err->addConstant("EUNATCH", new QoreBigIntNode(EUNATCH));
#endif
#ifdef ENOCSI
   Err->addConstant("ENOCSI", new QoreBigIntNode(ENOCSI));
#endif
#ifdef EL2HLT
   Err->addConstant("EL2HLT", new QoreBigIntNode(EL2HLT));
#endif
#ifdef EBADE
   Err->addConstant("EBADE", new QoreBigIntNode(EBADE));
#endif
#ifdef EBADR
   Err->addConstant("EBADR", new QoreBigIntNode(EBADR));
#endif
#ifdef EXFULL
   Err->addConstant("EXFULL", new QoreBigIntNode(EXFULL));
#endif
#ifdef ENOANO
   Err->addConstant("ENOANO", new QoreBigIntNode(ENOANO));
#endif
#ifdef EBADRQC
   Err->addConstant("EBADRQC", new QoreBigIntNode(EBADRQC));
#endif
#ifdef EBADSLT
   Err->addConstant("EBADSLT", new QoreBigIntNode(EBADSLT));
#endif
#ifdef EDEADLOCK
   Err->addConstant("EDEADLOCK", new QoreBigIntNode(EDEADLOCK));
#endif
#ifdef EBFONT
   Err->addConstant("EBFONT", new QoreBigIntNode(EBFONT));
#endif
#ifdef ENOSTR
   Err->addConstant("ENOSTR", new QoreBigIntNode(ENOSTR));
#endif
#ifdef ENODATA
   Err->addConstant("ENODATA", new QoreBigIntNode(ENODATA));
#endif
#ifdef ETIME
   Err->addConstant("ETIME", new QoreBigIntNode(ETIME));
#endif
#ifdef ENOSR
   Err->addConstant("ENOSR", new QoreBigIntNode(ENOSR));
#endif
#ifdef ENONET
   Err->addConstant("ENONET", new QoreBigIntNode(ENONET));
#endif
#ifdef ENOPKG
   Err->addConstant("ENOPKG", new QoreBigIntNode(ENOPKG));
#endif
#ifdef EREMOTE
   Err->addConstant("EREMOTE", new QoreBigIntNode(EREMOTE));
#endif
#ifdef ENOLINK
   Err->addConstant("ENOLINK", new QoreBigIntNode(ENOLINK));
#endif
#ifdef EADV
   Err->addConstant("EADV", new QoreBigIntNode(EADV));
#endif
#ifdef ESRMNT
   Err->addConstant("ESRMNT", new QoreBigIntNode(ESRMNT));
#endif
#ifdef ECOMM
   Err->addConstant("ECOMM", new QoreBigIntNode(ECOMM));
#endif
#ifdef EPROTO
   Err->addConstant("EPROTO", new QoreBigIntNode(EPROTO));
#endif
#ifdef EMULTIHOP
   Err->addConstant("EMULTIHOP", new QoreBigIntNode(EMULTIHOP));
#endif
#ifdef EDOTDOT
   Err->addConstant("EDOTDOT", new QoreBigIntNode(EDOTDOT));
#endif
#ifdef EBADMSG
   Err->addConstant("EBADMSG", new QoreBigIntNode(EBADMSG));
#endif
#ifdef EOVERFLOW
   Err->addConstant("EOVERFLOW", new QoreBigIntNode(EOVERFLOW));
#endif
#ifdef ENOTUNIQ
   Err->addConstant("ENOTUNIQ", new QoreBigIntNode(ENOTUNIQ));
#endif
#ifdef EBADFD
   Err->addConstant("EBADFD", new QoreBigIntNode(EBADFD));
#endif
#ifdef EREMCHG
   Err->addConstant("EREMCHG", new QoreBigIntNode(EREMCHG));
#endif
#ifdef ELIBACC
   Err->addConstant("ELIBACC", new QoreBigIntNode(ELIBACC));
#endif
#ifdef ELIBBAD
   Err->addConstant("ELIBBAD", new QoreBigIntNode(ELIBBAD));
#endif
#ifdef ELIBSCN
   Err->addConstant("ELIBSCN", new QoreBigIntNode(ELIBSCN));
#endif
#ifdef ELIBMAX
   Err->addConstant("ELIBMAX", new QoreBigIntNode(ELIBMAX));
#endif
#ifdef ELIBEXEC
   Err->addConstant("ELIBEXEC", new QoreBigIntNode(ELIBEXEC));
#endif
#ifdef EILSEQ
   Err->addConstant("EILSEQ", new QoreBigIntNode(EILSEQ));
#endif
#ifdef ERESTART
   Err->addConstant("ERESTART", new QoreBigIntNode(ERESTART));
#endif
#ifdef ESTRPIPE
   Err->addConstant("ESTRPIPE", new QoreBigIntNode(ESTRPIPE));
#endif
#ifdef EUSERS
   Err->addConstant("EUSERS", new QoreBigIntNode(EUSERS));
#endif
#ifdef ENOTSOCK
   Err->addConstant("ENOTSOCK", new QoreBigIntNode(ENOTSOCK));
#endif
#ifdef EDESTADDRREQ
   Err->addConstant("EDESTADDRREQ", new QoreBigIntNode(EDESTADDRREQ));
#endif
#ifdef EMSGSIZE
   Err->addConstant("EMSGSIZE", new QoreBigIntNode(EMSGSIZE));
#endif
#ifdef EPROTOTYPE
   Err->addConstant("EPROTOTYPE", new QoreBigIntNode(EPROTOTYPE));
#endif
#ifdef ENOPROTOOPT
   Err->addConstant("ENOPROTOOPT", new QoreBigIntNode(ENOPROTOOPT));
#endif
#ifdef EPROTONOSUPPORT
   Err->addConstant("EPROTONOSUPPORT", new QoreBigIntNode(EPROTONOSUPPORT));
#endif
#ifdef ESOCKTNOSUPPORT
   Err->addConstant("ESOCKTNOSUPPORT", new QoreBigIntNode(ESOCKTNOSUPPORT));
#endif
#ifdef EOPNOTSUPP
   Err->addConstant("EOPNOTSUPP", new QoreBigIntNode(EOPNOTSUPP));
#endif
#ifdef EPFNOSUPPORT
   Err->addConstant("EPFNOSUPPORT", new QoreBigIntNode(EPFNOSUPPORT));
#endif
#ifdef EAFNOSUPPORT
   Err->addConstant("EAFNOSUPPORT", new QoreBigIntNode(EAFNOSUPPORT));
#endif
#ifdef EADDRINUSE
   Err->addConstant("EADDRINUSE", new QoreBigIntNode(EADDRINUSE));
#endif
#ifdef EADDRNOTAVAIL
   Err->addConstant("EADDRNOTAVAIL", new QoreBigIntNode(EADDRNOTAVAIL));
#endif
#ifdef ENETDOWN
   Err->addConstant("ENETDOWN", new QoreBigIntNode(ENETDOWN));
#endif
#ifdef ENETUNREACH
   Err->addConstant("ENETUNREACH", new QoreBigIntNode(ENETUNREACH));
#endif
#ifdef ENETRESET
   Err->addConstant("ENETRESET", new QoreBigIntNode(ENETRESET));
#endif
#ifdef ECONNABORTED
   Err->addConstant("ECONNABORTED", new QoreBigIntNode(ECONNABORTED));
#endif
#ifdef ECONNRESET
   Err->addConstant("ECONNRESET", new QoreBigIntNode(ECONNRESET));
#endif
#ifdef ENOBUFS
   Err->addConstant("ENOBUFS", new QoreBigIntNode(ENOBUFS));
#endif
#ifdef EISCONN
   Err->addConstant("EISCONN", new QoreBigIntNode(EISCONN));
#endif
#ifdef ENOTCONN
   Err->addConstant("ENOTCONN", new QoreBigIntNode(ENOTCONN));
#endif
#ifdef ESHUTDOWN
   Err->addConstant("ESHUTDOWN", new QoreBigIntNode(ESHUTDOWN));
#endif
#ifdef ETOOMANYREFS
   Err->addConstant("ETOOMANYREFS", new QoreBigIntNode(ETOOMANYREFS));
#endif
#ifdef ETIMEDOUT
   Err->addConstant("ETIMEDOUT", new QoreBigIntNode(ETIMEDOUT));
#endif
#ifdef ECONNREFUSED
   Err->addConstant("ECONNREFUSED", new QoreBigIntNode(ECONNREFUSED));
#endif
#ifdef EHOSTDOWN
   Err->addConstant("EHOSTDOWN", new QoreBigIntNode(EHOSTDOWN));
#endif
#ifdef EHOSTUNREACH
   Err->addConstant("EHOSTUNREACH", new QoreBigIntNode(EHOSTUNREACH));
#endif
#ifdef EALREADY
   Err->addConstant("EALREADY", new QoreBigIntNode(EALREADY));
#endif
#ifdef EINPROGRESS
   Err->addConstant("EINPROGRESS", new QoreBigIntNode(EINPROGRESS));
#endif
#ifdef ESTALE
   Err->addConstant("ESTALE", new QoreBigIntNode(ESTALE));
#endif
#ifdef EUCLEAN
   Err->addConstant("EUCLEAN", new QoreBigIntNode(EUCLEAN));
#endif
#ifdef ENOTNAM
   Err->addConstant("ENOTNAM", new QoreBigIntNode(ENOTNAM));
#endif
#ifdef ENAVAIL
   Err->addConstant("ENAVAIL", new QoreBigIntNode(ENAVAIL));
#endif
#ifdef EISNAM
   Err->addConstant("EISNAM", new QoreBigIntNode(EISNAM));
#endif
#ifdef EREMOTEIO
   Err->addConstant("EREMOTEIO", new QoreBigIntNode(EREMOTEIO));
#endif
#ifdef EDQUOT
   Err->addConstant("EDQUOT", new QoreBigIntNode(EDQUOT));
#endif
#ifdef ENOMEDIUM
   Err->addConstant("ENOMEDIUM", new QoreBigIntNode(ENOMEDIUM));
#endif
#ifdef EMEDIUMTYPE
   Err->addConstant("EMEDIUMTYPE", new QoreBigIntNode(EMEDIUMTYPE));
#endif

   return Err;
}

#endif
