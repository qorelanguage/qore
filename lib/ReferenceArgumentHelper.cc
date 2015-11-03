/*
  ReferenceArgumentHelper.cc

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

#include <qore/Qore.h>

#include "ReferenceArgumentHelper.h"

struct lvih_intern {
      LocalVar lv;
      ExceptionSink *xsink;
      ReferenceNode *ref;

      DLLLOCAL lvih_intern(AbstractQoreNode *val, ExceptionSink *xs) : lv("ref_arg_helper"), xsink(xs)
      {
	 printd(5, "ReferenceArgumentHelper::ReferenceArgumentHelper() instantiating %08p (val=%08p type='%s') \n", &lv, val, val ? val->getTypeName() : "n/a");
	 lv.instantiate(val);
	 VarRefNode *vr = new VarRefNode(strdup("ref_arg_helper"), VT_LOCAL);
	 vr->ref.id = &lv;
	 ref = new ReferenceNode(vr);
      }

      DLLLOCAL ~lvih_intern()
      {
	 ref->deref();
	 lv.uninstantiate(xsink);
      }

      DLLLOCAL AbstractQoreNode *getOutputValue()
      {
	 // there will be no locking here, because it's our temporary local "variable"
	 ExceptionSink xsink2;
	 LValueHelper vp(ref->getExpression(), &xsink2);

	 // no exception should be possible here
	 assert(!xsink2);
	 if (!vp)
	    return 0;

	 // take output value from our temporary "variable" and return it
	 return vp.take_value();
      }
      
      DLLLOCAL AbstractQoreNode *getArg()
      {
	 return ref->refSelf();
      }
};

ReferenceArgumentHelper::ReferenceArgumentHelper(AbstractQoreNode *val, ExceptionSink *xsink) : priv(new lvih_intern(val, xsink))
{
}

ReferenceArgumentHelper::~ReferenceArgumentHelper()
{
   delete priv;
}

AbstractQoreNode *ReferenceArgumentHelper::getArg() const
{
   return priv->getArg();
}

AbstractQoreNode *ReferenceArgumentHelper::getOutputValue()
{
   return priv->getOutputValue();
}

