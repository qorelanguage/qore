/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ASTOperator.h

  Qore Programming Language

  Copyright (C) 2017 Qore Technologies, s.r.o.

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#ifndef _QLS_AST_ASTOPERATOR_H
#define _QLS_AST_ASTOPERATOR_H

#include "ASTNode.h"

enum ASTOperatorKind {
   AOK_None = 0,
   AOK_Background,
   AOK_Chomp,
   AOK_Delete,
   AOK_Elements,
   AOK_Exists,
   AOK_Extract,
   AOK_Foldl,
   AOK_Foldr,
   AOK_Instanceof,
   AOK_Keys,
   AOK_Map,
   AOK_New,
   AOK_Pop,
   AOK_Push,
   AOK_Remove,
   AOK_Select,
   AOK_Shift,
   AOK_Splice,
   AOK_Trim,
   AOK_Unshift,
   AOK_Reference,
   AOK_PreIncrement,
   AOK_PostIncrement,
   AOK_PreDecrement,
   AOK_PostDecrement,
   AOK_Plus,
   AOK_Minus,
   AOK_Multiply,
   AOK_Divide,
   AOK_Modulo,
   AOK_UnaryPlus,
   AOK_UnaryMinus,
   AOK_BinaryAnd,
   AOK_BinaryOr,
   AOK_BinaryXor,
   AOK_BinaryNot,
   AOK_ShiftLeft,
   AOK_ShiftRight,
   AOK_Assignment,
   AOK_PlusEquals,
   AOK_MinusEquals,
   AOK_MultiplyEquals,
   AOK_DivideEquals,
   AOK_ModuloEquals,
   AOK_AndEquals,
   AOK_OrEquals,
   AOK_XorEquals,
   AOK_ShiftLeftEquals,
   AOK_ShiftRightEquals,
   AOK_Equals,
   AOK_NotEquals,
   AOK_Comparison,
   AOK_GreaterThan,
   AOK_GreaterThanOrEquals,
   AOK_LessThan,
   AOK_LessThanOrEquals,
   AOK_AbsoluteEquals,
   AOK_AbsoluteNotEquals,
   AOK_LogicalAnd,
   AOK_LogicalOr,
   AOK_LogicalNot,
   AOK_NullCoalesce,
   AOK_ValueCoalesce,
   AOK_RegexMatch,
   AOK_RegexNotMatch,
   AOK_BrokenLogicalOr,
   AOK_BrokenBinaryOr,
   AOK_BrokenBinaryXor,
   AOK_WS_LOGICAL_LE,
   AOK_WS_LOGICAL_GE,
   AOK_WS_LOGICAL_NE,
   AOK_WS_LOGICAL_EQ,
   AOK_WS_ABSOLUTE_EQ,
   AOK_WS_ABSOLUTE_NE,
   AOK_WS_LOGICAL_CMP,
   AOK_WS_LOGICAL_AND,
   AOK_WS_BROKEN_LOGICAL_OR,
   AOK_WS_SHIFT_RIGHT,
   AOK_WS_SHIFT_LEFT,
   AOK_WS_PLUS_EQUALS,
   AOK_WS_MINUS_EQUALS,
   AOK_WS_AND_EQUALS,
   AOK_WS_OR_EQUALS,
   AOK_WS_MODULA_EQUALS,
   AOK_WS_MULTIPLY_EQUALS,
   AOK_WS_DIVIDE_EQUALS,
   AOK_WS_XOR_EQUALS,
   AOK_WS_SHIFT_RIGHT_EQUALS,
   AOK_WS_SHIFT_LEFT_EQUALS,
};

class ASTOperator {
public:
   ASTOperatorKind op;

   ASTOperator() : op(AOK_None) {}
   ASTOperator(ASTOperatorKind opKind) : op(opKind) {}
};

#endif // _QLS_AST_ASTOPERATOR_H
