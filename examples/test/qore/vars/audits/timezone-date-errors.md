# Checked TimeZone date parsing audit

Copyright (C) 2026 Qore Technologies, s.r.o.

Scope: DateTimeNode header/implementation, TimeZone string conversion, its regression
suite, release notes and implemented design. Other developers' DataProvider and
astparser work is outside this change. All 62 audit-changes checklist entries are
recorded below; module structure, sandboxing, cancellation and provider guides
were consulted for applicability.

Root cause: TimeZone::date(string) selected the zone-aware constructor without an
exception sink. The parser therefore could not report INVALID-DATE. The additive
constructor passes both zone and sink directly to the existing parser. The QPP
method owns its result until parsing succeeds and advertises RET_VALUE_ONLY.

Validation completed using /tmp/wsdl-core-date/build-debug, Debug with the installed
/usr prefix and jemalloc disabled. No installation or push was performed. Native
SHA-256: cd01f8657d66b37f858a86f492a861ef216e28006cfabb10dc94f920e6d42476.

- The baseline regression fails on the missing INVALID-DATE; its four positive
  cases pass. The final suite passes 5 cases / 59 assertions in AST, IR, JIT and
  tiered modes under both UTC and Europe/Prague ambient zones.
- Sixteen core invocations report 66 cases / 1446 assertions, without runtime
  warnings or errors. Existing date, format, duration and relative-date suites
  pass in AST and IR; the pre-existing Windows-only date case is inapplicable
  on Linux. No new skips were added.
- The new suite and existing date suite pass Valgrind with qore -b --enable-debug
  and QORE_PCRE2_NO_JIT=1: zero errors, zero definite/indirect/possible loss,
  no suppressions. The previously recorded DW_AT_abstract_origin reader warning
  remains a P9 environment finding; reachable library state is reported separately.
- All 56 affected XML/SOAP suites pass: 658 cases, no runtime diagnostics. Both
  the both-version survey and strict coverage report are identical to P3-12
  outside version metadata. Strict selection: 89 WSDLs / 756 message directions,
  zero selected failures. Existing later-phase diagnostic failures remain visible.
- DateTimeNode header and generated TimeZone documentation pass focused Doxygen
  with parameter warnings enabled and the actual core tag file for references.
  The documented example runs successfully with asserted UTC instants.
- The isolated DataProvider-qmod target and dependencies were rebuilt before the
  final XML checks. The first downstream run exposed a source-hash mismatch in
  the older pinned DataProvider artifact after concurrent core commit 1985d3266;
  the final run uses its own matching source and AOT artifact, without suppression.

The fresh dependency build reports unrelated GCC 16.2.1 diagnostics: ngtcp2
-Winline under -Og; jsoncons bigint allocator-temporary -Wmaybe-uninitialized;
DataFrame's raw QoreValue bit copy -Wclass-memaccess and an unused overload of
qdfCallObjectMethod. These are retained in the build logs and routed to P9 build
verification; this commit makes no claim that the whole dependency build is
warning-free. Changed native date sources emit no compiler warnings.

Evidence: /tmp/wsdl-core-date-{baseline,build,aot-configure,aot-build}.log;
/tmp/wsdl-core-date-final-{checks,valgrind,existing-valgrind,xml-checks,survey,
coverage,docs,qpp-docs,example}.log and per-suite logs. Final survey/coverage JSON
files share the same prefix. The old nonthrowing exported constructor remains
present alongside the new overload (verified with nm -DC).

