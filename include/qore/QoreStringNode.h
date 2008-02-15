/*
  QoreString.h

  QoreString Class Definition

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

#ifndef _QORE_QORESTRINGNODE_H

#define _QORE_QORESTRINGNODE_H

#include <qore/AbstractQoreNode.h>
#include <qore/QoreString.h>

class QoreStringNode : public SimpleQoreNode, public QoreString
{
   private:
      // these functions are not actually implemented
      DLLLOCAL QoreStringNode(QoreString *str);
      DLLLOCAL QoreStringNode& operator=(const QoreStringNode&); // not implemented

      DLLLOCAL virtual bool getAsBoolImpl() const;
      DLLLOCAL virtual int getAsIntImpl() const;
      DLLLOCAL virtual int64 getAsBigIntImpl() const;
      DLLLOCAL virtual double getAsFloatImpl() const;

   protected:
      // destructor only called when references = 0, use deref() instead
      DLLEXPORT virtual ~QoreStringNode();

   public:
      DLLEXPORT QoreStringNode();

      DLLEXPORT QoreStringNode(const char *str, const class QoreEncoding *enc = QCS_DEFAULT);
      // copies str
      DLLEXPORT QoreStringNode(const QoreString &str);
      // copies str
      DLLEXPORT QoreStringNode(const QoreStringNode &str);
      // copies str
      DLLEXPORT QoreStringNode(const std::string &str, const class QoreEncoding *enc = QCS_DEFAULT);
      // copies binary object and makes a base64-encoded string out of it
      DLLEXPORT QoreStringNode(const class BinaryNode *b);
      // takes ownership of the nbuf passed
      DLLEXPORT QoreStringNode(char *nbuf, int nlen, int nallocated, const class QoreEncoding *enc);
      // copies str
      DLLEXPORT QoreStringNode(const char *str, int len, const class QoreEncoding *new_qorecharset = QCS_DEFAULT);

      // creates a string from a single character
      DLLEXPORT QoreStringNode(char c);

      DLLEXPORT virtual bool needs_eval() const;

      // get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
      // the ExceptionSink is only needed for QoreObject where a method may be executed
      // use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
      // returns -1 for exception raised, 0 = OK
      DLLEXPORT int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;
      // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
      DLLEXPORT QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

      // get the value of the type in a string context (0 (NULL) for complex types = default implementation)
      // if del is true, then the returned QoreString should be deleted, if false, then it should not
      DLLEXPORT virtual QoreString *getStringRepresentation(bool &del) const;
      // concatenate string representation to a QoreString (no action for complex types = default implementation)
      DLLEXPORT virtual void getStringRepresentation(QoreString &str) const;

      // if del is true, then the returned DateTime * should be deleted, if false, then it should not
      DLLEXPORT virtual class DateTime *getDateTimeRepresentation(bool &del) const;
      // assign date representation to a DateTime * (no action for complex types = default implementation)
      DLLEXPORT virtual void getDateTimeRepresentation(DateTime &dt) const;

      DLLEXPORT virtual class AbstractQoreNode *realCopy() const;

      DLLEXPORT virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const;
      DLLEXPORT virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      // returns the data type
      DLLEXPORT virtual const QoreType *getType() const;
      DLLEXPORT virtual const char *getTypeName() const;

      DLLEXPORT QoreStringNode *convertEncoding(const class QoreEncoding *nccs, class ExceptionSink *xsink) const;
      DLLEXPORT QoreStringNode *substr(int offset) const;
      DLLEXPORT QoreStringNode *substr(int offset, int length) const;

      // return a QoreString with the characters reversed
      DLLEXPORT QoreStringNode *reverse() const;

      // copy function
      DLLEXPORT QoreStringNode *copy() const;

      // creates a new QoreStringNode from a string and converts its encoding
      DLLEXPORT static QoreStringNode *createAndConvertEncoding(const char *str, const class QoreEncoding *from, const class QoreEncoding *to, ExceptionSink *xsink);
      // parses the string as a base64-encoded binary and returns the decoded value as a QoreStringNode
      DLLEXPORT class QoreStringNode *parseBase64ToString(class ExceptionSink *xsink) const;

      // constructor supporting createAndConvertEncoding()
      DLLLOCAL QoreStringNode(const char *str, const class QoreEncoding *from, const class QoreEncoding *to, ExceptionSink *xsink);
      DLLLOCAL QoreStringNode(struct qore_string_private *p);
};

extern QoreStringNode *NullString;

class QoreStringValueHelper {
   private:
      QoreString *str;
      bool del;

      DLLLOCAL QoreStringValueHelper(const QoreStringValueHelper&); // not implemented
      DLLLOCAL QoreStringValueHelper& operator=(const QoreStringValueHelper&); // not implemented
      DLLLOCAL void* operator new(size_t); // not implemented, make sure it is not new'ed

   public:
      DLLLOCAL QoreStringValueHelper(const AbstractQoreNode *n)
      {
	 if (n) {
	    //optimization to remove the need for a virtual function call in the most common case
	    if (n->type == NT_STRING) {
	       del = false;
	       str = const_cast<QoreStringNode *>(reinterpret_cast<const QoreStringNode *>(n));
	    }
	    else
	       str = n->getStringRepresentation(del);
	 }
	 else {
	    str = NullString;
	    del = false;
	 }
      }
      DLLLOCAL QoreStringValueHelper(const AbstractQoreNode *n, const QoreEncoding *enc, ExceptionSink *xsink)
      {
	 if (n) {
	    //optimization to remove the need for a virtual function call in the most common case
	    if (n->type == NT_STRING) {
	       del = false;
	       str = const_cast<QoreStringNode *>(reinterpret_cast<const QoreStringNode *>(n));
	    }
	    else
	       str = n->getStringRepresentation(del);
	    if (str->getEncoding() != enc) {
	       QoreString *t = str->convertEncoding(enc, xsink);
	       if (!t)
		  return;
	       if (del)
		  delete str;
	       str = t;
	       del = true;
	    }
	 }
	 else {
	    str = NullString;
	    del = false;
	 }
      }
      DLLLOCAL ~QoreStringValueHelper()
      {
	 if (del)
	    delete str;
      }
      DLLLOCAL const QoreString *operator->() { return str; }
      DLLLOCAL const QoreString *operator*() { return str; }
      // returns a copy of the string that the caller owns
      DLLLOCAL QoreString *giveString()
      {
	 if (!str)
	    return 0;
	 if (!del)
	    return str->copy();

	 QoreString *rv = str;
	 del = false;
	 str = 0;
	 return rv;
      }
      DLLLOCAL bool is_temp() const
      {
	 return del;
      }
};

class QoreStringNodeValueHelper {
   private:
      QoreStringNode *str;
      bool temp;

      DLLLOCAL QoreStringNodeValueHelper(const QoreStringNodeValueHelper&); // not implemented
      DLLLOCAL QoreStringNodeValueHelper& operator=(const QoreStringNodeValueHelper&); // not implemented
      DLLLOCAL void* operator new(size_t); // not implemented, make sure it is not new'ed

   public:
      DLLLOCAL QoreStringNodeValueHelper(const AbstractQoreNode *n)
      {
	 if (!n) {
	    str = NullString;
	    temp = false;
	    return;
	 }

	 const QoreType *ntype = n->getType();
	 if (ntype == NT_STRING) {
	    str = const_cast<QoreStringNode *>(reinterpret_cast<const QoreStringNode *>(n));
	    temp = false;
	 }
	 else {
	    str = new QoreStringNode();
	    n->getStringRepresentation(*(static_cast<QoreString *>(str)));
	    temp = true;
	 }
      }
      DLLLOCAL ~QoreStringNodeValueHelper()
      {
	 if (temp)
	    str->deref();
      }
      DLLLOCAL const QoreStringNode *operator->() { return str; }
      DLLLOCAL const QoreStringNode *operator*() { return str; }

      //! returns a referenced value - the caller will own the reference
      /**
	 The string is referenced if necessary (if it was a temporary value)
	 @return the string value, where the caller will own the reference count
      */
      DLLLOCAL QoreStringNode *getReferencedValue()
      {
	 if (temp)
	    temp = false;
	 else if (str)
	    str->ref();
	 return str;
      }

};

