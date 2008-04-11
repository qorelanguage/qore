/*
 QC_QLibraryInfo.cc
 
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

#include "QC_QLibraryInfo.h"

QoreNamespace *initQLibraryInfoNS()
{
   QoreNamespace *ns = new QoreNamespace("QLibraryInfo");

   // LibraryLocation enum
   ns->addConstant("PrefixPath",               new QoreBigIntNode(QLibraryInfo::PrefixPath));
   ns->addConstant("DocumentationPath",        new QoreBigIntNode(QLibraryInfo::DocumentationPath));
   ns->addConstant("HeadersPath",              new QoreBigIntNode(QLibraryInfo::HeadersPath));
   ns->addConstant("LibrariesPath",            new QoreBigIntNode(QLibraryInfo::LibrariesPath));
   ns->addConstant("BinariesPath",             new QoreBigIntNode(QLibraryInfo::BinariesPath));
   ns->addConstant("PluginsPath",              new QoreBigIntNode(QLibraryInfo::PluginsPath));
   ns->addConstant("DataPath",                 new QoreBigIntNode(QLibraryInfo::DataPath));
   ns->addConstant("TranslationsPath",         new QoreBigIntNode(QLibraryInfo::TranslationsPath));
   ns->addConstant("SettingsPath",             new QoreBigIntNode(QLibraryInfo::SettingsPath));
   ns->addConstant("DemosPath",                new QoreBigIntNode(QLibraryInfo::DemosPath));
   ns->addConstant("ExamplesPath",             new QoreBigIntNode(QLibraryInfo::ExamplesPath));

   return ns;
}

//QString buildKey ()
static AbstractQoreNode *f_QLibraryInfo_buildKey(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(QLibraryInfo::buildKey().toUtf8().data(), QCS_UTF8);
}

//QString licensedProducts ()
static AbstractQoreNode *f_QLibraryInfo_licensedProducts(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(QLibraryInfo::licensedProducts().toUtf8().data(), QCS_UTF8);
}

//QString licensee ()
static AbstractQoreNode *f_QLibraryInfo_licensee(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(QLibraryInfo::licensee().toUtf8().data(), QCS_UTF8);
}

//QString location ( LibraryLocation loc )
static AbstractQoreNode *f_QLibraryInfo_location(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QLibraryInfo::LibraryLocation loc = (QLibraryInfo::LibraryLocation)(p ? p->getAsInt() : 0);
   return new QoreStringNode(QLibraryInfo::location(loc).toUtf8().data(), QCS_UTF8);
}

void initQLibraryInfoStaticFunctions()
{
   builtinFunctions.add("QLibraryInfo_buildKey",                     f_QLibraryInfo_buildKey, QDOM_GUI);
   builtinFunctions.add("QLibraryInfo_licensedProducts",             f_QLibraryInfo_licensedProducts, QDOM_GUI);
   builtinFunctions.add("QLibraryInfo_licensee",                     f_QLibraryInfo_licensee, QDOM_GUI);
   builtinFunctions.add("QLibraryInfo_location",                     f_QLibraryInfo_location, QDOM_GUI);
}
