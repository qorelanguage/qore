/*
  RegexSubstNode.h

  regular expression substitution node definition

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

#ifndef _QORE_REGEXSUBSTNODE_H

#define _QORE_REGEXSUBSTNODE_H

#include <qore/intern/QoreRegexBase.h>
#include <sys/types.h>

class RegexSubstNode : public ParseNoEvalNode, public QoreRegexBase
{
   protected:

   private:
      bool global;
      class QoreString *newstr;

      DLLLOCAL void init();
      DLLLOCAL static void concat(QoreString *str, int *ovector, int olen, const char *ptr, const char *target);

   public:
      DLLLOCAL RegexSubstNode();
      // used at run-time
      DLLLOCAL RegexSubstNode(const QoreString *pstr, int opts, ExceptionSink *xsink);
      DLLLOCAL virtual ~RegexSubstNode();

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

      DLLLOCAL void parseRT(const QoreString *pstr, ExceptionSink *xsink);
      DLLLOCAL void parse();
      DLLLOCAL QoreStringNode *exec(const QoreString *target, ExceptionSink *xsink) const;
      DLLLOCAL QoreStringNode *exec(const QoreString *target, const QoreString *newstr, ExceptionSink *xsink) const;
      DLLLOCAL void concatSource(char c);
      DLLLOCAL void concatTarget(char c);
      DLLLOCAL void setGlobal();
      DLLLOCAL QoreString *getPattern() const;
};

#endif // _QORE_REGEXSUBSTNODE_H
