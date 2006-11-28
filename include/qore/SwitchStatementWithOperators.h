/*
  SwitchStatementWithOperators.h

  Qore Programming Language

  Copyright (C) 2006 Qore Technologies

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

#ifndef QORE_SWITCHSTATEMENT_WITH_OPERATORS_H
#define QORE_SWITCHSTATEMENT_WITH_OPERATORS_H

#include <qore/SwitchStatement.h>

// Class supporting:
// switch ($a) {
// case > 1: ..
// case < 0.0: ...
// case >= 1.0: ...
// case <= -1: ...
//
class CaseNodeWithOperator : public CaseNode
{
private:
  virtual bool isCaseNodeImpl() const;
public:
  // <=, <, >=, >
  enum comparison_type_t { LessOrEqual, Less, GreaterOrEqual, Greater };
private:
  comparison_type_t m_comparison_type;
public:
  CaseNodeWithOperator(QoreNode* v, StatementBlock* c, comparison_type_t cmp_type)
  : CaseNode(v, c), m_comparison_type(cmp_type) {}
  ~CaseNodeWithOperator() {}

  virtual bool matches(QoreNode* lhs_value);
};

#endif

// EOF

