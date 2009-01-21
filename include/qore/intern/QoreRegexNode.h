/*
  QoreRegexNode.h

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

/*
  PCRE-based matching (Perl-compatible regular expression matching)
  see: http://www.pcre.org for more information on this library

  NOTE: all regular expression matching is done with UTF-8 encoding, so character set
  encodings are converted if necessary
 */

#ifndef _QORE_QOREREGEXNODE_H

#define _QORE_QOREREGEXNODE_H

#include <qore/intern/QoreRegexBase.h>

class QoreRegexNode : public ParseNoEvalNode, public QoreRegexBase 
{
   private:
      DLLLOCAL void init();

   public:
      DLLLOCAL QoreRegexNode();
      // this version is used while parsing, takes ownership of str
      DLLLOCAL QoreRegexNode(QoreString *str);
      // used at run-time, does not change str
      DLLLOCAL QoreRegexNode(const QoreString *str, int options, ExceptionSink *xsink);
      DLLLOCAL virtual ~QoreRegexNode();

      DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const;
      // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
      DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const;

      // returns the data type
      DLLLOCAL virtual qore_type_t getType() const;
      // returns the type name as a c string
      DLLLOCAL virtual const char *getTypeName() const;      

      DLLLOCAL void concat(char c);
      DLLLOCAL void parse();
      DLLLOCAL void parseRT(const QoreString *pattern, ExceptionSink *xsink);
      DLLLOCAL bool exec(const QoreString *target, ExceptionSink *xsink) const;
      DLLLOCAL QoreListNode *extractSubstrings(const QoreString *target, ExceptionSink *xsink) const;
      // caller owns QoreString returned
      DLLLOCAL QoreString *getString();
};

#endif // _QORE_QOREREGEXNODE_H
