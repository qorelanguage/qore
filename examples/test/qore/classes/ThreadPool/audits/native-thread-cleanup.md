# Native external-thread cleanup audit

Copyright (C) 2026 Qore Technologies, s.r.o.

Scope: core native thread lifecycle implementation/headers, CMake native test
target, regression and README, durable TLS design, and Qore release notes.
The user authorized core prerequisites on develop with no push. Concurrent
JinaDataProvider commit `41ae16005`, concurrent AOT namespace work and astparser work are excluded. The independent HTTP fixture correction is already committed as `4f43e60f7`.

Root cause: Qore decremented its external counter and detached each worker
before pthread native TLS destruction and final thread termination. Valgrind
plus vgdb confirms a worker still in glibc `start_thread -> madvise` when the
main thread reaches libc `exit`. A native barrier regression deterministically
observes the premature zero counter on the old runtime.

Implementation: lazily start one native cleanup worker before accepting the
first external thread; transfer completed ThreadArg ownership through an
allocation-free queue; join each native thread before decrementing the external
counter. Stop and join the cleanup worker before native module unloading.
Task dispatch and ThreadPool stop/cancellation behavior are unchanged. Join
cleanup is deliberately non-cancellable because abandoning it permits module
unloading while native TLS callbacks still execute.

Design references read: `design/qore-module-structure.md`,
`design/module-sandboxing-audit-guide.md`, `design/cooperative-cancellation.md`,
and affected `design/thread-local-variable-handling.md`.

Final validation (2026-09-08):

- Debug build uses `/usr`, matching the installed executable. Targets `qore` and
  `qore-native-thread-cleanup-test` build without compiler warnings. No install
  or astparser target. Final helper symbols are hidden (`nm -D` verifies this).
- Native ordinary, empty and actual creation-failure modes pass. The deterministic
  TLS barrier fails against the old runtime and passes against this implementation.
- Six core suites pass: 78 cases / 555 assertions, including ThreadPool, async I/O,
  HTTP bounded shutdown, persistent teardown and the full HTTP suite.
- All 36 affected XML/SOAP suites pass: 482 cases. Both-version catalog survey and
  strict coverage differ only in version metadata: 293 WSDLs / 1136 messages,
  zero strict failures, 360 known broad failures with unchanged phase owners.
  Python retains the same 15 assigned later-phase failures among 92 tests.
- Native ordinary, ThreadPool, plain HTTP and SOAP consumers have zero Valgrind
  errors and zero definite/indirect/possible loss, with no suppressions. Final
  audited SOAP consumer run passes 15 cases / 479 assertions. Regex-heavy memory
  runs use the documented PCRE2 interpreter switch; functional XML runs use JIT.
- Fault injection separately exposes a host glibc 2.43-8.fc44 pthread_create TLS
  leak (704 bytes in the Qore fault test). The standalone C diagnostic and native
  allocator growth reproduce it without Qore. This independent dependency finding
  is explicitly routed to P9; no Qore counter/ownership error occurs, and no
  suppression or cache workaround is adopted. See the adjacent README.
- Valgrind emits existing DWARF-reader warnings; HTTP also emits an invalid-fd
  fstat tool warning. These remain recorded for P9; memory-error summaries are
  zero and the runs are not represented as warning-free.

Evidence: `/tmp/wsdl-p2-24-core-build-audited.log`,
`/tmp/wsdl-p2-24-core-tests-audited.log`,
`/tmp/wsdl-p2-24-native-audited-vg.log`,
`/tmp/wsdl-p2-24-vg-consumers-audited{,-test}.log`,
`/tmp/wsdl-p2-24-{affected,python,survey,coverage}.log`.
The original isolated HTTP native-termination evidence is in
`/tmp/wsdl-p2-24-http-{gdb,vg-debug}.log`.

