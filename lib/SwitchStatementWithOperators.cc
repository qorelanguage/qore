/*
  SwitchStatementWithOperators.cc

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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/QoreNode.h>
#include <qore/Statement.h>
#include <qore/SwitchStatementWithOperators.h>
#include <qore/minitest.hpp>

#ifdef DEBUG
#  include "tests/SwitchStatementWithOperators_tests.cc"
#endif

//-----------------------------------------------------------------------------
bool CaseNodeWithOperator::matches(QoreNode* lhs_value)
{
  if (lhs_value->type != NT_INT && lhs_value->type != NT_FLOAT) {
    return false;
  }
  if (val->type != NT_INT && val->type != NT_FLOAT) {
    return false;
  }

  bool res;
  if (lhs_value->type == NT_INT && val->type == NT_INT) {
    int64 lhs = lhs_value->val.intval;
    int64 rhs = val->val.intval;
    switch (m_comparison_type) {
      case GreaterOrEqual:
        res = lhs >= rhs;
        break;
      case Greater:
        res = lhs > rhs;
        break;
      case LessOrEqual:
        res = lhs <= rhs;
        break;
      case Less:
        res = lhs < rhs;
        break;
    }
  } else {
    double lhs = lhs_value->type == NT_INT ? (double)lhs_value->val.intval : lhs_value->val.floatval;
    double rhs = val->type == NT_INT ? (double)val->val.intval : val->val.floatval;
    switch (m_comparison_type) {
      case GreaterOrEqual:
        res = lhs >= rhs;
        break;
      case Greater:
        res = lhs > rhs;
        break;
      case LessOrEqual:
        res = lhs <= rhs;
        break;
      case Less:
        res = lhs < rhs;
        break;
    }
  }
  return res;
}

// EOF

