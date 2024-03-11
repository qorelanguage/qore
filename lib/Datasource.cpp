/*
    Datasource.cpp

    Qore Programming Language

    Copyright (C) 2003 - 2024 Qore Technologies, s.r.o.

    NOTE that 2 copies of connection values are kept in case
    the values are changed while a connection is in use

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

    Note that the Qore library is released under a choice of three open-source
    licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
    information.
*/

#include <qore/Qore.h>
#include "qore/intern/qore_dbi_private.h"
#include "qore/intern/qore_ds_private.h"
#include "qore/intern/QoreHashNodeIntern.h"
#include "qore/intern/QoreSQLStatement.h"
#include "qore/intern/QC_SQLStatement.h"

#include <cstdlib>
#include <cstring>

void qore_ds_private::statementExecuted(int rc) {
    // we always assume we are in a transaction after executing a transaction-relevant statement
    // unless the connection was aborted
    if (!in_transaction) {
        if (!connection_aborted) {
            assert(!active_transaction);
            assert(isopen);
            in_transaction = true;
            active_transaction = true;
            return;
        }
    }
    else if (!rc && !active_transaction)
        active_transaction = true;
}

QoreHashNode* qore_ds_private::getCurrentOptionHash(bool ensure_hash) const {
    QoreHashNode* options = nullptr;

    ReferenceHolder<QoreHashNode> opts(getOptionHash(), nullptr);
    ConstHashIterator hi(*opts);
    while (hi.next()) {
        QoreValue v = hi.get();
        // if we have private data, then we are dealing with runtime data
        if (private_data) {
            const QoreHashNode* ov = hi.get().get<const QoreHashNode>();
            v = ov->getKeyValue("value");
        }
        // otherwise for pending data, we already have the value in "v"
        if (v.isNothing() || (v.getType() == NT_BOOLEAN && !v.getAsBool())) {
            continue;
        }

        if (!options) {
            options = new QoreHashNode(autoTypeInfo);
        }

        qore_hash_private::get(*options)->setKeyValueIntern(hi.getKey(), v.refSelf());
    }

    if (ensure_hash && !options) {
        options = new QoreHashNode(autoTypeInfo);
    }

    return options;
}

QoreHashNode* qore_ds_private::getConfigHash() const {
    ReferenceHolder<QoreHashNode> h(new QoreHashNode(autoTypeInfo), nullptr);

    h->setKeyValue("type", new QoreStringNode(dsl->getName()), nullptr);
    if (private_data) {
        if (!username.empty()) {
            h->setKeyValue("user", new QoreStringNode(username), nullptr);
        }
        if (!password.empty()) {
            h->setKeyValue("pass", new QoreStringNode(password), nullptr);
        }
        if (!dbname.empty()) {
            h->setKeyValue("db", new QoreStringNode(dbname), nullptr);
        }
        if (!db_encoding.empty()) {
            h->setKeyValue("charset", new QoreStringNode(db_encoding), nullptr);
        }
        if (!hostname.empty()) {
            h->setKeyValue("host", new QoreStringNode(hostname), nullptr);
        }
        if (port) {
            h->setKeyValue("port", port, nullptr);
        }
    } else {
        if (!p_username.empty()) {
            h->setKeyValue("user", new QoreStringNode(p_username), nullptr);
        }
        if (!p_password.empty()) {
            h->setKeyValue("pass", new QoreStringNode(p_password), nullptr);
        }
        if (!p_dbname.empty()) {
            h->setKeyValue("db", new QoreStringNode(p_dbname), nullptr);
        }
        if (!p_db_encoding.empty()) {
            h->setKeyValue("charset", new QoreStringNode(p_db_encoding), nullptr);
        }
        if (!p_hostname.empty()) {
            h->setKeyValue("host", new QoreStringNode(p_hostname), nullptr);
        }
        if (p_port) {
            h->setKeyValue("port", p_port, nullptr);
        }
    }

    QoreHashNode* options = getCurrentOptionHash();
    if (options) {
        h->setKeyValue("options", options, nullptr);
    }

    return h.release();
}

