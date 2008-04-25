/*
 QC_QSvgRenderer.h
 
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

#ifndef _QORE_QT_QC_QSVGRENDERER_H

#define _QORE_QT_QC_QSVGRENDERER_H

#include <QSvgRenderer>
#include "QoreAbstractQObject.h"
#include "qore-qt-events.h"

DLLLOCAL extern int CID_QSVGRENDERER;
DLLLOCAL extern QoreClass *QC_QSvgRenderer;
DLLLOCAL QoreClass *initQSvgRendererClass(QoreClass *);

class myQSvgRenderer : public QSvgRenderer, public QoreQObjectExtension
{
#define QOREQTYPE QSvgRenderer
#define MYQOREQTYPE myQSvgRenderer
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQSvgRenderer(QoreObject *obj, QObject* parent = 0) : QSvgRenderer(parent), QoreQObjectExtension(obj, this)
      {
      }
      DLLLOCAL myQSvgRenderer(QoreObject *obj, const QString& filename, QObject* parent = 0) : QSvgRenderer(filename, parent), QoreQObjectExtension(obj, this)
      {
      }
      DLLLOCAL myQSvgRenderer(QoreObject *obj, const QByteArray& contents, QObject* parent = 0) : QSvgRenderer(contents, parent), QoreQObjectExtension(obj, this)
      {
      }
};

typedef QoreQObjectBase<myQSvgRenderer, QoreAbstractQObject> QoreQSvgRendererImpl;

class QoreQSvgRenderer : public QoreQSvgRendererImpl
{
   public:
      DLLLOCAL QoreQSvgRenderer(QoreObject *obj, QObject* parent = 0) : QoreQSvgRendererImpl(new myQSvgRenderer(obj, parent))
      {
      }
      DLLLOCAL QoreQSvgRenderer(QoreObject *obj, const QString& filename, QObject* parent = 0) : QoreQSvgRendererImpl(new myQSvgRenderer(obj, filename, parent))
      {
      }
      DLLLOCAL QoreQSvgRenderer(QoreObject *obj, const QByteArray& contents, QObject* parent = 0) : QoreQSvgRendererImpl(new myQSvgRenderer(obj, contents, parent))
      {
      }
};

typedef QoreQtQObjectBase<QSvgRenderer, QoreAbstractQObject> QoreQtQSvgRendererImpl;

class QoreQtQSvgRenderer : public QoreQtQSvgRendererImpl
{
   public:
      DLLLOCAL QoreQtQSvgRenderer(QoreObject *obj, QSvgRenderer *qsvgrenderer) : QoreQtQSvgRendererImpl(obj, qsvgrenderer)
      {
      }
};

#endif // _QORE_QT_QC_QSVGRENDERER_H
