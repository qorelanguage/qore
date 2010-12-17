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

// SQLStatement::constructor(Datasource $ds)
static void SQLSTATEMENT_constructor_ds(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_OBJ_DATA(ds, ManagedDatasource, args, 0, CID_DATASOURCE, "Datasource", "SQLStatment::constructor", xsink);
   if (*xsink)
      return;

   ReferenceHolder<ManagedDatasource> ds_holder(ds, xsink);

   if (!ds->getDriver()->hasStatementAPI()) {
      xsink->raiseException("SQLSTATEMENT-ERROR", "DBI driver '%s' does not support the prepared statement API", ds->getDriver()->getName());
      return;
   }

   ReferenceHolder<QoreSQLStatement> ss(new QoreSQLStatement, xsink);
   // FIXME: reuse reference from this call
   ss->init(ds->getReferencedHelper(*ss));
   self->setPrivate(CID_SQLSTATEMENT, ss.release());
}

// SQLStatement::constructor(DatasourcePool $dsp)
static void SQLSTATEMENT_constructor_dsp(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_OBJ_DATA(dsp, DatasourcePool, args, 0, CID_DATASOURCEPOOL, "DatasourcePool", "SQLStatment::constructor", xsink);
   if (*xsink)
      return;

   ReferenceHolder<DatasourcePool> dsp_holder(dsp, xsink);

   if (!dsp->getDriver()->hasStatementAPI()) {
      xsink->raiseException("SQLSTATEMENT-ERROR", "DBI driver '%s' does not support the prepared statement API", dsp->getDriver()->getName());
      return;
   }

   ReferenceHolder<QoreSQLStatement> ss(new QoreSQLStatement, xsink);
   // FIXME: reuse reference from this call
   ss->init(dsp->getReferencedHelper(*ss));
   self->setPrivate(CID_SQLSTATEMENT, ss.release());
}

static void SQLSTATEMENT_destructor(QoreObject *self, QoreSQLStatement *stmt, ExceptionSink *xsink) {
   stmt->close(xsink);
   stmt->deref(xsink);
}

static void SQLSTATEMENT_copy(QoreObject *self, QoreObject *old, QoreSQLStatement *stmt, ExceptionSink *xsink) {
   xsink->raiseException("SQLSTATEMENT-COPY-ERROR", "SQLStatement objects cannot be copied");
   //self->setPrivate(CID_SQLSTATEMENT, new SQLStatement(*old));
}

// nothing SQLStatement::prepare(string $sql, ...)   
static AbstractQoreNode *SQLSTATEMENT_prepare(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   ReferenceHolder<QoreListNode> l(xsink);

   if (num_args(args) > 1)
      l = args->copyListFrom(1);

   stmt->prepare(*HARD_QORE_STRING(args, 0), *l, xsink);
   return 0;
}

// nothing SQLStatement::prepareRaw(string $sql)   
static AbstractQoreNode *SQLSTATEMENT_prepareRaw(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   stmt->prepareRaw(*HARD_QORE_STRING(args, 0), xsink);
   return 0;
}

// nothing SQLStatement::bind(...)   
static AbstractQoreNode *SQLSTATEMENT_bind(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   stmt->bind(*args, xsink);
   return 0;
}

// nothing SQLStatement::bindArgs(list)   
static AbstractQoreNode *SQLSTATEMENT_bindArgs(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   const QoreListNode *sql_args = HARD_QORE_LIST(args, 0);
   stmt->bind(*sql_args, xsink);
   return 0;
}

// nothing SQLStatement::bindPlaceholders(...)   
static AbstractQoreNode *SQLSTATEMENT_bindPlaceholders(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   stmt->bindPlaceholders(*args, xsink);
   return 0;
}

// nothing SQLStatement::bindPlaceholdersArgs(list)   
static AbstractQoreNode *SQLSTATEMENT_bindPlaceholdersArgs(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   const QoreListNode *sql_args = HARD_QORE_LIST(args, 0);
   stmt->bindPlaceholders(*sql_args, xsink);
   return 0;
}

// nothing SQLStatement::bindValues(...)   
static AbstractQoreNode *SQLSTATEMENT_bindValues(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   stmt->bindValues(*args, xsink);
   return 0;
}

// nothing SQLStatement::bindValuesArgs(list)   
static AbstractQoreNode *SQLSTATEMENT_bindValuesArgs(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   const QoreListNode *sql_args = HARD_QORE_LIST(args, 0);
   stmt->bindValues(*sql_args, xsink);
   return 0;
}

// nothing SQLStatement::exec(...)  
static AbstractQoreNode *SQLSTATEMENT_exec(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   stmt->exec(args, xsink);
   return 0;
}

