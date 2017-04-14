/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ASTModifiers.h

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

#ifndef _QLS_AST_ASTMODIFIERS_H
#define _QLS_AST_ASTMODIFIERS_H

#include "ASTNode.h"

enum ASTModifier {
    AM_Abstract            = 1 << 0,   //!< The `abstract` modifier.
    AM_Deprecated          = 1 << 1,   //!< The `deprecated` modifier.
    AM_Final               = 1 << 2,   //!< The `final` modifier.
    AM_Static              = 1 << 3,   //!< The `static` modifier.
    AM_Synchronized        = 1 << 4,   //!< The `synchronized` modifier.
    AM_Our                 = 1 << 5,   //!< The `our` modifier.
    AM_My                  = 1 << 6,   //!< The `my` modifier.
    AM_Public              = 1 << 7,   //!< The `public` modifier.
    AM_Private             = 1 << 8,   //!< The `private` modifier.
    AM_PrivateHierarchy    = 1 << 9,   //!< The `private:hierarchy` modifier.
    AM_PrivateInternal     = 1 << 10,   //!< The `private:internal` modifier.
};

class ASTModifiers : public ASTNode {
public:
   ASTModifiers() : ASTNode() {}
   ASTModifiers(unsigned int val) : ASTNode(), value(val) {}

   void set(unsigned int newVal) {
      value = newVal;
   }

   void add(ASTModifier mod) {
      value |= mod;
   }

   void remove(ASTModifier mod) {
      value &= ~mod;
   }

   bool contains(ASTModifier mod) {
      return value & mod;
   }
private:
   unsigned int value;
};

#endif // _QLS_AST_ASTMODIFIERS_H
