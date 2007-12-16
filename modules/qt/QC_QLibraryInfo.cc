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
   ns->addConstant("PrefixPath",               new QoreNode((int64)QLibraryInfo::PrefixPath));
   ns->addConstant("DocumentationPath",        new QoreNode((int64)QLibraryInfo::DocumentationPath));
   ns->addConstant("HeadersPath",              new QoreNode((int64)QLibraryInfo::HeadersPath));
   ns->addConstant("LibrariesPath",            new QoreNode((int64)QLibraryInfo::LibrariesPath));
   ns->addConstant("BinariesPath",             new QoreNode((int64)QLibraryInfo::BinariesPath));
   ns->addConstant("PluginsPath",              new QoreNode((int64)QLibraryInfo::PluginsPath));
   ns->addConstant("DataPath",                 new QoreNode((int64)QLibraryInfo::DataPath));
   ns->addConstant("TranslationsPath",         new QoreNode((int64)QLibraryInfo::TranslationsPath));
   ns->addConstant("SettingsPath",             new QoreNode((int64)QLibraryInfo::SettingsPath));
   ns->addConstant("DemosPath",                new QoreNode((int64)QLibraryInfo::DemosPath));
   ns->addConstant("ExamplesPath",             new QoreNode((int64)QLibraryInfo::ExamplesPath));

   return ns;
}

//QString buildKey ()
static QoreNode *f_QLibraryInfo_buildKey(const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(QLibraryInfo::buildKey().toUtf8().data(), QCS_UTF8));
}

//QString licensedProducts ()
static QoreNode *f_QLibraryInfo_licensedProducts(const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(QLibraryInfo::licensedProducts().toUtf8().data(), QCS_UTF8));
}

//QString licensee ()
static QoreNode *f_QLibraryInfo_licensee(const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(QLibraryInfo::licensee().toUtf8().data(), QCS_UTF8));
}

//QString location ( LibraryLocation loc )
static QoreNode *f_QLibraryInfo_location(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QLibraryInfo::LibraryLocation loc = (QLibraryInfo::LibraryLocation)(p ? p->getAsInt() : 0);
   return new QoreNode(new QoreString(QLibraryInfo::location(loc).toUtf8().data(), QCS_UTF8));
}

void initQLibraryInfoStaticFunctions()
{
   builtinFunctions.add("QLibraryInfo_buildKey",                     f_QLibraryInfo_buildKey);
   builtinFunctions.add("QLibraryInfo_licensedProducts",             f_QLibraryInfo_licensedProducts);
   builtinFunctions.add("QLibraryInfo_licensee",                     f_QLibraryInfo_licensee);
   builtinFunctions.add("QLibraryInfo_location",                     f_QLibraryInfo_location);
}