// nothing SQLStatement::execArgs(list $args)  
static AbstractQoreNode *SQLSTATEMENT_execArgs(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   stmt->exec(HARD_QORE_LIST(args, 0), xsink);
   return 0;
}

// int SQLStatement::affectedRows()  
static AbstractQoreNode *SQLSTATEMENT_affectedRows(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   int rows = stmt->affectedRows(xsink);
   return *xsink ? 0 : new QoreBigIntNode(rows);
}

// hash SQLStatement::getOutput()  
static AbstractQoreNode *SQLSTATEMENT_getOutput(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   return stmt->getOutput(xsink);
}

// hash SQLStatement::getOutputRows()  
static AbstractQoreNode *SQLSTATEMENT_getOutputRows(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   return stmt->getOutputRows(xsink);
}

// nothing SQLStatement::define()  
static AbstractQoreNode *SQLSTATEMENT_define(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   stmt->define(xsink);
   return 0;
}

// nothing SQLStatement::close()  
static AbstractQoreNode *SQLSTATEMENT_close(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   stmt->close(xsink);
   return 0;
}

// nothing SQLStatement::commit()  
static AbstractQoreNode *SQLSTATEMENT_commit(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   stmt->commit(xsink);
   return 0;
}

// nothing SQLStatement::rollback()  
static AbstractQoreNode *SQLSTATEMENT_rollback(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   stmt->rollback(xsink);
   return 0;
}

// nothing SQLStatement::beginTransaction()  
static AbstractQoreNode *SQLSTATEMENT_beginTransaction(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   stmt->beginTransaction(xsink);
   return 0;
}

// bool SQLStatement::next()  
static AbstractQoreNode *SQLSTATEMENT_next(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   bool b = stmt->next(xsink);
   return *xsink ? 0 : get_bool_node(b);
}

// *hash SQLStatement::fetchRow()  
static AbstractQoreNode *SQLSTATEMENT_fetchRow(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   return stmt->fetchRow(xsink);
}

// list SQLStatement::fetchRows(softint $rows = -1)  
static AbstractQoreNode *SQLSTATEMENT_fetchRows(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   return stmt->fetchRows((int)HARD_QORE_INT(args, 0), xsink);
}

// hash SQLStatement::fetchColumns(softint $rows = -1)  
static AbstractQoreNode *SQLSTATEMENT_fetchColumns(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   return stmt->fetchColumns((int)HARD_QORE_INT(args, 0), xsink);
}

// *string SQLStatement::getSQL()  
static AbstractQoreNode *SQLSTATEMENT_getSQL(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   return stmt->getSQL(xsink);
}

// bool SQLStatement::active()  
static AbstractQoreNode *SQLSTATEMENT_active(QoreObject *self, QoreSQLStatement *stmt, const QoreListNode *args, ExceptionSink *xsink) {
   return get_bool_node(stmt->active());
}

