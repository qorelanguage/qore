/*
 QC_QTimeLine.cc
 
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

#include "qore-qt.h"

#include "QC_QTimeLine.h"
#include "QC_QObject.h"

int CID_QTIMELINE;
QoreClass *QC_QTimeLine = 0;

//QTimeLine ( int duration = 1000, QObject * parent = 0 )
static void QTIMELINE_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int duration = !is_nothing(p) ? p->getAsInt() : 1000;
   p = get_param(params, 1);
   QoreAbstractQObject *parent = (p && p->getType() == NT_OBJECT) ? (QoreAbstractQObject *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QTIMELINE, new QoreQTimeLine(self, duration, parent ? parent->getQObject() : 0));
   return;
}

static void QTIMELINE_copy(QoreObject *self, QoreObject *old, QoreQTimeLine *qtl, ExceptionSink *xsink)
{
   xsink->raiseException("QTIMELINE-COPY-ERROR", "objects of this class cannot be copied");
}

//int currentFrame () const
static AbstractQoreNode *QTIMELINE_currentFrame(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtl->qobj->currentFrame());
}

//int currentTime () const
static AbstractQoreNode *QTIMELINE_currentTime(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtl->qobj->currentTime());
}

//qreal currentValue () const
static AbstractQoreNode *QTIMELINE_currentValue(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qtl->qobj->currentValue());
}

//CurveShape curveShape () const
static AbstractQoreNode *QTIMELINE_curveShape(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtl->qobj->curveShape());
}

//Direction direction () const
static AbstractQoreNode *QTIMELINE_direction(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtl->qobj->direction());
}

//int duration () const
static AbstractQoreNode *QTIMELINE_duration(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtl->qobj->duration());
}

//int endFrame () const
static AbstractQoreNode *QTIMELINE_endFrame(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtl->qobj->endFrame());
}

//int frameForTime ( int msec ) const
static AbstractQoreNode *QTIMELINE_frameForTime(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int msec = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qtl->qobj->frameForTime(msec));
}

//int loopCount () const
static AbstractQoreNode *QTIMELINE_loopCount(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtl->qobj->loopCount());
}

//void setCurveShape ( CurveShape shape )
static AbstractQoreNode *QTIMELINE_setCurveShape(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QTimeLine::CurveShape shape = (QTimeLine::CurveShape)(p ? p->getAsInt() : 0);
   qtl->qobj->setCurveShape(shape);
   return 0;
}

//void setDirection ( Direction direction )
static AbstractQoreNode *QTIMELINE_setDirection(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QTimeLine::Direction direction = (QTimeLine::Direction)(p ? p->getAsInt() : 0);
   qtl->qobj->setDirection(direction);
   return 0;
}

//void setDuration ( int duration )
static AbstractQoreNode *QTIMELINE_setDuration(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int duration = p ? p->getAsInt() : 0;
   qtl->qobj->setDuration(duration);
   return 0;
}

//void setEndFrame ( int frame )
static AbstractQoreNode *QTIMELINE_setEndFrame(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int frame = p ? p->getAsInt() : 0;
   qtl->qobj->setEndFrame(frame);
   return 0;
}

//void setFrameRange ( int startFrame, int endFrame )
static AbstractQoreNode *QTIMELINE_setFrameRange(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int startFrame = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int endFrame = p ? p->getAsInt() : 0;
   qtl->qobj->setFrameRange(startFrame, endFrame);
   return 0;
}

//void setLoopCount ( int count )
static AbstractQoreNode *QTIMELINE_setLoopCount(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int count = p ? p->getAsInt() : 0;
   qtl->qobj->setLoopCount(count);
   return 0;
}

//void setStartFrame ( int frame )
static AbstractQoreNode *QTIMELINE_setStartFrame(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int frame = p ? p->getAsInt() : 0;
   qtl->qobj->setStartFrame(frame);
   return 0;
}

//void setUpdateInterval ( int interval )
static AbstractQoreNode *QTIMELINE_setUpdateInterval(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int interval = p ? p->getAsInt() : 0;
   qtl->qobj->setUpdateInterval(interval);
   return 0;
}

//int startFrame () const
static AbstractQoreNode *QTIMELINE_startFrame(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtl->qobj->startFrame());
}

//State state () const
static AbstractQoreNode *QTIMELINE_state(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtl->qobj->state());
}

//int updateInterval () const
static AbstractQoreNode *QTIMELINE_updateInterval(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtl->qobj->updateInterval());
}

//virtual qreal valueForTime ( int msec ) const
static AbstractQoreNode *QTIMELINE_valueForTime(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int msec = p ? p->getAsInt() : 0;
   return new QoreFloatNode(qtl->qobj->valueForTime(msec));
}

//void resume ()
static AbstractQoreNode *QTIMELINE_resume(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   qtl->qobj->resume();
   return 0;
}

//void setCurrentTime ( int msec )
static AbstractQoreNode *QTIMELINE_setCurrentTime(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int msec = p ? p->getAsInt() : 0;
   qtl->qobj->setCurrentTime(msec);
   return 0;
}

//void setPaused ( bool paused )
static AbstractQoreNode *QTIMELINE_setPaused(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool paused = p ? p->getAsBool() : false;
   qtl->qobj->setPaused(paused);
   return 0;
}

//void start ()
static AbstractQoreNode *QTIMELINE_start(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   qtl->qobj->start();
   return 0;
}

//void stop ()
static AbstractQoreNode *QTIMELINE_stop(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   qtl->qobj->stop();
   return 0;
}

//void toggleDirection ()
static AbstractQoreNode *QTIMELINE_toggleDirection(QoreObject *self, QoreQTimeLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   qtl->qobj->toggleDirection();
   return 0;
}

static QoreClass *initQTimeLineClass(QoreClass *qobject)
{
   QC_QTimeLine = new QoreClass("QTimeLine", QDOM_GUI);
   CID_QTIMELINE = QC_QTimeLine->getID();

   QC_QTimeLine->addBuiltinVirtualBaseClass(qobject);

   QC_QTimeLine->setConstructor(QTIMELINE_constructor);
   QC_QTimeLine->setCopy((q_copy_t)QTIMELINE_copy);

   QC_QTimeLine->addMethod("currentFrame",                (q_method_t)QTIMELINE_currentFrame);
   QC_QTimeLine->addMethod("currentTime",                 (q_method_t)QTIMELINE_currentTime);
   QC_QTimeLine->addMethod("currentValue",                (q_method_t)QTIMELINE_currentValue);
   QC_QTimeLine->addMethod("curveShape",                  (q_method_t)QTIMELINE_curveShape);
   QC_QTimeLine->addMethod("direction",                   (q_method_t)QTIMELINE_direction);
   QC_QTimeLine->addMethod("duration",                    (q_method_t)QTIMELINE_duration);
   QC_QTimeLine->addMethod("endFrame",                    (q_method_t)QTIMELINE_endFrame);
   QC_QTimeLine->addMethod("frameForTime",                (q_method_t)QTIMELINE_frameForTime);
   QC_QTimeLine->addMethod("loopCount",                   (q_method_t)QTIMELINE_loopCount);
   QC_QTimeLine->addMethod("setCurveShape",               (q_method_t)QTIMELINE_setCurveShape);
   QC_QTimeLine->addMethod("setDirection",                (q_method_t)QTIMELINE_setDirection);
   QC_QTimeLine->addMethod("setDuration",                 (q_method_t)QTIMELINE_setDuration);
   QC_QTimeLine->addMethod("setEndFrame",                 (q_method_t)QTIMELINE_setEndFrame);
   QC_QTimeLine->addMethod("setFrameRange",               (q_method_t)QTIMELINE_setFrameRange);
   QC_QTimeLine->addMethod("setLoopCount",                (q_method_t)QTIMELINE_setLoopCount);
   QC_QTimeLine->addMethod("setStartFrame",               (q_method_t)QTIMELINE_setStartFrame);
   QC_QTimeLine->addMethod("setUpdateInterval",           (q_method_t)QTIMELINE_setUpdateInterval);
   QC_QTimeLine->addMethod("startFrame",                  (q_method_t)QTIMELINE_startFrame);
   QC_QTimeLine->addMethod("state",                       (q_method_t)QTIMELINE_state);
   QC_QTimeLine->addMethod("updateInterval",              (q_method_t)QTIMELINE_updateInterval);
   QC_QTimeLine->addMethod("valueForTime",                (q_method_t)QTIMELINE_valueForTime);
   QC_QTimeLine->addMethod("resume",                      (q_method_t)QTIMELINE_resume);
   QC_QTimeLine->addMethod("setCurrentTime",              (q_method_t)QTIMELINE_setCurrentTime);
   QC_QTimeLine->addMethod("setPaused",                   (q_method_t)QTIMELINE_setPaused);
   QC_QTimeLine->addMethod("start",                       (q_method_t)QTIMELINE_start);
   QC_QTimeLine->addMethod("stop",                        (q_method_t)QTIMELINE_stop);
   QC_QTimeLine->addMethod("toggleDirection",             (q_method_t)QTIMELINE_toggleDirection);

   return QC_QTimeLine;
}

QoreNamespace *initQTimeLineNS(QoreClass *qobject)
{
   QoreNamespace *ns = new QoreNamespace("QTimeLine");
   ns->addSystemClass(initQTimeLineClass(qobject));

   // State enum   
   ns->addConstant("NotRunning",  new QoreBigIntNode(QTimeLine::NotRunning));
   ns->addConstant("Paused",      new QoreBigIntNode(QTimeLine::Paused));
   ns->addConstant("Running",     new QoreBigIntNode(QTimeLine::Running));

   // Direction enum
   ns->addConstant("Forward",                  new QoreBigIntNode(QTimeLine::Forward));
   ns->addConstant("Backward",                 new QoreBigIntNode(QTimeLine::Backward));

   // CurveShape enum
   ns->addConstant("EaseInCurve",              new QoreBigIntNode(QTimeLine::EaseInCurve));
   ns->addConstant("EaseOutCurve",             new QoreBigIntNode(QTimeLine::EaseOutCurve));
   ns->addConstant("EaseInOutCurve",           new QoreBigIntNode(QTimeLine::EaseInOutCurve));
   ns->addConstant("LinearCurve",              new QoreBigIntNode(QTimeLine::LinearCurve));
   ns->addConstant("SineCurve",                new QoreBigIntNode(QTimeLine::SineCurve));

   return ns;
}