| Check | Status | Evidence |
| --- | --- | --- |
| 1. Entry exists in `doxygen/lang/120_modules.dox.tmpl` (for modules in the Qore repo; N/A for external module repos) | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 2. Entry exists in `doxygen/lang/900_release_notes.dox.tmpl` (for modules in the Qore repo; external modules have release notes in their .qm) | Pass | Qore 3.0 release notes document INVALID-DATE propagation, zone preservation and the additive C++ constructor. |
| 3. `qore_user_module()` or `qore_external_user_module()` call in `CMakeLists.txt` | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 4. Module added to QMOD list in `CMakeLists.txt` | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 5. `.qm` file has `@section <lowercasemodname>intro` as first doc section — **must be all lowercase** (e.g., `avrodataproviderintro`, not `AvroDataProviderintro`) | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 6. `%modern` in `.qm` file — no redundant `%new-style`, `%require-types`, `%strict-args`, `%enable-all-warnings` | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 7. No parse directives (`%requires`, `%modern`, `%new-style`) in separated `.qc` files (check OUTSIDE of `@code` blocks only) | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 8. No `%include` usage (deprecated for modules) | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 9. Copyright 2026 on all new files | Pass | New test/design/audit files and touched native notices use 2026. |
| 10. Directory layout: `.qm` inside `qlib/<ModuleName>/` directory (not at `qlib/<ModuleName>.qm` for multi-file modules) | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 11. No second `.qm` for the same module at `qlib/<ModuleName>.qm` | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 12. `ns=Qore::XX` matches the QoreNamespace constructor path | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 13. `%modern` directive present | Pass | The new Qore test uses %modern. |
| 14. Executable permission set (`chmod +x`) | Pass | timezone-date-errors.qtest is executable (0755). |
| 15. Uses %prepend-module-path  before %requires for in-repo modules (Qore and Qore modules only; not Qorus) | Pass | The local qlib prepend precedes a relative hard requirement for ../../../../qlib/QUnit.qm. |
| 16. External module dependencies use `%try-module` — except modules delivered with the project itself (Qore ex: DataProvider, ConnectionProvider, QUnit, etc.) which use hard `%requires` | N/A | Only in-repository QUnit is required; no external binary test dependency. |
| 17. No filesystem operations (fopen, open, creat, unlink, remove, rename, mkdir, rmdir, stat, chmod) without sandbox checks | Pass | The added constructor and method perform no filesystem operations. |
| 18. No network operations (connect, bind, socket, getaddrinfo, gethostbyname) without sandbox checks | Pass | The added constructor and method perform no network operations. |
| 19. If filesystem/network ops exist, verify `QoreSandboxManagerHelper` usage | N/A | No filesystem or network operation is added by the date conversion change. |
| 20. No `File::`, `Dir::`, `Socket::`, `HTTPClient::` usage without justification | N/A | No Qore module changes; the test uses no direct file or network operations. |
| 21. All `for`/`while` loops that could iterate >100 times have `qore_check_cancel()` checks | N/A | The new native path contains no loops. |
| 22. Uses `qore_check_cancel()` (NOT deprecated `qore_check_io_interrupt()`) | N/A | No cancellation API or long-running operation is introduced. |
| 23. Check frequency: every 100 iterations for tight loops, every 10 for expensive iterations | N/A | No new native loops require periodic checks. |
| 24. No blocking operations without cancellation support | Pass | The change only invokes the existing in-memory date parser; no blocking operations. |
| 25. Every action has `display_name`, `short_desc` (plain text, <80 chars), `desc` (markdown) | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 26. Every action has `options` populated via `getActionOptionFromFields()` — without this, the action shows an empty, unusable form | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 27. Every action has `output_type` set to a typed data type constant (e.g., `MyResponseDataType`) — not omitted | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 28. DPAT_API actions: provider has `"supports_request": True` and implements `doRequestImpl()` | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 29. DPAT_FIND actions: every option exists in `SearchOptions`, `getRecordTypeImpl()` returns `*hash<string, AbstractDataField>` | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 30. Scheme-based apps (with `"scheme"` in registerApp): actions use `"path"` and do NOT use `"cls"` — having both `scheme` and `cls` causes a runtime error | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 31. Single-key hash slices use trailing comma: `Fields{"key",}` (without trailing comma, `Fields{"key"}` returns the value, not a hash) | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 32. **Typed data type classes exist** for request and response types — inherit `HashDataType`, have `const Fields` hash, call `addQoreFields(Fields)` in constructor, export public constant at bottom (e.g., `public const MyDataType = new MyDataType();`) | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 33. Request/input types use `public` Fields (enables `ClassName::Fields` in action registration) | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 34. Response/output types use `private` Fields | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 35. Each field in data types has `display_name`, `type`, and `desc` (markdown-formatted) | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 36. Input fields have `example_value` where useful (string fields, endpoint URIs, SQL queries, etc.) | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 37. Fields with finite allowed values use `allowed_values` with `AllowedValueInfo` containing both `value` and `display_name` (Title Case, human-readable) — never bare values, never described only in text | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 38. Password/secret fields have `"sensitive": True` | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 39. `groups` uses `AppGroup` enum values from `qlib/DataProvider/AppGroup.qc` | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 40. App `logo` stored as separate file, loaded at module level in `Priv` namespace | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 41. App `desc` uses markdown: bullet list of capabilities, links to project website, business-language explanation of value | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 42. `display_name` is user-friendly ("Apache Avro" not "avro") | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 43. `short_desc` is plain text, under 80 chars, single sentence — no markdown | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 44. `desc` uses markdown: backticks for code/field refs (`` `field_name` ``, `` `True` ``, `` `pdf` ``), `\n\n` for paragraphs, `- ` bullet lists for enumerations, `**bold**` for caveats | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 45. Descriptions use plain business language relating to common challenges — not just technical "what" but "why" and "when to use" | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 46. No bare `True`/`False`/`NOTHING` — must be backtick-wrapped in `desc` | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 47. No bare field/option names in prose — must use backticks | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 48. Long descriptions (>500 chars) use bold section headers and bullet lists | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 49. **Factory registration in Qore repo**: every factory name registered in `qlib/DataProvider/DataProvider.qc` → `FactoryMap` (without this, module loads but doesn't appear in Qorus apps) | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 50. **`getRecordTypeImpl()` signature**: must be `private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options)` — NOT returning `*AbstractDataProviderType` | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 51. **Dependency JARs committed** (for JNI modules): JAR files in `qlib/*/jar/` may be gitignored — use `git add -f` to ensure they're tracked, otherwise CI compilation fails | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 52. JAR install rules in CMakeLists.txt for all dependency JARs | N/A | No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency. |
| 53. **No workarounds**: No TODOs, FIXMEs, stubs, or partially-implemented features | Pass | The existing parser receives its missing error sink and the intended zone together. No second parse, lexical workaround, exception suppression or stub. |
| 54. **Exception safety**: C++ uses `ReferenceHolder` for Qore allocations, `std::unique_ptr` for C++ allocations, `*xsink` checked after every fallible operation | Pass | ReferenceHolder owns the date allocation before the sink is inspected and releases it only on success. DateTime owns its private state; there is no raw unmanaged temporary. |
| 55. **Thread safety**: All mutable shared state protected by `std::lock_guard<std::mutex>` or documented as immutable-after-construction | Pass | No shared mutable state is added. The existing immutable zone is passed to a per-call date object. |
| 56. **Type safety**: Strongly-typed `code<return(args)>` instead of untyped `code`; `static_cast` instead of C casts; typed hashdecls for results; enums where appropriate | Pass | Strongly typed native constructor parameters, ReferenceHolder<DateTimeNode> and typed Qore test variables; no casts or untyped result hashes. |
| 57. **Performance**: No O(n²) where O(n) is possible; no unnecessary copies; coordinate descent uses incremental residuals not full matrix multiply | Pass | One existing parse per call, with constant additional ownership/error-handling work. |
| 58. **Error handling**: All inputs validated (dimensions, empty data, unfitted models); C++ I/O handles EAGAIN/EINTR if applicable | Pass | The new suite rejects 18 malformed/calendar/time/offset inputs with INVALID-DATE, recovers after each, and preserves fixed/region/explicit zones and all valid neighboring overloads. |
| 59. **Documentation**: Doxygen `@param`, `@return`, `@throw` on all public methods; `@par Example` with realistic business scenarios; `@note` for important caveats | Pass | Public parameters, return value, INVALID-DATE and zone caveats are documented. Native header and generated TimeZone Doxygen checks pass with parameter warnings enabled. Examples and durable design explain explicit-zone use. |
| 60. **QPP flags**: `[flags=CONSTANT]` on methods that never throw; `[flags=RET_VALUE_ONLY]` on methods that throw but have no side effects | Pass | The fallible side-effect-free string method now uses RET_VALUE_ONLY,NAMED_ARGS; other overload flags are unchanged. |
| 61. **Security**: No user-controlled format strings; no buffer overflows; bounds checking on array indices; no credentials in code | Pass | No new buffer arithmetic, user-controlled format strings, credentials or external I/O. |
| 62. **Correctness**: Algorithms verified against reference implementations; edge cases tested (empty data, single sample, all-zero features) | Pass | Baseline reproduces the missing exception. Final execution-mode/zone matrix, 56 downstream suites, unchanged survey/strict coverage and both Valgrind runs pass; see validation evidence above. |
