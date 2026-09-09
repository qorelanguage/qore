# HTTP/1 client connection persistence

Copyright (C) 2026 Qore Technologies, s.r.o.

The HTTP/1 client poll operation determines persistence after parsing each response,
using the HTTP version and every `Connection` field. HTTP/1.0 requires an explicit
`keep-alive` option; HTTP/1.1 persists by default. A `close` option takes precedence
for both versions. Options are case-insensitive, comma-separated tokens, including
when multiple header fields are received. Prefixes such as `xclose` do not match.
This follows [RFC 9112 section 9.3](https://www.rfc-editor.org/rfc/rfc9112.html#section-9.3).

The existing response-dispatch path marks a nonpersistent connection closed before
resolving the response future. A subsequent request, including a redirect, therefore
acquires a new connection. The response body and status remain available to the
caller. Keeping a connection alive does not change message-body framing rules.

`examples/test/qore/classes/HTTPClient/Http1Persistence.qtest` exercises both HTTP
versions, explicit/default persistence, duplicate and comma-separated fields, token
boundaries and redirects. For nonpersistent responses, the server waits for the
client to close before accepting a replacement connection. Incorrect reuse is thus
observable without relying on the timing of a server FIN. Every socket operation
and completion event has a bounded deadline.
