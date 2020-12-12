/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    qore_socket_private.h

    Qore Programming Language

    Copyright (C) 2003 - 2020 Qore Technologies, s.r.o.

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

#ifndef _QORE_QORE_SOCKET_PRIVATE_H
#define _QORE_QORE_SOCKET_PRIVATE_H

#include "qore/intern/SSLSocketHelper.h"

#include "qore/intern/QC_Queue.h"

#include <cctype>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <strings.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#if defined HAVE_POLL
#include <poll.h>
#elif defined HAVE_SYS_SELECT_H
#include <sys/select.h>
#elif (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
#define HAVE_SELECT 1
#else
#error no async socket I/O APIs available
#endif

#ifndef DEFAULT_SOCKET_BUFSIZE
#define DEFAULT_SOCKET_BUFSIZE 4096
#endif

#ifndef QORE_MAX_HEADER_SIZE
#define QORE_MAX_HEADER_SIZE 16384
#endif

#define CHF_HTTP11  (1 << 0)
#define CHF_PROCESS (1 << 1)
#define CHF_REQUEST (1 << 2)

#ifndef DEFAULT_SOCKET_MIN_THRESHOLD_BYTES
#define DEFAULT_SOCKET_MIN_THRESHOLD_BYTES 1024
#endif

static constexpr int SOCK_POLLIN  = (1 << 0);
static constexpr int SOCK_POLLOUT = (1 << 1);
static constexpr int SOCK_POLLERR = (1 << 2);

DLLLOCAL void concat_target(QoreString& str, const struct sockaddr *addr, const char* type = "target");
DLLLOCAL int do_read_error(qore_offset_t rc, const char* method_name, int timeout_ms, ExceptionSink* xsink);
DLLLOCAL int sock_get_raw_error();
DLLLOCAL int sock_get_error();
DLLLOCAL void qore_socket_error(ExceptionSink* xsink, const char* err, const char* cdesc, const char* mname = 0, const char* host = 0, const char* svc = 0, const struct sockaddr *addr = 0);
DLLLOCAL void qore_socket_error_intern(int rc, ExceptionSink* xsink, const char* err, const char* cdesc, const char* mname = 0, const char* host = 0, const char* svc = 0, const struct sockaddr *addr = 0);
DLLLOCAL void se_in_op(const char* cname, const char* meth, ExceptionSink* xsink);
DLLLOCAL void se_in_op_thread(const char* cname, const char* meth, ExceptionSink* xsink);
DLLLOCAL void se_not_open(const char* cname, const char* meth, ExceptionSink* xsink, const char* extra = nullptr);
DLLLOCAL void se_timeout(const char* cname, const char* meth, int timeout_ms, ExceptionSink* xsink);
DLLLOCAL void se_closed(const char* cname, const char* mname, ExceptionSink* xsink);

#ifdef _Q_WINDOWS
#define GETSOCKOPT_ARG_4 char*
#define SETSOCKOPT_ARG_4 const char*
#define SHUTDOWN_ARG SD_BOTH
#define QORE_INVALID_SOCKET ((int)INVALID_SOCKET)
#define QORE_SOCKET_ERROR SOCKET_ERROR
DLLLOCAL int check_windows_rc(int rc);

#ifndef ECONNRESET
#define ECONNRESET WSAECONNRESET
#endif

#else
// UNIX/Cygwin
#define GETSOCKOPT_ARG_4 void*
#define SETSOCKOPT_ARG_4 void*
#define SHUTDOWN_ARG SHUT_RDWR
#define QORE_INVALID_SOCKET -1
#define QORE_SOCKET_ERROR -1
#endif

template <typename T>
class PrivateDataListHolder {
public:
    DLLLOCAL PrivateDataListHolder(ExceptionSink* xsink) : xsink(xsink) {
    }

    DLLLOCAL ~PrivateDataListHolder() {
        for (auto& i : pd_vec)
            i->deref(xsink);
    }

    DLLLOCAL T* add(const QoreObject* o, qore_classid_t cid) {
        T* pd = static_cast<T*>(o->getReferencedPrivateData(cid, xsink));
        if (!pd)
            return nullptr;
        pd_vec.push_back(pd);
        return pd;
    }

private:
    typedef std::vector<T*> pd_vec_t;
    pd_vec_t pd_vec;
    ExceptionSink* xsink;
};

struct qore_socketsource_private {
    QoreStringNode* address;
    QoreStringNode* hostname;

    DLLLOCAL qore_socketsource_private() : address(0), hostname(0) {
    }

    DLLLOCAL ~qore_socketsource_private() {
        if (address)  address->deref();
        if (hostname) hostname->deref();
    }

    DLLLOCAL void setAddress(QoreStringNode* addr) {
        assert(!address);
        address = addr;
    }

    DLLLOCAL void setAddress(const char* addr) {
        assert(!address);
        address = new QoreStringNode(addr);
    }

    DLLLOCAL void setHostName(const char* host) {
        assert(!hostname);
        hostname = new QoreStringNode(host);
    }

    DLLLOCAL void setAll(QoreObject* o, ExceptionSink* xsink) {
        if (address) {
            o->setValue("source", address, xsink);
            address = 0;
        }

        if (hostname) {
            o->setValue("source_host", hostname, xsink);
            hostname = 0;
        }
    }
};

class OptionalNonBlockingHelper {
public:
    qore_socket_private& sock;
    ExceptionSink* xsink;
    bool set;

    DLLLOCAL OptionalNonBlockingHelper(qore_socket_private& s, bool n_set, ExceptionSink* xs);
    DLLLOCAL ~OptionalNonBlockingHelper();
};

class PrivateQoreSocketTimeoutBase {
protected:
    struct qore_socket_private* sock;
    int64 start;

public:
    DLLLOCAL PrivateQoreSocketTimeoutBase(qore_socket_private* s) : sock(s), start(sock ? q_clock_getmicros() : 0) {
    }
};

class PrivateQoreSocketTimeoutHelper : public PrivateQoreSocketTimeoutBase {
protected:
    const char* op;
public:
    DLLLOCAL PrivateQoreSocketTimeoutHelper(qore_socket_private* s, const char* op);
    DLLLOCAL ~PrivateQoreSocketTimeoutHelper();
};

class PrivateQoreSocketThroughputHelper : public PrivateQoreSocketTimeoutBase {
protected:
    bool send;
public:
    DLLLOCAL PrivateQoreSocketThroughputHelper(qore_socket_private* s, bool snd);
    DLLLOCAL ~PrivateQoreSocketThroughputHelper();

    DLLLOCAL void finalize(int64 bytes);
};

struct qore_socket_private;

struct qore_socket_op_helper {
protected:
    qore_socket_private* s;

public:
    DLLLOCAL qore_socket_op_helper(qore_socket_private* sock);
    DLLLOCAL ~qore_socket_op_helper();
};

class SSLSocketHelperHelper {
protected:
    qore_socket_private* s;
    SSLSocketHelper* ssl;
    bool context_saved = false;

public:
    DLLLOCAL SSLSocketHelperHelper(qore_socket_private* sock, bool set_thread_context = false);

    DLLLOCAL ~SSLSocketHelperHelper();

    DLLLOCAL void error();
};

struct qore_socket_private {
    friend class PrivateQoreSocketTimeoutHelper;
    friend class PrivateQoreSocketThroughputHelper;

    // for client certificate capture
    static thread_local qore_socket_private* current_socket;

    int sock, sfamily, port, stype, sprot;

    // issue #3558: connection sequence to show when a connection has been reestablished
    int64 connection_id = 0;

    const QoreEncoding* enc;

    std::string socketname;
    // issue #3053: client target for SNI
    std::string client_target;
    SSLSocketHelper* ssl = nullptr;
    Queue* event_queue = nullptr,   //!< event queue
        * warn_queue = nullptr;     //!< warning queue

    // issue #3633: HTTP encoding to assume
    std::string assume_http_encoding = "ISO-8859-1";

    // socket buffer for buffered reads
    char rbuf[DEFAULT_SOCKET_BUFSIZE];

    // current buffer size
    size_t buflen = 0,
        bufoffset = 0;

    int64 tl_warning_us = 0;     // timeout threshold for network action warning in microseconds
    double tp_warning_bs = 0;    // throughput warning threshold in B/s
    int64 tp_bytes_sent = 0,     // throughput: bytes sent
        tp_bytes_recv = 0,       // throughput: bytes received
        tp_us_sent = 0,          // throughput: time sending
        tp_us_recv = 0,          // throughput: time receiving
        tp_us_min = 0            // throughput: minimum time for transfer to be considered
        ;

    //! callback argument for the warning queue
    QoreValue warn_callback_arg;
    //! argument for the event queue
    QoreValue event_arg;
    bool del = false,
        http_exp_chunked_body = false,
        ssl_accept_all_certs = false,
        ssl_capture_remote_cert = false,
        event_data = false;
    int in_op = -1,
        ssl_verify_mode = SSL_VERIFY_NONE;

    // issue #3512: the remote certificate captured
    QoreObject* remote_cert = nullptr;

    // issue #3818: verbose certificate verification error info
    QoreStringNode* ssl_err_str = nullptr;

    DLLLOCAL qore_socket_private(int n_sock = QORE_INVALID_SOCKET, int n_sfamily = AF_UNSPEC, int n_stype = SOCK_STREAM, int n_prot = 0, const QoreEncoding* n_enc = QCS_DEFAULT) :
        sock(n_sock), sfamily(n_sfamily), port(-1), stype(n_stype), sprot(n_prot), enc(n_enc) {
    }

    DLLLOCAL ~qore_socket_private() {
        close_internal();

        // must be dereferenced and removed before deleting
        assert(!event_queue);
        assert(!warn_queue);
    }

    DLLLOCAL bool isOpen() {
        return sock != QORE_INVALID_SOCKET;
    }

    DLLLOCAL int close() {
        int rc = close_internal();
        if (in_op >= 0)
            in_op = -1;
        if (http_exp_chunked_body)
            http_exp_chunked_body = false;
        sfamily = AF_UNSPEC;
        stype = SOCK_STREAM;
        sprot = 0;

        return rc;
    }

    DLLLOCAL int close_and_reset() {
        assert(sock != QORE_INVALID_SOCKET);
        int rc;
        while (true) {
#ifdef _Q_WINDOWS
            rc = ::closesocket(sock);
#else
            rc = ::close(sock);
#endif
            // try again if close was interrupted by a signal
            if (!rc || sock_get_error() != EINTR)
                break;
        }
        //printd(5, "qore_socket_private::close_and_reset(this: %p) close(%d) returned %d\n", this, sock, rc);
        sock = QORE_INVALID_SOCKET;
        if (buflen)
            buflen = 0;
        if (bufoffset)
            bufoffset = 0;
        if (del)
            del = false;
        if (port != -1)
            port = -1;
        // issue #3053: clear hostname for SNI
        client_target.clear();
        return rc;
    }

    DLLLOCAL int close_internal() {
        //printd(5, "qore_socket_private::close_internal(this: %p) sock: %d\n", this, sock);
        if (ssl_err_str) {
            ssl_err_str->deref();
            ssl_err_str = nullptr;
        }
        if (remote_cert) {
            remote_cert->deref(nullptr);
            remote_cert = nullptr;
        }
        if (sock >= 0) {
            // if an SSL connection has been established, shut it down first
            if (ssl) {
                ssl->shutdown();
                ssl->deref();
                ssl = nullptr;
            }

            if (!socketname.empty()) {
                if (del)
                    unlink(socketname.c_str());
                socketname.clear();
            }
            do_close_event();
            // issue #3558: increment the connection sequence here. so the connection sequence is different as soon as
            // it's closed
            ++connection_id;

            return close_and_reset();
        } else {
            return 0;
        }
    }

    DLLLOCAL void setAssumedEncoding(const char* str) {
        assume_http_encoding = str;
    }

    DLLLOCAL const char* getAssumedEncoding() const {
        return assume_http_encoding.c_str();
    }

    DLLLOCAL int getSendTimeout() const {
        struct timeval tv;

#if defined(HPUX) && defined(__ia64) && defined(__LP64__)
        // on HPUX 64-bit the OS defines socklen_t to be 8 bytes
        // but the library expects a 32-bit value
        int size = sizeof(struct timeval);
#else
        socklen_t size = sizeof(struct timeval);
#endif

        if (getsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (GETSOCKOPT_ARG_4)&tv, (socklen_t *)&size))
            return -1;