QoreStringNode* qore_ds_private::getConfigString() const {
    SimpleRefHolder<QoreStringNode> str(new QoreStringNode(dsl->getName()));
    str->concat(':');

    if (private_data) {
        if (!username.empty()) {
            str->concat(username);
        }
        if (!password.empty()) {
            str->sprintf("/%s", password.c_str());
        }
        // the '@' symbol must be present even if the database name is empty
        str->concat('@');
        if (!dbname.empty()) {
            str->concat(dbname);
        }
        if (!db_encoding.empty()) {
            str->sprintf("(%s)", db_encoding.c_str());
        }
        if (!hostname.empty()) {
            str->sprintf("%%%s", hostname.c_str());
        }
        if (port) {
            str->sprintf(":%d", port);
        }
    } else {
        if (!p_username.empty()) {
            str->concat(p_username);
        }
        if (!p_password.empty()) {
            str->sprintf("/%s", p_password.c_str());
        }
        if (!p_dbname.empty()) {
            str->sprintf("@%s", p_dbname.c_str());
        }
        if (!p_db_encoding.empty()) {
            str->sprintf("(%s)", p_db_encoding.c_str());
        }
        if (!p_hostname.empty()) {
            str->sprintf("%%%s", p_hostname.c_str());
        }
        if (p_port) {
            str->sprintf(":%d", p_port);
        }
    }

    bool first = false;
    ReferenceHolder<QoreHashNode> opts(getOptionHash(), nullptr);
    ConstHashIterator hi(*opts);
    while (hi.next()) {
        QoreValue v = hi.get();
        // if we have private data, then we are dealing with runtime data
        if (private_data) {
            const QoreHashNode* ov = hi.get().get<const QoreHashNode>();
            v = ov->getKeyValue("value");
        }
        // otherwise for pending data, we already have the value in "v"
        if (v.isNothing() || (v.getType() == NT_BOOLEAN && !v.getAsBool()))
            continue;

        if (first) {
            str->concat(',');
        } else {
            str->concat('{');
            first = true;
        }
        str->concat(hi.getKey());
        if (v.getType() == NT_BOOLEAN && v.getAsBool()) {
            continue;
        }

        QoreStringValueHelper sv(v);
        str->sprintf("=%s", sv->getBuffer());
    }
    if (first) {
        str->concat('}');
    }

    return str.release();
}

Datasource::Datasource(DBIDriver* ndsl, DatasourceStatementHelper* dsh) : priv(new qore_ds_private(this, ndsl, dsh)) {
}

Datasource::Datasource(const Datasource& old, DatasourceStatementHelper* dsh) : priv(new qore_ds_private(*old.priv, this, dsh)) {
}

Datasource::Datasource(DBIDriver* ndsl) : priv(new qore_ds_private(this, ndsl, nullptr)) {
}

Datasource::Datasource(const Datasource& old) : priv(new qore_ds_private(*old.priv, this, nullptr)) {
}

Datasource::~Datasource() {
    if (priv->isopen)
        close();

    delete priv;
}

void Datasource::setPendingConnectionValues(const Datasource* other) {
    priv->setPendingConnectionValues(other->priv);
}

void Datasource::setTransactionStatus(bool t) {
    //printd(5, "Datasource::setTS(%d) this=%p\n", t, this);
    priv->in_transaction = t;
}

QoreListNode* Datasource::getCapabilityList() const {
    return qore_dbi_private::get(*priv->dsl)->getCapList();
}

int Datasource::getCapabilities() const {
    return qore_dbi_private::get(*priv->dsl)->getCaps();
}

bool Datasource::isInTransaction() const {
    return priv->in_transaction;
}

bool Datasource::activeTransaction() const {
    return priv->active_transaction;
}

bool Datasource::getAutoCommit() const {
    return priv->autocommit;
}

bool Datasource::isOpen() const {
    return priv->isopen;
}

Datasource* Datasource::copy() const {
    return new Datasource(*this);
}

void Datasource::setConnectionValues() {
    priv->setConnectionValues();
}

void Datasource::setAutoCommit(bool ac) {
    priv->autocommit = ac;
}

QoreValue Datasource::select(const QoreString* query_str, const QoreListNode* args, ExceptionSink* xsink) {
    assert(xsink);
    QoreValue rv = qore_dbi_private::get(*priv->dsl)->select(this, query_str, args, xsink);
    autoCommit(xsink);

    // set active_transaction flag if in a transaction and the active_transaction flag
    // has not yet been set and no exception was raised
    if (priv->in_transaction && !priv->active_transaction && !*xsink)
        priv->active_transaction = true;

    return rv;
}

