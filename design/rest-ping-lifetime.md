# REST polling operation lifetime

<!-- Copyright 2026 Qore Technologies, s.r.o. -->

`RestConnection::startPollConnect()` returns an operation that may outlive the caller's
reference to the connection. Both `RestPingPollOperation` and
`RestClientIoPingPollOperation` retain that connection with a strong reference for their
entire lifetime. Response validation, authentication state changes, and OAuth2 refresh
still need the connection after construction, including on HTTP errors.

The connection does not own its returned operation, so this reference does not introduce
an ownership cycle. A connection monitor owns its connection and operation separately;
it must abort and dispose of the operation when removing its polling state. Aborting an
operation stops pending work; disposing of the operation releases its connection reference.

The Jina REST client tests exercise both implementations with a connection scoped only to
the operation factory. A destructor counter verifies retention and eventual release for
successful completion, HTTP authentication errors, and early abort. The same suite checks
malformed responses and HTTP 401, 429, and 500 responses on all supported ping transports.
