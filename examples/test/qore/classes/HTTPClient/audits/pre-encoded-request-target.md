# Pre-encoded HTTP URI tilde audit

Copyright (C) 2026 Qore Technologies, s.r.o.

Applied all 62 audit-changes items and the applicable module-structure, sandboxing
and cooperative-cancellation guides to the exact C++, tests, notes and this audit:
19 Pass / 43 N/A / 0 Fail.

The pre-encoded URL rejection set incorrectly included tilde. RFC 3986 section 2.3
classifies it as unreserved. Removing that entry lets /~user/catalog.xsd reach the
requested resource; existing percent escapes and rejection of the other listed
characters retain their behavior. The ordinary URL encoding path is not changed.
The fixture first reproduced URL-ENCODING-ERROR on literal tilde in all three cases.

Validation uses an isolated Debug build with prefix /usr and local LD_LIBRARY_PATH;
no installation is needed. Main-source equality was verified before transferring
only the two-character C++/comment edit and the exact executable regressions.
The Debug library SHA-256 is
`40c2824efa1fdd66cd20c50ad767dd18614c6f6f2842d9a5fd4e94311e89802e`.
The source and test digests are in /tmp/wsdl-p3-37-core-uri-manifest.json.

Run python3 examples/test/qore/classes/HTTPClient/test_pre_encoded_request_target.py -v
with the tested Qore in PATH and its library in LD_LIBRARY_PATH. QORE_EXEC_MODE
selects AST, IR, JIT or tiered execution. QORE_TEST_VALGRIND_DIR enables separate,
unfiltered Valgrind diagnostic logs. Each run covers ten exact successful request
targets and 39 rejected unencoded paths, including reuse after rejection. Both the
constructor option and setter are exercised. All three methods pass in all four
modes (12 method executions, 196 path outcomes) and under Valgrind with Qore JIT,
-b --enable-debug and the authorized QORE_PCRE2_NO_JIT=1. Each instrumented process
reports zero errors and zero definite/indirect/possible lost bytes; no suppressions.

Existing HTTPClient reports 256 assertions, 22 completed cases and two existing
prerequisite skips (unset proxy and external peer without advertised HTTP/3).
The full XML gate passes 84 suites, 902 cases and 33,321 reported assertions;
the existing SOAP assertion-accounting anomaly remains recorded separately.
Both-version raw and strict corpus reports are structurally identical to P3-36,
including the tracked later-phase failures. The 15 survey unit tests pass.
Logs use the /tmp/wsdl-p3-37-core-* prefix. Known unsuppressed environment DWARF/
SSSD warnings remain in the module-xml P9 register and are not a clean environment
acceptance result. P3 and the overall XML plan remain in progress.

