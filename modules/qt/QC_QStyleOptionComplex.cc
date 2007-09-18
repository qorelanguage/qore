/*
 QC_QStyleOptionComplex.cc
 
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

#include "QC_QStyleOptionComplex.h"

int CID_QSTYLEOPTIONCOMPLEX;
class QoreClass *QC_QStyleOptionComplex = 0;

//QStyleOptionComplex ( int version = QStyleOptionComplex::Version, int type = SO_Complex )
//QStyleOptionComplex ( const QStyleOptionComplex & other )
static void QSTYLEOPTIONCOMPLEX_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int version = !is_nothing(p) ? p->getAsInt() : QStyleOptionComplex::Version;
   p = get_param(params, 1);
   int type = !is_nothing(p) ? p->getAsInt() : QStyleOption::SO_Complex;
   self->setPrivate(CID_QSTYLEOPTIONCOMPLEX, new QoreQStyleOptionComplex(version, type));
   return;
}

static void QSTYLEOPTIONCOMPLEX_copy(class Object *self, class Object *old, class QoreQStyleOptionComplex *qsoc, ExceptionSink *xsink)
{
   xsink->raiseException("QSTYLEOPTIONCOMPLEX-COPY-ERROR", "objects of this class cannot be copied");
}

QoreClass *initQStyleOptionComplexClass(QoreClass *qstyleoption)
{
   QC_QStyleOptionComplex = new QoreClass("QStyleOptionComplex", QDOM_GUI);
   CID_QSTYLEOPTIONCOMPLEX = QC_QStyleOptionComplex->getID();

   QC_QStyleOptionComplex->addBuiltinVirtualBaseClass(qstyleoption);

   QC_QStyleOptionComplex->setConstructor(QSTYLEOPTIONCOMPLEX_constructor);
   QC_QStyleOptionComplex->setCopy((q_copy_t)QSTYLEOPTIONCOMPLEX_copy);

   return QC_QStyleOptionComplex;
}
