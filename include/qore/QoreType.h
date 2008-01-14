/*
  QoreType.h
  
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

#ifndef _QORE_QORETYPE_H

#define _QORE_QORETYPE_H

#include <qore/common.h>
#include <qore/node_types.h>

#include <map>

// global default values
DLLEXPORT extern class QoreNode *False, *True, *Nothing, *Null, *Zero, *ZeroFloat, *ZeroDate, *emptyList, *emptyHash;
DLLEXPORT extern class QoreStringNode *NullString;

DLLEXPORT extern class QoreString NothingTypeString, NullTypeString, TrueString, FalseString, EmptyHashString, 
   EmptyListString;

typedef class QoreNode *(*no_arg_func_t)();
typedef class QoreNode *(*single_arg_func_t)(const class QoreNode *, class ExceptionSink *xsink);
typedef class QoreNode *(*eval_opt_deref_func_t)(bool &, const class QoreNode *, class ExceptionSink *xsink);
typedef bool (*needs_eval_func_t)(const class QoreNode *);
typedef bool (*bool_eval_type_func_t)(const class QoreNode *, class ExceptionSink *xsink);
typedef int64 (*bigint_eval_type_func_t)(const class QoreNode *, class ExceptionSink *xsink);
typedef double (*float_eval_type_func_t)(const class QoreNode *, class ExceptionSink *xsink);
typedef class QoreNode *(*convert_func_t)(const class QoreNode *, class ExceptionSink *xsink);
typedef bool (*compare_func_t)(const class QoreNode *, const class QoreNode *, class ExceptionSink *xsink);
typedef void (*delete_func_t)(class QoreNode *);
typedef class QoreString *(*string_func_t)(const class QoreNode *, int format, class ExceptionSink *xsink);

#define QTM_USER_START   200

// sort_compare -1 l < r, 0 l == r, 1 l > r
// compare 0 l = r, 1 l != r
class QoreType {
   private:
      const char *name;
      needs_eval_func_t       f_needs_eval;
      single_arg_func_t       f_eval;
      eval_opt_deref_func_t   f_eval_opt_deref;
      bool_eval_type_func_t   f_bool_eval;
      bigint_eval_type_func_t f_bigint_eval;
      float_eval_type_func_t  f_float_eval;
      convert_func_t          f_convert_to;
      no_arg_func_t           f_default_value;
      single_arg_func_t       f_copy;
      compare_func_t          f_compare;
      delete_func_t           f_delete_contents;
      string_func_t           f_make_string;
      bool is_value;
      bool is_container;
      int  id;

   public:
      // note that this method is not thread safe - should only be called in library or module initialization
      DLLEXPORT QoreType(const char *            p_name, 
			 needs_eval_func_t       p_needs_eval,
			 single_arg_func_t       p_eval, 
			 eval_opt_deref_func_t   p_eval_opt_deref,
			 bool_eval_type_func_t	 p_bool_eval,
			 bigint_eval_type_func_t p_bigint_eval,
			 float_eval_type_func_t  p_float_eval,
			 convert_func_t          p_convert_to, 
			 no_arg_func_t           p_default_value,
			 single_arg_func_t       p_copy,
			 compare_func_t          p_compare,
			 delete_func_t           p_delete_contents,
			 string_func_t           p_make_string,
			 bool   p_is_value, 
			 bool   p_is_container);
      DLLEXPORT int getID() const;
      DLLEXPORT bool isValue() const;
      DLLEXPORT const char *getName() const;
      DLLEXPORT bool needs_eval(const class QoreNode *n) const;
      DLLEXPORT class QoreNode *eval(const class QoreNode *n, class ExceptionSink *xsink) const;
      DLLEXPORT class QoreNode *eval(bool &needs_deref, const class QoreNode *n, class ExceptionSink *xsink) const;
      DLLEXPORT bool bool_eval(const class QoreNode *n, class ExceptionSink *xsink) const;
      DLLEXPORT int64 bigint_eval(const class QoreNode *n, class ExceptionSink *xsink) const;
      DLLEXPORT double float_eval(const class QoreNode *n, class ExceptionSink *xsink) const;
      DLLEXPORT class QoreNode *copy(const class QoreNode *n, class ExceptionSink *xsink) const;
      DLLEXPORT class QoreNode *getDefaultValue() const;
      // compare = 0 means values are equal
      DLLEXPORT bool compare(const class QoreNode *n1, const class QoreNode *n2, class ExceptionSink *xsink) const;
      DLLEXPORT void deleteContents(class QoreNode *n) const;
      DLLEXPORT class QoreString *getAsString(const class QoreNode *n, int format, class ExceptionSink *xsink) const;
};

typedef std::map<int, class QoreType *> qore_type_map_t;

class QoreTypeManager : public qore_type_map_t
{
   friend class QoreType;
   
   private:
      DLLLOCAL static int lastid;
      DLLLOCAL static QoreType *typelist[NUM_VALUE_TYPES];
      
   public:
      DLLEXPORT void add(class QoreType *t);

      DLLLOCAL QoreTypeManager();
      DLLLOCAL ~QoreTypeManager();
      DLLLOCAL static void init();
      DLLLOCAL static void del();
      DLLLOCAL class QoreType *find(int id);
};

DLLEXPORT extern class QoreTypeManager QTM;

DLLEXPORT bool compareHard(const QoreNode *l, const QoreNode *r, class ExceptionSink *xsink);
DLLEXPORT bool compareSoft(const QoreNode *l, const QoreNode *r, class ExceptionSink *xsink);

static inline class QoreNode *nothing()
{
   Nothing->ref();
   return Nothing;
}

static inline class QoreNode *null()
{
   Null->ref();
   return Null;
}

static inline class QoreNode *boolean_false()
{
   False->ref();
   return False;
}

static inline class QoreNode *boolean_true()
{
   True->ref();
   return True;
}

static inline class QoreNode *zero()
{
   Zero->ref();
   return Zero;
}

static inline class QoreNode *zero_float()
{
   ZeroFloat->ref();
   return ZeroFloat;
}

static inline class QoreNode *zero_date()
{
   ZeroDate->ref();
   return ZeroDate;
}

static inline class QoreStringNode *null_string()
{
   NullString->ref();
   return NullString;
}

#endif // _QORE_QORETYPE_H
