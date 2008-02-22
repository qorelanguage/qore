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

#include "qore-qt.h"

DLLLOCAL int CID_QPALETTE;
DLLLOCAL QoreClass *QC_QPalette = 0;

static void QPALETTE_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreQPalette *qp;

   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p))
      qp = new QoreQPalette();
   else if (p->getType() == NT_OBJECT)
   {
      QoreQColor *button = (QoreQColor *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QCOLOR, xsink);
      if (button) {
	 ReferenceHolder<QoreQColor> holder(button, xsink);
	 
	 p = get_param(params, 1);
	 QoreQColor *window = p && p->getType() == NT_OBJECT ? (QoreQColor *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QCOLOR, xsink) : 0;
	 if (!window)
	    qp = new QoreQPalette(*button);
	 else
	 {
	    ReferenceHolder<QoreQColor> holder1(window, xsink);
	    qp = new QoreQPalette(*button, *window);
	 }
      }
      else {
	 QBrush windowText;
	 if (get_qbrush(p, windowText, xsink))
	    return;
	 
	 p = get_param(params, 1);
	 QBrush button;
	 if (get_qbrush(p, button, xsink))
	    return;

	 p = get_param(params, 2);
	 QBrush light;
	 if (get_qbrush(p, light, xsink))
	    return;

	 p = get_param(params, 3);
	 QBrush dark;
	 if (get_qbrush(p, dark, xsink))
	    return;

	 p = get_param(params, 4);
	 QBrush mid;
	 if (get_qbrush(p, mid, xsink))
	    return;

	 p = get_param(params, 5);
	 QBrush text;
	 if (get_qbrush(p, text, xsink))
	    return;

	 p = get_param(params, 6);
	 QBrush bright_text;
	 if (get_qbrush(p, bright_text, xsink))
	    return;

	 p = get_param(params, 7);
	 QBrush base;
	 if (get_qbrush(p, base, xsink))
	    return;

	 p = get_param(params, 8);
	 QBrush window;
	 if (get_qbrush(p, window, xsink))
	    return;

	 qp = new QoreQPalette(windowText, button, light, dark, mid, text, bright_text, base, window);
      }
   }
   else
   {
      Qt::GlobalColor button = (Qt::GlobalColor)p->getAsInt();
      qp = new QoreQPalette(button);
   }

   self->setPrivate(CID_QPALETTE, qp);
}

static void QPALETTE_copy(class QoreObject *self, class QoreObject *old, class QoreQPalette *qf, ExceptionSink *xsink)
{
   self->setPrivate(CID_QPALETTE, new QoreQPalette(*qf->getQPalette()));
   //xsink->raiseException("QPALETTE-COPY-ERROR", "objects of this class cannot be copied");
}

