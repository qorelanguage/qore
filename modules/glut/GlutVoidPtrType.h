/*
  GlutVoidPtrType.h

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

#ifndef _QORE_QT_GLUTVOIDPTRTYPE_H

#define _QORE_QT_GLUTVOIDPTRTYPE_H

DLLLOCAL extern qore_type_t NT_GLUTPTR;

class GlutVoidPtrType : public SimpleValueQoreNode
{
   private:
      void *ptr;

      DLLLOCAL virtual bool getAsBoolImpl() const;
      DLLLOCAL virtual int getAsIntImpl() const;
      DLLLOCAL virtual int64 getAsBigIntImpl() const;
      DLLLOCAL virtual double getAsFloatImpl() const;

   public:
      DLLLOCAL GlutVoidPtrType(void *p) : SimpleValueQoreNode(NT_GLUTPTR), ptr(p)
      {
      }

      DLLLOCAL ~GlutVoidPtrType()
      {
      }

      DLLLOCAL virtual QoreString *getStringRepresentation(bool &del) const;

      DLLLOCAL virtual void getStringRepresentation(QoreString &str) const;

      DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

      DLLLOCAL virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;

      DLLLOCAL virtual class AbstractQoreNode *realCopy() const;

      // the type passed must always be equal to the current type
      DLLLOCAL virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      DLLLOCAL virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      // returns the type name as a c string
      DLLLOCAL virtual const char *getTypeName() const;

      DLLLOCAL void *getPtr() const
      {
	 return ptr;
      }
};

void addGlutVoidPtrType();

static inline const GlutVoidPtrType *test_glutptr_param(const QoreListNode *n, int i)
{
   if (!n) return 0;

   // this is faster, although more verbose, than a dynamic_cast<>
   const AbstractQoreNode *p = n->retrieve_entry(i);
   if (!p || p->getType() != NT_GLUTPTR)
      return 0;
   return reinterpret_cast<const GlutVoidPtrType *>(p);
}

#endif
