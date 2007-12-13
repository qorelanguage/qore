/*
  QoreType.cc

  extensible and type system for the Qore programming language

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

#include <qore/Qore.h>
#include <qore/intern/Find.h>
#include <qore/intern/RegexSubst.h>
#include <qore/intern/RegexTrans.h>
#include <qore/intern/QoreRegex.h>
#include <qore/intern/ObjectMethodReference.h>
#include <qore/intern/FunctionReference.h>

// get type functions
#include <qore/intern/QT_NOTHING.h>
#include <qore/intern/QT_bigint.h>
#include <qore/intern/QT_float.h>
#include <qore/intern/QT_string.h>
#include <qore/intern/QT_date.h>
#include <qore/intern/QT_boolean.h>
#include <qore/intern/QT_NULL.h>
#include <qore/intern/QT_binary.h>
#include <qore/intern/QT_list.h>
#include <qore/intern/QT_hash.h>
#include <qore/intern/QT_object.h>
#include <qore/intern/QT_backquote.h>
#include <qore/intern/QT_context.h>
#include <qore/intern/QT_varref.h>

#include <string.h>
#include <assert.h>

#define QTM_NO_VALUE     false
#define QTM_VALUE        true
#define QTM_NO_CONTAINER false
#define QTM_CONTAINER    true

DLLLOCAL int QoreTypeManager::lastid = 0;
DLLLOCAL class QoreType *QoreTypeManager::typelist[NUM_VALUE_TYPES];

DLLEXPORT class QoreTypeManager QTM;

// system types
DLLEXPORT class QoreType *NT_NOTHING, *NT_INT, *NT_FLOAT, *NT_STRING, *NT_DATE,
   *NT_BOOLEAN, *NT_NULL, *NT_BINARY, *NT_LIST, *NT_HASH,
   *NT_OBJECT, *NT_FLIST, *NT_BACKQUOTE, *NT_CONTEXTREF, *NT_COMPLEXCONTEXTREF,
   *NT_VARREF, *NT_TREE, *NT_FIND, *NT_FUNCTION_CALL, *NT_SELF_VARREF,
   *NT_SCOPE_REF, *NT_CONSTANT, *NT_BAREWORD, *NT_REFERENCE, *NT_CONTEXT_ROW,
   *NT_REGEX_SUBST, *NT_REGEX_TRANS, *NT_VLIST, *NT_REGEX, *NT_CLASSREF,
   *NT_OBJMETHREF, *NT_FUNCREF, *NT_FUNCREFCALL;

// default value nodes for builtin types
DLLEXPORT class QoreNode *Nothing, *Null, *Zero, *NullString, *ZeroFloat, *ZeroDate, *True, *False, *emptyList, *emptyHash;

QoreType::QoreType(const char *            p_name, 
		   needs_eval_func_t       p_needs_eval,
		   single_arg_func_t       p_eval, 
		   eval_opt_deref_func_t   p_eval_opt_deref,
		   bool_eval_type_func_t   p_bool_eval,
		   bigint_eval_type_func_t p_bigint_eval,
		   float_eval_type_func_t  p_float_eval,
		   convert_func_t          p_convert_to, 
		   no_arg_func_t           p_default_value,
		   single_arg_func_t       p_copy,
		   compare_func_t          p_compare,
		   delete_func_t           p_delete_contents,
		   string_func_t           p_make_string,
		   bool                    p_is_value, 
		   bool                    p_is_container)
{
   name                   = p_name;
   f_needs_eval           = p_needs_eval;
   f_eval                 = p_eval;
   f_eval_opt_deref       = p_eval_opt_deref;
   f_bool_eval		  = p_bool_eval;
   f_bigint_eval	  = p_bigint_eval;
   f_float_eval		  = p_float_eval;
   f_convert_to           = p_convert_to;
   f_default_value        = p_default_value;
   f_copy                 = p_copy;
   f_compare              = p_compare;
   f_delete_contents      = p_delete_contents;
   f_make_string          = p_make_string;
   
   is_value        = p_is_value;
   is_container    = p_is_container;
   
   id = QoreTypeManager::lastid++;
}

class QoreNode *QoreType::eval(class QoreNode *n, class ExceptionSink *xsink)
{
   if (!f_eval)
      return n->RefSelf();
   return f_eval(n, xsink);
}

bool QoreType::needs_eval(class QoreNode *n) const
{
   if (f_needs_eval)
      return f_needs_eval(n);
   if (is_value)
      return false;
   return true;
}

class QoreNode *QoreType::eval(bool &needs_deref, class QoreNode *n, class ExceptionSink *xsink)
{
   if (!f_eval_opt_deref)
   {
      if (is_value)
      {
	 needs_deref = false;
	 return n;
      }
      class QoreNode *rv = eval(n, xsink);
      if (!rv)
	 needs_deref = false;
      else
	 needs_deref = true;
      return rv;
   }
   class QoreNode *rv = f_eval_opt_deref(needs_deref, n, xsink);
   if (!rv && needs_deref)
      needs_deref = false;
   return rv;
}

bool QoreType::bool_eval(class QoreNode *n, class ExceptionSink *xsink)
{
   if (!f_bool_eval)
   {
      bool needs_deref;
      class QoreNode *v = eval(needs_deref, n, xsink);
      if (!v)
	 return false;
      bool rv = v->getAsBool();
      if (needs_deref) v->deref(xsink);
      return rv;
   }
   return f_bool_eval(n, xsink);
}

int64 QoreType::bigint_eval(class QoreNode *n, class ExceptionSink *xsink)
{
   if (!f_bigint_eval)
   {
      bool needs_deref;
      class QoreNode *v = eval(needs_deref, n, xsink);
      if (!v)
	 return 0;
      int64 rv = v->getAsBigInt();
      if (needs_deref) v->deref(xsink);
      return rv;
   }
   return f_bigint_eval(n, xsink);
}

double QoreType::float_eval(class QoreNode *n, class ExceptionSink *xsink)
{
   if (!f_float_eval)
   {
      bool needs_deref;
      class QoreNode *v = eval(needs_deref, n, xsink);
      if (!v)
	 return 0.0;
      double rv = v->getAsFloat();
      if (needs_deref) v->deref(xsink);
      return rv;
   }
   return f_float_eval(n, xsink);
}

class QoreNode *QoreType::getDefaultValue()
{
   assert(f_default_value);
   return f_default_value();
}

class QoreNode *QoreType::convertTo(class QoreNode *n, class ExceptionSink *xsink)
{
   assert(f_convert_to);
   return f_convert_to(n, xsink);
}

class QoreNode *QoreType::copy(class QoreNode *n, class ExceptionSink *xsink)
{
   if (!f_copy)
   {
      class QoreNode *rv = new QoreNode(n->type);
      memcpy(&rv->val, &n->val, sizeof(union node_u));
      return rv;
   }
   return f_copy(n, xsink);
}

void QoreTypeManager::add(class QoreType *t)
{
   if (t->getID() < NUM_VALUE_TYPES)
      typelist[t->getID()] = t;

   insert(std::make_pair(t->getID(), t));
}

bool QoreType::compare(class QoreNode *n1, class QoreNode *n2, class ExceptionSink *xsink) const
{
   if (!f_compare)
      return 0;
   return f_compare(n1, n2, xsink);
}

void QoreType::deleteContents(class QoreNode *n)
{
   if (f_delete_contents)
      f_delete_contents(n);
}

class QoreString *QoreType::getAsString(class QoreNode *n, int format, class ExceptionSink *xsink) const
{
   if (f_make_string)
      return f_make_string(n, format, xsink);
   
   QoreString *rv = new QoreString();
   rv->sprintf("%s (0x%08p)", name, n);
   return rv;
}

int QoreType::getID() const
{ 
   return id; 
}

bool QoreType::isValue() const
{ 
   return is_value; 
}

const char *QoreType::getName() const
{
   return name;
}

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
   return n->val.tree->eval(xsink);
}

static bool tree_bool_eval(class QoreNode *n, class ExceptionSink *xsink)
{
   return n->val.tree->bool_eval(xsink);
}

static void tree_DeleteContents(class QoreNode *n)
{
   delete n->val.tree;
}

// this should never be executed
static class QoreNode *INVALID_COPY(class QoreNode *n, class ExceptionSink *xsink)
{
   assert(false);
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
   assert(getStackObject());
   return getStackObject()->evalMemberNoMethod(n->val.c_str, xsink);
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

static class QoreNode *objmethref_eval(class QoreNode *n, class ExceptionSink *xsink)
{
   return n->val.objmethref->eval(xsink);
}

static void objmethref_del(class QoreNode *n)
{
   delete n->val.objmethref;
}

static class QoreNode *funcref_eval(class QoreNode *n, class ExceptionSink *xsink)
{
   return n->val.funcref->eval(n);
}

static class QoreNode *funcrefcall_eval(class QoreNode *n, class ExceptionSink *xsink)
{
   return n->val.funcrefcall->eval(xsink);
}

static void funcrefcall_del(class QoreNode *n)
{
   delete n->val.funcrefcall;
}

// FIXME: eliminate this crap by using a class hierarchy and virtual methods
QoreTypeManager::QoreTypeManager()
{
   tracein("QoreTypeManager::QoreTypeManager()");
   
   // register system data types
   // first, value types for operator matrix optimization
   add(NT_NOTHING = new QoreType("nothing", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NOTHING_DefaultValue, NULL, NULL, NULL, NOTHING_MakeString, QTM_VALUE, QTM_NO_CONTAINER));
   add(NT_INT = new QoreType("integer", NULL, NULL, NULL, NULL, NULL, NULL, bigint_ConvertTo, bigint_DefaultValue, NULL, bigint_Compare, NULL, bigint_MakeString, QTM_VALUE, QTM_NO_CONTAINER));
   add(NT_FLOAT = new QoreType("float", NULL, NULL, NULL, NULL, NULL, NULL, float_ConvertTo, float_DefaultValue, NULL, float_Compare, NULL, float_MakeString, QTM_VALUE, QTM_NO_CONTAINER));
   add(NT_STRING = new QoreType("string", NULL, NULL, NULL, NULL, NULL, NULL, string_ConvertTo, string_DefaultValue, string_Copy, string_Compare, string_DeleteContents, string_MakeString, QTM_VALUE, QTM_NO_CONTAINER));
   add(NT_DATE = new QoreType("date", NULL, NULL, NULL, NULL, NULL, NULL, date_ConvertTo, date_DefaultValue, date_Copy, date_Compare, date_DeleteContents, date_MakeString, QTM_VALUE, QTM_NO_CONTAINER));
   add(NT_BOOLEAN = new QoreType("boolean", NULL, NULL, NULL, NULL, NULL, NULL, boolean_ConvertTo, boolean_DefaultValue, NULL, boolean_Compare, NULL, boolean_MakeString, QTM_VALUE, QTM_NO_CONTAINER));
   add(NT_NULL = new QoreType("NULL", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL_DefaultValue, NULL, NULL, NULL, NULL_MakeString, QTM_VALUE, QTM_NO_CONTAINER));
   add(NT_BINARY = new QoreType("binary", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, binary_Copy, binary_Compare, binary_DeleteContents, binary_MakeString, QTM_VALUE, QTM_NO_CONTAINER));
   add(NT_LIST = new QoreType("list", list_needs_eval, list_Eval, list_eval_opt_deref, NULL, NULL, NULL, list_ConvertTo, list_DefaultValue, list_Copy, list_Compare, list_DeleteContents, list_MakeString, QTM_VALUE, QTM_CONTAINER));
   add(NT_HASH = new QoreType("hash", hash_needs_eval, hash_Eval, hash_eval_opt_deref, NULL, NULL, NULL, hash_ConvertTo, hash_DefaultValue, hash_Copy, hash_Compare, hash_DeleteContents, hash_MakeString, QTM_VALUE, QTM_CONTAINER));
   add(NT_OBJECT = new QoreType("object", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, object_Copy, object_Compare, NULL, object_MakeString, QTM_VALUE, QTM_CONTAINER));

   // now parse types
   add(NT_FLIST = new QoreType("flist", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, list_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_BACKQUOTE = new QoreType("backquote", NULL, backquote_Eval, NULL, NULL, NULL, NULL, NULL, NULL, simpleStringCopy, NULL, simpleStringDelete, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_CONTEXTREF = new QoreType("context reference", NULL, contextref_Eval, NULL, NULL, NULL, NULL, NULL, NULL, simpleStringCopy, NULL, temp_crefDelete, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_COMPLEXCONTEXTREF = new QoreType("complex context reference", NULL, complexcontextref_Eval, NULL, NULL, NULL, NULL, NULL, NULL, complexcontextref_Copy, NULL, complexcontextref_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_VARREF = new QoreType("variable reference", NULL, varref_Eval, varref_eval_opt_deref, NULL, NULL, NULL, NULL, NULL, varref_Copy, NULL, varref_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_TREE = new QoreType("expression tree", NULL, tree_Eval, NULL, tree_bool_eval, NULL, NULL, NULL, NULL, INVALID_COPY, NULL, tree_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_FIND = new QoreType("find", NULL, find_Eval, NULL, NULL, NULL, NULL, NULL, NULL, INVALID_COPY, NULL, find_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_FUNCTION_CALL = new QoreType("function call", NULL, fcall_Eval, NULL, NULL, NULL, NULL, NULL, NULL, INVALID_COPY, NULL, fcall_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_SELF_VARREF = new QoreType("in-object variable reference", NULL, selfref_Eval, NULL, NULL, NULL, NULL, NULL, NULL, simpleStringCopy, NULL, simpleStringDelete, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_SCOPE_REF = new QoreType("scoped object call", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, INVALID_COPY, NULL, scoped_call_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_CONSTANT = new QoreType("constant reference", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, INVALID_COPY, NULL, scoped_ref_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_BAREWORD = new QoreType("bareword", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, simpleStringCopy, NULL, simpleStringDelete, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_REFERENCE = new QoreType("reference to lvalue", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, INVALID_COPY, NULL, ref_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_CONTEXT_ROW = new QoreType("get context row", NULL, contextrow_Eval, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_REGEX_SUBST = new QoreType("regular expression substitution", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, INVALID_COPY, NULL, regexsubst_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_REGEX_TRANS = new QoreType("transliteration", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, INVALID_COPY, NULL, regextrans_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_VLIST = new QoreType("variable list", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, list_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_REGEX = new QoreType("regular expression", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, INVALID_COPY, NULL, regex_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_CLASSREF = new QoreType("class reference", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, INVALID_COPY, NULL, classref_DeleteContents, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_OBJMETHREF = new QoreType("object method reference", NULL, objmethref_eval, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, objmethref_del, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_FUNCREF = new QoreType("call reference", NULL, funcref_eval, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));
   add(NT_FUNCREFCALL = new QoreType("call reference call", NULL, funcrefcall_eval, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, funcrefcall_del, NULL, QTM_NO_VALUE, QTM_NO_CONTAINER));

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
   ZeroDate    = new QoreNode(new DateTime((int64)0));
   
   emptyList   = new QoreNode(new QoreList());
   emptyHash   = new QoreNode(new QoreHash());
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
   qore_type_map_t::iterator i;
   while ((i = begin()) != end())
   {
      delete i->second;
      erase(i);
   }

   traceout("QoreTypeManager::~QoreTypeManager()");
}

class QoreType *QoreTypeManager::find(int id)
{
   if (id < NUM_VALUE_TYPES)
      return typelist[id];

   qore_type_map_t::const_iterator i = qore_type_map_t::find(id);
   if (i == end())
      return NULL;
   return i->second;
}

bool compareHard(QoreNode *l, QoreNode *r, class ExceptionSink *xsink)
{
   if (is_nothing(l))
      if (is_nothing(r))
         return 0;
      else
         return 1;
   if (is_nothing(r))
      return 1;
   if (l->type != r->type)
      return 1;
   
   return l->type->compare(l, r, xsink);
}

// this function calls the operator function that will                                                                                                                      
// convert values to do the conversion                                                                                                                                      
bool compareSoft(class QoreNode *node1, class QoreNode *node2, class ExceptionSink *xsink)
{
   // logical equals always returns an integer result                                                                                                                       
   return !OP_LOG_EQ->bool_eval(node1, node2, xsink);
}
