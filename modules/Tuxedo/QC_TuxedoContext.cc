#if !((defined USE_ONE_TRANSLATION_UNIT) && !(defined SKIP_THIS_FILE))
/*
  modules/Tuxedo/QC_TuxedoContext.cc

  Tuxedo integration to QORE

  Qore Programming Language

  Copyright (C) 2006 Qore Technologies

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

#include <qore/common.h>
#include <qore/support.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/minitest.hpp>

#include "QC_TuxedoContext.h"
#include "QoreTuxedoContext.h"

int CID_TUXEDOCONTEXT;

//------------------------------------------------------------------------------
static void getTuxedoContext(void* obj)
{
  ((QoreTuxedoContext*)obj)->ROreference();
}

static void releaseTuxedoContext(void* obj)
{
  ((QoreTuxedoContext*)obj)->deref();
}

//------------------------------------------------------------------------------
// No parameters.
static void TUXEDOCTX_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
  tracein("TUXEDOCTX_constructor");

  if (get_param(params, 0)) {
    xsink->raiseException("TUXEDO-CONTEXT-CONSTRUCTOR", "No parameters are allowed in the constructor.");
    return;
  }

  QoreTuxedoContext* ctx = new QoreTuxedoContext();
  self->setPrivate(CID_TUXEDOCONTEXT, ctx, getTuxedoContext, releaseTuxedoContext);

  traceout("TUXEDOCTX_constructor");
}

//------------------------------------------------------------------------------
static void TUXEDOCTX_destructor(Object *self, QoreTuxedoContext* ctx, ExceptionSink *xsink)
{
  tracein("TUXEDOCTX_destructor");
  ctx->deref();
  traceout("TUXEDOCTX_destructor");
}

//------------------------------------------------------------------------------
static void TUXEDOCTX_copy(Object *self, Object *old, QoreTuxedoContext* ctx, ExceptionSink *xsink)
{
  xsink->raiseException("TUXEDO-CONTEXT-COPY", "copying Tuxedo::TuxedoContext objects is not yet supported.");
}

//-----------------------------------------------------------------------------
static QoreNode* TUXEDOCTX_is_null(Object* self, QoreTuxedoContext* ctx, QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("TuxedoContext::isNullContext", "No parameters expected.");
    return 0;
  }
  return new QoreNode(ctx->ctx == TPNULLCONTEXT);
}

//-----------------------------------------------------------------------------
class QoreClass* initTuxedoContextClass()
{
  tracein("initTuxedoContextClass");
  QoreClass* ctx = new QoreClass(QDOM_NETWORK, strdup("TuxedoContext"));
  CID_TUXEDOCONTEXT = ctx->getID();  

  ctx->setConstructor((q_constructor_t)TUXEDOCTX_constructor);
  ctx->setDestructor((q_destructor_t)TUXEDOCTX_destructor);
  ctx->setCopy((q_copy_t)TUXEDOCTX_copy);
  ctx->addMethod("isNullContext", (q_method_t)TUXEDOCTX_is_null);

  traceout("initTuxedoContextClass");
  return ctx;
}

//-----------------------------------------------------------------------------
#ifdef DEBUG
TEST()
{
  // test it could be instantiated
  QoreTuxedoContext ctx;
}

TEST()
{
  // test isNullContext
  QoreTuxedoContext ctx;
  ExceptionSink xsink;

  List* l = new List();
  QoreNode* params = new QoreNode(l);

  QoreNode* res = TUXEDOCTX_is_null(0, &ctx, params, &xsink);
  assert(!xsink.isException());
  assert(res);
  assert(res->type == NT_BOOLEAN);
  assert(res->val.boolval);

  res->deref(&xsink);
  params->deref(&xsink);
  assert(!xsink.isException());
}

TEST()
{
  // isNullContext with unexpected parameter
  QoreTuxedoContext ctx;
  ExceptionSink xsink;

  List* l = new List();
  l->push(new QoreNode("aaa"));
  QoreNode* params = new QoreNode(l);

  QoreNode* res = TUXEDOCTX_is_null(0, &ctx, params, &xsink);
  assert(xsink.isException());
  xsink.clear();
  assert(!res);

  params->deref(&xsink);
  assert(!xsink.isException());
}
#endif  // DEBUG

#endif
// EOF

