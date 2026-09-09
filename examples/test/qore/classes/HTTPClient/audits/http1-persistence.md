# HTTP/1 client persistence audit

Copyright (C) 2026 Qore Technologies, s.r.o.

The full audit-changes checklist and applicable module-structure, sandboxing and
cooperative-cancellation design guides were applied to the exact implementation,
regression, release-note entry and implemented design committed with this report.
No concurrent AsyncIoController or WebSocketHandler development is included.

Checklist: 25 Pass / 37 N/A / 0 Fail.

Evidence uses the isolated Debug build with `/usr` prefix and local libraries,
without installation. The source matches this checkout's original implementation
before applying the patch; only the audited files are copied into develop.

- Core SHA-256: `8fe8c3c1bc4b1c09eddb76f0001cdba50f98be6ea3dcabe3f4e92b05e8eadd6b`.
- Implementation SHA-256: `3c6855a2660fa6766f740ff6410711db0f7c392be5976f4f8c482b3b49f55e5d`.
- Regression SHA-256: `f0074339f6a3b419586d2303d887ba5649ba27bb500b07c98f567432fd980979`.
- New regression: 4 cases / 111 assertions in each AST, IR, JIT and tiered mode.
- Existing HTTPClient suite: 256 assertions; 22 completed cases and two existing
  prerequisite skips (proxy absent; external server does not advertise HTTP/3).
  Its QUnit summary counts those skipped rows in the displayed total of 24.
- Existing HttpClientIo redirects: 7 cases / 16 assertions.
- Independent XML schema HTTP matrix: five Python methods, including HTTP/1.0
  redirects, relative includes, ISO-8859-1 bytes, invalid schemas/documents,
  destination policy denial, redirect destination checks, and later recovery.
- XML module gate: 84 suites return success, with 901 cases and 33,303 reported
  assertions. Every existing suite result is identical to P3-35. Both-version raw
  and strict corpus reports are structurally identical, including the previously
  assigned later-phase findings. HTTP peer checks are separate from payload coverage.
- Affected regression under Valgrind with `qore -b --enable-debug --exec-mode=jit`
  and the authorized `QORE_PCRE2_NO_JIT=1`: 0 errors, 0 definite/indirect/possible
  lost bytes, no suppressions. Qore JIT remains enabled.

The pre-fix deterministic server observes a second request on the nonpersistent
connection. The independent HTTP/1.0 redirect produces HTTP1-CONNECTION-CLOSED.
Both pass after the persistence fix.

Environment findings remain explicit in module-xml's P9 register: Valgrind's known
core DWARF-reader warning and a system SSSD `fstat(-1)` warning. A syscall stack
traces the latter to libnss_sss's service-name lookup via c-ares during socket bind,
not to this response-header implementation. Neither is suppressed or counted as
a clean environment check. Core/XSD implementation and memory assertions pass;
these findings do not close P9.

Reproducible logs and the frozen manifest are `/tmp/wsdl-p3-36-core-final-*`.
The syscall evidence is `/tmp/wsdl-p3-36-core-http-fstat.strace`.

