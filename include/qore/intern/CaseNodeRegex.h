/*
 CaseNodeRegex.h
 
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

#ifndef QORE_CASENODEREGEX_H
#define QORE_CASENODEREGEX_H

#include <qore/intern/SwitchStatement.h>
#include <qore/intern/QoreRegexNode.h>

// Class supporting:
// switch ($a) {
// case ~= /regex_exp/: ..
//
class CaseNodeRegex : public CaseNode
{
protected:
   QoreRegexNode *re;
   
   DLLLOCAL virtual bool isCaseNodeImpl() const
   {
      return false;
   }
   DLLLOCAL virtual bool isDefault() const
   {
      return false;
   }
   
public:
   DLLLOCAL CaseNodeRegex(QoreRegexNode *m_re, StatementBlock *blk) : CaseNode(NULL, blk), re(m_re) 
   {
   }
   DLLLOCAL virtual ~CaseNodeRegex()
   {
      delete re;
   }
   DLLLOCAL virtual bool matches(AbstractQoreNode *lhs_value, class ExceptionSink *xsink);
};

class CaseNodeNegRegex : public CaseNodeRegex
{
public:
   DLLLOCAL CaseNodeNegRegex(QoreRegexNode *m_re, StatementBlock *blk) : CaseNodeRegex(m_re, blk) 
   {
   }
   DLLLOCAL virtual bool matches(AbstractQoreNode *lhs_value, class ExceptionSink *xsink);
};

#endif
