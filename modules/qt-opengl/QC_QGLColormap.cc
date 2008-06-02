/*
 QC_QGLColormap.cc
 
 Qore Programming Language
 
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

#include "qore-qt-gui.h"

#include "QC_QGLColormap.h"
#include "QC_QColor.h"

int CID_QGLCOLORMAP;
QoreClass *QC_QGLColormap = 0;

//QGLColormap ()
//QGLColormap ( const QGLColormap & map )
static void QGLCOLORMAP_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QGLCOLORMAP, new QoreQGLColormap());
   return;
}

static void QGLCOLORMAP_copy(QoreObject *self, QoreObject *old, QoreQGLColormap *qglc, ExceptionSink *xsink)
{
   xsink->raiseException("QGLCOLORMAP-COPY-ERROR", "objects of this class cannot be copied");
}

//QColor entryColor ( int idx ) const
static AbstractQoreNode *QGLCOLORMAP_entryColor(QoreObject *self, QoreQGLColormap *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int idx = p ? p->getAsInt() : 0;
   return return_object(QC_QColor, new QoreQColor(qglc->entryColor(idx)));
}

//QRgb entryRgb ( int idx ) const
static AbstractQoreNode *QGLCOLORMAP_entryRgb(QoreObject *self, QoreQGLColormap *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int idx = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qglc->entryRgb(idx));
}

//int find ( QRgb color ) const
static AbstractQoreNode *QGLCOLORMAP_find(QoreObject *self, QoreQGLColormap *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int64 color = p ? p->getAsBigInt() : 0;
   return new QoreBigIntNode(qglc->find(color));
}

//int findNearest ( QRgb color ) const
static AbstractQoreNode *QGLCOLORMAP_findNearest(QoreObject *self, QoreQGLColormap *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int64 color = p ? p->getAsBigInt() : 0;
   return new QoreBigIntNode(qglc->findNearest(color));
}

//bool isEmpty () const
static AbstractQoreNode *QGLCOLORMAP_isEmpty(QoreObject *self, QoreQGLColormap *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglc->isEmpty());
}

/*
//void setEntries ( int count, const QRgb * colors, int base = 0 )
static AbstractQoreNode *QGLCOLORMAP_setEntries(QoreObject *self, QoreQGLColormap *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int count = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   ??? QRgb* colors = p;
   p = get_param(params, 2);
   int base = !is_nothing(p) ? p->getAsInt() : 0;
   qglc->setEntries(count, colors, base);
   return 0;
}
*/

//void setEntry ( int idx, QRgb color )
//void setEntry ( int idx, const QColor & color )
static AbstractQoreNode *QGLCOLORMAP_setEntry(QoreObject *self, QoreQGLColormap *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int idx = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreQColor *color = (p && p->getType() == NT_OBJECT) ? (QoreQColor *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QCOLOR, xsink) : 0;
   if (!color) {
      int64 c = p ? p->getAsBigInt() : 0;
      qglc->setEntry(idx, c);
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> colorHolder(static_cast<AbstractPrivateData *>(color), xsink);
   qglc->setEntry(idx, *(static_cast<QColor *>(color)));
   return 0;
}

//int size () const
static AbstractQoreNode *QGLCOLORMAP_size(QoreObject *self, QoreQGLColormap *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qglc->size());
}

QoreClass *initQGLColormapClass()
{
   QC_QGLColormap = new QoreClass("QGLColormap", QDOM_GUI);
   CID_QGLCOLORMAP = QC_QGLColormap->getID();

   QC_QGLColormap->setConstructor(QGLCOLORMAP_constructor);
   QC_QGLColormap->setCopy((q_copy_t)QGLCOLORMAP_copy);

   QC_QGLColormap->addMethod("entryColor",                  (q_method_t)QGLCOLORMAP_entryColor);
   QC_QGLColormap->addMethod("entryRgb",                    (q_method_t)QGLCOLORMAP_entryRgb);
   QC_QGLColormap->addMethod("find",                        (q_method_t)QGLCOLORMAP_find);
   QC_QGLColormap->addMethod("findNearest",                 (q_method_t)QGLCOLORMAP_findNearest);
   QC_QGLColormap->addMethod("isEmpty",                     (q_method_t)QGLCOLORMAP_isEmpty);
   //QC_QGLColormap->addMethod("setEntries",                  (q_method_t)QGLCOLORMAP_setEntries);
   QC_QGLColormap->addMethod("setEntry",                    (q_method_t)QGLCOLORMAP_setEntry);
   QC_QGLColormap->addMethod("size",                        (q_method_t)QGLCOLORMAP_size);

   return QC_QGLColormap;
}
