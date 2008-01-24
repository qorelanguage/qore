/*
 QC_QRegExp.cc
 
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

#include "QC_QRegExp.h"

#include "qore-qt.h"

int CID_QREGEXP;
class QoreClass *QC_QRegExp = 0;

//QRegExp ()
//QRegExp ( const QString & pattern, Qt::CaseSensitivity cs = Qt::CaseSensitive, PatternSyntax syntax = RegExp )
//QRegExp ( const QRegExp & rx )
static void QREGEXP_constructor(QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QREGEXP, new QoreQRegExp());
      return;
   }
   QString pattern;
   if (get_qstring(p, pattern, xsink))
      return;
   p = get_param(params, 1);
   Qt::CaseSensitivity cs = !is_nothing(p) ? (Qt::CaseSensitivity)p->getAsInt() : Qt::CaseSensitive;
   p = get_param(params, 2);
   QRegExp::PatternSyntax syntax = !is_nothing(p) ? (QRegExp::PatternSyntax)p->getAsInt() : QRegExp::RegExp;
   self->setPrivate(CID_QREGEXP, new QoreQRegExp(pattern, cs, syntax));
   return;
}

static void QREGEXP_copy(class QoreObject *self, class QoreObject *old, class QoreQRegExp *qre, ExceptionSink *xsink)
{
   self->setPrivate(CID_QREGEXP, new QoreQRegExp(*qre));
}

//QString cap ( int nth = 0 )
static QoreNode *QREGEXP_cap(QoreObject *self, QoreQRegExp *qre, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int nth = !is_nothing(p) ? p->getAsInt() : 0;
   return new QoreStringNode(qre->cap(nth).toUtf8().data(), QCS_UTF8);
}

//QStringList capturedTexts ()
static QoreNode *QREGEXP_capturedTexts(QoreObject *self, QoreQRegExp *qre, const QoreList *params, ExceptionSink *xsink)
{
   QStringList strlist_rv = qre->capturedTexts();
   QoreList *l = new QoreList();
   for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)
      l->push(new QoreStringNode((*i).toUtf8().data(), QCS_UTF8));
   return l;
}

//Qt::CaseSensitivity caseSensitivity () const
static QoreNode *QREGEXP_caseSensitivity(QoreObject *self, QoreQRegExp *qre, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qre->caseSensitivity());
}

//QString errorString ()
static QoreNode *QREGEXP_errorString(QoreObject *self, QoreQRegExp *qre, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qre->errorString().toUtf8().data(), QCS_UTF8);
}

//bool exactMatch ( const QString & str ) const
static QoreNode *QREGEXP_exactMatch(QoreObject *self, QoreQRegExp *qre, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString str;
   if (get_qstring(p, str, xsink))
      return 0;
   return new QoreNode(qre->exactMatch(str));
}

//int indexIn ( const QString & str, int offset = 0, CaretMode caretMode = CaretAtZero ) const
static QoreNode *QREGEXP_indexIn(QoreObject *self, QoreQRegExp *qre, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString str;
   if (get_qstring(p, str, xsink))
      return 0;
   p = get_param(params, 1);
   int offset = !is_nothing(p) ? p->getAsInt() : 0;
   p = get_param(params, 2);
   QRegExp::CaretMode caretMode = !is_nothing(p) ? (QRegExp::CaretMode)p->getAsInt() : QRegExp::CaretAtZero;
   return new QoreNode((int64)qre->indexIn(str, offset, caretMode));
}

//bool isEmpty () const
static QoreNode *QREGEXP_isEmpty(QoreObject *self, QoreQRegExp *qre, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qre->isEmpty());
}

//bool isMinimal () const
static QoreNode *QREGEXP_isMinimal(QoreObject *self, QoreQRegExp *qre, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qre->isMinimal());
}

//bool isValid () const
static QoreNode *QREGEXP_isValid(QoreObject *self, QoreQRegExp *qre, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qre->isValid());
}

//int lastIndexIn ( const QString & str, int offset = -1, CaretMode caretMode = CaretAtZero ) const
static QoreNode *QREGEXP_lastIndexIn(QoreObject *self, QoreQRegExp *qre, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString str;
   if (get_qstring(p, str, xsink))
      return 0;
   p = get_param(params, 1);
   int offset = !is_nothing(p) ? p->getAsInt() : -1;
   p = get_param(params, 2);
   QRegExp::CaretMode caretMode = !is_nothing(p) ? (QRegExp::CaretMode)p->getAsInt() : QRegExp::CaretAtZero;
   return new QoreNode((int64)qre->lastIndexIn(str, offset, caretMode));
}

//int matchedLength () const
static QoreNode *QREGEXP_matchedLength(QoreObject *self, QoreQRegExp *qre, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qre->matchedLength());
}

//int numCaptures () const
static QoreNode *QREGEXP_numCaptures(QoreObject *self, QoreQRegExp *qre, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qre->numCaptures());
}

//QString pattern () const
static QoreNode *QREGEXP_pattern(QoreObject *self, QoreQRegExp *qre, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qre->pattern().toUtf8().data(), QCS_UTF8);
}

//PatternSyntax patternSyntax () const
static QoreNode *QREGEXP_patternSyntax(QoreObject *self, QoreQRegExp *qre, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qre->patternSyntax());
}

//int pos ( int nth = 0 )
static QoreNode *QREGEXP_pos(QoreObject *self, QoreQRegExp *qre, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int nth = !is_nothing(p) ? p->getAsInt() : 0;
   return new QoreNode((int64)qre->pos(nth));
}

//void setCaseSensitivity ( Qt::CaseSensitivity cs )
static QoreNode *QREGEXP_setCaseSensitivity(QoreObject *self, QoreQRegExp *qre, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::CaseSensitivity cs = (Qt::CaseSensitivity)(p ? p->getAsInt() : 0);
   qre->setCaseSensitivity(cs);
   return 0;
}

//void setMinimal ( bool minimal )
static QoreNode *QREGEXP_setMinimal(QoreObject *self, QoreQRegExp *qre, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool minimal = p ? p->getAsBool() : false;
   qre->setMinimal(minimal);
   return 0;
}

//void setPattern ( const QString & pattern )
static QoreNode *QREGEXP_setPattern(QoreObject *self, QoreQRegExp *qre, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString pattern;
   if (get_qstring(p, pattern, xsink))
      return 0;
   qre->setPattern(pattern);
   return 0;
}

//void setPatternSyntax ( PatternSyntax syntax )
static QoreNode *QREGEXP_setPatternSyntax(QoreObject *self, QoreQRegExp *qre, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QRegExp::PatternSyntax syntax = (QRegExp::PatternSyntax)(p ? p->getAsInt() : 0);
   qre->setPatternSyntax(syntax);
   return 0;
}

QoreClass *initQRegExpClass()
{
   QC_QRegExp = new QoreClass("QRegExp", QDOM_GUI);
   CID_QREGEXP = QC_QRegExp->getID();

   QC_QRegExp->setConstructor(QREGEXP_constructor);
   QC_QRegExp->setCopy((q_copy_t)QREGEXP_copy);

   QC_QRegExp->addMethod("cap",                         (q_method_t)QREGEXP_cap);
   QC_QRegExp->addMethod("capturedTexts",               (q_method_t)QREGEXP_capturedTexts);
   QC_QRegExp->addMethod("caseSensitivity",             (q_method_t)QREGEXP_caseSensitivity);
   QC_QRegExp->addMethod("errorString",                 (q_method_t)QREGEXP_errorString);
   QC_QRegExp->addMethod("exactMatch",                  (q_method_t)QREGEXP_exactMatch);
   QC_QRegExp->addMethod("indexIn",                     (q_method_t)QREGEXP_indexIn);
   QC_QRegExp->addMethod("isEmpty",                     (q_method_t)QREGEXP_isEmpty);
   QC_QRegExp->addMethod("isMinimal",                   (q_method_t)QREGEXP_isMinimal);
   QC_QRegExp->addMethod("isValid",                     (q_method_t)QREGEXP_isValid);
   QC_QRegExp->addMethod("lastIndexIn",                 (q_method_t)QREGEXP_lastIndexIn);
   QC_QRegExp->addMethod("matchedLength",               (q_method_t)QREGEXP_matchedLength);
   QC_QRegExp->addMethod("numCaptures",                 (q_method_t)QREGEXP_numCaptures);
   QC_QRegExp->addMethod("pattern",                     (q_method_t)QREGEXP_pattern);
   QC_QRegExp->addMethod("patternSyntax",               (q_method_t)QREGEXP_patternSyntax);
   QC_QRegExp->addMethod("pos",                         (q_method_t)QREGEXP_pos);
   QC_QRegExp->addMethod("setCaseSensitivity",          (q_method_t)QREGEXP_setCaseSensitivity);
   QC_QRegExp->addMethod("setMinimal",                  (q_method_t)QREGEXP_setMinimal);
   QC_QRegExp->addMethod("setPattern",                  (q_method_t)QREGEXP_setPattern);
   QC_QRegExp->addMethod("setPatternSyntax",            (q_method_t)QREGEXP_setPatternSyntax);

   return QC_QRegExp;
}
