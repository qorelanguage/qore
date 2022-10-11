/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    QC_SocketPollOperation.h

    Qore Programming Language

    Copyright (C) 2003 - 2022 Qore Technologies, s.r.o.

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
    DLLLOCAL SocketConnectPollOperation(ExceptionSink* xsink, bool ssl, const char* target, QoreSocketObject* sock)
            : sock(sock) {
        sgoal = ssl ? SPG_CONNECT_SSL : SPG_CONNECT;

        AutoLocker al(sock->priv->m);

        if (preVerify(xsink)) {
            return;
        }
        if (!sock->priv->setNonBlock(xsink)) {
            set_non_block = true;
            poll_state.reset(sock->priv->socket->startConnect(xsink, target));
            if (!*xsink) {
                if (poll_state) {
                    state = SPS_CONNECTING;
                } else {
                    if (sgoal == SPG_CONNECT) {
                        sock->priv->clearNonBlock();
                        set_non_block = false;
                        connected();
                    } else {
                        assert(sgoal == SPG_CONNECT_SSL);
                        startSslConnect(xsink);
                    }
                }
            }
            if (*xsink) {
                sock->priv->clearNonBlock();
                set_non_block = false;
            }
        }
    }

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

    DLLLOCAL virtual QoreHashNode* continuePoll(ExceptionSink* xsink) {
        QoreHashNode* rv = nullptr;

        AutoLocker al(sock->priv->m);

        switch (state) {
            case SPS_CONNECTING: {
                int rc = checkContinuePoll(xsink);
                if (rc != 0) {
                    rv = *xsink ? nullptr : getSocketPollInfoHash(xsink, rc);
                    break;
                }

                // if we are just connecting, we are done
                if (sgoal == SPG_CONNECT) {
                    // SPS_CONNECTED set below
                    break;
                }

                assert(sgoal == SPG_CONNECT_SSL);

                if (startSslConnect(xsink)) {
                    break;
                }
            }
            // fall down to next case

            case SPS_CONNECTING_SSL: {
                int rc = checkContinuePoll(xsink);
                if (rc != 0) {
                    rv = *xsink ? nullptr : getSocketPollInfoHash(xsink, rc);
                    break;
                }

                // SPS_CONNECTED set below
                break;
            }

            case SPS_CONNECTED: {
                break;
            }

            default:
                assert(false);
        }

        if (!rv) {
            if (*xsink) {
                state = SPS_NONE;
            } else {
                connected();
            }
            sock->priv->clearNonBlock();
        } else {
            assert(!*xsink);
        }
        return rv;
    }

protected:
    QoreSocketObject* sock;

    //! Called in the constructor
    DLLLOCAL virtual int preVerify(ExceptionSink* xsink) {
        return 0;
    }

    //! Called when the connection is established
    DLLLOCAL virtual void connected() {
        // socket lock must be held here
        assert(sock->priv->m.trylock());
        state = SPS_CONNECTED;
    }

    //! Called to switch to the connect-ssl state
    DLLLOCAL int startSslConnect(ExceptionSink* xsink) {
        // socket lock must be held here
        assert(sock->priv->m.trylock());

        state = SPS_CONNECTING_SSL;

        poll_state.reset(sock->priv->socket->startSslConnect(xsink,
            sock->priv->cert ? sock->priv->cert->getData() : nullptr,
            sock->priv->pk ? sock->priv->pk->getData() : nullptr));
        if (*xsink) {
            poll_state.reset();
            state = SPS_NONE;
            return -1;
        }
        return 0;
    }

private:
    unique_ptr<AbstractPollState> poll_state;
    std::string target;

    int sgoal = 0;
    int state = SPS_NONE;

    bool set_non_block = false;

    DLLLOCAL virtual const char* getStateImpl() const {
        const char* str;
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

    DLLLOCAL int checkContinuePoll(ExceptionSink* xsink) {
        // socket lock must be held here
        assert(sock->priv->m.trylock());
        assert(poll_state.get());

        // see if we are able to continue
        int rc = poll_state->continuePoll(xsink);
        //printd(5, "SocketConnectPollOperation::continuePoll() state: %s rc: %d (exp: %d)\n", getStateImpl(), rc,
        //    (int)*xsink);
        if (*xsink) {
            assert(rc < 0);
            state = SPS_NONE;
            return -1;
        }
        if (!rc) {
            // release the AbstractPollState value
            poll_state.reset();
        }
        return rc;
    }
};

