/*
 QC_QChar.cc
 
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

#include "QC_QChar.h"

#include "qt-core.h"

qore_classid_t CID_QCHAR;
QoreClass *QC_QChar = 0;

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
   return get_bool_node(qc->hasMirrored());
}

//bool isDigit () const
static AbstractQoreNode *QCHAR_isDigit(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qc->isDigit());
}

//bool isHighSurrogate () const
static AbstractQoreNode *QCHAR_isHighSurrogate(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qc->isHighSurrogate());
}

//bool isLetter () const
static AbstractQoreNode *QCHAR_isLetter(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qc->isLetter());
}

//bool isLetterOrNumber () const
static AbstractQoreNode *QCHAR_isLetterOrNumber(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qc->isLetterOrNumber());
}

//bool isLowSurrogate () const
static AbstractQoreNode *QCHAR_isLowSurrogate(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qc->isLowSurrogate());
}

//bool isLower () const
static AbstractQoreNode *QCHAR_isLower(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qc->isLower());
}

//bool isMark () const
static AbstractQoreNode *QCHAR_isMark(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qc->isMark());
}

//bool isNull () const
static AbstractQoreNode *QCHAR_isNull(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qc->isNull());
}

//bool isNumber () const
static AbstractQoreNode *QCHAR_isNumber(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qc->isNumber());
}

//bool isPrint () const
static AbstractQoreNode *QCHAR_isPrint(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qc->isPrint());
}

//bool isPunct () const
static AbstractQoreNode *QCHAR_isPunct(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qc->isPunct());
}

//bool isSpace () const
static AbstractQoreNode *QCHAR_isSpace(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qc->isSpace());
}

//bool isSymbol () const
static AbstractQoreNode *QCHAR_isSymbol(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qc->isSymbol());
}

//bool isTitleCase () const
static AbstractQoreNode *QCHAR_isTitleCase(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qc->isTitleCase());
}

//bool isUpper () const
static AbstractQoreNode *QCHAR_isUpper(QoreObject *self, QoreQChar *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qc->isUpper());
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

static QoreClass *initQCharClass()
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

QoreNamespace *initQCharNS()
{
   QoreNamespace *qchar = new QoreNamespace("QChar");

   qchar->addSystemClass(initQCharClass());

   // SpecialCharacter enum
   qchar->addConstant("Null",                     new QoreBigIntNode(QChar::Null));
   qchar->addConstant("Nbsp",                     new QoreBigIntNode(QChar::Nbsp));
   qchar->addConstant("ReplacementCharacter",     new QoreBigIntNode(QChar::ReplacementCharacter));
   qchar->addConstant("ObjectReplacementCharacter", new QoreBigIntNode(QChar::ObjectReplacementCharacter));
   qchar->addConstant("ByteOrderMark",            new QoreBigIntNode(QChar::ByteOrderMark));
   qchar->addConstant("ByteOrderSwapped",         new QoreBigIntNode(QChar::ByteOrderSwapped));
   qchar->addConstant("ParagraphSeparator",       new QoreBigIntNode(QChar::ParagraphSeparator));
   qchar->addConstant("LineSeparator",            new QoreBigIntNode(QChar::LineSeparator));

   // Category enum
   qchar->addConstant("NoCategory",               new QoreBigIntNode(QChar::NoCategory));
   qchar->addConstant("Mark_NonSpacing",          new QoreBigIntNode(QChar::Mark_NonSpacing));
   qchar->addConstant("Mark_SpacingCombining",    new QoreBigIntNode(QChar::Mark_SpacingCombining));
   qchar->addConstant("Mark_Enclosing",           new QoreBigIntNode(QChar::Mark_Enclosing));
   qchar->addConstant("Number_DecimalDigit",      new QoreBigIntNode(QChar::Number_DecimalDigit));
   qchar->addConstant("Number_Letter",            new QoreBigIntNode(QChar::Number_Letter));
   qchar->addConstant("Number_Other",             new QoreBigIntNode(QChar::Number_Other));
   qchar->addConstant("Separator_Space",          new QoreBigIntNode(QChar::Separator_Space));
   qchar->addConstant("Separator_Line",           new QoreBigIntNode(QChar::Separator_Line));
   qchar->addConstant("Separator_Paragraph",      new QoreBigIntNode(QChar::Separator_Paragraph));
   qchar->addConstant("Other_Control",            new QoreBigIntNode(QChar::Other_Control));
   qchar->addConstant("Other_Format",             new QoreBigIntNode(QChar::Other_Format));
   qchar->addConstant("Other_Surrogate",          new QoreBigIntNode(QChar::Other_Surrogate));
   qchar->addConstant("Other_PrivateUse",         new QoreBigIntNode(QChar::Other_PrivateUse));
   qchar->addConstant("Other_NotAssigned",        new QoreBigIntNode(QChar::Other_NotAssigned));
   qchar->addConstant("Letter_Uppercase",         new QoreBigIntNode(QChar::Letter_Uppercase));
   qchar->addConstant("Letter_Lowercase",         new QoreBigIntNode(QChar::Letter_Lowercase));
   qchar->addConstant("Letter_Titlecase",         new QoreBigIntNode(QChar::Letter_Titlecase));
   qchar->addConstant("Letter_Modifier",          new QoreBigIntNode(QChar::Letter_Modifier));
   qchar->addConstant("Letter_Other",             new QoreBigIntNode(QChar::Letter_Other));
   qchar->addConstant("Punctuation_Connector",    new QoreBigIntNode(QChar::Punctuation_Connector));
   qchar->addConstant("Punctuation_Dash",         new QoreBigIntNode(QChar::Punctuation_Dash));
   qchar->addConstant("Punctuation_Open",         new QoreBigIntNode(QChar::Punctuation_Open));
   qchar->addConstant("Punctuation_Close",        new QoreBigIntNode(QChar::Punctuation_Close));
   qchar->addConstant("Punctuation_InitialQuote", new QoreBigIntNode(QChar::Punctuation_InitialQuote));
   qchar->addConstant("Punctuation_FinalQuote",   new QoreBigIntNode(QChar::Punctuation_FinalQuote));
   qchar->addConstant("Punctuation_Other",        new QoreBigIntNode(QChar::Punctuation_Other));
   qchar->addConstant("Symbol_Math",              new QoreBigIntNode(QChar::Symbol_Math));
   qchar->addConstant("Symbol_Currency",          new QoreBigIntNode(QChar::Symbol_Currency));
   qchar->addConstant("Symbol_Modifier",          new QoreBigIntNode(QChar::Symbol_Modifier));
   qchar->addConstant("Symbol_Other",             new QoreBigIntNode(QChar::Symbol_Other));
   qchar->addConstant("Punctuation_Dask",         new QoreBigIntNode(QChar::Punctuation_Dask));

   // Direction enum
   qchar->addConstant("DirL",                     new QoreBigIntNode(QChar::DirL));
   qchar->addConstant("DirR",                     new QoreBigIntNode(QChar::DirR));
   qchar->addConstant("DirEN",                    new QoreBigIntNode(QChar::DirEN));
   qchar->addConstant("DirES",                    new QoreBigIntNode(QChar::DirES));
   qchar->addConstant("DirET",                    new QoreBigIntNode(QChar::DirET));
   qchar->addConstant("DirAN",                    new QoreBigIntNode(QChar::DirAN));
   qchar->addConstant("DirCS",                    new QoreBigIntNode(QChar::DirCS));
   qchar->addConstant("DirB",                     new QoreBigIntNode(QChar::DirB));
   qchar->addConstant("DirS",                     new QoreBigIntNode(QChar::DirS));
   qchar->addConstant("DirWS",                    new QoreBigIntNode(QChar::DirWS));
   qchar->addConstant("DirON",                    new QoreBigIntNode(QChar::DirON));
   qchar->addConstant("DirLRE",                   new QoreBigIntNode(QChar::DirLRE));
   qchar->addConstant("DirLRO",                   new QoreBigIntNode(QChar::DirLRO));
   qchar->addConstant("DirAL",                    new QoreBigIntNode(QChar::DirAL));
   qchar->addConstant("DirRLE",                   new QoreBigIntNode(QChar::DirRLE));
   qchar->addConstant("DirRLO",                   new QoreBigIntNode(QChar::DirRLO));
   qchar->addConstant("DirPDF",                   new QoreBigIntNode(QChar::DirPDF));
   qchar->addConstant("DirNSM",                   new QoreBigIntNode(QChar::DirNSM));
   qchar->addConstant("DirBN",                    new QoreBigIntNode(QChar::DirBN));

   // Decomposition enum
   qchar->addConstant("NoDecomposition",          new QoreBigIntNode(QChar::NoDecomposition));
   qchar->addConstant("Canonical",                new QoreBigIntNode(QChar::Canonical));
   qchar->addConstant("Font",                     new QoreBigIntNode(QChar::Font));
   qchar->addConstant("NoBreak",                  new QoreBigIntNode(QChar::NoBreak));
   qchar->addConstant("Initial",                  new QoreBigIntNode(QChar::Initial));
   qchar->addConstant("Medial",                   new QoreBigIntNode(QChar::Medial));
   qchar->addConstant("Final",                    new QoreBigIntNode(QChar::Final));
   qchar->addConstant("Isolated",                 new QoreBigIntNode(QChar::Isolated));
   qchar->addConstant("Circle",                   new QoreBigIntNode(QChar::Circle));
   qchar->addConstant("Super",                    new QoreBigIntNode(QChar::Super));
   qchar->addConstant("Sub",                      new QoreBigIntNode(QChar::Sub));
   qchar->addConstant("Vertical",                 new QoreBigIntNode(QChar::Vertical));
   qchar->addConstant("Wide",                     new QoreBigIntNode(QChar::Wide));
   qchar->addConstant("Narrow",                   new QoreBigIntNode(QChar::Narrow));
   qchar->addConstant("Small",                    new QoreBigIntNode(QChar::Small));
   qchar->addConstant("Square",                   new QoreBigIntNode(QChar::Square));
   qchar->addConstant("Compat",                   new QoreBigIntNode(QChar::Compat));
   qchar->addConstant("Fraction",                 new QoreBigIntNode(QChar::Fraction));

   // Joining enum
   qchar->addConstant("OtherJoining",             new QoreBigIntNode(QChar::OtherJoining));
   qchar->addConstant("Dual",                     new QoreBigIntNode(QChar::Dual));
   qchar->addConstant("Right",                    new QoreBigIntNode(QChar::Right));
   qchar->addConstant("Center",                   new QoreBigIntNode(QChar::Center));

   // Combining class
   qchar->addConstant("Combining_BelowLeftAttached", new QoreBigIntNode(QChar::Combining_BelowLeftAttached));
   qchar->addConstant("Combining_BelowAttached",  new QoreBigIntNode(QChar::Combining_BelowAttached));
   qchar->addConstant("Combining_BelowRightAttached", new QoreBigIntNode(QChar::Combining_BelowRightAttached));
   qchar->addConstant("Combining_LeftAttached",   new QoreBigIntNode(QChar::Combining_LeftAttached));
   qchar->addConstant("Combining_RightAttached",  new QoreBigIntNode(QChar::Combining_RightAttached));
   qchar->addConstant("Combining_AboveLeftAttached", new QoreBigIntNode(QChar::Combining_AboveLeftAttached));
   qchar->addConstant("Combining_AboveAttached",  new QoreBigIntNode(QChar::Combining_AboveAttached));
   qchar->addConstant("Combining_AboveRightAttached", new QoreBigIntNode(QChar::Combining_AboveRightAttached));
   qchar->addConstant("Combining_BelowLeft",      new QoreBigIntNode(QChar::Combining_BelowLeft));
   qchar->addConstant("Combining_Below",          new QoreBigIntNode(QChar::Combining_Below));
   qchar->addConstant("Combining_BelowRight",     new QoreBigIntNode(QChar::Combining_BelowRight));
   qchar->addConstant("Combining_Left",           new QoreBigIntNode(QChar::Combining_Left));
   qchar->addConstant("Combining_Right",          new QoreBigIntNode(QChar::Combining_Right));
   qchar->addConstant("Combining_AboveLeft",      new QoreBigIntNode(QChar::Combining_AboveLeft));
   qchar->addConstant("Combining_Above",          new QoreBigIntNode(QChar::Combining_Above));
   qchar->addConstant("Combining_AboveRight",     new QoreBigIntNode(QChar::Combining_AboveRight));
   qchar->addConstant("Combining_DoubleBelow",    new QoreBigIntNode(QChar::Combining_DoubleBelow));
   qchar->addConstant("Combining_DoubleAbove",    new QoreBigIntNode(QChar::Combining_DoubleAbove));
   qchar->addConstant("Combining_IotaSubscript",  new QoreBigIntNode(QChar::Combining_IotaSubscript));

   // UnicodeVersion
   qchar->addConstant("Unicode_Unassigned",       new QoreBigIntNode(QChar::Unicode_Unassigned));
   qchar->addConstant("Unicode_1_1",              new QoreBigIntNode(QChar::Unicode_1_1));
   qchar->addConstant("Unicode_2_0",              new QoreBigIntNode(QChar::Unicode_2_0));
   qchar->addConstant("Unicode_2_1_2",            new QoreBigIntNode(QChar::Unicode_2_1_2));
   qchar->addConstant("Unicode_3_0",              new QoreBigIntNode(QChar::Unicode_3_0));
   qchar->addConstant("Unicode_3_1",              new QoreBigIntNode(QChar::Unicode_3_1));
   qchar->addConstant("Unicode_3_2",              new QoreBigIntNode(QChar::Unicode_3_2));
   qchar->addConstant("Unicode_4_0",              new QoreBigIntNode(QChar::Unicode_4_0));
   qchar->addConstant("Unicode_4_1",              new QoreBigIntNode(QChar::Unicode_4_1));
   qchar->addConstant("Unicode_5_0",              new QoreBigIntNode(QChar::Unicode_5_0));

   return qchar;
}
