/*
  ReferenceNode.h
  
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

#ifndef _QORE_REFERENCENODE_H

#define _QORE_REFERENCENODE_H 

#include <qore/AbstractQoreNode.h>

//! parse type: reference to a lvalue expression
/** This type could be passed to a builtin function.  To get and set the value of the reference, 
    use the ReferenceArgumentHelper class
    @see ReferenceArgumentHelper
 */
class ReferenceNode : public SimpleValueQoreNode
{
   protected:
      DLLEXPORT virtual ~ReferenceNode();

   public:
      // FIXME: make private and provide functions to access
      //! lvalue expression for reference
      AbstractQoreNode *lvexp;

      //! 
      DLLLOCAL ReferenceNode(AbstractQoreNode *exp);

      // get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
      // the ExceptionSink is only needed for QoreObject where a method may be executed
      // use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
      // returns -1 for exception raised, 0 = OK
      DLLEXPORT virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;
      // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
      DLLEXPORT virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

      DLLEXPORT virtual class AbstractQoreNode *realCopy() const;

      // the type passed must always be equal to the current type
      DLLEXPORT virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const;
      DLLEXPORT virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      // returns the type name as a c string
      DLLEXPORT virtual const char *getTypeName() const;
};

#endif
