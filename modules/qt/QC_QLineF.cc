/*
 QC_QLineF.cc
 
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

#include "QC_QLineF.h"
#include "QC_QLine.h"
#include "QC_QPointF.h"

#include "qore-qt.h"

int CID_QLINEF;
class QoreClass *QC_QLineF = 0;

//QLineF ()
//QLineF ( const QPointF & p1, const QPointF & p2 )
//QLineF ( qreal x1, qreal y1, qreal x2, qreal y2 )
//QLineF ( const QLine & line )
static void QLINEF_constructor(QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QLINEF, new QoreQLineF());
      return;
   }
   if (p && p->type == NT_OBJECT) {
      QoreQLine *line = (QoreQLine *)p->val.object->getReferencedPrivateData(CID_QLINE, xsink);
      if (!line) {
         QoreQPointF *p1 = (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink);
         if (!p1) {
            if (!xsink->isException())
               xsink->raiseException("QLINEF-QLINEF-PARAM-ERROR", "QLineF::QLineF() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
            return;
         }
         ReferenceHolder<QoreQPointF> p1Holder(p1, xsink);
         p = get_param(params, 1);
         QoreQPointF *p2 = p ? (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
         if (!p2) {
            if (!xsink->isException())
               xsink->raiseException("QLINEF-QLINEF-PARAM-ERROR", "this version of QLineF::QLineF() expects an object derived from QPointF as the second argument", p->val.object->getClass()->getName());
            return;
         }
         ReferenceHolder<QoreQPointF> p2Holder(p2, xsink);
         self->setPrivate(CID_QLINEF, new QoreQLineF(*(static_cast<QPointF *>(p1)), *(static_cast<QPointF *>(p2))));
         return;
      }
      ReferenceHolder<QoreQLine> lineHolder(line, xsink);
      self->setPrivate(CID_QLINEF, new QoreQLineF(*(static_cast<QLine *>(line))));
      return;
   }
   qreal x1 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y1 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal x2 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal y2 = p ? p->getAsFloat() : 0.0;
   self->setPrivate(CID_QLINEF, new QoreQLineF(x1, y1, x2, y2));
   return;
}

static void QLINEF_copy(class QoreObject *self, class QoreObject *old, class QoreQLineF *qlf, ExceptionSink *xsink)
{
   self->setPrivate(CID_QLINEF, new QoreQLineF(*qlf));
   //xsink->raiseException("QLINEF-COPY-ERROR", "objects of this class cannot be copied");
}

//QPointF p1 () const
static QoreNode *QLINEF_p1(QoreObject *self, QoreQLineF *qlf, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_qpf = new QoreObject(QC_QPointF, getProgram());
   QoreQPointF *q_qpf = new QoreQPointF(qlf->p1());
   o_qpf->setPrivate(CID_QPOINTF, q_qpf);
   return new QoreNode(o_qpf);
}

//QPointF p2 () const
static QoreNode *QLINEF_p2(QoreObject *self, QoreQLineF *qlf, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_qpf = new QoreObject(QC_QPointF, getProgram());
   QoreQPointF *q_qpf = new QoreQPointF(qlf->p2());
   o_qpf->setPrivate(CID_QPOINTF, q_qpf);
   return new QoreNode(o_qpf);
}

//qreal x1 () const
static QoreNode *QLINEF_x1(QoreObject *self, QoreQLineF *qlf, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qlf->x1());
}

//qreal x2 () const
static QoreNode *QLINEF_x2(QoreObject *self, QoreQLineF *qlf, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qlf->x2());
}

//qreal y1 () const
static QoreNode *QLINEF_y1(QoreObject *self, QoreQLineF *qlf, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qlf->y1());
}

//qreal y2 () const
static QoreNode *QLINEF_y2(QoreObject *self, QoreQLineF *qlf, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qlf->y2());
}

//qreal angle ( const QLineF & line ) const
static QoreNode *QLINEF_angle(QoreObject *self, QoreQLineF *qlf, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQLineF *line = (p && p->type == NT_OBJECT) ? (QoreQLineF *)p->val.object->getReferencedPrivateData(CID_QLINEF, xsink) : 0;
   if (!line) {
      if (!xsink->isException())
         xsink->raiseException("QLINEF-ANGLE-PARAM-ERROR", "expecting a QLineF object as first argument to QLineF::angle()");
      return 0;
   }
   ReferenceHolder<QoreQLineF> lineHolder(line, xsink);
   return new QoreNode((double)qlf->angle(*(static_cast<QLineF *>(line))));
}

//qreal dx () const
static QoreNode *QLINEF_dx(QoreObject *self, QoreQLineF *qlf, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qlf->dx());
}

//qreal dy () const
static QoreNode *QLINEF_dy(QoreObject *self, QoreQLineF *qlf, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qlf->dy());
}

////IntersectType intersect ( const QLineF & line, QPointF * intersectionPoint ) const
//static QoreNode *QLINEF_intersect(QoreObject *self, QoreQLineF *qlf, const QoreList *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   QoreQLineF *line = (p && p->type == NT_OBJECT) ? (QoreQLineF *)p->val.object->getReferencedPrivateData(CID_QLINEF, xsink) : 0;
//   if (!line) {
//      if (!xsink->isException())
//         xsink->raiseException("QLINEF-INTERSECT-PARAM-ERROR", "expecting a QLineF object as first argument to QLineF::intersect()");
//      return 0;
//   }
//   ReferenceHolder<QoreQLineF> lineHolder(line, xsink);
//   p = get_param(params, 1);
//   QoreQPointF *intersectionPoint = (p && p->type == NT_OBJECT) ? (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
//   if (!intersectionPoint) {
//      if (!xsink->isException())
//         xsink->raiseException("QLINEF-INTERSECT-PARAM-ERROR", "expecting a QPointF object as second argument to QLineF::intersect()");
//      return 0;
//   }
//   ReferenceHolder<QoreQPointF> intersectionPointHolder(intersectionPoint, xsink);
//   ??? return new QoreNode((int64)qlf->intersect(*(static_cast<QLineF *>(line)), *(*(static_cast<QPointF *>(intersectionPoint)))));
//}

//bool isNull () const
static QoreNode *QLINEF_isNull(QoreObject *self, QoreQLineF *qlf, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qlf->isNull());
}

//qreal length () const
static QoreNode *QLINEF_length(QoreObject *self, QoreQLineF *qlf, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qlf->length());
}

//QLineF normalVector () const
static QoreNode *QLINEF_normalVector(QoreObject *self, QoreQLineF *qlf, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_qlf = new QoreObject(self->getClass(CID_QLINEF), getProgram());
   QoreQLineF *q_qlf = new QoreQLineF(qlf->normalVector());
   o_qlf->setPrivate(CID_QLINEF, q_qlf);
   return new QoreNode(o_qlf);
}

//QPointF pointAt ( qreal t ) const
static QoreNode *QLINEF_pointAt(QoreObject *self, QoreQLineF *qlf, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   qreal t = p ? p->getAsFloat() : 0.0;
   QoreObject *o_qpf = new QoreObject(QC_QPointF, getProgram());
   QoreQPointF *q_qpf = new QoreQPointF(qlf->pointAt(t));
   o_qpf->setPrivate(CID_QPOINTF, q_qpf);
   return new QoreNode(o_qpf);
}

//void setLength ( qreal length )
static QoreNode *QLINEF_setLength(QoreObject *self, QoreQLineF *qlf, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   qreal length = p ? p->getAsFloat() : 0.0;
   qlf->setLength(length);
   return 0;
}

//QLine toLine () const
static QoreNode *QLINEF_toLine(QoreObject *self, QoreQLineF *qlf, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_ql = new QoreObject(QC_QLine, getProgram());
   QoreQLine *q_ql = new QoreQLine(qlf->toLine());
   o_ql->setPrivate(CID_QLINE, q_ql);
   return new QoreNode(o_ql);
}

//void translate ( const QPointF & offset )
//void translate ( qreal dx, qreal dy )
static QoreNode *QLINEF_translate(QoreObject *self, QoreQLineF *qlf, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPointF *offset = (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink);
      if (!offset) {
         if (!xsink->isException())
            xsink->raiseException("QLINEF-TRANSLATE-PARAM-ERROR", "QLineF::translate() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<QoreQPointF> offsetHolder(offset, xsink);
      qlf->translate(*(static_cast<QPointF *>(offset)));
      return 0;
   }
   qreal dx = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal dy = p ? p->getAsFloat() : 0.0;
   qlf->translate(dx, dy);
   return 0;
}

//QLineF unitVector () const
static QoreNode *QLINEF_unitVector(QoreObject *self, QoreQLineF *qlf, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_qlf = new QoreObject(self->getClass(CID_QLINEF), getProgram());
   QoreQLineF *q_qlf = new QoreQLineF(qlf->unitVector());
   o_qlf->setPrivate(CID_QLINEF, q_qlf);
   return new QoreNode(o_qlf);
}

QoreClass *initQLineFClass()
{
   QC_QLineF = new QoreClass("QLineF", QDOM_GUI);
   CID_QLINEF = QC_QLineF->getID();

   QC_QLineF->setConstructor(QLINEF_constructor);
   QC_QLineF->setCopy((q_copy_t)QLINEF_copy);

   QC_QLineF->addMethod("p1",                          (q_method_t)QLINEF_p1);
   QC_QLineF->addMethod("p2",                          (q_method_t)QLINEF_p2);
   QC_QLineF->addMethod("x1",                          (q_method_t)QLINEF_x1);
   QC_QLineF->addMethod("x2",                          (q_method_t)QLINEF_x2);
   QC_QLineF->addMethod("y1",                          (q_method_t)QLINEF_y1);
   QC_QLineF->addMethod("y2",                          (q_method_t)QLINEF_y2);
   QC_QLineF->addMethod("angle",                       (q_method_t)QLINEF_angle);
   QC_QLineF->addMethod("dx",                          (q_method_t)QLINEF_dx);
   QC_QLineF->addMethod("dy",                          (q_method_t)QLINEF_dy);
   //QC_QLineF->addMethod("intersect",                   (q_method_t)QLINEF_intersect);
   QC_QLineF->addMethod("isNull",                      (q_method_t)QLINEF_isNull);
   QC_QLineF->addMethod("length",                      (q_method_t)QLINEF_length);
   QC_QLineF->addMethod("normalVector",                (q_method_t)QLINEF_normalVector);
   QC_QLineF->addMethod("pointAt",                     (q_method_t)QLINEF_pointAt);
   QC_QLineF->addMethod("setLength",                   (q_method_t)QLINEF_setLength);
   QC_QLineF->addMethod("toLine",                      (q_method_t)QLINEF_toLine);
   QC_QLineF->addMethod("translate",                   (q_method_t)QLINEF_translate);
   QC_QLineF->addMethod("unitVector",                  (q_method_t)QLINEF_unitVector);

   return QC_QLineF;
}
