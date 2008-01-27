/*
 QC_QIODevice.cc
 
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

#include "QC_QIODevice.h"
#include "QC_QByteArray.h"

#include "qore-qt.h"

int CID_QIODEVICE;
class QoreClass *QC_QIODevice = 0;

//QIODevice ()
//QIODevice ( QObject * parent )
static void QIODEVICE_constructor(QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   xsink->raiseException("QIODEVICE-CONSTRUCTOR-ERROR", "QIODevice is an abstract class");
}

static void QIODEVICE_copy(class QoreObject *self, class QoreObject *old, class QoreQIODevice *qiod, ExceptionSink *xsink)
{
   xsink->raiseException("QIODEVICE-COPY-ERROR", "objects of this class cannot be copied");
}

//virtual bool atEnd () const
static QoreNode *QIODEVICE_atEnd(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qiod->getQIODevice()->atEnd());
}

//virtual qint64 bytesAvailable () const
static QoreNode *QIODEVICE_bytesAvailable(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qiod->getQIODevice()->bytesAvailable());
}

//virtual qint64 bytesToWrite () const
static QoreNode *QIODEVICE_bytesToWrite(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qiod->getQIODevice()->bytesToWrite());
}

//virtual bool canReadLine () const
static QoreNode *QIODEVICE_canReadLine(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qiod->getQIODevice()->canReadLine());
}

//virtual void close ()
static QoreNode *QIODEVICE_close(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   qiod->getQIODevice()->close();
   return 0;
}

//QString errorString () const
static QoreNode *QIODEVICE_errorString(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qiod->getQIODevice()->errorString().toUtf8().data(), QCS_UTF8);
}

/*
//bool getChar ( char * c )
static QoreNode *QIODEVICE_getChar(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p) {
      xsink->raiseException("QIODEVICE-GETCHAR-PARAM-ERROR", "expecting a string as first argument to QIODevice::getChar()");
      return 0;
   }
   const char *c = p->getBuffer();
   return new QoreNode(qiod->getQIODevice()->getChar(c));
}
*/

//bool isOpen () const
static QoreNode *QIODEVICE_isOpen(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qiod->getQIODevice()->isOpen());
}

//bool isReadable () const
static QoreNode *QIODEVICE_isReadable(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qiod->getQIODevice()->isReadable());
}

//virtual bool isSequential () const
static QoreNode *QIODEVICE_isSequential(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qiod->getQIODevice()->isSequential());
}

//bool isTextModeEnabled () const
static QoreNode *QIODEVICE_isTextModeEnabled(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qiod->getQIODevice()->isTextModeEnabled());
}

//bool isWritable () const
static QoreNode *QIODEVICE_isWritable(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qiod->getQIODevice()->isWritable());
}

//virtual bool open ( OpenMode mode )
static QoreNode *QIODEVICE_open(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QIODevice::OpenMode mode = (QIODevice::OpenMode)(p ? p->getAsInt() : 0);
   return new QoreNode(qiod->getQIODevice()->open(mode));
}

//OpenMode openMode () const
static QoreNode *QIODEVICE_openMode(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qiod->getQIODevice()->openMode());
}

//QByteArray peek ( qint64 maxSize )
static QoreNode *QIODEVICE_peek(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int64 maxSize = p ? p->getAsBigInt() : 0;
   QoreObject *o_qba = new QoreObject(QC_QByteArray, getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qiod->getQIODevice()->peek(maxSize));
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return o_qba;
}

//virtual qint64 pos () const
static QoreNode *QIODEVICE_pos(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qiod->getQIODevice()->pos());
}

//bool putChar ( char c )
static QoreNode *QIODEVICE_putChar(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p) {
      xsink->raiseException("QIODEVICE-PUTCHAR-PARAM-ERROR", "expecting a string as first argument to QIODevice::putChar()");
      return 0;
   }
   const char c = p->getBuffer()[0];
   return new QoreNode(qiod->getQIODevice()->putChar(c));
}

//QByteArray read ( qint64 maxSize )
static QoreNode *QIODEVICE_read(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int64 maxSize = p ? p->getAsBigInt() : 0;
   QoreObject *o_qba = new QoreObject(QC_QByteArray, getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qiod->getQIODevice()->read(maxSize));
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return o_qba;
}

//QByteArray readAll ()
static QoreNode *QIODEVICE_readAll(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_qba = new QoreObject(QC_QByteArray, getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qiod->getQIODevice()->readAll());
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return o_qba;
}

//QByteArray readLine ( qint64 maxSize = 0 )
static QoreNode *QIODEVICE_readLine(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int64 maxSize = !is_nothing(p) ? p->getAsBigInt() : 0;
   QoreObject *o_qba = new QoreObject(QC_QByteArray, getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qiod->getQIODevice()->readLine(maxSize));
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return o_qba;
}

//virtual bool reset ()
static QoreNode *QIODEVICE_reset(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qiod->getQIODevice()->reset());
}

//virtual bool seek ( qint64 pos )
static QoreNode *QIODEVICE_seek(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int64 pos = p ? p->getAsBigInt() : 0;
   return new QoreNode(qiod->getQIODevice()->seek(pos));
}

