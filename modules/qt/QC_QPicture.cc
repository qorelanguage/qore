/*
 QC_QPicture.cc
 
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

#include "QC_QPicture.h"

int CID_QPICTURE;
QoreClass *QC_QPicture = 0;

static void QPICTURE_constructor(class QoreObject *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreQPicture *qp;
   QoreNode *p = get_param(params, 0);

   if (p && p->type == NT_OBJECT)
   {
      QoreQPicture *pic = (QoreQPicture *)p->val.object->getReferencedPrivateData(CID_QPICTURE, xsink);
      if (!pic)
      {
         xsink->raiseException("QPICTURE-CONSTRUCTOR-ERROR", "object passed to QPicture::constructor() is not derived from QWidget (class: '%s')", p->val.object->getClass()->getName());
         return;
      }
    
      ReferenceHolder<QoreQPicture> holder(pic, xsink);

      qp = new QoreQPicture(*pic);
   }
   else {
      int f = !is_nothing(p) ? p->getAsInt() : -1;
      qp = new QoreQPicture(f);
   }

   self->setPrivate(CID_QPICTURE, qp);
}

static void QPICTURE_copy(class QoreObject *self, class QoreObject *old, class QoreQPicture *qlcdn, ExceptionSink *xsink)
{
   self->setPrivate(CID_QPICTURE, new QoreQPicture(*qlcdn));
   //xsink->raiseException("QPICTURE-COPY-ERROR", "objects of this class cannot be copied");
}


//QRect boundingRect () const
static QoreNode *QPICTURE_boundingRect(QoreObject *self, QoreQPicture *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreQRect *q_qr = new QoreQRect(qp->boundingRect());
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//const char * data () const
static QoreNode *QPICTURE_data(QoreObject *self, QoreQPicture *qp, QoreNode *params, ExceptionSink *xsink)
{
   const char *ptr = qp->data();
   if (!ptr)
      return 0;

   BinaryObject *b = new BinaryObject();
   b->append(ptr, qp->size());
   return new QoreNode(b);
}

//DataPtr & data_ptr ()
//static QoreNode *QPICTURE_data_ptr(QoreObject *self, QoreQPicture *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qp->data_ptr());
//}

//bool isNull () const
static QoreNode *QPICTURE_isNull(QoreObject *self, QoreQPicture *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qp->isNull());
}

//bool load ( const QString & fileName, const char * format = 0 )
//bool load ( QIODevice * dev, const char * format = 0 )
static QoreNode *QPICTURE_load(QoreObject *self, QoreQPicture *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QPICTURE-LOAD-PARAM-ERROR", "expecting a string as first argument to QPicture::load()");
      return 0;
   }
   const char *fileName = p->val.String->getBuffer();
   p = get_param(params, 1);
   const char *format = p ? p->val.String->getBuffer() : 0;
   return new QoreNode(qp->load(fileName, format));
}

//bool play ( QPainter * painter )
static QoreNode *QPICTURE_play(QoreObject *self, QoreQPicture *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPainter *painter = (p && p->type == NT_OBJECT) ? (QoreQPainter *)p->val.object->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QPICTURE-PLAY-PARAM-ERROR", "expecting a QPainter object as first argument to QPicture::play()");
      return 0;
   }
   ReferenceHolder<QoreQPainter> holder(painter, xsink);
   return new QoreNode(qp->play((QPainter *)painter));
}

//bool save ( const QString & fileName, const char * format = 0 )
//bool save ( QIODevice * dev, const char * format = 0 )
static QoreNode *QPICTURE_save(QoreObject *self, QoreQPicture *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QPICTURE-SAVE-PARAM-ERROR", "expecting a string as first argument to QPicture::save()");
      return 0;
   }
   const char *fileName = p->val.String->getBuffer();
   p = get_param(params, 1);
   const char *format = p ? p->val.String->getBuffer() : 0;
   return new QoreNode(qp->save(fileName, format));
}

//void setBoundingRect ( const QRect & r )
static QoreNode *QPICTURE_setBoundingRect(QoreObject *self, QoreQPicture *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRect *r = (p && p->type == NT_OBJECT) ? (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!r) {
      if (!xsink->isException())
         xsink->raiseException("QPICTURE-SETBOUNDINGRECT-PARAM-ERROR", "expecting a QRect object as first argument to QPicture::setBoundingRect()");
      return 0;
   }
   ReferenceHolder<QoreQRect> holder(r, xsink);
   qp->setBoundingRect(*((QRect *)r));
   return 0;
}

//virtual void setData ( const char * data, uint size )
static QoreNode *QPICTURE_setData(QoreObject *self, QoreQPicture *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QPICTURE-SETDATA-PARAM-ERROR", "expecting a string as first argument to QPicture::setData()");
      return 0;
   }
   const char *data = p->val.String->getBuffer();
   p = get_param(params, 1);
   int size = p ? p->getAsInt() : 0;
   qp->setData(data, size);
   return 0;
}

//uint size () const
static QoreNode *QPICTURE_size(QoreObject *self, QoreQPicture *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qp->size());
}

class QoreClass *initQPictureClass(class QoreClass *qpaintdevice)
{
   tracein("initQPictureClass()");
   
   QC_QPicture = new QoreClass("QPicture", QDOM_GUI);
   CID_QPICTURE = QC_QPicture->getID();

   QC_QPicture->addBuiltinVirtualBaseClass(qpaintdevice);

   QC_QPicture->setConstructor(QPICTURE_constructor);
   QC_QPicture->setCopy((q_copy_t)QPICTURE_copy);

   QC_QPicture->addMethod("boundingRect",                (q_method_t)QPICTURE_boundingRect);
   QC_QPicture->addMethod("data",                        (q_method_t)QPICTURE_data);
   //QC_QPicture->addMethod("data_ptr",                    (q_method_t)QPICTURE_data_ptr);
   QC_QPicture->addMethod("isNull",                      (q_method_t)QPICTURE_isNull);
   QC_QPicture->addMethod("load",                        (q_method_t)QPICTURE_load);
   QC_QPicture->addMethod("play",                        (q_method_t)QPICTURE_play);
   QC_QPicture->addMethod("save",                        (q_method_t)QPICTURE_save);
   QC_QPicture->addMethod("setBoundingRect",             (q_method_t)QPICTURE_setBoundingRect);
   QC_QPicture->addMethod("setData",                     (q_method_t)QPICTURE_setData);
   QC_QPicture->addMethod("size",                        (q_method_t)QPICTURE_size);

   traceout("initQPictureClass()");
   return QC_QPicture;
}
