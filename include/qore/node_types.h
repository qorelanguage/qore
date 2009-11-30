/*
  node_types.h

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

#ifndef _QORE_NODE_TYPES_H

#define _QORE_NODE_TYPES_H

/** @file node_types.h
    defines qore node type constants for all types implemented by the library
 */

// qore global system type constants
// value types must come first to support the operator matrix optimization 
const qore_type_t NT_NOTHING            = 0;  //!< type value for QoreNothingNode
const qore_type_t NT_INT                = 1;  //!< type value for QoreBigIntNode
const qore_type_t NT_FLOAT              = 2;  //!< type value for QoreFloatNode
const qore_type_t NT_STRING             = 3;  //!< type value for QoreStringNode
const qore_type_t NT_DATE               = 4;  //!< type value for DateTimeNode
const qore_type_t NT_BOOLEAN            = 5;  //!< type value for QoreBoolNode
const qore_type_t NT_NULL               = 6;  //!< type value for QoreNullNode
const qore_type_t NT_BINARY             = 7;  //!< type value for BinaryNode
const qore_type_t NT_LIST               = 8;  //!< type value for QoreListNode
const qore_type_t NT_HASH               = 9;  //!< type value for QoreHashNode
const qore_type_t NT_OBJECT             = 10; //!< type value for QoreObject
const qore_type_t NT_BACKQUOTE          = 11; //!< type value for BackquoteNode
const qore_type_t NT_CONTEXTREF         = 12; //!< type value for ContextrefNode
const qore_type_t NT_COMPLEXCONTEXTREF  = 13; //!< type value for ComplexContextrefNode
const qore_type_t NT_VARREF             = 14; //!< type value for VarRefNode
const qore_type_t NT_TREE               = 15; //!< type value for QoreTreeNode
const qore_type_t NT_FIND               = 16; //!< type value for FindNode
const qore_type_t NT_FUNCTION_CALL      = 17; //!< type value for FunctionCallNode
const qore_type_t NT_SELF_VARREF        = 18; //!< type value for SelfVarrefNode
const qore_type_t NT_SCOPE_REF          = 19; //!< type value for ScopedObjectCallNode
const qore_type_t NT_CONSTANT           = 20; //!< type value for ConstantNode
const qore_type_t NT_BAREWORD           = 21; //!< type value for BarewordNode
const qore_type_t NT_REFERENCE          = 22; //!< type value for ReferenceNode
const qore_type_t NT_CONTEXT_ROW        = 23; //!< type value for ContextRowNode
const qore_type_t NT_REGEX_SUBST        = 24; //!< type value for RegexSubstNode
const qore_type_t NT_REGEX_TRANS        = 25; //!< type value for RegexTransNode
const qore_type_t NT_REGEX              = 26; //!< type value for QoreRegexNode
const qore_type_t NT_CLASSREF           = 27; //!< type value for ClassRefNode
const qore_type_t NT_OBJMETHREF         = 28; //!< type value for AbstractParseObjectMethodReferenceNode
const qore_type_t NT_FUNCREF            = 29; //!< type value for AbstractCallReferenceNode
const qore_type_t NT_FUNCREFCALL        = 30; //!< type value for CallReferenceCallNode
const qore_type_t NT_CLOSURE            = 31; //!< type value for QoreClosureParseNode (private class)
const qore_type_t NT_RUNTIME_CLOSURE    = 32; //!< type value for ResolvedCallReferenceNode (QoreClosureNode, QoreObjectClosureNode)
const qore_type_t NT_IMPLICIT_ARG       = 33; //!< type value for QoreImplicitArgumentNode (private class)
const qore_type_t NT_METHOD_CALL        = 34; //!< type value for MethodCallNode (private class)
const qore_type_t NT_STATIC_METHOD_CALL = 35; //!< type value for StaticMethodCallNode (private class)

//! number of types implemented in the Qore library
#define QORE_NUM_TYPES 36

//! number of simple value types (not containers)
#define NUM_SIMPLE_TYPES 8

//! number of potential value types (including container types)
#define NUM_VALUE_TYPES 11

#endif