class TempQoreStringNode {
   private:
      QoreStringNode *str;

      DLLLOCAL TempQoreStringNode(const TempQoreStringNode&); // not implemented
      DLLLOCAL TempQoreStringNode& operator=(const TempQoreStringNode&); // not implemented
      DLLLOCAL void* operator new(size_t); // not implemented, make sure it is not new'ed

   public:
      DLLLOCAL TempQoreStringNode(QoreStringNode *s)
      {
	 str = s;
      }
      DLLLOCAL ~TempQoreStringNode()
      {
	 if (str)
	    str->deref();
      }
      DLLLOCAL QoreStringNode *operator->() { return str; }
      DLLLOCAL QoreStringNode *operator*() { return str; }
      DLLLOCAL operator bool() const { return str != 0; }
      DLLLOCAL void operator=(QoreStringNode *nv)
      {
	 if (str)
	    str->deref();
	 str = nv;
      }
      DLLLOCAL QoreStringNode *release() 
      {
	 QoreStringNode *rv = str;
	 str = 0;
	 return rv;
      }
};

extern QoreString NothingTypeString;

class QoreNodeAsStringHelper {
   private:
      QoreString *str;
      bool del;

      DLLLOCAL QoreNodeAsStringHelper(const QoreNodeAsStringHelper&); // not implemented
      DLLLOCAL QoreNodeAsStringHelper& operator=(const QoreNodeAsStringHelper&); // not implemented
      DLLLOCAL void* operator new(size_t); // not implemented, make sure it is not new'ed

   public:
      DLLLOCAL QoreNodeAsStringHelper(const AbstractQoreNode *n, int offset, ExceptionSink *xsink)
      {
	 if (n)
	    str = n->getAsString(del, offset, xsink);
	 else {
	    str = &NothingTypeString;
	    del = false;
	 }
      }
      DLLLOCAL ~QoreNodeAsStringHelper()
      {
	 if (del)
	    delete str;
      }
      DLLLOCAL const QoreString *operator->() { return str; }
      DLLLOCAL const QoreString *operator*() { return str; }
      // returns a copy of the string that the caller owns
      DLLLOCAL QoreString *giveString()
      {
	 if (!str)
	    return 0;
	 if (!del)
	    return str->copy();

	 QoreString *rv = str;
	 del = false;
	 str = 0;
	 return rv;
      }
};

#endif