| Check | Status | Evidence |
| --- | --- | --- |
| 1. Entry exists in `doxygen/lang/120_modules.dox.tmpl` (for modules in the Qore repo; N/A for external module repos) | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 2. Entry exists in `doxygen/lang/900_release_notes.dox.tmpl` (for modules in the Qore repo; external modules have release notes in their .qm) | Pass | Qore 3.0 release notes describe joining external workers before native TLS/module teardown. |
| 3. `qore_user_module()` or `qore_external_user_module()` call in `CMakeLists.txt` | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 4. Module added to QMOD list in `CMakeLists.txt` | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 5. `.qm` file has `@section <lowercasemodname>intro` as first doc section — **must be all lowercase** (e.g., `avrodataproviderintro`, not `AvroDataProviderintro`) | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 6. `%modern` in `.qm` file — no redundant `%new-style`, `%require-types`, `%strict-args`, `%enable-all-warnings` | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 7. No parse directives (`%requires`, `%modern`, `%new-style`) in separated `.qc` files (check OUTSIDE of `@code` blocks only) | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 8. No `%include` usage (deprecated for modules) | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 9. Copyright 2026 on all new files | Pass | Native test and README have 2026 notices; affected runtime/header notices already cover 2026. |
| 10. Directory layout: `.qm` inside `qlib/<ModuleName>/` directory (not at `qlib/<ModuleName>.qm` for multi-file modules) | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 11. No second `.qm` for the same module at `qlib/<ModuleName>.qm` | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 12. `ns=Qore::XX` matches the QoreNamespace constructor path | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 13. `%modern` directive present | N/A | New regression is a native C++ executable with a CMake target; existing Qore regression sources are unchanged. |
| 14. Executable permission set (`chmod +x`) | N/A | New regression is a native C++ executable with a CMake target; existing Qore regression sources are unchanged. |
| 15. Uses %prepend-module-path  before %requires for in-repo modules (Qore and Qore modules only; not Qorus) | N/A | New regression is a native C++ executable with a CMake target; existing Qore regression sources are unchanged. |
| 16. External module dependencies use `%try-module` — except modules delivered with the project itself (Qore ex: DataProvider, ConnectionProvider, QUnit, etc.) which use hard `%requires` | N/A | New regression is a native C++ executable with a CMake target; existing Qore regression sources are unchanged. |
| 17. No filesystem operations (fopen, open, creat, unlink, remove, rename, mkdir, rmdir, stat, chmod) without sandbox checks | Pass | No filesystem/network operation is added. One native cleanup worker executes no user code; existing worker functional domains and program/resource ownership remain intact. Linux fault tests temporarily change only their own process soft limit with RAII restoration. |
| 18. No network operations (connect, bind, socket, getaddrinfo, gethostbyname) without sandbox checks | Pass | No filesystem/network operation is added. One native cleanup worker executes no user code; existing worker functional domains and program/resource ownership remain intact. Linux fault tests temporarily change only their own process soft limit with RAII restoration. |
| 19. If filesystem/network ops exist, verify `QoreSandboxManagerHelper` usage | Pass | No filesystem/network operation is added. One native cleanup worker executes no user code; existing worker functional domains and program/resource ownership remain intact. Linux fault tests temporarily change only their own process soft limit with RAII restoration. |
| 20. No `File::`, `Dir::`, `Socket::`, `HTTPClient::` usage without justification | Pass | No filesystem/network operation is added. One native cleanup worker executes no user code; existing worker functional domains and program/resource ownership remain intact. Linux fault tests temporarily change only their own process soft limit with RAII restoration. |
| 21. All `for`/`while` loops that could iterate >100 times have `qore_check_cancel()` checks | Pass | Native cleanup has no Qore program/TID and cannot abandon accepted joins on cancellation. Its condition wait is event-driven; joins run outside the queue lock. Runtime drain uses the existing internal non-interruptible counter wait before module unload. This is cleanup synchronization, not interruptible user work. |
| 22. Uses `qore_check_cancel()` (NOT deprecated `qore_check_io_interrupt()`) | Pass | Native cleanup has no Qore program/TID and cannot abandon accepted joins on cancellation. Its condition wait is event-driven; joins run outside the queue lock. Runtime drain uses the existing internal non-interruptible counter wait before module unload. This is cleanup synchronization, not interruptible user work. |
| 23. Check frequency: every 100 iterations for tight loops, every 10 for expensive iterations | Pass | Native cleanup has no Qore program/TID and cannot abandon accepted joins on cancellation. Its condition wait is event-driven; joins run outside the queue lock. Runtime drain uses the existing internal non-interruptible counter wait before module unload. This is cleanup synchronization, not interruptible user work. |
| 24. No blocking operations without cancellation support | Pass | Native cleanup has no Qore program/TID and cannot abandon accepted joins on cancellation. Its condition wait is event-driven; joins run outside the queue lock. Runtime drain uses the existing internal non-interruptible counter wait before module unload. This is cleanup synchronization, not interruptible user work. |
| 25. Every action has `display_name`, `short_desc` (plain text, <80 chars), `desc` (markdown) | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 26. Every action has `options` populated via `getActionOptionFromFields()` — without this, the action shows an empty, unusable form | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 27. Every action has `output_type` set to a typed data type constant (e.g., `MyResponseDataType`) — not omitted | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 28. DPAT_API actions: provider has `"supports_request": True` and implements `doRequestImpl()` | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 29. DPAT_FIND actions: every option exists in `SearchOptions`, `getRecordTypeImpl()` returns `*hash<string, AbstractDataField>` | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 30. Scheme-based apps (with `"scheme"` in registerApp): actions use `"path"` and do NOT use `"cls"` — having both `scheme` and `cls` causes a runtime error | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 31. Single-key hash slices use trailing comma: `Fields{"key",}` (without trailing comma, `Fields{"key"}` returns the value, not a hash) | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 32. **Typed data type classes exist** for request and response types — inherit `HashDataType`, have `const Fields` hash, call `addQoreFields(Fields)` in constructor, export public constant at bottom (e.g., `public const MyDataType = new MyDataType();`) | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 33. Request/input types use `public` Fields (enables `ClassName::Fields` in action registration) | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 34. Response/output types use `private` Fields | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 35. Each field in data types has `display_name`, `type`, and `desc` (markdown-formatted) | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 36. Input fields have `example_value` where useful (string fields, endpoint URIs, SQL queries, etc.) | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 37. Fields with finite allowed values use `allowed_values` with `AllowedValueInfo` containing both `value` and `display_name` (Title Case, human-readable) — never bare values, never described only in text | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 38. Password/secret fields have `"sensitive": True` | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 39. `groups` uses `AppGroup` enum values from `qlib/DataProvider/AppGroup.qc` | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 40. App `logo` stored as separate file, loaded at module level in `Priv` namespace | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 41. App `desc` uses markdown: bullet list of capabilities, links to project website, business-language explanation of value | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 42. `display_name` is user-friendly ("Apache Avro" not "avro") | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 43. `short_desc` is plain text, under 80 chars, single sentence — no markdown | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 44. `desc` uses markdown: backticks for code/field refs (`` `field_name` ``, `` `True` ``, `` `pdf` ``), `\n\n` for paragraphs, `- ` bullet lists for enumerations, `**bold**` for caveats | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 45. Descriptions use plain business language relating to common challenges — not just technical "what" but "why" and "when to use" | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 46. No bare `True`/`False`/`NOTHING` — must be backtick-wrapped in `desc` | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 47. No bare field/option names in prose — must use backticks | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 48. Long descriptions (>500 chars) use bold section headers and bullet lists | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 49. **Factory registration in Qore repo**: every factory name registered in `qlib/DataProvider/DataProvider.qc` → `FactoryMap` (without this, module loads but doesn't appear in Qorus apps) | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 50. **`getRecordTypeImpl()` signature**: must be `private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options)` — NOT returning `*AbstractDataProviderType` | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 51. **Dependency JARs committed** (for JNI modules): JAR files in `qlib/*/jar/` may be gitignored — use `git add -f` to ensure they're tracked, otherwise CI compilation fails | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 52. JAR install rules in CMakeLists.txt for all dependency JARs | N/A | No new Qore module, QPP class, provider action/app, field metadata, factory, or dependency packaging. |
| 53. **No workarounds**: No TODOs, FIXMEs, stubs, or partially-implemented features | Pass | Debugger shows a live glibc worker at process exit. A native TLS barrier fails against the old runtime; fix uses pthread_join, with no delays, cache tuning, suppression, or fixture-specific branch. |
| 54. **Exception safety**: C++ uses `ReferenceHolder` for Qore allocations, `std::unique_ptr` for C++ allocations, `*xsink` checked after every fallible operation | Pass | Final native/HTTP/SOAP memory checks have no lost allocations or errors; the separately isolated libc failure-path finding is recorded above. Completion transfers the existing ThreadArg without allocation; reaper owns it with unique_ptr. Startup failure returns THREAD-CREATION-FAILURE before launching a worker; failed worker creation retains existing counter/TID rollback. Join ownership failure terminates rather than performing unsafe teardown. |
| 55. **Thread safety**: All mutable shared state protected by `std::lock_guard<std::mutex>` or documented as immutable-after-construction | Pass | Queue and start/stop state use QoreThreadLock/AutoLocker/SafeLocker, consistent with this runtime. Native ownership is assigned during activation under the thread-list lock, before TID reuse. Publication and dequeue are synchronized; only the reaper joins workers and only runtime cleanup joins the reaper. |
| 56. **Type safety**: Strongly-typed `code<return(args)>` instead of untyped `code`; `static_cast` instead of C casts; typed hashdecls for results; enums where appropriate | Pass | Typed native callbacks, static_cast, bool state, pthread_t handles and ThreadArg links. No new untyped Qore code/hash or C-style cast. |
| 57. **Performance**: No O(n²) where O(n) is possible; no unnecessary copies; coordinate descent uses incremental residuals not full matrix multiply | Pass | One lazy native worker; O(1) completion enqueue/dequeue; no allocation during completion and no per-termination worker creation. Joins reclaim completed thread storage during runtime rather than retaining every handle until shutdown. |
| 58. **Error handling**: All inputs validated (dimensions, empty data, unfitted models); C++ I/O handles EAGAIN/EINTR if applicable | Pass | Actual kernel pthread_create failure for reaper and worker, invalid stack size, counter rollback and subsequent retry pass final native checks. Native join does not return EINTR; unexpected ownership errors are fatal instead of ignored. |
| 59. **Documentation**: Doxygen `@param`, `@return`, `@throw` on all public methods; `@par Example` with realistic business scenarios; `@note` for important caveats | Pass | Existing q_start_thread flag contract, durable TLS lifecycle design, native test README with build/memory-check/fault-test commands and release notes updated. No new public method. |
| 60. **QPP flags**: `[flags=CONSTANT]` on methods that never throw; `[flags=RET_VALUE_ONLY]` on methods that throw but have no side effects | N/A | No QPP signature or method flags change. |
| 61. **Security**: No user-controlled format strings; no buffer overflows; bounds checking on array indices; no credentials in code | Pass | Fixed diagnostic format strings, no buffers or credentials; private native queue cannot be controlled through arbitrary thread IDs. Test resource limits restored by a scope guard. |
| 62. **Correctness**: Algorithms verified against reference implementations; edge cases tested (empty data, single sample, all-zero features) | Pass | Final build, 78 core and 482 XML/SOAP cases pass; corpus results and later-phase owners are unchanged. Ordinary native and affected HTTP/SOAP memory checks are clean. Native empty/failure/retry checks pass; the barrier fails against the old runtime. Independent libc/tool findings are explicitly recorded above and assigned to P9. |

Final artifact SHA-256:

- `lib/thread.cpp`: `bdf652556d157976d4a07ff14945044787e6f51901220dced662762346b65197`
- `lib/qore-main.cpp`: `cb47f55ddffce64259ec0a0177cfe7b9245733e5f3482e662940940564bd5d20`
- `examples/test/qore/classes/ThreadPool/native_thread_cleanup.cpp`: `9bae221650724ea82ba4e6576fd65537b348dc207432f402d232398d6d7f9580`
- `build-debug/libqore.so.20.0.0`: `eaa99dd28ec1ecc9d756009d1c6c735d4c6cb40a547a57a12137fd5ed541f752`
