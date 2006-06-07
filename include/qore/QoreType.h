/*
  QoreType.h
  
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

#ifndef _QORE_TYPE_H

#define _QORE_TYPE_H

#include <qore/common.h>
#include <qore/node_types.h>

// global default values
extern class QoreNode *False, *True, *Nothing, *Null, *Zero, *ZeroFloat, *NullString, 
   *ZeroDate, *emptyList, *emptyHash;

typedef class QoreNode *(* no_arg_func_t)();
typedef class QoreNode *(* single_arg_func_t)(class QoreNode *, class ExceptionSink *xsink);
//typedef class QoreNode *(* double_arg_func_t)(class QoreNode *, class QoreNode *, class ExceptionSink *xsink);
typedef class QoreNode *(* convert_func_t)(class QoreNode *, class ExceptionSink *xsink);
typedef bool (*compare_func_t)(class QoreNode *, class QoreNode *);
typedef void (*delete_func_t)(class QoreNode *);
typedef class QoreString *(* string_func_t)(class QoreNode *, int format, class ExceptionSink *xsink);

#define QTM_NO_VALUE     false
#define QTM_VALUE        true
#define QTM_NO_CONTAINER false
#define QTM_CONTAINER    true

#define QTM_USER_START   200

// sort_compare -1 l < r, 0 l == r, 1 l > r
// compare 0 l = r, 1 l != r
class QoreType {
   private:
      single_arg_func_t  f_eval;
      convert_func_t     f_convert_to;
      no_arg_func_t      f_default_value;
      single_arg_func_t  f_copy;
      compare_func_t     f_compare;
      delete_func_t      f_delete_contents;
      string_func_t      f_make_string;
      bool is_value;
      bool is_container;
      int  id;

   public:
      char *name;
      class QoreType *next;

      inline QoreType(char *                 p_name, 
		      single_arg_func_t      p_eval, 
		      convert_func_t         p_convert_to, 
		      no_arg_func_t          p_default_value,
		      single_arg_func_t      p_copy,
		      compare_func_t         p_compare,
		      delete_func_t          p_delete_contents,
		      string_func_t          p_make_string,
		      bool   p_is_value, 
		      bool   p_is_container)
      {
	 name                 = p_name;
	 f_eval                 = p_eval;
	 f_convert_to           = p_convert_to;
	 f_default_value        = p_default_value;
	 f_copy                 = p_copy;
	 f_compare              = p_compare;
	 f_delete_contents      = p_delete_contents;
	 f_make_string          = p_make_string;

	 is_value        = p_is_value;
	 is_container    = p_is_container;
      }
      inline void setID(int p_id) { id = p_id; }
      inline int getID() { return id; }
      inline class QoreNode *eval(class QoreNode *n, class ExceptionSink *xsink);
      inline class QoreNode *copy(class QoreNode *n, class ExceptionSink *xsink);
      inline class QoreNode *getDefaultValue();
      inline class QoreNode *convertTo(class QoreNode *n, class ExceptionSink *xsink);
      // compare = 0 means values are equal
      inline bool compare(class QoreNode *n1, class QoreNode *n2);
      inline void deleteContents(class QoreNode *n);
      inline class QoreString *getAsString(class QoreNode *n, int format, class ExceptionSink *xsink);
      inline bool isValue() { return is_value; }
};

class QoreTypeManager {
   private:
      class QoreType *head;
      int num;
      int lastid;

   public:
      QoreType *typelist[NUM_VALUE_TYPES];

      QoreTypeManager();
      ~QoreTypeManager();
      inline void add(class QoreType *t)
      {
	 if (lastid < NUM_VALUE_TYPES)
	    typelist[lastid] = t;
	 t->setID(lastid++);
	 t->next = head;
	 head = t;
      }
      inline QoreType *getHead()
      {
	 return head;
      }      
};

extern class QoreTypeManager QTM;

static inline bool compareHard(QoreNode *l, QoreNode *r);
static inline bool compareSoft(class QoreNode *node1, class QoreNode *node2, class ExceptionSink *xsink);

#include <qore/support.h>
#include <qore/List.h>
#include <qore/QoreNode.h>
#include <qore/QoreString.h>
#include <qore/Operator.h>

#include <string.h>

inline class QoreNode *QoreType::eval(class QoreNode *n, class ExceptionSink *xsink)
{
   if (!f_eval)
   {
      n->ref();
      return n;
   }
   return f_eval(n, xsink);
}

inline class QoreNode *QoreType::getDefaultValue()
{
#ifdef DEBUG
   if (!f_default_value)
      run_time_error("QoreType::getDefaultValue() '%s' has no default value\n", name);
#endif
   return f_default_value();
}

inline class QoreNode *QoreType::convertTo(class QoreNode *n, class ExceptionSink *xsink)
{
#ifdef DEBUG
   if (!f_convert_to)
      run_time_error("QoreType::convertTo() '%s' f_convert_to is NULL\n", name);
#endif
   return f_convert_to(n, xsink);
}

inline class QoreNode *QoreType::copy(class QoreNode *n, class ExceptionSink *xsink)
{
   if (!f_copy)
   {
      class QoreNode *rv = new QoreNode(n->type);
      memcpy(&rv->val, &n->val, sizeof(union node_u));
      return rv;
   }
   return f_copy(n, xsink);
}

inline bool QoreType::compare(class QoreNode *n1, class QoreNode *n2)
{
   if (!f_compare)
      return 0;
   return f_compare(n1, n2);
}

inline void QoreType::deleteContents(class QoreNode *n)
{
   if (f_delete_contents)
      f_delete_contents(n);
}

inline class QoreString *QoreType::getAsString(class QoreNode *n, int format, class ExceptionSink *xsink)
{
   if (f_make_string)
      return f_make_string(n, format, xsink);

   QoreString *rv = new QoreString();
   rv->sprintf("%s (0x%08x)", name, n);
   return rv;
}

static inline void indent(QoreString *str, int num)
{
   for (int i = 0; i < num; i++)
      str->concat(' ');
}

static inline bool compareHard(QoreNode *l, QoreNode *r)
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

   return l->type->compare(l, r);
}

// this function calls the operator function that will                                                                                                                      
// convert values to do the conversion                                                                                                                                      
static inline bool compareSoft(class QoreNode *node1, class QoreNode *node2, class ExceptionSink *xsink)
{
   // logical equals always returns an integer result                                                                                                                       
   class QoreNode *node = OP_LOG_EQ->eval(node1, node2, xsink);
   bool b = node->val.boolval;
   node->deref(NULL);
   return !b;
}

#endif // _QORE_NODETYPE_H