QoreValue Datasource::selectRows(const QoreString* query_str, const QoreListNode* args, ExceptionSink* xsink) {
    assert(xsink);
    QoreValue rv = qore_dbi_private::get(*priv->dsl)->selectRows(this, query_str, args, xsink);
    autoCommit(xsink);

    // set active_transaction flag if in a transaction and the active_transaction flag
    // has not yet been set and no exception was raised
    if (priv->in_transaction && !priv->active_transaction && !*xsink)
        priv->active_transaction = true;

    return rv;
}

QoreHashNode* Datasource::selectRow(const QoreString* query_str, const QoreListNode* args, ExceptionSink* xsink) {
    assert(xsink);
    QoreHashNode* rv = qore_dbi_private::get(*priv->dsl)->selectRow(this, query_str, args, xsink);
    autoCommit(xsink);

    // set active_transaction flag if in a transaction and the active_transaction flag
    // has not yet been set and no exception was raised
    if (priv->in_transaction && !priv->active_transaction && !*xsink)
        priv->active_transaction = true;

    return rv;
}

QoreValue Datasource::exec_internal(bool doBind, const QoreString* query_str, const QoreListNode* args,
        ExceptionSink* xsink) {
    assert(xsink);
    if (!priv->autocommit && !priv->in_transaction && beginImplicitTransaction(xsink))
        return QoreValue();

    assert(priv->isopen && priv->private_data);

    QoreValue rv = doBind ? qore_dbi_private::get(*priv->dsl)->execSQL(this, query_str, args, xsink)
        : qore_dbi_private::get(*priv->dsl)->execRawSQL(this, query_str, xsink);;
    //printd(5, "Datasource::exec_internal() this=%p, autocommit=%d, in_transaction=%d, xsink=%d\n", this,
    //    priv->autocommit, priv->in_transaction, xsink->isException());

    if (priv->connection_aborted) {
        assert(*xsink);
        assert(!rv);
        return QoreValue();
    }

    if (priv->autocommit) {
        qore_dbi_private::get(*priv->dsl)->autoCommit(this, xsink);
    } else {
        priv->statementExecuted(*xsink);
    }

    return rv;
}

int Datasource::autoCommit(ExceptionSink* xsink) {
    if (priv->autocommit && !priv->connection_aborted)
        return qore_dbi_private::get(*priv->dsl)->autoCommit(this, xsink);
    return 0;
}

QoreValue Datasource::exec(const QoreString* query_str, const QoreListNode* args, ExceptionSink* xsink) {
    return exec_internal(true, query_str, args, xsink);
}

// deprecated: remove due to extraneous ignored "args" argument
QoreValue Datasource::execRaw(const QoreString* query_str, const QoreListNode* args, ExceptionSink* xsink) {
    assert(!args);
    return exec_internal(false, query_str, nullptr, xsink);
}

QoreValue Datasource::execRaw(const QoreString* query_str, ExceptionSink* xsink) {
    return exec_internal(false, query_str, nullptr, xsink);
}

QoreHashNode* Datasource::describe(const QoreString* query_str, const QoreListNode* args, ExceptionSink* xsink) {
    assert(xsink);
    QoreHashNode* rv = qore_dbi_private::get(*priv->dsl)->describe(this, query_str, args, xsink);
    autoCommit(xsink);

    // set active_transaction flag if in a transaction and the active_transaction flag
    // has not yet been set and no exception was raised
    if (priv->in_transaction && !priv->active_transaction && !*xsink)
        priv->active_transaction = true;

    return rv;
}

int Datasource::beginImplicitTransaction(ExceptionSink* xsink) {
    //printd(5, "Datasource::beginImplicitTransaction() autocommit=%s\n", autocommit ? "true" : "false");
    if (priv->autocommit) {
        xsink->raiseException("AUTOCOMMIT-ERROR", "%s:%s@%s: transaction management is not available because "
            "autocommit is enabled for this Datasource", getDriverName(), priv->username.c_str(), priv->dbname.c_str());
        return -1;
    }
    return qore_dbi_private::get(*priv->dsl)->beginTransaction(this, xsink);
}

