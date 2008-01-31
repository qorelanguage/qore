/*
  QoreLibIntern.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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

#ifndef _QORE_QORELIBINTERN_H

#define _QORE_QORELIBINTERN_H

#ifndef HAVE_GETHOSTBYNAME_R
DLLLOCAL extern LockedObject lck_gethostbyname;
#endif
#ifndef HAVE_GETHOSTBYADDR_R
DLLLOCAL extern LockedObject lck_gethostbyaddr;
#endif

#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX    108
#endif

#ifndef NET_BUFSIZE
#define NET_BUFSIZE      1024
#endif

#ifndef HOSTNAMEBUFSIZE
#define HOSTNAMEBUFSIZE 512
#endif

#ifndef HAVE_LOCALTIME_R
DLLLOCAL extern LockedObject lck_localtime;
#endif

#ifndef HAVE_GMTIME_R
DLLLOCAL extern LockedObject lck_gmtime;
#endif

DLLLOCAL extern char table64[64];

DLLLOCAL int get_nibble(char c, ExceptionSink *xsink);
DLLLOCAL BinaryNode *parseBase64(const char *buf, int len, ExceptionSink *xsink);
DLLLOCAL BinaryNode *parseHex(const char *buf, int len, ExceptionSink *xsink);
DLLLOCAL BinaryNode *parseHex(const char *buf, int len);
DLLLOCAL void print_node(FILE *fp, QoreNode *node);
DLLLOCAL void delete_global_variables();
DLLLOCAL void initENV(char *env[]);
DLLLOCAL ResolvedFunctionReferenceNode *getFunctionReference(QoreString *str, ExceptionSink *xsink);

// the following functions are implemented in support.cc
DLLLOCAL void parse_error(int sline, int eline, const char *fmt, ...);
DLLLOCAL void parse_error(const char *fmt, ...);
DLLLOCAL void parseException(const char *err, const char *fmt, ...);
DLLLOCAL QoreString *findFileInEnvPath(const char *file, const char *varname);

#endif
