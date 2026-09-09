# Caught TLS handshake diagnostics audit

Copyright (C) 2026 Qore Technologies, s.r.o.

The full audit-changes checklist and applicable module-structure, sandboxing and
cooperative-cancellation guides were applied to the exact source, regressions,
release notes and audit committed together. Checklist: 19 Pass / 43 N/A / 0 Fail.

`SSLSocketHelper::startConnect()` already reports handshake errors through its
exception sink. Its additional `printd(0)` also wrote to stderr in ordinary Debug
execution, even when callers caught the exception. Debug level 5 retains optional
detailed logging while caught errors remain exclusively available to the caller.
No verification, TLS state transition or exception handling is removed.

The isolated Debug build uses prefix `/usr`, local LD_LIBRARY_PATH and no test
installation. Main-source equality was verified before copying this one-line fix.
Core SHA-256: `e7445dbdfd81e79f053c78cb9e60b77c368eaa04a619f9093014091ae4bda8da`.
QoreSocket.cpp SHA-256: `cd65012bc67f61c64b70b7888d1f7bfd29264aeddb65e6b9ab803b60e04a1c71`.
The exact test/source manifest and logs are `/tmp/wsdl-p3-36-core-tls-final-*`.

Validation:

- The standalone Python regression runs an independent local TLS peer with fresh
  certificates: trusted success, untrusted rejection and wrong-host rejection.
  Each case asserts empty Qore stderr, exact success bytes or the error category
  and retained certificate-verification diagnostic, and whether HTTP was reached.
- All three cases pass in each AST, IR, JIT and tiered mode (12 case executions).
- All three pass under Valgrind with Qore JIT enabled, `qore -b --enable-debug`,
  and the authorized `QORE_PCRE2_NO_JIT=1`: zero errors and zero
  definite/indirect/possible lost bytes; no suppressions.
- The first instrumented attempt exceeded the fixture's 30-second accept deadline
  during Qore startup. The final fixture allows 180 seconds for instrumented
  startup, while keeping the client operation's 20-second timeout. Final TLS
  assertions pass; the initial failed run remains in the recorded logs.
- Existing HTTPClient: 256 assertions, 22 completed cases and two existing
  prerequisite skips (proxy unset; the external server does not advertise HTTP/3).
- XML schema HTTPS/HTTP/catalog matrix: seven Python methods. XML gate: 84 suites,
  901 cases and 33,309 reported assertions; all previous 83 suite results unchanged.
  Both-version raw and strict corpus reports are structurally unchanged, including
  the independently tracked later-phase failures. This is not phase acceptance.

The known Valgrind DWARF-reader and host SSSD `fstat(-1)` warnings remain visible
and unsuppressed in the module-xml P9 environment register. The latter was traced
to libnss_sss/c-ares service lookup in the HTTP persistence audit. These warnings
are not counted as a clean environment gate.

Run `python3 examples/test/qore/classes/HTTPClient/test_tls_error_output.py -v`
with the intended Debug Qore in PATH and its library in LD_LIBRARY_PATH. Set
`QORE_EXEC_MODE` for a specific mode; `QORE_TEST_VALGRIND_DIR` enables Valgrind and
puts its diagnostics in separate files without filtering Qore stderr.

