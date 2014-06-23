/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ReferenceArgumentHelper.h

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

#ifndef _QORE_REFERENCEARGUMENTHELPER_H

#define _QORE_REFERENCEARGUMENTHELPER_H

//! allows a reference to be passed as an argument to Qore code
/** this class creates a fake local variable and then creates a reference the local
    variable that can be used in an argument list to be passed to a function.
    Then the ReferenceArgumentHelper::getOutputValue() function can be called to
    retrieve the value of the local variable after the Qore-language code has been
    executed.  This allows values to be passed by reference to Qore-language code
    and then the value of the variable read back out and processed.
    @code
    // create an argument list
    ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
    // instantiate "val" as a reference as the only argument in the argument list
    ReferenceArgumentHelper lvh("arg0", val, &xsink);
    args->push(lvh.getArg());

    // execute method "m_fixup" and discard any return value
    discard(m_fixup->eval(qore_obj, *args, &xsink), &xsink);

    // return the value of the reference after executing the method
    return lvh.getOutputValue();
    @endcode
 */ 
class ReferenceArgumentHelper {
   private:
      struct lvih_intern *priv;

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL ReferenceArgumentHelper(const ReferenceArgumentHelper&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL ReferenceArgumentHelper& operator=(const ReferenceArgumentHelper&);

   public:
      //! creates a fake local variable assigned to "val" and creates a reference to the local variable
      /**
	 @param val the value to assign to the local variable
	 @param xsink this value is saved to be used for dereferencing the fake local variable in the destructor
       */
      DLLEXPORT ReferenceArgumentHelper(AbstractQoreNode *val, ExceptionSink *xsink);

      //! frees all memory still managed by the object
      DLLEXPORT ~ReferenceArgumentHelper();

      //! returns the reference to the fake local variable for use in an argument list, the caller owns the reference returned
      /**
	 @return the reference to the fake local variable for use in an argument list, the caller owns the reference returned
      */
      DLLEXPORT AbstractQoreNode *getArg() const;

      //! returns the value of the reference and leaves the reference empty, the caller owns the reference returned
      /**
	 @return the value of the reference and leaves the reference empty, the caller owns the reference returned
       */
      DLLEXPORT AbstractQoreNode *getOutputValue();
};

#endif
