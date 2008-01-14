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

#include <qore/QoreNode.h>
#include <qore/QoreString.h>

class QoreStringNode : public QoreNode, public QoreString
{
   protected:
      // destructor only called when references = 0, use deref() instead
      DLLEXPORT virtual ~QoreStringNode();

   public:
      DLLEXPORT QoreStringNode();

      DLLEXPORT QoreStringNode(const char *str, const class QoreEncoding *enc = QCS_DEFAULT);
      // FIXME: remove this function, not actually implemented
      DLLLOCAL QoreStringNode(QoreString *str);
      // copies str
      DLLEXPORT QoreStringNode(const QoreString &str);
      // copies str
      DLLEXPORT QoreStringNode(const QoreStringNode &str);
      // copies str
      DLLEXPORT QoreStringNode(const std::string &str, const class QoreEncoding *enc = QCS_DEFAULT);
      // copies binary object and makes a base64-encoded string out of it
      DLLEXPORT QoreStringNode(const class BinaryObject *b);
      // takes ownership of the nbuf passed
      DLLEXPORT QoreStringNode(char *nbuf, int nlen, int nallocated, const class QoreEncoding *enc);
      // copies str
      DLLEXPORT QoreStringNode(const char *str, int len, const class QoreEncoding *new_qorecharset = QCS_DEFAULT);

      // creates a string from a single character
      DLLEXPORT QoreStringNode(char c);

      DLLEXPORT virtual bool needs_eval() const;
      DLLEXPORT virtual bool getAsBool() const;
      DLLEXPORT virtual int getAsInt() const;
      DLLEXPORT virtual int64 getAsBigInt() const;
      DLLEXPORT virtual double getAsFloat() const;

      // get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
      DLLEXPORT virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

      // returns 0 if the value is not immediately returnable as a QoreString (without conversion)
      //DLLEXPORT virtual class QoreString *getQoreStringValue() const;
      // returns 0 if the value is not immediately returnable as a const char * (without conversion)
      //DLLEXPORT virtual const char *getStringValue() const;

      // get the value of the type in a string context (0 (NULL) for complex types = default implementation)
      // if del is true, then the returned QoreString should be deleted, if false, then it should not
      DLLEXPORT virtual QoreString *getStringRepresentation(bool &del) const;
      // concatenate string representation to a QoreString (no action for complex types = default implementation)
      DLLEXPORT virtual void getStringRepresentation(QoreString &str) const;

      // if del is true, then the returned DateTime * should be deleted, if false, then it should not
      DLLEXPORT virtual class DateTime *getDateTimeRepresentation(bool &del) const;
      // assign date representation to a DateTime * (no action for complex types = default implementation)
      DLLEXPORT virtual void getDateTimeRepresentation(DateTime &dt) const;

      DLLEXPORT virtual class QoreNode *realCopy(class ExceptionSink *xsink) const;

      DLLEXPORT virtual bool is_equal_soft(const QoreNode *v, ExceptionSink *xsink) const;
      DLLEXPORT virtual bool is_equal_hard(const QoreNode *v, ExceptionSink *xsink) const;

      DLLEXPORT QoreStringNode *convertEncoding(const class QoreEncoding *nccs, class ExceptionSink *xsink) const;
      DLLEXPORT QoreStringNode *substr(int offset) const;
      DLLEXPORT QoreStringNode *substr(int offset, int length) const;

      // return a Qorestring with the characters reversed
      DLLEXPORT QoreStringNode *reverse() const;

      // can be used as an alternative for deref(xsink) - no exception possible when deleting
      DLLEXPORT void deref();

      // copy function
      DLLEXPORT QoreStringNode *copy() const;

      // creates a new QoreStringNode from a string and converts its encoding
      DLLEXPORT static QoreStringNode *createAndConvertEncoding(const char *str, const class QoreEncoding *from, const class QoreEncoding *to, ExceptionSink *xsink);

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
      DLLLOCAL QoreStringValueHelper(const QoreNode *n)
      {
	 if (n)
	    str = n->getStringRepresentation(del);
	 else {
	    str = NullString;
	    del = false;
	 }
      }
      DLLLOCAL QoreStringValueHelper(const QoreNode *n, const QoreEncoding *enc, ExceptionSink *xsink)
      {
	 if (n) {
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
      DLLLOCAL QoreStringNodeValueHelper(QoreNode *n)
      {
	 if (!n) {
	    str = NullString;
	    temp = false;
	 }
	 else if (n->type == NT_STRING) {
	    str = reinterpret_cast<QoreStringNode *>(n);
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
      // takes the referenced value and leaves this object empty, value is referenced if necessary
      DLLLOCAL QoreStringNode *takeReferencedValue()
      {
         QoreStringNode *rv = str;
	 if (str && !temp)
	    str->ref();

         str = 0;
         temp = false;
         return rv;
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
      DLLLOCAL QoreNodeAsStringHelper(const QoreNode *n, int offset, ExceptionSink *xsink)
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
