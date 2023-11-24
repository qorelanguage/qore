/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    QC_SocketPollOperation.h

    Qore Programming Language

    Copyright (C) 2003 - 2023 Qore Technologies, s.r.o.

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

#ifndef _QORE_CLASS_SOCKETPOLLOPERATION_H

#define _QORE_CLASS_SOCKETPOLLOPERATION_H

#include "qore/intern/QC_SocketPollOperationBase.h"
#include "qore/intern/qore_socket_private.h"
#include "qore/QoreSocketObject.h"

#include <memory>

// goals: connect, connect-ssl
constexpr int SPG_CONNECT = 1;
constexpr int SPG_CONNECT_SSL = 2;

// states: none -> connecting -> [connecting-ssl ->] connected
constexpr int SPS_NONE = 0;
constexpr int SPS_CONNECTING = 1;
constexpr int SPS_CONNECTING_SSL = 2;
constexpr int SPS_CONNECTED = 3;

class SocketConnectPollOperation : public SocketPollOperationBase {
public:
    DLLLOCAL SocketConnectPollOperation(ExceptionSink* xsink, bool ssl, const char* target, QoreSocketObject* sock);

    DLLLOCAL void deref(ExceptionSink* xsink) {
        if (ROdereference()) {
            if (set_non_block) {
                sock->clearNonBlock();
            }
            sock->deref(xsink);
            delete this;
        }
    }

    DLLLOCAL virtual bool goalReached() const {
        return state == SPS_CONNECTED;
    }

    DLLLOCAL virtual QoreHashNode* continuePoll(ExceptionSink* xsink);

protected:
    QoreSocketObject* sock;

    //! Called in the constructor
    DLLLOCAL virtual int preVerify(ExceptionSink* xsink) {
        return 0;
    }

    //! Called when the connection is established
    DLLLOCAL virtual void connected();

    //! Called to switch to the connect-ssl state
    DLLLOCAL int startSslConnect(ExceptionSink* xsink);

private:
    std::unique_ptr<AbstractPollState> poll_state;
    std::string target;

    int sgoal = 0;
    int state = SPS_NONE;

    bool set_non_block = false;

    DLLLOCAL virtual const char* getStateImpl() const {
        switch (state) {
            case SPS_NONE:
                return "none";
            case SPS_CONNECTING:
                return "connecting";
            case SPS_CONNECTING_SSL:
                return "connecting-ssl";
            case SPS_CONNECTED:
                return "connected";
            default:
                assert(false);
        }
        return "";
    }

    DLLLOCAL int checkContinuePoll(ExceptionSink* xsink);
};

class SocketSendPollOperation : public SocketPollOperationBase {
public:
    // "data" must be passed already referenced
    DLLLOCAL SocketSendPollOperation(ExceptionSink* xsink, QoreStringNode* data, QoreSocketObject* sock);

    // "data" must be passed already referenced
    DLLLOCAL SocketSendPollOperation(ExceptionSink* xsink, BinaryNode* data, QoreSocketObject* sock);

    DLLLOCAL void deref(ExceptionSink* xsink) {
        if (ROdereference()) {
            if (set_non_block) {
                sock->clearNonBlock();
            }
            sock->deref(xsink);
            delete this;
        }
    }

    DLLLOCAL virtual bool goalReached() const {
        return sent;
    }

    DLLLOCAL virtual const char* getStateImpl() const {
        return sent ? "sent" : "sending";
    }

    DLLLOCAL virtual QoreHashNode* continuePoll(ExceptionSink* xsink);

private:
    std::unique_ptr<AbstractPollState> poll_state;
    SimpleRefHolder<SimpleValueQoreNode> data;
    QoreSocketObject* sock;
    const char* buf;
    size_t size;
    bool set_non_block = false;
    bool sent = false;
};

class SocketRecvPollOperationBase : public SocketPollOperationBase {
public:
    DLLLOCAL SocketRecvPollOperationBase(QoreSocketObject* sock, bool to_string) : sock(sock),
            to_string(to_string) {
    }

    DLLLOCAL virtual void deref(ExceptionSink* xsink) {
        if (ROdereference()) {
            if (set_non_block) {
                sock->clearNonBlock();
            }
            sock->deref(xsink);
            delete this;
        }
    }

    DLLLOCAL virtual bool goalReached() const {
        return received;
    }

    DLLLOCAL virtual const char* getStateImpl() const {
        return received ? "received" : "receiving";
    }

    DLLLOCAL virtual QoreValue getOutput() const {
        return data ? data->refSelf() : QoreValue();
    }

    DLLLOCAL virtual QoreHashNode* continuePoll(ExceptionSink* xsink);

protected:
    std::unique_ptr<AbstractPollState> poll_state;
    SimpleRefHolder<SimpleValueQoreNode> data;
    QoreSocketObject* sock;
    bool to_string;
    bool set_non_block = false;
    bool received = false;

    DLLLOCAL int initIntern(ExceptionSink* xsink);
};

class SocketRecvPollOperation : public SocketRecvPollOperationBase {
public:
    // "data" must be passed already referenced
    DLLLOCAL SocketRecvPollOperation(ExceptionSink* xsink, ssize_t size, QoreSocketObject* sock, bool to_string);

private:
    size_t size;
};

class SocketRecvDataPollOperation : public SocketRecvPollOperationBase {
public:
    // "data" must be passed already referenced
    DLLLOCAL SocketRecvDataPollOperation(ExceptionSink* xsink, QoreSocketObject* sock, bool to_string);
};

class SocketRecvUntilBytesPollOperation : public SocketRecvPollOperationBase {
public:
    // "data" must be passed already referenced
    DLLLOCAL SocketRecvUntilBytesPollOperation(ExceptionSink* xsink, const QoreStringNode* pattern,
            QoreSocketObject* sock, bool to_string);

private:
    SimpleRefHolder<QoreStringNode> pattern;
};

class SocketUpgradeClientSslPollOperation : public SocketPollOperationBase {
public:
    DLLLOCAL SocketUpgradeClientSslPollOperation(ExceptionSink* xsink, QoreSocketObject* sock);

    DLLLOCAL void deref(ExceptionSink* xsink) {
        if (ROdereference()) {
            if (set_non_block) {
                sock->clearNonBlock();
            }
            sock->deref(xsink);
            delete this;
        }
    }

    DLLLOCAL virtual bool goalReached() const {
        return done;
    }

    DLLLOCAL virtual QoreHashNode* continuePoll(ExceptionSink* xsink);

    DLLLOCAL virtual const char* getStateImpl() const {
        return "connecting-ssl";
    }

private:
    QoreSocketObject* sock;
    std::unique_ptr<AbstractPollState> poll_state;
    bool set_non_block = false;
    bool done = false;
};

DLLLOCAL QoreClass* initSocketPollOperationClass(QoreNamespace& qorens);

#endif // _QORE_CLASS_SOCKETPOLLOPERATION_H