        return tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }

    DLLLOCAL int getRecvTimeout() const {
        struct timeval tv;

#if defined(HPUX) && defined(__ia64) && defined(__LP64__)
        // on HPUX 64-bit the OS defines socklen_t to be 8 bytes
        // but the library expects a 32-bit value
        int size = sizeof(struct timeval);
#else
        socklen_t size = sizeof(struct timeval);
#endif

        if (getsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (GETSOCKOPT_ARG_4)&tv, (socklen_t *)&size))
            return -1;

        return tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }

    DLLLOCAL int getPort() {
        // if we don't need to find out what port we are, then return current value
        if (sock == QORE_INVALID_SOCKET || (sfamily != AF_INET && sfamily != AF_INET6) || port > 0)
            return port;

        // otherwise find out what port we're connected to
        struct sockaddr_storage addr;
#if defined(HPUX) && defined(__ia64) && defined(__LP64__)
        // on HPUX 64-bit the OS defines socklen_t to be 8 bytes, but the library expects a 32-bit value
        int size = sizeof addr;
#else
        socklen_t size = sizeof addr;
#endif

        if (getsockname(sock, (struct sockaddr *)&addr, (socklen_t *)&size) < 0)
            return -1;

        port = q_get_port_from_addr((const struct sockaddr *)&addr);
        return port;
    }

    DLLLOCAL static void do_header(const char* key, QoreString& hdr, const QoreValue& v) {
        switch (v.getType()) {
            case NT_STRING:
                hdr.sprintf("%s: %s\r\n", key, v.get<const QoreStringNode>()->c_str());
                break;
            case NT_INT:
                hdr.sprintf("%s: " QLLD "\r\n", key, v.getAsBigInt());
                break;
            case NT_FLOAT: {
                hdr.sprintf("%s: ", key);
                size_t offset = hdr.size();
                hdr.sprintf("%f\r\n", v.getAsFloat());
                // issue 1556: external modules that call setlocale() can change
                // the decimal point character used here from '.' to ','
                // only search the double added, QoreString::sprintf() concatenates
                q_fix_decimal(&hdr, offset);
                break;
            }
            case NT_NUMBER:
                hdr.sprintf("%s: ", key);
                v.get<const QoreNumberNode>()->toString(hdr);
                hdr.concat("\r\n");
                break;
            case NT_BOOLEAN:
                hdr.sprintf("%s: %d\r\n", key, (int)v.getAsBool());
                break;
        }
    }

    // issue #3879: must add Content-Length if not present, even if there is no message body
    /** see https://www.w3.org/Protocols/rfc2616/rfc2616-sec9.html
    */
    DLLLOCAL static void do_headers(QoreString& hdr, const QoreHashNode* headers, qore_size_t size, bool addsize = true) {
        // RFC-2616 4.4 (http://tools.ietf.org/html/rfc2616#section-4.4)
        // add Content-Length: 0 to headers for responses without a body where there is no transfer-encoding
        if (headers) {
            ConstHashIterator hi(headers);

            while (hi.next()) {
                const QoreValue v = hi.get();
                const char* key = hi.getKey();
                if (addsize && !strcasecmp(key, "transfer-encoding"))
                    addsize = false;
                if (addsize && !strcasecmp(key, "content-length"))
                    addsize = false;
                if (v.getType() == NT_LIST) {
                    ConstListIterator li(v.get<const QoreListNode>());
                    while (li.next())
                        do_header(key, hdr, li.getValue());
                } else
                    do_header(key, hdr, v);
            }
        }
        // add data and content-length header if necessary
        if (size || addsize) {
            hdr.sprintf("Content-Length: " QSD "\r\n", size);
        }

        hdr.concat("\r\n");
    }

    DLLLOCAL int listen(int backlog = 20) {
        if (sock == QORE_INVALID_SOCKET)
            return QSE_NOT_OPEN;
        if (in_op >= 0)
            return QSE_IN_OP;
#ifdef _Q_WINDOWS
        if (::listen(sock, backlog)) {
            // set errno
            sock_get_error();
            return -1;
        }
        return 0;
#else
        return ::listen(sock, backlog);
#endif
    }

    DLLLOCAL int accept_intern(ExceptionSink* xsink, struct sockaddr *addr, socklen_t *size, int timeout_ms = -1) {
        //printd(5, "qore_socket_private::accept_intern() to: %d\n", timeout_ms);
        assert(xsink);
        while (true) {
            if (timeout_ms >= 0 && !isDataAvailable(timeout_ms, "accept", xsink)) {
                if (*xsink)
                    return -1;
                // do not throw exception here, NOTHING will be returned in Qore on timeout
                return QSE_TIMEOUT; // -3
            }

            int rc = ::accept(sock, addr, size);
            if (rc != QORE_INVALID_SOCKET)
                return rc;

            // retry if interrupted by a signal
            if (sock_get_error() == EINTR)
                continue;

            qore_socket_error(xsink, "SOCKET-ACCEPT-ERROR", "error in accept()", 0, 0, 0, addr);
            return -1;
        }
    }

    // returns a new socket
    DLLLOCAL int accept_internal(ExceptionSink* xsink, SocketSource *source, int timeout_ms = -1) {
        assert(xsink);
        if (sock == QORE_INVALID_SOCKET) {
            xsink->raiseException("SOCKET-NOT-OPEN", "socket must be opened, bound, and in a listening state before new connections can be accepted");
            return QSE_NOT_OPEN;
        }
        if (in_op >= 0) {
            if (in_op == q_gettid()) {
                se_in_op("Socket", "accept", xsink);
                return QSE_IN_OP;
            }
            se_in_op_thread("Socket", "accept", xsink);
            return QSE_IN_OP_THREAD;
        }

        int rc;
        if (sfamily == AF_UNIX) {
#ifdef _Q_WINDOWS
            xsink->raiseException("SOCKET-ACCEPT-ERROR", "UNIX sockets are not available under Windows");
            return -1;
#else
            struct sockaddr_un addr_un;

#if defined(HPUX) && defined(__ia64) && defined(__LP64__)
            // on HPUX 64-bit the OS defines socklen_t to be 8 bytes
            // but the library expects a 32-bit value
            int size = sizeof(struct sockaddr_un);
#else
            socklen_t size = sizeof(struct sockaddr_un);
#endif
            rc = accept_intern(xsink, (struct sockaddr *)&addr_un, (socklen_t *)&size, timeout_ms);
            //printd(1, "qore_socket_private::accept_internal() " QSD " bytes returned\n", size);

            if (rc >= 0 && source) {
                QoreStringNode* addr = new QoreStringNode(enc);
                addr->sprintf("UNIX socket: %s", socketname.c_str());
                source->priv->setAddress(addr);
                source->priv->setHostName("localhost");
            }
#endif // windows
        } else if (sfamily == AF_INET || sfamily == AF_INET6) {
            struct sockaddr_storage addr_in;
#if defined(HPUX) && defined(__ia64) && defined(__LP64__)
            // on HPUX 64-bit the OS defines socklen_t to be 8 bytes
            // but the library expects a 32-bit value
            int size = sizeof(addr_in);
#else
            socklen_t size = sizeof(addr_in);
#endif

            rc = accept_intern(xsink, (struct sockaddr *)&addr_in, (socklen_t *)&size, timeout_ms);
            //printd(1, "qore_socket_private::accept_internal() rc: %d, %d bytes returned\n", rc, size);

            if (rc >= 0 && source) {
                char host[NI_MAXHOST + 1];
                char service[NI_MAXSERV + 1];

                if (!getnameinfo((struct sockaddr *)&addr_in, qore_get_in_len((struct sockaddr *)&addr_in), host, sizeof(host), service, sizeof(service), NI_NUMERICSERV)) {
                    source->priv->setHostName(host);
                }

                // get ipv4 or ipv6 address
                char ifname[INET6_ADDRSTRLEN];
                if (inet_ntop(addr_in.ss_family, qore_get_in_addr((struct sockaddr *)&addr_in), ifname, sizeof(ifname))) {
                    //printd(5, "inet_ntop() '%s' host: '%s'\n", ifname, host);
                    source->priv->setAddress(ifname);
                }
            }
        } else {
            // should not happen
            xsink->raiseException("SOCKET-ACCEPT-ERROR", "do not know how to accept connections with address family %d", sfamily);
            rc = -1;
        }
        return rc;
    }

    DLLLOCAL QoreHashNode* getEvent(int event, int source = QORE_SOURCE_SOCKET) const {
        QoreHashNode* h = new QoreHashNode(autoTypeInfo);
        if (event_arg) {
            h->setKeyValue("arg", event_arg.refSelf(), nullptr);
        }

        h->setKeyValue("event", event, nullptr);
        h->setKeyValue("source", source, nullptr);
        h->setKeyValue("id", (int64)this, nullptr);

        return h;
    }

    DLLLOCAL void cleanup(ExceptionSink* xsink) {
        if (event_queue) {
            // close the socket before the delete message is put on the queue
            // the socket would be closed anyway in the destructor
            close_internal();

            event_queue->pushAndTakeRef(getEvent(QORE_EVENT_DELETED));

            // deref and remove event queue
            event_queue->deref(xsink);
            event_queue = nullptr;
        }
        if (warn_queue) {
            warn_queue->deref(xsink);
            warn_queue = nullptr;
            if (warn_callback_arg) {
                warn_callback_arg.discard(xsink);
                warn_callback_arg.clear();
            }
        }
    }

    DLLLOCAL void setEventQueue(ExceptionSink* xsink, Queue* q, QoreValue arg, bool with_data) {
        if (event_queue) {
            if (event_arg) {
                event_arg.discard(xsink);
            }
            event_queue->deref(xsink);
        }
        event_queue = q;
        event_arg = arg;
        event_data = with_data;
    }

    DLLLOCAL void do_start_ssl_event() {
        if (event_queue) {
            event_queue->pushAndTakeRef(getEvent(QORE_EVENT_START_SSL));
        }
    }

    DLLLOCAL void do_ssl_established_event() {
        if (event_queue) {
            QoreHashNode* h = getEvent(QORE_EVENT_SSL_ESTABLISHED);
            h->setKeyValue("cipher", new QoreStringNode(ssl->getCipherName()), nullptr);
            h->setKeyValue("cipher_version", new QoreStringNode(ssl->getCipherVersion()), nullptr);
            event_queue->pushAndTakeRef(h);
        }
    }

    DLLLOCAL void do_connect_event(int af, const struct sockaddr* addr, const char* target, const char* service = nullptr, int prt = -1) {
        if (event_queue) {
            QoreHashNode* h = getEvent(QORE_EVENT_CONNECTING);
            QoreStringNode* str = q_addr_to_string2(addr);
            if (str) {
                h->setKeyValue("address", str, nullptr);
            } else {
                h->setKeyValue("error", q_strerror(sock_get_error()), nullptr);
            }
            q_af_to_hash(af, *h, nullptr);
            h->setKeyValue("target", new QoreStringNode(target), nullptr);
            if (service)
                h->setKeyValue("service", new QoreStringNode(service), nullptr);
            if (prt != -1)
                h->setKeyValue("port", prt, nullptr);
            event_queue->pushAndTakeRef(h);
        }
    }

    DLLLOCAL void do_connected_event() {
        if (event_queue) {
            event_queue->pushAndTakeRef(getEvent(QORE_EVENT_CONNECTED));
        }
    }

    DLLLOCAL void do_data_event_intern(int event, int source, const QoreStringNode& str) const {
        assert(event_queue && event_data && str.size());
        ReferenceHolder<QoreHashNode> h(getEvent(event, source), nullptr);
        h->setKeyValue("data", str.refSelf(), nullptr);
        event_queue->pushAndTakeRef(h.release());
    }

    DLLLOCAL void do_data_event(int event, int source, const QoreStringNode& str) const {
        if (event_queue && event_data && str.size()) {
            do_data_event_intern(event, source, str);
        }
    }

    DLLLOCAL void do_data_event(int event, int source, const BinaryNode& b) const {
        if (event_queue && event_data && b.size()) {
            ReferenceHolder<QoreHashNode> h(getEvent(event, source), nullptr);
            h->setKeyValue("data", b.refSelf(), nullptr);
            event_queue->pushAndTakeRef(h.release());
        }
    }

    DLLLOCAL void do_data_event(int event, int source, const void* data, size_t size) const {
        if (event_queue && event_data && size) {
            ReferenceHolder<QoreHashNode> h(getEvent(event, source), nullptr);
            SimpleRefHolder<BinaryNode> b(new BinaryNode);
            b->append(data, size);
            h->setKeyValue("data", b.release(), nullptr);
            event_queue->pushAndTakeRef(h.release());
        }
    }

    DLLLOCAL void do_header_event(int event, int source, const QoreHashNode& hdr) const {
        if (event_queue && event_data && !hdr.empty()) {
            ReferenceHolder<QoreHashNode> h(getEvent(event, source), nullptr);
            h->setKeyValue("headers", hdr.refSelf(), nullptr);
            event_queue->pushAndTakeRef(h.release());
        }
    }

    DLLLOCAL void do_chunked_read(int event, qore_size_t bytes, qore_size_t total_read, int source) {
        if (event_queue) {
            QoreHashNode* h = getEvent(event, source);
            if (event == QORE_EVENT_HTTP_CHUNKED_DATA_RECEIVED)
                h->setKeyValue("read", bytes, nullptr);
            else
                h->setKeyValue("size", bytes, nullptr);
            h->setKeyValue("total_read", total_read, nullptr);
            event_queue->pushAndTakeRef(h);
        }
    }

    DLLLOCAL void do_read_http_header(int event, const QoreHashNode* headers, int source) {
        if (event_queue) {
            QoreHashNode* h = getEvent(event, source);
            h->setKeyValue("headers", headers->hashRefSelf(), nullptr);
            event_queue->pushAndTakeRef(h);
        }
    }

    DLLLOCAL void do_send_http_message_event(const QoreString& str, const QoreHashNode* headers, int source) {
        if (event_queue) {
            QoreHashNode* h = getEvent(QORE_EVENT_HTTP_SEND_MESSAGE, source);
            h->setKeyValue("message", new QoreStringNode(str), nullptr);
            //printd(5, "do_send_http_message_event() str='%s' headers: %p (%d %s)\n", str.getBuffer(), headers, headers->getType(), headers->getTypeName());
            h->setKeyValue("headers", headers->hashRefSelf(), nullptr);
            event_queue->pushAndTakeRef(h);
        }
    }

    DLLLOCAL void do_close_event() {
        if (event_queue) {
            event_queue->pushAndTakeRef(getEvent(QORE_EVENT_CHANNEL_CLOSED));
        }
    }

    DLLLOCAL void do_read_event(size_t bytes_read, size_t total_read, size_t bufsize = 0, int source = QORE_SOURCE_SOCKET) {
        // post bytes read on event queue, if any
        if (event_queue) {
            QoreHashNode* h = getEvent(QORE_EVENT_PACKET_READ, source);
            h->setKeyValue("read", bytes_read, nullptr);
            h->setKeyValue("total_read", total_read, nullptr);
            // set total bytes to read and remaining bytes if bufsize > 0
            if (bufsize > 0)
                h->setKeyValue("total_to_read", bufsize, nullptr);
            event_queue->pushAndTakeRef(h);
        }
    }

    DLLLOCAL void do_send_event(int bytes_sent, int total_sent, int bufsize) {
        // post bytes sent on event queue, if any
        if (event_queue) {
            QoreHashNode* h = getEvent(QORE_EVENT_PACKET_SENT);
            h->setKeyValue("sent", bytes_sent, nullptr);
            h->setKeyValue("total_sent", total_sent, nullptr);
            h->setKeyValue("total_to_send", bufsize, nullptr);
            event_queue->pushAndTakeRef(h);
        }
    }

    DLLLOCAL void do_resolve_event(const char* host, const char* service = 0) {
        // post bytes sent on event queue, if any
        if (event_queue) {
            QoreHashNode* h = getEvent(QORE_EVENT_HOSTNAME_LOOKUP);
            if (host)
                h->setKeyValue("name", new QoreStringNode(host), nullptr);
            if (service)
                h->setKeyValue("service", new QoreStringNode(service), nullptr);
            event_queue->pushAndTakeRef(h);
        }
    }

    DLLLOCAL void do_resolved_event(const struct sockaddr* addr) {
        // post bytes sent on event queue, if any
        if (event_queue) {
            QoreHashNode* h = getEvent(QORE_EVENT_HOSTNAME_RESOLVED);
            QoreStringNode* str = q_addr_to_string2(addr);
            if (str)
                h->setKeyValue("address", str, nullptr);
            else
                h->setKeyValue("error", q_strerror(sock_get_error()), nullptr);
            int prt = q_get_port_from_addr(addr);
            if (prt > 0)
                h->setKeyValue("port", prt, nullptr);
            q_af_to_hash(addr->sa_family, *h, nullptr);
            event_queue->pushAndTakeRef(h);
        }
    }

    DLLLOCAL int64 getObjectIDForEvents() const {
        return (int64)this;
    }

    DLLLOCAL int connectUNIX(const char* p, int sock_type, int protocol, ExceptionSink* xsink) {
        assert(xsink);
        assert(p);
        QORE_TRACE("connectUNIX()");

#ifdef _Q_WINDOWS
        xsink->raiseException("SOCKET-CONNECTUNIX-ERROR", "UNIX sockets are not available under Windows");
        return -1;
#else
        // close socket if already open
        close();

        printd(5, "qore_socket_private::connectUNIX(%s)\n", p);

        struct sockaddr_un addr;

        addr.sun_family = AF_UNIX;
        // copy path and terminate if necessary
        strncpy(addr.sun_path, p, sizeof(addr.sun_path) - 1);
        addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';
        if ((sock = socket(AF_UNIX, sock_type, protocol)) == QORE_SOCKET_ERROR) {
            xsink->raiseErrnoException("SOCKET-CONNECT-ERROR", errno, "error connecting to UNIX socket: '%s'", p);
            return -1;
        }

        do_connect_event(AF_UNIX, (sockaddr*)&addr, p);
        while (true) {
            if (!::connect(sock, (const sockaddr *)&addr, sizeof(struct sockaddr_un)))
                break;

            // try again if we were interrupted by a signal
            if (sock_get_error() == EINTR)
                continue;

            // otherwise close the socket and return an exception with the error code
            // do not have to worry about windows API calls here; this is a UNIX-only function
            close_and_reset();
            qore_socket_error(xsink, "SOCKET-CONNECT-ERROR", "error in connect()", 0, p);

            return -1;
        }

        // save file name for deleting when socket is closed
        socketname = addr.sun_path;
        sfamily = AF_UNIX;

        do_connected_event();

        return 0;
#endif // windows
    }

    // socket must be open or -1 is returned and a Qore-language exception is raised
    /* return values:
        -1: error
        0: timeout
        > 0: I/O can continue
    */
    DLLLOCAL int asyncIoWait(int timeout_ms, bool read, bool write, const char* cname, const char* mname, ExceptionSink* xsink) const {
        assert(xsink);
        assert(read || write);
        if (sock == QORE_INVALID_SOCKET) {
            se_not_open(cname, mname, xsink, "asyncIoWait");
            return -1;
        }

        return asyncIoWait(timeout_ms, read, write, xsink);
    }

    DLLLOCAL int asyncIoWait(int timeout_ms, bool read, bool write, ExceptionSink* xsink) const {
        assert(xsink);
#if defined HAVE_POLL
        return poll_intern(xsink, timeout_ms, read, write);
#elif defined HAVE_SELECT
        return select_intern(xsink, timeout_ms, read, write);
#else
#error no async socket operations supported
#endif
    }

