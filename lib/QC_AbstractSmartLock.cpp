/*
 QC_AbstractSmartLock.cpp
 
 Qore Programming Language
 
 Copyright 2003 - 2010 David Nichols
 
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
#include <qore/intern/QC_AbstractSmartLock.h>

qore_classid_t CID_ABSTRACTSMARTLOCK;

static void ASL_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   xsink->raiseException("ABSTRACTSMARTLOCK-CONSTRUCTOR-ERROR", "this class is an abstract class and cannot be constructed directly or inherited directly by a user-defined class");
}

static AbstractQoreNode *ASL_getName(QoreObject *self, AbstractSmartLock *asl, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreStringNode(asl->getName());
}

QoreClass *initAbstractSmartLockClass() {
   QORE_TRACE("initAbstractSmartLockClass()");
   
   QoreClass *QC_AbstractSmartLock = new QoreClass("AbstractSmartLock", QDOM_THREAD_CLASS);
   CID_ABSTRACTSMARTLOCK = QC_AbstractSmartLock->getID();
   QC_AbstractSmartLock->setConstructorExtended(ASL_constructor);

   QC_AbstractSmartLock->addMethodExtended("getName", (q_method_t)ASL_getName, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringTypeInfo);
   
   return QC_AbstractSmartLock;
}
