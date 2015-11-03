/*
  params.h

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

#ifndef _QORE_PARAMS_H

#define _QORE_PARAMS_H

#include <qore/AbstractQoreNode.h>

/** @file params.h
    Contains inline functions for accessing function and class method arguments.
 */

//! returns the number of arguments passed to the function
/** @param n a pointer to the argument list
    @return the number of arguments passed to the function
 */
static inline int num_params(const QoreListNode *n) {
   if (!n)
      return 0;
   return n->size();
}

//! returns the argument in the position given or 0 if there is none
/**
   @param n a pointer to the argument list
   @param i the offset in the list to test (first element is offset 0)
   @return the argument in the position given or 0 if there is none
 */
static inline const AbstractQoreNode *get_param(const QoreListNode *n, qore_size_t i) {
   if (!n) return 0;
   const AbstractQoreNode *p = n->retrieve_entry(i);
   return is_nothing(p) ? 0 : p;
}

//! returns the argument type in the position given or 0 if there is none
/**
   @param n a pointer to the argument list
   @param i the offset in the list to test (first element is offset 0)
   @return the argument type in the position given or 0 if there is none
 */
static inline qore_type_t get_param_type(const QoreListNode *n, qore_size_t i) {
   if (!n) return NT_NOTHING;
   const AbstractQoreNode *p = n->retrieve_entry(i);
   return p ? p->getType() : NT_NOTHING;
}

//! returns an integer corresponding to the argument given or 0 if there is none
static inline int get_int_param(const QoreListNode *n, qore_size_t i) {
   if (!n) return 0;
   const AbstractQoreNode *p = n->retrieve_entry(i);
   return is_nothing(p) ? 0 : p->getAsInt();
}

//! returns a 64-bit integer corresponding to the argument given or 0 if there is none
static inline int64 get_bigint_param(const QoreListNode *n, qore_size_t i) {
   if (!n) return 0;
   const AbstractQoreNode *p = n->retrieve_entry(i);
   return is_nothing(p) ? 0 : p->getAsBigInt();
}

//! returns a boolean value corresponding to the argument given or false if there is none
static inline bool get_bool_param(const QoreListNode *n, qore_size_t i) {
   if (!n) return 0;
   const AbstractQoreNode *p = n->retrieve_entry(i);
   return is_nothing(p) ? false : p->getAsBool();
}

//! returns a const BinaryNode pointer for the argument position given or 0 if there is no argument there or if the argument is not a BinaryNode
/**
   @param n a pointer to the argument list
   @param i the offset in the list to test (first element is offset 0)
   @return a const BinaryNode pointer for the argument position given or 0 if there is no argument there or if the argument is not a BinaryNode
 */
static inline const BinaryNode *test_binary_param(const QoreListNode *n, qore_size_t i) {
   if (!n) return 0;
   const AbstractQoreNode *p = n->retrieve_entry(i);
   // the following is faster than a dynamic_cast
   return p && p->getType() == NT_BINARY ? reinterpret_cast<const BinaryNode *>(p) : 0;
}

//! returns a const QoreStringNode pointer for the argument position given or 0 if there is no argument there or if the argument is not a QoreStringNode
/**
   @param n a pointer to the argument list
   @param i the offset in the list to test (first element is offset 0)
   @return a const QoreStringNode pointer for the argument position given or 0 if there is no argument there or if the argument is not a QoreStringNode
 */
static inline const QoreStringNode *test_string_param(const QoreListNode *n, qore_size_t i) {
   if (!n) return 0;
   const AbstractQoreNode *p = n->retrieve_entry(i);
   // the following is faster than a dynamic_cast
   return p && p->getType() == NT_STRING ? reinterpret_cast<const QoreStringNode *>(p) : 0;
}

//! returns a QoreObject pointer for the argument position given or 0 if there is no argument there or if the argument is not a QoreObject
/** QoreObjects are returned not as "const" because they are unique and can always be manipulated
    @param n a pointer to the argument list
    @param i the offset in the list to test (first element is offset 0)
    @return a QoreObject pointer for the argument position given or 0 if there is no argument there or if the argument is not a QoreObject
 */
static inline QoreObject *test_object_param(const QoreListNode *n, qore_size_t i) {
   if (!n) return 0;
   const AbstractQoreNode *p = n->retrieve_entry(i);
   // the following is faster than a dynamic_cast
   return p && p->getType() == NT_OBJECT ? const_cast<QoreObject *>(reinterpret_cast<const QoreObject *>(p)) : 0;
}