|!Check|!Status|!Evidence
|1. Entry exists in `doxygen/lang/120_modules.dox.tmpl` (for modules in the Qore repo; N/A for external module repos)|N/A|No new user module, module layout, registration or QPP class.
|2. Entry exists in `doxygen/lang/900_release_notes.dox.tmpl` (for modules in the Qore repo; external modules have release notes in their .qm)|Pass|Qore 3.0 release notes describe literal tilde support with /~user/catalog.xsd as an example.
|3. `qore_user_module()` or `qore_external_user_module()` call in `CMakeLists.txt`|N/A|No new user module, module layout, registration or QPP class.
|4. Module added to QMOD list in `CMakeLists.txt`|N/A|No new user module, module layout, registration or QPP class.
|5. `.qm` file has `@section <lowercasemodname>intro` as first doc section — **must be all lowercase** (e.g., `avrodataproviderintro`, not `AvroDataProviderintro`)|N/A|No new user module, module layout, registration or QPP class.
|6. `%modern` in `.qm` file — no redundant `%new-style`, `%require-types`, `%strict-args`, `%enable-all-warnings`|N/A|No new user module, module layout, registration or QPP class.
|7. No parse directives (`%requires`, `%modern`, `%new-style`) in separated `.qc` files (check OUTSIDE of `@code` blocks only)|N/A|No new user module, module layout, registration or QPP class.
|8. No `%include` usage (deprecated for modules)|Pass|The driver adds no module inclusion or deprecated directives.
|9. Copyright 2026 on all new files|Pass|Both new executable tests and this audit carry 2026 copyright; the C++ file already carries 2026.
|10. Directory layout: `.qm` inside `qlib/<ModuleName>/` directory (not at `qlib/<ModuleName>.qm` for multi-file modules)|N/A|No new user module, module layout, registration or QPP class.
|11. No second `.qm` for the same module at `qlib/<ModuleName>.qm`|N/A|No new user module, module layout, registration or QPP class.
|12. `ns=Qore::XX` matches the QoreNamespace constructor path|N/A|No new user module, module layout, registration or QPP class.
|13. `%modern` directive present|Pass|The Qore driver explicitly uses %modern.
|14. Executable permission set (`chmod +x`)|Pass|Both authored test files have executable permission (0755).
|15. Uses %prepend-module-path  before %requires for in-repo modules (Qore and Qore modules only; not Qorus)|N/A|The new Qore driver imports no modules; all classes are built into Qore.
|16. External module dependencies use `%try-module` — except modules delivered with the project itself (Qore ex: DataProvider, ConnectionProvider, QUnit, etc.) which use hard `%requires`|Pass|Only built-in HTTPClient and ExceptionInfo are used; no external dependency is imported.
|17. No filesystem operations (fopen, open, creat, unlink, remove, rename, mkdir, rmdir, stat, chmod) without sandbox checks|Pass|The production change only removes a character from an immutable rejection set; no filesystem access is added.
|18. No network operations (connect, bind, socket, getaddrinfo, gethostbyname) without sandbox checks|Pass|No network operation is added to production. The independent test peer binds ephemeral loopback sockets and closes them on every exit.
|19. If filesystem/network ops exist, verify `QoreSandboxManagerHelper` usage|N/A|The edited immutable character set accesses no filesystem or network resource.
|20. No `File::`, `Dir::`, `Socket::`, `HTTPClient::` usage without justification|Pass|The test intentionally exercises the HTTPClient URI path boundary against a separate byte-level Python peer.
|21. All `for`/`while` loops that could iterate >100 times have `qore_check_cancel()` checks|N/A|The production edit adds no loop, cancellation point or I/O; existing cancellation behavior is unchanged.
|22. Uses `qore_check_cancel()` (NOT deprecated `qore_check_io_interrupt()`)|N/A|The production edit adds no loop, cancellation point or I/O; existing cancellation behavior is unchanged.
|23. Check frequency: every 100 iterations for tight loops, every 10 for expensive iterations|N/A|The production edit adds no loop, cancellation point or I/O; existing cancellation behavior is unchanged.
|24. No blocking operations without cancellation support|Pass|No blocking production operation is added. The peer uses bounded socket/process deadlines and deterministic completion, with a separate instrumented startup deadline.
|25. Every action has `display_name`, `short_desc` (plain text, <80 chars), `desc` (markdown)|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|26. Every action has `options` populated via `getActionOptionFromFields()` — without this, the action shows an empty, unusable form|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|27. Every action has `output_type` set to a typed data type constant (e.g., `MyResponseDataType`) — not omitted|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|28. DPAT_API actions: provider has `"supports_request": True` and implements `doRequestImpl()`|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|29. DPAT_FIND actions: every option exists in `SearchOptions`, `getRecordTypeImpl()` returns `*hash<string, AbstractDataField>`|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|30. Scheme-based apps (with `"scheme"` in registerApp): actions use `"path"` and do NOT use `"cls"` — having both `scheme` and `cls` causes a runtime error|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|31. Single-key hash slices use trailing comma: `Fields{"key",}` (without trailing comma, `Fields{"key"}` returns the value, not a hash)|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|32. **Typed data type classes exist** for request and response types — inherit `HashDataType`, have `const Fields` hash, call `addQoreFields(Fields)` in constructor, export public constant at bottom (e.g., `public const MyDataType = new MyDataType();`)|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|33. Request/input types use `public` Fields (enables `ClassName::Fields` in action registration)|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|34. Response/output types use `private` Fields|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|35. Each field in data types has `display_name`, `type`, and `desc` (markdown-formatted)|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|36. Input fields have `example_value` where useful (string fields, endpoint URIs, SQL queries, etc.)|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|37. Fields with finite allowed values use `allowed_values` with `AllowedValueInfo` containing both `value` and `display_name` (Title Case, human-readable) — never bare values, never described only in text|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|38. Password/secret fields have `"sensitive": True`|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|39. `groups` uses `AppGroup` enum values from `qlib/DataProvider/AppGroup.qc`|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|40. App `logo` stored as separate file, loaded at module level in `Priv` namespace|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|41. App `desc` uses markdown: bullet list of capabilities, links to project website, business-language explanation of value|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|42. `display_name` is user-friendly ("Apache Avro" not "avro")|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|43. `short_desc` is plain text, under 80 chars, single sentence — no markdown|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|44. `desc` uses markdown: backticks for code/field refs (`` `field_name` ``, `` `True` ``, `` `pdf` ``), `\n\n` for paragraphs, `- ` bullet lists for enumerations, `**bold**` for caveats|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|45. Descriptions use plain business language relating to common challenges — not just technical "what" but "why" and "when to use"|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|46. No bare `True`/`False`/`NOTHING` — must be backtick-wrapped in `desc`|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|47. No bare field/option names in prose — must use backticks|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|48. Long descriptions (>500 chars) use bold section headers and bullet lists|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|49. **Factory registration in Qore repo**: every factory name registered in `qlib/DataProvider/DataProvider.qc` → `FactoryMap` (without this, module loads but doesn't appear in Qorus apps)|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|50. **`getRecordTypeImpl()` signature**: must be `private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options)` — NOT returning `*AbstractDataProviderType`|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|51. **Dependency JARs committed** (for JNI modules): JAR files in `qlib/*/jar/` may be gitignored — use `git add -f` to ensure they're tracked, otherwise CI compilation fails|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|52. JAR install rules in CMakeLists.txt for all dependency JARs|N/A|No DataProvider actions, types, apps, factories, JNI or related build dependencies change.
|53. **No workarounds**: No TODOs, FIXMEs, stubs, or partially-implemented features|Pass|The incorrect tilde entry is removed at its source per RFC 3986; no caller rewriting, skips, or validation suppression is added.
|54. **Exception safety**: C++ uses `ReferenceHolder` for Qore allocations, `std::unique_ptr` for C++ allocations, `*xsink` checked after every fallible operation|Pass|The immutable constant change adds no allocation or fallible operation. Existing exception and resource cleanup remain intact; all three affected processes pass Valgrind.
|55. **Thread safety**: All mutable shared state protected by `std::lock_guard<std::mutex>` or documented as immutable-after-construction|Pass|The rejection set is constexpr. Peer records are inspected after thread completion; no mutable production state is added.
|56. **Type safety**: Strongly-typed `code<return(args)>` instead of untyped `code`; `static_cast` instead of C casts; typed hashdecls for results; enums where appropriate|Pass|The existing constexpr character pointer remains typed. The Qore driver uses HTTPClient, int indices and hash<ExceptionInfo>.
|57. **Performance**: No O(n²) where O(n) is possible; no unnecessary copies; coordinate descent uses incremental residuals not full matrix multiply|Pass|No allocation, loop or copy is added to production. The existing path scan is unchanged in complexity.
|58. **Error handling**: All inputs validated (dimensions, empty data, unfitted models); C++ I/O handles EAGAIN/EINTR if applicable|Pass|Ten exact successful HTTP targets and 39 rejected unencoded-character paths are checked; a valid request on the same client after all rejections proves recovery.
|59. **Documentation**: Doxygen `@param`, `@return`, `@throw` on all public methods; `@par Example` with realistic business scenarios; `@note` for important caveats|Pass|Release notes include the changed behavior and a concrete path example. Tests document the normative source and executable driver; no public signature changes.
|60. **QPP flags**: `[flags=CONSTANT]` on methods that never throw; `[flags=RET_VALUE_ONLY]` on methods that throw but have no side effects|N/A|No QPP method or flag changes.
|61. **Security**: No user-controlled format strings; no buffer overflows; bounds checking on array indices; no credentials in code|Pass|No new format string, buffer operation or credential. Reserved percent escapes retain their exact HTTP bytes; controls and the existing forbidden delimiters remain rejected.
|62. **Correctness**: Algorithms verified against reference implementations; edge cases tested (empty data, single sample, all-zero features)|Pass|Independent byte-level targets and RFC 3986 expectations pass in AST, IR, JIT and tiered modes and Valgrind. Existing HTTPClient, 84 XML suites, survey unit tests and unchanged both-version corpus reports provide regression evidence.
