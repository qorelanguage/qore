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
      QoreNode *ref;

      DLLLOCAL lvih_intern(const char *name, QoreNode *val, ExceptionSink *xs) : xsink(xs)
      {
	 lv = instantiateLVar(name, val);

	 ref = new QoreNode(NT_REFERENCE);
	 VarRef *vr = new VarRef(strdup(name), VT_LOCAL);
	 vr->ref.id = name;
	 ref->val.lvexp = new QoreNode(vr);
      }

      DLLLOCAL ~lvih_intern()
      {
	 ref->deref(xsink);
	 uninstantiateLVar(xsink);
      }

      DLLLOCAL QoreNode *getOutputValue()
      {
	 // there will be no locking here, because it's our temporary local "variable"
	 class AutoVLock vl;
	 class QoreNode **vp = get_var_value_ptr(ref->val.lvexp, &vl, xsink);

	 // no exception should be possible here
	 if (*xsink)
	    return 0;

	 // take output value from our temporary "variable" 
	 QoreNode *rv = *vp;
	 *vp = 0;
	 // and return it
	 
	 return rv;
      }
};

LVarInstantiatorHelper::LVarInstantiatorHelper(const char *name, QoreNode *val, ExceptionSink *xsink) : priv(new lvih_intern(name, val, xsink))
{
}

LVarInstantiatorHelper::~LVarInstantiatorHelper()
{
   delete priv;
}

QoreNode *LVarInstantiatorHelper::getArg() const
{
   return priv->ref;
}

QoreNode *LVarInstantiatorHelper::getOutputValue()
{
   return priv->getOutputValue();
}