//! returns a DateTimeNode pointer for the argument position given or 0 if there is no argument there or if the argument is not a DateTimeNode
/**
   @param n a pointer to the argument list
   @param i the offset in the list to test (first element is offset 0)
   @return a DateTimeNode pointer for the argument position given or 0 if there is no argument there or if the argument is not a DateTimeNode
 */
static inline const DateTimeNode *test_date_param(const QoreListNode *n, qore_size_t i) {
   if (!n) return 0;
   const AbstractQoreNode *p = n->retrieve_entry(i);
   // the following is faster than a dynamic_cast
   return p && p->getType() == NT_DATE ? reinterpret_cast<const DateTimeNode *>(p) : 0;
}

//! returns a QoreHashNode pointer for the argument position given or 0 if there is no argument there or if the argument is not a QoreHashNode
/**
   @param n a pointer to the argument list
   @param i the offset in the list to test (first element is offset 0)
   @return a QoreHashNode pointer for the argument position given or 0 if there is no argument there or if the argument is not a QoreHashNode
 */
static inline const QoreHashNode *test_hash_param(const QoreListNode *n, qore_size_t i) {
   if (!n) return 0;
   const AbstractQoreNode *p = n->retrieve_entry(i);
   // the following is faster than a dynamic_cast
   return p && p->getType() == NT_HASH ? reinterpret_cast<const QoreHashNode *>(p) : 0;
}

//! returns a QoreListNode pointer for the argument position given or 0 if there is no argument there or if the argument is not a QoreListNode
/**
   @param n a pointer to the argument list
   @param i the offset in the list to test (first element is offset 0)
   @return a QoreListNode pointer for the argument position given or 0 if there is no argument there or if the argument is not a QoreListNode
 */
static inline const QoreListNode *test_list_param(const QoreListNode *n, qore_size_t i) {
   if (!n) return 0;
   const AbstractQoreNode *p = n->retrieve_entry(i);
   // the following is faster than a dynamic_cast
   return p && p->getType() == NT_LIST ? reinterpret_cast<const QoreListNode *>(p) : 0;
}

//! returns a ResolvedCallReferenceNode pointer for the argument position given or 0 if there is no argument there or if the argument is not a ResolvedCallReferenceNode
/**
   @param n a pointer to the argument list
   @param i the offset in the list to test (first element is offset 0)
   @return a ResolvedCallReferenceNode pointer for the argument position given or 0 if there is no argument there or if the argument is not a ResolvedCallReferenceNode
 */
static inline const ResolvedCallReferenceNode *test_callref_param(const QoreListNode *n, qore_size_t i) {
   if (!n) return 0;
   const AbstractQoreNode *p = n->retrieve_entry(i);
   // the following is faster than a dynamic_cast
   return p && (p->getType() == NT_FUNCREF || p->getType() == NT_RUNTIME_CLOSURE) ? reinterpret_cast<const ResolvedCallReferenceNode *>(p) : 0;
}

//! returns a ResolvedCallReferenceNode pointer for the argument position given or 0 if there is no argument there or if the argument is not a ResolvedCallReferenceNode
/**
   @param n a pointer to the argument list
   @param i the offset in the list to test (first element is offset 0)
   @return a ResolvedCallReferenceNode pointer for the argument position given or 0 if there is no argument there or if the argument is not a ResolvedCallReferenceNode
 */
static inline const ResolvedCallReferenceNode *test_funcref_param(const QoreListNode *n, qore_size_t i) {
   return test_callref_param(n, i);
}

//! returns a ReferenceNode pointer for the argument position given or 0 if there is no argument there or if the argument is not a ReferenceNode
/**
   Note that this will also return a value for a closure; this is a synonym for test_funcref_param
   @param n a pointer to the argument list
   @param i the offset in the list to test (first element is offset 0)
   @return a ReferenceNode pointer for the argument position given or 0 if there is no argument there or if the argument is not a ReferenceNode
 */
static inline const ReferenceNode *test_reference_param(const QoreListNode *n, qore_size_t i) {
   if (!n) return 0;
   const AbstractQoreNode *p = n->retrieve_entry(i);
   // the following is faster than a dynamic_cast
   return p && p->getType() == NT_REFERENCE ? reinterpret_cast<const ReferenceNode *>(p) : 0;
}

//! returns true if the arugment position given is NOTHING
/**
   @param n a pointer to the argument list
   @param i the offset in the list to test (first element is offset 0)
   @return true if the arugment position given is NOTHING
 */
static inline bool test_nothing_param(const QoreListNode *n, qore_size_t i) {
   if (!n) return true;
   return is_nothing(n->retrieve_entry(i));
}

#endif
