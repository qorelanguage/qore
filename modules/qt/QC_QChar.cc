/*
 QC_QChar.cc
 
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

#include "QC_QChar.h"

int CID_QCHAR;
class QoreClass *QC_QChar = 0;

//QChar ()
//QChar ( char ch )
//QChar ( int code )
//QChar ( SpecialCharacter ch )
static void QCHAR_constructor(QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QCHAR, new QoreQChar());
      return;
   }
   int code;
   if (p->type == NT_STRING) {
      code = p->val.String->getUnicodePoint(0, xsink);
      if (*xsink)
	 return;
   }
   else
      code = p ? p->getAsInt() : 0;

   self->setPrivate(CID_QCHAR, new QoreQChar(code));   
}

static void QCHAR_copy(class QoreObject *self, class QoreObject *old, class QoreQChar *qc, ExceptionSink *xsink)
{
   xsink->raiseException("QCHAR-COPY-ERROR", "objects of this class cannot be copied");
}

//Category category () const
static QoreNode *QCHAR_category(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qc->category());
}

//uchar cell () const
static QoreNode *QCHAR_cell(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qc->cell());
}

//unsigned char combiningClass () const
static QoreNode *QCHAR_combiningClass(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   const char c_rv = qc->combiningClass();
   QoreString *rv_str = new QoreString();
   rv_str->concat(c_rv);
   return new QoreNode(rv_str);
}

//QString decomposition () const
static QoreNode *QCHAR_decomposition(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qc->decomposition().toUtf8().data(), QCS_UTF8));
}

//Decomposition decompositionTag () const
static QoreNode *QCHAR_decompositionTag(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qc->decompositionTag());
}

//int digitValue () const
static QoreNode *QCHAR_digitValue(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qc->digitValue());
}

//Direction direction () const
static QoreNode *QCHAR_direction(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qc->direction());
}

//bool hasMirrored () const
static QoreNode *QCHAR_hasMirrored(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->hasMirrored());
}

//bool isDigit () const
static QoreNode *QCHAR_isDigit(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->isDigit());
}

//bool isHighSurrogate () const
static QoreNode *QCHAR_isHighSurrogate(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->isHighSurrogate());
}

//bool isLetter () const
static QoreNode *QCHAR_isLetter(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->isLetter());
}

//bool isLetterOrNumber () const
static QoreNode *QCHAR_isLetterOrNumber(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->isLetterOrNumber());
}

//bool isLowSurrogate () const
static QoreNode *QCHAR_isLowSurrogate(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->isLowSurrogate());
}

//bool isLower () const
static QoreNode *QCHAR_isLower(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->isLower());
}

//bool isMark () const
static QoreNode *QCHAR_isMark(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->isMark());
}

//bool isNull () const
static QoreNode *QCHAR_isNull(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->isNull());
}

//bool isNumber () const
static QoreNode *QCHAR_isNumber(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->isNumber());
}

//bool isPrint () const
static QoreNode *QCHAR_isPrint(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->isPrint());
}

//bool isPunct () const
static QoreNode *QCHAR_isPunct(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->isPunct());
}

//bool isSpace () const
static QoreNode *QCHAR_isSpace(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->isSpace());
}

//bool isSymbol () const
static QoreNode *QCHAR_isSymbol(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->isSymbol());
}

//bool isTitleCase () const
static QoreNode *QCHAR_isTitleCase(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->isTitleCase());
}

//bool isUpper () const
static QoreNode *QCHAR_isUpper(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->isUpper());
}

//Joining joining () const
static QoreNode *QCHAR_joining(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qc->joining());
}

//QChar mirroredChar () const
static QoreNode *QCHAR_mirroredChar(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   QoreString *rv_str = new QoreString(QCS_UTF8);
   QChar rv_qc = qc->mirroredChar();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return new QoreNode(rv_str);
}

//uchar row () const
static QoreNode *QCHAR_row(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qc->row());
}

//const char toAscii () const
static QoreNode *QCHAR_toAscii(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   const char c_rv = qc->toAscii();
   QoreString *rv_str = new QoreString();
   rv_str->concat(c_rv);
   return new QoreNode(rv_str);
}

//QChar toCaseFolded () const
static QoreNode *QCHAR_toCaseFolded(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   QoreString *rv_str = new QoreString(QCS_UTF8);
   QChar rv_qc = qc->toCaseFolded();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return new QoreNode(rv_str);
}

//const char toLatin1 () const
static QoreNode *QCHAR_toLatin1(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   const char c_rv = qc->toLatin1();
   QoreString *rv_str = new QoreString();
   rv_str->concat(c_rv);
   return new QoreNode(rv_str);
}

//QChar toLower () const
static QoreNode *QCHAR_toLower(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   QoreString *rv_str = new QoreString(QCS_UTF8);
   QChar rv_qc = qc->toLower();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return new QoreNode(rv_str);
}

//QChar toTitleCase () const
static QoreNode *QCHAR_toTitleCase(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   QoreString *rv_str = new QoreString(QCS_UTF8);
   QChar rv_qc = qc->toTitleCase();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return new QoreNode(rv_str);
}

//QChar toUpper () const
static QoreNode *QCHAR_toUpper(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   QoreString *rv_str = new QoreString(QCS_UTF8);
   QChar rv_qc = qc->toUpper();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return new QoreNode(rv_str);
}

//ushort & unicode ()
//const ushort unicode () const
static QoreNode *QCHAR_unicode(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qc->unicode());
}

//UnicodeVersion unicodeVersion () const
static QoreNode *QCHAR_unicodeVersion(QoreObject *self, QoreQChar *qc, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qc->unicodeVersion());
}

QoreClass *initQCharClass()
{
   QC_QChar = new QoreClass("QChar", QDOM_GUI);
   CID_QCHAR = QC_QChar->getID();

   QC_QChar->setConstructor(QCHAR_constructor);
   QC_QChar->setCopy((q_copy_t)QCHAR_copy);

   QC_QChar->addMethod("category",                    (q_method_t)QCHAR_category);
   QC_QChar->addMethod("cell",                        (q_method_t)QCHAR_cell);
   QC_QChar->addMethod("combiningClass",              (q_method_t)QCHAR_combiningClass);
   QC_QChar->addMethod("decomposition",               (q_method_t)QCHAR_decomposition);
   QC_QChar->addMethod("decompositionTag",            (q_method_t)QCHAR_decompositionTag);
   QC_QChar->addMethod("digitValue",                  (q_method_t)QCHAR_digitValue);
   QC_QChar->addMethod("direction",                   (q_method_t)QCHAR_direction);
   QC_QChar->addMethod("hasMirrored",                 (q_method_t)QCHAR_hasMirrored);
   QC_QChar->addMethod("isDigit",                     (q_method_t)QCHAR_isDigit);
   QC_QChar->addMethod("isHighSurrogate",             (q_method_t)QCHAR_isHighSurrogate);
   QC_QChar->addMethod("isLetter",                    (q_method_t)QCHAR_isLetter);
   QC_QChar->addMethod("isLetterOrNumber",            (q_method_t)QCHAR_isLetterOrNumber);
   QC_QChar->addMethod("isLowSurrogate",              (q_method_t)QCHAR_isLowSurrogate);
   QC_QChar->addMethod("isLower",                     (q_method_t)QCHAR_isLower);
   QC_QChar->addMethod("isMark",                      (q_method_t)QCHAR_isMark);
   QC_QChar->addMethod("isNull",                      (q_method_t)QCHAR_isNull);
   QC_QChar->addMethod("isNumber",                    (q_method_t)QCHAR_isNumber);
   QC_QChar->addMethod("isPrint",                     (q_method_t)QCHAR_isPrint);
   QC_QChar->addMethod("isPunct",                     (q_method_t)QCHAR_isPunct);
   QC_QChar->addMethod("isSpace",                     (q_method_t)QCHAR_isSpace);
   QC_QChar->addMethod("isSymbol",                    (q_method_t)QCHAR_isSymbol);
   QC_QChar->addMethod("isTitleCase",                 (q_method_t)QCHAR_isTitleCase);
   QC_QChar->addMethod("isUpper",                     (q_method_t)QCHAR_isUpper);
   QC_QChar->addMethod("joining",                     (q_method_t)QCHAR_joining);
   QC_QChar->addMethod("mirroredChar",                (q_method_t)QCHAR_mirroredChar);
   QC_QChar->addMethod("row",                         (q_method_t)QCHAR_row);
   QC_QChar->addMethod("toAscii",                     (q_method_t)QCHAR_toAscii);
   QC_QChar->addMethod("toCaseFolded",                (q_method_t)QCHAR_toCaseFolded);
   QC_QChar->addMethod("toLatin1",                    (q_method_t)QCHAR_toLatin1);
   QC_QChar->addMethod("toLower",                     (q_method_t)QCHAR_toLower);
   QC_QChar->addMethod("toTitleCase",                 (q_method_t)QCHAR_toTitleCase);
   QC_QChar->addMethod("toUpper",                     (q_method_t)QCHAR_toUpper);
   QC_QChar->addMethod("unicode",                     (q_method_t)QCHAR_unicode);
   QC_QChar->addMethod("unicodeVersion",              (q_method_t)QCHAR_unicodeVersion);

   return QC_QChar;
}