int Datasource::beginTransaction(ExceptionSink* xsink) {
    int rc = beginImplicitTransaction(xsink);
    if (!rc && !priv->in_transaction) {
        priv->in_transaction = true;
        assert(!priv->active_transaction);
    }
    return rc;
}

int Datasource::commit(ExceptionSink* xsink) {
    if (!priv->in_transaction && beginImplicitTransaction(xsink))
        return -1;

    return priv->commit(xsink);
}

int Datasource::rollback(ExceptionSink* xsink) {
    if (!priv->in_transaction && beginImplicitTransaction(xsink))
        return -1;

    return priv->rollback(xsink);
}

int Datasource::open(ExceptionSink* xsink) {
    assert(xsink);
    int rc;

    if (!priv->isopen) {
        // copy pending connection values to connection values
        setConnectionValues();

        priv->connection_aborted = false;

        rc = qore_dbi_private::get(*priv->dsl)->init(this, xsink);
        if (!*xsink) {
            assert(priv->qorecharset);
            priv->isopen = true;
        }
    } else {
        rc = 0;
    }

    return rc;
}

int Datasource::close() {
    return priv->close();
}

void Datasource::connectionAborted() {
    ExceptionSink xsink;
    priv->connectionAborted(&xsink);
    xsink.clear();
}

void Datasource::connectionAborted(ExceptionSink* xsink) {
    priv->connectionAborted(xsink);
}

void Datasource::connectionLost(ExceptionSink* xsink) {
    priv->connectionLost(xsink);
}

void Datasource::connectionRecovered(ExceptionSink* xsink) {
    priv->connectionRecovered(xsink);
}

bool Datasource::wasConnectionAborted() const {
    return priv->connection_aborted;
}

// forces a close and open to reset a database connection
void Datasource::reset(ExceptionSink* xsink) {
    if (priv->isopen) {
        // close the Datasource
        qore_dbi_private::get(*priv->dsl)->close(this);
        priv->isopen = false;

        // open the connection
        open(xsink);

        //printd(5, "Datasource::reset() this: %p priv: %p in_transaction: %d active_transaction: %d\n", this, priv, priv->in_transaction, priv->active_transaction);

        // close any open transaction(s)
        priv->in_transaction = false;
        priv->active_transaction = false;
    }
}

void* Datasource::getPrivateData() const {
    return priv->private_data;
}

void Datasource::setPrivateData(void* data) {
    priv->private_data = data;
}

void Datasource::setPendingUsername(const char* u) {
    priv->p_username = u;
}

void Datasource::setPendingPassword(const char* p) {
    priv->p_password = p;
}

void Datasource::setPendingDBName(const char* d) {
    priv->p_dbname = d;
}

void Datasource::setPendingDBEncoding(const char* c) {
    priv->p_db_encoding = c;
}

void Datasource::setPendingHostName(const char* h) {
    priv->p_hostname = h;
}

void Datasource::setPendingPort(int port) {
    priv->p_port = port;
}

const std::string &Datasource::getUsernameStr() const {
    return priv->username;
}

const std::string &Datasource::getPasswordStr() const {
    return priv->password;
}

const std::string &Datasource::getDBNameStr() const {
    return priv->dbname;
}

const std::string &Datasource::getDBEncodingStr() const {
    return priv->db_encoding;
}

const std::string &Datasource::getHostNameStr() const {
    return priv->hostname;
}

const char* Datasource::getUsername() const {
    return priv->username.empty() ? nullptr : priv->username.c_str();
}

const char* Datasource::getPassword() const {
    return priv->password.empty() ? nullptr : priv->password.c_str();
}

const char* Datasource::getDBName() const {
    return priv->dbname.empty() ? nullptr : priv->dbname.c_str();
}

const char* Datasource::getDBEncoding() const {
    return priv->db_encoding.empty() ? nullptr : priv->db_encoding.c_str();
}

const char* Datasource::getOSEncoding() const {
    return priv->qorecharset ? priv->qorecharset->getCode() : nullptr;
}

const char* Datasource::getHostName() const {
    return priv->hostname.empty() ? nullptr : priv->hostname.c_str();
}

