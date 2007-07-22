/*
 QC_QFont.cc
 
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
#include "QC_QFont.h"

DLLLOCAL int CID_QFONT;

static void QF_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p) {
      xsink->raiseException("QFONT-CONSTRUCTOR-ERROR", "missing font name as first parameter");
      return;
   }
   const char *fname = p->val.String->getBuffer();

   // get point size
   p = get_param(params, 1);
   int point_size = p ? p->getAsInt() : -1;

   // get weight
   p = get_param(params, 2);
   int weight = p ? p->getAsInt() : -1;
   
   // get italic flag
   p = get_param(params, 3);
   bool italic = p ? p->getAsBool() : false;

   QoreQFont *qf = new QoreQFont(fname, point_size, weight, italic);
   self->setPrivate(CID_QFONT, qf);
}

static void QF_copy(class Object *self, class Object *old, class QoreQFont *qf, ExceptionSink *xsink)
{
   xsink->raiseException("QFONT-COPY-ERROR", "objects of this class cannot be copied");
}

/*
static class QoreNode *QF_exec(class Object *self, class QoreQFont *qf, class QoreNode *params, ExceptionSink *xsink)
{
   qf->exec();
   return 0;
}
*/

class QoreClass *initQFontClass()
{
   tracein("initQFontClass()");
   
   class QoreClass *QC_QFont = new QoreClass("QFont", QDOM_GUI);
   CID_QFONT = QC_QFont->getID();
   QC_QFont->setConstructor(QF_constructor);
   QC_QFont->setCopy((q_copy_t)QF_copy);

   traceout("initQFontClass()");
   return QC_QFont;
}
