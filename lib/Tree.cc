/*
 Tree.cc
 
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

#include <qore/Qore.h>

Tree::Tree(class QoreNode *l, class Operator *o, class QoreNode *r)
{
   left = l;
   op = o;
   right = r;
   ref_rv = true;
}

Tree::~Tree()
{
   if (left)
      left->deref(NULL);
   if (right)
      right->deref(NULL);
}

class QoreNode *Tree::eval(class ExceptionSink *xsink)
{
   return op->eval(left, right, ref_rv, xsink);
}

bool Tree::bool_eval(class ExceptionSink *xsink)
{
   return op->bool_eval(left, right, xsink);
}

void Tree::ignoreReturnValue()
{
   // OPTIMIZATION: change post incremement to pre increment for top-level expressions to avoid extra SMP cache invalidations
   if (op == OP_POST_INCREMENT)
      op = OP_PRE_INCREMENT;
   else if (op == OP_POST_DECREMENT)
      op = OP_PRE_DECREMENT;
      
   ref_rv = false;
}

