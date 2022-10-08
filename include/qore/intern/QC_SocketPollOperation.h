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
    DLLLOCAL SocketConnectPollOperation(ExceptionSink* xsink, bool ssl, const char* target,
            QoreSocketObject* sock) : sock(sock) {
        sgoal = ssl ? SPG_CONNECT_SSL : SPG_CONNECT;
        if (!sock->setNonBlock(xsink)) {
            set_non_block = true;
            poll_state.reset(sock->startConnect(xsink, target));
            if (!*xsink) {
                if (poll_state) {
                    state = SPS_CONNECTING;
                } else {
                    sock->clearNonBlock();
                    state = SPS_CONNECTED;
                }
            } else {
                sock->clearNonBlock();
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
                state = SPS_CONNECTING_SSL;

                poll_state.reset(sock->startSslConnect(xsink, sock->priv->cert ? sock->priv->cert->getData() : nullptr,
                    sock->priv->pk ? sock->priv->pk->getData() : nullptr));
                if (*xsink) {
                    poll_state.reset();
                    state = SPS_NONE;
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

            if (SPS_CONNECTED) {
                break;
            }

            default:
                assert(false);
        }

        if (!rv) {
            if (*xsink) {
                state = SPS_NONE;
            } else {
                state = SPS_CONNECTED;
            }
            sock->clearNonBlock();
        } else {
            assert(!*xsink);
        }
        return rv;
    }

private:
    unique_ptr<AbstractPollState> poll_state;
    QoreSocketObject* sock;
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

DLLLOCAL QoreClass* initSocketPollOperationClass(QoreNamespace& qorens);

#endif // _QORE_CLASS_SOCKETPOLLOPERATION_H
