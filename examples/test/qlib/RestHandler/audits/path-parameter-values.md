# REST parameter and SQL documentation audit

Copyright (C) 2026 Qore Technologies, s.r.o.

Scope: RestHandler.qm, Swagger.qm, PgsqlSqlUtilBase.qm (documentation only),
RestPathEncoding.qtest and this report. During review, the original PostgreSQL
foreign-key change and REST encoding change were committed by parallel work as
a29e8a63e and eef86281d. This commit completes the defects found by the expanded
audit. Concurrent Fireworks development is excluded; no astparser source, build
target or documentation is changed.

The complete audit-changes skill and applicable module-structure and sandboxing
guides were reviewed. All 62 checklist items are recorded below. No native or
DataProvider registration implementation is changed.

Root causes and verification:

- REST now splits raw URI components before decoding (eef86281d), but Swagger
  still passed encoded substrings into parameter conversion and validation.
  A live request for folder%2Fcustomer%20API reproduced a decoded resource name
  paired with an encoded args.id. Decoding the extracted parameter fixes both
  its value and length validation. Percent-encoded literal percent signs are
  decoded once. This follows [RFC 3986 section 2.4](https://www.rfc-editor.org/rfc/rfc3986#section-2.4).
- A name of "0" passed through boolean tests as false. String val() checks now
  distinguish an empty path from a nonempty resource for internal and HTTP
  dispatch. The test checks both interfaces, including Swagger validation.
- The original PostgreSQL change uses recorded SQL column names for local and
  referenced columns. Its 5/6 regression suite and 9/56 SQL generation suite
  pass. This follow-up changes only that module's documentation, fixing invalid
  Doxygen member separators and literal generic type markup.
- RestHandler constructor comments named a nonexistent validator parameter;
  Swagger helper comments omitted opt_flags. These are corrected. Debug
  documentation dependency tags were stale: the current build/HttpServerUtil.tag
  and build/SqlUtil.tag contain setBodyCancel/createTempTable. The focused
  Doxygen configurations use those existing current dependency tags. No warning
  option was disabled; FAIL_ON_WARNINGS passes for all three modules.

Validation uses build-debug/qore --enable-debug with LD_LIBRARY_PATH=build-debug
and QORE_MODULE_DIR=qlib. The Debug build retains /usr as its install prefix.
All three actual qlib qmod targets were rebuilt with build/qcc, using the local
Debug library and current source. No installation or broad astparser build was
performed. Seven suites pass: RestPathEncoding 5/24, RestHandler 15/221,
PgsqlForeignConstraint 5/6, SqlUtilSelectGeneration 9/56, Swagger 25/785,
SwaggerDataProvider 3/25 and RestClient 19/129: **81 cases / 1246 assertions**.

Logs: /tmp/qore-uncommitted-<module>-compile.log and -docs.log for RestHandler,
PgsqlSqlUtilBase and Swagger; /tmp/qore-uncommitted-<suite-directory>-<suite>.log
for the seven suites; /tmp/qore-uncommitted-rest-schema-before.log retains the
pre-fix value mismatch. Test subprocesses have 180-second deadlines, requests
have 5-second deadlines, and listener startup supplies readiness synchronously.
No sleeps or polling were introduced. No push was performed.

| Check | Status | Evidence |
| --- | --- | --- |
| 1. Entry exists in `doxygen/lang/120_modules.dox.tmpl` (for modules in the Qore repo; N/A for external module repos) | N/A | Existing single-file modules; no new module, separated .qc file, directory layout or build registration. |
| 2. Entry exists in `doxygen/lang/900_release_notes.dox.tmpl` (for modules in the Qore repo; external modules have release notes in their .qm) | Pass | RestHandler and Swagger release notes describe the value and decoding changes; PgsqlSqlUtilBase already documents foreign-key quoting in a29e8a63e. |
| 3. `qore_user_module()` or `qore_external_user_module()` call in `CMakeLists.txt` | Pass | All three existing modules remain registered in CMake. Their actual qlib qmod targets were rebuilt directly with qcc, avoiding unrelated binary-module builds. |
| 4. Module added to QMOD list in `CMakeLists.txt` | N/A | Existing single-file modules; no new module, separated .qc file, directory layout or build registration. |
| 5. `.qm` file has `@section <lowercasemodname>intro` as first doc section — **must be all lowercase** (e.g., `avrodataproviderintro`, not `AvroDataProviderintro`) | Pass | Existing first sections remain resthandlerintro, swaggerintro and pgsqlsqlutilbaseintro. |
| 6. `%modern` in `.qm` file — no redundant `%new-style`, `%require-types`, `%strict-args`, `%enable-all-warnings` | Pass | All three modules use %modern. No parse directives are added to production modules. |
| 7. No parse directives (`%requires`, `%modern`, `%new-style`) in separated `.qc` files (check OUTSIDE of `@code` blocks only) | N/A | Existing single-file modules; no new module, separated .qc file, directory layout or build registration. |
| 8. No `%include` usage (deprecated for modules) | Pass | No deprecated %include is introduced. |
| 9. Copyright 2026 on all new files | Pass | The authored audit and modified modules/test carry 2026 copyright statements. |
| 10. Directory layout: `.qm` inside `qlib/<ModuleName>/` directory (not at `qlib/<ModuleName>.qm` for multi-file modules) | N/A | Existing single-file modules; no new module, separated .qc file, directory layout or build registration. |
| 11. No second `.qm` for the same module at `qlib/<ModuleName>.qm` | N/A | Existing single-file modules; no new module, separated .qc file, directory layout or build registration. |
| 12. `ns=Qore::XX` matches the QoreNamespace constructor path | N/A | No C++/QPP changes, native I/O, native loops, allocation ownership or method flags. Valgrind is not required for this Qore-only change. |
| 13. `%modern` directive present | Pass | RestPathEncoding.qtest uses %modern. |
| 14. Executable permission set (`chmod +x`) | Pass | RestPathEncoding.qtest retains executable mode 0755. |
| 15. Uses %prepend-module-path  before %requires for in-repo modules (Qore and Qore modules only; not Qorus) | Pass | The test prepends local qlib, then requires in-repository QUnit, HttpServer, RestHandler, RestClient and Swagger by relative paths. |
| 16. External module dependencies use `%try-module` — except modules delivered with the project itself (Qore ex: DataProvider, ConnectionProvider, QUnit, etc.) which use hard `%requires` | Pass | The only added binary requirement is json, delivered by Qore itself. No external binary dependency is added. |
| 17. No filesystem operations (fopen, open, creat, unlink, remove, rename, mkdir, rmdir, stat, chmod) without sandbox checks | N/A | No C++/QPP changes, native I/O, native loops, allocation ownership or method flags. Valgrind is not required for this Qore-only change. |
| 18. No network operations (connect, bind, socket, getaddrinfo, gethostbyname) without sandbox checks | N/A | No C++/QPP changes, native I/O, native loops, allocation ownership or method flags. Valgrind is not required for this Qore-only change. |
| 19. If filesystem/network ops exist, verify `QoreSandboxManagerHelper` usage | N/A | No C++/QPP changes, native I/O, native loops, allocation ownership or method flags. Valgrind is not required for this Qore-only change. |
| 20. No `File::`, `Dir::`, `Socket::`, `HTTPClient::` usage without justification | Pass | Production changes add no I/O. HTTP integration tests deliberately use loopback ephemeral listeners, 5-second connect/transfer deadlines and on_exit/globalTearDown server cleanup. |
| 21. All `for`/`while` loops that could iterate >100 times have `qore_check_cancel()` checks | N/A | No C++/QPP changes, native I/O, native loops, allocation ownership or method flags. Valgrind is not required for this Qore-only change. |
| 22. Uses `qore_check_cancel()` (NOT deprecated `qore_check_io_interrupt()`) | N/A | No C++/QPP changes, native I/O, native loops, allocation ownership or method flags. Valgrind is not required for this Qore-only change. |
| 23. Check frequency: every 100 iterations for tight loops, every 10 for expensive iterations | N/A | No C++/QPP changes, native I/O, native loops, allocation ownership or method flags. Valgrind is not required for this Qore-only change. |
| 24. No blocking operations without cancellation support | N/A | No C++/QPP changes, native I/O, native loops, allocation ownership or method flags. Valgrind is not required for this Qore-only change. |
| 25. Every action has `display_name`, `short_desc` (plain text, <80 chars), `desc` (markdown) | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 26. Every action has `options` populated via `getActionOptionFromFields()` — without this, the action shows an empty, unusable form | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 27. Every action has `output_type` set to a typed data type constant (e.g., `MyResponseDataType`) — not omitted | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 28. DPAT_API actions: provider has `"supports_request": True` and implements `doRequestImpl()` | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 29. DPAT_FIND actions: every option exists in `SearchOptions`, `getRecordTypeImpl()` returns `*hash<string, AbstractDataField>` | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 30. Scheme-based apps (with `"scheme"` in registerApp): actions use `"path"` and do NOT use `"cls"` — having both `scheme` and `cls` causes a runtime error | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 31. Single-key hash slices use trailing comma: `Fields{"key",}` (without trailing comma, `Fields{"key"}` returns the value, not a hash) | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 32. **Typed data type classes exist** for request and response types — inherit `HashDataType`, have `const Fields` hash, call `addQoreFields(Fields)` in constructor, export public constant at bottom (e.g., `public const MyDataType = new MyDataType();`) | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 33. Request/input types use `public` Fields (enables `ClassName::Fields` in action registration) | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 34. Response/output types use `private` Fields | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 35. Each field in data types has `display_name`, `type`, and `desc` (markdown-formatted) | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 36. Input fields have `example_value` where useful (string fields, endpoint URIs, SQL queries, etc.) | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 37. Fields with finite allowed values use `allowed_values` with `AllowedValueInfo` containing both `value` and `display_name` (Title Case, human-readable) — never bare values, never described only in text | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 38. Password/secret fields have `"sensitive": True` | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 39. `groups` uses `AppGroup` enum values from `qlib/DataProvider/AppGroup.qc` | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 40. App `logo` stored as separate file, loaded at module level in `Priv` namespace | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 41. App `desc` uses markdown: bullet list of capabilities, links to project website, business-language explanation of value | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 42. `display_name` is user-friendly ("Apache Avro" not "avro") | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 43. `short_desc` is plain text, under 80 chars, single sentence — no markdown | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 44. `desc` uses markdown: backticks for code/field refs (`` `field_name` ``, `` `True` ``, `` `pdf` ``), `\n\n` for paragraphs, `- ` bullet lists for enumerations, `**bold**` for caveats | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 45. Descriptions use plain business language relating to common challenges — not just technical "what" but "why" and "when to use" | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 46. No bare `True`/`False`/`NOTHING` — must be backtick-wrapped in `desc` | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 47. No bare field/option names in prose — must use backticks | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 48. Long descriptions (>500 chars) use bold section headers and bullet lists | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 49. **Factory registration in Qore repo**: every factory name registered in `qlib/DataProvider/DataProvider.qc` → `FactoryMap` (without this, module loads but doesn't appear in Qorus apps) | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 50. **`getRecordTypeImpl()` signature**: must be `private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options)` — NOT returning `*AbstractDataProviderType` | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 51. **Dependency JARs committed** (for JNI modules): JAR files in `qlib/*/jar/` may be gitignored — use `git add -f` to ensure they're tracked, otherwise CI compilation fails | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 52. JAR install rules in CMakeLists.txt for all dependency JARs | N/A | No DataProvider app, action, field type, factory, options, presentation catalog or JAR changes in this commit. Concurrent Fireworks work is excluded. |
| 53. **No workarounds**: No TODOs, FIXMEs, stubs, or partially-implemented features | Pass | Decoding is performed on extracted parameter data before conversion/validation; string presence uses val() instead of boolean conversion. No suppression, fixture special case, TODO or stub is introduced. |
| 54. **Exception safety**: C++ uses `ReferenceHolder` for Qore allocations, `std::unique_ptr` for C++ allocations, `*xsink` checked after every fallible operation | Pass | All values are managed Qore values. Schema failures propagate through the existing error path; the new local server stops via on_exit even if a request/assertion throws. |
| 55. **Thread safety**: All mutable shared state protected by `std::lock_guard<std::mutex>` or documented as immutable-after-construction | Pass | New work is local to each request/test. No new mutable global state or shared configuration is introduced. |
| 56. **Type safety**: Strongly-typed `code<return(args)>` instead of untyped `code`; `static_cast` instead of C casts; typed hashdecls for results; enums where appropriate | Pass | Existing typed URI/query hashes and HTTP response hashdecls are retained. Parameter values remain auto because schema conversion selects string/int/number/bool; resource presence tests operate on strings. |
| 57. **Performance**: No O(n²) where O(n) is possible; no unnecessary copies; coordinate descent uses incremental residuals not full matrix multiply | Pass | A single linear URL decode is added per already-extracted path parameter. Presence checks are constant time. Existing matching/SQL generation algorithms are unchanged. |
| 58. **Error handling**: All inputs validated (dimensions, empty data, unfitted models); C++ I/O handles EAGAIN/EINTR if applicable | Pass | The test asserts missing-subclass HTTP 404, schema-length HTTP 400 with SCHEMA-VALIDATION-ERROR, and UNKNOWN-REST-METHOD. Exact percent literals are decoded once; zero-valued names remain valid nonempty resources. |
| 59. **Documentation**: Doxygen `@param`, `@return`, `@throw` on all public methods; `@par Example` with realistic business scenarios; `@note` for important caveats | Pass | Public internal-dispatch documentation includes an encoded account identifier example. Release notes and constructor notes are corrected, SQL cross-references and hash markup are repaired, and Swagger option parameters are documented. Qdx plus Doxygen FAIL_ON_WARNINGS passes for all three modules. |
| 60. **QPP flags**: `[flags=CONSTANT]` on methods that never throw; `[flags=RET_VALUE_ONLY]` on methods that throw but have no side effects | N/A | No C++/QPP changes, native I/O, native loops, allocation ownership or method flags. Valgrind is not required for this Qore-only change. |
| 61. **Security**: No user-controlled format strings; no buffer overflows; bounds checking on array indices; no credentials in code | Pass | No credentials, user-controlled format string or unchecked buffer operations are introduced. Path/query boundaries are established before data decoding; negative routing and query tests preserve separation. |
| 62. **Correctness**: Algorithms verified against reference implementations; edge cases tested (empty data, single sample, all-zero features) | Pass | Seven affected suites pass with --enable-debug: 81 cases / 1246 assertions, including live HTTP plus Swagger schema checks, provider tests and SQL generation. The schema mismatch was reproduced before its fix; zero-path and encoded-length cases now pass. No skips or warnings occurred. |
