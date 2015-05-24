/*
  QoreHashMapSelectOperatorNode.cpp
 
  Qore Programming Language
 
  Copyright (C) 2003 - 2015 Qore Technologies, sro
  
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

QoreString QoreHashMapSelectOperatorNode::map_str("map operator expression");

// if del is true, then the returned QoreString * should be mapd, if false, then it must not be
QoreString *QoreHashMapSelectOperatorNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = false;
   return &map_str;
}

int QoreHashMapSelectOperatorNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.concat(&map_str);
   return 0;
}

AbstractQoreNode* QoreHashMapSelectOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, 
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

   // check iteratorTypeInfo expression
   const QoreTypeInfo* selectTypeInfo = 0;
   e[3] = e[3]->parseInit(oflag, pflag, lvids, selectTypeInfo);
   
   return this;
}

QoreValue QoreHashMapSelectOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   ValueEvalRefHolder arg_lst(e[2], xsink);
   if (*xsink || arg_lst->isNothing()) {
      needs_deref = false;
      return QoreValue();
   }
   
   qore_type_t arglst_type = arg_lst->getType();
   assert(arglst_type != NT_NOTHING);
   ReferenceHolder<QoreHashNode> ret_val(ref_rv ? new QoreHashNode : 0, xsink);
   if (NT_LIST != arglst_type) { // Single value
      // check if it's an AbstractIterator object
      if (NT_OBJECT == arglst_type) {
         AbstractIteratorHelper h(xsink, "hmap operator select", 
				  const_cast<QoreObject*>(arg_lst->get<const QoreObject>()));
         if (*xsink) {
	    needs_deref = false;
	    return QoreValue();
	 }
         if (h) {
	    needs_deref = ref_rv;
            return mapIterator(h, xsink); // TODO!!
         }// passed iterator
      }

      // check if value can be mapped     
      ReferenceHolder<> arg_ival(arg_lst.getReferencedValue(), xsink);
      SingleArgvContextHelper argv_helper(*arg_ival, xsink);
      ValueEvalRefHolder result(e[3], xsink);
      if (*xsink || !result->getAsBool()) {
	 needs_deref = false;
	 return QoreValue();
      }

      ValueEvalRefHolder arg_key(e[0], xsink);
      if (*xsink) {
	 needs_deref = false;
	 return QoreValue();
      }

      ValueEvalRefHolder arg_val(e[1], xsink);
      if (*xsink) {
	 needs_deref = false;
	 return QoreValue();
      }

      // we have to convert to a string in the default encoding to use a hash key
      QoreStringValueHelper str_util(*arg_key, QCS_DEFAULT, xsink);
      if (*xsink)
	 return QoreValue();
      
      // Insert key-Value pair to the hash
      ret_val->setKeyValue(str_util->getBuffer(), arg_val.getReferencedValue(), xsink);
   }
   else {// List of values
      ConstListIterator li(arg_lst->get<const QoreListNode>());
      while (li.next()) {
         // set offset in thread-local data for "$#"
         ImplicitElementHelper eh(li.index()); 
         SingleArgvContextHelper argv_helper(li.getValue(), xsink);

	 ValueEvalRefHolder result(e[3], xsink);
	 if (*xsink) {
	    needs_deref = false;
	    return QoreValue();
	 }

	 if (!result->getAsBool())
	    continue;
        
	 {
	    ValueEvalRefHolder ekey(e[0], xsink);
	    if (*xsink) {
	       needs_deref = false;
	       return QoreValue();
	    }

	    // we have to convert to a string in the default encoding to use a hash key
	    QoreStringValueHelper key(*ekey, QCS_DEFAULT, xsink);
	    if (*xsink)
	       return QoreValue();
	    
	    ValueEvalRefHolder val(e[1], xsink);
	    if (*xsink) {
	       needs_deref = false;
	       return QoreValue();
	    }

	    if (ref_rv)
	       ret_val->setKeyValue(key->getBuffer(), val.getReferencedValue(), xsink);
	 }
	 // if there is an exception dereferencing one of the evaluted nodes above, then exit the loop
	 if (*xsink)
	    return QoreValue();
      }
   }
   if (*xsink || !ref_rv) {
      needs_deref = false;
      return QoreValue();
   }
   needs_deref = true;
   return ret_val.release();
}

QoreValue QoreHashMapSelectOperatorNode::mapIterator(AbstractIteratorHelper& h, ExceptionSink* xsink) const {
   ReferenceHolder<QoreHashNode> rv(ref_rv ? new QoreHashNode : 0, xsink);

   qore_size_t i = 0;
   // set offset in thread-local data for "$#"
   while (true) {
      bool has_next = h.next(xsink);
      if (*xsink)
         return QoreValue();
      if (!has_next)
         return rv.release();

      ImplicitElementHelper eh(i++);

      ReferenceHolder<> iv(h.getValue(xsink), xsink);
      if (*xsink)
         return QoreValue();

      // check if value can be mapped
      SingleArgvContextHelper argv_helper(*iv, xsink);

      ValueEvalRefHolder result(e[3], xsink);
      if (*xsink)
	 return QoreValue();
      
      if (!result->getAsBool())
	 continue;

      {
	 ValueEvalRefHolder ekey(e[0], xsink);
	 if (*xsink)
	    return QoreValue();
	 
	 // we have to convert to a string in the default encoding to use a hash key
	 QoreStringValueHelper key(*ekey, QCS_DEFAULT, xsink);
	 if (*xsink)
	    return QoreValue();
	 
	 ValueEvalRefHolder val(e[1], xsink);
	 if (*xsink)
	    return QoreValue();
	 
	 if (ref_rv)
	    rv->setKeyValue(key->getBuffer(), val.getReferencedValue(), xsink);
      }
      // if there is an exception dereferencing one of the evaluted nodes above, then exit the loop
      if (*xsink)
	 return QoreValue();
   }
   
   return rv.release();
}
