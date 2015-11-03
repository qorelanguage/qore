/*
  ReferenceHelper.h

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

#ifndef _QORE_REFERENCEHELPER_H

#define _QORE_REFERENCEHELPER_H

//! helper class to manage variable references passed to functions and class methods, stack only, cannot be dynamically allocated
/** Takes care of safely accessing ReferenceNode objects, for example when they are passed
    as arguments to a builtin function or method.  Locks are automatically acquired in the constructor
    if necessary and released in the destructor.  The constructor could raise a Qore-language
    exception if there is a deadlock acquiring any locks to access the ReferenceNode's value
    as given by the lvalue expression, so the object should be checked for this state right
    after the constructor as in the following example:
    @code
    const AbstractQoreNode *p = get_param(params, 0);   
    if (p && p->getType() == NT_REFERENCE) {
       const ReferenceNode *r = reinterpret_cast<const ReferenceNode *>(p);
       AutoVLock vl;
       ReferenceHelper ref(r, vl, xsink);
       // a deadlock exception occurred accessing the reference's value pointer
       if (!ref)
          return 0;

       // more code to access the reference
    }
    @endcode
 */
class ReferenceHelper {
   private:
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL ReferenceHelper(const ReferenceHelper&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL ReferenceHelper& operator=(const ReferenceHelper&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL void *operator new(size_t);

   protected:
      //! pointer to the pointer to the lvalue expression target of the reference
      AbstractQoreNode **vp;

   public:
      //! initializes the object and tries to get the pointer to the pointer of the lvalue expression target
      /** @param ref the ReferenceNode to use
	  @param vl the reference to the AutoVLock structure for managing nested locks
	  @param xsink Qore-language exceptions raised will be added here (for example, a deadlock accessing the object)
       */
      DLLEXPORT ReferenceHelper(const ReferenceNode *ref, AutoVLock &vl, ExceptionSink *xsink);

      //! destroys the object
      DLLEXPORT ~ReferenceHelper();

      //! returns true if the reference is valid, false if not
      /** false will only be returned if a Qore-language exception was raised in the constructor
       */
      DLLLOCAL operator bool() const { return vp != 0; }

      //! returns the type of the reference's value
      /** @return the type of the reference's value
       */
      DLLLOCAL qore_type_t getType() const { return *vp ? (*vp)->getType() : NT_NOTHING; }

      //! returns a pointer to the value with a unique reference count (so it can be updated in place), assumes the reference is valid
      /** @param xsink required for the call to AbstractQoreNode::deref()
	  @returns a pointer to the reference's value with a unique reference count (so it can be modified), or 0 if the value was 0 to start with or if a Qore-language exception was raised
	  @note you must check that the reference is valid before calling this function
	  @note take care to only call this function on types where the AbstractQoreNode::realCopy() function has a valid implementation (on all value types suitable for in-place modification this function has a valid implementation), as in debugging builds other types will abort(); in non-debugging builds this function will simply do nothing
	  @code
	  AutoVLock vl;
	  ReferenceHelper rh(ref, vl, xsink);
	  // if the reference is not valid, then return
	  if (!rh)  
	     return;
	  // get the unique value
	  AbstractQoreNode *val = rh.getUnique(xsink);
	  // if a Qore-language exception was raised, then return
	  if (*xsink)
	      return;
	  @endcode
       */
      DLLEXPORT AbstractQoreNode *getUnique(ExceptionSink *xsink);

      //! assigns a value to the reference, assumes the reference is valid
      /** @param val the value to assign (must be already referenced for the assignment)
	  @param xsink required for the call to AbstractQoreNode::deref() of the current value
	  @return 0 if there was no error and the variable was assigned, -1 if a Qore-language exception occured dereferencing the current value, in this case no assignment was made and the reference count for val is dereferenced automatically by the ReferenceHelper object
	  @note you must check that the reference is valid before calling this function
	  @code
	  AutoVLock vl;
	  ReferenceHelper rh(ref, vl, xsink);
	  // if the reference is not valid, then return
	  if (!rh)  
	     return;
	  // make the assignment (if the assignment fails, the value will be dereferenced automatically)
	  rh.assign(val->refSelf(), xsink);
	  @endcode
       */
      DLLEXPORT int assign(AbstractQoreNode *val, ExceptionSink *xsink);

      //! returns a constant pointer to the reference's value
      /** @return the value of the lvalue reference (may be 0)
       */
      DLLEXPORT const AbstractQoreNode *getValue() const;

      //! swaps the values of two references
      DLLEXPORT void swap(ReferenceHelper &other);
};

#endif
