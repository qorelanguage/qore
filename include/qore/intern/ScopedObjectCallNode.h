/*
 ScopedObjectCallNode.h
 
 Qore Programming Language
 
 Copyright 2003 - 2010 David Nichols
 
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

#ifndef _QORE_SCOPEDOBJECTCALLNODE_H

#define _QORE_SCOPEDOBJECTCALLNODE_H

#include <qore/intern/FunctionCallNode.h>

// objects of this class will not be evaluated
class ScopedObjectCallNode : public AbstractFunctionCallNode {
protected:
      DLLLOCAL virtual int64 bigIntEvalImpl(class ExceptionSink *xsink) const {
         assert(false);
         return 0;
      }
      DLLLOCAL virtual int integerEvalImpl(class ExceptionSink *xsink) const {
         assert(false);
         return 0;
      }
      DLLLOCAL virtual bool boolEvalImpl(class ExceptionSink *xsink) const {
         assert(false);
         return false;
      }
      DLLLOCAL virtual double floatEvalImpl(class ExceptionSink *xsink) const {
         assert(false);
         return 0.0;
      }
      DLLLOCAL virtual AbstractQoreNode *evalImpl(class ExceptionSink *xsink) const {
         assert(false);
         return 0;
      }
      DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, class ExceptionSink *xsink) const {
         assert(false);
         return 0;
      }

public:
   NamedScope *name;
   QoreClass *oc;
   QoreListNode *args;
   QoreString desc;
      
   DLLLOCAL ScopedObjectCallNode(NamedScope *n, QoreListNode *a) : AbstractFunctionCallNode(NT_SCOPE_REF, a, false) {
      name = n; 
      args = a; 
   }
   DLLLOCAL virtual ~ScopedObjectCallNode() {
      delete name; 
   }

   // get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
   // the ExceptionSink is only needed for QoreObject where a method may be executed
   // use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
   // returns -1 for exception raised, 0 = OK
   DLLLOCAL virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;
   // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
   DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;
   
   // returns the data type
   DLLLOCAL virtual qore_type_t getType() const;
   // returns the type name as a c string
   DLLLOCAL virtual const char *getTypeName() const;

   // returns the description
   DLLLOCAL virtual const char *getName() const {
      return desc.getBuffer();
   }
   
   DLLLOCAL AbstractQoreNode *parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);
};

#endif
