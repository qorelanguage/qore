# Static-file caching and compression

`WebUtil::FileHandler` selects a representation before evaluating conditional requests.
The selected cache policy, ETag, Last-Modified, Expires, and Vary survive a `304` response.
An extension policy overrides the default, and a subclass header overrides both. Directory
indexes and SPA fallbacks use the resolved file's path.

Both identity and encoded responses include `Vary: Accept-Encoding` when encoding selection
can affect the response. Existing Vary fields, including `Origin` and `*`, are preserved.
Identity, precompressed, and dynamically compressed representations have separate ETags.
Dynamic tags are weak because buffering and streaming need not generate identical compressed
bytes. GET/HEAD validation uses weak comparison; If-Range requires a strong match or matching date.

Precompressed `.br`, `.zst`, `.gz`, and `.bz2` files retain preference over dynamic compression.
A sidecar older than its original file is ignored. Sidecar ETags identify the compressed file;
Last-Modified remains the original file's modification time.

Files over 1024 bytes are eligible for dynamic compression when the client accepts a supported
encoding. `max_compress_size` (10 MiB by default) bounds buffering, not compression eligibility.
Larger files use `TransformInputStream(FileInputStream, get_compressor(...))`. A zero limit
streams all eligible files; a negative constructor value is rejected. The existing transport
owns the stream and its cleanup, and dispatches non-I/O-safe streams to producer workers.
No original Content-Length is sent for a dynamically compressed stream. HTTP/1.1 uses chunked
framing, while HTTP/2 and HTTP/3 use their native DATA framing. SSE and other non-file streams
are unaffected.

Byte ranges select identity bytes and disable transport compression. Matching ordinary
preconditions are evaluated before Range. A mismatched, weak, or invalid If-Range causes a full
response, which can use compression. HEAD has the corresponding GET metadata without a body.
Empty files retain the existing 204 response with cache metadata. Missing files only fall back
when a default target is configured.

Example:

```qore
FileHandler files("/srv/site", "site", {
    "default_target": "index.html",
    "default_cache_control": "private, no-cache",
    "max_compress_size": 1024 * 1024,
});
```

Applications serving content-hashed assets should override `getResponseHeadersForFile()` based
on the resolved asset path. A `.js` extension by itself does not establish immutability.

Regression coverage is in `examples/test/qlib/WebUtil/FileHandlerCaching.qtest` and
`examples/test/qore/streams/zstd-finish.qtest`. The latter verifies that repeated EOF reads do
not restart the native Zstandard compressor and emit an endless sequence of empty frames.
