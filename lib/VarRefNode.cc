/*
  VarRefNode.cc

  Qore programming language

  Copyright 2003 - 2008 David Nichols

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

VarRefNode::VarRefNode(char *nme, int typ) : ParseNode(NT_VARREF)
{
   name = nme;
   type = typ;
}

VarRefNode::~VarRefNode()
{
   if (name)
   {
      printd(3, "VarRefNode::~VarRefNode() deleting variable reference %08p %s\n", name, name);
      free(name);
   }
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int VarRefNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const
{
   str.sprintf("variable reference '%s' %s (0x%08p)", name, type == VT_GLOBAL ? "global" : type == VT_LOCAL ? "local" : "unresolved", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *VarRefNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const
{
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the type name as a c string
const char *VarRefNode::getTypeName() const
{
   return "variable reference";
}

void VarRefNode::resolve()
{
   LocalVar *id;
   if ((id = find_local_var(name))) {
      type = VT_LOCAL;
      ref.id = id;
      printd(5, "VarRefNode::resolve(): local var %s resolved (id=%08p)\n", name, ref.id);
   }
   else {
      ref.var = getProgram()->checkGlobalVar(name);
      type = VT_GLOBAL;
      printd(5, "VarRefNode::resolve(): global var %s resolved (var=%08p)\n", name, ref.var);
   }
}

// returns 0 for OK, 1 for would be a new variable
int VarRefNode::resolveExisting()
{
   LocalVar *id;
   if ((id = find_local_var(name))) {
      type = VT_LOCAL;
      ref.id = id;
      printd(5, "VarRefNode::resolveExisting(): local var %s resolved (id=%08p)\n", name, ref.id);
      return 0;
   }
   
   ref.var = getProgram()->findGlobalVar(name);
   type = VT_GLOBAL;
   printd(5, "VarRefNode::resolveExisting(): global var %s resolved (var=%08p)\n", name, ref.var);
   return !ref.var;
}

AbstractQoreNode *VarRefNode::evalImpl(ExceptionSink *xsink) const
{
   if (type == VT_LOCAL) {
      printd(5, "VarRefNode::eval() lvar %08p (%s)\n", ref.id, ref.id);
      return ref.id->eval(xsink);
   }
   printd(5, "VarRefNode::eval() global var=%08p\n", ref.var);
   return ref.var->eval(xsink);
}

AbstractQoreNode *VarRefNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const
{
   if (type == VT_LOCAL)
      return ref.id->eval(needs_deref, xsink);
   needs_deref = true;
   return ref.var->eval(xsink);
}

int64 VarRefNode::bigIntEvalImpl(ExceptionSink *xsink) const
{
   VarRefNodeEvalOptionalRefHolder rv(this, xsink);
   return rv ? rv->getAsBigInt() : 0;
}

int VarRefNode::integerEvalImpl(ExceptionSink *xsink) const
{
   VarRefNodeEvalOptionalRefHolder rv(this, xsink);
   return rv ? rv->getAsInt() : 0;
}

bool VarRefNode::boolEvalImpl(ExceptionSink *xsink) const
{
   VarRefNodeEvalOptionalRefHolder rv(this, xsink);
   return rv ? rv->getAsBool() : 0;
}

double VarRefNode::floatEvalImpl(ExceptionSink *xsink) const
{
   VarRefNodeEvalOptionalRefHolder rv(this, xsink);
   return rv ? rv->getAsFloat() : 0;
}

AbstractQoreNode **VarRefNode::getValuePtr(AutoVLock *vl, ExceptionSink *xsink) const
{
   if (type == VT_LOCAL)
      return ref.id->getValuePtr(vl, xsink);
   return ref.var->getValuePtr(vl, xsink);
}

AbstractQoreNode *VarRefNode::getValue(AutoVLock *vl, ExceptionSink *xsink) const
{
   if (type == VT_LOCAL)
      return ref.id->getValue(vl, xsink);
   return ref.var->getValue(vl, xsink);
}

void VarRefNode::setValue(AbstractQoreNode *val, ExceptionSink *xsink)
{
   if (type == VT_LOCAL)
      ref.id->setValue(val, xsink);
   else
      ref.var->setValue(val, xsink);
}

char *VarRefNode::takeName()
{
   assert(name);
   char *p = name;
   name = 0;
   return p;
}
