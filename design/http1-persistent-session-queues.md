# HTTP/1 persistent session queue ownership

Copyright (C) 2026 Qore Technologies, s.r.o.

An HTTP/1 application session keeps its handler thread dedicated while the asynchronous I/O controller
routes subsequent requests to that session's queue. Ending the session removes its registration before
returning the socket to asynchronous dispatch. The client can immediately start another session on the
same connection while the earlier handler finishes its close notification and thread-local cleanup.

`HttpServer` binds its deregistration callback to the queue it registered.
`HttpAsyncSocketIoController::deregisterPersistentQueue()` compares this expected queue with the current
registration under `persistent_queue_mutex`. A repeated or delayed cleanup removes only its own queue;
it cannot remove a replacement session that happens to have the same socket key. Omitting the expected
queue retains the explicit key-only removal operation for existing callers.

The controller regression in `HttpServerAsyncIo.qtest` exercises the complete transition without sleeps:
register, remove, replace, clean up the old session again, and route a request to the replacement. It also
checks matching cleanup, repeated cleanup, and the existing key-only operation. `HttpServer.qtest` covers
the corresponding persistent-session lifecycle on an actual reused HTTP connection.
