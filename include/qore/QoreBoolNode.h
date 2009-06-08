/*
  QoreBoolNode.h
  
  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

#ifndef _QORE_QOREBOOLNODE_H

#define _QORE_QOREBOOLNODE_H

#include <qore/AbstractQoreNode.h>

/** @file QoreBoolNode.h
    contains definitions related to QoreBoolNode (Qore's boolean data type)
*/

//! base class for Qore's 2 boolean classes: QoreBoolTrueNode and QoreBoolFalseNode
/** @note this class cannot be instantiated; use get_bool_node() to get a pointer to an object of a subclass
 */
class QoreBoolNode : public UniqueValueQoreNode
{
   private:
      //! returns the value as a boolean
      DLLLOCAL virtual bool getAsBoolImpl() const;

      //! returns the value as an integer
      DLLLOCAL virtual int getAsIntImpl() const;

      //! returns the value as a 64-bit integer
      DLLLOCAL virtual int64 getAsBigIntImpl() const;

      //! returns the value as a float
      DLLLOCAL virtual double getAsFloatImpl() const;

   protected:
      //! boolean value for the object
      bool b;

      //! the constructor can only be called by a subclass
      DLLLOCAL QoreBoolNode(bool n_b);

   public:
      DLLEXPORT virtual ~QoreBoolNode();

      // get the value of the type in a string context (default implementation = del = false and returns NullString)
      // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
      // use the QoreStringValueHelper class (defined in QoreStringNode.h) instead of using this function directly
      DLLEXPORT virtual QoreString *getStringRepresentation(bool &del) const;

      // concatenate string representation to a QoreString (no action for complex types = default implementation)
      DLLEXPORT virtual void getStringRepresentation(QoreString &str) const;

      // if del is true, then the returned DateTime * should be deleted, if false, then it should not
      DLLEXPORT virtual DateTime *getDateTimeRepresentation(bool &del) const;

      // assign date representation to a DateTime (no action for complex types = default implementation)
      DLLEXPORT virtual void getDateTimeRepresentation(DateTime &dt) const;

      // get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
      // the ExceptionSink is only needed for QoreObject where a method may be executed
      // use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
      // returns -1 for exception raised, 0 = OK
      DLLEXPORT virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;

      // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
      DLLEXPORT virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

      // the type passed must always be equal to the current type
      DLLEXPORT virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const;
      DLLEXPORT virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      // returns the type name as a c string
      DLLEXPORT virtual const char *getTypeName() const;

      DLLLOCAL static const char *getStaticTypeName()
      {
	 return "bool";
      }

      DLLLOCAL bool getValue() const
      {
	 return b;
      }
};

//! Qore's boolean "true" node, unique, not dynamically-allocated, not reference-counted
/** @note This class cannot be instantiated (there can only be one of these objects in the entire Qore library).
    Use get_bool_node() or simply &True to acquire a pointer to this object
 */
class QoreBoolTrueNode : public QoreBoolNode
{
   public:
      DLLLOCAL QoreBoolTrueNode();
};

//! Qore's boolean "false" node, unique, not dynamically-allocated, not reference-counted
/** @note This class cannot be instantiated (there can only be one of these objects in the entire Qore library).
    Use get_bool_node() or simply &False to acquire a pointer to this object
 */
class QoreBoolFalseNode : public QoreBoolNode
{
   public:
      DLLLOCAL QoreBoolFalseNode();
};

//! Qore's boolean false value
DLLEXPORT extern QoreBoolFalseNode False;

//! Qore's boolean true value
DLLEXPORT extern QoreBoolTrueNode True;

//! a little inline helper function for getting a boolean node
static inline QoreBoolNode *get_bool_node(bool v)
{
   return v ? (QoreBoolNode *)&True : (QoreBoolNode *)&False;
}

#endif
