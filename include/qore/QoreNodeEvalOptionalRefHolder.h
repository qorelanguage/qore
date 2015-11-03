/*
  QoreNodeEvalOptionalRefHolder.h
  
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

#ifndef _QORE_QORENODEEVALOPTIONALREFHOLDER_H

#define _QORE_QORENODEEVALOPTIONALREFHOLDER_H

//! this class manages reference counts for the optional evaluation of AbstractQoreNode objects
/**
   This class can only be used on the stack (cannot be allocated dynamically).
   This class is designed to avoid atomic reference count increments and decrements whenever possible and to avoid an "eval()" call for types that do not require it (such as value types).  It is used extensively internally but normally should not need to be used outside of the qore library itself.
   @code
   QoreNodeEvalOptionalRefHolder evaluated_node(node, xsink);
   return evaluated_node ? evaluated_node->getAsBool() : false;
   @endcode
 */
class QoreNodeEvalOptionalRefHolder {
   private:
      AbstractQoreNode *val;
      ExceptionSink *xsink;
      bool needs_deref;

      DLLLOCAL void discard_intern() {
	 if (needs_deref && val)
	    val->deref(xsink);
      }

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreNodeEvalOptionalRefHolder(const QoreNodeEvalOptionalRefHolder&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreNodeEvalOptionalRefHolder& operator=(const QoreNodeEvalOptionalRefHolder&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      /** this function is not implemented in order to require objects of this type to be allocated on the stack.
       */
      DLLLOCAL void *operator new(size_t);

   public:
      //! constructor used to create a holder object
      DLLLOCAL QoreNodeEvalOptionalRefHolder(ExceptionSink *n_xsink) : val(0), xsink(n_xsink), needs_deref(false) {
      }

      //! constructor with a value that will call the class' eval(needs_deref) method
      DLLLOCAL QoreNodeEvalOptionalRefHolder(const AbstractQoreNode *exp, ExceptionSink *n_xsink) : xsink(n_xsink) {
	 if (exp)
	    val = exp->eval(needs_deref, xsink);
	 else {
	    val = 0;
	    needs_deref = false;
	 }	    
      }

      //! discards any temporary value evaluated by the constructor or assigned by "assign()"
      DLLLOCAL ~QoreNodeEvalOptionalRefHolder() {
	 discard_intern();
      }
      
      //! discards any temporary value evaluated by the constructor or assigned by "assign()"
      DLLLOCAL void discard() {
	 discard_intern();
	 needs_deref = false;
	 val = 0;
      }

      //! assigns a new value to this holder object
      DLLLOCAL void assign(bool n_needs_deref, AbstractQoreNode *n_val) {
	 discard_intern();
	 needs_deref = n_needs_deref;
	 val = n_val;
      }

      //! returns a referenced value - the caller will own the reference
      DLLLOCAL AbstractQoreNode *getReferencedValue() {
	 if (needs_deref)
	    needs_deref = false;
	 else if (val)
	    val->ref();
	 return val;
      }

      //! returns the object being managed
      DLLLOCAL const AbstractQoreNode *operator->() const { return val; }

      //! returns the object being managed
      DLLLOCAL const AbstractQoreNode *operator*() const { return val; }

      //! returns true if a value is being held
      DLLLOCAL operator bool() const { return val != 0; }

      //! returns true if the value is temporary (needs dereferencing)
      DLLLOCAL bool isTemp() const { return needs_deref; }

};

#endif