class SocketSendPollOperation : public SocketPollOperationBase {
public:
    // "data" must be passed already referenced
    DLLLOCAL SocketSendPollOperation(ExceptionSink* xsink, QoreStringNode* data, QoreSocketObject* sock)
            : data(data), sock(sock), buf(data->c_str()), size(data->size()) {
        AutoLocker al(sock->priv->m);

        assert(data->getEncoding() == sock->getEncoding());
        if (!sock->priv->setNonBlock(xsink)) {
            poll_state.reset(sock->priv->socket->startSend(xsink, buf, size));
            if (!poll_state) {
                sock->priv->clearNonBlock();
            } else {
                set_non_block = true;
            }
        }
    }

    // "data" must be passed already referenced
    DLLLOCAL SocketSendPollOperation(ExceptionSink* xsink, BinaryNode* data, QoreSocketObject* sock)
            : data(data), sock(sock), buf(reinterpret_cast<const char*>(data->getPtr())),
            size(data->size()) {
        AutoLocker al(sock->priv->m);

        if (!sock->priv->setNonBlock(xsink)) {
            poll_state.reset(sock->priv->socket->startSend(xsink, buf, size));
            if (!poll_state) {
                sock->priv->clearNonBlock();
            } else {
                set_non_block = true;
            }
        }
    }

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

    DLLLOCAL virtual QoreHashNode* continuePoll(ExceptionSink* xsink) {
        AutoLocker al(sock->priv->m);
        if (!poll_state) {
            return nullptr;
        }

        // see if we are able to continue
        int rc = poll_state->continuePoll(xsink);
        //printd(5, "SocketConnectPollOperation::continuePoll() state: %s rc: %d (exp: %d)\n", getStateImpl(), rc,
        //    (int)*xsink);
        if (*xsink || !rc) {
            // release the AbstractPollState value
            poll_state.reset();
            sock->priv->clearNonBlock();
            set_non_block = false;
            if (!*xsink) {
                sent = true;
            }
            return nullptr;
        }
        return getSocketPollInfoHash(xsink, rc);
    }

private:
    unique_ptr<AbstractPollState> poll_state;
    SimpleRefHolder<SimpleValueQoreNode> data;
    QoreSocketObject* sock;
    const char* buf;
    size_t size;
    bool set_non_block = false;
    bool sent = false;
};

class SocketRecvPollOperation : public SocketPollOperationBase {
public:
    // "data" must be passed already referenced
    DLLLOCAL SocketRecvPollOperation(ExceptionSink* xsink, ssize_t size, QoreSocketObject* sock, bool to_string)
            : sock(sock), size(size), to_string(to_string) {
        AutoLocker al(sock->priv->m);

        if (!sock->priv->setNonBlock(xsink)) {
            poll_state.reset(sock->priv->socket->startRecv(xsink, size));
            if (!poll_state) {
                sock->priv->clearNonBlock();
            } else {
                set_non_block = true;
            }
        }
    }

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
        return received;
    }

    DLLLOCAL virtual const char* getStateImpl() const {
        return received ? "received" : "receiving";
    }

    DLLLOCAL virtual QoreValue getOutput() const {
        return data ? data->refSelf() : QoreValue();
    }

    DLLLOCAL virtual QoreHashNode* continuePoll(ExceptionSink* xsink) {
        AutoLocker al(sock->priv->m);
        if (!poll_state) {
            return nullptr;
        }

        // see if we are able to continue
        int rc = poll_state->continuePoll(xsink);
        //printd(5, "SocketConnectPollOperation::continuePoll() state: %s rc: %d (exp: %d)\n", getStateImpl(), rc,
        //    (int)*xsink);
        if (!rc) {
            // get output data
            SimpleRefHolder<BinaryNode> d(poll_state->takeOutput().get<BinaryNode>());
            if (to_string) {
                size_t len = d->size();
                data = new QoreStringNode(reinterpret_cast<char*>(d->giveBuffer()), len, len + 1,
                    sock->getEncoding());
            } else {
                data = d.release();
            }
            received = true;
        }
        if (*xsink || !rc) {
            // release the AbstractPollState value
            poll_state.reset();
            sock->priv->clearNonBlock();
            set_non_block = false;
            return nullptr;
        }
        return getSocketPollInfoHash(xsink, rc);
    }

private:
    unique_ptr<AbstractPollState> poll_state;
    SimpleRefHolder<SimpleValueQoreNode> data;
    QoreSocketObject* sock;
    size_t size;
    bool to_string;
    bool set_non_block = false;
    bool received = false;
};

DLLLOCAL QoreClass* initSocketPollOperationClass(QoreNamespace& qorens);

#endif // _QORE_CLASS_SOCKETPOLLOPERATION_H