/*
 QC_QLabel.cc
 
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

#include "QC_QLabel.h"
#include "QC_QMovie.h"
#include "QC_QPicture.h"
#include "QC_QPixmap.h"
#include "QC_QWidget.h"

int CID_QLABEL;

static void QLABEL_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreQLabel *ql;

   const AbstractQoreNode *p = get_param(params, 0);
   const char *text = 0;

   {
      const QoreStringNode *pstr = dynamic_cast<const QoreStringNode *>(p);
      if (pstr) {
	 text = pstr->getBuffer();
	 p = get_param(params, 1);
      }
   }

   QoreAbstractQWidget *parent = 0;
   if (p && p->type == NT_OBJECT)
   {
      parent = (QoreAbstractQWidget *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink);
      if (!parent)
      {
         xsink->raiseException("QLABEL-CONSTRUCTOR-ERROR", "object passed to QLabel::constructor() is not derived from QWidget (class: '%s')", (reinterpret_cast<const QoreObject *>(p))->getClassName());
         return;
      }
   }
    
   ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);

   // get windowflags parameter
   p = get_param(params, 1 + (text == 0 ? 0 : 1));
   Qt::WindowFlags f = (Qt::WindowFlags)(p ? p->getAsInt() : 0);

   if (text)
      ql = new QoreQLabel(self, text, parent ? parent->getQWidget() : 0, f);
   else
      ql = new QoreQLabel(self, parent ? parent->getQWidget() : 0, f);

   self->setPrivate(CID_QLABEL, ql);
}

static void QLABEL_copy(class QoreObject *self, class QoreObject *old, class QoreQLabel *qlcdn, ExceptionSink *xsink)
{
   xsink->raiseException("QLABEL-COPY-ERROR", "objects of this class cannot be copied");
}

//Qt::Alignment alignment () const
static AbstractQoreNode *QLABEL_alignment(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(ql->qobj->alignment());
}

//QWidget * buddy () const
//static AbstractQoreNode *QLABEL_buddy(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return ql->qobj->buddy();
//}

//bool hasScaledContents () const
static AbstractQoreNode *QLABEL_hasScaledContents(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(ql->qobj->hasScaledContents());
}

//int indent () const
static AbstractQoreNode *QLABEL_indent(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(ql->qobj->indent());
}

//int margin () const
static AbstractQoreNode *QLABEL_margin(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(ql->qobj->margin());
}

//QMovie * movie () const
static AbstractQoreNode *QLABEL_movie(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qm = new QoreObject(QC_QMovie, getProgram());
   QoreQMovie *q_qm;
   QMovie *mov = ql->qobj->movie();
   if (mov->device())
      q_qm = new QoreQMovie(o_qm, mov->device(), mov->format());
   else
      q_qm = new QoreQMovie(o_qm, mov->fileName(), mov->format());
   o_qm->setPrivate(CID_QMOVIE, q_qm);
   return o_qm;
}

//bool openExternalLinks () const
static AbstractQoreNode *QLABEL_openExternalLinks(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(ql->qobj->openExternalLinks());
}

//const QPicture * picture () const
static AbstractQoreNode *QLABEL_picture(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreQPicture *q_qp = new QoreQPicture(*(ql->qobj->picture()));
   QoreObject *o_qp = new QoreObject(QC_QPicture, getProgram());
   o_qp->setPrivate(CID_QPICTURE, q_qp);
   return o_qp;
}

//const QPixmap * pixmap () const
static AbstractQoreNode *QLABEL_pixmap(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreQPixmap *q_qp = new QoreQPixmap(*(ql->qobj->pixmap()));
   QoreObject *o_qp = new QoreObject(QC_QPixmap, getProgram());
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return o_qp;
}

//void setAlignment ( Qt::Alignment )
static AbstractQoreNode *QLABEL_setAlignment(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::Alignment alignment = (Qt::Alignment)(p ? p->getAsInt() : 0);
   ql->qobj->setAlignment(alignment);
   return 0;
}

//void setBuddy ( QWidget * buddy )
static AbstractQoreNode *QLABEL_setBuddy(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreAbstractQWidget *buddy = p ? (QoreAbstractQWidget *)p->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!buddy) {
      if (!xsink->isException())
         xsink->raiseException("QLABEL-SETBUDDY-PARAM-ERROR", "expecting a QWidget object as first argument to QLabel::setBuddy()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> holder(buddy, xsink);
   ql->qobj->setBuddy(buddy->getQWidget());
   return 0;
}

//void setIndent ( int )
static AbstractQoreNode *QLABEL_setIndent(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   ql->qobj->setIndent(x);
   return 0;
}

//void setMargin ( int )
static AbstractQoreNode *QLABEL_setMargin(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   ql->qobj->setMargin(x);
   return 0;
}

//void setOpenExternalLinks ( bool open )
static AbstractQoreNode *QLABEL_setOpenExternalLinks(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool open = p ? p->getAsBool() : false;
   ql->qobj->setOpenExternalLinks(open);
   return 0;
}

//void setScaledContents ( bool )
static AbstractQoreNode *QLABEL_setScaledContents(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   ql->qobj->setScaledContents(b);
   return 0;
}

//void setTextFormat ( Qt::TextFormat )
static AbstractQoreNode *QLABEL_setTextFormat(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::TextFormat textformat = (Qt::TextFormat)(p ? p->getAsInt() : 0);
   ql->qobj->setTextFormat(textformat);
   return 0;
}

//void setTextInteractionFlags ( Qt::TextInteractionFlags flags )
static AbstractQoreNode *QLABEL_setTextInteractionFlags(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::TextInteractionFlags flags = (Qt::TextInteractionFlags)(p ? p->getAsInt() : 0);
   ql->qobj->setTextInteractionFlags(flags);
   return 0;
}

//void setWordWrap ( bool on )
static AbstractQoreNode *QLABEL_setWordWrap(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool on = p ? p->getAsBool() : false;
   ql->qobj->setWordWrap(on);
   return 0;
}

//QString text () const
static AbstractQoreNode *QLABEL_text(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(ql->qobj->text().toUtf8().data(), QCS_UTF8);
}

//Qt::TextFormat textFormat () const
static AbstractQoreNode *QLABEL_textFormat(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(ql->qobj->textFormat());
}

//Qt::TextInteractionFlags textInteractionFlags () const
static AbstractQoreNode *QLABEL_textInteractionFlags(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(ql->qobj->textInteractionFlags());
}

//bool wordWrap () const
static AbstractQoreNode *QLABEL_wordWrap(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(ql->qobj->wordWrap());
}

//void clear ()
static AbstractQoreNode *QLABEL_clear(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   ql->qobj->clear();
   return 0;
}

//void setMovie ( QMovie * movie )
static AbstractQoreNode *QLABEL_setMovie(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQMovie *movie = p ? (QoreQMovie *)p->getReferencedPrivateData(CID_QMOVIE, xsink) : 0;
   if (!movie) {
      if (!xsink->isException())
         xsink->raiseException("QLABEL-SETMOVIE-PARAM-ERROR", "expecting a QMovie object as first argument to QLabel::setMovie()");
      return 0;
   }
   ReferenceHolder<QoreQMovie> holder(movie, xsink);
   ql->qobj->setMovie(movie->qobj);
   return 0;
}

//void setNum ( int num )
//void setNum ( double num )
static AbstractQoreNode *QLABEL_setNum(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->type == NT_INT) {
      int num = p ? p->getAsInt() : 0;
      ql->qobj->setNum(num);
      return 0;
   }
   float num = p ? p->getAsFloat() : 0.0;
   ql->qobj->setNum(num);
   return 0;
}

//void setPicture ( const QPicture & picture )
static AbstractQoreNode *QLABEL_setPicture(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQPicture *picture = p ? (QoreQPicture *)p->getReferencedPrivateData(CID_QPICTURE, xsink) : 0;
   if (!picture) {
      if (!xsink->isException())
         xsink->raiseException("QLABEL-SETPICTURE-PARAM-ERROR", "expecting a QPicture object as first argument to QLabel::setPicture()");
      return 0;
   }
   ReferenceHolder<QoreQPicture> holder(picture, xsink);
   ql->qobj->setPicture(*((QPicture *)picture));
   return 0;
}

//void setPixmap ( const QPixmap & )
static AbstractQoreNode *QLABEL_setPixmap(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   AbstractPrivateData *apd_qpixmap = (p && p->type == NT_OBJECT) ? (reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (!apd_qpixmap) {
      if (!xsink->isException())
         xsink->raiseException("QLABEL-SETPIXMAP-PARAM-ERROR", "expecting a QPixmap object as first argument to QLabel::setPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> holder(apd_qpixmap, xsink);
   QoreAbstractQPixmap *qpixmap = dynamic_cast<QoreAbstractQPixmap *>(apd_qpixmap);
   assert(qpixmap);
   ql->qobj->setPixmap(*(qpixmap->getQPixmap()));
   return 0;
}

//void setText ( const QString & )
static AbstractQoreNode *QLABEL_setText(QoreObject *self, QoreQLabel *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p) {
      xsink->raiseException("QLABEL-SETTEXT-PARAM-ERROR", "expecting a string as first argument to QLabel::setText()");
      return 0;
   }
   const char *qstring = p->getBuffer();
   ql->qobj->setText(qstring);
   return 0;
}

class QoreClass *initQLabelClass(class QoreClass *qframe)
{
   tracein("initQLabelClass()");
   
   class QoreClass *QC_QLabel = new QoreClass("QLabel", QDOM_GUI);
   CID_QLABEL = QC_QLabel->getID();

   QC_QLabel->addBuiltinVirtualBaseClass(qframe);

   QC_QLabel->setConstructor(QLABEL_constructor);
   QC_QLabel->setCopy((q_copy_t)QLABEL_copy);

   QC_QLabel->addMethod("alignment",                   (q_method_t)QLABEL_alignment);
   //QC_QLabel->addMethod("buddy",                       (q_method_t)QLABEL_buddy);
   QC_QLabel->addMethod("hasScaledContents",           (q_method_t)QLABEL_hasScaledContents);
   QC_QLabel->addMethod("indent",                      (q_method_t)QLABEL_indent);
   QC_QLabel->addMethod("margin",                      (q_method_t)QLABEL_margin);
   QC_QLabel->addMethod("movie",                       (q_method_t)QLABEL_movie);
   QC_QLabel->addMethod("openExternalLinks",           (q_method_t)QLABEL_openExternalLinks);
   QC_QLabel->addMethod("picture",                     (q_method_t)QLABEL_picture);
   QC_QLabel->addMethod("pixmap",                      (q_method_t)QLABEL_pixmap);
   QC_QLabel->addMethod("setAlignment",                (q_method_t)QLABEL_setAlignment);
   QC_QLabel->addMethod("setBuddy",                    (q_method_t)QLABEL_setBuddy);
   QC_QLabel->addMethod("setIndent",                   (q_method_t)QLABEL_setIndent);
   QC_QLabel->addMethod("setMargin",                   (q_method_t)QLABEL_setMargin);
   QC_QLabel->addMethod("setOpenExternalLinks",        (q_method_t)QLABEL_setOpenExternalLinks);
   QC_QLabel->addMethod("setScaledContents",           (q_method_t)QLABEL_setScaledContents);
   QC_QLabel->addMethod("setTextFormat",               (q_method_t)QLABEL_setTextFormat);
   QC_QLabel->addMethod("setTextInteractionFlags",     (q_method_t)QLABEL_setTextInteractionFlags);
   QC_QLabel->addMethod("setWordWrap",                 (q_method_t)QLABEL_setWordWrap);
   QC_QLabel->addMethod("text",                        (q_method_t)QLABEL_text);
   QC_QLabel->addMethod("textFormat",                  (q_method_t)QLABEL_textFormat);
   QC_QLabel->addMethod("textInteractionFlags",        (q_method_t)QLABEL_textInteractionFlags);
   QC_QLabel->addMethod("wordWrap",                    (q_method_t)QLABEL_wordWrap);
   QC_QLabel->addMethod("clear",                       (q_method_t)QLABEL_clear);
   QC_QLabel->addMethod("setMovie",                    (q_method_t)QLABEL_setMovie);
   QC_QLabel->addMethod("setNum",                      (q_method_t)QLABEL_setNum);
   QC_QLabel->addMethod("setPicture",                  (q_method_t)QLABEL_setPicture);
   QC_QLabel->addMethod("setPixmap",                   (q_method_t)QLABEL_setPixmap);
   QC_QLabel->addMethod("setText",                     (q_method_t)QLABEL_setText);

   traceout("initQLabelClass()");
   return QC_QLabel;
}
