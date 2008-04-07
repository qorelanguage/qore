/*
 QC_QGLFramebufferObject.h
 
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

#ifndef _QORE_QT_QC_QGLFRAMEBUFFEROBJECT_H

#define _QORE_QT_QC_QGLFRAMEBUFFEROBJECT_H

#include <QGLFramebufferObject>

#include "QoreAbstractQPaintDevice.h"

DLLLOCAL extern int CID_QGLFRAMEBUFFEROBJECT;
DLLLOCAL extern QoreClass *QC_QGLFramebufferObject;
DLLLOCAL QoreNamespace *initQGLFramebufferObjectNS(QoreClass *);
DLLLOCAL void initQGLFramebufferObjectStaticFunctions();

class QoreQGLFramebufferObject : public AbstractPrivateData, public QoreAbstractQPaintDevice, public QGLFramebufferObject
{
   public:
      DLLLOCAL QoreQGLFramebufferObject(const QSize& size, GLenum target = GL_TEXTURE_2D) : QGLFramebufferObject(size, target)
      {
      }
      DLLLOCAL QoreQGLFramebufferObject(int width, int height, GLenum target = GL_TEXTURE_2D) : QGLFramebufferObject(width, height, target)
      {
      }
      DLLLOCAL QoreQGLFramebufferObject(int width, int height, Attachment attachment, GLenum target = GL_TEXTURE_2D, GLenum internal_format = GL_RGBA8) : QGLFramebufferObject(width, height, attachment, target, internal_format)
      {
      }
      DLLLOCAL QoreQGLFramebufferObject(const QSize& size, Attachment attachment, GLenum target = GL_TEXTURE_2D, GLenum internal_format = GL_RGBA8) : QGLFramebufferObject(size, attachment, target, internal_format)
      {
      }

      DLLLOCAL virtual class QPaintDevice *getQPaintDevice() const
      {
	 return static_cast<QPaintDevice *>(const_cast<QoreQGLFramebufferObject *>(this));
      }

};

#endif // _QORE_QT_QC_QGLFRAMEBUFFEROBJECT_H
