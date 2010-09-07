/*
 QC_SQLStatement.cpp
 
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
#include <qore/intern/QC_SQLStatement.h>
#include <qore/intern/QC_Datasource.h>
#include <qore/intern/QC_DatasourcePool.h>

qore_classid_t CID_SQLSTATEMENT;

// SQLStatement::constructor(Datasource *ds)
static void SQLSTATEMENT_constructor_ds(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_OBJ_DATA(ds, ManagedDatasource, params, 0, CID_DATASOURCE, "Datasource", "SQLStatment::constructor", xsink);
   if (*xsink)
      return;

   self->setPrivate(CID_SQLSTATEMENT, new QoreSQLStatement(ds->getReferencedHelper()));
}

static void SQLSTATEMENT_destructor(QoreObject *self, QoreSQLStatement *stmt, ExceptionSink *xsink) {
   stmt->destructor(xsink);
   stmt->deref(xsink);
}

/*
static void SQLSTATEMENT_copy(QoreObject *self, QoreObject *old, QoreSQLStatement *stmt, ExceptionSink *xsink) {
   self->setPrivate(CID_SQLSTATEMENT, new SQLStatement);
}
*/

/*
static AbstractQoreNode *SQLSTATEMENT_lock(QoreObject *self, SQLStatement *m, const QoreListNode *params, ExceptionSink *xsink) {
   m->grab(xsink);
   return 0;
}
*/

QoreClass *initSQLStatementClass(QoreClass *QC_Datasource, QoreClass *QC_DatasourcePool) {
   QORE_TRACE("initSQLStatementClass()");

   QoreClass *QC_SQLSTATEMENT = new QoreClass("SQLStatement", QDOM_DATABASE);
   CID_SQLSTATEMENT = QC_SQLSTATEMENT->getID();

   QC_SQLSTATEMENT->setConstructorExtended(SQLSTATEMENT_constructor_ds, false, QC_NO_FLAGS, QDOM_DATABASE, 1, QC_Datasource->getTypeInfo(), QORE_PARAM_NO_ARG);
   //QC_SQLSTATEMENT->setConstructorExtended(SQLSTATEMENT_constructor_dsp, false, QC_NO_FLAGS, QDOM_DATABASE, 1, QC_DatasourcePool->getTypeInfo(), QORE_PARAM_NO_ARG);

   QC_SQLSTATEMENT->setDestructor((q_destructor_t)SQLSTATEMENT_destructor);
   //QC_SQLSTATEMENT->setCopy((q_copy_t)SQLSTATEMENT_copy);

   //QC_SQLSTATEMENT->addMethodExtended("lock",     (q_method_t)SQLSTATEMENT_lock, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   return QC_SQLSTATEMENT;
}
