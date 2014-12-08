/*
  QoreHashMapOperatorNode.cpp
 
  Qore Programming Language
 
  Copyright (C) 2003 - 2014 David Nichols
  
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

#include <qore/Qore.h>

#include <qore/intern/qore_program_private.h>

QoreString QoreHashMapOperatorNode::map_str("map operator expression");

// if del is true, then the returned QoreString * should be mapd, if false, then it must not be
QoreString *QoreHashMapOperatorNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = false;
   return &map_str;
}

int QoreHashMapOperatorNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.concat(&map_str);
   return 0;
}

AbstractQoreNode* QoreHashMapOperatorNode::evalImpl(ExceptionSink *xsink) const {
   return map(xsink);
}

AbstractQoreNode* QoreHashMapOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = ref_rv;
   return map(xsink);
}

AbstractQoreNode* QoreHashMapOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, 
                                                         const QoreTypeInfo *&typeInfo) {
   assert(!typeInfo);
   
   pflag &= ~PF_RETURN_VALUE_IGNORED;

   // check iterated expression
   const QoreTypeInfo* expTypeInfo = 0;
   e[0] = e[0]->parseInit(oflag, pflag, lvids, expTypeInfo);

   // check iterator expression2
   const QoreTypeInfo* expTypeInfo2 = 0;
   e[1] = e[1]->parseInit(oflag, pflag, lvids, expTypeInfo2);

   // check iteratorTypeInfo expression
   const QoreTypeInfo* iteratorTypeInfo = 0;
   e[2] = e[2]->parseInit(oflag, pflag, lvids, iteratorTypeInfo);

   // FIXME: if iterator is a list or an iterator, then the return type is a list, 
   //        otherwise it's the return type of the iterated expression
   return this;
}

AbstractQoreNode* QoreHashMapOperatorNode::map(ExceptionSink* xsink) const {
   // conditionally evaluate argument
   QoreNodeEvalOptionalRefHolder arg_lst(e[2], xsink);
   if (*xsink || is_nothing(*arg_lst))
      return 0;
   
   qore_type_t arglst_type = get_node_type(*arg_lst);
   ReferenceHolder<QoreHashNode> ret_val(ref_rv ? new QoreHashNode() : 0, xsink);
   if (NT_LIST != arglst_type) { // Single value
      // check if it's an AbstractIterator object
      if (NT_OBJECT == arglst_type) {
         AbstractIteratorHelper h(xsink, "hmap operator", 
                const_cast<QoreObject*>(reinterpret_cast<const QoreObject*>(*arg_lst)));
         if (*xsink)
            return 0;
         if (h) {
            return mapIterator(h, xsink); // TODO!!
         }// passed iterator
            
      }
      if (NT_NOTHING == arglst_type) {
         return 0;
      }
         
      // check if value can be mapped
      SingleArgvContextHelper argv_helper(*arg_lst, xsink);
      
      ReferenceHolder<AbstractQoreNode> arg_key(e[0]->eval(xsink), xsink);
      if (*xsink)
         return 0;
      ReferenceHolder<AbstractQoreNode> arg_val(e[1]->eval(xsink), xsink);
      if (*xsink)
         return 0;
      // For converting to string USE QoreStringValueHelper!!
      QoreStringValueHelper str_util(*arg_key);
      // Insert key-Value pair to the hash
      ret_val->setKeyValue(str_util->getBuffer(), arg_val.release(), xsink);
   }
   else {// List of values
      ConstListIterator li(reinterpret_cast<const QoreListNode*>(*arg_lst));
      while (li.next()) {
         // set offset in thread-local data for "$#"
         ImplicitElementHelper eh(li.index()); 
         SingleArgvContextHelper argv_helper(li.getValue(), xsink);
         
         QoreStringValueHelper key(e[0]->eval(xsink));
         if (*xsink)
            return 0;
         ReferenceHolder<AbstractQoreNode> val(e[1]->eval(xsink), xsink);

         if (*xsink)
            return 0;
         if (ref_rv)
             ret_val->setKeyValue(key->getBuffer(), val.release(), xsink);
         if (*xsink)
            return 0;
      }
   }
   return *xsink ? 0 : ret_val.release();
}

QoreHashNode* QoreHashMapOperatorNode::mapIterator(AbstractIteratorHelper& h, ExceptionSink* xsink) const {
   ReferenceHolder<QoreHashNode> rv(ref_rv ? new QoreHashNode : 0, xsink);

   qore_size_t i = 0;
   // set offset in thread-local data for "$#"
   while (true) {
      bool b = h.next(xsink);
      if (*xsink)
         return 0;
      if (!b)
         return rv.release();

      ImplicitElementHelper eh(i++);

      ReferenceHolder<> iv(h.getValue(xsink), xsink);
      if (*xsink)
         return 0;
      SingleArgvContextHelper argv_helper(*iv, xsink);

      QoreStringValueHelper key(e[0]->eval(xsink));
      if (*xsink)
         return 0;
      
      ReferenceHolder<AbstractQoreNode> val(e[1]->eval(xsink), xsink);
      if (*xsink)
         return 0;
      if (ref_rv)
          rv->setKeyValue(key->getBuffer(), val.release(), xsink);
      if (*xsink)
         return 0;
   }
   
   return rv.release();
}