|!Check|!Status|!Evidence
|1. Entry exists in `doxygen/lang/120_modules.dox.tmpl` (for modules in the Qore repo; N/A for external module repos)|N/A|No new user module, module layout, registration or QPP class.
|2. Entry exists in `doxygen/lang/900_release_notes.dox.tmpl` (for modules in the Qore repo; external modules have release notes in their .qm)|Pass|The Qore 3.0 release notes describe caught TLS errors and optional detailed diagnostics.
|3. `qore_user_module()` or `qore_external_user_module()` call in `CMakeLists.txt`|N/A|No new user module, module layout, registration or QPP class.
|4. Module added to QMOD list in `CMakeLists.txt`|N/A|No new user module, module layout, registration or QPP class.
|5. `.qm` file has `@section <lowercasemodname>intro` as first doc section — **must be all lowercase** (e.g., `avrodataproviderintro`, not `AvroDataProviderintro`)|N/A|No new user module, module layout, registration or QPP class.
|6. `%modern` in `.qm` file — no redundant `%new-style`, `%require-types`, `%strict-args`, `%enable-all-warnings`|N/A|No new user module, module layout, registration or QPP class.
|7. No parse directives (`%requires`, `%modern`, `%new-style`) in separated `.qc` files (check OUTSIDE of `@code` blocks only)|N/A|No new user module, module layout, registration or QPP class.
|8. No `%include` usage (deprecated for modules)|Pass|No module inclusion or deprecated directive is introduced.
|9. Copyright 2026 on all new files|Pass|The new Python regression, Qore driver and audit carry 2026 copyright; QoreSocket.cpp already carries 2026.
|10. Directory layout: `.qm` inside `qlib/<ModuleName>/` directory (not at `qlib/<ModuleName>.qm` for multi-file modules)|N/A|No new user module, module layout, registration or QPP class.
|11. No second `.qm` for the same module at `qlib/<ModuleName>.qm`|N/A|No new user module, module layout, registration or QPP class.
|12. `ns=Qore::XX` matches the QoreNamespace constructor path|N/A|No new user module, module layout, registration or QPP class.
|13. `%modern` directive present|Pass|The executable Qore driver explicitly uses %modern.
|14. Executable permission set (`chmod +x`)|Pass|The Python regression and Qore driver are executable (0755).
|15. Uses %prepend-module-path  before %requires for in-repo modules (Qore and Qore modules only; not Qorus)|N/A|The new Qore driver has no module imports; all referenced classes are built into Qore.
|16. External module dependencies use `%try-module` — except modules delivered with the project itself (Qore ex: DataProvider, ConnectionProvider, QUnit, etc.) which use hard `%requires`|Pass|The driver uses only built-in HTTPClient and ExceptionInfo; no external module dependency.
|17. No filesystem operations (fopen, open, creat, unlink, remove, rename, mkdir, rmdir, stat, chmod) without sandbox checks|Pass|The production change adds no filesystem access. Test certificates are generated in a private temporary directory and deleted by its context manager.
|18. No network operations (connect, bind, socket, getaddrinfo, gethostbyname) without sandbox checks|Pass|The production change adds no network operation. The regression binds an ephemeral loopback listener and closes it on every exit.
|19. If filesystem/network ops exist, verify `QoreSandboxManagerHelper` usage|N/A|The changed logging statement performs no resource access requiring a new policy check.
|20. No `File::`, `Dir::`, `Socket::`, `HTTPClient::` usage without justification|Pass|The regression uses HTTPClient to exercise its actual TLS handshake; independent Python/OpenSSL peers establish trusted, untrusted and wrong-host cases.
|21. All `for`/`while` loops that could iterate >100 times have `qore_check_cancel()` checks|N/A|The production change contains no loop or new cancellation point; existing TLS cancellation is unchanged.
|22. Uses `qore_check_cancel()` (NOT deprecated `qore_check_io_interrupt()`)|N/A|The production change contains no loop or new cancellation point; existing TLS cancellation is unchanged.
|23. Check frequency: every 100 iterations for tight loops, every 10 for expensive iterations|N/A|The production change contains no loop or new cancellation point; existing TLS cancellation is unchanged.
|24. No blocking operations without cancellation support|Pass|No blocking production operation is added. Qore retains its existing cancel-aware TLS poll operation. The test uses socket deadlines, bounded processes and deterministic thread completion; instrumented startup has its own accept deadline.
|25. Every action has `display_name`, `short_desc` (plain text, <80 chars), `desc` (markdown)|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|26. Every action has `options` populated via `getActionOptionFromFields()` — without this, the action shows an empty, unusable form|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|27. Every action has `output_type` set to a typed data type constant (e.g., `MyResponseDataType`) — not omitted|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|28. DPAT_API actions: provider has `"supports_request": True` and implements `doRequestImpl()`|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|29. DPAT_FIND actions: every option exists in `SearchOptions`, `getRecordTypeImpl()` returns `*hash<string, AbstractDataField>`|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|30. Scheme-based apps (with `"scheme"` in registerApp): actions use `"path"` and do NOT use `"cls"` — having both `scheme` and `cls` causes a runtime error|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|31. Single-key hash slices use trailing comma: `Fields{"key",}` (without trailing comma, `Fields{"key"}` returns the value, not a hash)|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|32. **Typed data type classes exist** for request and response types — inherit `HashDataType`, have `const Fields` hash, call `addQoreFields(Fields)` in constructor, export public constant at bottom (e.g., `public const MyDataType = new MyDataType();`)|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|33. Request/input types use `public` Fields (enables `ClassName::Fields` in action registration)|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|34. Response/output types use `private` Fields|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|35. Each field in data types has `display_name`, `type`, and `desc` (markdown-formatted)|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|36. Input fields have `example_value` where useful (string fields, endpoint URIs, SQL queries, etc.)|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|37. Fields with finite allowed values use `allowed_values` with `AllowedValueInfo` containing both `value` and `display_name` (Title Case, human-readable) — never bare values, never described only in text|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|38. Password/secret fields have `"sensitive": True`|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|39. `groups` uses `AppGroup` enum values from `qlib/DataProvider/AppGroup.qc`|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|40. App `logo` stored as separate file, loaded at module level in `Priv` namespace|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|41. App `desc` uses markdown: bullet list of capabilities, links to project website, business-language explanation of value|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|42. `display_name` is user-friendly ("Apache Avro" not "avro")|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|43. `short_desc` is plain text, under 80 chars, single sentence — no markdown|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|44. `desc` uses markdown: backticks for code/field refs (`` `field_name` ``, `` `True` ``, `` `pdf` ``), `\n\n` for paragraphs, `- ` bullet lists for enumerations, `**bold**` for caveats|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|45. Descriptions use plain business language relating to common challenges — not just technical "what" but "why" and "when to use"|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|46. No bare `True`/`False`/`NOTHING` — must be backtick-wrapped in `desc`|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|47. No bare field/option names in prose — must use backticks|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|48. Long descriptions (>500 chars) use bold section headers and bullet lists|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|49. **Factory registration in Qore repo**: every factory name registered in `qlib/DataProvider/DataProvider.qc` → `FactoryMap` (without this, module loads but doesn't appear in Qorus apps)|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|50. **`getRecordTypeImpl()` signature**: must be `private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options)` — NOT returning `*AbstractDataProviderType`|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|51. **Dependency JARs committed** (for JNI modules): JAR files in `qlib/*/jar/` may be gitignored — use `git add -f` to ensure they're tracked, otherwise CI compilation fails|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|52. JAR install rules in CMakeLists.txt for all dependency JARs|N/A|No DataProvider action, type, app, factory, JNI or related build dependency changes.
|53. **No workarounds**: No TODOs, FIXMEs, stubs, or partially-implemented features|Pass|The erroneous default-level diagnostic is changed at its source to debug level 5. No test filters stderr and no TLS verification path is disabled.
|54. **Exception safety**: C++ uses `ReferenceHolder` for Qore allocations, `std::unique_ptr` for C++ allocations, `*xsink` checked after every fallible operation|Pass|The statement changes only a debug level and preserves sslError() propagation and socket/reference cleanup. All three affected TLS cases pass Valgrind with zero memory errors or lost blocks.
|55. **Thread safety**: All mutable shared state protected by `std::lock_guard<std::mutex>` or documented as immutable-after-construction|Pass|No mutable production state is added. Test peer records are inspected after the bounded thread join.
|56. **Type safety**: Strongly-typed `code<return(args)>` instead of untyped `code`; `static_cast` instead of C casts; typed hashdecls for results; enums where appropriate|Pass|The existing typed SSL error code and literal format string are unchanged. The Qore driver uses HTTPClient and hash<ExceptionInfo>.
|57. **Performance**: No O(n²) where O(n) is possible; no unnecessary copies; coordinate descent uses incremental residuals not full matrix multiply|Pass|No loop, allocation, copy or transport behavior is added.
|58. **Error handling**: All inputs validated (dimensions, empty data, unfitted models); C++ I/O handles EAGAIN/EINTR if applicable|Pass|Caught exceptions retain SOCKET-SSL-ERROR and certificate verify failed diagnostics. Trusted success preserves exact bytes; wrong-host and untrusted certificates reach no HTTP request.
|59. **Documentation**: Doxygen `@param`, `@return`, `@throw` on all public methods; `@par Example` with realistic business scenarios; `@note` for important caveats|Pass|The release notes describe the behavior. The regression module documents local Debug use and the optional unfiltered Valgrind logs. No public method signature changes.
|60. **QPP flags**: `[flags=CONSTANT]` on methods that never throw; `[flags=RET_VALUE_ONLY]` on methods that throw but have no side effects|N/A|No QPP method or flag changes.
|61. **Security**: No user-controlled format strings; no buffer overflows; bounds checking on array indices; no credentials in code|Pass|Certificate verification remains enabled. Certificates and keys are ephemeral fixtures, and no real credential or user-controlled format string is added.
|62. **Correctness**: Algorithms verified against reference implementations; edge cases tested (empty data, single sample, all-zero features)|Pass|The three TLS cases pass in AST, IR, JIT and tiered modes and under Valgrind. Existing HTTPClient tests and the XML/corpus comparison show no regression.
