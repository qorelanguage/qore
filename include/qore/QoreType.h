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
DLLEXPORT extern class QoreNode *False, *True, *Nothing, *Null, *Zero, *ZeroFloat, *emptyList, *emptyHash;
DLLEXPORT extern class QoreStringNode *NullString;
DLLEXPORT extern class DateTimeNode *ZeroDate;

DLLEXPORT extern class QoreString NothingTypeString, NullTypeString, TrueString, FalseString, EmptyHashString, 
   EmptyListString;

#define QTM_USER_START   200

class QoreType {
   private:
      const char *name;
      int  id;

   public:
      // note that this method is not thread safe - should only be called in library or module initialization
      DLLEXPORT QoreType(const char *p_name);
      DLLEXPORT int getID() const;
      DLLEXPORT const char *getName() const;
      // compare = 0 means values are equal
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

static inline DateTimeNode *zero_date()
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