#if defined HAVE_POLL
    DLLLOCAL int poll_intern(ExceptionSink* xsink, int timeout_ms, bool read, bool write) const {
        int rc;
        short arg = 0;
        if (read)
            arg |= POLLIN;
        if (write)
            arg |= POLLOUT;
        pollfd fds = {sock, arg, 0};
        while (true) {
            rc = ::poll(&fds, 1, timeout_ms);
            if (rc == -1 && errno == EINTR)
                continue;
            break;
        }
        if (rc < 0)
            qore_socket_error(xsink, "SOCKET-SELECT-ERROR", "poll(2) returned an error");
        else if (!rc && ((fds.revents & POLLHUP) || (fds.revents & (POLLERR|POLLNVAL))))
            rc = -1;

        return rc;
    }
#elif defined HAVE_SELECT
    DLLLOCAL int select_intern(ExceptionSink* xsink, int timeout_ms, bool read, bool write) const {
        bool aborted = false;
        int rc = select_intern(xsink, timeout_ms, read, write, aborted);
        if (rc != QORE_SOCKET_ERROR && aborted)
            rc = -1;
        return rc;
    }

    DLLLOCAL int select_intern(ExceptionSink* xsink, int timeout_ms, bool read, bool write, bool& aborted) const {
        assert(xsink);
        assert(!aborted);
        // windows does not use FD_SETSIZE to limit the value of the highest socket descriptor in the set
        // instead it has a maximum of 64 sockets in the set; we only need one anyway
#ifndef _Q_WINDOWS
        // select is inherently broken since it can only handle descriptors < FD_SETSIZE, which is 1024 on Linux for example
        if (sock >= FD_SETSIZE) {
            xsink->raiseException("SOCKET-SELECT-ERROR", "fd is %d which is >= %d; contact the Qore developers to implement an alternative to select() on this platform", sock, FD_SETSIZE);
            return -1;
        }
#endif
        struct timeval tv;
        int rc;
        while (true) {
            // to be safe, we set the file descriptor arg after each EINTR (required on Linux for example)
            fd_set sfs, err;

            FD_ZERO(&sfs);
            FD_ZERO(&err);
            FD_SET(sock, &sfs);
            FD_SET(sock, &err);

            tv.tv_sec  = timeout_ms / 1000;
            tv.tv_usec = (timeout_ms % 1000) * 1000;

            fd_set* readfd = read ? &sfs : 0;
            fd_set* writefd = write ? &sfs : 0;

            rc = select(sock + 1, readfd, writefd, &err, &tv);
            //printd(5, "select_intern() rc: %d err: %d\n", rc, FD_ISSET(sock, &err));
            if (rc != QORE_SOCKET_ERROR) {
                if (FD_ISSET(sock, &err))
                    aborted = true;
                break;
            }
            if (sock_get_error() != EINTR)
                break;
        }
        if (rc == QORE_SOCKET_ERROR) {
            // do not close the socket here, even in case of EBADF, just return an error
            rc = 0;
            qore_socket_error(xsink, "SOCKET-SELECT-ERROR", "select(2) returned an error");
        }

        return rc;
    }