//void setTextModeEnabled ( bool enabled )
static QoreNode *QIODEVICE_setTextModeEnabled(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enabled = p ? p->getAsBool() : false;
   qiod->getQIODevice()->setTextModeEnabled(enabled);
   return 0;
}

//virtual qint64 size () const
static QoreNode *QIODEVICE_size(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qiod->getQIODevice()->size());
}

//void ungetChar ( char c )
static QoreNode *QIODEVICE_ungetChar(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p) {
      xsink->raiseException("QIODEVICE-UNGETCHAR-PARAM-ERROR", "expecting a string as first argument to QIODevice::ungetChar()");
      return 0;
   }
   const char c = p->getBuffer()[0];
   qiod->getQIODevice()->ungetChar(c);
   return 0;
}

//virtual bool waitForBytesWritten ( int msecs )
static QoreNode *QIODEVICE_waitForBytesWritten(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int msecs = p ? p->getAsInt() : 0;
   return new QoreNode(qiod->getQIODevice()->waitForBytesWritten(msecs));
}

//virtual bool waitForReadyRead ( int msecs )
static QoreNode *QIODEVICE_waitForReadyRead(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int msecs = p ? p->getAsInt() : 0;
   return new QoreNode(qiod->getQIODevice()->waitForReadyRead(msecs));
}

//qint64 write ( const QByteArray & byteArray )
static QoreNode *QIODEVICE_write(QoreObject *self, QoreAbstractQIODevice *qiod, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QByteArray byteArray;
   if (get_qbytearray(p, byteArray, xsink))
      return 0;
   return new QoreNode((int64)qiod->getQIODevice()->write(byteArray));
}

QoreClass *initQIODeviceClass(QoreClass *qobject)
{
   QC_QIODevice = new QoreClass("QIODevice", QDOM_GUI);
   CID_QIODEVICE = QC_QIODevice->getID();

   QC_QIODevice->addBuiltinVirtualBaseClass(qobject);

   QC_QIODevice->setConstructor(QIODEVICE_constructor);
   QC_QIODevice->setCopy((q_copy_t)QIODEVICE_copy);

   QC_QIODevice->addMethod("atEnd",                       (q_method_t)QIODEVICE_atEnd);
   QC_QIODevice->addMethod("bytesAvailable",              (q_method_t)QIODEVICE_bytesAvailable);
   QC_QIODevice->addMethod("bytesToWrite",                (q_method_t)QIODEVICE_bytesToWrite);
   QC_QIODevice->addMethod("canReadLine",                 (q_method_t)QIODEVICE_canReadLine);
   QC_QIODevice->addMethod("close",                       (q_method_t)QIODEVICE_close);
   QC_QIODevice->addMethod("errorString",                 (q_method_t)QIODEVICE_errorString);
   //QC_QIODevice->addMethod("getChar",                     (q_method_t)QIODEVICE_getChar);
   QC_QIODevice->addMethod("isOpen",                      (q_method_t)QIODEVICE_isOpen);
   QC_QIODevice->addMethod("isReadable",                  (q_method_t)QIODEVICE_isReadable);
   QC_QIODevice->addMethod("isSequential",                (q_method_t)QIODEVICE_isSequential);
   QC_QIODevice->addMethod("isTextModeEnabled",           (q_method_t)QIODEVICE_isTextModeEnabled);
   QC_QIODevice->addMethod("isWritable",                  (q_method_t)QIODEVICE_isWritable);
   QC_QIODevice->addMethod("open",                        (q_method_t)QIODEVICE_open);
   QC_QIODevice->addMethod("openMode",                    (q_method_t)QIODEVICE_openMode);
   QC_QIODevice->addMethod("peek",                        (q_method_t)QIODEVICE_peek);
   QC_QIODevice->addMethod("pos",                         (q_method_t)QIODEVICE_pos);
   QC_QIODevice->addMethod("putChar",                     (q_method_t)QIODEVICE_putChar);
   QC_QIODevice->addMethod("read",                        (q_method_t)QIODEVICE_read);
   QC_QIODevice->addMethod("readAll",                     (q_method_t)QIODEVICE_readAll);
   QC_QIODevice->addMethod("readLine",                    (q_method_t)QIODEVICE_readLine);
   QC_QIODevice->addMethod("reset",                       (q_method_t)QIODEVICE_reset);
   QC_QIODevice->addMethod("seek",                        (q_method_t)QIODEVICE_seek);
   QC_QIODevice->addMethod("setTextModeEnabled",          (q_method_t)QIODEVICE_setTextModeEnabled);
   QC_QIODevice->addMethod("size",                        (q_method_t)QIODEVICE_size);
   QC_QIODevice->addMethod("ungetChar",                   (q_method_t)QIODEVICE_ungetChar);
   QC_QIODevice->addMethod("waitForBytesWritten",         (q_method_t)QIODEVICE_waitForBytesWritten);
   QC_QIODevice->addMethod("waitForReadyRead",            (q_method_t)QIODEVICE_waitForReadyRead);
   QC_QIODevice->addMethod("write",                       (q_method_t)QIODEVICE_write);

   return QC_QIODevice;
}
