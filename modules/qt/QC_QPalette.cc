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
DLLLOCAL QoreClass *QC_QPalette = 0;

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

//const QBrush & alternateBase () const
static QoreNode *QPALETTE_alternateBase(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qb = new Object(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qp->getQPalette()->alternateBase());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

//const QBrush & background () const
static QoreNode *QPALETTE_background(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qb = new Object(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qp->getQPalette()->background());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

//const QBrush & base () const
static QoreNode *QPALETTE_base(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qb = new Object(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qp->getQPalette()->base());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

//const QBrush & brightText () const
static QoreNode *QPALETTE_brightText(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qb = new Object(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qp->getQPalette()->brightText());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

//const QBrush & brush ( ColorGroup group, ColorRole role ) const
//const QBrush & brush ( ColorRole role ) const
static QoreNode *QPALETTE_brush(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQBrush *q_qb;
   if (num_params(params) == 1) {
      QPalette::ColorRole role = (QPalette::ColorRole)(p ? p->getAsInt() : 0);
      q_qb = new QoreQBrush(qp->getQPalette()->brush(role));
   }
   else {
      QPalette::ColorGroup group = (QPalette::ColorGroup)(p ? p->getAsInt() : 0);
      p = get_param(params, 1);
      QPalette::ColorRole role = (QPalette::ColorRole)(p ? p->getAsInt() : 0);
      q_qb = new QoreQBrush(qp->getQPalette()->brush(group, role));
   }
   Object *o_qb = new Object(QC_QBrush, getProgram());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

//const QBrush & button () const
static QoreNode *QPALETTE_button(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qb = new Object(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qp->getQPalette()->button());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

//const QBrush & buttonText () const
static QoreNode *QPALETTE_buttonText(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qb = new Object(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qp->getQPalette()->buttonText());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

//qint64 cacheKey () const
static QoreNode *QPALETTE_cacheKey(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qp->getQPalette()->cacheKey());
}

//const QColor & color ( ColorGroup group, ColorRole role ) const
//const QColor & color ( ColorRole role ) const
static QoreNode *QPALETTE_color(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QPalette::ColorGroup group = (QPalette::ColorGroup)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QPalette::ColorRole role = (QPalette::ColorRole)(p ? p->getAsInt() : 0);
   Object *o_qc = new Object(QC_QColor, getProgram());
   QoreQColor *q_qc = new QoreQColor(qp->getQPalette()->color(group, role));
   o_qc->setPrivate(CID_QCOLOR, q_qc);
   return new QoreNode(o_qc);
}

//ColorGroup currentColorGroup () const
static QoreNode *QPALETTE_currentColorGroup(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qp->getQPalette()->currentColorGroup());
}

//const QBrush & dark () const
static QoreNode *QPALETTE_dark(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qb = new Object(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qp->getQPalette()->dark());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

//const QBrush & foreground () const
static QoreNode *QPALETTE_foreground(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qb = new Object(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qp->getQPalette()->foreground());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

//const QBrush & highlight () const
static QoreNode *QPALETTE_highlight(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qb = new Object(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qp->getQPalette()->highlight());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

//const QBrush & highlightedText () const
static QoreNode *QPALETTE_highlightedText(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qb = new Object(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qp->getQPalette()->highlightedText());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

//bool isBrushSet ( ColorGroup cg, ColorRole cr ) const
static QoreNode *QPALETTE_isBrushSet(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QPalette::ColorGroup cg = (QPalette::ColorGroup)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QPalette::ColorRole cr = (QPalette::ColorRole)(p ? p->getAsInt() : 0);
   return new QoreNode(qp->getQPalette()->isBrushSet(cg, cr));
}

//bool isCopyOf ( const QPalette & p ) const
static QoreNode *QPALETTE_isCopyOf(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPalette *palette = (p && p->type == NT_OBJECT) ? (QoreQPalette *)p->val.object->getReferencedPrivateData(CID_QPALETTE, xsink) : 0;
   if (!palette) {
      if (!xsink->isException())
         xsink->raiseException("QPALETTE-ISCOPYOF-PARAM-ERROR", "expecting a QPalette object as first argument to QPalette::isCopyOf()");
      return 0;
   }
   ReferenceHolder<QoreQPalette> pHolder(palette, xsink);
   return new QoreNode(qp->getQPalette()->isCopyOf(*(palette->getQPalette())));
}

//bool isEqual ( ColorGroup cg1, ColorGroup cg2 ) const
static QoreNode *QPALETTE_isEqual(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QPalette::ColorGroup cg1 = (QPalette::ColorGroup)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QPalette::ColorGroup cg2 = (QPalette::ColorGroup)(p ? p->getAsInt() : 0);
   return new QoreNode(qp->getQPalette()->isEqual(cg1, cg2));
}

//const QBrush & light () const
static QoreNode *QPALETTE_light(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qb = new Object(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qp->getQPalette()->light());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

//const QBrush & link () const
static QoreNode *QPALETTE_link(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qb = new Object(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qp->getQPalette()->link());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

//const QBrush & linkVisited () const
static QoreNode *QPALETTE_linkVisited(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qb = new Object(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qp->getQPalette()->linkVisited());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

//const QBrush & mid () const
static QoreNode *QPALETTE_mid(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qb = new Object(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qp->getQPalette()->mid());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

//const QBrush & midlight () const
static QoreNode *QPALETTE_midlight(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qb = new Object(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qp->getQPalette()->midlight());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

//QPalette resolve ( const QPalette & other ) const
static QoreNode *QPALETTE_resolve(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPalette *other = (p && p->type == NT_OBJECT) ? (QoreQPalette *)p->val.object->getReferencedPrivateData(CID_QPALETTE, xsink) : 0;
   if (!other) {
      if (!xsink->isException())
         xsink->raiseException("QPALETTE-RESOLVE-PARAM-ERROR", "expecting a QPalette object as first argument to QPalette::resolve()");
      return 0;
   }
   ReferenceHolder<QoreQPalette> otherHolder(other, xsink);
   Object *o_qp = new Object(self->getClass(CID_QPALETTE), getProgram());
   QoreQPalette *q_qp = new QoreQPalette(qp->getQPalette()->resolve(*(other->getQPalette())));
   o_qp->setPrivate(CID_QPALETTE, q_qp);
   return new QoreNode(o_qp);
}


//void set ( const QPalette & other )
static QoreNode *QPALETTE_set(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPalette *other = (p && p->type == NT_OBJECT) ? (QoreQPalette *)p->val.object->getReferencedPrivateData(CID_QPALETTE, xsink) : 0;
   if (!other) {
      if (!xsink->isException())
         xsink->raiseException("QPALETTE-SET-PARAM-ERROR", "expecting a QPalette object as first argument to QPalette::set()");
      return 0;
   }
   ReferenceHolder<QoreQPalette> otherHolder(other, xsink);
   
   *(qp->getQPalette()) = *(other->getQPalette());
   return 0;
/*
   Object *o_qp = new Object(self->getClass(CID_QPALETTE), getProgram());
   QoreQPalette *q_qp = new QoreQPalette(qp->getQPalette()->resolve(*(other->getQPalette())));
   o_qp->setPrivate(CID_QPALETTE, q_qp);
   return new QoreNode(o_qp);
*/
}

//void setBrush ( ColorRole role, const QBrush & brush )
//void setBrush ( ColorGroup group, ColorRole role, const QBrush & brush )
static QoreNode *QPALETTE_setBrush(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int a1 = p ? p->getAsInt() : 0;

   bool got_role = false;
   QPalette::ColorRole role = QPalette::NoRole;
   p = get_param(params, 1);
   if (num_params(params) > 2) {
      role = p ? (QPalette::ColorRole)p->getAsInt() : QPalette::NoRole;
      got_role = true;
      p = get_param(params, 2);
   }

   QBrush brush;
   if (get_qbrush(p, brush, xsink))
      return 0;

   if (got_role)
      qp->getQPalette()->setBrush((QPalette::ColorGroup)a1, role, brush);
   else
      qp->getQPalette()->setBrush((QPalette::ColorRole)a1, brush);
   return 0;
}

//void setColor ( ColorGroup group, ColorRole role, const QColor & color )
//void setColor ( ColorRole role, const QColor & color )
static QoreNode *QPALETTE_setColor(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int a1 = p ? p->getAsInt() : 0;

   p = get_param(params, 1);
   int a2;
   bool got_a2 = false;
   if (!p || p->type != NT_OBJECT) {
      a2 = p ? p->getAsInt() : 0;
      got_a2 = true;
      p = get_param(params, 2);
   }

   QoreQColor *color = (p && p->type == NT_OBJECT) ? (QoreQColor *)p->val.object->getReferencedPrivateData(CID_QCOLOR, xsink) : 0;
   if (!color) {
      if (!xsink->isException())
         xsink->raiseException("QPALETTE-SETCOLOR-PARAM-ERROR", "this version of QPalette::setColor() expects an object derived from QColor as the final argument", p->val.object->getClass()->getName());
      return 0;
   }
   ReferenceHolder<QoreQColor> colorHolder(color, xsink);

   if (got_a2)
      qp->getQPalette()->setColor((QPalette::ColorGroup)a1, (QPalette::ColorRole)a2, *(static_cast<QColor *>(color)));
   else
      qp->getQPalette()->setColor((QPalette::ColorRole)a1, *(static_cast<QColor *>(color)));
   return 0;
}

//void setColorGroup ( ColorGroup cg, const QBrush & windowText, const QBrush & button, const QBrush & light, const QBrush & dark, const QBrush & mid, const QBrush & text, const QBrush & bright_text, const QBrush & base, const QBrush & window )
static QoreNode *QPALETTE_setColorGroup(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QPalette::ColorGroup cg = (QPalette::ColorGroup)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQBrush *windowText = (p && p->type == NT_OBJECT) ? (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink) : 0;
   if (!windowText) {
      if (!xsink->isException())
         xsink->raiseException("QPALETTE-SETCOLORGROUP-PARAM-ERROR", "expecting a QBrush object as second argument to QPalette::setColorGroup()");
      return 0;
   }
   ReferenceHolder<QoreQBrush> windowTextHolder(windowText, xsink);
   p = get_param(params, 2);
   QoreQBrush *button = (p && p->type == NT_OBJECT) ? (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink) : 0;
   if (!button) {
      if (!xsink->isException())
         xsink->raiseException("QPALETTE-SETCOLORGROUP-PARAM-ERROR", "expecting a QBrush object as third argument to QPalette::setColorGroup()");
      return 0;
   }
   ReferenceHolder<QoreQBrush> buttonHolder(button, xsink);
   p = get_param(params, 3);
   QoreQBrush *light = (p && p->type == NT_OBJECT) ? (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink) : 0;
   if (!light) {
      if (!xsink->isException())
         xsink->raiseException("QPALETTE-SETCOLORGROUP-PARAM-ERROR", "expecting a QBrush object as fourth argument to QPalette::setColorGroup()");
      return 0;
   }
   ReferenceHolder<QoreQBrush> lightHolder(light, xsink);
   p = get_param(params, 4);
   QoreQBrush *dark = (p && p->type == NT_OBJECT) ? (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink) : 0;
   if (!dark) {
      if (!xsink->isException())
         xsink->raiseException("QPALETTE-SETCOLORGROUP-PARAM-ERROR", "expecting a QBrush object as fifth argument to QPalette::setColorGroup()");
      return 0;
   }
   ReferenceHolder<QoreQBrush> darkHolder(dark, xsink);
   p = get_param(params, 5);
   QoreQBrush *mid = (p && p->type == NT_OBJECT) ? (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink) : 0;
   if (!mid) {
      if (!xsink->isException())
         xsink->raiseException("QPALETTE-SETCOLORGROUP-PARAM-ERROR", "expecting a QBrush object as sixth argument to QPalette::setColorGroup()");
      return 0;
   }
   ReferenceHolder<QoreQBrush> midHolder(mid, xsink);
   p = get_param(params, 6);
   QoreQBrush *text = (p && p->type == NT_OBJECT) ? (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink) : 0;
   if (!text) {
      if (!xsink->isException())
         xsink->raiseException("QPALETTE-SETCOLORGROUP-PARAM-ERROR", "expecting a QBrush object as seventh argument to QPalette::setColorGroup()");
      return 0;
   }
   ReferenceHolder<QoreQBrush> textHolder(text, xsink);
   p = get_param(params, 7);
   QoreQBrush *bright_text = (p && p->type == NT_OBJECT) ? (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink) : 0;
   if (!bright_text) {
      if (!xsink->isException())
         xsink->raiseException("QPALETTE-SETCOLORGROUP-PARAM-ERROR", "expecting a QBrush object as eighth argument to QPalette::setColorGroup()");
      return 0;
   }
   ReferenceHolder<QoreQBrush> bright_textHolder(bright_text, xsink);
   p = get_param(params, 8);
   QoreQBrush *base = (p && p->type == NT_OBJECT) ? (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink) : 0;
   if (!base) {
      if (!xsink->isException())
         xsink->raiseException("QPALETTE-SETCOLORGROUP-PARAM-ERROR", "expecting a QBrush object as ninth argument to QPalette::setColorGroup()");
      return 0;
   }
   ReferenceHolder<QoreQBrush> baseHolder(base, xsink);
   p = get_param(params, 9);
   QoreQBrush *window = (p && p->type == NT_OBJECT) ? (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink) : 0;
   if (!window) {
      if (!xsink->isException())
         xsink->raiseException("QPALETTE-SETCOLORGROUP-PARAM-ERROR", "expecting a QBrush object as tenth argument to QPalette::setColorGroup()");
      return 0;
   }
   ReferenceHolder<QoreQBrush> windowHolder(window, xsink);
   qp->getQPalette()->setColorGroup(cg, *(static_cast<QBrush *>(windowText)), *(static_cast<QBrush *>(button)), *(static_cast<QBrush *>(light)), *(static_cast<QBrush *>(dark)), *(static_cast<QBrush *>(mid)), *(static_cast<QBrush *>(text)), *(static_cast<QBrush *>(bright_text)), *(static_cast<QBrush *>(base)), *(static_cast<QBrush *>(window)));

   return 0;
}

//void setCurrentColorGroup ( ColorGroup cg )
static QoreNode *QPALETTE_setCurrentColorGroup(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QPalette::ColorGroup cg = (QPalette::ColorGroup)(p ? p->getAsInt() : 0);
   qp->getQPalette()->setCurrentColorGroup(cg);
   return 0;
}

//const QBrush & shadow () const
static QoreNode *QPALETTE_shadow(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qb = new Object(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qp->getQPalette()->shadow());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

//const QBrush & text () const
static QoreNode *QPALETTE_text(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qb = new Object(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qp->getQPalette()->text());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

//const QBrush & window () const
static QoreNode *QPALETTE_window(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qb = new Object(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qp->getQPalette()->window());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

//const QBrush & windowText () const
static QoreNode *QPALETTE_windowText(Object *self, QoreQPalette *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qb = new Object(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qp->getQPalette()->windowText());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

static class QoreClass *initQPaletteClass()
{
   tracein("initQPaletteClass()");
   
   QC_QPalette = new QoreClass("QPalette", QDOM_GUI);
   CID_QPALETTE = QC_QPalette->getID();
   QC_QPalette->setConstructor(QPALETTE_constructor);
   QC_QPalette->setCopy((q_copy_t)QPALETTE_copy);

   QC_QPalette->addMethod("alternateBase",               (q_method_t)QPALETTE_alternateBase);
   QC_QPalette->addMethod("background",                  (q_method_t)QPALETTE_background);
   QC_QPalette->addMethod("base",                        (q_method_t)QPALETTE_base);
   QC_QPalette->addMethod("brightText",                  (q_method_t)QPALETTE_brightText);
   QC_QPalette->addMethod("brush",                       (q_method_t)QPALETTE_brush);
   QC_QPalette->addMethod("button",                      (q_method_t)QPALETTE_button);
   QC_QPalette->addMethod("buttonText",                  (q_method_t)QPALETTE_buttonText);
   QC_QPalette->addMethod("cacheKey",                    (q_method_t)QPALETTE_cacheKey);
   QC_QPalette->addMethod("color",                       (q_method_t)QPALETTE_color);
   QC_QPalette->addMethod("currentColorGroup",           (q_method_t)QPALETTE_currentColorGroup);
   QC_QPalette->addMethod("dark",                        (q_method_t)QPALETTE_dark);
   QC_QPalette->addMethod("foreground",                  (q_method_t)QPALETTE_foreground);
   QC_QPalette->addMethod("highlight",                   (q_method_t)QPALETTE_highlight);
   QC_QPalette->addMethod("highlightedText",             (q_method_t)QPALETTE_highlightedText);
   QC_QPalette->addMethod("isBrushSet",                  (q_method_t)QPALETTE_isBrushSet);
   QC_QPalette->addMethod("isCopyOf",                    (q_method_t)QPALETTE_isCopyOf);
   QC_QPalette->addMethod("isEqual",                     (q_method_t)QPALETTE_isEqual);
   QC_QPalette->addMethod("light",                       (q_method_t)QPALETTE_light);
   QC_QPalette->addMethod("link",                        (q_method_t)QPALETTE_link);
   QC_QPalette->addMethod("linkVisited",                 (q_method_t)QPALETTE_linkVisited);
   QC_QPalette->addMethod("mid",                         (q_method_t)QPALETTE_mid);
   QC_QPalette->addMethod("midlight",                    (q_method_t)QPALETTE_midlight);
   QC_QPalette->addMethod("resolve",                     (q_method_t)QPALETTE_resolve);
   QC_QPalette->addMethod("set",                         (q_method_t)QPALETTE_set);
   QC_QPalette->addMethod("setBrush",                    (q_method_t)QPALETTE_setBrush);
   QC_QPalette->addMethod("setColor",                    (q_method_t)QPALETTE_setColor);
   QC_QPalette->addMethod("setColorGroup",               (q_method_t)QPALETTE_setColorGroup);
   QC_QPalette->addMethod("setCurrentColorGroup",        (q_method_t)QPALETTE_setCurrentColorGroup);
   QC_QPalette->addMethod("shadow",                      (q_method_t)QPALETTE_shadow);
   QC_QPalette->addMethod("text",                        (q_method_t)QPALETTE_text);
   QC_QPalette->addMethod("window",                      (q_method_t)QPALETTE_window);
   QC_QPalette->addMethod("windowText",                  (q_method_t)QPALETTE_windowText);

   traceout("initQPaletteClass()");
   return QC_QPalette;
}

Namespace *initQPaletteNS()
{   
   Namespace *qpalette = new Namespace("QPalette");

   qpalette->addSystemClass(initQPaletteClass());

   // ColorGroup enum
   qpalette->addConstant("Active",                   new QoreNode((int64)QPalette::Active));
   qpalette->addConstant("Disabled",                 new QoreNode((int64)QPalette::Disabled));
   qpalette->addConstant("Inactive",                 new QoreNode((int64)QPalette::Inactive));
   qpalette->addConstant("NColorGroups",             new QoreNode((int64)QPalette::NColorGroups));
   qpalette->addConstant("Current",                  new QoreNode((int64)QPalette::Current));
   qpalette->addConstant("All",                      new QoreNode((int64)QPalette::All));
   qpalette->addConstant("Normal",                   new QoreNode((int64)QPalette::Normal));

   // ColorRole enum
   qpalette->addConstant("WindowText",               new QoreNode((int64)QPalette::WindowText));
   qpalette->addConstant("Button",                   new QoreNode((int64)QPalette::Button));
   qpalette->addConstant("Light",                    new QoreNode((int64)QPalette::Light));
   qpalette->addConstant("Midlight",                 new QoreNode((int64)QPalette::Midlight));
   qpalette->addConstant("Dark",                     new QoreNode((int64)QPalette::Dark));
   qpalette->addConstant("Mid",                      new QoreNode((int64)QPalette::Mid));
   qpalette->addConstant("Text",                     new QoreNode((int64)QPalette::Text));
   qpalette->addConstant("BrightText",               new QoreNode((int64)QPalette::BrightText));
   qpalette->addConstant("ButtonText",               new QoreNode((int64)QPalette::ButtonText));
   qpalette->addConstant("Base",                     new QoreNode((int64)QPalette::Base));
   qpalette->addConstant("Window",                   new QoreNode((int64)QPalette::Window));
   qpalette->addConstant("Shadow",                   new QoreNode((int64)QPalette::Shadow));
   qpalette->addConstant("Highlight",                new QoreNode((int64)QPalette::Highlight));
   qpalette->addConstant("HighlightedText",          new QoreNode((int64)QPalette::HighlightedText));
   qpalette->addConstant("Link",                     new QoreNode((int64)QPalette::Link));
   qpalette->addConstant("LinkVisited",              new QoreNode((int64)QPalette::LinkVisited));
   qpalette->addConstant("AlternateBase",            new QoreNode((int64)QPalette::AlternateBase));
   qpalette->addConstant("NoRole",                   new QoreNode((int64)QPalette::NoRole));
   qpalette->addConstant("NColorRoles",              new QoreNode((int64)QPalette::NColorRoles));
   qpalette->addConstant("Foreground",               new QoreNode((int64)QPalette::Foreground));
   qpalette->addConstant("Background",               new QoreNode((int64)QPalette::Background));

   return qpalette;
}
