/*
 QC_QPalette.cc
 
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

#include "QC_QPalette.h"
#include "QC_QColor.h"
#include "QC_QBrush.h"

DLLLOCAL int CID_QPALETTE;

static void QPALETTE_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreQPalette *qp;

   QoreNode *p = get_param(params, 0);
   if (is_nothing(p))
      qp = new QoreQPalette();
   else if (p->type == NT_OBJECT)
   {
      QoreQColor *button = (QoreQColor *)p->val.object->getReferencedPrivateData(CID_QCOLOR, xsink);
      if (button) {
	 ReferenceHolder<QoreQColor> holder(button, xsink);
	 
	 p = get_param(params, 1);
	 QoreQColor *window = p && p->type == NT_OBJECT ? (QoreQColor *)p->val.object->getReferencedPrivateData(CID_QCOLOR, xsink) : 0;
	 if (!window)
	    qp = new QoreQPalette(*button);
	 else
	 {
	    ReferenceHolder<QoreQColor> holder1(window, xsink);
	    qp = new QoreQPalette(*button, *window);
	 }
      }
      else {
	 QoreQBrush *windowText = (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink);

	 if (!windowText) {
	    if (!*xsink)
	       xsink->raiseException("QPALETTE-CONSTRUCTOR-ERROR", "QPalette::constructor() does not take objects of class '%s' as an argument", p->val.object->getClass()->getName());
	    return;
	 }
	 ReferenceHolder<QoreQBrush> windowTextHolder(windowText, xsink);
	 
	 p = get_param(params, 1);
	 QoreQBrush *button = p && p->type == NT_OBJECT ? (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink) : 0;
	 if (!button) {
	    if (!*xsink)
	       xsink->raiseException("QPALETTE-CONSTRUCTOR-ERROR", "QPalette::constructor() expecting QBrush object as second parameter");
	    return;
	 }
	 ReferenceHolder<QoreQBrush> buttonHolder(button, xsink);

	 p = get_param(params, 2);
	 QoreQBrush *light = p && p->type == NT_OBJECT ? (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink) : 0;
	 if (!light) {
	    if (!*xsink)
	       xsink->raiseException("QPALETTE-CONSTRUCTOR-ERROR", "QPalette::constructor() expecting QBrush object as third parameter");
	    return;
	 }
	 ReferenceHolder<QoreQBrush> lightHolder(light, xsink);

	 p = get_param(params, 3);
	 QoreQBrush *dark = p && p->type == NT_OBJECT ? (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink) : 0;
	 if (!dark) {
	    if (!*xsink)
	       xsink->raiseException("QPALETTE-CONSTRUCTOR-ERROR", "QPalette::constructor() expecting QBrush object as fourth parameter");
	    return;
	 }
	 ReferenceHolder<QoreQBrush> darkHolder(dark, xsink);

	 p = get_param(params, 4);
	 QoreQBrush *mid = p && p->type == NT_OBJECT ? (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink) : 0;
	 if (!mid) {
	    if (!*xsink)
	       xsink->raiseException("QPALETTE-CONSTRUCTOR-ERROR", "QPalette::constructor() expecting QBrush object as fifth parameter");
	    return;
	 }
	 ReferenceHolder<QoreQBrush> midHolder(mid, xsink);

	 p = get_param(params, 5);
	 QoreQBrush *text = p && p->type == NT_OBJECT ? (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink) : 0;
	 if (!text) {
	    if (!*xsink)
	       xsink->raiseException("QPALETTE-CONSTRUCTOR-ERROR", "QPalette::constructor() expecting QBrush object as sixth parameter");
	    return;
	 }
	 ReferenceHolder<QoreQBrush> textHolder(text, xsink);

	 p = get_param(params, 6);
	 QoreQBrush *bright_text = p && p->type == NT_OBJECT ? (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink) : 0;
	 if (!bright_text) {
	    if (!*xsink)
	       xsink->raiseException("QPALETTE-CONSTRUCTOR-ERROR", "QPalette::constructor() expecting QBrush object as seventh parameter");
	    return;
	 }
	 ReferenceHolder<QoreQBrush> bright_textHolder(bright_text, xsink);

	 p = get_param(params, 7);
	 QoreQBrush *base = p && p->type == NT_OBJECT ? (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink) : 0;
	 if (!base) {
	    if (!*xsink)
	       xsink->raiseException("QPALETTE-CONSTRUCTOR-ERROR", "QPalette::constructor() expecting QBrush object as eighth parameter");
	    return;
	 }
	 ReferenceHolder<QoreQBrush> baseHolder(base, xsink);

	 p = get_param(params, 8);
	 QoreQBrush *window = p && p->type == NT_OBJECT ? (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink) : 0;
	 if (!window) {
	    if (!*xsink)
	       xsink->raiseException("QPALETTE-CONSTRUCTOR-ERROR", "QPalette::constructor() expecting QBrush object as ninth parameter");
	    return;
	 }
	 ReferenceHolder<QoreQBrush> windowHolder(window, xsink);

	 qp = new QoreQPalette(*windowText, *button, *light, *dark, *mid, *text, *bright_text, *base, *window);
      }
   }
   else
   {
      Qt::GlobalColor button = (Qt::GlobalColor)p->getAsInt();
      qp = new QoreQPalette(button);
   }

   self->setPrivate(CID_QPALETTE, qp);
}

static void QPALETTE_copy(class Object *self, class Object *old, class QoreQPalette *qf, ExceptionSink *xsink)
{
   xsink->raiseException("QPALETTE-COPY-ERROR", "objects of this class cannot be copied");
}

class QoreClass *initQPaletteClass()
{
   tracein("initQPaletteClass()");
   
   class QoreClass *QC_QPalette = new QoreClass("QPalette", QDOM_GUI);
   CID_QPALETTE = QC_QPalette->getID();
   QC_QPalette->setConstructor(QPALETTE_constructor);
   QC_QPalette->setCopy((q_copy_t)QPALETTE_copy);


   traceout("initQPaletteClass()");
   return QC_QPalette;
}
