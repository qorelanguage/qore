# Address resolution hints

Copyright (C) 2026 Qore Technologies, s.r.o.

Qore's socket and global address-resolution APIs accept the platform `AI_*` constants.
`QoreCaresAddrInfoResolver` translates these to the corresponding `ARES_AI_*` constants before calling
`ares_getaddrinfo()`. Their numeric values are unrelated and can differ between platforms. The original
flags remain available when formatting results, including the optional canonical name.

Numeric-only host requests are checked with `ares_inet_pton()` before any lookup: a resolvable hostname
such as `localhost` is still invalid when `AI_NUMERICHOST` is requested. The c-ares API defines the matching
flag but does not enforce this restriction itself. Numeric service requests use `ARES_AI_NUMERICSERV`.
Qore also sets this flag for every nonempty decimal service string, so binding port zero and connecting
to a numeric port do not invoke a potentially blocking system NSS service-name lookup.

The resolver retains its asynchronous callback and error-handling paths. Numeric validation failures use
the same `QOREADDRINFO-GETINFO-ERROR` exception as other resolution failures, and asynchronous callers
receive that exception through their future.

`examples/test/qore/classes/Socket/resolve-addrinfo.qtest` covers numeric restrictions through synchronous
and asynchronous APIs, valid and invalid numeric port boundaries, passive wildcard binding, canonical
numeric addresses, and rejection of synchronous resolution on the I/O worker.

The c-ares [API documentation](https://c-ares.org/docs/ares_getaddrinfo.html) describes its hint constants.
