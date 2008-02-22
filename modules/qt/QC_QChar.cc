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

#include "qore-qt.h"

int CID_QCHAR;
class QoreClass *QC_QChar = 0;

//QChar ()
//QChar ( char ch )
//QChar ( int code )
//QChar ( SpecialCharacter ch )
static void QCHAR_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QCHAR, new QoreQChar());
      return;
   }
   int code;
   if (p->getType() == NT_STRING) {
      code = (reinterpret_cast<const QoreStringNode *>(p))->getUnicodePoint(0, xsink);
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
static AbstractQoreNode *QCHAR_category(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qc->category());
}

//uchar cell () const
static AbstractQoreNode *QCHAR_cell(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qc->cell());
}

//unsigned char combiningClass () const
static AbstractQoreNode *QCHAR_combiningClass(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   const char c_rv = qc->combiningClass();
   QoreStringNode *rv_str = new QoreStringNode();
   rv_str->concat(c_rv);
   return rv_str;
}

//QString decomposition () const
static AbstractQoreNode *QCHAR_decomposition(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qc->decomposition().toUtf8().data(), QCS_UTF8);
}

//Decomposition decompositionTag () const
static AbstractQoreNode *QCHAR_decompositionTag(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qc->decompositionTag());
}

//int digitValue () const
static AbstractQoreNode *QCHAR_digitValue(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qc->digitValue());
}

//Direction direction () const
static AbstractQoreNode *QCHAR_direction(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qc->direction());
}

//bool hasMirrored () const
static AbstractQoreNode *QCHAR_hasMirrored(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qc->hasMirrored());
}

//bool isDigit () const
static AbstractQoreNode *QCHAR_isDigit(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qc->isDigit());
}

//bool isHighSurrogate () const
static AbstractQoreNode *QCHAR_isHighSurrogate(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qc->isHighSurrogate());
}

//bool isLetter () const
static AbstractQoreNode *QCHAR_isLetter(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qc->isLetter());
}

//bool isLetterOrNumber () const
static AbstractQoreNode *QCHAR_isLetterOrNumber(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qc->isLetterOrNumber());
}

//bool isLowSurrogate () const
static AbstractQoreNode *QCHAR_isLowSurrogate(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qc->isLowSurrogate());
}

//bool isLower () const
static AbstractQoreNode *QCHAR_isLower(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qc->isLower());
}

//bool isMark () const
static AbstractQoreNode *QCHAR_isMark(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qc->isMark());
}

//bool isNull () const
static AbstractQoreNode *QCHAR_isNull(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qc->isNull());
}

//bool isNumber () const
static AbstractQoreNode *QCHAR_isNumber(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qc->isNumber());
}

//bool isPrint () const
static AbstractQoreNode *QCHAR_isPrint(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qc->isPrint());
}

//bool isPunct () const
static AbstractQoreNode *QCHAR_isPunct(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qc->isPunct());
}

//bool isSpace () const
static AbstractQoreNode *QCHAR_isSpace(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qc->isSpace());
}

//bool isSymbol () const
static AbstractQoreNode *QCHAR_isSymbol(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qc->isSymbol());
}

//bool isTitleCase () const
static AbstractQoreNode *QCHAR_isTitleCase(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qc->isTitleCase());
}

//bool isUpper () const
static AbstractQoreNode *QCHAR_isUpper(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qc->isUpper());
}

//Joining joining () const
static AbstractQoreNode *QCHAR_joining(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qc->joining());
}

//QChar mirroredChar () const
static AbstractQoreNode *QCHAR_mirroredChar(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *rv_str = new QoreStringNode(QCS_UTF8);
   QChar rv_qc = qc->mirroredChar();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return rv_str;
}

//uchar row () const
static AbstractQoreNode *QCHAR_row(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qc->row());
}

//const char toAscii () const
static AbstractQoreNode *QCHAR_toAscii(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   const char c_rv = qc->toAscii();
   QoreStringNode *rv_str = new QoreStringNode();
   rv_str->concat(c_rv);
   return rv_str;
}

//QChar toCaseFolded () const
static AbstractQoreNode *QCHAR_toCaseFolded(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *rv_str = new QoreStringNode(QCS_UTF8);
   QChar rv_qc = qc->toCaseFolded();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return rv_str;
}

//const char toLatin1 () const
static AbstractQoreNode *QCHAR_toLatin1(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   const char c_rv = qc->toLatin1();
   QoreStringNode *rv_str = new QoreStringNode();
   rv_str->concat(c_rv);
   return rv_str;
}

//QChar toLower () const
static AbstractQoreNode *QCHAR_toLower(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *rv_str = new QoreStringNode(QCS_UTF8);
   QChar rv_qc = qc->toLower();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return rv_str;
}

//QChar toTitleCase () const
static AbstractQoreNode *QCHAR_toTitleCase(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *rv_str = new QoreStringNode(QCS_UTF8);
   QChar rv_qc = qc->toTitleCase();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return rv_str;
}

//QChar toUpper () const
static AbstractQoreNode *QCHAR_toUpper(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *rv_str = new QoreStringNode(QCS_UTF8);
   QChar rv_qc = qc->toUpper();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return rv_str;
}

//ushort & unicode ()
//const ushort unicode () const
static AbstractQoreNode *QCHAR_unicode(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qc->unicode());
}

//UnicodeVersion unicodeVersion () const
static AbstractQoreNode *QCHAR_unicodeVersion(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qc->unicodeVersion());
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
