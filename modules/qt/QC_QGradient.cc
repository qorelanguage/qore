/*
 QC_QGradient.cc
 
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

#include "QC_QGradient.h"
#include "QC_QLinearGradient.h"

int CID_QGRADIENT;
class QoreClass *QC_QGradient = 0;

//QGradient ()
static void QGRADIENT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QGRADIENT, new QoreQGradient());
   return;
}

static void QGRADIENT_copy(QoreObject *self, QoreObject *old, QoreQGradient *qg, ExceptionSink *xsink)
{
   xsink->raiseException("QGRADIENT-COPY-ERROR", "objects of this class cannot be copied");
}

//void setColorAt ( qreal position, const QColor & color )
static AbstractQoreNode *QGRADIENT_setColorAt(QoreObject *self, QoreAbstractQGradient *qg, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal position = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   QoreQColor *color = (p && p->getType() == NT_OBJECT) ? (QoreQColor *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QCOLOR, xsink) : 0;
   if (!color) {
      Qt::GlobalColor color = (Qt::GlobalColor)(p ? p->getAsInt() : 0);
      qg->getQGradient()->setColorAt(position, color);
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> colorHolder(static_cast<AbstractPrivateData *>(color), xsink);
   qg->getQGradient()->setColorAt(position, *(static_cast<QColor *>(color)));
   return 0;
}

//void setSpread ( Spread method )
static AbstractQoreNode *QGRADIENT_setSpread(QoreObject *self, QoreAbstractQGradient *qg, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QGradient::Spread method = (QGradient::Spread)(p ? p->getAsInt() : 0);
   qg->getQGradient()->setSpread(method);
   return 0;
}

//void setStops ( const QGradientStops & stopPoints )
static AbstractQoreNode *QGRADIENT_setStops(QoreObject *self, QoreAbstractQGradient *qg, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (!p || p->getType() != NT_LIST) {
      xsink->raiseException("QGRADIENT-SETSTOPS-PARAM-ERROR", "expecting a list giving gradient stop points as first argument to QGradient::setStops()");
      return 0;
   }
   QGradientStops stopPoints;
   ConstListIterator li_stopPoints(reinterpret_cast<const QoreListNode *>(p));
   while (li_stopPoints.next()) {
      p = li_stopPoints.getValue();
      if (!p || p->getType() != NT_LIST || reinterpret_cast<const QoreListNode *>(p)->size() != 2) {
         xsink->raiseException("QGRADIENT-SETSTOPS-QGRADIENTSTOPS-ERROR", "expecting a 2-element list specifying a gradient stop for every element in the QGradientStops list in a call to QGradient::setStops()");
         return 0;
      }
      const QoreListNode *l = reinterpret_cast<const QoreListNode *>(p);
      const AbstractQoreNode *p_i = l->retrieve_entry(0);
      qreal r = p_i ? p_i->getAsFloat() : 0.0;
      p_i = l->retrieve_entry(1);
      QoreQColor *qc = (p_i && p_i->getType() == NT_OBJECT) ? (QoreQColor *)reinterpret_cast<const QoreObject *>(p_i)->getReferencedPrivateData(CID_QCOLOR, xsink) : 0;
      if (*xsink)
         return 0;
      if (!qc) {
         xsink->raiseException("QGRADIENT-SETSTOPS-PARAM-ERROR", "expecting an object derived from QColor as the second element in the gradient stop list in call to QGradient::setStops()");
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> qc_holder(qc, xsink);
      stopPoints.push_back(qMakePair(r, *static_cast<QColor *>(qc)));
   }
   qg->getQGradient()->setStops(stopPoints);
   return 0;
}

//Spread spread () const
static AbstractQoreNode *QGRADIENT_spread(QoreObject *self, QoreAbstractQGradient *qg, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qg->getQGradient()->spread());
}

//QGradientStops stops () const
static AbstractQoreNode *QGRADIENT_stops(QoreObject *self, QoreAbstractQGradient *qg, const QoreListNode *params, ExceptionSink *xsink)
{
   QGradientStops gs_rv = qg->getQGradient()->stops();
   QoreListNode *l = new QoreListNode();
   for (QGradientStops::iterator i = gs_rv.begin(), e = gs_rv.end(); i != e; ++i) {
      QoreListNode *e = new QoreListNode();
      e->push(new QoreFloatNode(i->first));
      e->push(return_object(QC_QColor, new QoreQColor(i->second)));
      l->push(e);
   }
   return l;
}

//Type type () const
static AbstractQoreNode *QGRADIENT_type(QoreObject *self, QoreAbstractQGradient *qg, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qg->getQGradient()->type());
}

static QoreClass *initQGradientClass()
{
   QC_QGradient = new QoreClass("QGradient", QDOM_GUI);
   CID_QGRADIENT = QC_QGradient->getID();

   QC_QGradient->setConstructor(QGRADIENT_constructor);
   QC_QGradient->setCopy((q_copy_t)QGRADIENT_copy);

   QC_QGradient->addMethod("setColorAt",                  (q_method_t)QGRADIENT_setColorAt);
   QC_QGradient->addMethod("setSpread",                   (q_method_t)QGRADIENT_setSpread);
   QC_QGradient->addMethod("setStops",                    (q_method_t)QGRADIENT_setStops);
   QC_QGradient->addMethod("spread",                      (q_method_t)QGRADIENT_spread);
   QC_QGradient->addMethod("stops",                       (q_method_t)QGRADIENT_stops);
   QC_QGradient->addMethod("type",                        (q_method_t)QGRADIENT_type);

   return QC_QGradient;
}

QoreNamespace *initQGradientNS()
{
   QoreNamespace *ns = new QoreNamespace("QGradient");
   QoreClass *qgradient;
   ns->addSystemClass((qgradient = initQGradientClass()));
   ns->addSystemClass(initQLinearGradientClass(qgradient));

   // Spread enum
   ns->addConstant("PadSpread",                new QoreBigIntNode(QGradient::PadSpread));
   ns->addConstant("ReflectSpread",            new QoreBigIntNode(QGradient::ReflectSpread));
   ns->addConstant("RepeatSpread",             new QoreBigIntNode(QGradient::RepeatSpread));   

   // Type enum
   ns->addConstant("LinearGradient",           new QoreBigIntNode(QGradient::LinearGradient));
   ns->addConstant("RadialGradient",           new QoreBigIntNode(QGradient::RadialGradient));
   ns->addConstant("ConicalGradient",          new QoreBigIntNode(QGradient::ConicalGradient));
   ns->addConstant("NoGradient",               new QoreBigIntNode(QGradient::NoGradient));

   // CoordinateMode enum
   /* <- undocumented enum
   ns->addConstant("LogicalMode",              new QoreBigIntNode(QGradient::LogicalMode));
   ns->addConstant("StretchToDeviceMode",      new QoreBigIntNode(QGradient::StretchToDeviceMode));
   ns->addConstant("ObjectBoundingMode",       new QoreBigIntNode(QGradient::ObjectBoundingMode));
   */

   return ns;
}
