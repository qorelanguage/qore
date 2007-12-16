/*
 QC_QColorDialog.cc
 
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

#include "QC_QColorDialog.h"

//QRgb customColor ( int i )
static QoreNode *f_QColorDialog_customColor(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int i = p ? p->getAsInt() : 0;
   return new QoreNode((int64)QColorDialog::customColor(i));
}

//int customCount ()
static QoreNode *f_QColorDialog_customCount(const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)QColorDialog::customCount());
}

//QColor getColor ( const QColor & initial = Qt::white, QWidget * parent = 0 )
static QoreNode *f_QColorDialog_getColor(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQColor *initial = (p && p->type == NT_OBJECT) ? (QoreQColor *)p->val.object->getReferencedPrivateData(CID_QCOLOR, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> initialHolder(static_cast<AbstractPrivateData *>(initial), xsink);
   p = get_param(params, 1);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   QoreObject *o_qc = new QoreObject(QC_QColor, getProgram());
   QoreQColor *q_qc = new QoreQColor(QColorDialog::getColor(initial ? *(static_cast<QColor *>(initial)) : Qt::white, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   o_qc->setPrivate(CID_QCOLOR, q_qc);
   return new QoreNode(o_qc);
}

/*
//QRgb getRgba ( QRgb initial = 0xffffffff, bool * ok = 0, QWidget * parent = 0 )
static QoreNode *f_QColorDialog_getRgba(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int64 initial = !is_nothing(p) ? p->getAsBigInt() : 0xffffffff;
   p = get_param(params, 1);
   ??? bool* ok = p;
   p = get_param(params, 2);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   return new QoreNode((int64)QColorDialog::getRgba(initial, ok, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
}
*/

//void setCustomColor ( int number, QRgb color )
static QoreNode *f_QColorDialog_setCustomColor(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int number = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int64 color = p ? p->getAsBigInt() : 0;
   QColorDialog::setCustomColor(number, color);
   return 0;
}

//void setStandardColor ( int number, QRgb color )
static QoreNode *f_QColorDialog_setStandardColor(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int number = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int64 color = p ? p->getAsBigInt() : 0;
   QColorDialog::setStandardColor(number, color);
   return 0;
}

void initQColorDialogStaticFunctions()
{
   builtinFunctions.add("QColorDialog_customColor",                  f_QColorDialog_customColor);
   builtinFunctions.add("QColorDialog_customCount",                  f_QColorDialog_customCount);
   builtinFunctions.add("QColorDialog_getColor",                     f_QColorDialog_getColor);
   //builtinFunctions.add("QColorDialog_getRgba",                      f_QColorDialog_getRgba);
   builtinFunctions.add("QColorDialog_setCustomColor",               f_QColorDialog_setCustomColor);
   builtinFunctions.add("QColorDialog_setStandardColor",             f_QColorDialog_setStandardColor);
}
