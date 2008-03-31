/*
 QC_QGLContext.h
 
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

#ifndef _QORE_QT_QC_QGLCONTEXT_H

#define _QORE_QT_QC_QGLCONTEXT_H

#include <QGLContext>

DLLLOCAL extern int CID_QGLCONTEXT;
DLLLOCAL extern QoreClass *QC_QGLContext;
DLLLOCAL QoreClass *initQGLContextClass();

class QoreAbstractQGLContext : public AbstractPrivateData
{
   public:
      DLLLOCAL virtual QGLContext *getQGLContext() = 0;
      DLLLOCAL virtual bool parent_chooseContext(const QGLContext *shareContext = 0) = 0;

#ifdef Q_WS_MAC
      DLLLOCAL virtual void *parent_chooseMacVisual(GDHandle handle) = 0;
#endif

#ifdef Q_WS_WIN
      DLLLOCAL virtual int parent_choosePixelFormat(void *dummyPfd, HDC pdc) = 0;
#endif

#ifdef Q_WS_X11
      DLLLOCAL virtual void *parent_chooseVisual() = 0;
#endif

      DLLLOCAL virtual bool parent_deviceIsPixmap() const = 0;
      DLLLOCAL virtual bool parent_initialized() const = 0;
      DLLLOCAL virtual void parent_setInitialized(bool on) = 0;
      DLLLOCAL virtual void parent_setWindowCreated(bool on) = 0;
      DLLLOCAL virtual bool parent_windowCreated() const = 0;
};

class QoreQGLContext : public AbstractPrivateData, public QGLContext
{
   protected:
/*
      QoreObject *qore_obj;

      const QoreMethod *m_chooseContext; 
#ifdef Q_WS_MAC
      // *m_chooseMacVisual,
#endif
#ifdef Q_WS_WIN
      // *m_choosePixelFormat, 
#endif
#ifdef Q_WS_X11
      // *m_chooseVisual
#endif
;

      void init()
      {
	 const QoreClass *oc = qore_obj->getClass();

	 m_chooseContext      = oc->findMethod("chooseContext");
	 //m_chooseMacVisual    = oc->findMethod("chooseMacVisual");
	 //m_choosePixelFormat  = oc->findMethod("choosePixelFormat");
	 //m_chooseVisual       = oc->findMethod("chooseVisual");
      }

      virtual bool chooseContext ( const QGLContext * shareContext = 0 )
      {
	 if (!m_chooseContext)
	    return QGLContext::chooseContext();

	 
      }

#ifdef Q_WS_MAC
      virtual void * chooseMacVisual ( GDHandle handle )
      {
      }
#endif

#ifdef Q_WS_WIN
      virtual int choosePixelFormat ( void * dummyPfd, HDC pdc )
      {
      }
#endif

#ifdef Q_WS_X11
      virtual void * chooseVisual ()
      {
      }
#endif
*/
   public:
      DLLLOCAL QoreQGLContext(const QGLFormat& format) : QGLContext(format)
      {
      }

      DLLLOCAL QGLContext *getQGLContext()
      {
	 return this;
      }

      DLLLOCAL bool parent_chooseContext(const QGLContext *shareContext = 0)
      {
	 return chooseContext(shareContext);
      }

#ifdef Q_WS_MAC
      DLLLOCAL void *parent_chooseMacVisual(GDHandle handle)
      {
	 return chooseMacVisual(handle);
      }
#endif

#ifdef Q_WS_WIN
      DLLLOCAL int parent_choosePixelFormat(void *dummyPfd, HDC pdc)
      {
	 return choosePixelFormat(dummyPfd, pdc);
      }
#endif

#ifdef Q_WS_X11
      DLLLOCAL void *parent_chooseVisual()
      {
	 return chooseVisual();
      }
#endif

      DLLLOCAL bool parent_deviceIsPixmap() const
      {
	 return deviceIsPixmap();
      }

      DLLLOCAL bool parent_initialized() const
      {
	 return initialized();
      }

      DLLLOCAL void parent_setInitialized(bool on)
      {
	 setInitialized(on);
      }

      DLLLOCAL void parent_setWindowCreated(bool on)
      {
	 setWindowCreated(on);
      }

      DLLLOCAL bool parent_windowCreated() const
      {
	 return windowCreated();
      }
};

class QoreQtQGLContext : public AbstractPrivateData
{
   protected:
      QGLContext *context;

   public:
      DLLLOCAL QoreQtQGLContext(QGLContext *n_context) : context(n_context)
      {
      }

      DLLLOCAL QGLContext *getQGLContext()
      {
	 return context;
      }

      DLLLOCAL bool parent_chooseContext(const QGLContext *shareContext = 0)
      {
	 return false;
      }

#ifdef Q_WS_MAC
      DLLLOCAL void *parent_chooseMacVisual(GDHandle handle)
      {
	 return 0;
      }
#endif

#ifdef Q_WS_WIN
      DLLLOCAL int parent_choosePixelFormat(void *dummyPfd, HDC pdc)
      {
	 return 0;
      }
#endif

#ifdef Q_WS_X11
      DLLLOCAL void *parent_chooseVisual()
      {
	 return 0;
      }
#endif

      DLLLOCAL bool parent_deviceIsPixmap() const
      {
	 return false;
      }
      DLLLOCAL bool parent_initialized() const
      {
	 return false;
      }
      DLLLOCAL void parent_setInitialized(bool on)
      {
      }
      DLLLOCAL void parent_setWindowCreated(bool on)
      {
      }
      DLLLOCAL bool parent_windowCreated() const
      {
	 return false;
      }

};

#endif // _QORE_QT_QC_QGLCONTEXT_H
