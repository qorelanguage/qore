/*
 QC_QClipboard.cc
 
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

#include "QC_QClipboard.h"
#include "QC_QImage.h"
#include "QC_QPixmap.h"
#include "QC_QMimeData.h"

#include "qore-qt.h"

int CID_QCLIPBOARD;
class QoreClass *QC_QClipboard = 0;

//QClipboard ()
static void QCLIPBOARD_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("QCLIPBOARD-CONSTRUCTOR-ERROR", "this class cannot be created directly (or subclassed); to get a QClipboard object call QApplication_clipboard()");
}

static void QCLIPBOARD_copy(class QoreObject *self, class QoreObject *old, class QoreQClipboard *qc, ExceptionSink *xsink)
{
   xsink->raiseException("QCLIPBOARD-COPY-ERROR", "objects of this class cannot be copied");
}

//void clear ( Mode mode = Clipboard )
static AbstractQoreNode *QCLIPBOARD_clear(QoreObject *self, QoreQClipboard *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QClipboard::Mode mode = (QClipboard::Mode)(p ? p->getAsInt() : 0);
   qc->qobj->clear(mode);
   return 0;
}

//QImage image ( Mode mode = Clipboard ) const
static AbstractQoreNode *QCLIPBOARD_image(QoreObject *self, QoreQClipboard *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QClipboard::Mode mode = (QClipboard::Mode)(p ? p->getAsInt() : 0);
   QoreObject *o_qi = new QoreObject(QC_QImage, getProgram());
   QoreQImage *q_qi = new QoreQImage(qc->qobj->image(mode));
   o_qi->setPrivate(CID_QIMAGE, q_qi);
   return o_qi;
}

//const QMimeData * mimeData ( Mode mode = Clipboard ) const
static AbstractQoreNode *QCLIPBOARD_mimeData(QoreObject *self, QoreQClipboard *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QClipboard::Mode mode = (QClipboard::Mode)(p ? p->getAsInt() : 0);
   const QMimeData *qt_qobj = qc->qobj->mimeData(mode);
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return rv_obj;
}

//bool ownsClipboard () const
static AbstractQoreNode *QCLIPBOARD_ownsClipboard(QoreObject *self, QoreQClipboard *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qc->qobj->ownsClipboard());
}

//bool ownsFindBuffer () const
static AbstractQoreNode *QCLIPBOARD_ownsFindBuffer(QoreObject *self, QoreQClipboard *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qc->qobj->ownsFindBuffer());
}

//bool ownsSelection () const
static AbstractQoreNode *QCLIPBOARD_ownsSelection(QoreObject *self, QoreQClipboard *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qc->qobj->ownsSelection());
}

//QPixmap pixmap ( Mode mode = Clipboard ) const
static AbstractQoreNode *QCLIPBOARD_pixmap(QoreObject *self, QoreQClipboard *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QClipboard::Mode mode = (QClipboard::Mode)(p ? p->getAsInt() : 0);
   QoreObject *o_qp = new QoreObject(QC_QPixmap, getProgram());
   QoreQPixmap *q_qp = new QoreQPixmap(qc->qobj->pixmap(mode));
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return o_qp;
}

//void setImage ( const QImage & image, Mode mode = Clipboard )
static AbstractQoreNode *QCLIPBOARD_setImage(QoreObject *self, QoreQClipboard *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o = test_object_param(params, 0);
   QoreQImage *image = o ? (QoreQImage *)o->getReferencedPrivateData(CID_QIMAGE, xsink) : 0;
   if (!image) {
      if (!xsink->isException())
         xsink->raiseException("QCLIPBOARD-SETIMAGE-PARAM-ERROR", "expecting a QImage object as first argument to QClipboard::setImage()");
      return 0;
   }
   ReferenceHolder<QoreQImage> imageHolder(image, xsink);
   const AbstractQoreNode *p = get_param(params, 1);
   QClipboard::Mode mode = (QClipboard::Mode)(p ? p->getAsInt() : 0);
   qc->qobj->setImage(*(static_cast<QImage *>(image)), mode);
   return 0;
}

//void setMimeData ( QMimeData * src, Mode mode = Clipboard )
static AbstractQoreNode *QCLIPBOARD_setMimeData(QoreObject *self, QoreQClipboard *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o = test_object_param(params, 0);
   QoreQMimeData *src = o ? (QoreQMimeData *)o->getReferencedPrivateData(CID_QMIMEDATA, xsink) : 0;
   if (!src) {
      if (!xsink->isException())
         xsink->raiseException("QCLIPBOARD-SETMIMEDATA-PARAM-ERROR", "expecting a QMimeData object as first argument to QClipboard::setMimeData()");
      return 0;
   }
   ReferenceHolder<QoreQMimeData> srcHolder(src, xsink);
   const AbstractQoreNode *p = get_param(params, 1);
   QClipboard::Mode mode = (QClipboard::Mode)(p ? p->getAsInt() : 0);
   qc->qobj->setMimeData(static_cast<QMimeData *>(src->qobj), mode);
   return 0;
}

//void setPixmap ( const QPixmap & pixmap, Mode mode = Clipboard )
static AbstractQoreNode *QCLIPBOARD_setPixmap(QoreObject *self, QoreQClipboard *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o = test_object_param(params, 0);
   QoreQPixmap *pixmap = o ? (QoreQPixmap *)o->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (!pixmap) {
      if (!xsink->isException())
         xsink->raiseException("QCLIPBOARD-SETPIXMAP-PARAM-ERROR", "expecting a QPixmap object as first argument to QClipboard::setPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pixmapHolder((AbstractPrivateData *)pixmap, xsink);
   
   const AbstractQoreNode *p = get_param(params, 1);
   QClipboard::Mode mode = (QClipboard::Mode)(p ? p->getAsInt() : 0);
   qc->qobj->setPixmap(*(static_cast<QPixmap *>(pixmap)), mode);
   return 0;
}

//void setText ( const QString & text, Mode mode = Clipboard )
static AbstractQoreNode *QCLIPBOARD_setText(QoreObject *self, QoreQClipboard *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;

   p = get_param(params, 1);
   QClipboard::Mode mode = (QClipboard::Mode)(p ? p->getAsInt() : 0);
   qc->qobj->setText(text, mode);
   return 0;
}

//bool supportsFindBuffer () const
static AbstractQoreNode *QCLIPBOARD_supportsFindBuffer(QoreObject *self, QoreQClipboard *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qc->qobj->supportsFindBuffer());
}

//bool supportsSelection () const
static AbstractQoreNode *QCLIPBOARD_supportsSelection(QoreObject *self, QoreQClipboard *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qc->qobj->supportsSelection());
}

//QString text ( Mode mode = Clipboard ) const
//QString text ( QString & subtype, Mode mode = Clipboard ) const
static AbstractQoreNode *QCLIPBOARD_text(QoreObject *self, QoreQClipboard *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);

   const char *subtype = 0;
   {
      const QoreStringNode *pstr = dynamic_cast<const QoreStringNode *>(p);
      if (pstr) {
	 subtype = pstr->getBuffer();
	 p = get_param(params, 1);
      }
   }
   QClipboard::Mode mode = p ? (QClipboard::Mode)p->getAsInt() : QClipboard::Clipboard;

   if (subtype) {
      QString st(subtype);
      QString rv = qc->qobj->text(st, mode);
      QoreHashNode *h = new QoreHashNode();
      h->setKeyValue("subtype", new QoreStringNode(st.toUtf8().data(), QCS_UTF8), 0);
      h->setKeyValue("text", new QoreStringNode(rv.toUtf8().data(), QCS_UTF8), 0);

      return h;
   }

   return new QoreStringNode(qc->qobj->text(mode).toUtf8().data(), QCS_UTF8);
}

QoreClass *initQClipboardClass(QoreClass *qobject)
{
   QC_QClipboard = new QoreClass("QClipboard", QDOM_GUI);
   CID_QCLIPBOARD = QC_QClipboard->getID();

   QC_QClipboard->addBuiltinVirtualBaseClass(qobject);

   QC_QClipboard->setConstructor(QCLIPBOARD_constructor);
   QC_QClipboard->setCopy((q_copy_t)QCLIPBOARD_copy);

   QC_QClipboard->addMethod("clear",                       (q_method_t)QCLIPBOARD_clear);
   QC_QClipboard->addMethod("image",                       (q_method_t)QCLIPBOARD_image);
   QC_QClipboard->addMethod("mimeData",                    (q_method_t)QCLIPBOARD_mimeData);
   QC_QClipboard->addMethod("ownsClipboard",               (q_method_t)QCLIPBOARD_ownsClipboard);
   QC_QClipboard->addMethod("ownsFindBuffer",              (q_method_t)QCLIPBOARD_ownsFindBuffer);
   QC_QClipboard->addMethod("ownsSelection",               (q_method_t)QCLIPBOARD_ownsSelection);
   QC_QClipboard->addMethod("pixmap",                      (q_method_t)QCLIPBOARD_pixmap);
   QC_QClipboard->addMethod("setImage",                    (q_method_t)QCLIPBOARD_setImage);
   QC_QClipboard->addMethod("setMimeData",                 (q_method_t)QCLIPBOARD_setMimeData);
   QC_QClipboard->addMethod("setPixmap",                   (q_method_t)QCLIPBOARD_setPixmap);
   QC_QClipboard->addMethod("setText",                     (q_method_t)QCLIPBOARD_setText);
   QC_QClipboard->addMethod("supportsFindBuffer",          (q_method_t)QCLIPBOARD_supportsFindBuffer);
   QC_QClipboard->addMethod("supportsSelection",           (q_method_t)QCLIPBOARD_supportsSelection);
   QC_QClipboard->addMethod("text",                        (q_method_t)QCLIPBOARD_text);

   return QC_QClipboard;
}