int Datasource::getPort() const {
    return priv->port;
}

const QoreEncoding* Datasource::getQoreEncoding() const {
    return priv->qorecharset;
}

void Datasource::setDBEncoding(const char* name) {
    priv->db_encoding = name;
}

void Datasource::setQoreEncoding(const char* name) {
    priv->qorecharset = QEM.findCreate(name);
}

void Datasource::setQoreEncoding(const QoreEncoding* enc) {
    priv->qorecharset = enc;
}

QoreStringNode* Datasource::getPendingUsername() const {
    return priv->p_username.empty() ? nullptr : new QoreStringNode(priv->p_username.c_str());
}

QoreStringNode* Datasource::getPendingPassword() const {
    return priv->p_password.empty() ? nullptr : new QoreStringNode(priv->p_password.c_str());
}

QoreStringNode* Datasource::getPendingDBName() const {
    return priv->p_dbname.empty() ? nullptr : new QoreStringNode(priv->p_dbname.c_str());
}

QoreStringNode* Datasource::getPendingDBEncoding() const {
    return priv->p_db_encoding.empty() ? nullptr : new QoreStringNode(priv->p_db_encoding.c_str());
}

QoreStringNode* Datasource::getPendingHostName() const {
    return priv->p_hostname.empty() ? nullptr : new QoreStringNode(priv->p_hostname.c_str());
}

int Datasource::getPendingPort() const {
    return priv->p_port;
}

const char* Datasource::getDriverName() const {
    return priv->dsl->getName();
}

const DBIDriver* Datasource::getDriver() const {
    return priv->dsl;
}

QoreValue Datasource::getServerVersion(ExceptionSink* xsink) {
    return qore_dbi_private::get(*priv->dsl)->getServerVersion(this, xsink);
}

QoreValue Datasource::getClientVersion(ExceptionSink* xsink) const {
    return qore_dbi_private::get(*priv->dsl)->getClientVersion(this, xsink);
}

QoreStringNode* Datasource::getDriverRealName(ExceptionSink* xsink) {
    return qore_dbi_private::get(*priv->dsl)->getDriverRealName(this, xsink);
}

QoreHashNode* Datasource::getOptionHash() const {
    return priv->getOptionHash();
}

int Datasource::setOption(const char* opt, const QoreValue val, ExceptionSink* xsink) {
    // maintain a copy of the option internally
    priv->setOption(opt, val, xsink);
    // only set options in private data if private data is already set
    if (priv->private_data) {
        return qore_dbi_private::get(*priv->dsl)->opt_set(this, opt, val, xsink);
    }

    // issue #3243: validate options before sending them to the driver
    OptInputHelper opt_helper(xsink, *qore_dbi_private::get(*priv->dsl), opt, true, val);
    return *xsink ? -1 : 0;
}

const QoreHashNode* Datasource::getConnectOptions() const {
    return priv->opt;
}

QoreValue Datasource::getOption(const char* opt, ExceptionSink* xsink) {
    if (!isOpen()) {
        xsink->raiseException("DATASOURCE-ERROR", "cannot retrieve the value for option '%s' when the datasource is " \
            "closed; use getOptionHash() to retrieve raw configuration option when the datasource is closed", opt);
        return QoreValue();
    }
    return qore_dbi_private::get(*priv->dsl)->opt_get(this, opt, xsink);
}

QoreHashNode* Datasource::getConfigHash() const {
    return priv->getConfigHash();
}

QoreHashNode* Datasource::getCurrentOptionHash() const {
    return priv->getCurrentOptionHash();
}

QoreStringNode* Datasource::getConfigString() const {
    return priv->getConfigString();
}

void Datasource::setEventQueue(Queue* q, QoreValue arg, ExceptionSink* xsink) {
    priv->setEventQueue(q, arg, xsink);
}

QoreHashNode* Datasource::getEventQueueHash(Queue*& q, int event_code) const {
    return priv->getEventQueueHash(q, event_code);
}

QoreObject* Datasource::getSQLStatementObjectForResultSet(void* stmt_private_data) {
    return new QoreObject(QC_SQLSTATEMENT, getProgram(), new QoreSQLStatement(this, stmt_private_data, priv->dsh, STMT_EXECED));
}
