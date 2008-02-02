/*
 ObjectMethodReference.h
 
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

#ifndef _QORE_OBJECTMETHODREFERENCE_H

#define _QORE_OBJECTMETHODREFERENCE_H

class AbstractParseObjectMethodReferenceNode : public ParseNode
{
   public:
      DLLLOCAL AbstractParseObjectMethodReferenceNode() : ParseNode(NT_OBJMETHREF)
      {
      }
      DLLLOCAL virtual ~AbstractParseObjectMethodReferenceNode() {}
      // get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
      // the ExceptionSink is only needed for QoreObject where a method may be executed
      // use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
      // returns -1 for exception raised, 0 = OK
      DLLLOCAL virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const
      {
	 str.sprintf("object method reference (0x%08p)", this);
	 return 0;
      }

      // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
      DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const
      {
	 del = true;
	 QoreString *rv = new QoreString();
	 getAsString(*rv, foff, xsink);
	 return rv;
      }

      // returns the data type
      DLLLOCAL virtual const QoreType *getType() const
      {
	 return NT_OBJMETHREF;
      }

      // returns the type name as a c string
      DLLLOCAL virtual const char *getTypeName() const
      {
	 return "object method reference";
      }

      // returns a RunTimeObjectMethodReference or NULL if there's an exception
      DLLLOCAL virtual AbstractQoreNode *eval(class ExceptionSink *xsink) const = 0;
      DLLLOCAL virtual int parseInit(lvh_t oflag, int pflag) = 0;
};

class ParseObjectMethodReferenceNode : public AbstractParseObjectMethodReferenceNode
{
   private:
      class AbstractQoreNode *exp;
      char *method;

      DLLLOCAL virtual ~ParseObjectMethodReferenceNode();
   
   public:
      DLLLOCAL ParseObjectMethodReferenceNode(class AbstractQoreNode *n_exp, char *n_method);
      // returns a RunTimeObjectMethodReference or NULL if there's an exception
      DLLLOCAL virtual class AbstractQoreNode *eval(class ExceptionSink *xsink) const;
      DLLLOCAL virtual int parseInit(lvh_t oflag, int pflag);
};

class ParseSelfMethodReferenceNode : public AbstractParseObjectMethodReferenceNode
{
   private:
      char *method;

      DLLLOCAL virtual ~ParseSelfMethodReferenceNode();
   
   public:
      DLLLOCAL ParseSelfMethodReferenceNode(char *n_method);
      // returns a RunTimeObjectMethodReference or NULL if there's an exception
      DLLLOCAL virtual AbstractQoreNode *eval(class ExceptionSink *xsink) const;
      DLLLOCAL virtual int parseInit(lvh_t oflag, int pflag);
};

class ParseScopedSelfMethodReferenceNode : public AbstractParseObjectMethodReferenceNode
{
   private:
      class NamedScope *nscope;
      const class QoreMethod *method;

      DLLLOCAL virtual ~ParseScopedSelfMethodReferenceNode();
      
   public:
      DLLLOCAL ParseScopedSelfMethodReferenceNode(NamedScope *n_nscope);
      // returns a RunTimeObjectMethodReference or NULL if there's an exception
      DLLLOCAL virtual class AbstractQoreNode *eval(ExceptionSink *xsink) const;
      DLLLOCAL virtual int parseInit(lvh_t oflag, int pflag);
};

#endif
