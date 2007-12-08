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

class AbstractParseObjectMethodReference
{
public:
   DLLLOCAL virtual ~AbstractParseObjectMethodReference() {}
   // returns a RunTimeObjectMethodReference or NULL if there's an exception
   DLLLOCAL virtual class QoreNode *eval(class ExceptionSink *xsink) const = 0;
   DLLLOCAL virtual int parseInit(lvh_t oflag, int pflag) = 0;
};

class ParseObjectMethodReference : public AbstractParseObjectMethodReference
{
private:
   class QoreNode *exp;
   char *method;
   
public:
   DLLLOCAL ParseObjectMethodReference(class QoreNode *n_exp, char *n_method);
   DLLLOCAL virtual ~ParseObjectMethodReference();
   // returns a RunTimeObjectMethodReference or NULL if there's an exception
   DLLLOCAL virtual class QoreNode *eval(class ExceptionSink *xsink) const;
   DLLLOCAL virtual int parseInit(lvh_t oflag, int pflag);
};

class ParseSelfMethodReference : public AbstractParseObjectMethodReference
{
private:
   char *method;
   
public:
   DLLLOCAL ParseSelfMethodReference(char *n_method);
   DLLLOCAL virtual ~ParseSelfMethodReference();
   // returns a RunTimeObjectMethodReference or NULL if there's an exception
   DLLLOCAL virtual class QoreNode *eval(class ExceptionSink *xsink) const;
   DLLLOCAL virtual int parseInit(lvh_t oflag, int pflag);
};

class ParseScopedSelfMethodReference : public AbstractParseObjectMethodReference
{
private:
   class NamedScope *nscope;
   class QoreMethod *method;
   
public:
   DLLLOCAL ParseScopedSelfMethodReference(class NamedScope *n_nscope);
   DLLLOCAL virtual ~ParseScopedSelfMethodReference();
   // returns a RunTimeObjectMethodReference or NULL if there's an exception
   DLLLOCAL virtual class QoreNode *eval(class ExceptionSink *xsink) const;
   DLLLOCAL virtual int parseInit(lvh_t oflag, int pflag);
};

#endif
