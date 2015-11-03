/*
  ReferenceArgumentHelper.h

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
