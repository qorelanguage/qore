/*
 QC_QFontDialog.cc
 
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

#include "QC_QFontDialog.h"
#include "QC_QWidget.h"
#include "QC_QFont.h"

//QFont getFont ( bool * ok, const QFont & initial, QWidget * parent, const QString & caption )
//QFont getFont ( bool * ok, const QFont & initial, QWidget * parent = 0 )
//QFont getFont ( bool * ok, QWidget * parent = 0 )
static AbstractQoreNode *f_QFontDialog_getFont(const QoreListNode *params, ExceptionSink *xsink)
{
   const ReferenceNode *r = test_reference_param(params, 0);
   if (!r) {
      const AbstractQoreNode *p = get_param(params, 0);
      xsink->raiseException("QFONTDIALOG-ERROR", "first argument to QFontDialot_getFont() must be an lvalue reference (got: '%s')", p ? p->getTypeName() : "NOTHING");
      return 0;
   }

   bool ok;
   QFont f;
   
   if (num_params(params) == 1 || test_nothing_param(params, 1)) {
      f = QFontDialog::getFont(&ok);
   }
   else {
      const QoreObject *o = test_object_param(params, 1);
      if (!o) {
	 const AbstractQoreNode *p = get_param(params, 1);
	 xsink->raiseException("QFONTDIALOG-ERROR", "if present, second argument to QFontDialot_getFont() must be an object (got: '%s')", p ? p->getTypeName() : "NOTHING");
	 return 0;
      }

      QoreQFont *font = (QoreQFont *)o->getReferencedPrivateData(CID_QFONT, xsink);
      if (!font) {
	 QoreQWidget *parent = (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink);
	 if (!parent) {
            if (!xsink->isException())
               xsink->raiseException("QFONTDIALOG-GETFONT-PARAM-ERROR", "this version of QFontDialog_getFont() expects an object derived from either QFont or QWidget as the second argument");
            return 0;
	 }

	 ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
	 
	 f = QFontDialog::getFont(&ok, parent->getQWidget());
      }
      else {
	 ReferenceHolder<AbstractPrivateData> fontHolder(static_cast<AbstractPrivateData *>(font), xsink);
	 
	 if (num_params(params) == 2 || test_nothing_param(params, 2)) {
	    f = QFontDialog::getFont(&ok, *font);
	 }
	 else {
	    o = test_object_param(params, 2);
	    if (!o) {
	       const AbstractQoreNode *p = get_param(params, 2);
	       xsink->raiseException("QFONTDIALOG-ERROR", "this version of QFontDialot_getFont() expects an object as the third argument (got: '%s')", p ? p->getTypeName() : "NOTHING");
	       return 0;
	    }
	    
	    QoreQWidget *parent = (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink);
	    if (!parent) {
	       if (!xsink->isException())
		  xsink->raiseException("QFONTDIALOG-GETFONT-PARAM-ERROR", "this version of QFontDialog_getFont() expects an object derived from QWidget as the third argument");
	       return 0;
	    }
	    
	    ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
	    
	    if (num_params(params) == 3 || test_nothing_param(params, 3))
	       f = QFontDialog::getFont(&ok, *font, parent->getQWidget());
	    else {
	       QString caption;
	       const AbstractQoreNode *p = get_param(params, 3);
	       if (get_qstring(p, caption, xsink))
		  return 0;
	       
	       f = QFontDialog::getFont(&ok, *font, parent->getQWidget(), caption);
	    }
	 }
      }
   }

   // write back ok value
   AutoVLock vl(xsink);
   ReferenceHelper ref(r, vl, xsink);
   if (*xsink)
      return 0;

   ref.assign(get_bool_node(ok), xsink);
   if (*xsink)
      return 0;

   return return_object(QC_QFont, new QoreQFont(f));
}

void initQFontDialogStaticFunctions()
{
   builtinFunctions.add("QFontDialog_getFont",  f_QFontDialog_getFont, QDOM_GUI);
}
