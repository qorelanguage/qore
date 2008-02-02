/*
  LVarInstantiatorHelper.cc

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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

#include "LVarInstantiatorHelper.h"

struct lvih_intern {
      LVar *lv;
      ExceptionSink *xsink;
      ReferenceNode *ref;

      DLLLOCAL lvih_intern(const char *name, AbstractQoreNode *val, ExceptionSink *xs) : xsink(xs)
      {
	 char *new_name = strdup(name);
	 printd(5, "LVarInstantiatorHelper::LVarInstantiatorHelper() instantiating '%s' %08p (val=%08p type='%s') \n", new_name, new_name, val, val ? val->getTypeName() : "n/a");
	 lv = instantiateLVar(new_name, val);
	 VarRefNode *vr = new VarRefNode(new_name, VT_LOCAL);
	 vr->ref.id = new_name;
	 ref = new ReferenceNode(vr);
      }

      DLLLOCAL ~lvih_intern()
      {
	 ref->deref();
	 uninstantiateLVar(xsink);
      }

      DLLLOCAL AbstractQoreNode *getOutputValue()
      {
	 // there will be no locking here, because it's our temporary local "variable"
	 class AutoVLock vl;
	 ExceptionSink xsink2;
	 class AbstractQoreNode **vp = get_var_value_ptr(ref->lvexp, &vl, &xsink2);

	 // no exception should be possible here
	 assert(!xsink2);
	 if (xsink2)
	    return 0;

	 // take output value from our temporary "variable" 
	 AbstractQoreNode *rv = *vp;
	 *vp = 0;
	 // and return it
	 
	 return rv;
      }
      
      DLLLOCAL class AbstractQoreNode *getArg()
      {
	 return ref->RefSelf();
      }
};

LVarInstantiatorHelper::LVarInstantiatorHelper(const char *name, AbstractQoreNode *val, ExceptionSink *xsink) : priv(new lvih_intern(name, val, xsink))
{
}

LVarInstantiatorHelper::~LVarInstantiatorHelper()
{
   delete priv;
}

AbstractQoreNode *LVarInstantiatorHelper::getArg() const
{
   return priv->getArg();
}

AbstractQoreNode *LVarInstantiatorHelper::getOutputValue()
{
   return priv->getOutputValue();
}

