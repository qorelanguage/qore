/*
  QoreType.cc

  extensible and type system for the Qore programming language

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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/QoreNode.h>
#include <qore/QoreType.h>
#include <qore/support.h>
#include <qore/Variable.h>
#include <qore/Operator.h>
#include <qore/Function.h>
#include <qore/Find.h>
#include <qore/Object.h>
#include <qore/QoreString.h>
#include <qore/DateTime.h>
#include <qore/qore_thread.h>
#include <qore/QoreProgram.h>
#include <qore/Namespace.h>
#include <qore/Context.h>
#include <qore/RegexSubst.h>
#include <qore/RegexTrans.h>
#include <qore/QoreRegex.h>

// get type functions
#include <qore/QT_NOTHING.h>
#include <qore/QT_bigint.h>
#include <qore/QT_float.h>
#include <qore/QT_string.h>
#include <qore/QT_date.h>
#include <qore/QT_boolean.h>
#include <qore/QT_NULL.h>
#include <qore/QT_binary.h>
#include <qore/QT_list.h>
#include <qore/QT_hash.h>
#include <qore/QT_object.h>
#include <qore/QT_backquote.h>
#include <qore/QT_context.h>
#include <qore/QT_varref.h>

class QoreTypeManager QTM;

// system types
class QoreType *NT_NOTHING, *NT_INT, *NT_FLOAT, *NT_STRING, *NT_DATE,
   *NT_BOOLEAN, *NT_NULL, *NT_BINARY, *NT_LIST, *NT_HASH,
   *NT_OBJECT, *NT_FLIST, *NT_BACKQUOTE, *NT_CONTEXTREF, *NT_COMPLEXCONTEXTREF,
   *NT_VARREF, *NT_TREE, *NT_FIND, *NT_FUNCTION_CALL, *NT_SELF_VARREF,
   *NT_SCOPE_REF, *NT_CONSTANT, *NT_BAREWORD, *NT_REFERENCE, *NT_CONTEXT_ROW,
   *NT_REGEX_SUBST, *NT_REGEX_TRANS, *NT_VLIST, *NT_REGEX, *NT_CLASSREF;

// default value nodes for builtin types
class QoreNode *Nothing, *Null, *Zero, *NullString, *ZeroFloat, *ZeroDate, *True, *False, *emptyList, *emptyHash;

static class QoreNode *simpleStringCopy(class QoreNode *orig, class ExceptionSink *xsink)
{
   QoreNode *n = new QoreNode(orig->type);
   if (orig->val.c_str)
      n->val.c_str = strdup(orig->val.c_str);
   else
      n->val.c_str = NULL;
   return n;
}

static void simpleStringDelete(class QoreNode *n)
{
   if (n->val.c_str)
      free(n->val.c_str);
}

static void temp_crefDelete(class QoreNode *n)
{
   //printd(5, "context ref, %s, %08p, %08p, delete\n", n->val.c_str, n, n->val.c_str);
   if (n->val.c_str)
      free(n->val.c_str);
}

static class QoreNode *tree_Eval(class QoreNode *n, class ExceptionSink *xsink)
{
   return n->val.tree.eval(xsink);
}

static void tree_DeleteContents(class QoreNode *n)
{
   if (n->val.tree.left)
      n->val.tree.left->deref(NULL);
   if (n->val.tree.right)
      n->val.tree.right->deref(NULL);
}

// this should never be executed
static class QoreNode *INVALID_COPY(class QoreNode *n, class ExceptionSink *xsink)
{
#ifdef DEBUG
   run_time_error("copy attempted of type %s", n->type->name);
#endif
   return NULL;
}

static class QoreNode *find_Eval(class QoreNode *n, class ExceptionSink *xsink)
{
   return n->val.find->eval(xsink);
}

static void find_DeleteContents(class QoreNode *n)
{
   delete n->val.find;
}

static class QoreNode *fcall_Eval(class QoreNode *n, class ExceptionSink *xsink)
{
   return n->val.fcall->eval(xsink);
}

static void fcall_DeleteContents(class QoreNode *n)
{
   delete n->val.fcall;
}

static class QoreNode *selfref_Eval(class QoreNode *n, class ExceptionSink *xsink)
{
   return internalObjectVarRef(n, xsink);
}

static void scoped_call_DeleteContents(class QoreNode *n)
{
   delete n->val.socall;
}

static void scoped_ref_DeleteContents(class QoreNode *n)
{
   delete n->val.scoped_ref;
}

static void classref_DeleteContents(class QoreNode *n)
{
   delete n->val.classref;
}

static void ref_DeleteContents(class QoreNode *n)
{
   // this is a parse expression, therefore no exception can happen with the dereference
   n->val.lvexp->deref(NULL);
}

static class QoreNode *contextrow_Eval(class QoreNode *n, class ExceptionSink *xsink)
{
   return evalContextRow(xsink);
}

static void regexsubst_DeleteContents(class QoreNode *n)
{
   delete n->val.resub;
}

static void regextrans_DeleteContents(class QoreNode *n)
{
   delete n->val.retrans;
}

static void regex_DeleteContents(class QoreNode *n)
{
   delete n->val.regex;
}

QoreTypeManager::QoreTypeManager()
{
   tracein("QoreTypeManager::QoreTypeManager()");

   head = NULL;
   lastid = num = 0;
   
   // register system data types
   // first, value types for operator matrix optimization
   add(NT_NOTHING = new QoreType("nothing", NULL, NULL, NOTHING_DefaultValue, NULL, NULL, NULL, NOTHING_MakeString, QTM_VALUE, QTM_NO_CONTAINER));
   add(NT_INT = new QoreType("integer", NULL, bigint_ConvertTo, bigint_DefaultValue, NULL, bigint_Compare, NULL, bigint_MakeString, QTM_VALUE, QTM_NO_CONTAINER));
   add(NT_FLOAT = new QoreType("float", NULL, float_ConvertTo, float_DefaultValue, NULL, float_Compare, NULL, float_MakeString, QTM_VALUE, QTM_NO_CONTAINER));
   add(NT_STRING = new QoreType("string", NULL, string_ConvertTo, string_DefaultValue, string_Copy, string_Compare, string_DeleteContents, string_MakeString, QTM_VALUE, QTM_NO_CONTAINER));
   add(NT_DATE = new QoreType("date", NULL, date_ConvertTo, date_DefaultValue, date_Copy, date_Compare, date_DeleteContents, date_MakeString, QTM_VALUE, QTM_NO_CONTAINER));
   add(NT_BOOLEAN = new QoreType("boolean", NULL, boolean_ConvertTo, boolean_DefaultValue, NULL, boolean_Compare, NULL, boolean_MakeString, QTM_VALUE, QTM_NO_CONTAINER));
   add(NT_NULL = new QoreType("NULL", NULL, NULL, NULL_DefaultValue, NULL, NULL, NULL, NULL_MakeString, QTM_VALUE, QTM_NO_CONTAINER));
   add(NT_BINARY = new QoreType("binary", NULL, NULL, NULL, binary_Copy, binary_Compare, binary_DeleteContents, binary_MakeString, QTM_VALUE, QTM_NO_CONTAINER));
   add(NT_LIST = new QoreType("list", list_Eval, list_ConvertTo, list_DefaultValue, list_Copy, list_Compare, list_DeleteContents, list_MakeString, QTM_VALUE, QTM_CONTAINER));
   add(NT_HASH = new QoreType("hash", hash_Eval, hash_ConvertTo, hash_DefaultValue, hash_Copy, hash_Compare, hash_DeleteContents, hash_MakeString, QTM_VALUE, QTM_CONTAINER));
   add(NT_OBJECT = new QoreType("object", /*object_Eval*/ NULL, NULL, NULL, object_Copy, object_Compare, NULL, object_MakeString, QTM_VALUE, QTM_CONTAINER));

   // now parse types
   add(NT_FLIST = new QoreType("flist", NULL, NULL, NULL, NULL, NULL, list_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_BACKQUOTE = new QoreType("backquote", backquote_Eval, NULL, NULL, simpleStringCopy, NULL, simpleStringDelete, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_CONTEXTREF = new QoreType("context reference", contextref_Eval, NULL, NULL, simpleStringCopy, NULL, temp_crefDelete, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_COMPLEXCONTEXTREF = new QoreType("complex context reference", complexcontextref_Eval, NULL, NULL, complexcontextref_Copy, NULL, complexcontextref_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_VARREF = new QoreType("variable reference", varref_Eval, NULL, NULL, varref_Copy, NULL, varref_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_TREE = new QoreType("tree", tree_Eval, NULL, NULL, INVALID_COPY, NULL, tree_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_FIND = new QoreType("find", find_Eval, NULL, NULL, INVALID_COPY, NULL, find_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_FUNCTION_CALL = new QoreType("function call", fcall_Eval, NULL, NULL, INVALID_COPY, NULL, fcall_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_SELF_VARREF = new QoreType("in-object variable reference", selfref_Eval, NULL, NULL, simpleStringCopy, NULL, simpleStringDelete, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_SCOPE_REF = new QoreType("scoped object call", NULL, NULL, NULL, INVALID_COPY, NULL, scoped_call_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_CONSTANT = new QoreType("constant reference", NULL, NULL, NULL, INVALID_COPY, NULL, scoped_ref_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_BAREWORD = new QoreType("bareword", NULL, NULL, NULL, simpleStringCopy, NULL, simpleStringDelete, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_REFERENCE = new QoreType("reference to lvalue", NULL, NULL, NULL, INVALID_COPY, NULL, ref_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_CONTEXT_ROW = new QoreType("get context row", contextrow_Eval, NULL, NULL, NULL, NULL, NULL, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_REGEX_SUBST = new QoreType("regular expression substitution", NULL, NULL, NULL, INVALID_COPY, NULL, regexsubst_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_REGEX_TRANS = new QoreType("regular expression translation", NULL, NULL, NULL, INVALID_COPY, NULL, regextrans_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_VLIST = new QoreType("variable list", NULL, NULL, NULL, NULL, NULL, list_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_REGEX = new QoreType("regular expression", NULL, NULL, NULL, INVALID_COPY, NULL, regex_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_CLASSREF = new QoreType("class reference", NULL, NULL, NULL, INVALID_COPY, NULL, classref_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));

   // from now on, assign IDs in the user space 
   lastid = QTM_USER_START;

   traceout("QoreTypeManager::QoreTypeManager()");
}

// at least the NullString must be created after the default character encoding is set
void QoreTypeManager::init()
{
   // initialize global default values
   False       = new QoreNode(false);
   True        = new QoreNode(true);
   Nothing     = new QoreNode(NT_NOTHING);
   Null        = new QoreNode(NT_NULL);
   Zero        = new QoreNode((int64)0);
   ZeroFloat   = new QoreNode(0.0);
   NullString  = new QoreNode("");
   ZeroDate    = new QoreNode(new DateTime(0ll));
   
   emptyList   = new QoreNode(new List());
   emptyHash   = new QoreNode(new Hash());
}

void QoreTypeManager::del()
{
   // dereference global default values
   True->deref(NULL);
   False->deref(NULL);
   Nothing->deref(NULL);
   Null->deref(NULL);
   Zero->deref(NULL);
   ZeroFloat->deref(NULL);
   NullString->deref(NULL);
   ZeroDate->deref(NULL);
   emptyList->deref(NULL);
   emptyHash->deref(NULL);
}

QoreTypeManager::~QoreTypeManager()
{
   tracein("QoreTypeManager::~QoreTypeManager()");

   // delete all stored types
   while (head)
   {
      class QoreType *w = head->next;
      delete head;
      head = w;
   }

   traceout("QoreTypeManager::~QoreTypeManager()");
}
