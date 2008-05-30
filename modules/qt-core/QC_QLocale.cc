/*
 QC_QLocale.cc
 
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

#include "QC_QLocale.h"

#include "qt-core.h"

qore_classid_t CID_QLOCALE;
QoreClass *QC_QLocale = 0;

//QLocale ()
//QLocale ( const QString & name )
//QLocale ( Language language, Country country = AnyCountry )
//QLocale ( const QLocale & other )
static void QLOCALE_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QLOCALE, new QoreQLocale());
      return;
   }
   if (p && p->getType() == NT_STRING) {
      QString name;
      if (get_qstring(p, name, xsink))
	 return;
      self->setPrivate(CID_QLOCALE, new QoreQLocale(name));
      return;
   }
   QLocale::Language language = (QLocale::Language)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QLocale::Country country = (QLocale::Country)(p ? p->getAsInt() : 0);
   self->setPrivate(CID_QLOCALE, new QoreQLocale(language, country));
   return;
}

static void QLOCALE_copy(class QoreObject *self, class QoreObject *old, class QoreQLocale *ql, ExceptionSink *xsink)
{
   self->setPrivate(CID_QLOCALE, new QoreQLocale(*ql));
}

//Country country () const
static AbstractQoreNode *QLOCALE_country(QoreObject *self, QoreQLocale *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(ql->country());
}

//QString dateFormat ( FormatType format = LongFormat ) const
static AbstractQoreNode *QLOCALE_dateFormat(QoreObject *self, QoreQLocale *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QLocale::FormatType format = (QLocale::FormatType)(p ? p->getAsInt() : 0);
   return new QoreStringNode(ql->dateFormat(format).toUtf8().data(), QCS_UTF8);
}

//QString dayName ( int day, FormatType type = LongFormat ) const
static AbstractQoreNode *QLOCALE_dayName(QoreObject *self, QoreQLocale *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int day = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QLocale::FormatType type = (QLocale::FormatType)(p ? p->getAsInt() : 0);
   return new QoreStringNode(ql->dayName(day, type).toUtf8().data(), QCS_UTF8);
}

//QChar decimalPoint () const
static AbstractQoreNode *QLOCALE_decimalPoint(QoreObject *self, QoreQLocale *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *rv_str = new QoreStringNode(QCS_UTF8);
   QChar rv_qc = ql->decimalPoint();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return rv_str;
}

//QChar exponential () const
static AbstractQoreNode *QLOCALE_exponential(QoreObject *self, QoreQLocale *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *rv_str = new QoreStringNode(QCS_UTF8);
   QChar rv_qc = ql->exponential();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return rv_str;
}

//QChar groupSeparator () const
static AbstractQoreNode *QLOCALE_groupSeparator(QoreObject *self, QoreQLocale *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *rv_str = new QoreStringNode(QCS_UTF8);
   QChar rv_qc = ql->groupSeparator();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return rv_str;
}

//Language language () const
static AbstractQoreNode *QLOCALE_language(QoreObject *self, QoreQLocale *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(ql->language());
}

//QString monthName ( int month, FormatType type = LongFormat ) const
static AbstractQoreNode *QLOCALE_monthName(QoreObject *self, QoreQLocale *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int month = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QLocale::FormatType type = (QLocale::FormatType)(!is_nothing(p) ? p->getAsInt() : QLocale::LongFormat);
   return new QoreStringNode(ql->monthName(month, type).toUtf8().data(), QCS_UTF8);
}

//QString name () const
static AbstractQoreNode *QLOCALE_name(QoreObject *self, QoreQLocale *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(ql->name().toUtf8().data(), QCS_UTF8);
}

//QChar negativeSign () const
static AbstractQoreNode *QLOCALE_negativeSign(QoreObject *self, QoreQLocale *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *rv_str = new QoreStringNode(QCS_UTF8);
   QChar rv_qc = ql->negativeSign();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return rv_str;
}

//NumberOptions numberOptions () const
static AbstractQoreNode *QLOCALE_numberOptions(QoreObject *self, QoreQLocale *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(ql->numberOptions());
}

//QChar percent () const
static AbstractQoreNode *QLOCALE_percent(QoreObject *self, QoreQLocale *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *rv_str = new QoreStringNode(QCS_UTF8);
   QChar rv_qc = ql->percent();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return rv_str;
}

//void setNumberOptions ( NumberOptions options )
static AbstractQoreNode *QLOCALE_setNumberOptions(QoreObject *self, QoreQLocale *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QLocale::NumberOptions options = (QLocale::NumberOptions)(p ? p->getAsInt() : 0);
   ql->setNumberOptions(options);
   return 0;
}

//QString timeFormat ( FormatType format = LongFormat ) const
static AbstractQoreNode *QLOCALE_timeFormat(QoreObject *self, QoreQLocale *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QLocale::FormatType format = (QLocale::FormatType)(p ? p->getAsInt() : 0);
   return new QoreStringNode(ql->timeFormat(format).toUtf8().data(), QCS_UTF8);
}

//double toDouble ( const QString & s, bool * ok = 0 ) const
static AbstractQoreNode *QLOCALE_toDouble(QoreObject *self, QoreQLocale *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString s;
   if (get_qstring(p, s, xsink))
      return 0;
   bool ok;
   double rv = ql->toDouble(s, &ok);
   if (!ok) {
      xsink->raiseException("QLOCALE-TODOUBLE-ERROR", "error encountered in QLocale::toDouble()");
      return 0;
   }
      
   return new QoreFloatNode(rv);
}

//qlonglong toLongLong ( const QString & s, bool * ok = 0, int base = 0 ) const
static AbstractQoreNode *QLOCALE_toLongLong(QoreObject *self, QoreQLocale *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString s;
   if (get_qstring(p, s, xsink))
      return 0;

   p = get_param(params, 1);
   int base = p ? p->getAsInt() : 0;

   bool ok;
   int64 rv = ql->toLongLong(s, &ok, base);
   if (!ok) {
      xsink->raiseException("QLOCALE-TOLONGLONG-ERROR", "error encountered in QLocale::toLongLong()");
      return 0;
   }
   return new QoreBigIntNode(rv);
}

////QString toString ( qlonglong i ) const
////QString toString ( const QDate & date, const QString & format ) const
////QString toString ( const QDate & date, FormatType format = LongFormat ) const
////QString toString ( const QTime & time, const QString & format ) const
////QString toString ( const QTime & time, FormatType format = LongFormat ) const
////QString toString ( qulonglong i ) const
////QString toString ( double i, char f = 'g', int prec = 6 ) const
////QString toString ( short i ) const
////QString toString ( ushort i ) const
////QString toString ( int i ) const
////QString toString ( uint i ) const
////QString toString ( float i, char f = 'g', int prec = 6 ) const
//static AbstractQoreNode *QLOCALE_toString(QoreObject *self, QoreQLocale *ql, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   if (p && p->getType() == NT_???) {
//      QDate date;
//      if (get_qdate(p, date, xsink))
//         return 0;
//   p = get_param(params, 1);
//   QLocale::char f = (QLocale::char)(p ? p->getAsInt() : 0);
//   p = get_param(params, 2);
//   int prec = !is_nothing(p) ? p->getAsInt() : 6;
//   return new QoreStringNode(ql->toString(date, f, prec).toUtf8().data(), QCS_UTF8);
//   }
//   if (p && p->getType() == NT_???) {
//      QDate date;
//      if (get_qdate(p, date, xsink))
//         return 0;
//   p = get_param(params, 1);
//   QLocale::char f = (QLocale::char)(p ? p->getAsInt() : 0);
//   p = get_param(params, 2);
//   int prec = !is_nothing(p) ? p->getAsInt() : 6;
//   return new QoreStringNode(ql->toString(date, f, prec).toUtf8().data(), QCS_UTF8);
//   }
//   if (p && p->getType() == NT_???) {
//      QTime time;
//      if (get_qtime(p, time, xsink))
//         return 0;
//   p = get_param(params, 1);
//   QLocale::char f = (QLocale::char)(p ? p->getAsInt() : 0);
//   p = get_param(params, 2);
//   int prec = !is_nothing(p) ? p->getAsInt() : 6;
//   return new QoreStringNode(ql->toString(time, f, prec).toUtf8().data(), QCS_UTF8);
//   }
//   if (p && p->getType() == NT_???) {
//      QTime time;
//      if (get_qtime(p, time, xsink))
//         return 0;
//   p = get_param(params, 1);
//   QLocale::char f = (QLocale::char)(p ? p->getAsInt() : 0);
//   p = get_param(params, 2);
//   int prec = !is_nothing(p) ? p->getAsInt() : 6;
//   return new QoreStringNode(ql->toString(time, f, prec).toUtf8().data(), QCS_UTF8);
//   }
//   if (p && p->getType() == NT_INT) {
//      QLocale::qlonglong i = (QLocale::qlonglong)(p ? p->getAsInt() : 0);
//      return new QoreStringNode(ql->toString(i).toUtf8().data(), QCS_UTF8);
//   }
//   if (p && p->getType() == NT_INT) {
//      QLocale::qulonglong i = (QLocale::qulonglong)(p ? p->getAsInt() : 0);
//      return new QoreStringNode(ql->toString(i).toUtf8().data(), QCS_UTF8);
//   }
//   if (p && p->getType() == NT_INT) {
//      QLocale::short i = (QLocale::short)(p ? p->getAsInt() : 0);
//      return new QoreStringNode(ql->toString(i).toUtf8().data(), QCS_UTF8);
//   }
//   if (p && p->getType() == NT_INT) {
//      QLocale::ushort i = (QLocale::ushort)(p ? p->getAsInt() : 0);
//      return new QoreStringNode(ql->toString(i).toUtf8().data(), QCS_UTF8);
//   }
//   if (p && p->getType() == NT_INT) {
//      int i = p ? p->getAsInt() : 0;
//      return new QoreStringNode(ql->toString(i).toUtf8().data(), QCS_UTF8);
//   }
//   if (p && p->getType() == NT_INT) {
//      unsigned i = p ? p->getAsBigInt() : 0;
//      return new QoreStringNode(ql->toString(i).toUtf8().data(), QCS_UTF8);
//   }
//   double i = p ? p->getAsFloat() : 0.0;
//   p = get_param(params, 1);
//   QLocale::char f = (QLocale::char)(p ? p->getAsInt() : 0);
//   p = get_param(params, 2);
//   int prec = !is_nothing(p) ? p->getAsInt() : 6;
//   return new QoreStringNode(ql->toString(i, f, prec).toUtf8().data(), QCS_UTF8);
//}

//QChar zeroDigit () const
static AbstractQoreNode *QLOCALE_zeroDigit(QoreObject *self, QoreQLocale *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *rv_str = new QoreStringNode(QCS_UTF8);
   QChar rv_qc = ql->zeroDigit();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return rv_str;
}

static AbstractQoreNode *f_QLocale_countriesForLanguage(const QoreListNode *params, class ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QLocale::Language language = (QLocale::Language)(p ? p->getAsInt() : 0);
   QList<QLocale::Country> ql = QLocale::countriesForLanguage(language);
   if (ql.empty())
      return 0;

   QoreListNode *l = new QoreListNode();
   for (int i = 0; i < ql.count(); ++i)
      l->push(new QoreBigIntNode(ql.at(i)));
   return l;
}

static AbstractQoreNode *f_QLocale_languageToString(const QoreListNode *params, class ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QLocale::Language language = (QLocale::Language)(p ? p->getAsInt() : 0);
   return new QoreStringNode(QLocale::languageToString(language).toUtf8().data(), QCS_UTF8);
}

static AbstractQoreNode *f_QLocale_countryToString(const QoreListNode *params, class ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QLocale::Country country = (QLocale::Country)(p ? p->getAsInt() : 0);
   return new QoreStringNode(QLocale::countryToString(country).toUtf8().data(), QCS_UTF8);
}

static QoreClass *initQLocaleClass()
{
   QC_QLocale = new QoreClass("QLocale", QDOM_GUI);
   CID_QLOCALE = QC_QLocale->getID();

   QC_QLocale->setConstructor(QLOCALE_constructor);
   QC_QLocale->setCopy((q_copy_t)QLOCALE_copy);

   QC_QLocale->addMethod("country",                     (q_method_t)QLOCALE_country);
   QC_QLocale->addMethod("dateFormat",                  (q_method_t)QLOCALE_dateFormat);
   QC_QLocale->addMethod("dayName",                     (q_method_t)QLOCALE_dayName);
   QC_QLocale->addMethod("decimalPoint",                (q_method_t)QLOCALE_decimalPoint);
   QC_QLocale->addMethod("exponential",                 (q_method_t)QLOCALE_exponential);
   QC_QLocale->addMethod("groupSeparator",              (q_method_t)QLOCALE_groupSeparator);
   QC_QLocale->addMethod("language",                    (q_method_t)QLOCALE_language);
   QC_QLocale->addMethod("monthName",                   (q_method_t)QLOCALE_monthName);
   QC_QLocale->addMethod("name",                        (q_method_t)QLOCALE_name);
   QC_QLocale->addMethod("negativeSign",                (q_method_t)QLOCALE_negativeSign);
   QC_QLocale->addMethod("numberOptions",               (q_method_t)QLOCALE_numberOptions);
   QC_QLocale->addMethod("percent",                     (q_method_t)QLOCALE_percent);
   QC_QLocale->addMethod("setNumberOptions",            (q_method_t)QLOCALE_setNumberOptions);
   QC_QLocale->addMethod("timeFormat",                  (q_method_t)QLOCALE_timeFormat);
   QC_QLocale->addMethod("toDouble",                    (q_method_t)QLOCALE_toDouble);
   QC_QLocale->addMethod("toFloat",                     (q_method_t)QLOCALE_toDouble);
   QC_QLocale->addMethod("toInt",                       (q_method_t)QLOCALE_toLongLong);
   QC_QLocale->addMethod("toLongLong",                  (q_method_t)QLOCALE_toLongLong);
   QC_QLocale->addMethod("toShort",                     (q_method_t)QLOCALE_toLongLong);
   //QC_QLocale->addMethod("toString",                    (q_method_t)QLOCALE_toString);
   QC_QLocale->addMethod("toUInt",                      (q_method_t)QLOCALE_toLongLong);
   QC_QLocale->addMethod("toULongLong",                 (q_method_t)QLOCALE_toLongLong);
   QC_QLocale->addMethod("toUShort",                    (q_method_t)QLOCALE_toLongLong);
   QC_QLocale->addMethod("zeroDigit",                   (q_method_t)QLOCALE_zeroDigit);

   return QC_QLocale;
}

//QLocale c ()
static AbstractQoreNode *f_QLocale_c(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_ql = new QoreObject(QC_QLocale, getProgram());
   QoreQLocale *q_ql = new QoreQLocale(QLocale::c());
   o_ql->setPrivate(CID_QLOCALE, q_ql);
   return o_ql;
}

//void setDefault ( const QLocale & locale )
static AbstractQoreNode *f_QLocale_setDefault(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQLocale *locale = p ? (QoreQLocale *)p->getReferencedPrivateData(CID_QLOCALE, xsink) : 0;
   if (!locale) {
      if (!xsink->isException())
         xsink->raiseException("QLOCALE-SETDEFAULT-PARAM-ERROR", "expecting a QLocale object as first argument to QLocale::setDefault()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> localeHolder(static_cast<AbstractPrivateData *>(locale), xsink);
   QLocale::setDefault(*(static_cast<QLocale *>(locale)));
   return 0;
}

//QLocale system ()
static AbstractQoreNode *f_QLocale_system(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_ql = new QoreObject(QC_QLocale, getProgram());
   QoreQLocale *q_ql = new QoreQLocale(QLocale::system());
   o_ql->setPrivate(CID_QLOCALE, q_ql);
   return o_ql;
}

void initQLocaleStaticFunctions()
{
   // add builtin functions
   builtinFunctions.add("QLocale_countriesForLanguage",       f_QLocale_countriesForLanguage, QDOM_GUI);
   builtinFunctions.add("QLocale_languageToString",           f_QLocale_languageToString, QDOM_GUI);
   builtinFunctions.add("QLocale_countryToString",            f_QLocale_countryToString, QDOM_GUI);
   builtinFunctions.add("QLocale_c",                          f_QLocale_c, QDOM_GUI);
   builtinFunctions.add("QLocale_setDefault",                 f_QLocale_setDefault, QDOM_GUI);
   builtinFunctions.add("QLocale_system",                     f_QLocale_system, QDOM_GUI);
}

QoreNamespace *initQLocaleNS()
{
   QoreNamespace *qlocale = new QoreNamespace("QLocale");
   qlocale->addSystemClass(initQLocaleClass());

   // Language enum
   qlocale->addConstant("C",                        new QoreBigIntNode(QLocale::C));
   qlocale->addConstant("Abkhazian",                new QoreBigIntNode(QLocale::Abkhazian));
   qlocale->addConstant("Afan",                     new QoreBigIntNode(QLocale::Afan));
   qlocale->addConstant("Afar",                     new QoreBigIntNode(QLocale::Afar));
   qlocale->addConstant("Afrikaans",                new QoreBigIntNode(QLocale::Afrikaans));
   qlocale->addConstant("Albanian",                 new QoreBigIntNode(QLocale::Albanian));
   qlocale->addConstant("Amharic",                  new QoreBigIntNode(QLocale::Amharic));
   qlocale->addConstant("Arabic",                   new QoreBigIntNode(QLocale::Arabic));
   qlocale->addConstant("Armenian",                 new QoreBigIntNode(QLocale::Armenian));
   qlocale->addConstant("Assamese",                 new QoreBigIntNode(QLocale::Assamese));
   qlocale->addConstant("Aymara",                   new QoreBigIntNode(QLocale::Aymara));
   qlocale->addConstant("Azerbaijani",              new QoreBigIntNode(QLocale::Azerbaijani));
   qlocale->addConstant("Bashkir",                  new QoreBigIntNode(QLocale::Bashkir));
   qlocale->addConstant("Basque",                   new QoreBigIntNode(QLocale::Basque));
   qlocale->addConstant("Bengali",                  new QoreBigIntNode(QLocale::Bengali));
   qlocale->addConstant("Bhutani",                  new QoreBigIntNode(QLocale::Bhutani));
   qlocale->addConstant("Bihari",                   new QoreBigIntNode(QLocale::Bihari));
   qlocale->addConstant("Bislama",                  new QoreBigIntNode(QLocale::Bislama));
   qlocale->addConstant("Breton",                   new QoreBigIntNode(QLocale::Breton));
   qlocale->addConstant("Bulgarian",                new QoreBigIntNode(QLocale::Bulgarian));
   qlocale->addConstant("Burmese",                  new QoreBigIntNode(QLocale::Burmese));
   qlocale->addConstant("Byelorussian",             new QoreBigIntNode(QLocale::Byelorussian));
   qlocale->addConstant("Cambodian",                new QoreBigIntNode(QLocale::Cambodian));
   qlocale->addConstant("Catalan",                  new QoreBigIntNode(QLocale::Catalan));
   qlocale->addConstant("Chinese",                  new QoreBigIntNode(QLocale::Chinese));
   qlocale->addConstant("Corsican",                 new QoreBigIntNode(QLocale::Corsican));
   qlocale->addConstant("Croatian",                 new QoreBigIntNode(QLocale::Croatian));
   qlocale->addConstant("Czech",                    new QoreBigIntNode(QLocale::Czech));
   qlocale->addConstant("Danish",                   new QoreBigIntNode(QLocale::Danish));
   qlocale->addConstant("Dutch",                    new QoreBigIntNode(QLocale::Dutch));
   qlocale->addConstant("English",                  new QoreBigIntNode(QLocale::English));
   qlocale->addConstant("Esperanto",                new QoreBigIntNode(QLocale::Esperanto));
   qlocale->addConstant("Estonian",                 new QoreBigIntNode(QLocale::Estonian));
   qlocale->addConstant("Faroese",                  new QoreBigIntNode(QLocale::Faroese));
   qlocale->addConstant("FijiLanguage",             new QoreBigIntNode(QLocale::FijiLanguage));
   qlocale->addConstant("Finnish",                  new QoreBigIntNode(QLocale::Finnish));
   qlocale->addConstant("French",                   new QoreBigIntNode(QLocale::French));
   qlocale->addConstant("Frisian",                  new QoreBigIntNode(QLocale::Frisian));
   qlocale->addConstant("Gaelic",                   new QoreBigIntNode(QLocale::Gaelic));
   qlocale->addConstant("Galician",                 new QoreBigIntNode(QLocale::Galician));
   qlocale->addConstant("Georgian",                 new QoreBigIntNode(QLocale::Georgian));
   qlocale->addConstant("German",                   new QoreBigIntNode(QLocale::German));
   qlocale->addConstant("Greek",                    new QoreBigIntNode(QLocale::Greek));
   qlocale->addConstant("Greenlandic",              new QoreBigIntNode(QLocale::Greenlandic));
   qlocale->addConstant("Guarani",                  new QoreBigIntNode(QLocale::Guarani));
   qlocale->addConstant("Gujarati",                 new QoreBigIntNode(QLocale::Gujarati));
   qlocale->addConstant("Hausa",                    new QoreBigIntNode(QLocale::Hausa));
   qlocale->addConstant("Hebrew",                   new QoreBigIntNode(QLocale::Hebrew));
   qlocale->addConstant("Hindi",                    new QoreBigIntNode(QLocale::Hindi));
   qlocale->addConstant("Hungarian",                new QoreBigIntNode(QLocale::Hungarian));
   qlocale->addConstant("Icelandic",                new QoreBigIntNode(QLocale::Icelandic));
   qlocale->addConstant("Indonesian",               new QoreBigIntNode(QLocale::Indonesian));
   qlocale->addConstant("Interlingua",              new QoreBigIntNode(QLocale::Interlingua));
   qlocale->addConstant("Interlingue",              new QoreBigIntNode(QLocale::Interlingue));
   qlocale->addConstant("Inuktitut",                new QoreBigIntNode(QLocale::Inuktitut));
   qlocale->addConstant("Inupiak",                  new QoreBigIntNode(QLocale::Inupiak));
   qlocale->addConstant("Irish",                    new QoreBigIntNode(QLocale::Irish));
   qlocale->addConstant("Italian",                  new QoreBigIntNode(QLocale::Italian));
   qlocale->addConstant("Japanese",                 new QoreBigIntNode(QLocale::Japanese));
   qlocale->addConstant("Javanese",                 new QoreBigIntNode(QLocale::Javanese));
   qlocale->addConstant("Kannada",                  new QoreBigIntNode(QLocale::Kannada));
   qlocale->addConstant("Kashmiri",                 new QoreBigIntNode(QLocale::Kashmiri));
   qlocale->addConstant("Kazakh",                   new QoreBigIntNode(QLocale::Kazakh));
   qlocale->addConstant("Kinyarwanda",              new QoreBigIntNode(QLocale::Kinyarwanda));
   qlocale->addConstant("Kirghiz",                  new QoreBigIntNode(QLocale::Kirghiz));
   qlocale->addConstant("Korean",                   new QoreBigIntNode(QLocale::Korean));
   qlocale->addConstant("Kurdish",                  new QoreBigIntNode(QLocale::Kurdish));
   qlocale->addConstant("Kurundi",                  new QoreBigIntNode(QLocale::Kurundi));
   qlocale->addConstant("Laothian",                 new QoreBigIntNode(QLocale::Laothian));
   qlocale->addConstant("Latin",                    new QoreBigIntNode(QLocale::Latin));
   qlocale->addConstant("Latvian",                  new QoreBigIntNode(QLocale::Latvian));
   qlocale->addConstant("Lingala",                  new QoreBigIntNode(QLocale::Lingala));
   qlocale->addConstant("Lithuanian",               new QoreBigIntNode(QLocale::Lithuanian));
   qlocale->addConstant("Macedonian",               new QoreBigIntNode(QLocale::Macedonian));
   qlocale->addConstant("Malagasy",                 new QoreBigIntNode(QLocale::Malagasy));
   qlocale->addConstant("Malay",                    new QoreBigIntNode(QLocale::Malay));
   qlocale->addConstant("Malayalam",                new QoreBigIntNode(QLocale::Malayalam));
   qlocale->addConstant("Maltese",                  new QoreBigIntNode(QLocale::Maltese));
   qlocale->addConstant("Maori",                    new QoreBigIntNode(QLocale::Maori));
   qlocale->addConstant("Marathi",                  new QoreBigIntNode(QLocale::Marathi));
   qlocale->addConstant("Moldavian",                new QoreBigIntNode(QLocale::Moldavian));
   qlocale->addConstant("Mongolian",                new QoreBigIntNode(QLocale::Mongolian));
   qlocale->addConstant("NauruLanguage",            new QoreBigIntNode(QLocale::NauruLanguage));
   qlocale->addConstant("Nepali",                   new QoreBigIntNode(QLocale::Nepali));
   qlocale->addConstant("Norwegian",                new QoreBigIntNode(QLocale::Norwegian));
   qlocale->addConstant("NorwegianBokmal",          new QoreBigIntNode(QLocale::NorwegianBokmal));
   qlocale->addConstant("Occitan",                  new QoreBigIntNode(QLocale::Occitan));
   qlocale->addConstant("Oriya",                    new QoreBigIntNode(QLocale::Oriya));
   qlocale->addConstant("Pashto",                   new QoreBigIntNode(QLocale::Pashto));
   qlocale->addConstant("Persian",                  new QoreBigIntNode(QLocale::Persian));
   qlocale->addConstant("Polish",                   new QoreBigIntNode(QLocale::Polish));
   qlocale->addConstant("Portuguese",               new QoreBigIntNode(QLocale::Portuguese));
   qlocale->addConstant("Punjabi",                  new QoreBigIntNode(QLocale::Punjabi));
   qlocale->addConstant("Quechua",                  new QoreBigIntNode(QLocale::Quechua));
   qlocale->addConstant("RhaetoRomance",            new QoreBigIntNode(QLocale::RhaetoRomance));
   qlocale->addConstant("Romanian",                 new QoreBigIntNode(QLocale::Romanian));
   qlocale->addConstant("Russian",                  new QoreBigIntNode(QLocale::Russian));
   qlocale->addConstant("Samoan",                   new QoreBigIntNode(QLocale::Samoan));
   qlocale->addConstant("Sangho",                   new QoreBigIntNode(QLocale::Sangho));
   qlocale->addConstant("Sanskrit",                 new QoreBigIntNode(QLocale::Sanskrit));
   qlocale->addConstant("Serbian",                  new QoreBigIntNode(QLocale::Serbian));
   qlocale->addConstant("SerboCroatian",            new QoreBigIntNode(QLocale::SerboCroatian));
   qlocale->addConstant("Sesotho",                  new QoreBigIntNode(QLocale::Sesotho));
   qlocale->addConstant("Setswana",                 new QoreBigIntNode(QLocale::Setswana));
   qlocale->addConstant("Shona",                    new QoreBigIntNode(QLocale::Shona));
   qlocale->addConstant("Sindhi",                   new QoreBigIntNode(QLocale::Sindhi));
   qlocale->addConstant("Singhalese",               new QoreBigIntNode(QLocale::Singhalese));
   qlocale->addConstant("Siswati",                  new QoreBigIntNode(QLocale::Siswati));
   qlocale->addConstant("Slovak",                   new QoreBigIntNode(QLocale::Slovak));
   qlocale->addConstant("Slovenian",                new QoreBigIntNode(QLocale::Slovenian));
   qlocale->addConstant("Somali",                   new QoreBigIntNode(QLocale::Somali));
   qlocale->addConstant("Spanish",                  new QoreBigIntNode(QLocale::Spanish));
   qlocale->addConstant("Sundanese",                new QoreBigIntNode(QLocale::Sundanese));
   qlocale->addConstant("Swahili",                  new QoreBigIntNode(QLocale::Swahili));
   qlocale->addConstant("Swedish",                  new QoreBigIntNode(QLocale::Swedish));
   qlocale->addConstant("Tagalog",                  new QoreBigIntNode(QLocale::Tagalog));
   qlocale->addConstant("Tajik",                    new QoreBigIntNode(QLocale::Tajik));
   qlocale->addConstant("Tamil",                    new QoreBigIntNode(QLocale::Tamil));
   qlocale->addConstant("Tatar",                    new QoreBigIntNode(QLocale::Tatar));
   qlocale->addConstant("Telugu",                   new QoreBigIntNode(QLocale::Telugu));
   qlocale->addConstant("Thai",                     new QoreBigIntNode(QLocale::Thai));
   qlocale->addConstant("Tibetan",                  new QoreBigIntNode(QLocale::Tibetan));
   qlocale->addConstant("Tigrinya",                 new QoreBigIntNode(QLocale::Tigrinya));
   qlocale->addConstant("TongaLanguage",            new QoreBigIntNode(QLocale::TongaLanguage));
   qlocale->addConstant("Tsonga",                   new QoreBigIntNode(QLocale::Tsonga));
   qlocale->addConstant("Turkish",                  new QoreBigIntNode(QLocale::Turkish));
   qlocale->addConstant("Turkmen",                  new QoreBigIntNode(QLocale::Turkmen));
   qlocale->addConstant("Twi",                      new QoreBigIntNode(QLocale::Twi));
   qlocale->addConstant("Uigur",                    new QoreBigIntNode(QLocale::Uigur));
   qlocale->addConstant("Ukrainian",                new QoreBigIntNode(QLocale::Ukrainian));
   qlocale->addConstant("Urdu",                     new QoreBigIntNode(QLocale::Urdu));
   qlocale->addConstant("Uzbek",                    new QoreBigIntNode(QLocale::Uzbek));
   qlocale->addConstant("Vietnamese",               new QoreBigIntNode(QLocale::Vietnamese));
   qlocale->addConstant("Volapuk",                  new QoreBigIntNode(QLocale::Volapuk));
   qlocale->addConstant("Welsh",                    new QoreBigIntNode(QLocale::Welsh));
   qlocale->addConstant("Wolof",                    new QoreBigIntNode(QLocale::Wolof));
   qlocale->addConstant("Xhosa",                    new QoreBigIntNode(QLocale::Xhosa));
   qlocale->addConstant("Yiddish",                  new QoreBigIntNode(QLocale::Yiddish));
   qlocale->addConstant("Yoruba",                   new QoreBigIntNode(QLocale::Yoruba));
   qlocale->addConstant("Zhuang",                   new QoreBigIntNode(QLocale::Zhuang));
   qlocale->addConstant("Zulu",                     new QoreBigIntNode(QLocale::Zulu));
   qlocale->addConstant("NorwegianNynorsk",         new QoreBigIntNode(QLocale::NorwegianNynorsk));
   qlocale->addConstant("Nynorsk",                  new QoreBigIntNode(QLocale::Nynorsk));
   qlocale->addConstant("Bosnian",                  new QoreBigIntNode(QLocale::Bosnian));
   qlocale->addConstant("Divehi",                   new QoreBigIntNode(QLocale::Divehi));
   qlocale->addConstant("Manx",                     new QoreBigIntNode(QLocale::Manx));
   qlocale->addConstant("Cornish",                  new QoreBigIntNode(QLocale::Cornish));
   qlocale->addConstant("Akan",                     new QoreBigIntNode(QLocale::Akan));
   qlocale->addConstant("Konkani",                  new QoreBigIntNode(QLocale::Konkani));
   qlocale->addConstant("Ga",                       new QoreBigIntNode(QLocale::Ga));
   qlocale->addConstant("Igbo",                     new QoreBigIntNode(QLocale::Igbo));
   qlocale->addConstant("Kamba",                    new QoreBigIntNode(QLocale::Kamba));
   qlocale->addConstant("Syriac",                   new QoreBigIntNode(QLocale::Syriac));
   qlocale->addConstant("Blin",                     new QoreBigIntNode(QLocale::Blin));
   qlocale->addConstant("Geez",                     new QoreBigIntNode(QLocale::Geez));
   qlocale->addConstant("Koro",                     new QoreBigIntNode(QLocale::Koro));
   qlocale->addConstant("Sidamo",                   new QoreBigIntNode(QLocale::Sidamo));
   qlocale->addConstant("Atsam",                    new QoreBigIntNode(QLocale::Atsam));
   qlocale->addConstant("Tigre",                    new QoreBigIntNode(QLocale::Tigre));
   qlocale->addConstant("Jju",                      new QoreBigIntNode(QLocale::Jju));
   qlocale->addConstant("Friulian",                 new QoreBigIntNode(QLocale::Friulian));
   qlocale->addConstant("Venda",                    new QoreBigIntNode(QLocale::Venda));
   qlocale->addConstant("Ewe",                      new QoreBigIntNode(QLocale::Ewe));
   qlocale->addConstant("Walamo",                   new QoreBigIntNode(QLocale::Walamo));
   qlocale->addConstant("Hawaiian",                 new QoreBigIntNode(QLocale::Hawaiian));
   qlocale->addConstant("Tyap",                     new QoreBigIntNode(QLocale::Tyap));
   qlocale->addConstant("Chewa",                    new QoreBigIntNode(QLocale::Chewa));
   qlocale->addConstant("LastLanguage",             new QoreBigIntNode(QLocale::LastLanguage));

   // Country enum
   qlocale->addConstant("AnyCountry",               new QoreBigIntNode(QLocale::AnyCountry));
   qlocale->addConstant("Afghanistan",              new QoreBigIntNode(QLocale::Afghanistan));
   qlocale->addConstant("Albania",                  new QoreBigIntNode(QLocale::Albania));
   qlocale->addConstant("Algeria",                  new QoreBigIntNode(QLocale::Algeria));
   qlocale->addConstant("AmericanSamoa",            new QoreBigIntNode(QLocale::AmericanSamoa));
   qlocale->addConstant("Andorra",                  new QoreBigIntNode(QLocale::Andorra));
   qlocale->addConstant("Angola",                   new QoreBigIntNode(QLocale::Angola));
   qlocale->addConstant("Anguilla",                 new QoreBigIntNode(QLocale::Anguilla));
   qlocale->addConstant("Antarctica",               new QoreBigIntNode(QLocale::Antarctica));
   qlocale->addConstant("AntiguaAndBarbuda",        new QoreBigIntNode(QLocale::AntiguaAndBarbuda));
   qlocale->addConstant("Argentina",                new QoreBigIntNode(QLocale::Argentina));
   qlocale->addConstant("Armenia",                  new QoreBigIntNode(QLocale::Armenia));
   qlocale->addConstant("Aruba",                    new QoreBigIntNode(QLocale::Aruba));
   qlocale->addConstant("Australia",                new QoreBigIntNode(QLocale::Australia));
   qlocale->addConstant("Austria",                  new QoreBigIntNode(QLocale::Austria));
   qlocale->addConstant("Azerbaijan",               new QoreBigIntNode(QLocale::Azerbaijan));
   qlocale->addConstant("Bahamas",                  new QoreBigIntNode(QLocale::Bahamas));
   qlocale->addConstant("Bahrain",                  new QoreBigIntNode(QLocale::Bahrain));
   qlocale->addConstant("Bangladesh",               new QoreBigIntNode(QLocale::Bangladesh));
   qlocale->addConstant("Barbados",                 new QoreBigIntNode(QLocale::Barbados));
   qlocale->addConstant("Belarus",                  new QoreBigIntNode(QLocale::Belarus));
   qlocale->addConstant("Belgium",                  new QoreBigIntNode(QLocale::Belgium));
   qlocale->addConstant("Belize",                   new QoreBigIntNode(QLocale::Belize));
   qlocale->addConstant("Benin",                    new QoreBigIntNode(QLocale::Benin));
   qlocale->addConstant("Bermuda",                  new QoreBigIntNode(QLocale::Bermuda));
   qlocale->addConstant("Bhutan",                   new QoreBigIntNode(QLocale::Bhutan));
   qlocale->addConstant("Bolivia",                  new QoreBigIntNode(QLocale::Bolivia));
   qlocale->addConstant("BosniaAndHerzegowina",     new QoreBigIntNode(QLocale::BosniaAndHerzegowina));
   qlocale->addConstant("Botswana",                 new QoreBigIntNode(QLocale::Botswana));
   qlocale->addConstant("BouvetIsland",             new QoreBigIntNode(QLocale::BouvetIsland));
   qlocale->addConstant("Brazil",                   new QoreBigIntNode(QLocale::Brazil));
   qlocale->addConstant("BritishIndianOceanTerritory", new QoreBigIntNode(QLocale::BritishIndianOceanTerritory));
   qlocale->addConstant("BruneiDarussalam",         new QoreBigIntNode(QLocale::BruneiDarussalam));
   qlocale->addConstant("Bulgaria",                 new QoreBigIntNode(QLocale::Bulgaria));
   qlocale->addConstant("BurkinaFaso",              new QoreBigIntNode(QLocale::BurkinaFaso));
   qlocale->addConstant("Burundi",                  new QoreBigIntNode(QLocale::Burundi));
   qlocale->addConstant("Cambodia",                 new QoreBigIntNode(QLocale::Cambodia));
   qlocale->addConstant("Cameroon",                 new QoreBigIntNode(QLocale::Cameroon));
   qlocale->addConstant("Canada",                   new QoreBigIntNode(QLocale::Canada));
   qlocale->addConstant("CapeVerde",                new QoreBigIntNode(QLocale::CapeVerde));
   qlocale->addConstant("CaymanIslands",            new QoreBigIntNode(QLocale::CaymanIslands));
   qlocale->addConstant("CentralAfricanRepublic",   new QoreBigIntNode(QLocale::CentralAfricanRepublic));
   qlocale->addConstant("Chad",                     new QoreBigIntNode(QLocale::Chad));
   qlocale->addConstant("Chile",                    new QoreBigIntNode(QLocale::Chile));
   qlocale->addConstant("China",                    new QoreBigIntNode(QLocale::China));
   qlocale->addConstant("ChristmasIsland",          new QoreBigIntNode(QLocale::ChristmasIsland));
   qlocale->addConstant("CocosIslands",             new QoreBigIntNode(QLocale::CocosIslands));
   qlocale->addConstant("Colombia",                 new QoreBigIntNode(QLocale::Colombia));
   qlocale->addConstant("Comoros",                  new QoreBigIntNode(QLocale::Comoros));
   qlocale->addConstant("DemocraticRepublicOfCongo", new QoreBigIntNode(QLocale::DemocraticRepublicOfCongo));
   qlocale->addConstant("PeoplesRepublicOfCongo",   new QoreBigIntNode(QLocale::PeoplesRepublicOfCongo));
   qlocale->addConstant("CookIslands",              new QoreBigIntNode(QLocale::CookIslands));
   qlocale->addConstant("CostaRica",                new QoreBigIntNode(QLocale::CostaRica));
   qlocale->addConstant("IvoryCoast",               new QoreBigIntNode(QLocale::IvoryCoast));
   qlocale->addConstant("Croatia",                  new QoreBigIntNode(QLocale::Croatia));
   qlocale->addConstant("Cuba",                     new QoreBigIntNode(QLocale::Cuba));
   qlocale->addConstant("Cyprus",                   new QoreBigIntNode(QLocale::Cyprus));
   qlocale->addConstant("CzechRepublic",            new QoreBigIntNode(QLocale::CzechRepublic));
   qlocale->addConstant("Denmark",                  new QoreBigIntNode(QLocale::Denmark));
   qlocale->addConstant("Djibouti",                 new QoreBigIntNode(QLocale::Djibouti));
   qlocale->addConstant("Dominica",                 new QoreBigIntNode(QLocale::Dominica));
   qlocale->addConstant("DominicanRepublic",        new QoreBigIntNode(QLocale::DominicanRepublic));
   qlocale->addConstant("EastTimor",                new QoreBigIntNode(QLocale::EastTimor));
   qlocale->addConstant("Ecuador",                  new QoreBigIntNode(QLocale::Ecuador));
   qlocale->addConstant("Egypt",                    new QoreBigIntNode(QLocale::Egypt));
   qlocale->addConstant("ElSalvador",               new QoreBigIntNode(QLocale::ElSalvador));
   qlocale->addConstant("EquatorialGuinea",         new QoreBigIntNode(QLocale::EquatorialGuinea));
   qlocale->addConstant("Eritrea",                  new QoreBigIntNode(QLocale::Eritrea));
   qlocale->addConstant("Estonia",                  new QoreBigIntNode(QLocale::Estonia));
   qlocale->addConstant("Ethiopia",                 new QoreBigIntNode(QLocale::Ethiopia));
   qlocale->addConstant("FalklandIslands",          new QoreBigIntNode(QLocale::FalklandIslands));
   qlocale->addConstant("FaroeIslands",             new QoreBigIntNode(QLocale::FaroeIslands));
   qlocale->addConstant("FijiCountry",              new QoreBigIntNode(QLocale::FijiCountry));
   qlocale->addConstant("Finland",                  new QoreBigIntNode(QLocale::Finland));
   qlocale->addConstant("France",                   new QoreBigIntNode(QLocale::France));
   qlocale->addConstant("MetropolitanFrance",       new QoreBigIntNode(QLocale::MetropolitanFrance));
   qlocale->addConstant("FrenchGuiana",             new QoreBigIntNode(QLocale::FrenchGuiana));
   qlocale->addConstant("FrenchPolynesia",          new QoreBigIntNode(QLocale::FrenchPolynesia));
   qlocale->addConstant("FrenchSouthernTerritories", new QoreBigIntNode(QLocale::FrenchSouthernTerritories));
   qlocale->addConstant("Gabon",                    new QoreBigIntNode(QLocale::Gabon));
   qlocale->addConstant("Gambia",                   new QoreBigIntNode(QLocale::Gambia));
   qlocale->addConstant("Georgia",                  new QoreBigIntNode(QLocale::Georgia));
   qlocale->addConstant("Germany",                  new QoreBigIntNode(QLocale::Germany));
   qlocale->addConstant("Ghana",                    new QoreBigIntNode(QLocale::Ghana));
   qlocale->addConstant("Gibraltar",                new QoreBigIntNode(QLocale::Gibraltar));
   qlocale->addConstant("Greece",                   new QoreBigIntNode(QLocale::Greece));
   qlocale->addConstant("Greenland",                new QoreBigIntNode(QLocale::Greenland));
   qlocale->addConstant("Grenada",                  new QoreBigIntNode(QLocale::Grenada));
   qlocale->addConstant("Guadeloupe",               new QoreBigIntNode(QLocale::Guadeloupe));
   qlocale->addConstant("Guam",                     new QoreBigIntNode(QLocale::Guam));
   qlocale->addConstant("Guatemala",                new QoreBigIntNode(QLocale::Guatemala));
   qlocale->addConstant("Guinea",                   new QoreBigIntNode(QLocale::Guinea));
   qlocale->addConstant("GuineaBissau",             new QoreBigIntNode(QLocale::GuineaBissau));
   qlocale->addConstant("Guyana",                   new QoreBigIntNode(QLocale::Guyana));
   qlocale->addConstant("Haiti",                    new QoreBigIntNode(QLocale::Haiti));
   qlocale->addConstant("HeardAndMcDonaldIslands",  new QoreBigIntNode(QLocale::HeardAndMcDonaldIslands));
   qlocale->addConstant("Honduras",                 new QoreBigIntNode(QLocale::Honduras));
   qlocale->addConstant("HongKong",                 new QoreBigIntNode(QLocale::HongKong));
   qlocale->addConstant("Hungary",                  new QoreBigIntNode(QLocale::Hungary));
   qlocale->addConstant("Iceland",                  new QoreBigIntNode(QLocale::Iceland));
   qlocale->addConstant("India",                    new QoreBigIntNode(QLocale::India));
   qlocale->addConstant("Indonesia",                new QoreBigIntNode(QLocale::Indonesia));
   qlocale->addConstant("Iran",                     new QoreBigIntNode(QLocale::Iran));
   qlocale->addConstant("Iraq",                     new QoreBigIntNode(QLocale::Iraq));
   qlocale->addConstant("Ireland",                  new QoreBigIntNode(QLocale::Ireland));
   qlocale->addConstant("Israel",                   new QoreBigIntNode(QLocale::Israel));
   qlocale->addConstant("Italy",                    new QoreBigIntNode(QLocale::Italy));
   qlocale->addConstant("Jamaica",                  new QoreBigIntNode(QLocale::Jamaica));
   qlocale->addConstant("Japan",                    new QoreBigIntNode(QLocale::Japan));
   qlocale->addConstant("Jordan",                   new QoreBigIntNode(QLocale::Jordan));
   qlocale->addConstant("Kazakhstan",               new QoreBigIntNode(QLocale::Kazakhstan));
   qlocale->addConstant("Kenya",                    new QoreBigIntNode(QLocale::Kenya));
   qlocale->addConstant("Kiribati",                 new QoreBigIntNode(QLocale::Kiribati));
   qlocale->addConstant("DemocraticRepublicOfKorea", new QoreBigIntNode(QLocale::DemocraticRepublicOfKorea));
   qlocale->addConstant("RepublicOfKorea",          new QoreBigIntNode(QLocale::RepublicOfKorea));
   qlocale->addConstant("Kuwait",                   new QoreBigIntNode(QLocale::Kuwait));
   qlocale->addConstant("Kyrgyzstan",               new QoreBigIntNode(QLocale::Kyrgyzstan));
   qlocale->addConstant("Lao",                      new QoreBigIntNode(QLocale::Lao));
   qlocale->addConstant("Latvia",                   new QoreBigIntNode(QLocale::Latvia));
   qlocale->addConstant("Lebanon",                  new QoreBigIntNode(QLocale::Lebanon));
   qlocale->addConstant("Lesotho",                  new QoreBigIntNode(QLocale::Lesotho));
   qlocale->addConstant("Liberia",                  new QoreBigIntNode(QLocale::Liberia));
   qlocale->addConstant("LibyanArabJamahiriya",     new QoreBigIntNode(QLocale::LibyanArabJamahiriya));
   qlocale->addConstant("Liechtenstein",            new QoreBigIntNode(QLocale::Liechtenstein));
   qlocale->addConstant("Lithuania",                new QoreBigIntNode(QLocale::Lithuania));
   qlocale->addConstant("Luxembourg",               new QoreBigIntNode(QLocale::Luxembourg));
   qlocale->addConstant("Macau",                    new QoreBigIntNode(QLocale::Macau));
   qlocale->addConstant("Macedonia",                new QoreBigIntNode(QLocale::Macedonia));
   qlocale->addConstant("Madagascar",               new QoreBigIntNode(QLocale::Madagascar));
   qlocale->addConstant("Malawi",                   new QoreBigIntNode(QLocale::Malawi));
   qlocale->addConstant("Malaysia",                 new QoreBigIntNode(QLocale::Malaysia));
   qlocale->addConstant("Maldives",                 new QoreBigIntNode(QLocale::Maldives));
   qlocale->addConstant("Mali",                     new QoreBigIntNode(QLocale::Mali));
   qlocale->addConstant("Malta",                    new QoreBigIntNode(QLocale::Malta));
   qlocale->addConstant("MarshallIslands",          new QoreBigIntNode(QLocale::MarshallIslands));
   qlocale->addConstant("Martinique",               new QoreBigIntNode(QLocale::Martinique));
   qlocale->addConstant("Mauritania",               new QoreBigIntNode(QLocale::Mauritania));
   qlocale->addConstant("Mauritius",                new QoreBigIntNode(QLocale::Mauritius));
   qlocale->addConstant("Mayotte",                  new QoreBigIntNode(QLocale::Mayotte));
   qlocale->addConstant("Mexico",                   new QoreBigIntNode(QLocale::Mexico));
   qlocale->addConstant("Micronesia",               new QoreBigIntNode(QLocale::Micronesia));
   qlocale->addConstant("Moldova",                  new QoreBigIntNode(QLocale::Moldova));
   qlocale->addConstant("Monaco",                   new QoreBigIntNode(QLocale::Monaco));
   qlocale->addConstant("Mongolia",                 new QoreBigIntNode(QLocale::Mongolia));
   qlocale->addConstant("Montserrat",               new QoreBigIntNode(QLocale::Montserrat));
   qlocale->addConstant("Morocco",                  new QoreBigIntNode(QLocale::Morocco));
   qlocale->addConstant("Mozambique",               new QoreBigIntNode(QLocale::Mozambique));
   qlocale->addConstant("Myanmar",                  new QoreBigIntNode(QLocale::Myanmar));
   qlocale->addConstant("Namibia",                  new QoreBigIntNode(QLocale::Namibia));
   qlocale->addConstant("NauruCountry",             new QoreBigIntNode(QLocale::NauruCountry));
   qlocale->addConstant("Nepal",                    new QoreBigIntNode(QLocale::Nepal));
   qlocale->addConstant("Netherlands",              new QoreBigIntNode(QLocale::Netherlands));
   qlocale->addConstant("NetherlandsAntilles",      new QoreBigIntNode(QLocale::NetherlandsAntilles));
   qlocale->addConstant("NewCaledonia",             new QoreBigIntNode(QLocale::NewCaledonia));
   qlocale->addConstant("NewZealand",               new QoreBigIntNode(QLocale::NewZealand));
   qlocale->addConstant("Nicaragua",                new QoreBigIntNode(QLocale::Nicaragua));
   qlocale->addConstant("Niger",                    new QoreBigIntNode(QLocale::Niger));
   qlocale->addConstant("Nigeria",                  new QoreBigIntNode(QLocale::Nigeria));
   qlocale->addConstant("Niue",                     new QoreBigIntNode(QLocale::Niue));
   qlocale->addConstant("NorfolkIsland",            new QoreBigIntNode(QLocale::NorfolkIsland));
   qlocale->addConstant("NorthernMarianaIslands",   new QoreBigIntNode(QLocale::NorthernMarianaIslands));
   qlocale->addConstant("Norway",                   new QoreBigIntNode(QLocale::Norway));
   qlocale->addConstant("Oman",                     new QoreBigIntNode(QLocale::Oman));
   qlocale->addConstant("Pakistan",                 new QoreBigIntNode(QLocale::Pakistan));
   qlocale->addConstant("Palau",                    new QoreBigIntNode(QLocale::Palau));
   qlocale->addConstant("PalestinianTerritory",     new QoreBigIntNode(QLocale::PalestinianTerritory));
   qlocale->addConstant("Panama",                   new QoreBigIntNode(QLocale::Panama));
   qlocale->addConstant("PapuaNewGuinea",           new QoreBigIntNode(QLocale::PapuaNewGuinea));
   qlocale->addConstant("Paraguay",                 new QoreBigIntNode(QLocale::Paraguay));
   qlocale->addConstant("Peru",                     new QoreBigIntNode(QLocale::Peru));
   qlocale->addConstant("Philippines",              new QoreBigIntNode(QLocale::Philippines));
   qlocale->addConstant("Pitcairn",                 new QoreBigIntNode(QLocale::Pitcairn));
   qlocale->addConstant("Poland",                   new QoreBigIntNode(QLocale::Poland));
   qlocale->addConstant("Portugal",                 new QoreBigIntNode(QLocale::Portugal));
   qlocale->addConstant("PuertoRico",               new QoreBigIntNode(QLocale::PuertoRico));
   qlocale->addConstant("Qatar",                    new QoreBigIntNode(QLocale::Qatar));
   qlocale->addConstant("Reunion",                  new QoreBigIntNode(QLocale::Reunion));
   qlocale->addConstant("Romania",                  new QoreBigIntNode(QLocale::Romania));
   qlocale->addConstant("RussianFederation",        new QoreBigIntNode(QLocale::RussianFederation));
   qlocale->addConstant("Rwanda",                   new QoreBigIntNode(QLocale::Rwanda));
   qlocale->addConstant("SaintKittsAndNevis",       new QoreBigIntNode(QLocale::SaintKittsAndNevis));
   qlocale->addConstant("StLucia",                  new QoreBigIntNode(QLocale::StLucia));
   qlocale->addConstant("StVincentAndTheGrenadines", new QoreBigIntNode(QLocale::StVincentAndTheGrenadines));
   qlocale->addConstant("Samoa",                    new QoreBigIntNode(QLocale::Samoa));
   qlocale->addConstant("SanMarino",                new QoreBigIntNode(QLocale::SanMarino));
   qlocale->addConstant("SaoTomeAndPrincipe",       new QoreBigIntNode(QLocale::SaoTomeAndPrincipe));
   qlocale->addConstant("SaudiArabia",              new QoreBigIntNode(QLocale::SaudiArabia));
   qlocale->addConstant("Senegal",                  new QoreBigIntNode(QLocale::Senegal));
   qlocale->addConstant("Seychelles",               new QoreBigIntNode(QLocale::Seychelles));
   qlocale->addConstant("SierraLeone",              new QoreBigIntNode(QLocale::SierraLeone));
   qlocale->addConstant("Singapore",                new QoreBigIntNode(QLocale::Singapore));
   qlocale->addConstant("Slovakia",                 new QoreBigIntNode(QLocale::Slovakia));
   qlocale->addConstant("Slovenia",                 new QoreBigIntNode(QLocale::Slovenia));
   qlocale->addConstant("SolomonIslands",           new QoreBigIntNode(QLocale::SolomonIslands));
   qlocale->addConstant("Somalia",                  new QoreBigIntNode(QLocale::Somalia));
   qlocale->addConstant("SouthAfrica",              new QoreBigIntNode(QLocale::SouthAfrica));
   qlocale->addConstant("SouthGeorgiaAndTheSouthSandwichIslands", new QoreBigIntNode(QLocale::SouthGeorgiaAndTheSouthSandwichIslands));
   qlocale->addConstant("Spain",                    new QoreBigIntNode(QLocale::Spain));
   qlocale->addConstant("SriLanka",                 new QoreBigIntNode(QLocale::SriLanka));
   qlocale->addConstant("StHelena",                 new QoreBigIntNode(QLocale::StHelena));
   qlocale->addConstant("StPierreAndMiquelon",      new QoreBigIntNode(QLocale::StPierreAndMiquelon));
   qlocale->addConstant("Sudan",                    new QoreBigIntNode(QLocale::Sudan));
   qlocale->addConstant("Suriname",                 new QoreBigIntNode(QLocale::Suriname));
   qlocale->addConstant("SvalbardAndJanMayenIslands", new QoreBigIntNode(QLocale::SvalbardAndJanMayenIslands));
   qlocale->addConstant("Swaziland",                new QoreBigIntNode(QLocale::Swaziland));
   qlocale->addConstant("Sweden",                   new QoreBigIntNode(QLocale::Sweden));
   qlocale->addConstant("Switzerland",              new QoreBigIntNode(QLocale::Switzerland));
   qlocale->addConstant("SyrianArabRepublic",       new QoreBigIntNode(QLocale::SyrianArabRepublic));
   qlocale->addConstant("Taiwan",                   new QoreBigIntNode(QLocale::Taiwan));
   qlocale->addConstant("Tajikistan",               new QoreBigIntNode(QLocale::Tajikistan));
   qlocale->addConstant("Tanzania",                 new QoreBigIntNode(QLocale::Tanzania));
   qlocale->addConstant("Thailand",                 new QoreBigIntNode(QLocale::Thailand));
   qlocale->addConstant("Togo",                     new QoreBigIntNode(QLocale::Togo));
   qlocale->addConstant("Tokelau",                  new QoreBigIntNode(QLocale::Tokelau));
   qlocale->addConstant("TongaCountry",             new QoreBigIntNode(QLocale::TongaCountry));
   qlocale->addConstant("TrinidadAndTobago",        new QoreBigIntNode(QLocale::TrinidadAndTobago));
   qlocale->addConstant("Tunisia",                  new QoreBigIntNode(QLocale::Tunisia));
   qlocale->addConstant("Turkey",                   new QoreBigIntNode(QLocale::Turkey));
   qlocale->addConstant("Turkmenistan",             new QoreBigIntNode(QLocale::Turkmenistan));
   qlocale->addConstant("TurksAndCaicosIslands",    new QoreBigIntNode(QLocale::TurksAndCaicosIslands));
   qlocale->addConstant("Tuvalu",                   new QoreBigIntNode(QLocale::Tuvalu));
   qlocale->addConstant("Uganda",                   new QoreBigIntNode(QLocale::Uganda));
   qlocale->addConstant("Ukraine",                  new QoreBigIntNode(QLocale::Ukraine));
   qlocale->addConstant("UnitedArabEmirates",       new QoreBigIntNode(QLocale::UnitedArabEmirates));
   qlocale->addConstant("UnitedKingdom",            new QoreBigIntNode(QLocale::UnitedKingdom));
   qlocale->addConstant("UnitedStates",             new QoreBigIntNode(QLocale::UnitedStates));
   qlocale->addConstant("UnitedStatesMinorOutlyingIslands", new QoreBigIntNode(QLocale::UnitedStatesMinorOutlyingIslands));
   qlocale->addConstant("Uruguay",                  new QoreBigIntNode(QLocale::Uruguay));
   qlocale->addConstant("Uzbekistan",               new QoreBigIntNode(QLocale::Uzbekistan));
   qlocale->addConstant("Vanuatu",                  new QoreBigIntNode(QLocale::Vanuatu));
   qlocale->addConstant("VaticanCityState",         new QoreBigIntNode(QLocale::VaticanCityState));
   qlocale->addConstant("Venezuela",                new QoreBigIntNode(QLocale::Venezuela));
   qlocale->addConstant("VietNam",                  new QoreBigIntNode(QLocale::VietNam));
   qlocale->addConstant("BritishVirginIslands",     new QoreBigIntNode(QLocale::BritishVirginIslands));
   qlocale->addConstant("USVirginIslands",          new QoreBigIntNode(QLocale::USVirginIslands));
   qlocale->addConstant("WallisAndFutunaIslands",   new QoreBigIntNode(QLocale::WallisAndFutunaIslands));
   qlocale->addConstant("WesternSahara",            new QoreBigIntNode(QLocale::WesternSahara));
   qlocale->addConstant("Yemen",                    new QoreBigIntNode(QLocale::Yemen));
   qlocale->addConstant("Yugoslavia",               new QoreBigIntNode(QLocale::Yugoslavia));
   qlocale->addConstant("Zambia",                   new QoreBigIntNode(QLocale::Zambia));
   qlocale->addConstant("Zimbabwe",                 new QoreBigIntNode(QLocale::Zimbabwe));
   qlocale->addConstant("SerbiaAndMontenegro",      new QoreBigIntNode(QLocale::SerbiaAndMontenegro));
   qlocale->addConstant("LastCountry",              new QoreBigIntNode(QLocale::LastCountry));

   return qlocale;
}
