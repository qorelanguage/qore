/*
 Tree.h
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006 David Nichols
 
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

// FIXME: add derefWithObjectDelete() method to QoreNode to do a delete in addition to a deref if the qorenode is an object
//        and use this method instead of calling Object::doDelete directly in places around the library

#ifndef _QORE_TREE_H

#define _QORE_TREE_H

class Tree {
private:
   bool ref_rv;

public:
   class Operator *op;
   class QoreNode *left;
   class QoreNode *right;
   
   DLLLOCAL Tree(class QoreNode *l, class Operator *op, class QoreNode *r = NULL);
   DLLLOCAL ~Tree();
   DLLLOCAL class QoreNode *eval(class ExceptionSink *xsink);
   DLLLOCAL bool bool_eval(class ExceptionSink *xsink);
   DLLLOCAL void ignoreReturnValue();
};

#endif
