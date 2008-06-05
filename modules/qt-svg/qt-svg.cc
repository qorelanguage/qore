/*
  qt-svg.cc
  
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

#include "QC_QSvgRenderer.h"
#include "QC_QSvgGenerator.h"
#include "QC_QSvgWidget.h"

#include "QC_QObject.h"
#include "QC_QWidget.h"

static class QoreStringNode *qt_svg_module_init();
static void qt_svg_module_ns_init(QoreNamespace *rns, QoreNamespace *qns);
static void qt_svg_module_delete();

#ifndef QORE_MONOLITHIC
DLLEXPORT char qore_module_name[] = "qt-svg";
DLLEXPORT char qore_module_version[] = "0.1";
DLLEXPORT char qore_module_description[] = "QT 4 SVG module";
DLLEXPORT char qore_module_author[] = "David Nichols";
DLLEXPORT char qore_module_url[] = "http://www.qoretechnologies.com/qore";
DLLEXPORT int qore_module_api_major = QORE_MODULE_API_MAJOR;
DLLEXPORT int qore_module_api_minor = QORE_MODULE_API_MINOR;
DLLEXPORT qore_module_init_t qore_module_init = qt_svg_module_init;
DLLEXPORT qore_module_ns_init_t qore_module_ns_init = qt_svg_module_ns_init;
DLLEXPORT qore_module_delete_t qore_module_delete = qt_svg_module_delete;
DLLEXPORT const char *qore_module_dependencies[] = { "qt-gui", 0 };
DLLEXPORT qore_license_t qore_module_license = QL_GPL;
#endif

static QoreNamespace svg_ns("QtSvg");

static void init_namespace()
{
   svg_ns.addSystemClass(initQSvgRendererClass(QC_QObject));
   svg_ns.addSystemClass(initQSvgGeneratorClass());
   svg_ns.addSystemClass(initQSvgWidgetClass(QC_QWidget));
}

static QoreStringNode *qt_svg_module_init()
{
   // initialize namespace
   init_namespace();

   return 0;
}

static void qt_svg_module_ns_init(QoreNamespace *rns, QoreNamespace *qns)
{
   qns->addInitialNamespace(svg_ns.copy());
}

static void qt_svg_module_delete()
{
}
