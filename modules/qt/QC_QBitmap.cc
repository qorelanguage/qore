/*
 QC_QBitmap.cc
 
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

#include "QC_QBitmap.h"
#include "QC_QPixmap.h"

int CID_QBITMAP;
QoreClass *QC_QBitmap = 0;

static void QBITMAP_constructor(class QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   QoreQBitmap *qp;
   QoreNode *p = get_param(params, 0);

   if (p && p->type == NT_OBJECT) {
      AbstractPrivateData *apd = p->val.object->getReferencedPrivateData(CID_QPIXMAP, xsink);
      if (!apd) {
	 if (!xsink->isException())
	    xsink->raiseException("QBITMAP-CONSTRUCTOR-PARAM-ERROR", "QBitmap::cosntructor() cannot handle arguments of class '%s' (expecting an object derived from QPixmap)", p->val.object->getClass()->getName());
	 return;
      }
      ReferenceHolder<AbstractPrivateData> holder(apd, xsink);
      QoreAbstractQPixmap *qpix = dynamic_cast<QoreAbstractQPixmap *>(apd);
      assert(qpix);

      qp = new QoreQBitmap(*(qpix->getQPixmap()));
   }
   if (p && p->type == NT_STRING) {
      const char *filename = p->val.String->getBuffer();

      p = get_param(params, 1);
      const char *format = p ? p->val.String->getBuffer() : 0;

      qp = new QoreQBitmap(filename, format);
   }
   else {
      int w = p ? p->getAsInt() : 0;
      p = get_param(params, 1);
      int h = p ? p->getAsInt() : 0;
      qp = new QoreQBitmap(w, h);
   }

   self->setPrivate(CID_QBITMAP, qp);
}

static void QBITMAP_copy(class QoreObject *self, class QoreObject *old, class QoreQBitmap *qlcdn, ExceptionSink *xsink)
{
   self->setPrivate(CID_QBITMAP, new QoreQBitmap(*qlcdn));
   //xsink->raiseException("QBITMAP-COPY-ERROR", "objects of this class cannot be copied");
}

//void clear ()
static QoreNode *QBITMAP_clear(QoreObject *self, QoreQBitmap *qb, const QoreNode *params, ExceptionSink *xsink)
{
   qb->clear();
   return 0;
}

//QBitmap transformed ( const QMatrix & matrix ) const
//QBitmap transformed ( const QTransform & matrix ) const
//static QoreNode *QBITMAP_transformed(QoreObject *self, QoreQBitmap *qb, const QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QMatrix matrix = p;
//   QoreObject *o_qb = new QoreObject(self->getClass(CID_QBITMAP), getProgram());
//   QoreQBitmap *q_qb = new QoreQBitmap(qb->transformed(matrix));
//   o_qb->setPrivate(CID_QBITMAP, q_qb);
//   return new QoreNode(o_qb);
//}


class QoreClass *initQBitmapClass(class QoreClass *qpixmap)
{
   tracein("initQBitmapClass()");
   
   QC_QBitmap = new QoreClass("QBitmap", QDOM_GUI);
   CID_QBITMAP = QC_QBitmap->getID();

   QC_QBitmap->addBuiltinVirtualBaseClass(qpixmap);

   QC_QBitmap->setConstructor(QBITMAP_constructor);
   QC_QBitmap->setCopy((q_copy_t)QBITMAP_copy);


   QC_QBitmap->addMethod("clear",                       (q_method_t)QBITMAP_clear);
   //QC_QBitmap->addMethod("transformed",                 (q_method_t)QBITMAP_transformed);

   traceout("initQBitmapClass()");
   return QC_QBitmap;
}
