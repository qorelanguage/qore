/*
 QC_QTextOption.cc
 
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

#include "QC_QTextOption.h"

qore_classid_t CID_QTEXTOPTION;
QoreClass *QC_QTextOption = 0;

//QTextOption ()
//QTextOption ( Qt::Alignment alignment )
//QTextOption ( const QTextOption & other )
static void QTEXTOPTION_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QTEXTOPTION, new QoreQTextOption());
      return;
   }
   Qt::Alignment alignment = (Qt::Alignment)(p ? p->getAsInt() : 0);
   self->setPrivate(CID_QTEXTOPTION, new QoreQTextOption(alignment));
   return;
}

static void QTEXTOPTION_copy(QoreObject *self, QoreObject *old, QoreQTextOption *qto, ExceptionSink *xsink)
{
   self->setPrivate(CID_QTEXTOPTION, new QoreQTextOption(*qto));
}

//Qt::Alignment alignment () const
static AbstractQoreNode *QTEXTOPTION_alignment(QoreObject *self, QoreQTextOption *qto, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qto->alignment());
}

//Flags flags () const
static AbstractQoreNode *QTEXTOPTION_flags(QoreObject *self, QoreQTextOption *qto, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qto->flags());
}

//void setAlignment ( Qt::Alignment alignment )
static AbstractQoreNode *QTEXTOPTION_setAlignment(QoreObject *self, QoreQTextOption *qto, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::Alignment alignment = (Qt::Alignment)(p ? p->getAsInt() : 0);
   qto->setAlignment(alignment);
   return 0;
}

//void setFlags ( Flags flags )
static AbstractQoreNode *QTEXTOPTION_setFlags(QoreObject *self, QoreQTextOption *qto, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QTextOption::Flags flags = (QTextOption::Flags)(p ? p->getAsInt() : 0);
   qto->setFlags(flags);
   return 0;
}

//void setTabArray ( QList<qreal> tabStops )
static AbstractQoreNode *QTEXTOPTION_setTabArray(QoreObject *self, QoreQTextOption *qto, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (!p || p->getType() != NT_LIST) {
      xsink->raiseException("QTEXTOPTION-SETTABARRAY-PARAM-ERROR", "expecting a list as first argument to QTextOption::setTabArray()");
      return 0;
   }
   QList<qreal> tabStops;
   ConstListIterator li_tabStops(reinterpret_cast<const QoreListNode *>(p));
   while (li_tabStops.next()) {
      const AbstractQoreNode *n = li_tabStops.getValue();
      tabStops.push_back(n ? n->getAsFloat() : 0);
   }
   qto->setTabArray(tabStops);
   return 0;
}

//void setTabStop ( qreal tabStop )
static AbstractQoreNode *QTEXTOPTION_setTabStop(QoreObject *self, QoreQTextOption *qto, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal tabStop = p ? p->getAsFloat() : 0.0;
   qto->setTabStop(tabStop);
   return 0;
}

//void setTextDirection ( Qt::LayoutDirection direction )
static AbstractQoreNode *QTEXTOPTION_setTextDirection(QoreObject *self, QoreQTextOption *qto, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::LayoutDirection direction = (Qt::LayoutDirection)(p ? p->getAsInt() : 0);
   qto->setTextDirection(direction);
   return 0;
}

//void setUseDesignMetrics ( bool enable )
static AbstractQoreNode *QTEXTOPTION_setUseDesignMetrics(QoreObject *self, QoreQTextOption *qto, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qto->setUseDesignMetrics(enable);
   return 0;
}

//void setWrapMode ( WrapMode mode )
static AbstractQoreNode *QTEXTOPTION_setWrapMode(QoreObject *self, QoreQTextOption *qto, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QTextOption::WrapMode mode = (QTextOption::WrapMode)(p ? p->getAsInt() : 0);
   qto->setWrapMode(mode);
   return 0;
}

//QList<qreal> tabArray () const
static AbstractQoreNode *QTEXTOPTION_tabArray(QoreObject *self, QoreQTextOption *qto, const QoreListNode *params, ExceptionSink *xsink)
{
   QList<qreal> ilist_rv = qto->tabArray();
   QoreListNode *l = new QoreListNode();
   for (QList<qreal>::iterator i = ilist_rv.begin(), e = ilist_rv.end(); i != e; ++i)
      l->push(new QoreFloatNode((*i)));
   return l;
}

//qreal tabStop () const
static AbstractQoreNode *QTEXTOPTION_tabStop(QoreObject *self, QoreQTextOption *qto, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qto->tabStop());
}

//Qt::LayoutDirection textDirection () const
static AbstractQoreNode *QTEXTOPTION_textDirection(QoreObject *self, QoreQTextOption *qto, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qto->textDirection());
}

//bool useDesignMetrics () const
static AbstractQoreNode *QTEXTOPTION_useDesignMetrics(QoreObject *self, QoreQTextOption *qto, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qto->useDesignMetrics());
}

//WrapMode wrapMode () const
static AbstractQoreNode *QTEXTOPTION_wrapMode(QoreObject *self, QoreQTextOption *qto, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qto->wrapMode());
}

static QoreClass *initQTextOptionClass()
{
   QC_QTextOption = new QoreClass("QTextOption", QDOM_GUI);
   CID_QTEXTOPTION = QC_QTextOption->getID();

   QC_QTextOption->setConstructor(QTEXTOPTION_constructor);
   QC_QTextOption->setCopy((q_copy_t)QTEXTOPTION_copy);

   QC_QTextOption->addMethod("alignment",                   (q_method_t)QTEXTOPTION_alignment);
   QC_QTextOption->addMethod("flags",                       (q_method_t)QTEXTOPTION_flags);
   QC_QTextOption->addMethod("setAlignment",                (q_method_t)QTEXTOPTION_setAlignment);
   QC_QTextOption->addMethod("setFlags",                    (q_method_t)QTEXTOPTION_setFlags);
   QC_QTextOption->addMethod("setTabArray",                 (q_method_t)QTEXTOPTION_setTabArray);
   QC_QTextOption->addMethod("setTabStop",                  (q_method_t)QTEXTOPTION_setTabStop);
   QC_QTextOption->addMethod("setTextDirection",            (q_method_t)QTEXTOPTION_setTextDirection);
   QC_QTextOption->addMethod("setUseDesignMetrics",         (q_method_t)QTEXTOPTION_setUseDesignMetrics);
   QC_QTextOption->addMethod("setWrapMode",                 (q_method_t)QTEXTOPTION_setWrapMode);
   QC_QTextOption->addMethod("tabArray",                    (q_method_t)QTEXTOPTION_tabArray);
   QC_QTextOption->addMethod("tabStop",                     (q_method_t)QTEXTOPTION_tabStop);
   QC_QTextOption->addMethod("textDirection",               (q_method_t)QTEXTOPTION_textDirection);
   QC_QTextOption->addMethod("useDesignMetrics",            (q_method_t)QTEXTOPTION_useDesignMetrics);
   QC_QTextOption->addMethod("wrapMode",                    (q_method_t)QTEXTOPTION_wrapMode);

   return QC_QTextOption;
}

QoreNamespace *initQTextOptionNS()
{
   QoreNamespace *ns = new QoreNamespace("QTextOption");
   ns->addSystemClass(initQTextOptionClass());

   // Flag enum
   ns->addConstant("IncludeTrailingSpaces",    new QoreBigIntNode(QTextOption::IncludeTrailingSpaces));

   // WrapMode enum
   ns->addConstant("NoWrap",                   new QoreBigIntNode(QTextOption::NoWrap));
   ns->addConstant("WordWrap",                 new QoreBigIntNode(QTextOption::WordWrap));
   ns->addConstant("ManualWrap",               new QoreBigIntNode(QTextOption::ManualWrap));
   ns->addConstant("WrapAnywhere",             new QoreBigIntNode(QTextOption::WrapAnywhere));
   ns->addConstant("WrapAtWordBoundaryOrAnywhere", new QoreBigIntNode(QTextOption::WrapAtWordBoundaryOrAnywhere));

   return ns;
}