QoreClass *initSQLStatementClass(QoreClass *QC_Datasource, QoreClass *QC_DatasourcePool) {
   QORE_TRACE("initSQLStatementClass()");

   QoreClass *QC_SQLSTATEMENT = new QoreClass("SQLStatement", QDOM_DATABASE);
   CID_SQLSTATEMENT = QC_SQLSTATEMENT->getID();

   QC_SQLSTATEMENT->setConstructorExtended(SQLSTATEMENT_constructor_ds, false, QC_NO_FLAGS, QDOM_DATABASE, 1, QC_Datasource->getTypeInfo(), QORE_PARAM_NO_ARG);
   QC_SQLSTATEMENT->setConstructorExtended(SQLSTATEMENT_constructor_dsp, false, QC_NO_FLAGS, QDOM_DATABASE, 1, QC_DatasourcePool->getTypeInfo(), QORE_PARAM_NO_ARG);

   QC_SQLSTATEMENT->setDestructor((q_destructor_t)SQLSTATEMENT_destructor);
   QC_SQLSTATEMENT->setCopy((q_copy_t)SQLSTATEMENT_copy);

   // nothing SQLStatement::prepare(string $sql, ...)   
   QC_SQLSTATEMENT->addMethodExtended("prepare",          (q_method_t)SQLSTATEMENT_prepare, false, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // nothing SQLStatement::prepareRaw(string $sql)   
   QC_SQLSTATEMENT->addMethodExtended("prepareRaw",       (q_method_t)SQLSTATEMENT_prepareRaw, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // nothing SQLStatement::bind(...)  
   QC_SQLSTATEMENT->addMethodExtended("bind",             (q_method_t)SQLSTATEMENT_bind, false, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, nothingTypeInfo);

   // nothing SQLStatement::bindArgs(list)   
   QC_SQLSTATEMENT->addMethodExtended("bindArgs",         (q_method_t)SQLSTATEMENT_bindArgs, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, listTypeInfo, QORE_PARAM_NO_ARG);

   // nothing SQLStatement::bindPlaceholders(...)  
   QC_SQLSTATEMENT->addMethodExtended("bindPlaceholders", (q_method_t)SQLSTATEMENT_bindPlaceholders, false, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, nothingTypeInfo);

   // nothing SQLStatement::bindPlaceholdersArgs(list)   
   QC_SQLSTATEMENT->addMethodExtended("bindPlaceholdersArgs", (q_method_t)SQLSTATEMENT_bindPlaceholdersArgs, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, listTypeInfo, QORE_PARAM_NO_ARG);

   // nothing SQLStatement::bindValues(...)  
   QC_SQLSTATEMENT->addMethodExtended("bindValues",       (q_method_t)SQLSTATEMENT_bindValues, false, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, nothingTypeInfo);

   // nothing SQLStatement::bindValuesArgs(list)   
   QC_SQLSTATEMENT->addMethodExtended("bindValuesArgs",   (q_method_t)SQLSTATEMENT_bindValuesArgs, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, listTypeInfo, QORE_PARAM_NO_ARG);

   // nothing SQLStatement::exec(...)  
   QC_SQLSTATEMENT->addMethodExtended("exec",             (q_method_t)SQLSTATEMENT_exec, false, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, nothingTypeInfo);

   // nothing SQLStatement::execArgs(list $args)  
   QC_SQLSTATEMENT->addMethodExtended("execArgs",         (q_method_t)SQLSTATEMENT_execArgs, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, listTypeInfo, QORE_PARAM_NO_ARG);

   // int SQLStatement::affectedRows()  
   QC_SQLSTATEMENT->addMethodExtended("affectedRows",     (q_method_t)SQLSTATEMENT_affectedRows, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);

   // hash SQLStatement::getOutput()  
   QC_SQLSTATEMENT->addMethodExtended("getOutput",        (q_method_t)SQLSTATEMENT_getOutput, false, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo);

   // hash SQLStatement::getOutputRows()  
   QC_SQLSTATEMENT->addMethodExtended("getOutputRows",    (q_method_t)SQLSTATEMENT_getOutputRows, false, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo);

   // nothing SQLStatement::define()  
   QC_SQLSTATEMENT->addMethodExtended("define",           (q_method_t)SQLSTATEMENT_define, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   // nothing SQLStatement::close()  
   QC_SQLSTATEMENT->addMethodExtended("close",            (q_method_t)SQLSTATEMENT_close, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   // nothing SQLStatement::commit()  
   QC_SQLSTATEMENT->addMethodExtended("commit",           (q_method_t)SQLSTATEMENT_commit, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   // nothing SQLStatement::rollback()  
   QC_SQLSTATEMENT->addMethodExtended("rollback",         (q_method_t)SQLSTATEMENT_rollback, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   // nothing SQLStatement::beginTransaction()  
   QC_SQLSTATEMENT->addMethodExtended("beginTransaction", (q_method_t)SQLSTATEMENT_beginTransaction, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   // bool SQLStatement::next()  
   QC_SQLSTATEMENT->addMethodExtended("next",             (q_method_t)SQLSTATEMENT_next, false, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo);

   // *hash SQLStatement::fetchRow()  
   QC_SQLSTATEMENT->addMethodExtended("fetchRow",         (q_method_t)SQLSTATEMENT_fetchRow, false, QC_NO_FLAGS, QDOM_DEFAULT, hashOrNothingTypeInfo);

   // list SQLStatement::fetchRows(softint $rows = -1)  
   QC_SQLSTATEMENT->addMethodExtended("fetchRows",        (q_method_t)SQLSTATEMENT_fetchRows, false, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo, 1, softBigIntTypeInfo, new QoreBigIntNode(-1));

   // hash SQLStatement::fetchColumns(softint $rows = -1)  
   QC_SQLSTATEMENT->addMethodExtended("fetchColumns",     (q_method_t)SQLSTATEMENT_fetchColumns, false, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo, 1, softBigIntTypeInfo, new QoreBigIntNode(-1));

   // *string SQLStatement::getSQL()  
   QC_SQLSTATEMENT->addMethodExtended("getSQL",           (q_method_t)SQLSTATEMENT_getSQL, false, QC_NO_FLAGS, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // bool SQLStatement::active()  
   QC_SQLSTATEMENT->addMethodExtended("active",           (q_method_t)SQLSTATEMENT_active, false, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo);

   return QC_SQLSTATEMENT;
}