|!Check|!Status|!Evidence
|1. Entry exists in `doxygen/lang/120_modules.dox.tmpl` (for modules in the Qore repo; N/A for external module repos)|N/A|Existing core implementation; no module registration or new QPP class is added.
|2. Entry exists in `doxygen/lang/900_release_notes.dox.tmpl` (for modules in the Qore repo; external modules have release notes in their .qm)|Pass|Qore 3.0 networking release notes describe HTTP/1.0 persistence, redirect reconnection and Connection options.
|3. `qore_user_module()` or `qore_external_user_module()` call in `CMakeLists.txt`|N/A|Existing core implementation; no module registration or new QPP class is added.
|4. Module added to QMOD list in `CMakeLists.txt`|N/A|Existing core implementation; no module registration or new QPP class is added.
|5. `.qm` file has `@section <lowercasemodname>intro` as first doc section — **must be all lowercase** (e.g., `avrodataproviderintro`, not `AvroDataProviderintro`)|N/A|No Qore user module or separated module layout is changed.
|6. `%modern` in `.qm` file — no redundant `%new-style`, `%require-types`, `%strict-args`, `%enable-all-warnings`|N/A|No Qore user module or separated module layout is changed.
|7. No parse directives (`%requires`, `%modern`, `%new-style`) in separated `.qc` files (check OUTSIDE of `@code` blocks only)|N/A|No Qore user module or separated module layout is changed.
|8. No `%include` usage (deprecated for modules)|Pass|No deprecated module inclusion is introduced.
|9. Copyright 2026 on all new files|Pass|New regression, implemented design and this audit carry 2026 copyright.
|10. Directory layout: `.qm` inside `qlib/<ModuleName>/` directory (not at `qlib/<ModuleName>.qm` for multi-file modules)|N/A|No Qore user module or separated module layout is changed.
|11. No second `.qm` for the same module at `qlib/<ModuleName>.qm`|N/A|No Qore user module or separated module layout is changed.
|12. `ns=Qore::XX` matches the QoreNamespace constructor path|N/A|Existing core implementation; no module registration or new QPP class is added.
|13. `%modern` directive present|Pass|The new HTTP/1 persistence regression uses %modern.
|14. Executable permission set (`chmod +x`)|Pass|The new qtest is executable (0755).
|15. Uses %prepend-module-path  before %requires for in-repo modules (Qore and Qore modules only; not Qorus)|Pass|The qtest prepends the repository qlib and requires QUnit.qm by its relative local path.
|16. External module dependencies use `%try-module` — except modules delivered with the project itself (Qore ex: DataProvider, ConnectionProvider, QUnit, etc.) which use hard `%requires`|Pass|The test uses only Qore core classes and the bundled QUnit module; no external binary dependency.
|17. No filesystem operations (fopen, open, creat, unlink, remove, rename, mkdir, rmdir, stat, chmod) without sandbox checks|Pass|The change adds no filesystem operation.
|18. No network operations (connect, bind, socket, getaddrinfo, gethostbyname) without sandbox checks|Pass|The changed response-header logic adds no connection or DNS operation. It uses the existing checked HTTP transport.
|19. If filesystem/network ops exist, verify `QoreSandboxManagerHelper` usage|Pass|Existing socket setup and HTTPClient paths use QoreSandboxManager; the XML integration matrix also verifies destination IP and redirect policy rejection.
|20. No `File::`, `Dir::`, `Socket::`, `HTTPClient::` usage without justification|Pass|The local test server and HTTPClient are required to verify real connection reuse; sockets bind ephemeral loopback ports and close deterministically.
|21. All `for`/`while` loops that could iterate >100 times have `qore_check_cancel()` checks|Pass|Connection option and duplicate-header scans check qore_check_cancel; trimming loops do the same.
|22. Uses `qore_check_cancel()` (NOT deprecated `qore_check_io_interrupt()`)|Pass|Only qore_check_cancel is used; no deprecated I/O interrupt API.
|23. Check frequency: every 100 iterations for tight loops, every 10 for expensive iterations|Pass|Cancellation is checked on each option, duplicate field and whitespace iteration.
|24. No blocking operations without cancellation support|Pass|No blocking production operation is added. Test socket operations have 20-second deadlines, and completion uses a bounded queue event.
|25. Every action has `display_name`, `short_desc` (plain text, <80 chars), `desc` (markdown)|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|26. Every action has `options` populated via `getActionOptionFromFields()` — without this, the action shows an empty, unusable form|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|27. Every action has `output_type` set to a typed data type constant (e.g., `MyResponseDataType`) — not omitted|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|28. DPAT_API actions: provider has `"supports_request": True` and implements `doRequestImpl()`|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|29. DPAT_FIND actions: every option exists in `SearchOptions`, `getRecordTypeImpl()` returns `*hash<string, AbstractDataField>`|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|30. Scheme-based apps (with `"scheme"` in registerApp): actions use `"path"` and do NOT use `"cls"` — having both `scheme` and `cls` causes a runtime error|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|31. Single-key hash slices use trailing comma: `Fields{"key",}` (without trailing comma, `Fields{"key"}` returns the value, not a hash)|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|32. **Typed data type classes exist** for request and response types — inherit `HashDataType`, have `const Fields` hash, call `addQoreFields(Fields)` in constructor, export public constant at bottom (e.g., `public const MyDataType = new MyDataType();`)|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|33. Request/input types use `public` Fields (enables `ClassName::Fields` in action registration)|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|34. Response/output types use `private` Fields|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|35. Each field in data types has `display_name`, `type`, and `desc` (markdown-formatted)|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|36. Input fields have `example_value` where useful (string fields, endpoint URIs, SQL queries, etc.)|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|37. Fields with finite allowed values use `allowed_values` with `AllowedValueInfo` containing both `value` and `display_name` (Title Case, human-readable) — never bare values, never described only in text|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|38. Password/secret fields have `"sensitive": True`|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|39. `groups` uses `AppGroup` enum values from `qlib/DataProvider/AppGroup.qc`|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|40. App `logo` stored as separate file, loaded at module level in `Priv` namespace|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|41. App `desc` uses markdown: bullet list of capabilities, links to project website, business-language explanation of value|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|42. `display_name` is user-friendly ("Apache Avro" not "avro")|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|43. `short_desc` is plain text, under 80 chars, single sentence — no markdown|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|44. `desc` uses markdown: backticks for code/field refs (`` `field_name` ``, `` `True` ``, `` `pdf` ``), `\n\n` for paragraphs, `- ` bullet lists for enumerations, `**bold**` for caveats|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|45. Descriptions use plain business language relating to common challenges — not just technical "what" but "why" and "when to use"|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|46. No bare `True`/`False`/`NOTHING` — must be backtick-wrapped in `desc`|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|47. No bare field/option names in prose — must use backticks|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|48. Long descriptions (>500 chars) use bold section headers and bullet lists|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|49. **Factory registration in Qore repo**: every factory name registered in `qlib/DataProvider/DataProvider.qc` → `FactoryMap` (without this, module loads but doesn't appear in Qorus apps)|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|50. **`getRecordTypeImpl()` signature**: must be `private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options)` — NOT returning `*AbstractDataProviderType`|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|51. **Dependency JARs committed** (for JNI modules): JAR files in `qlib/*/jar/` may be gitignored — use `git add -f` to ensure they're tracked, otherwise CI compilation fails|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|52. JAR install rules in CMakeLists.txt for all dependency JARs|N/A|No DataProvider action, type, app, factory or JNI dependency is changed.
|53. **No workarounds**: No TODOs, FIXMEs, stubs, or partially-implemented features|Pass|The change implements RFC 9112 section 9.3 directly; no fixture special case or disabled transport path.
|54. **Exception safety**: C++ uses `ReferenceHolder` for Qore allocations, `std::unique_ptr` for C++ allocations, `*xsink` checked after every fallible operation|Pass|The new parser allocates no owned native/Qore object. It preserves existing response and future ownership; Valgrind reports no lost blocks or memory errors.
|55. **Thread safety**: All mutable shared state protected by `std::lock_guard<std::mutex>` or documented as immutable-after-construction|Pass|The new keep_alive variable and lambda are local to response parsing on the I/O thread. Existing connection_close ownership and close-before-future dispatch are preserved.
|56. **Type safety**: Strongly-typed `code<return(args)>` instead of untyped `code`; `static_cast` instead of C casts; typed hashdecls for results; enums where appropriate|Pass|The scan uses QoreStringNode, QoreValue type checks, size_t and a checked const char pointer span; no untyped callable API is introduced.
|57. **Performance**: No O(n²) where O(n) is possible; no unnecessary copies; coordinate descent uses incremental residuals not full matrix multiply|Pass|Each header value is scanned linearly with bounded state, without copying strings or resolving service names.
|58. **Error handling**: All inputs validated (dimensions, empty data, unfitted models); C++ I/O handles EAGAIN/EINTR if applicable|Pass|Empty options, duplicate fields, mixed case, close precedence, and nonmatching token prefixes are tested. Existing response-body/error handling is unchanged.
|59. **Documentation**: Doxygen `@param`, `@return`, `@throw` on all public methods; `@par Example` with realistic business scenarios; `@note` for important caveats|Pass|The implemented design documents the persistence rule, reference, response ownership and deterministic test; release notes describe user-visible behavior.
|60. **QPP flags**: `[flags=CONSTANT]` on methods that never throw; `[flags=RET_VALUE_ONLY]` on methods that throw but have no side effects|Pass|No QPP method signature or flags change; response parsing remains an internal method with side effects.
|61. **Security**: No user-controlled format strings; no buffer overflows; bounds checking on array indices; no credentials in code|Pass|Length checks precede case-insensitive token comparisons. No credential, unchecked format string or new resource access.
|62. **Correctness**: Algorithms verified against reference implementations; edge cases tested (empty data, single sample, all-zero features)|Pass|Four-mode native integration passes 16 cases/444 assertions; live HTTP/1.0/1.1 connection and redirect tests assert exact values and connections. Independent Python schema HTTP tests pass all five methods.
