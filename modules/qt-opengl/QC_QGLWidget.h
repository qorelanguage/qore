/*
 QC_QGLWidget.h
 
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

#ifndef _QORE_QT_QC_QGLWIDGET_H

#define _QORE_QT_QC_QGLWIDGET_H

#include <QGLWidget>
#include "QoreAbstractQWidget.h"
#include "qore-qt-events.h"

DLLLOCAL extern int CID_QGLWIDGET;
DLLLOCAL extern QoreClass *QC_QGLWidget;
DLLLOCAL QoreClass *initQGLWidgetClass(QoreClass *);

class myQGLWidget : public QGLWidget, public QoreQWidgetExtension
{
#define QOREQTYPE QGLWidget
#define MYQOREQTYPE myQGLWidget
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   protected:
      const QoreMethod *m_glDraw, *m_glInit, *m_initializeGL, *m_initializeOverlayGL, 
	 *m_paintGL, *m_paintOverlayGL, *m_resizeGL, *m_resizeOverlayGL;

      void init(const QoreClass *oc)
      {
	 m_glDraw               = oc->findMethod("glDraw");
	 m_glInit               = oc->findMethod("glInit");
	 m_initializeGL         = oc->findMethod("initializeGL");
	 m_initializeOverlayGL  = oc->findMethod("initializeOverlayGL");
	 m_paintGL              = oc->findMethod("paintGL");
	 m_paintOverlayGL       = oc->findMethod("paintOverlayGL");
	 m_resizeGL             = oc->findMethod("resizeGL");
	 m_resizeOverlayGL      = oc->findMethod("resizeOverlayGL");
      }

      virtual void glDraw()
      {
	 if (!m_glDraw) {
	    QGLWidget::glDraw();
	    return;
	 }

	 dispatch_event(qore_obj, m_glDraw, 0);
      }

      virtual void glInit()
      {
	 if (!m_glInit) {
	    QGLWidget::glInit();
	    return;
	 }

	 dispatch_event(qore_obj, m_glInit, 0);
      }

      virtual void initializeGL()
      {
	 if (!m_initializeGL) {
	    QGLWidget::initializeGL();
	    return;
	 }

	 dispatch_event(qore_obj, m_initializeGL, 0);
      }

      virtual void initializeOverlayGL()
      {
	 if (!m_initializeOverlayGL) {
	    QGLWidget::initializeOverlayGL();
	    return;
	 }

	 dispatch_event(qore_obj, m_initializeOverlayGL, 0);
      }

      virtual void paintGL()
      {
	 if (!m_paintGL) {
	    QGLWidget::paintGL();
	    return;
	 }

	 dispatch_event(qore_obj, m_paintGL, 0);
      }

      virtual void paintOverlayGL()
      {
	 if (!m_paintOverlayGL) {
	    QGLWidget::paintOverlayGL();
	    return;
	 }

	 dispatch_event(qore_obj, m_paintOverlayGL, 0);
      }
      
      virtual void resizeGL(int width, int height)
      {
	 if (!m_resizeGL) {
	    QGLWidget::resizeGL(width, height);
	    return;
	 }

         ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
         args->push(new QoreBigIntNode(width));
         args->push(new QoreBigIntNode(height));

	 dispatch_event(qore_obj, m_resizeGL, *args);
      }

      virtual void resizeOverlayGL(int width, int height)
      {
	 if (!m_resizeOverlayGL) {
	    QGLWidget::resizeOverlayGL(width, height);
	    return;
	 }

         ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
         args->push(new QoreBigIntNode(width));
         args->push(new QoreBigIntNode(height));

	 dispatch_event(qore_obj, m_resizeOverlayGL, *args);
      }

   public:
      DLLLOCAL myQGLWidget(QoreObject *obj, QWidget* parent = 0, const QGLWidget* shareWidget = 0, Qt::WindowFlags f = 0) : QGLWidget(parent, shareWidget, f), QoreQWidgetExtension(obj, this)
      {
	 init(obj->getClass());
      }
      DLLLOCAL myQGLWidget(QoreObject *obj, QGLContext* context, QWidget* parent = 0, const QGLWidget* shareWidget = 0, Qt::WindowFlags f = 0) : QGLWidget(context, parent, shareWidget, f), QoreQWidgetExtension(obj, this)
      {
	 init(obj->getClass());
      }
      DLLLOCAL myQGLWidget(QoreObject *obj, const QGLFormat& format, QWidget* parent = 0, const QGLWidget* shareWidget = 0, Qt::WindowFlags f = 0) : QGLWidget(format, parent, shareWidget, f), QoreQWidgetExtension(obj, this)
      {
	 init(obj->getClass());
      }

      bool parent_autoBufferSwap() const
      {
	 return autoBufferSwap();
      }

      void parent_glDraw()
      {
	 QGLWidget::glDraw();
      }

      void parent_glInit()
      {
	 QGLWidget::glInit();
      }

      void parent_initializeGL()
      {
	 QGLWidget::initializeGL();
      }
      
      void parent_initializeOverlayGL()
      {
	 QGLWidget::initializeOverlayGL();
      }

      void parent_paintGL()
      {
	 QGLWidget::paintGL();
      }

      void parent_paintOverlayGL()
      {
	 QGLWidget::paintOverlayGL();
      }

      void parent_resizeGL(int width, int height)
      {
	 QGLWidget::resizeGL(width, height);
      }

      void parent_resizeOverlayGL(int width, int height)
      {
	 QGLWidget::resizeOverlayGL(width, height);
      }

      void parent_setAutoBufferSwap(bool on)
      {
	 QGLWidget::setAutoBufferSwap(on);
      }
};

typedef QoreQWidgetBase<myQGLWidget, QoreAbstractQWidget> QoreQGLWidgetImpl;

class QoreQGLWidget : public QoreQGLWidgetImpl
{
   public:
      DLLLOCAL QoreQGLWidget(QoreObject *obj, QWidget* parent = 0, const QGLWidget* shareWidget = 0, Qt::WindowFlags f = 0) : QoreQGLWidgetImpl(new myQGLWidget(obj, parent, shareWidget, f))
      {
      }
      DLLLOCAL QoreQGLWidget(QoreObject *obj, QGLContext* context, QWidget* parent = 0, const QGLWidget* shareWidget = 0, Qt::WindowFlags f = 0) : QoreQGLWidgetImpl(new myQGLWidget(obj, context, parent, shareWidget, f))
      {
      }
      DLLLOCAL QoreQGLWidget(QoreObject *obj, const QGLFormat& format, QWidget* parent = 0, const QGLWidget* shareWidget = 0, Qt::WindowFlags f = 0) : QoreQGLWidgetImpl(new myQGLWidget(obj, format, parent, shareWidget, f))
      {
      }

      bool autoBufferSwap() const
      {
	 return qobj->parent_autoBufferSwap();
      }

      void glDraw()
      {
	 qobj->parent_glDraw();
      }

      void glInit()
      {
	 qobj->parent_glInit();
      }

      void initializeGL()
      {
	 qobj->parent_initializeGL();
      }

      void initializeOverlayGL()
      {
	 qobj->parent_initializeOverlayGL();
      }

      void paintGL()
      {
	 qobj->parent_paintGL();
      }

      void paintOverlayGL()
      {
	 qobj->parent_paintOverlayGL();
      }
      
      void resizeGL(int width, int height)
      {
	 qobj->parent_resizeGL(width, height);
      }

      void resizeOverlayGL(int width, int height)
      {
	 qobj->parent_resizeOverlayGL(width, height);
      }

      void setAutoBufferSwap(bool on)
      {
	 qobj->parent_setAutoBufferSwap(on);
      }
};

typedef QoreQtQWidgetBase<QGLWidget, QoreAbstractQWidget> QoreQtQGLWidgetImpl;

class QoreQtQGLWidget : public QoreQtQGLWidgetImpl
{
   public:
      DLLLOCAL QoreQtQGLWidget(QoreObject *obj, QGLWidget *qglwidget) : QoreQtQGLWidgetImpl(obj, qglwidget)
      {
      }
};

#endif // _QORE_QT_QC_QGLWIDGET_H
