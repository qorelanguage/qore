/*
 QC_QTextBrowser.cc
 
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

#include "QC_QTextBrowser.h"

int CID_QTEXTBROWSER;
class QoreClass *QC_QTextBrowser = 0;

//QTextBrowser ( QWidget * parent = 0 )
static void QTEXTBROWSER_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->getType() == NT_OBJECT) ? (QoreQWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QTEXTBROWSER, new QoreQTextBrowser(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QTEXTBROWSER_copy(QoreObject *self, QoreObject *old, QoreQTextBrowser *qtb, ExceptionSink *xsink)
{
   xsink->raiseException("QTEXTBROWSER-COPY-ERROR", "objects of this class cannot be copied");
}

//void clearHistory ()
static AbstractQoreNode *QTEXTBROWSER_clearHistory(QoreObject *self, QoreQTextBrowser *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   qtb->qobj->clearHistory();
   return 0;
}

//bool isBackwardAvailable () const
static AbstractQoreNode *QTEXTBROWSER_isBackwardAvailable(QoreObject *self, QoreQTextBrowser *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qtb->qobj->isBackwardAvailable());
}

//bool isForwardAvailable () const
static AbstractQoreNode *QTEXTBROWSER_isForwardAvailable(QoreObject *self, QoreQTextBrowser *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qtb->qobj->isForwardAvailable());
}

//virtual QVariant loadResource ( int type, const QUrl & name )
static AbstractQoreNode *QTEXTBROWSER_loadResource(QoreObject *self, QoreQTextBrowser *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int type = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreQUrl *name = (p && p->getType() == NT_OBJECT) ? (QoreQUrl *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QURL, xsink) : 0;
   if (!name) {
      if (!xsink->isException())
         xsink->raiseException("QTEXTBROWSER-LOADRESOURCE-PARAM-ERROR", "expecting a QUrl object as second argument to QTextBrowser::loadResource()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> nameHolder(static_cast<AbstractPrivateData *>(name), xsink);
   return return_qvariant(qtb->qobj->loadResource(type, *(static_cast<QUrl *>(name))));
}

//bool openExternalLinks () const
static AbstractQoreNode *QTEXTBROWSER_openExternalLinks(QoreObject *self, QoreQTextBrowser *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qtb->qobj->openExternalLinks());
}

//bool openLinks () const
static AbstractQoreNode *QTEXTBROWSER_openLinks(QoreObject *self, QoreQTextBrowser *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qtb->qobj->openLinks());
}

//QStringList searchPaths () const
static AbstractQoreNode *QTEXTBROWSER_searchPaths(QoreObject *self, QoreQTextBrowser *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_qstringlist(qtb->qobj->searchPaths());
}

//void setOpenExternalLinks ( bool open )
static AbstractQoreNode *QTEXTBROWSER_setOpenExternalLinks(QoreObject *self, QoreQTextBrowser *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool open = p ? p->getAsBool() : false;
   qtb->qobj->setOpenExternalLinks(open);
   return 0;
}

//void setOpenLinks ( bool open )
static AbstractQoreNode *QTEXTBROWSER_setOpenLinks(QoreObject *self, QoreQTextBrowser *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool open = p ? p->getAsBool() : false;
   qtb->qobj->setOpenLinks(open);
   return 0;
}

//void setSearchPaths ( const QStringList & paths )
static AbstractQoreNode *QTEXTBROWSER_setSearchPaths(QoreObject *self, QoreQTextBrowser *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QStringList paths;
   ConstListIterator li_paths(reinterpret_cast<const QoreListNode *>(p));
   while (li_paths.next()) {
      QoreStringNodeValueHelper str(li_paths.getValue());
      QString tmp;
      if (get_qstring(*str, tmp, xsink))
         return 0;
      paths.push_back(tmp);
   }
   qtb->qobj->setSearchPaths(paths);
   return 0;
}

//QUrl source () const
static AbstractQoreNode *QTEXTBROWSER_source(QoreObject *self, QoreQTextBrowser *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QUrl, new QoreQUrl(qtb->qobj->source()));
}

QoreClass *initQTextBrowserClass(QoreClass *qtextedit)
{
   QC_QTextBrowser = new QoreClass("QTextBrowser", QDOM_GUI);
   CID_QTEXTBROWSER = QC_QTextBrowser->getID();

   QC_QTextBrowser->addBuiltinVirtualBaseClass(qtextedit);

   QC_QTextBrowser->setConstructor(QTEXTBROWSER_constructor);
   QC_QTextBrowser->setCopy((q_copy_t)QTEXTBROWSER_copy);

   QC_QTextBrowser->addMethod("clearHistory",                (q_method_t)QTEXTBROWSER_clearHistory);
   QC_QTextBrowser->addMethod("isBackwardAvailable",         (q_method_t)QTEXTBROWSER_isBackwardAvailable);
   QC_QTextBrowser->addMethod("isForwardAvailable",          (q_method_t)QTEXTBROWSER_isForwardAvailable);
   QC_QTextBrowser->addMethod("loadResource",                (q_method_t)QTEXTBROWSER_loadResource);
   QC_QTextBrowser->addMethod("openExternalLinks",           (q_method_t)QTEXTBROWSER_openExternalLinks);
   QC_QTextBrowser->addMethod("openLinks",                   (q_method_t)QTEXTBROWSER_openLinks);
   QC_QTextBrowser->addMethod("searchPaths",                 (q_method_t)QTEXTBROWSER_searchPaths);
   QC_QTextBrowser->addMethod("setOpenExternalLinks",        (q_method_t)QTEXTBROWSER_setOpenExternalLinks);
   QC_QTextBrowser->addMethod("setOpenLinks",                (q_method_t)QTEXTBROWSER_setOpenLinks);
   QC_QTextBrowser->addMethod("setSearchPaths",              (q_method_t)QTEXTBROWSER_setSearchPaths);
   QC_QTextBrowser->addMethod("source",                      (q_method_t)QTEXTBROWSER_source);

   return QC_QTextBrowser;
}
