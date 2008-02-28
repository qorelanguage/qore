/*
 QC_QUrl.cc
 
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

#include "QC_QUrl.h"
#include "QC_QByteArray.h"

#include "qore-qt.h"

qore_classid_t CID_QURL;
class QoreClass *QC_QUrl = 0;

//QUrl ()
//QUrl ( const QString & url )
//QUrl ( const QUrl & other )
//QUrl ( const QString & url, ParsingMode parsingMode )
static void QURL_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QURL, new QoreQUrl());
      return;
   }
   QString url;
   if (get_qstring(p, url, xsink))
      return;

   p = get_param(params, 1);
   if (is_nothing(p))
      self->setPrivate(CID_QURL, new QoreQUrl(url));
   else {
      QUrl::ParsingMode parsingMode = (QUrl::ParsingMode)(p ? p->getAsInt() : 0);
      self->setPrivate(CID_QURL, new QoreQUrl(url, parsingMode));
   }
   return;
}

static void QURL_copy(class QoreObject *self, class QoreObject *old, class QoreQUrl *qu, ExceptionSink *xsink)
{
   self->setPrivate(CID_QURL, new QoreQUrl(*qu));
}

//void addQueryItem ( const QString & key, const QString & value )
static AbstractQoreNode *QURL_addQueryItem(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString key;

   if (get_qstring(p, key, xsink))
      return 0;

   p = get_param(params, 1);
   QString value;
   if (get_qstring(p, value, xsink))
      return 0;

   qu->addQueryItem(key, value);
   return 0;
}

//QStringList allQueryItemValues ( const QString & key ) const
static AbstractQoreNode *QURL_allQueryItemValues(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString key;
   if (get_qstring(p, key, xsink))
      return 0;
   QStringList strlist_rv = qu->allQueryItemValues(key);
   QoreListNode *l = new QoreListNode();
   for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)
      l->push(new QoreStringNode((*i).toUtf8().data(), QCS_UTF8));
   return l;
}

//QString authority () const
static AbstractQoreNode *QURL_authority(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qu->authority().toUtf8().data(), QCS_UTF8);
}

//void clear ()
static AbstractQoreNode *QURL_clear(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   qu->clear();
   return 0;
}

////DataPtr & data_ptr ()
//static AbstractQoreNode *QURL_data_ptr(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qu->data_ptr());
//}

//QByteArray encodedQuery () const
static AbstractQoreNode *QURL_encodedQuery(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qba = new QoreObject(QC_QByteArray, getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qu->encodedQuery());
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return o_qba;
}

//QString errorString () const
static AbstractQoreNode *QURL_errorString(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qu->errorString().toUtf8().data(), QCS_UTF8);
}

//QString fragment () const
static AbstractQoreNode *QURL_fragment(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qu->fragment().toUtf8().data(), QCS_UTF8);
}

//bool hasFragment () const
static AbstractQoreNode *QURL_hasFragment(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qu->hasFragment());
}

//bool hasQuery () const
static AbstractQoreNode *QURL_hasQuery(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qu->hasQuery());
}

//bool hasQueryItem ( const QString & key ) const
static AbstractQoreNode *QURL_hasQueryItem(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString key;
   if (get_qstring(p, key, xsink))
      return 0;
   return get_bool_node(qu->hasQueryItem(key));
}

//QString host () const
static AbstractQoreNode *QURL_host(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qu->host().toUtf8().data(), QCS_UTF8);
}

//bool isEmpty () const
static AbstractQoreNode *QURL_isEmpty(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qu->isEmpty());
}

//bool isParentOf ( const QUrl & childUrl ) const
static AbstractQoreNode *QURL_isParentOf(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQUrl *childUrl = p ? (QoreQUrl *)p->getReferencedPrivateData(CID_QURL, xsink) : 0;
   if (!childUrl) {
      if (!xsink->isException())
         xsink->raiseException("QURL-ISPARENTOF-PARAM-ERROR", "expecting a QUrl object as first argument to QUrl::isParentOf()");
      return 0;
   }
   ReferenceHolder<QoreQUrl> childUrlHolder(childUrl, xsink);
   return get_bool_node(qu->isParentOf(*(static_cast<QUrl *>(childUrl))));
}

//bool isRelative () const
static AbstractQoreNode *QURL_isRelative(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qu->isRelative());
}

//bool isValid () const
static AbstractQoreNode *QURL_isValid(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qu->isValid());
}

//QString password () const
static AbstractQoreNode *QURL_password(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qu->password().toUtf8().data(), QCS_UTF8);
}

//QString path () const
static AbstractQoreNode *QURL_path(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qu->path().toUtf8().data(), QCS_UTF8);
}

//int port () const
//int port ( int defaultPort ) const
static AbstractQoreNode *QURL_port(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      return new QoreBigIntNode(qu->port());
   }
   int defaultPort = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qu->port(defaultPort));
}

//QString queryItemValue ( const QString & key ) const
static AbstractQoreNode *QURL_queryItemValue(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString key;
   if (get_qstring(p, key, xsink))
      return 0;
   return new QoreStringNode(qu->queryItemValue(key).toUtf8().data(), QCS_UTF8);
}

////QList<QPair<QString, QString> > queryItems () const
//static AbstractQoreNode *QURL_queryItems(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qu->queryItems());
//}

//char queryPairDelimiter () const
static AbstractQoreNode *QURL_queryPairDelimiter(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const char c_rv = qu->queryPairDelimiter();
   QoreStringNode *rv_str = new QoreStringNode();
   rv_str->concat(c_rv);
   return rv_str;
}

//char queryValueDelimiter () const
static AbstractQoreNode *QURL_queryValueDelimiter(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const char c_rv = qu->queryValueDelimiter();
   QoreStringNode *rv_str = new QoreStringNode();
   rv_str->concat(c_rv);
   return rv_str;
}

//void removeAllQueryItems ( const QString & key )
static AbstractQoreNode *QURL_removeAllQueryItems(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString key;
   if (get_qstring(p, key, xsink))
      return 0;
   qu->removeAllQueryItems(key);
   return 0;
}

//void removeQueryItem ( const QString & key )
static AbstractQoreNode *QURL_removeQueryItem(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString key;
   if (get_qstring(p, key, xsink))
      return 0;
   qu->removeQueryItem(key);
   return 0;
}

//QUrl resolved ( const QUrl & relative ) const
static AbstractQoreNode *QURL_resolved(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQUrl *relative = p ? (QoreQUrl *)p->getReferencedPrivateData(CID_QURL, xsink) : 0;
   if (!relative) {
      if (!xsink->isException())
         xsink->raiseException("QURL-RESOLVED-PARAM-ERROR", "expecting a QUrl object as first argument to QUrl::resolved()");
      return 0;
   }
   ReferenceHolder<QoreQUrl> relativeHolder(relative, xsink);
   QoreObject *o_qu = new QoreObject(self->getClass(CID_QURL), getProgram());
   QoreQUrl *q_qu = new QoreQUrl(qu->resolved(*(static_cast<QUrl *>(relative))));
   o_qu->setPrivate(CID_QURL, q_qu);
   return o_qu;
}

//QString scheme () const
static AbstractQoreNode *QURL_scheme(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qu->scheme().toUtf8().data(), QCS_UTF8);
}

//void setAuthority ( const QString & authority )
static AbstractQoreNode *QURL_setAuthority(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString authority;
   if (get_qstring(p, authority, xsink))
      return 0;
   qu->setAuthority(authority);
   return 0;
}

//void setEncodedQuery ( const QByteArray & query )
static AbstractQoreNode *QURL_setEncodedQuery(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QByteArray query;
   if (get_qbytearray(p, query, xsink))
      return 0;
   qu->setEncodedQuery(query);
   return 0;
}

//void setEncodedUrl ( const QByteArray & encodedUrl )
//void setEncodedUrl ( const QByteArray & encodedUrl, ParsingMode parsingMode )
static AbstractQoreNode *QURL_setEncodedUrl(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QByteArray encodedUrl;
   if (get_qbytearray(p, encodedUrl, xsink))
      return 0;
   p = get_param(params, 1);
   QUrl::ParsingMode parsingMode = (QUrl::ParsingMode)(p ? p->getAsInt() : 0);
   qu->setEncodedUrl(encodedUrl, parsingMode);
   return 0;
}

//void setFragment ( const QString & fragment )
static AbstractQoreNode *QURL_setFragment(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString fragment;
   if (get_qstring(p, fragment, xsink))
      return 0;
   qu->setFragment(fragment);
   return 0;
}

//void setHost ( const QString & host )
static AbstractQoreNode *QURL_setHost(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString host;
   if (get_qstring(p, host, xsink))
      return 0;
   qu->setHost(host);
   return 0;
}

//void setPassword ( const QString & password )
static AbstractQoreNode *QURL_setPassword(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString password;
   if (get_qstring(p, password, xsink))
      return 0;
   qu->setPassword(password);
   return 0;
}

//void setPath ( const QString & path )
static AbstractQoreNode *QURL_setPath(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString path;
   if (get_qstring(p, path, xsink))
      return 0;
   qu->setPath(path);
   return 0;
}

//void setPort ( int port )
static AbstractQoreNode *QURL_setPort(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int port = p ? p->getAsInt() : 0;
   qu->setPort(port);
   return 0;
}

//void setQueryDelimiters ( char valueDelimiter, char pairDelimiter )
static AbstractQoreNode *QURL_setQueryDelimiters(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str) {
      xsink->raiseException("QURL-SETQUERYDELIMITERS-ERROR", "expecting a string as first argument");
      return 0;
   }
   char valueDelimiter = str->getBuffer()[0];
   str = test_string_param(params, 1);
   if (!str) {
      xsink->raiseException("QURL-SETQUERYDELIMITERS-ERROR", "expecting a string as second argument");
      return 0;
   }

   char pairDelimiter = str->getBuffer()[0];
   qu->setQueryDelimiters(valueDelimiter, pairDelimiter);
   return 0;
}

////void setQueryItems ( const QList<QPair<QString, QString> > & query )
//static AbstractQoreNode *QURL_setQueryItems(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   QUrl::const^QList<QPair<QString const^qlist<qpair<qstring = (QUrl::const^QList<QPair<QString)(p ? p->getAsInt() : 0);
//   p = get_param(params, 1);
//   ??? > query = p;
//   qu->setQueryItems(const^qlist<qpair<qstring, query);
//   return 0;
//}

//void setScheme ( const QString & scheme )
static AbstractQoreNode *QURL_setScheme(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString scheme;
   if (get_qstring(p, scheme, xsink))
      return 0;
   qu->setScheme(scheme);
   return 0;
}

//void setUrl ( const QString & url )
//void setUrl ( const QString & url, ParsingMode parsingMode )
static AbstractQoreNode *QURL_setUrl(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString url;
   if (get_qstring(p, url, xsink))
      return 0;
   p = get_param(params, 1);
   if (is_nothing(p))
      qu->setUrl(url);
   else {
      QUrl::ParsingMode parsingMode = (QUrl::ParsingMode)(p ? p->getAsInt() : 0);
      qu->setUrl(url, parsingMode);
   }
   return 0;
}

//void setUserInfo ( const QString & userInfo )
static AbstractQoreNode *QURL_setUserInfo(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString userInfo;
   if (get_qstring(p, userInfo, xsink))
      return 0;
   qu->setUserInfo(userInfo);
   return 0;
}

//void setUserName ( const QString & userName )
static AbstractQoreNode *QURL_setUserName(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString userName;
   if (get_qstring(p, userName, xsink))
      return 0;
   qu->setUserName(userName);
   return 0;
}

//QByteArray toEncoded ( FormattingOptions options = None ) const
static AbstractQoreNode *QURL_toEncoded(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QUrl::FormattingOptions options = (QUrl::FormattingOptions)(p ? p->getAsInt() : 0);
   QoreObject *o_qba = new QoreObject(QC_QByteArray, getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qu->toEncoded(options));
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return o_qba;
}

//QString toLocalFile () const
static AbstractQoreNode *QURL_toLocalFile(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qu->toLocalFile().toUtf8().data(), QCS_UTF8);
}

//QString toString ( FormattingOptions options = None ) const
static AbstractQoreNode *QURL_toString(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QUrl::FormattingOptions options = (QUrl::FormattingOptions)(p ? p->getAsInt() : 0);
   return new QoreStringNode(qu->toString(options).toUtf8().data(), QCS_UTF8);
}

//QString userInfo () const
static AbstractQoreNode *QURL_userInfo(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qu->userInfo().toUtf8().data(), QCS_UTF8);
}

//QString userName () const
static AbstractQoreNode *QURL_userName(QoreObject *self, QoreQUrl *qu, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qu->userName().toUtf8().data(), QCS_UTF8);
}

QoreClass *initQUrlClass()
{
   QC_QUrl = new QoreClass("QUrl", QDOM_GUI);
   CID_QURL = QC_QUrl->getID();

   QC_QUrl->setConstructor(QURL_constructor);
   QC_QUrl->setCopy((q_copy_t)QURL_copy);

   QC_QUrl->addMethod("addQueryItem",                (q_method_t)QURL_addQueryItem);
   QC_QUrl->addMethod("allQueryItemValues",          (q_method_t)QURL_allQueryItemValues);
   QC_QUrl->addMethod("authority",                   (q_method_t)QURL_authority);
   QC_QUrl->addMethod("clear",                       (q_method_t)QURL_clear);
   //QC_QUrl->addMethod("data_ptr",                    (q_method_t)QURL_data_ptr);
   QC_QUrl->addMethod("encodedQuery",                (q_method_t)QURL_encodedQuery);
   QC_QUrl->addMethod("errorString",                 (q_method_t)QURL_errorString);
   QC_QUrl->addMethod("fragment",                    (q_method_t)QURL_fragment);
   QC_QUrl->addMethod("hasFragment",                 (q_method_t)QURL_hasFragment);
   QC_QUrl->addMethod("hasQuery",                    (q_method_t)QURL_hasQuery);
   QC_QUrl->addMethod("hasQueryItem",                (q_method_t)QURL_hasQueryItem);
   QC_QUrl->addMethod("host",                        (q_method_t)QURL_host);
   QC_QUrl->addMethod("isEmpty",                     (q_method_t)QURL_isEmpty);
   QC_QUrl->addMethod("isParentOf",                  (q_method_t)QURL_isParentOf);
   QC_QUrl->addMethod("isRelative",                  (q_method_t)QURL_isRelative);
   QC_QUrl->addMethod("isValid",                     (q_method_t)QURL_isValid);
   QC_QUrl->addMethod("password",                    (q_method_t)QURL_password);
   QC_QUrl->addMethod("path",                        (q_method_t)QURL_path);
   QC_QUrl->addMethod("port",                        (q_method_t)QURL_port);
   QC_QUrl->addMethod("queryItemValue",              (q_method_t)QURL_queryItemValue);
   //QC_QUrl->addMethod("queryItems",                  (q_method_t)QURL_queryItems);
   QC_QUrl->addMethod("queryPairDelimiter",          (q_method_t)QURL_queryPairDelimiter);
   QC_QUrl->addMethod("queryValueDelimiter",         (q_method_t)QURL_queryValueDelimiter);
   QC_QUrl->addMethod("removeAllQueryItems",         (q_method_t)QURL_removeAllQueryItems);
   QC_QUrl->addMethod("removeQueryItem",             (q_method_t)QURL_removeQueryItem);
   QC_QUrl->addMethod("resolved",                    (q_method_t)QURL_resolved);
   QC_QUrl->addMethod("scheme",                      (q_method_t)QURL_scheme);
   QC_QUrl->addMethod("setAuthority",                (q_method_t)QURL_setAuthority);
   QC_QUrl->addMethod("setEncodedQuery",             (q_method_t)QURL_setEncodedQuery);
   QC_QUrl->addMethod("setEncodedUrl",               (q_method_t)QURL_setEncodedUrl);
   QC_QUrl->addMethod("setFragment",                 (q_method_t)QURL_setFragment);
   QC_QUrl->addMethod("setHost",                     (q_method_t)QURL_setHost);
   QC_QUrl->addMethod("setPassword",                 (q_method_t)QURL_setPassword);
   QC_QUrl->addMethod("setPath",                     (q_method_t)QURL_setPath);
   QC_QUrl->addMethod("setPort",                     (q_method_t)QURL_setPort);
   QC_QUrl->addMethod("setQueryDelimiters",          (q_method_t)QURL_setQueryDelimiters);
   //QC_QUrl->addMethod("setQueryItems",               (q_method_t)QURL_setQueryItems);
   QC_QUrl->addMethod("setScheme",                   (q_method_t)QURL_setScheme);
   QC_QUrl->addMethod("setUrl",                      (q_method_t)QURL_setUrl);
   QC_QUrl->addMethod("setUserInfo",                 (q_method_t)QURL_setUserInfo);
   QC_QUrl->addMethod("setUserName",                 (q_method_t)QURL_setUserName);
   QC_QUrl->addMethod("toEncoded",                   (q_method_t)QURL_toEncoded);
   QC_QUrl->addMethod("toLocalFile",                 (q_method_t)QURL_toLocalFile);
   QC_QUrl->addMethod("toString",                    (q_method_t)QURL_toString);
   QC_QUrl->addMethod("userInfo",                    (q_method_t)QURL_userInfo);
   QC_QUrl->addMethod("userName",                    (q_method_t)QURL_userName);

   return QC_QUrl;
}