#endif

    DLLLOCAL bool tryReadSocketData(const char* mname, ExceptionSink* xsink) {
        assert(xsink);
        assert(!buflen);
        if (!ssl) {
            // issue #3564: see if any data is available on the socket
            return asyncIoWait(0, true, false, "Socket", mname, xsink);
        }
        // select can return true if there is protocol negotiation data available,
        // so we try to peek 1 byte of application data with a timeout of 0 with the SSL connection
        int rc = ssl->doSSLRW(xsink, mname, rbuf, 1, 0, PEEK, false);
        if (*xsink || (rc == QSE_TIMEOUT)) {
            return false;
        }
        return rc > 0 ? true : false;
    }

    DLLLOCAL bool isSocketDataAvailable(int timeout_ms, const char* mname, ExceptionSink* xsink) {
        return asyncIoWait(timeout_ms, true, false, "Socket", mname, xsink);
    }

    DLLLOCAL bool isDataAvailable(int timeout_ms, const char* mname, ExceptionSink* xsink) {
        if (buflen)
            return true;
        return isSocketDataAvailable(timeout_ms, mname, xsink);
    }

    DLLLOCAL bool isWriteFinished(int timeout_ms, const char* mname, ExceptionSink* xsink) {
        return asyncIoWait(timeout_ms, false, true, "Socket", mname, xsink);
    }

    DLLLOCAL int close_and_exit() {
        if (sock != QORE_INVALID_SOCKET)
            close_and_reset();
        return -1;
    }

    DLLLOCAL int connectINETTimeout(int timeout_ms, const struct sockaddr* ai_addr, qore_size_t ai_addrlen, ExceptionSink* xsink, bool only_timeout) {
        assert(xsink);
        PrivateQoreSocketTimeoutHelper toh(this, "connect");

        while (true) {
            if (!::connect(sock, ai_addr, ai_addrlen))
                return 0;

#ifdef _Q_WINDOWS
            if (sock_get_error() != EAGAIN) {
                qore_socket_error(xsink, "SOCKET-CONNECT-ERROR", "error in connect()", 0, 0, 0, ai_addr);
                break;
            }
#else
            // try again if we were interrupted by a signal
            if (errno == EINTR)
                continue;

            if (errno != EINPROGRESS)
                break;
#endif

            //printd(5, "qore_socket_private::connectINETTimeout() timeout_ms: %d errno: %d\n", timeout_ms, errno);

            // check for timeout or connection with EINPROGRESS
            while (true) {
#ifdef _Q_WINDOWS
                bool aborted = false;
                int rc = select_intern(xsink, timeout_ms, false, true, aborted);

                //printd(5, "qore_socket_private::connectINETTimeout() timeout_ms: %d rc: %d aborted: %d\n", timeout_ms, rc, aborted);

                // windows select() returns an error in the error socket set instead of an WSAECONNREFUSED error like UNIX,
                // so we simulate it here
                if (rc != QORE_SOCKET_ERROR && aborted) {
                    qore_socket_error(xsink, "SOCKET-CONNECT-ERROR", "error in connect()", 0, 0, 0, ai_addr);
                    return -1;
                }
#else
                int rc = asyncIoWait(timeout_ms, false, true, "Socket", "connectINETTimeout", xsink);
#endif
                if (*xsink)
                    return -1;

                //printd(5, "asyncIoWait(%d) returned %d\n", timeout_ms, rc);
                if (rc == QORE_SOCKET_ERROR && sock_get_error() != EINTR) {
                    if (!only_timeout)
                        qore_socket_error(xsink, "SOCKET-CONNECT-ERROR", "error in asyncIoWait() with Socket::connect() with timeout", 0, 0, 0, ai_addr);
                    return -1;
                } else if (rc > 0) {
                    // socket selected for write
                    socklen_t lon = sizeof(int);
                    int val;

                    if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (GETSOCKOPT_ARG_4)(&val), &lon) == QORE_SOCKET_ERROR) {
                        if (!only_timeout)
                            qore_socket_error(xsink, "SOCKET-CONNECT-ERROR", "error in getsockopt()", 0, 0, 0, ai_addr);
                        return -1;
                    }

                    if (val) {
                        if (only_timeout) {
                            errno = val;
                            return -1;
                        }
                        qore_socket_error_intern(val, xsink, "SOCKET-CONNECT-ERROR", "error in getsockopt()", 0, 0, 0, ai_addr);
                        return -1;
                    }

                    // connected successfully within the timeout period
                    return 0;
                } else {
                    SimpleRefHolder<QoreStringNode> desc(new QoreStringNodeMaker("timeout in connection after %dms", timeout_ms));
                    concat_target(*(*desc), ai_addr);
                    xsink->raiseException("SOCKET-CONNECT-ERROR", desc.release());
                    return -1;
                }
            }
        }

        return -1;
    }

    DLLLOCAL int sock_errno_err(const char* err, const char* desc, ExceptionSink* xsink) {
        sock = QORE_INVALID_SOCKET;
        qore_socket_error(xsink, err, desc);
        return -1;
    }

    DLLLOCAL int set_non_blocking(bool non_blocking, ExceptionSink* xsink) {
        assert(xsink);

        // ignore call when socket already closed
        if (sock == QORE_INVALID_SOCKET) {
            assert(!xsink || *xsink);
            return -1;
        }

#ifdef _Q_WINDOWS
        u_long mode = non_blocking ? 1 : 0;
        int rc = ioctlsocket(sock, FIONBIO, &mode);
        if (check_windows_rc(rc))
            return sock_errno_err("SOCKET-CONNECT-ERROR", "error in ioctlsocket(FIONBIO)", xsink);
#else
        int arg;

        // get socket descriptor status flags
        if ((arg = fcntl(sock, F_GETFL, 0)) < 0)
            return sock_errno_err("SOCKET-CONNECT-ERROR", "error in fcntl() getting socket descriptor status flag", xsink);

        if (non_blocking) // set non-blocking
            arg |= O_NONBLOCK;
        else // set blocking
            arg &= ~O_NONBLOCK;

        if (fcntl(sock, F_SETFL, arg) < 0)
            return sock_errno_err("SOCKET-CONNECT-ERROR", "error in fcntl() setting socket descriptor status flag", xsink);
#endif

        return 0;
    }

    DLLLOCAL int connectINET(const char* host, const char* service, int timeout_ms, ExceptionSink* xsink, int family = AF_UNSPEC, int type = SOCK_STREAM, int protocol = 0) {
        assert(xsink);
        family = q_get_af(family);
        type = q_get_sock_type(type);

        QORE_TRACE("qore_socket_private::connectINET()");

        // close socket if already open
        close();

        printd(5, "qore_socket_private::connectINET(%s:%s, %dms)\n", host, service, timeout_ms);

        do_resolve_event(host, service);

        QoreAddrInfo ai;
        if (ai.getInfo(xsink, host, service, family, 0, type, protocol))
            return -1;

        struct addrinfo *aip = ai.getAddrInfo();

        // emit all "resolved" events
        if (event_queue)
            for (struct addrinfo *p = aip; p; p = p->ai_next)
                do_resolved_event(p->ai_addr);

        int prt = q_get_port_from_addr(aip->ai_addr);

        for (struct addrinfo *p = aip; p; p = p->ai_next) {
            if (!connectINETIntern(host, service, p->ai_family, p->ai_addr, p->ai_addrlen, p->ai_socktype, p->ai_protocol, prt, timeout_ms, xsink, true))
                return 0;
            if (*xsink)
                break;
        }

        if (!*xsink)
            qore_socket_error(xsink, "SOCKET-CONNECT-ERROR", "error in connect()", 0, host, service);
        return -1;
    }

    DLLLOCAL int connectINETIntern(const char* host, const char* service, int ai_family, struct sockaddr* ai_addr, size_t ai_addrlen, int ai_socktype, int ai_protocol, int prt, int timeout_ms, ExceptionSink* xsink, bool only_timeout = false) {
        assert(xsink);
        printd(5, "qore_socket_private::connectINETIntern() host: %s service: %s family: %d timeout_ms: %d\n", host, service, ai_family, timeout_ms);
        if ((sock = socket(ai_family, ai_socktype, ai_protocol)) == QORE_INVALID_SOCKET) {
            xsink->raiseErrnoException("SOCKET-CONNECT-ERROR", errno, "cannot establish a connection to %s:%s", host, service);
            return -1;
        }

        //printd(5, "qore_socket_private::connectINETIntern(this: %p, host='%s', port: %d, timeout_ms: %d) sock: %d\n", this, host, port, timeout_ms, sock);

        int rc;

        // perform connect with timeout if a non-negative timeout was passed
        if (timeout_ms >= 0) {
            // set non-blocking
            if (set_non_blocking(true, xsink))
                return close_and_exit();

            do_connect_event(ai_family, ai_addr, host, service, prt);

            rc = connectINETTimeout(timeout_ms, ai_addr, ai_addrlen, xsink, only_timeout);
            //printd(5, "qore_socket_private::connectINETIntern() errno: %d rc: %d, xsink: %d\n", errno, rc, xsink && *xsink);

            // set blocking
            if (set_non_blocking(false, xsink))
                return close_and_exit();
        } else {
            do_connect_event(ai_family, ai_addr, host, service, prt);

            while (true) {
                rc = ::connect(sock, ai_addr, ai_addrlen);

                // try again if rc == -1 and errno == EINTR
                if (!rc || sock_get_error() != EINTR)
                    break;
            }
        }

        if (rc < 0) {
            if (!only_timeout || errno == ETIMEDOUT)
                qore_socket_error(xsink, "SOCKET-CONNECT-ERROR", "error in connect()", 0, host, service);

            return close_and_exit();
        }

        sfamily = ai_family;
        stype = ai_socktype;
        sprot = ai_protocol;
        port = prt;
        //printd(5, "qore_socket_private::connectINETIntern(this: %p, host='%s', port: %d, timeout_ms: %d) success, rc: %d, sock: %d\n", this, host, port, timeout_ms, rc, sock);

        do_connected_event();

        // issue #3053: save hostname for SNI
        client_target = host;
        return 0;
    }

    DLLLOCAL int upgradeClientToSSLIntern(const char* mname, const char* sni_target_host, X509* cert, EVP_PKEY* pkey, int timeout_ms, ExceptionSink* xsink) {
        assert(!ssl);
        SSLSocketHelperHelper sshh(this, true);

        int rc;
        do_start_ssl_event();
        // issue #3053: send target hostname to support SNI
        if (!sni_target_host && !client_target.empty()) {
            sni_target_host = client_target.c_str();
        }
        if ((rc = ssl->setClient(mname, sni_target_host, sock, cert, pkey, xsink)) || ssl->connect(mname, timeout_ms, xsink)) {
            sshh.error();
            return rc ? rc : -1;
        }
        do_ssl_established_event();

        return 0;
    }

    DLLLOCAL int upgradeServerToSSLIntern(const char* mname, X509* cert, EVP_PKEY* pkey, int timeout_ms, ExceptionSink* xsink) {
        assert(!ssl);
        //printd(5, "qore_socket_private::upgradeServerToSSLIntern() this: %p mode: %d\n", this, ssl_verify_mode);
        SSLSocketHelperHelper sshh(this, true);

        do_start_ssl_event();
        if (ssl->setServer(mname, sock, cert, pkey, xsink) || ssl->accept(mname, timeout_ms, xsink)) {
            sshh.error();
            return -1;
        }
        do_ssl_established_event();

        return 0;
    }

    // returns 0 = success, -1 = error
    DLLLOCAL int openUNIX(int sock_type = SOCK_STREAM, int protocol = 0) {
        if (sock != QORE_INVALID_SOCKET)
            close();

        if ((sock = socket(AF_UNIX, sock_type, protocol)) == QORE_INVALID_SOCKET) {
            return -1;
        }

        sfamily = AF_UNIX;
        stype = sock_type;
        sprot = protocol;
        port = -1;
        return 0;
    }

    // returns 0 = success, -1 = error
    DLLLOCAL int openINET(int family = AF_INET, int sock_type = SOCK_STREAM, int protocol = 0) {
        if (sock != QORE_INVALID_SOCKET)
            close();

        if ((sock = socket(family, sock_type, protocol)) == QORE_INVALID_SOCKET)
            return -1;

        sfamily = family;
        stype = sock_type;
        sprot = protocol;
        port = -1;
        return 0;
    }

    DLLLOCAL int reuse(int opt) {
        //printf("qore_socket_private::reuse(%s)\n", opt ? "true" : "false");
        return setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (SETSOCKOPT_ARG_4)&opt, sizeof(int));
    }

    // the only place where xsink is optional
    DLLLOCAL int bindIntern(struct sockaddr* ai_addr, size_t ai_addrlen, int prt, bool reuseaddr, ExceptionSink* xsink = 0) {
        reuse(reuseaddr);

        if ((::bind(sock, ai_addr, ai_addrlen)) == QORE_SOCKET_ERROR) {
            if (xsink)
                qore_socket_error(xsink, "SOCKET-BIND-ERROR", "error in bind()", 0, 0, 0, ai_addr);
            close();
            return -1;
        }

        // set port number
        if (prt)
            port = prt;
        else {
            // get port number
#if defined(HPUX) && defined(__ia64) && defined(__LP64__)
            // on HPUX 64-bit the OS defines socklen_t to be 8 bytes, but the library expects a 32-bit value
            int len = ai_addrlen;
#else
            socklen_t len = ai_addrlen;
#endif

            if (getsockname(sock, ai_addr, &len))
                port = -1;
            else
                port = q_get_port_from_addr(ai_addr);
        }
        return 0;
    }

    // bind to UNIX domain socket file
    DLLLOCAL int bindUNIX(ExceptionSink* xsink, const char* name, int socktype = SOCK_STREAM, int protocol = 0) {
        assert(xsink);
#ifdef _Q_WINDOWS
        xsink->raiseException("SOCKET-BINDUNIX-ERROR", "UNIX sockets are not available under Windows");
        return -1;
#else
        close();

        // try to open socket if necessary
        if (openUNIX(socktype, protocol)) {
            xsink->raiseErrnoException("SOCKET-BIND-ERROR", errno, "error opening UNIX socket ('%s') for bind", name);
            return -1;
        }

        struct sockaddr_un addr;
        addr.sun_family = AF_UNIX;
        // copy path and terminate if necessary
        strncpy(addr.sun_path, name, sizeof(addr.sun_path) - 1);
        addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';

        if (bindIntern((sockaddr*)&addr, sizeof(struct sockaddr_un), -1, false, xsink))
            return -1;

        // save socket file name for deleting on close
        socketname = addr.sun_path;
        // delete UNIX domain socket on close
        del = true;
        return 0;
#endif // windows
    }

    DLLLOCAL int bindINET(ExceptionSink* xsink, const char* name, const char* service, bool reuseaddr = true, int family = AF_UNSPEC, int socktype = SOCK_STREAM, int protocol = 0) {
        assert(xsink);
        family = q_get_af(family);
        socktype = q_get_sock_type(socktype);

        close();

        QoreAddrInfo ai;
        do_resolve_event(name, service);
        if (ai.getInfo(xsink, name, service, family, AI_PASSIVE, socktype, protocol))
            return -1;

        struct addrinfo *aip = ai.getAddrInfo();
        // first emit all "resolved" events
        if (event_queue)
            for (struct addrinfo *p = aip; p; p = p->ai_next)
                do_resolved_event(p->ai_addr);

        // try to open socket if necessary
        if (openINET(aip->ai_family, aip->ai_socktype, protocol)) {
            qore_socket_error(xsink, "SOCKET-BINDINET-ERROR", "error opening socket for bind", 0, name, service);
            return -1;
        }

        int prt = q_get_port_from_addr(aip->ai_addr);

        int en = 0;
        // iterate through addresses and bind to the first interface possible
        for (struct addrinfo *p = aip; p; p = p->ai_next) {
            if (!bindIntern(p->ai_addr, p->ai_addrlen, prt, reuseaddr)) {
            //printd(5, "qore_socket_private::bindINET(family: %d) bound: name: %s service: %s f: %d st: %d p: %d\n", family, name ? name : "(null)", service ? service : "(null)", p->ai_family, p->ai_socktype, p->ai_protocol);
                return 0;
            }

            en = sock_get_raw_error();
            //printd(5, "qore_socket_private::bindINET() failed to bind: name: %s service: %s f: %d st: %d p: %d, errno: %d (%s)\n", name ? name : "(null)", service ? service : "(null)", p->ai_family, p->ai_socktype, p->ai_protocol, en, strerror(en));
        }

        // if no bind was possible, then raise an exception
        qore_socket_error_intern(en, xsink, "SOCKET-BIND-ERROR", "error binding on socket", 0, name, service);
        return -1;
    }

    // only called from qore-bound code - always with xsink
    DLLLOCAL QoreHashNode* getPeerInfo(ExceptionSink* xsink, bool host_lookup = true) const {
        assert(xsink);
        if (sock == QORE_INVALID_SOCKET) {
            se_not_open("Socket", "getPeerInfo", xsink);
            return 0;
        }

        struct sockaddr_storage addr;
        socklen_t len = sizeof addr;
        if (getpeername(sock, (struct sockaddr*)&addr, &len)) {
            qore_socket_error(xsink, "SOCKET-GETPEERINFO-ERROR", "error in getpeername()");
            return 0;
        }

        return getAddrInfo(addr, len, host_lookup);
    }

    // only called from qore-bound code - always with xsink
    DLLLOCAL QoreHashNode* getSocketInfo(ExceptionSink* xsink, bool host_lookup = true) const {
        assert(xsink);
        if (sock == QORE_INVALID_SOCKET) {
            se_not_open("Socket", "getSocketInfo", xsink);
            return 0;
        }

        struct sockaddr_storage addr;
#if defined(HPUX) && defined(__ia64) && defined(__LP64__)
        // on HPUX 64-bit the OS defines socklen_t to be 8 bytes, but the library expects a 32-bit value
        int len = sizeof addr;
#else
        socklen_t len = sizeof addr;
#endif

        if (getsockname(sock, (struct sockaddr*)&addr, &len)) {
            qore_socket_error(xsink, "SOCKET-GETSOCKETINFO-ERROR", "error in getsockname()");
            return 0;
        }

        return getAddrInfo(addr, len, host_lookup);
    }

    DLLLOCAL QoreHashNode* getAddrInfo(const struct sockaddr_storage& addr, socklen_t len, bool host_lookup = true) const {
        QoreHashNode* h = new QoreHashNode(autoTypeInfo);

        if (addr.ss_family == AF_INET || addr.ss_family == AF_INET6) {
            if (host_lookup) {
                char host[NI_MAXHOST + 1];

                if (!getnameinfo((struct sockaddr*)&addr, qore_get_in_len((struct sockaddr*)&addr), host, sizeof(host), 0, 0, 0)) {
                    QoreStringNode* hoststr = new QoreStringNode(host);
                    h->setKeyValue("hostname", hoststr, 0);
                    h->setKeyValue("hostname_desc", QoreAddrInfo::getAddressDesc(addr.ss_family, hoststr->getBuffer()), 0);
                }
            }

            // get ipv4 or ipv6 address
            char ifname[INET6_ADDRSTRLEN];
            if (inet_ntop(addr.ss_family, qore_get_in_addr((struct sockaddr*)&addr), ifname, sizeof(ifname))) {
                QoreStringNode* addrstr = new QoreStringNode(ifname);
                h->setKeyValue("address", addrstr, 0);
                h->setKeyValue("address_desc", QoreAddrInfo::getAddressDesc(addr.ss_family, addrstr->getBuffer()), 0);
            }

            int tport;
            if (addr.ss_family == AF_INET) {
                struct sockaddr_in* s = (struct sockaddr_in*)&addr;
                tport = ntohs(s->sin_port);
            } else {
                struct sockaddr_in6* s = (struct sockaddr_in6*)&addr;
                tport = ntohs(s->sin6_port);
            }

            h->setKeyValue("port", tport, 0);
        }
#ifndef _Q_WINDOWS
        else if (addr.ss_family == AF_UNIX) {
            assert(!socketname.empty());
            QoreStringNode* addrstr = new QoreStringNode(socketname);
            h->setKeyValue("address", addrstr, 0);
            h->setKeyValue("address_desc", QoreAddrInfo::getAddressDesc(addr.ss_family, addrstr->getBuffer()), 0);
        }
#endif

        h->setKeyValue("family", addr.ss_family, 0);
        h->setKeyValue("familystr", new QoreStringNode(QoreAddrInfo::getFamilyName(addr.ss_family)), 0);

        return h;
    }

    // set backwards-compatible object members on accept
    // to be (hopefully) deleted in a future version of qore
    DLLLOCAL void setAccept(QoreObject* o) {
        struct sockaddr_storage addr;

        socklen_t len = sizeof addr;
        if (getpeername(sock, (struct sockaddr*)&addr, &len))
            return;

        if (addr.ss_family == AF_INET || addr.ss_family == AF_INET6) {
            // get ipv4 or ipv6 address
            char ifname[INET6_ADDRSTRLEN];
            if (inet_ntop(addr.ss_family, qore_get_in_addr((struct sockaddr *)&addr), ifname, sizeof(ifname))) {
                //printd(5, "inet_ntop() '%s' host: '%s'\n", ifname, host);
                o->setValue("source", new QoreStringNode(ifname), 0);
            }

            char host[NI_MAXHOST + 1];
            if (!getnameinfo((struct sockaddr *)&addr, qore_get_in_len((struct sockaddr *)&addr), host, sizeof(host), 0, 0, 0))
                o->setValue("source_host", new QoreStringNode(host), 0);
        }
#ifndef _Q_WINDOWS
        else if (addr.ss_family == AF_UNIX) {
            QoreStringNode* astr = new QoreStringNode(enc);
            struct sockaddr_un *addr_un = (struct sockaddr_un *)&addr;
            astr->sprintf("UNIX socket: %s", addr_un->sun_path);
            o->setValue("source", astr, 0);
            o->setValue("source_host", new QoreStringNode("localhost"), 0);
        }
#endif
    }

    // buffered reads for high performance
    DLLLOCAL qore_offset_t brecv(ExceptionSink* xsink, const char* meth, char*& buf, qore_size_t bs, int flags, int timeout, bool do_event = true) {
        assert(xsink);
        // must be checked if open/connected before this function is called
        assert(sock != QORE_INVALID_SOCKET);
        assert(meth);

        // always returned buffered data first
        if (buflen) {
            buf = rbuf + bufoffset;
            if (buflen <= bs) {
                bs = buflen;
                buflen = 0;
                bufoffset = 0;
            } else {
                buflen -= bs;
                bufoffset += bs;
            }
            return (qore_offset_t)bs;
        }

        // real socket reads are only done when the buffer is empty

        //printd(5, "qore_socket_private::brecv(buf: %p, bs: %d, flags: %d, timeout: %d, do_event: %d) this: %p ssl: %d\n", buf, (int)bs, flags, timeout, (int)do_event, this, ssl);

        qore_offset_t rc;
        if (!ssl) {
            if (timeout != -1 && !isDataAvailable(timeout, meth, xsink)) {
                if (*xsink)
                return -1;
                se_timeout("Socket", meth, timeout, xsink);

                return QSE_TIMEOUT;
            }

            while (true) {
#ifdef DEBUG
                errno = 0;
#endif
                rc = ::recv(sock, rbuf, DEFAULT_SOCKET_BUFSIZE, flags);
                if (rc == QORE_SOCKET_ERROR) {
                    sock_get_error();
                    if (errno == EINTR)
                        continue;
#ifdef ECONNRESET
                    if (errno == ECONNRESET) {
                        se_closed("Socket", meth, xsink);
                        close();
                    } else
#endif
                        qore_socket_error(xsink, "SOCKET-RECV-ERROR", "error in recv()", meth);
                    break;
                }
                //printd(5, "qore_socket_private::brecv(%d, %p, %ld, %d) rc: %ld errno: %d\n", sock, buf, bs, flags, rc, errno);
                // try again if we were interrupted by a signal
                if (rc >= 0)
                    break;
            }
        } else
            rc = ssl->read(meth, rbuf, DEFAULT_SOCKET_BUFSIZE, timeout, xsink);

        //printd(5, "qore_socket_private::brecv(%d, %p, %ld, %d) rc: %ld errno: %d\n", sock, buf, bs, flags, rc, errno);
        if (rc > 0) {
            buf = rbuf;
            assert(!buflen);
            assert(!bufoffset);
            if (rc > (qore_offset_t)bs) {
                buflen = rc - bs;
                bufoffset = bs;
                rc = bs;
            }

            // register event
            if (do_event)
                do_read_event(rc, rc);
        } else {
#ifdef DEBUG
            buf = 0;
#endif
            if (!rc)
                close();
        }

        return rc;
    }

    //! read until \\r\\n\\r\\n and return the string
    DLLLOCAL QoreStringNode* readHTTPData(ExceptionSink* xsink, const char* meth, int timeout, qore_offset_t& rc, bool exit_early = false) {
        assert(xsink);
        assert(meth);
        if (sock == QORE_INVALID_SOCKET) {
            se_not_open("Socket", meth, xsink, "readHTTPData");
            rc = QSE_NOT_OPEN;
            return 0;
        }

        PrivateQoreSocketThroughputHelper th(this, false);

        // state:
        //   0 = '\r' received
        //   1 = '\r\n' received
        //   2 = '\r\n\r' received
        //   3 = '\n' received
        // read in HHTP header until \r\n\r\n or \n\n from socket
        int state = -1;
        QoreStringNodeHolder hdr(new QoreStringNode(enc));

        qore_size_t count = 0;

        while (true) {
            char* buf;
            rc = brecv(xsink, meth, buf, 1, 0, timeout, false);
            //printd(5, "qore_socket_private::readHTTPData() this: %p Socket::%s(): rc: " QLLD " read char: %c (%03d) (old state: %d)\n", this, meth, rc, rc > 0 && buf[0] > 31 ? buf[0] : '?', rc > 0 ? buf[0] : 0, state);
            if (rc <= 0) {
                //printd(5, "qore_socket_private::readHTTPData(timeout: %d) hdr='%s' (len: %d), rc=" QSD ", errno: %d: '%s'\n", timeout, hdr->getBuffer(), hdr->strlen(), rc, errno, strerror(errno));

                if (!*xsink) {
                    if (!count) {
                        //printd(5, "qore_socket_private::readHTTPData() this: %p rc: %d count: %d (%d) timeout: %d\n", this, rc, count, hdr->size(), timeout);
                        se_closed("Socket", meth, xsink);
                    } else {
                        xsink->raiseExceptionArg("SOCKET-HTTP-ERROR", hdr.release(), "socket closed on remote end while reading header data after reading " QSD " byte%s", count, count == 1 ? "" : "s");
                    }
                }
                return 0;
            }
            char c = buf[0];
            if (++count == QORE_MAX_HEADER_SIZE) {
                xsink->raiseException("SOCKET-HTTP-ERROR", "header size cannot exceed " QSD " bytes", count);
                return 0;
            }

            // check if we can progress to the next state
            if (c == '\n') {
                if (state == -1) {
                    state = 3;
                    continue;
                }
                if (!state) {
                    if (exit_early && hdr->empty())
                        return 0;
                    state = 1;
                    continue;
                }
                assert(state > 0);
                break;
            } else if (c == '\r') {
                if (state == -1) {
                    state = 0;
                    continue;
                }
                if (!state)
                    break;
                if (state == 1) {
                    state = 2;
                    continue;
                }
            }

            if (state != -1) {
                switch (state) {
                    case 0: hdr->concat('\r'); break;
                    case 1: hdr->concat("\r\n"); break;
                    case 2: hdr->concat("\r\n\r"); break;
                    case 3: hdr->concat('\n'); break;
                }
                state = -1;
            }
            hdr->concat(c);
        }
        hdr->concat('\n');

        //printd(5, "qore_socket_private::readHTTPData(timeout: %d) hdr='%s' (%d)\n", timeout, hdr->getBuffer(), hdr->size());

        th.finalize(hdr->size());

        return hdr.release();
    }

    DLLLOCAL QoreStringNode* recv(ExceptionSink* xsink, qore_offset_t bufsize, int timeout, qore_offset_t& rc, int source = QORE_SOURCE_SOCKET) {
        assert(xsink);
        if (sock == QORE_INVALID_SOCKET) {
            se_not_open("Socket", "recv", xsink, "recv");
            rc = QSE_NOT_OPEN;
            return 0;
        }
        if (in_op >= 0) {
            if (in_op == q_gettid()) {
                se_in_op("Socket", "recv", xsink);
                return 0;
            }
            se_in_op_thread("Socket", "recv", xsink);
            return 0;
        }

        PrivateQoreSocketThroughputHelper th(this, false);

        qore_size_t bs = bufsize > 0 && bufsize < DEFAULT_SOCKET_BUFSIZE ? bufsize : DEFAULT_SOCKET_BUFSIZE;

        QoreStringNodeHolder str(new QoreStringNode(enc));

        char* buf;

        while (true) {
            rc = brecv(xsink, "recv", buf, bs, 0, timeout, false);

            if (rc <= 0) {
                printd(5, "qore_socket_private::recv(" QSD ", %d) bs=" QSD ", br=" QSD ", rc=" QSD ", errno: %d (%s)\n", bufsize, timeout, bs, str->size(), rc, errno, strerror(errno));
                break;
            }

            str->concat(buf, rc);

            // register event
            if (source > 0) {
                do_read_event(rc, str->size(), bufsize, source);
            }

            if (bufsize > 0) {
                if (str->size() >= (qore_size_t)bufsize)
                    break;
                if ((bufsize - str->size()) < bs)
                    bs = bufsize - str->size();
            }
        }

        printd(5, "qore_socket_private::recv() received " QSD " byte(s), bufsize=" QSD ", strlen=" QSD " str='%s'\n", str->size(), bufsize, (str ? str->strlen() : 0), str ? str->getBuffer() : "n/a");

        // "fix" return code value if no error occurred
        if (rc >= 0)
            rc = str->size();

        th.finalize(str->size());

        if (*xsink) {
            return nullptr;
        }

        if (source > 0) {
            do_data_event(QORE_EVENT_SOCKET_DATA_READ, source, **str);
        }
        return str.release();
    }

    DLLLOCAL QoreStringNode* recvAll(ExceptionSink* xsink, int timeout, qore_offset_t& rc, int source = QORE_SOURCE_SOCKET) {
        assert(xsink);
        if (sock == QORE_INVALID_SOCKET) {
            se_not_open("Socket", "recv", xsink, "recvAll");
            rc = QSE_NOT_OPEN;
            return 0;
        }
        if (in_op >= 0) {
            if (in_op == q_gettid()) {
                se_in_op("Socket", "recv", xsink);
                return 0;
            }
            se_in_op_thread("Socket", "recv", xsink);
            return 0;
        }

        PrivateQoreSocketThroughputHelper th(this, false);

        QoreStringNodeHolder str(new QoreStringNode(enc));

        // perform first read with timeout
        char* buf;
        rc = brecv(xsink, "recv", buf, DEFAULT_SOCKET_BUFSIZE, 0, timeout, false);
        if (rc <= 0)
            return 0;

        str->concat(buf, rc);

        // register event
        do_read_event(rc, rc);

        // keep reading data until no more data is available without a timeout
        if (isDataAvailable(0, "recv", xsink)) {
            do {
                rc = brecv(xsink, "recv", buf, DEFAULT_SOCKET_BUFSIZE, 0, 0, false);
                //printd(5, "qore_socket_private::recv(to: %d) rc=" QSD " rd=" QSD "\n", timeout, rc, str->size());
                // if the remote end has closed the connection, return what we have
                if (!rc)
                    break;
                if (rc < 0) {
                    th.finalize(str->size());
                    return 0;
                }
                str->concat(buf, rc);

                // register event
                do_read_event(rc, str->size());
            } while (isDataAvailable(0, "recv", xsink));
        }

        th.finalize(str->size());

        if (*xsink) {
            return nullptr;
        }

        rc = str->size();
        if (source > 0) {
            do_data_event(QORE_EVENT_SOCKET_DATA_READ, source, **str);
        }
        return str.release();
    }

    DLLLOCAL int recv(int fd, qore_offset_t size, int timeout_ms, ExceptionSink* xsink);

    DLLLOCAL BinaryNode* recvBinary(ExceptionSink* xsink, qore_offset_t bufsize, int timeout, qore_offset_t& rc, int source = QORE_SOURCE_SOCKET) {
        assert(xsink);
        if (sock == QORE_INVALID_SOCKET) {
            se_not_open("Socket", "recvBinary", xsink, "recvBinary");
            rc = QSE_NOT_OPEN;
            return 0;
        }
        if (in_op >= 0) {
            if (in_op == q_gettid()) {
                se_in_op("Socket", "recvBinary", xsink);
                return 0;
            }
            se_in_op_thread("Socket", "recvBinary", xsink);
            return 0;
        }

        PrivateQoreSocketThroughputHelper th(this, false);

        qore_size_t bs = bufsize > 0 && bufsize < DEFAULT_SOCKET_BUFSIZE ? bufsize : DEFAULT_SOCKET_BUFSIZE;

        SimpleRefHolder<BinaryNode> b(new BinaryNode);

        char* buf;
        while (true) {
            rc = brecv(xsink, "recvBinary", buf, bs, 0, timeout);
            if (rc <= 0)
                break;

            b->append(buf, rc);

            if (bufsize > 0) {
                if (b->size() >= (qore_size_t)bufsize)
                    break;
                if ((bufsize - b->size()) < bs)
                    bs = bufsize - b->size();
            }
        }

        th.finalize(b->size());

        if (*xsink)
            return nullptr;

        // "fix" return code value if no error occurred
        if (rc >= 0)
            rc = b->size();

        if (source > 0) {
            do_data_event(QORE_EVENT_SOCKET_DATA_READ, source, **b);
        }
        printd(5, "qore_socket_private::recvBinary() received " QSD " byte(s), bufsize=" QSD ", blen=" QSD "\n", b->size(), bufsize, b->size());
        return b.release();
    }

    DLLLOCAL BinaryNode* recvBinaryAll(ExceptionSink* xsink, int timeout, qore_offset_t& rc, int source = QORE_SOURCE_SOCKET) {
        assert(xsink);
        if (sock == QORE_INVALID_SOCKET) {
            se_not_open("Socket", "recvBinary", xsink, "recvBinaryAll");
            rc = QSE_NOT_OPEN;
            return 0;
        }
        if (in_op >= 0) {
            if (in_op == q_gettid()) {
                se_in_op("Socket", "recvBinary", xsink);
                return 0;
            }
            se_in_op_thread("Socket", "recvBinary", xsink);
            return 0;
        }

        PrivateQoreSocketThroughputHelper th(this, false);

        SimpleRefHolder<BinaryNode> b(new BinaryNode);

        //printd(5, "QoreSocket::recvBinary(%d, " QSD ") this: %p\n", timeout, rc, this);
        // perform first read with timeout
        char* buf;
        rc = brecv(xsink, "recvBinary", buf, DEFAULT_SOCKET_BUFSIZE, 0, timeout, false);
        if (rc <= 0)
            return 0;

        b->append(buf, rc);

        // register event
        do_read_event(rc, rc);

        // keep reading data until no more data is available without a timeout
        if (isDataAvailable(0, "recvBinary", xsink)) {
            do {
                rc = brecv(xsink, "recvBinary", buf, DEFAULT_SOCKET_BUFSIZE, 0, 0, false);
                // if the remote end has closed the connection, return what we have
                if (!rc)
                    break;
                if (rc < 0) {
                    th.finalize(b->size());
                    return 0;
                }

                b->append(buf, rc);

                // register event
                do_read_event(rc, b->size());
            } while (isDataAvailable(0, "recvBinary", xsink));
        }

        th.finalize(b->size());

        if (*xsink)
            return nullptr;

        if (source > 0) {
            do_data_event(QORE_EVENT_SOCKET_DATA_READ, source, **b);
        }
        rc = b->size();
        //printd(5, "qore_socket_private() this: %p b: %p size: %lld\n", this, b->getPtr(), rc);
        return b.release();
    }

    DLLLOCAL void recvToOutputStream(OutputStream *os, int64 size, int64 timeout, ExceptionSink *xsink, QoreThreadLock* l, int source = QORE_SOURCE_SOCKET) {
        if (sock == QORE_INVALID_SOCKET) {
            se_not_open("Socket", "recvToOutputStream", xsink);
            return;
        }
        if (in_op >= 0) {
            if (in_op == q_gettid()) {
                se_in_op("Socket", "recvToOutputStream", xsink);
                return;
            }
            se_in_op_thread("Socket", "recvToOutputStream", xsink);
            return;
        }

        qore_socket_op_helper oh(this);

        char* buf;
        qore_offset_t br = 0;
        while (size < 0 || br < size) {
            // calculate bytes needed
            int bn = size < 0 ? DEFAULT_SOCKET_BUFSIZE : QORE_MIN(size - br, DEFAULT_SOCKET_BUFSIZE);

            qore_offset_t rc = brecv(xsink, "recvToOutputStream", buf, bn, 0, timeout);
            if (rc < 0) {
                //error - already reported in xsink
                return;
            }
            if (rc == 0) {
                //eof
                if (size >= 0) {
                    //not all size bytes were read
                    xsink->raiseException("SOCKET-RECV-ERROR", "Unexpected end of stream");
                }
                return;
            }

            if (source > 0) {
                do_data_event(QORE_EVENT_SOCKET_DATA_READ, source, buf, rc);
            }

            // write buffer to the stream
            {
                AutoUnlocker al(l);
                os->write(buf, rc, xsink);
                if (*xsink) {
                    return;
                }
            }

            br += rc;
        }
    }

    DLLLOCAL QoreStringNode* readHTTPHeaderString(ExceptionSink* xsink, int timeout, int source) {
        assert(xsink);
        qore_offset_t rc;
        QoreStringNodeHolder hdr(readHTTPData(xsink, "readHTTPHeaderString", timeout, rc));
        if (!hdr) {
            assert(*xsink);
            return 0;
        }
        assert(rc > 0);
        do_data_event(QORE_EVENT_HTTP_HEADERS_READ, source, **hdr);
        return hdr.release();
    }

    DLLLOCAL QoreHashNode* readHTTPHeader(ExceptionSink* xsink, QoreHashNode* info, int timeout,
        qore_offset_t& rc, int source, const char* headers_raw_key = "headers-raw") {
        assert(xsink);
        QoreStringNodeHolder hdr(readHTTPData(xsink, "readHTTPHeader", timeout, rc));
        if (!hdr) {
            assert(*xsink);
            return nullptr;
        }
        assert(rc > 0);

        const char* buf = hdr->getBuffer();

        char* p;
        if ((p = (char*)strstr(buf, "\r\n"))) {
            *p = '\0';
            p += 2;
        } else if ((p = (char*)strchr(buf, '\n'))) {
            *p = '\0';
            ++p;
        } else if ((p = (char*)strchr(buf, '\r'))) {
            *p = '\0';
            ++p;
        } else {
            // readHTTPData will only return a string that satisifies one of the above conditions,
            // however an embedded 0 could have been sent which would make the above searches invalid
            xsink->raiseException("SOCKET-HTTP-ERROR", "invalid header received with embedded nulls in Socket::readHTTPHeader()");
            return nullptr;
        }

        char* t1;
        if (!(t1 = (char*)strstr(buf, "HTTP/"))) {
            xsink->raiseExceptionArg("SOCKET-HTTP-ERROR", hdr.release(), "missing HTTP version string in first header line in Socket::readHTTPHeader()");
            return nullptr;
        }

        ReferenceHolder<QoreHashNode> h(new QoreHashNode(autoTypeInfo), xsink);

        // process header flags
        int flags = CHF_PROCESS;

        // get version
        {
            QoreStringNode* hv = new QoreStringNode(t1 + 5, 3, enc);
            h->setKeyValue("http_version", hv, nullptr);
            if (*hv == "1.1")
                flags |= CHF_HTTP11;
        }

        // if we are getting a response
        // key for info if applicable
        const char* info_key;
        if (t1 == buf) {
            char* t2 = (char*)strchr(buf + 8, ' ');
            if (t2) {
                t2++;
                if (isdigit(*(t2))) {
                    h->setKeyValue("status_code", atoi(t2), nullptr);
                    if (strlen(t2) > 4) {
                        h->setKeyValue("status_message", new QoreStringNode(t2 + 4), nullptr);
                    }
                }
            }
            // write the status line as the "response-uri" key in the info hash if present
            // NOTE: this is not a URI, so the name is not really appropriate
            info_key = "response-uri";
        } else { // get method and path
            char* t2 = (char*)strchr(buf, ' ');
            if (t2) {
                *t2 = '\0';
                h->setKeyValue("method", new QoreStringNode(buf), nullptr);
                t2++;
                t1 = strchr(t2, ' ');
                if (t1) {
                    *t1 = '\0';
                    //printd(5, "found path '%s'\n", t2);
                    // the path is returned as-is with no decodings - use decode_url() to decode
                    h->setKeyValue("path", new QoreStringNode(t2, enc), nullptr);
                }
            }
            info_key = "request-uri";
            flags |= CHF_REQUEST;
        }

        // write status line or request line to the info hash and raise a data event if applicable
        if (info || (event_queue && event_data)) {
            QoreStringNodeHolder status_line(new QoreStringNode(buf));
            if (info && event_queue && event_data) {
                status_line->ref();
            }
            if (event_queue && event_data) {
                do_data_event_intern(QORE_EVENT_SOCKET_DATA_READ, source, **status_line);
            }
            if (info) {
                info->setKeyValue(info_key, *status_line, nullptr);
            }
            status_line.release();
        }

        bool close = convertHeaderToHash(*h, p, flags, info, &http_exp_chunked_body, headers_raw_key);
        do_read_http_header(QORE_EVENT_HTTP_MESSAGE_RECEIVED, *h, source);

        // process header info
        if ((flags & CHF_REQUEST) && info)
            info->setKeyValue("close", close, 0);

        return h.release();
    }

    // info must be already referenced for the assignment, if present
    DLLLOCAL int runHeaderCallback(ExceptionSink* xsink, const char* cname, const char* mname, const ResolvedCallReferenceNode& callback, QoreThreadLock* l, const QoreHashNode* hdr, QoreHashNode* info, bool send_aborted = false, QoreObject* obj = nullptr) {
        assert(xsink);
        assert(obj);
        ReferenceHolder<QoreListNode> args(new QoreListNode(autoTypeInfo), xsink);
        QoreHashNode* arg = new QoreHashNode(autoTypeInfo);
        arg->setKeyValue("hdr", hdr ? hdr->refSelf() : nullptr, xsink);
        arg->setKeyValue("info", info, xsink);
        if (obj)
            arg->setKeyValue("obj", obj->refSelf(), xsink);
        arg->setKeyValue("send_aborted", send_aborted, xsink);
        args->push(arg, nullptr);

        ValueHolder rv(xsink);
        return runCallback(xsink, cname, mname, rv, callback, l, *args);
    }

    DLLLOCAL int runTrailerCallback(ExceptionSink* xsink, const char* cname, const char* mname, const ResolvedCallReferenceNode& callback, QoreThreadLock* l, ReferenceHolder<QoreHashNode>& hdr) {
        ValueHolder rv(xsink);
        if (runCallback(xsink, cname, mname, rv, callback, l, nullptr))
            return -1;

        switch (rv->getType()) {
            case NT_NOTHING:
                break;
            case NT_HASH: {
                hdr = rv.release().get<QoreHashNode>();
                break;
            }
            default:
                xsink->raiseException("HTTP-TRAILER-ERROR", "chunked callback returned type '%s'; expecting 'hash' or 'NOTHING'", rv->getTypeName());
                return -1;
        }
        return 0;
    }

    DLLLOCAL int runDataCallback(ExceptionSink* xsink, const char* cname, const char* mname, const ResolvedCallReferenceNode& callback, QoreThreadLock* l, const AbstractQoreNode* data, bool chunked) {
        assert(xsink);
        ReferenceHolder<QoreListNode> args(new QoreListNode(autoTypeInfo), xsink);
        QoreHashNode* arg = new QoreHashNode(autoTypeInfo);
        arg->setKeyValue("data", data->realCopy(), xsink);
        arg->setKeyValue("chunked", chunked, xsink);
        args->push(arg, nullptr);

        ValueHolder rv(xsink);
        return runCallback(xsink, cname, mname, rv, callback, l, *args);
    }

    DLLLOCAL int runCallback(ExceptionSink* xsink, const char* cname, const char* mname, ValueHolder& res, const ResolvedCallReferenceNode& callback, QoreThreadLock* l, const QoreListNode* args = nullptr) {
        assert(xsink);
        // FIXME: subtract callback execution time from socket performance measurement

        // unlock and execute callback
        {
            AutoUnlocker al(l);
            res = callback.execValue(args, xsink);
        }

        // check exception and socket status
        assert(xsink);
        if (*xsink)
            return -1;

        if (sock == QORE_INVALID_SOCKET) {
            se_not_open(cname, mname, xsink, "runCallback");
            return QSE_NOT_OPEN;
        }

        return 0;
    }

    DLLLOCAL int sendHttpChunkedWithCallback(ExceptionSink* xsink, const char* cname, const char* mname, const ResolvedCallReferenceNode& send_callback, QoreThreadLock& l, int source, int timeout_ms = -1, bool* aborted = nullptr) {
        assert(xsink);
        assert(!aborted || !(*aborted));

        if (sock == QORE_INVALID_SOCKET) {
            se_not_open(cname, mname, xsink, "sendHttpChunkedWithCallback");
            return QSE_NOT_OPEN;
        }
        if (in_op >= 0) {
            if (in_op == q_gettid()) {
                se_in_op(cname, mname, xsink);
                return 0;
            }
            se_in_op_thread(cname, mname, xsink);
            return 0;
        }

        PrivateQoreSocketThroughputHelper th(this, true);

        // set the non-blocking flag (for use with non-ssl connections)
        bool nb = (timeout_ms >= 0);
        // set non-blocking I/O (and restore on exit) if we have a timeout and a non-ssl connection
        OptionalNonBlockingHelper onbh(*this, !ssl && nb, xsink);
        if (*xsink)
            return -1;

        qore_socket_op_helper oh(this);

        qore_offset_t rc;
        int64 total = 0;
        bool done = false;

        while (!done) {
            // if we have response data already, then we assume an error and abort
            if (aborted) {
                bool data_available = tryReadSocketData(mname, xsink);
                //printd(5, "qore_socket_private::sendHttpChunkedWithCallback() this: %p aborted: %p iDA: %d\n", this, aborted, data_available);
                if (data_available || *xsink) {
                    *aborted = true;
                    return *xsink ? -1 : 0;
                }
            }

            // FIXME: subtract callback execution time from socket performance measurement
            ValueHolder res(xsink);
            rc = runCallback(xsink, cname, mname, res, send_callback, &l);
            if (rc)
                return rc;

            //printd(5, "qore_socket_private::sendHttpChunkedWithCallback() this: %p res: %s\n", this, get_type_name(*res));

            // check callback return val
            QoreString buf;
            // do not copy data here; set references and send the data directly
            const char* data_ptr = nullptr;
            size_t data_size = 0;

            switch (res->getType()) {
                case NT_STRING: {
                    const QoreStringNode* str = res->get<const QoreStringNode>();
                    if (str->empty()) {
                        done = true;
                        break;
                    }
                    buf.sprintf("%x\r\n", (int)str->size());
                    data_ptr = str->c_str();
                    data_size = str->size();
                    //buf.concat(str->c_str(), str->size());
                    break;
                }

                case NT_BINARY: {
                    const BinaryNode* b = res->get<const BinaryNode>();
                    if (b->empty()) {
                        done = true;
                        break;
                    }
                    buf.sprintf("%x\r\n", (int)b->size());
                    data_ptr = static_cast<const char*>(b->getPtr());
                    data_size = b->size();
                    //buf.concat((const char*)b->getPtr(), b->size());
                    break;
                }

                case NT_HASH: {
                    buf.concat("0\r\n");

                    const QoreHashNode* h = res->get<const QoreHashNode>();
                    ConstHashIterator hi(h);
                    while (hi.next()) {
                        const QoreValue v = hi.get();
                        const char* key = hi.getKey();

                        //printd(5, "qore_socket_private::sendHttpChunkedWithCallback() this: %p trailer %s\n", this, key);

                        if (v.getType() == NT_LIST) {
                            ConstListIterator li(v.get<const QoreListNode>());
                            while (li.next())
                                do_header(key, buf, li.getValue());
                        } else
                            do_header(key, buf, v);
                    }
                    // fall through to next case
                }

                case NT_NOTHING:
                case NT_NULL:
                    done = true;
                    break;

                default:
                    xsink->raiseException("SOCKET-CALLBACK-ERROR", "HTTP chunked data callback returned type '%s'; expecting one of: 'string', 'binary', 'hash', 'nothing' (or 'NULL')", res->getTypeName());
                    return -1;
            }

            // send chunk buffer data
            if (!buf.empty()) {
                rc = sendIntern(xsink, cname, mname, buf.c_str(), buf.size(), timeout_ms, total, true);
            }

            if (!*xsink) {
                assert(rc >= 0);
                // send actual data, if available
                if (data_ptr && data_size) {
                    rc = sendIntern(xsink, cname, mname, data_ptr, data_size, timeout_ms, total, true);
                }

                if (!*xsink) {
                    assert(rc >= 0);
                    if (buf.empty() && (!data_ptr || !data_size)) {
                        buf.set("0\r\n\r\n");
                    } else {
                        buf.set("\r\n");
                    }
                    rc = sendIntern(xsink, cname, mname, buf.c_str(), buf.size(), timeout_ms, total, true);
                }
            }

            if (!*xsink) {
                // do events
                switch (res->getType()) {
                    case NT_STRING: {
                        const QoreStringNode* str = res->get<const QoreStringNode>();
                        if (!str->empty()) {
                            do_data_event(QORE_EVENT_HTTP_CHUNKED_DATA_SENT, source, *str);
                        }
                        break;
                    }

                    case NT_BINARY: {
                        const BinaryNode* b = res->get<const BinaryNode>();
                        if (!b->empty()) {
                            do_data_event(QORE_EVENT_HTTP_CHUNKED_DATA_SENT, source, *b);
                        }
                        break;
                    }

                    case NT_HASH: {
                        const QoreHashNode* h = res->get<const QoreHashNode>();
                        do_header_event(QORE_EVENT_HTTP_FOOTERS_SENT, source, *h);
                        break;
                    }
                }
            }

            if (rc < 0) {
                // if we have a socket I/O error, but also data to be read on the socket, then clear the exception and return 0
                if (aborted && *xsink) {
                    bool data_available = tryReadSocketData(mname, xsink);
                    //printd(5, "qore_socket_private::sendHttpChunkedWithCallback() this: %p aborted: %p iDA: %d\n", this, aborted, data_available);
                    if (data_available) {
                        *aborted = true;
                        return *xsink ? -1 : 0;
                    }
                }

                //printd(5, "qore_socket_private::sendHttpChunkedWithCallback() this: %p rc: %d sock: %d xsink: %d\n", this, rc, sock, xsink->isException());
            }

            //printd(5, "qore_socket_private::sendHttpChunkedWithCallback() this: %p sent: %s\n", this, buf.getBuffer());

            if (rc < 0 || sock == QORE_INVALID_SOCKET)
                break;
        }

        th.finalize(total);

        return rc < 0 || sock == QORE_INVALID_SOCKET ? -1 : 0;
    }

    DLLLOCAL int sendIntern(ExceptionSink* xsink, const char* cname, const char* mname, const char* buf, qore_size_t size, int timeout_ms, int64& total, bool stream = false) {
        assert(xsink);
        qore_offset_t rc;
        qore_size_t bs = 0;

        // set the non-blocking flag (for use with non-ssl connections)
        bool nb = (timeout_ms >= 0);

        while (true) {
            if (ssl) {
                // SSL_MODE_ENABLE_PARTIAL_WRITE is enabled so we can get finer-grained socket events for do_send_event() below
                rc = ssl->write(mname, buf + bs, size - bs, timeout_ms, xsink);
            } else {
                while (true) {
                    rc = ::send(sock, buf + bs, size - bs, 0);
                    //printd(5, "qore_socket_private::send() this: %p Socket::%s() buf: %p size: " QLLD " timeout_ms: %d ssl: %p nb: %d bs: " QLLD " rc: " QLLD "\n", this, mname, buf, size, timeout_ms, ssl, nb, bs, rc);
                    // try again if we were interrupted by a signal
                    if (rc >= 0)
                        break;
                    sock_get_error();
                    // check that the send finishes before the timeout if we are using non-blocking I/O
                    if (nb && (errno == EAGAIN
#ifdef EWOULDBLOCK
                        || errno == EWOULDBLOCK
#endif
                        )) {
                        if (!isWriteFinished(timeout_ms, mname, xsink)) {
                            if (*xsink)
                                return -1;
                            se_timeout("Socket", mname, timeout_ms, xsink);
                            rc = QSE_TIMEOUT;
                            break;
                        }
                        continue;
                    }
                    if (errno != EINTR) {
                        //printd(5, "qore_socket_private::send() bs: %ld rc: " QSD " len: " QSD " (total: " QSD ") errno: %d sock: %d\n", bs, rc, size - bs, size, errno, sock);
                        xsink->raiseErrnoException("SOCKET-SEND-ERROR", errno, "error while executing %s::%s()", cname, mname);

                        // do not close the socket even if we have EPIPE or ECONNRESET in case there is data to be read when streaming
#ifdef EPIPE
                        if (!stream && errno == EPIPE)
                            close();
#endif
#ifdef ECONNRESET
                        if (!stream && errno == ECONNRESET)
                            close();
#endif
                        break;
                    }
                }
            }

            total += rc;

            //printd(5, "qore_socket_private::send() bs: %ld rc: " QSD " len: " QSD " (total: " QSD ") errno: %d\n", bs, rc, size - bs, size, errno);
            if (rc < 0 || sock == QORE_INVALID_SOCKET)
                break;

            bs += rc;

            do_send_event(rc, bs, size);

            if (bs >= size)
                break;
        }

        return rc;
    }

    DLLLOCAL int send(int fd, qore_offset_t size, int timeout_ms, ExceptionSink* xsink);

    DLLLOCAL int send(ExceptionSink* xsink, const char* cname, const char* mname, const char* buf, qore_size_t size, int timeout_ms = -1, int source = QORE_SOURCE_SOCKET) {
        assert(xsink);
        if (sock == QORE_INVALID_SOCKET) {
            se_not_open(cname, mname, xsink, "send");

            return QSE_NOT_OPEN;
        }
        if (in_op >= 0) {
            if (in_op == q_gettid()) {
                se_in_op(cname, mname, xsink);
                return 0;
            }
            se_in_op_thread(cname, mname, xsink);
            return 0;
        }
        if (!size) {
            return 0;
        }

        PrivateQoreSocketThroughputHelper th(this, true);

        // set the non-blocking flag (for use with non-ssl connections)
        bool nb = (timeout_ms >= 0);
        // set non-blocking I/O (and restore on exit) if we have a timeout and a non-ssl connection
        OptionalNonBlockingHelper onbh(*this, !ssl && nb, xsink);
        if (*xsink)
            return -1;

        int64 total = 0;
        qore_offset_t rc = sendIntern(xsink, cname, mname, buf, size, timeout_ms, total);
        th.finalize(total);

        if (rc > 0 && source > 0) {
            do_data_event(QORE_EVENT_SOCKET_DATA_SENT, source, buf, size);
        }

        return rc < 0 || sock == QORE_INVALID_SOCKET ? rc : 0;
    }

    DLLLOCAL void sendFromInputStream(InputStream *is, int64 size, int64 timeout, ExceptionSink *xsink, QoreThreadLock* l) {
        if (sock == QORE_INVALID_SOCKET) {
            se_not_open("Socket", "sendFromInputStream", xsink);
            return;
        }
        if (in_op >= 0) {
            if (in_op == q_gettid()) {
                se_in_op("Socket", "sendFromInputStream", xsink);
                return;
            }
            se_in_op_thread("Socket", "sendFromInputStream", xsink);
            return;
        }

        qore_socket_op_helper oh(this);

        PrivateQoreSocketThroughputHelper th(this, true);

        // set the non-blocking flag (for use with non-ssl connections)
        bool nb = (timeout >= 0);
        // set non-blocking I/O (and restore on exit) if we have a timeout and a non-ssl connection
        OptionalNonBlockingHelper onbh(*this, !ssl && nb, xsink);
        if (*xsink)
            return;

        char buf[DEFAULT_SOCKET_BUFSIZE];
        int64 sent = 0;
        int64 total = 0;
        while (size < 0 || sent < size) {
            int64 toRead = size < 0 ? DEFAULT_SOCKET_BUFSIZE : QORE_MIN(size - sent, DEFAULT_SOCKET_BUFSIZE);
            int64 r;
            {
                AutoUnlocker al(l);
                r = is->read(buf, toRead, xsink);
                if (*xsink) {
                    return;
                }
            }
            if (r == 0) {
                //eof
                if (size >= 0) {
                    //not all size bytes were sent
                    xsink->raiseException("SOCKET-SEND-ERROR", "Unexpected end of stream");
                    return;
                }
                break;
            }

            qore_offset_t rc = sendIntern(xsink, "Socket", "sendFromInputStream", buf, r, timeout, total);
            if (rc < 0) {
                return;
            }
            do_data_event(QORE_EVENT_SOCKET_DATA_SENT, QORE_SOURCE_SOCKET, buf, r);
            sent += r;
        }
        th.finalize(total);
    }

    DLLLOCAL void sendHttpChunkedBodyFromInputStream(InputStream* is, size_t max_chunk_size, int timeout, ExceptionSink* xsink, QoreThreadLock* l, const ResolvedCallReferenceNode* trailer_callback) {
        if (sock == QORE_INVALID_SOCKET) {
            se_not_open("Socket", "sendHttpChunkedBodyFromInputStream", xsink);
            return;
        }
        if (in_op >= 0) {
            if (in_op == q_gettid()) {
                se_in_op("Socket", "sendHttpChunkedBodyFromInputStream", xsink);
                return;
            }
            se_in_op_thread("Socket", "sendHttpChunkedBodyFromInputStream", xsink);
            return;
        }

        qore_socket_op_helper oh(this);

        PrivateQoreSocketThroughputHelper th(this, true);

        // set the non-blocking flag (for use with non-ssl connections)
        bool nb = (timeout >= 0);
        // set non-blocking I/O (and restore on exit) if we have a timeout and a non-ssl connection
        OptionalNonBlockingHelper onbh(*this, !ssl && nb, xsink);
        if (*xsink)
            return;

        SimpleRefHolder<BinaryNode> buf(new BinaryNode);
        // reserve enough space for the maximum size of the buffer + HTTP overhead
        buf->preallocate(max_chunk_size);
        int64 total = 0;
        while (true) {
            int64 r;
            {
                AutoUnlocker al(l);
                r = is->read((void*)buf->getPtr(), sizeof(max_chunk_size), xsink);
                if (*xsink)
                    return;
            }

            // send HTTP chunk prelude with chunk size
            QoreString str;
            str.sprintf("%x\r\n", (int)r);
            int rc = sendIntern(xsink, "Socket", "sendHttpChunkedBodyFromInputStream", str.c_str(), str.size(), timeout, total, true);
            if (rc < 0)
                return;

            bool trailers = false;

            // send chunk data, if any
            if (r) {
                rc = sendIntern(xsink, "Socket", "sendHttpChunkedBodyFromInputStream", (const char*)buf->getPtr(), r, timeout, total, true);
                if (rc < 0)
                    return;
                do_data_event(QORE_EVENT_HTTP_CHUNKED_DATA_SENT, QORE_SOURCE_SOCKET, buf->getPtr(), r);
            } else if (trailer_callback) {
                // get and send chunk trailers, if any
                ReferenceHolder<QoreHashNode> h(xsink);

                if (runTrailerCallback(xsink, "Socket", "sendHttpChunkedBodyFromInputStream", *trailer_callback, l, h))
                    return;
                if (h) {
                    str.clear();
                    do_headers(str, *h, 0, false);

                    rc = sendIntern(xsink, "Socket", "sendHttpChunkedBodyFromInputStream", str.c_str(), str.size(), timeout, total, true);
                    if (rc < 0)
                        return;

                    do_header_event(QORE_EVENT_HTTP_FOOTERS_SENT, QORE_SOURCE_SOCKET, **h);
                    trailers = true;
                }
            }

            // close chunk if we sent no trailers
            if (!trailers) {
                str.set("\r\n");
                rc = sendIntern(xsink, "Socket", "sendHttpChunkedBodyFromInputStream", str.c_str(), str.size(), timeout, total, true);
                if (rc < 0)
                    return;
            }

            if (!r) {
                // end of stream
                break;
            }
        }
        th.finalize(total);
    }

    DLLLOCAL void sendHttpChunkedBodyTrailer(const QoreHashNode* headers, int timeout, ExceptionSink* xsink) {
        if (sock == QORE_INVALID_SOCKET) {
            se_not_open("Socket", "sendHttpChunkedBodyTrailer", xsink);
            return;
        }
        if (in_op >= 0) {
            if (in_op == q_gettid()) {
                se_in_op("Socket", "sendHttpChunkedBodyTrailer", xsink);
                return;
            }
            se_in_op_thread("Socket", "sendHttpChunkedBodyTrailer", xsink);
            return;
        }

        QoreString buf;
        if (!headers) {
            ConstHashIterator hi(headers);

            while (hi.next()) {
                const QoreValue v = hi.get();
                const char* key = hi.getKey();

                if (v.getType() == NT_LIST) {
                    ConstListIterator li(v.get<const QoreListNode>());
                    while (li.next())
                        do_header(key, buf, li.getValue());
                }
                else
                do_header(key, buf, v);
            }
        }
        buf.concat("\r\n");
        int64 total;
        sendIntern(xsink, "Socket", "sendHttpChunkedBodyTrailer", buf.getBuffer(), buf.size(), timeout, total, true);
        if (!*xsink) {
            do_header_event(QORE_EVENT_HTTP_FOOTERS_SENT, QORE_SOURCE_SOCKET, *headers);
        }
    }

    DLLLOCAL int sendHttpMessage(ExceptionSink* xsink, QoreHashNode* info, const char* cname, const char* mname,
        const char* method, const char* path, const char* http_version, const QoreHashNode* headers,
        const QoreStringNode* body, const void *data, qore_size_t size,
        const ResolvedCallReferenceNode* send_callback, InputStream* input_stream, size_t max_chunk_size,
        const ResolvedCallReferenceNode* trailer_callback, int source, int timeout_ms = -1,
        QoreThreadLock* l = nullptr, bool* aborted = nullptr) {
        // prepare header string
        QoreString hdr(enc);

        hdr.sprintf("%s %s HTTP/%s", method, path && path[0] ? path : "/", http_version);

        // write request-uri key if info hash is non-null
        if (info) {
            info->setKeyValue("request-uri", new QoreStringNode(hdr), nullptr);
        }

        return sendHttpMessageCommon(xsink, hdr, info, cname, mname, headers, body, data, size, send_callback,
            input_stream, max_chunk_size, trailer_callback, source, timeout_ms, l, aborted);
    }

    DLLLOCAL int sendHttpResponse(ExceptionSink* xsink, QoreHashNode* info, const char* cname, const char* mname,
        int code, const char* desc, const char* http_version, const QoreHashNode* headers, const QoreStringNode* body,
        const void *data, qore_size_t size, const ResolvedCallReferenceNode* send_callback, InputStream* input_stream,
        size_t max_chunk_size, const ResolvedCallReferenceNode* trailer_callback, int source, int timeout_ms = -1,
        QoreThreadLock* l = nullptr, bool* aborted = nullptr) {
        // prepare header string
        QoreString hdr(enc);

        // write HTTP response status line
        hdr.sprintf("HTTP/%s %03d %s", http_version, code, desc);

        // write the status line as the "response-uri" key if info hash is non-null
        // NOTE: this is not a URI, so the name is not really appropriate
        if (info) {
            info->setKeyValue("response-uri", new QoreStringNode(hdr), nullptr);
        }

        return sendHttpMessageCommon(xsink, hdr, info, cname, mname, headers, body, data, size, send_callback,
            input_stream, max_chunk_size, trailer_callback, source, timeout_ms, l, aborted);
    }

    DLLLOCAL int sendHttpMessageCommon(ExceptionSink* xsink, QoreString& hdr, QoreHashNode* info, const char* cname,
        const char* mname, const QoreHashNode* headers, const QoreStringNode* body, const void *data,
        qore_size_t size, const ResolvedCallReferenceNode* send_callback, InputStream* input_stream,
        size_t max_chunk_size, const ResolvedCallReferenceNode* trailer_callback, int source, int timeout_ms = -1,
        QoreThreadLock* l = nullptr, bool* aborted = nullptr) {
        assert(xsink);
        assert(!(data && send_callback));
        assert(!(data && input_stream));
        assert(!(send_callback && input_stream));

        // send event
        do_send_http_message_event(hdr, headers, source);

        // add headers
        hdr.concat("\r\n");
        // insert headers
        do_headers(hdr, headers, size && data ? size : 0);

        //printd(5, "qore_socket_private::sendHttpMessage() hdr: %s\n", hdr.c_str());

        // send URI and headers
        int rc;
        if ((rc = send(xsink, cname, mname, hdr.c_str(), hdr.size(), timeout_ms, -1)))
            return rc;

        // header message sent above with do_sent_http_message_event()
        if (size && data) {
            int rc = send(xsink, cname, mname, (char*)data, size, timeout_ms, -1);
            if (!rc) {
                if (body) {
                    do_data_event(QORE_EVENT_SOCKET_DATA_SENT, source, *body);
                } else {
                    do_data_event(QORE_EVENT_SOCKET_DATA_SENT, source, data, size);
                }
            }
            return rc;
        } else if (send_callback) {
            assert(l);
            assert(!aborted || !(*aborted));
            return sendHttpChunkedWithCallback(xsink, cname, mname, *send_callback, *l, source, timeout_ms, aborted);
        } else if (input_stream) {
            assert(l);
            sendHttpChunkedBodyFromInputStream(input_stream, max_chunk_size, timeout_ms, xsink, l, trailer_callback);
            return *xsink ? -1 : 0;
        }

        return 0;
    }

    DLLLOCAL QoreHashNode* readHttpChunkedBodyBinary(int timeout, ExceptionSink* xsink, const char* cname, int source, const ResolvedCallReferenceNode* recv_callback = nullptr, QoreThreadLock* l = nullptr, QoreObject* obj = nullptr, OutputStream* os = nullptr) {
        assert(xsink);

        if (sock == QORE_INVALID_SOCKET) {
            se_not_open(cname, "readHTTPChunkedBodyBinary", xsink);
            return 0;
        }
        if (in_op >= 0) {
            if (in_op == q_gettid()) {
                se_in_op(cname, "readHTTPChunkedBodyBinary", xsink);
                return 0;
            }
            se_in_op_thread(cname, "readHTTPChunkedBodyBinary", xsink);
            return 0;
        }

        // reset "expecting HTTP chunked body" flag
        if (http_exp_chunked_body)
            http_exp_chunked_body = false;

        qore_socket_op_helper oh(this);

        SimpleRefHolder<BinaryNode> b(os ? nullptr : new BinaryNode);
        QoreString str; // for reading the size of each chunk

        qore_offset_t rc;
        // read the size then read the data and append to buffer
        while (true) {
            // state = 0, nothing
            // state = 1, \r received
            int state = 0;
            while (true) {
                char* buf;
                rc = brecv(xsink, "readHTTPChunkedBodyBinary", buf, 1, 0, timeout, false);
                if (rc <= 0) {
                    if (!*xsink) {
                        assert(!rc);
                        se_closed(cname, "readHTTPChunkedBodyBinary", xsink);
                    }
                    return 0;
                }

                char c = buf[0];

                if (!state && c == '\r')
                    state = 1;
                else if (state && c == '\n')
                    break;
                else {
                    if (state) {
                        state = 0;
                        str.concat('\r');
                    }
                    str.concat(c);
                }
            }
            // DEBUG
            //printd(5, "QoreSocket::readHTTPChunkedBodyBinary(): got chunk size (" QSD " bytes) string: %s\n", str.strlen(), str.getBuffer());

            // terminate string at ';' char if present
            char* p = (char*)strchr(str.getBuffer(), ';');
            if (p)
                *p = '\0';
            long size = strtol(str.c_str(), 0, 16);
            do_chunked_read(QORE_EVENT_HTTP_CHUNK_SIZE, size, str.size(), source);

            if (!size)
                break;

            if (size < 0) {
                xsink->raiseException("READ-HTTP-CHUNK-ERROR", "negative value given for chunk size (%ld)", size);
                return 0;
            }

            // prepare string for chunk
            //str.allocate(size + 1);

            qore_offset_t bs = size < DEFAULT_SOCKET_BUFSIZE ? size : DEFAULT_SOCKET_BUFSIZE;
            qore_offset_t br = 0; // bytes received
            while (true) {
                char* buf;
                rc = brecv(xsink, "readHTTPChunkedBodyBinary", buf, bs, 0, timeout, false);
                //printd(5, "qore_socket_private::readHTTPChunkedBodyBinary() str: '%s' bs: %lld rc: %lld b: %p (%lld) recv_callback: %p\n", str.c_str(), bs, rc, *b, b->size(), recv_callback);
                if (rc <= 0) {
                    if (!*xsink) {
                        assert(!rc);
                        se_closed(cname, "readHTTPChunkedBodyBinary", xsink);
                    }
                    return nullptr;
                }

                do_data_event(QORE_EVENT_HTTP_CHUNKED_DATA_READ, source, buf, (size_t)rc);

                if (os) {
                    AutoUnlocker al(l);
                    os->write(buf, rc, xsink);
                    if (*xsink)
                        return nullptr;
                } else {
                    b->append(buf, rc);
                }
                br += rc;

                if (br >= size)
                    break;
                if (size - br < bs)
                    bs = size - br;
            }

            // DEBUG
            //printd(5, "QoreSocket::readHTTPChunkedBodyBinary(): received binary chunk: size: %d br=" QSD " total=" QSD "\n", size, br, b->size());

            // read crlf after chunk
            // FIXME: bytes read are not checked if they equal CRLF
            br = 0;
            while (br < 2) {
                char* buf;
                rc = brecv(xsink, "readHTTPChunkedBodyBinary", buf, 2 - br, 0, timeout, false);
                if (rc <= 0) {
                    if (!*xsink) {
                        assert(!rc);
                        se_closed(cname, "readHTTPChunkedBodyBinary", xsink);
                    }
                    return nullptr;
                }
                br += rc;
            }

            do_chunked_read(QORE_EVENT_HTTP_CHUNKED_DATA_RECEIVED, size, size + 2, source);

            if (recv_callback && !os) {
                if (runDataCallback(xsink, cname, "readHTTPChunkedBodyBinary", *recv_callback, l, *b, true))
                    return nullptr;
                if (b)
                    b->clear();
            }

            // ensure string is blanked for next read
            str.clear();
        }

        // read footers or nothing
        QoreStringNodeHolder hdr(readHTTPData(xsink, "readHTTPChunkedBodyBinary", timeout, rc, true));
        if (*xsink)
            return nullptr;

        ReferenceHolder<QoreHashNode> h(new QoreHashNode(autoTypeInfo), xsink);
        if (!recv_callback && !os) {
            h->setKeyValue("body", b.release(), xsink);
        }

        ReferenceHolder<QoreHashNode> info(xsink);

        if (hdr) {
            if (hdr->strlen() >= 2 && hdr->strlen() <= 4)
                return recv_callback ? 0 : h.release();

            if (recv_callback) {
                info = new QoreHashNode(autoTypeInfo);
            }
            convertHeaderToHash(*h, (char*)hdr->c_str(), 0, *info, nullptr, "response-headers-raw");
            do_read_http_header(QORE_EVENT_HTTP_FOOTERS_RECEIVED, *h, source);
        }

        if (recv_callback) {
            runHeaderCallback(xsink, cname, "readHTTPChunkedBodyBinary", *recv_callback, l, h->empty() ? nullptr : *h,
                info.release(), false, obj);
            return 0;
        }

        return h.release();
    }

    // receive a message in HTTP chunked format
    DLLLOCAL QoreHashNode* readHttpChunkedBody(int timeout, ExceptionSink* xsink, const char* cname, int source, const ResolvedCallReferenceNode* recv_callback = 0, QoreThreadLock* l = 0, QoreObject* obj = 0) {
        assert(xsink);

        if (sock == QORE_INVALID_SOCKET) {
            se_not_open(cname, "readHTTPChunkedBody", xsink);
            return 0;
        }
        if (in_op >= 0) {
            if (in_op == q_gettid()) {
                se_in_op(cname, "readHTTPChunkedBody", xsink);
                return 0;
            }
            se_in_op_thread(cname, "readHTTPChunkedBody", xsink);
            return 0;
        }

        // reset "expecting HTTP chunked body" flag
        if (http_exp_chunked_body)
            http_exp_chunked_body = false;

        qore_socket_op_helper oh(this);

        QoreStringNodeHolder buf(new QoreStringNode(enc));
        QoreString str; // for reading the size of each chunk

        qore_offset_t rc;
        // read the size then read the data and append to buf
        while (true) {
            // state = 0, nothing
            // state = 1, \r received
            int state = 0;
            while (true) {
                char* tbuf;
                rc = brecv(xsink, "readHTTPChunkedBody", tbuf, 1, 0, timeout, false);
                if (rc <= 0) {
                    if (!*xsink) {
                        assert(!rc);
                        se_closed(cname, "readHTTPChunkedBody", xsink);
                    }
                    return 0;
                }

                char c = tbuf[0];

                if (!state && c == '\r')
                    state = 1;
                else if (state && c == '\n')
                    break;
                else {
                    if (state) {
                        state = 0;
                        str.concat('\r');
                    }
                    str.concat(c);
                }
            }
            // DEBUG
            //printd(5, "got chunk size (" QSD " bytes) string: %s\n", str.strlen(), str.getBuffer());

            // terminate string at ';' char if present
            char* p = (char*)strchr(str.getBuffer(), ';');
            if (p)
                *p = '\0';
            qore_offset_t size = strtol(str.getBuffer(), 0, 16);
            do_chunked_read(QORE_EVENT_HTTP_CHUNK_SIZE, size, str.strlen(), source);

            if (!size)
                break;

            if (size < 0) {
                xsink->raiseException("READ-HTTP-CHUNK-ERROR", "negative value given for chunk size (%ld)", size);
                return 0;
            }
            // ensure string is blanked for next read
            str.clear();

            // prepare string for chunk
            //buf->allocate((unsigned)(buf->strlen() + size + 1));

            // read chunk directly into string buffer
            qore_offset_t bs = size < DEFAULT_SOCKET_BUFSIZE ? size : DEFAULT_SOCKET_BUFSIZE;
            qore_offset_t br = 0; // bytes received
            str.clear();
            while (true) {
                char* tbuf;
                rc = brecv(xsink, "readHTTPChunkedBody", tbuf, bs, 0, timeout, false);
                if (rc <= 0) {
                    if (!*xsink) {
                        assert(!rc);
                        se_closed(cname, "readHTTPChunkedBody", xsink);
                    }
                    return 0;
                }

                br += rc;
                buf->concat(tbuf, rc);

                do_data_event(QORE_EVENT_HTTP_CHUNKED_DATA_READ, source, tbuf, (size_t)rc);

                if (br >= size)
                    break;
                if (size - br < bs)
                    bs = size - br;
            }

            // DEBUG
            //printd(5, "got chunk (" QSD " bytes): %s\n", br, buf->getBuffer() + buf->strlen() -  size);

            // read crlf after chunk
            // FIXME: bytes read are not checked if they equal CRLF
            br = 0;
            while (br < 2) {
                char* tbuf;
                rc = brecv(xsink, "readHTTPChunkedBody", tbuf, 2 - br, 0, timeout, false);
                if (rc <= 0) {
                    if (!*xsink) {
                        assert(!rc);
                        se_closed(cname, "readHTTPChunkedBody", xsink);
                    }
                    return nullptr;
                }
                br += rc;
            }

            do_chunked_read(QORE_EVENT_HTTP_CHUNKED_DATA_RECEIVED, size, size + 2, source);

            if (recv_callback) {
                if (runDataCallback(xsink, cname, "readHTTPChunkedBody", *recv_callback, l, *buf, true))
                    return nullptr;
                buf->clear();
            }
        }

        // read footers or nothing
        QoreStringNodeHolder hdr(readHTTPData(xsink, "readHTTPChunkedBody", timeout, rc, true));
        if (*xsink)
            return nullptr;

        //printd(5, "chunked body encoding: %s\n", buf->getEncoding()->getCode());
        ReferenceHolder<QoreHashNode> h(new QoreHashNode(autoTypeInfo), xsink);
        if (!recv_callback) {
            h->setKeyValue("body", buf.release(), xsink);
        }

        ReferenceHolder<QoreHashNode> info(xsink);

        if (hdr) {
            if (hdr->strlen() >= 2 && hdr->strlen() <= 4)
                return recv_callback ? 0 : h.release();

            if (recv_callback) {
                info = new QoreHashNode(autoTypeInfo);
            }
            convertHeaderToHash(*h, (char*)hdr->c_str(), 0, *info, nullptr, "response-headers-raw");
            do_read_http_header(QORE_EVENT_HTTP_FOOTERS_RECEIVED, *h, source);
        }

        if (recv_callback) {
            runHeaderCallback(xsink, cname, "readHTTPChunkedBody", *recv_callback, l, h->empty() ? nullptr : *h,
                info.release(), false, obj);
            return 0;
        }

        return h.release();
    }

    DLLLOCAL static void do_accept_encoding(char* t, QoreHashNode& info) {
        ReferenceHolder<QoreListNode> l(new QoreListNode(autoTypeInfo), 0);

        char* a = t;
        bool ok = true;
        while (*a) {
            if (ok) {
                ok = false;
                SimpleRefHolder<QoreStringNode> str(new QoreStringNode);
                while (*a && *a != ';' && *a != ',')
                    str->concat(*(a++));
                str->trim();
                if (!str->empty())
                    l->push(str.release(), nullptr);
                continue;
            }
            else if (*a == ',')
                ok = true;

            ++a;
        }

        if (!l->empty())
            info.setKeyValue("accept-encoding", l.release(), 0);
    }

    DLLLOCAL bool do_accept_charset(char* t, QoreHashNode& info) {
        bool acceptcharset = false;

        // see if we have "*" or utf8 or utf-8, in which case set it
        // otherwise set the first charset in the list
        char* a = t;
        char* div = 0;
        bool utf8 = false;
        bool ok = true;
        while (*a) {
            if (ok) {
                if (*a == '*') {
                    utf8 = true;
                    break;
                }
                ok = false;
                if (*a == 'u' || *a == 'U') {
                    ++a;
                    if (*a == 't' || *a == 'T') {
                        ++a;
                        if (*a == 'f' || *a == 'F') {
                            ++a;
                            if (*a == '-')
                                ++a;
                            if (*a == '8') {
                                utf8 = true;
                                break;
                            }
                        }
                    }
                    continue;
                }
            } else if (*a == ',') {
                if (!div)
                    div = a;
                ok = true;
            } else if (*a == ';') {
                if (!div)
                    div = a;
            }

            ++a;
        }
        if (utf8) {
            info.setKeyValue("accept-charset", new QoreStringNode("utf8"), 0);
            acceptcharset = true;
        } else {
            SimpleRefHolder<QoreStringNode> ac(new QoreStringNode);
            if (div)
                ac->concat(t, div - t);
            else
                ac->concat(t);
            ac->trim();
            if (!ac->empty()) {
                info.setKeyValue("accept-charset", ac.release(), 0);
                acceptcharset = true;
            }
        }

        return acceptcharset;
    }

    // returns true if the connection should be closed, false if not
    DLLLOCAL bool convertHeaderToHash(QoreHashNode* h, char* p, int flags = 0, QoreHashNode* info = nullptr,
        bool* chunked = nullptr, const char* headers_raw_key = "headers-raw") {
        bool close = !(flags & CHF_HTTP11);
        // socket encoding
        const char* senc = nullptr;
        // accept-charset
        bool acceptcharset = false;

        QoreHashNode* raw_hdr = nullptr;
        if (info) {
            info->setKeyValue(headers_raw_key, raw_hdr = new QoreHashNode(autoTypeInfo), nullptr);
        }

        // raw key for setting raw headers
        std::string raw_key;

        while (*p) {
            char* buf = p;

            if ((p = strstr(buf, "\r\n"))) {
                *p = '\0';
                p += 2;
            } else if ((p = strchr(buf, '\n'))) {
                *p = '\0';
                p++;
            } else if ((p = strchr(buf, '\r'))) {
                *p = '\0';
                p++;
            } else
                break;
            char* t = strchr(buf, ':');
            if (!t)
                break;
            *t = '\0';
            t++;
            while (t && qore_isblank(*t))
                t++;
            if (raw_hdr) {
                raw_key = buf;
            }
            strtolower(buf);
            //printd(5, "setting %s = '%s'\n", buf, t);

            ReferenceHolder<> val(new QoreStringNode(t), nullptr);

            if (flags & CHF_PROCESS) {
                if (!strcmp(buf, "connection")) {
                    if (flags & CHF_HTTP11) {
                        if (strcasestr(t, "close"))
                            close = true;
                    } else {
                        if (strcasestr(t, "keep-alive"))
                            close = false;
                    }
                } else if (!strcmp(buf, "content-type")) {
                    char* a = strcasestr(t, "charset=");
                    if (a) {
                        // find end
                        char* e = strchr(a + 8, ';');

                        QoreString cs;
                        if (e)
                            cs.concat(a + 8, e - a - 8);
                        else
                            cs.concat(a + 8);
                        cs.trim();
                        senc = cs.getBuffer();
                        //printd(5, "got encoding '%s' from request\n", senc);
                        enc = QEM.findCreate(senc);

                        if (info) {
                            qore_size_t len = cs.size();
                            info->setKeyValue("charset", new QoreStringNode(cs.giveBuffer(), len, len + 1, QCS_DEFAULT), nullptr);
                        }

                        if (info) {
                            SimpleRefHolder<QoreStringNode> ct(new QoreStringNode);
                            // remove any whitespace and ';' before charset=
                            if (a != t) {
                                do {
                                    --a;
                                } while (a > t && (*a == ' ' || *a == ';'));
                            }

                            if (a == t) {
                                if (e)
                                    ct->concat(e + 1);
                            } else {
                                ct->concat(t, a - t + 1);
                                if (e)
                                    ct->concat(e);
                            }
                            ct->trim();
                            if (!ct->empty())
                                info->setKeyValue("body-content-type", ct.release(), nullptr);
                        }
                    } else {
                        enc = QEM.findCreate(assume_http_encoding.c_str());
                        if (info) {
                            info->setKeyValue("charset", new QoreStringNode(assume_http_encoding), nullptr);
                            info->setKeyValue("body-content-type", val->refSelf(), nullptr);
                        }
                    }
                } else if (chunked && !strcmp(buf, "transfer-encoding") && !strcasecmp(t, "chunked")) {
                    *chunked = true;
                } else if (info) {
                    if (!strcmp(buf, "accept-charset"))
                        acceptcharset = do_accept_charset(t, *info);
                    else if ((flags & CHF_REQUEST) && !strcmp(buf, "accept-encoding"))
                        do_accept_encoding(t, *info);
                }
            }

            ReferenceHolder<> val_copy(nullptr);
            if (raw_hdr && val) {
                val_copy = val->realCopy();
            }

            // see if header exists, and if so make it a list and add value to the list
            hash_assignment_priv ha(*h, buf);
            if (!(*ha).isNothing()) {
                QoreListNode* l;
                if ((*ha).getType() == NT_LIST) {
                    l = (*ha).get<QoreListNode>();
                } else {
                    l = new QoreListNode(autoTypeInfo);
                    l->push(ha.swap(l), nullptr);
                }
                l->push(val.release(), nullptr);
            } else // otherwise set header normally
                ha.assign(val.release(), 0);

            // set raw headers if applicable
            if (raw_hdr) {
                hash_assignment_priv ha(*raw_hdr, raw_key);
                if (!(*ha).isNothing()) {
                    QoreListNode* l;
                    if ((*ha).getType() == NT_LIST) {
                        l = (*ha).get<QoreListNode>();
                    } else {
                        l = new QoreListNode(autoTypeInfo);
                        l->push(ha.swap(l), nullptr);
                    }
                    l->push(val_copy.release(), nullptr);
                } else // otherwise set header normally
                    ha.assign(val_copy.release(), nullptr);
            }
        }

        if ((flags & CHF_PROCESS)) {
            if (!senc)
                enc = QEM.findCreate(assume_http_encoding.c_str());
            // according to RFC-2616 section 14.2, "If no Accept-Charset header is present, the default is that any character set is acceptable" so we will use utf-8
            if (info && !acceptcharset)
                info->setKeyValue("accept-charset", new QoreStringNode("utf8"), nullptr);
        }

        return close;
    }

    DLLLOCAL int recvix(const char* meth, int len, void* targ, int timeout_ms, ExceptionSink* xsink) {
        assert(xsink);
        if (sock == QORE_INVALID_SOCKET) {
            se_not_open("Socket", meth, xsink, "recvix");
            return QSE_NOT_OPEN;
        }
        if (in_op >= 0) {
            if (in_op == q_gettid()) {
                se_in_op("Socket", meth, xsink);
                return 0;
            }
            se_in_op_thread("Socket", meth, xsink);
            return 0;
        }

        PrivateQoreSocketThroughputHelper th(this, false);

        char* buf;
        qore_offset_t br = 0;
        while (true) {
            qore_offset_t rc = brecv(xsink, meth, buf, len - br, 0, timeout_ms);
            if (rc <= 0) {
                do_read_error(rc, meth, timeout_ms, xsink);
                return (int)rc;
            }

            memcpy(targ, buf, rc);

            br += rc;
            if (br >= len)
                break;
        }

        th.finalize(br);
        do_data_event(QORE_EVENT_SOCKET_DATA_READ, QORE_SOURCE_SOCKET, targ, br);
        return (int)br;
    }

    DLLLOCAL void clearWarningQueue(ExceptionSink* xsink) {
        if (warn_queue) {
            if (warn_callback_arg) {
                warn_callback_arg.discard(xsink);
                warn_callback_arg = QoreValue();
            }
            warn_queue->deref(xsink);
            warn_queue = nullptr;
            tl_warning_us = 0;
            tp_warning_bs = 0.0;
            tp_us_min = 0;
        }
    }

    DLLLOCAL void setWarningQueue(ExceptionSink* xsink, int64 warning_ms, int64 warning_bs, Queue* wq, QoreValue arg, int64 min_ms = 1000) {
        ReferenceHolder<Queue> qholder(wq, xsink);
        ValueHolder holder(arg, xsink);
        if (warning_ms <= 0 && warning_bs <= 0) {
            xsink->raiseException("SOCKET-SETWARNINGQUEUE-ERROR", "Socket::setWarningQueue() at least one of warning ms argument: " QLLD " and warning B/s argument: " QLLD " must be greater than zero; to clear, call Socket::clearWarningQueue() with no arguments", warning_ms, warning_bs);
            return;
        }

        if (warning_ms < 0)
            warning_ms = 0;
        if (warning_bs < 0)
            warning_bs = 0;

        if (warn_queue) {
            warn_queue->deref(xsink);
            warn_callback_arg.discard(xsink);
        }

        warn_queue = qholder.release();
        warn_callback_arg = holder.release();
        tl_warning_us = (int64)warning_ms * 1000;
        tp_warning_bs = warning_bs;
        tp_us_min = min_ms * 1000;
    }

    DLLLOCAL void getUsageInfo(QoreHashNode& h, qore_socket_private& s) const {
        if (warn_queue) {
            h.setKeyValue("arg", warn_callback_arg.refSelf(), 0);
            h.setKeyValue("timeout", tl_warning_us, 0);
            h.setKeyValue("min_throughput", (int64)tp_warning_bs, 0);
            h.setKeyValue("min_throughput_us", (int64)tp_us_min, 0);
        }

        h.setKeyValue("bytes_sent", tp_bytes_sent + s.tp_bytes_sent, 0);
        h.setKeyValue("bytes_recv", tp_bytes_recv + s.tp_bytes_sent, 0);
        h.setKeyValue("us_sent", tp_us_sent + s.tp_us_sent, 0);
        h.setKeyValue("us_recv", tp_us_recv + s.tp_us_recv, 0);
    }

    DLLLOCAL void getUsageInfo(QoreHashNode& h) const {
        if (warn_queue) {
            h.setKeyValue("arg", warn_callback_arg.refSelf(), 0);
            h.setKeyValue("timeout", tl_warning_us, 0);
            h.setKeyValue("min_throughput", (int64)tp_warning_bs, 0);
            h.setKeyValue("min_throughput_us", (int64)tp_us_min, 0);
        }

        h.setKeyValue("bytes_sent", tp_bytes_sent, 0);
        h.setKeyValue("bytes_recv", tp_bytes_recv, 0);
        h.setKeyValue("us_sent", tp_us_sent, 0);
        h.setKeyValue("us_recv", tp_us_recv, 0);
    }

    DLLLOCAL QoreHashNode* getUsageInfo() const {
        QoreHashNode* h = new QoreHashNode(autoTypeInfo);
        getUsageInfo(*h);
        return h;
    }

    DLLLOCAL void clearStats() {
        tp_bytes_sent = 0;
        tp_bytes_recv = 0;
        tp_us_sent = 0;
        tp_us_recv = 0;
    }

    DLLLOCAL void doTimeoutWarning(const char* op, int64 dt) {
        assert(warn_queue);
        assert(dt > tl_warning_us);

        QoreHashNode* h = new QoreHashNode(autoTypeInfo);

        h->setKeyValue("type", new QoreStringNode("SOCKET-OPERATION-WARNING"), 0);
        h->setKeyValue("operation", new QoreStringNode(op), 0);
        h->setKeyValue("us", dt, 0);
        h->setKeyValue("timeout", tl_warning_us, 0);
        if (warn_callback_arg)
            h->setKeyValue("arg", warn_callback_arg.refSelf(), 0);

        warn_queue->pushAndTakeRef(h);
    }

    DLLLOCAL void doThroughputWarning(bool send, int64 bytes, int64 dt, double bs) {
        assert(warn_queue);
        assert(bs < tp_warning_bs);

        QoreHashNode* h = new QoreHashNode(autoTypeInfo);

        h->setKeyValue("type", new QoreStringNode("SOCKET-THROUGHPUT-WARNING"), 0);
        h->setKeyValue("dir", new QoreStringNode(send ? "send" : "recv"), 0);
        h->setKeyValue("bytes", bytes, 0);
        h->setKeyValue("us", dt, 0);
        h->setKeyValue("bytes_sec", bs, 0);
        h->setKeyValue("threshold", (int64)tp_warning_bs, 0);
        if (warn_callback_arg)
            h->setKeyValue("arg", warn_callback_arg.refSelf(), 0);

        warn_queue->pushAndTakeRef(h);
    }

    DLLLOCAL bool pendingHttpChunkedBody() const {
        return http_exp_chunked_body && sock != QORE_INVALID_SOCKET;
    }

    DLLLOCAL void setSslVerifyMode(int mode) {
        //printd(5, "qore_socket_private::setSslVerifyMode() this: %p mode: %d\n", this, mode);
        ssl_verify_mode = mode;
        if (ssl)
            ssl->setVerifyMode(ssl_verify_mode, ssl_accept_all_certs, client_target);
    }

    DLLLOCAL void acceptAllCertificates(bool accept_all = true) {
        ssl_accept_all_certs = accept_all;
        if (ssl)
            ssl->setVerifyMode(ssl_verify_mode, ssl_accept_all_certs, client_target);
    }

    DLLLOCAL void setSslErrorString(QoreStringNode* err_str) {
        if (ssl_err_str) {
            ssl_err_str->concat("; ");
            ssl_err_str->concat(err_str);
            err_str->deref();
        } else {
            ssl_err_str = err_str;
        }
    }

    DLLLOCAL static void getUsageInfo(const QoreSocket& sock, QoreHashNode& h, const QoreSocket& s) {
        sock.priv->getUsageInfo(h, *s.priv);
    }

    DLLLOCAL static qore_socket_private* get(QoreSocket& sock) {
        return sock.priv;
    }

    DLLLOCAL static const qore_socket_private* get(const QoreSocket& sock) {
        return sock.priv;
    }

    DLLLOCAL static void captureRemoteCert(X509_STORE_CTX* x509_ctx);

    DLLLOCAL static QoreListNode* poll(const QoreListNode* poll_list, int timeout_ms, ExceptionSink* xsink);
};

#endif