//const QBrush & alternateBase () const
static AbstractQoreNode *QPALETTE_alternateBase(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(&qp->getQPalette()->alternateBase());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//const QBrush & background () const
static AbstractQoreNode *QPALETTE_background(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(&qp->getQPalette()->background());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//const QBrush & base () const
static AbstractQoreNode *QPALETTE_base(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(&qp->getQPalette()->base());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//const QBrush & brightText () const
static AbstractQoreNode *QPALETTE_brightText(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(&qp->getQPalette()->brightText());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//const QBrush & brush ( ColorGroup group, ColorRole role ) const
//const QBrush & brush ( ColorRole role ) const
static AbstractQoreNode *QPALETTE_brush(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQBrush *q_qb;
   if (num_params(params) == 1) {
      QPalette::ColorRole role = (QPalette::ColorRole)(p ? p->getAsInt() : 0);
      q_qb = new QoreQBrush(&qp->getQPalette()->brush(role));
   }
   else {
      QPalette::ColorGroup group = (QPalette::ColorGroup)(p ? p->getAsInt() : 0);
      p = get_param(params, 1);
      QPalette::ColorRole role = (QPalette::ColorRole)(p ? p->getAsInt() : 0);
      q_qb = new QoreQBrush(&qp->getQPalette()->brush(group, role));
   }
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//const QBrush & button () const
static AbstractQoreNode *QPALETTE_button(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(&qp->getQPalette()->button());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//const QBrush & buttonText () const
static AbstractQoreNode *QPALETTE_buttonText(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(&qp->getQPalette()->buttonText());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//qint64 cacheKey () const
static AbstractQoreNode *QPALETTE_cacheKey(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qp->getQPalette()->cacheKey());
}

//const QColor & color ( ColorGroup group, ColorRole role ) const
//const QColor & color ( ColorRole role ) const
static AbstractQoreNode *QPALETTE_color(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQColor *q_qc;
   if (num_params(params) == 1) {
      QPalette::ColorRole role = (QPalette::ColorRole)(p ? p->getAsInt() : 0);
      q_qc = new QoreQColor(qp->getQPalette()->color(role));
   }
   else {
      QPalette::ColorGroup group = (QPalette::ColorGroup)(p ? p->getAsInt() : 0);
      p = get_param(params, 1);
      QPalette::ColorRole role = (QPalette::ColorRole)(p ? p->getAsInt() : 0);
      q_qc = new QoreQColor(qp->getQPalette()->color(group, role));
   }
   QoreObject *o_qc = new QoreObject(QC_QColor, getProgram());
   o_qc->setPrivate(CID_QCOLOR, q_qc);
   return o_qc;
}

//ColorGroup currentColorGroup () const
static AbstractQoreNode *QPALETTE_currentColorGroup(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qp->getQPalette()->currentColorGroup());
}

//const QBrush & dark () const
static AbstractQoreNode *QPALETTE_dark(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(&qp->getQPalette()->dark());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//const QBrush & foreground () const
static AbstractQoreNode *QPALETTE_foreground(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(&qp->getQPalette()->foreground());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//const QBrush & highlight () const
static AbstractQoreNode *QPALETTE_highlight(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(&qp->getQPalette()->highlight());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//const QBrush & highlightedText () const
static AbstractQoreNode *QPALETTE_highlightedText(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(&qp->getQPalette()->highlightedText());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//bool isBrushSet ( ColorGroup cg, ColorRole cr ) const
static AbstractQoreNode *QPALETTE_isBrushSet(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QPalette::ColorGroup cg = (QPalette::ColorGroup)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QPalette::ColorRole cr = (QPalette::ColorRole)(p ? p->getAsInt() : 0);
   return new QoreBoolNode(qp->getQPalette()->isBrushSet(cg, cr));
}

//bool isCopyOf ( const QPalette & p ) const
static AbstractQoreNode *QPALETTE_isCopyOf(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQPalette *palette = p ? (QoreQPalette *)p->getReferencedPrivateData(CID_QPALETTE, xsink) : 0;
   if (!palette) {
      if (!xsink->isException())
         xsink->raiseException("QPALETTE-ISCOPYOF-PARAM-ERROR", "expecting a QPalette object as first argument to QPalette::isCopyOf()");
      return 0;
   }
   ReferenceHolder<QoreQPalette> pHolder(palette, xsink);
   return new QoreBoolNode(qp->getQPalette()->isCopyOf(*(palette->getQPalette())));
}

//bool isEqual ( ColorGroup cg1, ColorGroup cg2 ) const
static AbstractQoreNode *QPALETTE_isEqual(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QPalette::ColorGroup cg1 = (QPalette::ColorGroup)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QPalette::ColorGroup cg2 = (QPalette::ColorGroup)(p ? p->getAsInt() : 0);
   return new QoreBoolNode(qp->getQPalette()->isEqual(cg1, cg2));
}

//const QBrush & light () const
static AbstractQoreNode *QPALETTE_light(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(&qp->getQPalette()->light());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//const QBrush & link () const
static AbstractQoreNode *QPALETTE_link(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(&qp->getQPalette()->link());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//const QBrush & linkVisited () const
static AbstractQoreNode *QPALETTE_linkVisited(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(&qp->getQPalette()->linkVisited());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//const QBrush & mid () const
static AbstractQoreNode *QPALETTE_mid(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(&qp->getQPalette()->mid());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//const QBrush & midlight () const
static AbstractQoreNode *QPALETTE_midlight(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(&qp->getQPalette()->midlight());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//QPalette resolve ( const QPalette & other ) const
static AbstractQoreNode *QPALETTE_resolve(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQPalette *other = p ? (QoreQPalette *)p->getReferencedPrivateData(CID_QPALETTE, xsink) : 0;
   if (!other) {
      if (!xsink->isException())
         xsink->raiseException("QPALETTE-RESOLVE-PARAM-ERROR", "expecting a QPalette object as first argument to QPalette::resolve()");
      return 0;
   }
   ReferenceHolder<QoreQPalette> otherHolder(other, xsink);
   QoreObject *o_qp = new QoreObject(self->getClass(CID_QPALETTE), getProgram());
   QoreQPalette *q_qp = new QoreQPalette(qp->getQPalette()->resolve(*(other->getQPalette())));
   o_qp->setPrivate(CID_QPALETTE, q_qp);
   return o_qp;
}


//void set ( const QPalette & other )
static AbstractQoreNode *QPALETTE_set(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQPalette *other = p ? (QoreQPalette *)p->getReferencedPrivateData(CID_QPALETTE, xsink) : 0;
   if (!other) {
      if (!xsink->isException())
         xsink->raiseException("QPALETTE-SET-PARAM-ERROR", "expecting a QPalette object as first argument to QPalette::set()");
      return 0;
   }
   ReferenceHolder<QoreQPalette> otherHolder(other, xsink);
   
   *(qp->getQPalette()) = *(other->getQPalette());
   return 0;
}

//void setBrush ( ColorRole role, const QBrush & brush )
//void setBrush ( ColorGroup group, ColorRole role, const QBrush & brush )
static AbstractQoreNode *QPALETTE_setBrush(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
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
static AbstractQoreNode *QPALETTE_setColor(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int a1 = p ? p->getAsInt() : 0;

   p = get_param(params, 1);
   int a2 = 0;
   bool got_a2 = false;
   if (!p || p->getType() != NT_OBJECT) {
      a2 = p ? p->getAsInt() : 0;
      got_a2 = true;
      p = get_param(params, 2);
   }

   const QoreObject *o = reinterpret_cast<const QoreObject *>(p);
   QoreQColor *color = (QoreQColor *)o->getReferencedPrivateData(CID_QCOLOR, xsink);
   if (!color) {
      if (!xsink->isException())
         xsink->raiseException("QPALETTE-SETCOLOR-PARAM-ERROR", "this version of QPalette::setColor() expects an object derived from QColor as the final argument");
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
static AbstractQoreNode *QPALETTE_setColorGroup(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QPalette::ColorGroup cg = (QPalette::ColorGroup)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QBrush windowText;
   if (get_qbrush(p, windowText, xsink))
      return 0;
   
   p = get_param(params, 2);
   QBrush button;
   if (get_qbrush(p, button, xsink))
      return 0;
   
   p = get_param(params, 3);
   QBrush light;
   if (get_qbrush(p, light, xsink))
      return 0;
   
   p = get_param(params, 4);
   QBrush dark;
   if (get_qbrush(p, dark, xsink))
      return 0;
   
   p = get_param(params, 5);
   QBrush mid;
   if (get_qbrush(p, mid, xsink))
      return 0;
   
   p = get_param(params, 6);
   QBrush text;
   if (get_qbrush(p, text, xsink))
      return 0;
   
   p = get_param(params, 7);
   QBrush bright_text;
   if (get_qbrush(p, bright_text, xsink))
      return 0;
   
   p = get_param(params, 8);
   QBrush base;
   if (get_qbrush(p, base, xsink))
      return 0;
   
   p = get_param(params, 9);
   QBrush window;
   if (get_qbrush(p, window, xsink))
      return 0;

   qp->getQPalette()->setColorGroup(cg, windowText, button, light, dark, mid, text, bright_text, base, window);

   return 0;
}

//void setCurrentColorGroup ( ColorGroup cg )
static AbstractQoreNode *QPALETTE_setCurrentColorGroup(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QPalette::ColorGroup cg = (QPalette::ColorGroup)(p ? p->getAsInt() : 0);
   qp->getQPalette()->setCurrentColorGroup(cg);
   return 0;
}

//const QBrush & shadow () const
static AbstractQoreNode *QPALETTE_shadow(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(&qp->getQPalette()->shadow());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//const QBrush & text () const
static AbstractQoreNode *QPALETTE_text(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(&qp->getQPalette()->text());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//const QBrush & window () const
static AbstractQoreNode *QPALETTE_window(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(&qp->getQPalette()->window());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//const QBrush & windowText () const
static AbstractQoreNode *QPALETTE_windowText(QoreObject *self, QoreQPalette *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(&qp->getQPalette()->windowText());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
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

QoreNamespace *initQPaletteNS()
{   
   QoreNamespace *qpalette = new QoreNamespace("QPalette");

   qpalette->addSystemClass(initQPaletteClass());

   // ColorGroup enum
   qpalette->addConstant("Active",                   new QoreBigIntNode(QPalette::Active));
   qpalette->addConstant("Disabled",                 new QoreBigIntNode(QPalette::Disabled));
   qpalette->addConstant("Inactive",                 new QoreBigIntNode(QPalette::Inactive));
   qpalette->addConstant("NColorGroups",             new QoreBigIntNode(QPalette::NColorGroups));
   qpalette->addConstant("Current",                  new QoreBigIntNode(QPalette::Current));
   qpalette->addConstant("All",                      new QoreBigIntNode(QPalette::All));
   qpalette->addConstant("Normal",                   new QoreBigIntNode(QPalette::Normal));

   // ColorRole enum
   qpalette->addConstant("WindowText",               new QoreBigIntNode(QPalette::WindowText));
   qpalette->addConstant("Button",                   new QoreBigIntNode(QPalette::Button));
   qpalette->addConstant("Light",                    new QoreBigIntNode(QPalette::Light));
   qpalette->addConstant("Midlight",                 new QoreBigIntNode(QPalette::Midlight));
   qpalette->addConstant("Dark",                     new QoreBigIntNode(QPalette::Dark));
   qpalette->addConstant("Mid",                      new QoreBigIntNode(QPalette::Mid));
   qpalette->addConstant("Text",                     new QoreBigIntNode(QPalette::Text));
   qpalette->addConstant("BrightText",               new QoreBigIntNode(QPalette::BrightText));
   qpalette->addConstant("ButtonText",               new QoreBigIntNode(QPalette::ButtonText));
   qpalette->addConstant("Base",                     new QoreBigIntNode(QPalette::Base));
   qpalette->addConstant("Window",                   new QoreBigIntNode(QPalette::Window));
   qpalette->addConstant("Shadow",                   new QoreBigIntNode(QPalette::Shadow));
   qpalette->addConstant("Highlight",                new QoreBigIntNode(QPalette::Highlight));
   qpalette->addConstant("HighlightedText",          new QoreBigIntNode(QPalette::HighlightedText));
   qpalette->addConstant("Link",                     new QoreBigIntNode(QPalette::Link));
   qpalette->addConstant("LinkVisited",              new QoreBigIntNode(QPalette::LinkVisited));
   qpalette->addConstant("AlternateBase",            new QoreBigIntNode(QPalette::AlternateBase));
   qpalette->addConstant("NoRole",                   new QoreBigIntNode(QPalette::NoRole));
   qpalette->addConstant("NColorRoles",              new QoreBigIntNode(QPalette::NColorRoles));
   qpalette->addConstant("Foreground",               new QoreBigIntNode(QPalette::Foreground));
   qpalette->addConstant("Background",               new QoreBigIntNode(QPalette::Background));

   return qpalette;
}
