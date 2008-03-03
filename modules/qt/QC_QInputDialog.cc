/*
 QC_QInputDialog.cc
 
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

#include "QC_QInputDialog.h"
#include "QC_QWidget.h"

#include "qore-qt.h"

qore_classid_t CID_QINPUTDIALOG;
class QoreClass *QC_QInputDialog = 0;

//double getDouble ( QWidget * parent, const QString & title, const QString & label, double value = 0, double minValue = -2147483647, double maxValue = 2147483647, int decimals = 1, bool * ok = 0, Qt::WindowFlags f = 0 )
static AbstractQoreNode *f_QInputDialog_getDouble(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QINPUTDIALOG-GETDOUBLE-PARAM-ERROR", "expecting a QWidget object as first argument to QInputDialog::getDouble()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);

   const AbstractQoreNode *p = get_param(params, 1);
   QString title;
   if (get_qstring(p, title, xsink))
      return 0;
   p = get_param(params, 2);
   QString label;
   if (get_qstring(p, label, xsink))
      return 0;
   p = get_param(params, 3);
   double value = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 4);
   double minValue = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 5);
   double maxValue = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 6);
   int decimals = !is_nothing(p) ? p->getAsInt() : 1;

   const ReferenceNode *pr = test_reference_param(params, 7);

   p = get_param(params, 8);
   Qt::WindowFlags f = (Qt::WindowFlags)(!is_nothing(p) ? p->getAsInt() : 0);

   bool ok;
   double rv = QInputDialog::getDouble(static_cast<QWidget *>(parent->getQWidget()), title, label, value, minValue, maxValue, decimals, &ok, f);

   if (pr) {
      class AutoVLock vl;
      class AbstractQoreNode **vp = get_var_value_ptr(pr->getExpression(), &vl, xsink);
      if (*xsink)
	 return 0;

      if (*vp)
	 (*vp)->deref(xsink);
      (*vp) = get_bool_node(ok);
   }

   return new QoreFloatNode(rv);
}

//int getInteger ( QWidget * parent, const QString & title, const QString & label, int value = 0, int minValue = -2147483647, int maxValue = 2147483647, int step = 1, bool * ok = 0, Qt::WindowFlags f = 0 )
static AbstractQoreNode *f_QInputDialog_getInteger(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QINPUTDIALOG-GETINTEGER-PARAM-ERROR", "expecting a QWidget object as first argument to QInputDialog::getInteger()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);

   const AbstractQoreNode *p = get_param(params, 1);
   QString title;
   if (get_qstring(p, title, xsink))
      return 0;

   p = get_param(params, 2);
   QString label;
   if (get_qstring(p, label, xsink))
      return 0;

   p = get_param(params, 3);
   int value = !is_nothing(p) ? p->getAsInt() : 0;

   p = get_param(params, 4);
   int minValue = !is_nothing(p) ? p->getAsInt() : -2147483647;

   p = get_param(params, 5);
   int maxValue = !is_nothing(p) ? p->getAsInt() : 2147483647;

   p = get_param(params, 6);
   int step = !is_nothing(p) ? p->getAsInt() : 1;

   const ReferenceNode *pr = test_reference_param(params, 7);

   p = get_param(params, 8);
   Qt::WindowFlags f = (Qt::WindowFlags)(!is_nothing(p) ? p->getAsInt() : 0);

   bool ok;
   int64 rv = QInputDialog::getInteger(static_cast<QWidget *>(parent->getQWidget()), title, label, value, minValue, maxValue, step, &ok, f);

   if (pr) {
      class AutoVLock vl;
      class AbstractQoreNode **vp = get_var_value_ptr(pr->getExpression(), &vl, xsink);
      if (*xsink)
	 return 0;

      if (*vp)
	 (*vp)->deref(xsink);
      (*vp) = get_bool_node(ok);
   }

   return new QoreBigIntNode(rv);
}

//QString getItem ( QWidget * parent, const QString & title, const QString & label, const QStringList & list, int current = 0, bool editable = true, bool * ok = 0, Qt::WindowFlags f = 0 )
static AbstractQoreNode *f_QInputDialog_getItem(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QINPUTDIALOG-GETITEM-PARAM-ERROR", "expecting a QWidget object as first argument to QInputDialog::getItem()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);

   const AbstractQoreNode *p = get_param(params, 1);
   QString title;
   if (get_qstring(p, title, xsink))
      return 0;

   p = get_param(params, 2);
   QString label;
   if (get_qstring(p, label, xsink))
      return 0;
   const QoreListNode *l = test_list_param(params, 3);
   if (!l) {
      xsink->raiseException("QINPUTDIALOG-GETITEM-PARAM-ERROR", "expecting a list as fourth argument to QInputDialog::getItem()");
      return 0;
   }
   QStringList list;
   ConstListIterator li_list(l);
   while (li_list.next())
   {
      QoreStringNodeValueHelper str(li_list.getValue());
      QString tmp;
      if (get_qstring(*str, tmp, xsink))
         return 0;
      list.push_back(tmp);
   }

   p = get_param(params, 4);
   int current = !is_nothing(p) ? p->getAsInt() : 0;

   p = get_param(params, 5);
   bool editable = !is_nothing(p) ? p->getAsBool() : true;

   const ReferenceNode *pr = test_reference_param(params, 6);

   p = get_param(params, 7);
   Qt::WindowFlags f = (Qt::WindowFlags)(!is_nothing(p) ? p->getAsInt() : 0);

   bool ok;
   QString rv = QInputDialog::getItem(static_cast<QWidget *>(parent->getQWidget()), title, label, list, current, editable, &ok, f);

   if (pr) {
      class AutoVLock vl;
      class AbstractQoreNode **vp = get_var_value_ptr(pr->getExpression(), &vl, xsink);
      if (*xsink)
	 return 0;

      if (*vp)
	 (*vp)->deref(xsink);
      (*vp) = get_bool_node(ok);
   }

   return new QoreStringNode(rv.toUtf8().data(), QCS_UTF8);
}

//QString getText ( QWidget * parent, const QString & title, const QString & label, QLineEdit::EchoMode mode = QLineEdit::Normal, const QString & text = QString(), bool * ok = 0, Qt::WindowFlags f = 0 )
static AbstractQoreNode *f_QInputDialog_getText(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QINPUTDIALOG-GETTEXT-PARAM-ERROR", "expecting a QWidget object as first argument to QInputDialog::getText()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);

   const AbstractQoreNode *p = get_param(params, 1);
   QString title;
   if (get_qstring(p, title, xsink))
      return 0;

   p = get_param(params, 2);
   QString label;
   if (get_qstring(p, label, xsink))
      return 0;

   p = get_param(params, 3);
   QLineEdit::EchoMode mode = !is_nothing(p) ? (QLineEdit::EchoMode)p->getAsInt() : QLineEdit::Normal;
   p = get_param(params, 4);
   QString text;
   if (get_qstring(p, text, xsink, true))
      text = QString();

   const ReferenceNode *pr = test_reference_param(params, 5);

   p = get_param(params, 6);
   Qt::WindowFlags f = (Qt::WindowFlags)(!is_nothing(p) ? p->getAsInt() : 0);

   bool ok;
   QString rv = QInputDialog::getText(static_cast<QWidget *>(parent->getQWidget()), title, label, mode, text, &ok, f);

   if (pr) {
      class AutoVLock vl;
      class AbstractQoreNode **vp = get_var_value_ptr(pr->getExpression(), &vl, xsink);
      if (*xsink)
	 return 0;

      if (*vp)
	 (*vp)->deref(xsink);
      (*vp) = get_bool_node(ok);
   }

   return new QoreStringNode(rv.toUtf8().data(), QCS_UTF8);
}

void initQInputDialogStaticFunctions()
{
   builtinFunctions.add("QInputDialog_getDouble",      f_QInputDialog_getDouble);
   builtinFunctions.add("QInputDialog_getInteger",     f_QInputDialog_getInteger);
   builtinFunctions.add("QInputDialog_getItem",        f_QInputDialog_getItem);
   builtinFunctions.add("QInputDialog_getText",        f_QInputDialog_getText);
}
