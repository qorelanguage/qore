/*
  QoreLibIntern.h

  Qore Programming Language

  Copyright 2003 - 2008 David Nichols

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

#include <list>

enum obe_type_e { OBE_Unconditional, OBE_Success, OBE_Error };

typedef std::pair<enum obe_type_e, class StatementBlock *> qore_conditional_block_exit_statement_t;

typedef std::list<qore_conditional_block_exit_statement_t> block_list_t;

#define NT_NONE  -1
#define NT_ALL   -2

#include <qore/intern/ParseNode.h>
#include <qore/intern/CallReferenceCallNode.h>
#include <qore/intern/CallReferenceNode.h>
#include <qore/intern/Function.h>
#include <qore/intern/AbstractStatement.h>
#include <qore/intern/Variable.h>
#include <qore/intern/LocalVar.h>
#include <qore/intern/NamedScope.h>
#include <qore/intern/ScopedObjectCallNode.h>
#include <qore/intern/ConstantNode.h>
#include <qore/intern/ClassRefNode.h>
#include <qore/intern/Context.h>
#include <qore/intern/Operator.h>
#include <qore/intern/QoreTreeNode.h>
#include <qore/intern/BarewordNode.h>
#include <qore/intern/SelfVarrefNode.h>
#include <qore/intern/BackquoteNode.h>
#include <qore/intern/ContextrefNode.h>
#include <qore/intern/ContextRowNode.h>
#include <qore/intern/ComplexContextrefNode.h>
#include <qore/intern/FindNode.h>
#include <qore/intern/VRMutex.h>
#include <qore/intern/VLock.h>
#include <qore/intern/QoreException.h>
#include <qore/intern/qore_thread_intern.h>
#include <qore/intern/StatementBlock.h>
#include <qore/intern/VarRefNode.h>
#include <qore/intern/FunctionCallNode.h>
#include <qore/intern/RegexSubstNode.h>
#include <qore/intern/QoreRegexNode.h>
#include <qore/intern/RegexTransNode.h>
#include <qore/intern/ObjectMethodReferenceNode.h>
#include <qore/intern/QoreClosureParseNode.h>
#include <qore/intern/QoreClosureNode.h>
#include <qore/intern/QoreImplicitArgumentNode.h>

DLLLOCAL extern int qore_library_options;

#ifndef HAVE_GETHOSTBYNAME_R
DLLLOCAL extern QoreThreadLock lck_gethostbyname;
#endif
#ifndef HAVE_GETHOSTBYADDR_R
DLLLOCAL extern QoreThreadLock lck_gethostbyaddr;
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
DLLLOCAL extern QoreThreadLock lck_localtime;
#endif

#ifndef HAVE_GMTIME_R
DLLLOCAL extern QoreThreadLock lck_gmtime;
#endif

DLLLOCAL extern char table64[64];

DLLLOCAL int get_nibble(char c, ExceptionSink *xsink);
DLLLOCAL BinaryNode *parseBase64(const char *buf, int len, ExceptionSink *xsink);
DLLLOCAL BinaryNode *parseHex(const char *buf, int len, ExceptionSink *xsink);
DLLLOCAL BinaryNode *parseHex(const char *buf, int len);
DLLLOCAL void print_node(FILE *fp, const AbstractQoreNode *node);
DLLLOCAL void delete_global_variables();
DLLLOCAL void initENV(char *env[]);
DLLLOCAL ResolvedCallReferenceNode *getCallReference(const QoreString *str, ExceptionSink *xsink);

// the following functions are implemented in support.cc
DLLLOCAL void parse_error(int sline, int eline, const char *fmt, ...);
DLLLOCAL void parse_error(const char *fmt, ...);
DLLLOCAL void parseException(const char *err, const char *fmt, ...);
DLLLOCAL QoreString *findFileInEnvPath(const char *file, const char *varname);

DLLLOCAL class AbstractQoreNode *copy_and_resolve_lvar_refs(const AbstractQoreNode *n, ExceptionSink *xsink);

DLLLOCAL void addProgramConstants(class QoreNamespace *ns);

#endif
