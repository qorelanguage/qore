# AOT constructor lexical class context audit

Copyright (C) 2026 Qore Technologies, s.r.o.

Date: 2026-09-10. Scope: the constructor-context changes after `95bdf29f4` in
`lib/QoreAOTRuntime.cpp`, `lib/JITRuntime.cpp`, `include/qore/intern/QoreAOT.h`,
`AOTConstructorClassContext.qtest`, the AOT artifact design, release notes and this audit.
The full [audit skill](/home/david/.codex/skills/audit-changes/SKILL.md) and its
module-structure, sandboxing and cooperative-cancellation references were applied.
DataProvider registration/development guidance was checked for applicability.
All 62 items are resolved: **18 Pass / 44 N/A / 0 Fail**.

The reduced reproducer calls a static factory from an instance method whose
operands are stored in members. The private typed-list constructor was present
in its compiled module but filtered out using the unrelated caller's class.
An existing compiled reproducer now returns 7; constructor slots retain the
lexical owner, including a null owner for global functions. Both owned-argument
and borrowed-argument helpers install and restore that context. No artifact
format, constructor visibility or optimization setting changes.

Verification uses the isolated `build-debug` with `CMAKE_BUILD_TYPE=Debug` and
`CMAKE_INSTALL_PREFIX=/usr`, matching `/usr/bin/qore`. Its native source matches
the main checkout, including the latest container retyping/deferred-constant
fixes. No system installation or push was performed. The debug build and all
13 affected suites pass without warnings: nine QUnit suites report **83 cases /
319 assertions**, and four standalone AOT regression scripts also pass.
`AOTConstructorClassContext.qtest` covers AST/IR/JIT/tiered source execution and
source-stripped AOT `-O0`/`-O3`, with access-denial and constructor-failure negatives.

The compiled constructor driver passes Valgrind 3.27.1 with **zero errors** and
zero definitely/indirectly/possibly lost bytes, without suppressions. Commands use
`qore -b --enable-debug`; only Valgrind runs use the already approved
`QORE_PCRE2_NO_JIT=1`. The existing GCC DWARF `zero subprog` reader warning remains
explicitly tracked under XML plan P9; it is not a new runtime error or a hidden
suppression. The rebuilt WSDL module also passes its affected ordered-value suite
(**11 cases / 107 assertions**). Logs use `/tmp/wsdl-p4-08-core-` and
`/tmp/wsdl-p4-08-verified-`.

The exact final XML source also passes **108 suites / 1,087 cases / 54,665
reported assertions**, all four source modes, the independently validated
both-binding ordered-value matrix, and WSDL AOT Valgrind with zero errors/lost
bytes. Both-version diagnostic coverage retains every original source/input hash
and has no formerly passing stage regression. Strict coverage passes all **130
selected descriptions / 1,260 directions**. Its unchanged aggregate worker hit
its 60-second deadline during concurrent gates; the same complete command passed
in isolation (66.316 seconds including validators), with no deadline change,
filter, skip or production modification.

Debug library SHA-256: `610b3079097387c923e5018042be404d79b6ab4d2fbadae708d9e7197b370a2c`.

| Check | Status | Evidence |
| --- | --- | --- |
| 1. Entry exists in `doxygen/lang/120_modules.dox.tmpl` (for modules in the Qore repo; N/A for external module repos) | N/A | No installed module is added; the generated module is a temporary regression fixture. |
| 2. Entry exists in `doxygen/lang/900_release_notes.dox.tmpl` (for modules in the Qore repo; external modules have release notes in their .qm) | Pass | The Qore 3.0 bug-fix release notes document lexical constructor access and exception behavior. |
| 3. `qore_user_module()` or `qore_external_user_module()` call in `CMakeLists.txt` | N/A | No installed module, build target or artifact kind is added. |
| 4. Module added to QMOD list in `CMakeLists.txt` | N/A | No QMOD registration changes. |
| 5. `.qm` file has `@section <lowercasemodname>intro` as first doc section — **must be all lowercase** (e.g., `avrodataproviderintro`, not `AvroDataProviderintro`) | N/A | No installed module documentation section is added. |
| 6. `%modern` in `.qm` file — no redundant `%new-style`, `%require-types`, `%strict-args`, `%enable-all-warnings` | Pass | The new qtest and generated source/module scenarios use %modern; no redundant directives. |
| 7. No parse directives (`%requires`, `%modern`, `%new-style`) in separated `.qc` files (check OUTSIDE of `@code` blocks only) | N/A | No separated .qc source is changed. |
| 8. No `%include` usage (deprecated for modules) | Pass | No deprecated %include usage is introduced. |
| 9. Copyright 2026 on all new files | Pass | The new test and audit carry copyright 2026; changed native files retain their existing 2026 notices. |
| 10. Directory layout: `.qm` inside `qlib/<ModuleName>/` directory (not at `qlib/<ModuleName>.qm` for multi-file modules) | N/A | No installed module layout changes. |
| 11. No second `.qm` for the same module at `qlib/<ModuleName>.qm` | N/A | No duplicate installed module is introduced. |
| 12. `ns=Qore::XX` matches the QoreNamespace constructor path | N/A | No QPP class is added. |
| 13. `%modern` directive present | Pass | The executable test and each generated program/module use %modern, and the child Program uses PO_MODERN. |
| 14. Executable permission set (`chmod +x`) | Pass | AOTConstructorClassContext.qtest is mode 0755. |
| 15. Uses %prepend-module-path  before %requires for in-repo modules (Qore and Qore modules only; not Qorus) | Pass | The test prepends repository qlib before relative QUnit.qm and FsUtil.qm requirements. |
| 16. External module dependencies use `%try-module` — except modules delivered with the project itself (Qore ex: DataProvider, ConnectionProvider, QUnit, etc.) which use hard `%requires` | Pass | QUnit and FsUtil are project modules with hard relative requirements; no external module dependency is added. |
| 17. No filesystem operations (fopen, open, creat, unlink, remove, rename, mkdir, rmdir, stat, chmod) without sandbox checks | N/A | Changed native statements perform no filesystem operations. |
| 18. No network operations (connect, bind, socket, getaddrinfo, gethostbyname) without sandbox checks | N/A | Changed native statements perform no network operations. |
| 19. If filesystem/network ops exist, verify `QoreSandboxManagerHelper` usage | N/A | No native I/O operation is introduced. |
| 20. No `File::`, `Dir::`, `Socket::`, `HTTPClient::` usage without justification | Pass | Test-only file access creates module/drivers and captures bounded subprocess output inside an owned TmpDir. Cleanup is automatic. |
| 21. All `for`/`while` loops that could iterate >100 times have `qore_check_cancel()` checks | N/A | The changed native paths add no loop or iterative work; existing loader iteration is unchanged. |
| 22. Uses `qore_check_cancel()` (NOT deprecated `qore_check_io_interrupt()`) | N/A | No cancellation API call is introduced or replaced. |
| 23. Check frequency: every 100 iterations for tight loops, every 10 for expensive iterations | N/A | No new loop or blocking operation requires a cancellation interval. |
| 24. No blocking operations without cancellation support | N/A | The production change performs only metadata lookup and thread-local substitution; no blocking operation is introduced. |
| 25. Every action has `display_name`, `short_desc` (plain text, <80 chars), `desc` (markdown) | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 26. Every action has `options` populated via `getActionOptionFromFields()` — without this, the action shows an empty, unusable form | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 27. Every action has `output_type` set to a typed data type constant (e.g., `MyResponseDataType`) — not omitted | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 28. DPAT_API actions: provider has `"supports_request": True` and implements `doRequestImpl()` | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 29. DPAT_FIND actions: every option exists in `SearchOptions`, `getRecordTypeImpl()` returns `*hash<string, AbstractDataField>` | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 30. Scheme-based apps (with `"scheme"` in registerApp): actions use `"path"` and do NOT use `"cls"` — having both `scheme` and `cls` causes a runtime error | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 31. Single-key hash slices use trailing comma: `Fields{"key",}` (without trailing comma, `Fields{"key"}` returns the value, not a hash) | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 32. **Typed data type classes exist** for request and response types — inherit `HashDataType`, have `const Fields` hash, call `addQoreFields(Fields)` in constructor, export public constant at bottom (e.g., `public const MyDataType = new MyDataType();`) | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 33. Request/input types use `public` Fields (enables `ClassName::Fields` in action registration) | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 34. Response/output types use `private` Fields | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 35. Each field in data types has `display_name`, `type`, and `desc` (markdown-formatted) | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 36. Input fields have `example_value` where useful (string fields, endpoint URIs, SQL queries, etc.) | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 37. Fields with finite allowed values use `allowed_values` with `AllowedValueInfo` containing both `value` and `display_name` (Title Case, human-readable) — never bare values, never described only in text | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 38. Password/secret fields have `"sensitive": True` | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 39. `groups` uses `AppGroup` enum values from `qlib/DataProvider/AppGroup.qc` | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 40. App `logo` stored as separate file, loaded at module level in `Priv` namespace | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 41. App `desc` uses markdown: bullet list of capabilities, links to project website, business-language explanation of value | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 42. `display_name` is user-friendly ("Apache Avro" not "avro") | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 43. `short_desc` is plain text, under 80 chars, single sentence — no markdown | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 44. `desc` uses markdown: backticks for code/field refs (`` `field_name` ``, `` `True` ``, `` `pdf` ``), `\n\n` for paragraphs, `- ` bullet lists for enumerations, `**bold**` for caveats | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 45. Descriptions use plain business language relating to common challenges — not just technical "what" but "why" and "when to use" | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 46. No bare `True`/`False`/`NOTHING` — must be backtick-wrapped in `desc` | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 47. No bare field/option names in prose — must use backticks | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 48. Long descriptions (>500 chars) use bold section headers and bullet lists | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 49. **Factory registration in Qore repo**: every factory name registered in `qlib/DataProvider/DataProvider.qc` → `FactoryMap` (without this, module loads but doesn't appear in Qorus apps) | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 50. **`getRecordTypeImpl()` signature**: must be `private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options)` — NOT returning `*AbstractDataProviderType` | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 51. **Dependency JARs committed** (for JNI modules): JAR files in `qlib/*/jar/` may be gitignored — use `git add -f` to ensure they're tracked, otherwise CI compilation fails | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 52. JAR install rules in CMakeLists.txt for all dependency JARs | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 53. **No workarounds**: No TODOs, FIXMEs, stubs, or partially-implemented features | Pass | The missing lexical owner is restored at constructor dispatch. No optimization is disabled, visibility widened, or failure skipped; existing artifacts also exercise the corrected loader. |
| 54. **Exception safety**: C++ uses `ReferenceHolder` for Qore allocations, `std::unique_ptr` for C++ allocations, `*xsink` checked after every fallible operation | Pass | ClassOnlySubstitutionHelper restores both TLS class locations on every return. Existing argument ownership, exception-sink checks and constructor cleanup remain intact. Repeated throw/recovery and Valgrind pass. |
| 55. **Thread safety**: All mutable shared state protected by `std::lock_guard<std::mutex>` or documented as immutable-after-construction | Pass | Constructor slot metadata is initialized before context publication and subsequently read-only. Substitution modifies only thread-local state and restores the previous caller. |
| 56. **Type safety**: Strongly-typed `code<return(args)>` instead of untyped `code`; `static_cast` instead of C casts; typed hashdecls for results; enums where appropriate | Pass | The slot stores a const qore_class_private pointer derived from the existing typed variant/closure binding. Test lists, closures and exceptions have explicit types; no unsafe cast or ownership conversion. |
| 57. **Performance**: No O(n²) where O(n) is possible; no unnecessary copies; coordinate descent uses incremental residuals not full matrix multiply | Pass | One metadata assignment per constructor slot and a constant-cost RAII scope per invocation; no new traversal, allocation or data copy. |
| 58. **Error handling**: All inputs validated (dimensions, empty data, unfitted models); C++ I/O handles EAGAIN/EINTR if applicable | Pass | Unknown runtime argument types resolve through the existing checked overload path. Tests reject private access from another class and global code, propagate constructor exceptions, and verify subsequent successful calls and child Programs. |
| 59. **Documentation**: Doxygen `@param`, `@return`, `@throw` on all public methods; `@par Example` with realistic business scenarios; `@note` for important caveats | Pass | The implemented AOT design describes lexical ownership, null global context, both argument-ownership paths and unchanged artifact format. Release notes and executable factory examples accompany the fix; no public runtime API is added. |
| 60. **QPP flags**: `[flags=CONSTANT]` on methods that never throw; `[flags=RET_VALUE_ONLY]` on methods that throw but have no side effects | N/A | No DataProvider action, app, data type, registration or JNI dependency changes. |
| 61. **Security**: No user-controlled format strings; no buffer overflows; bounds checking on array indices; no credentials in code | Pass | Negative tests prove global/unrelated callers do not inherit private privileges. Slot bounds retain their assertions; subprocess arguments are individually shell-quoted; no credentials or unchecked buffers. |
| 62. **Correctness**: Algorithms verified against reference implementations; edge cases tested (empty data, single sample, all-zero features) | Pass | All 13 affected suites pass without warnings, including four source modes and source-stripped AOT O0/O3. Exact child values, closures, failure/reuse and child Programs agree. The constructor scenario has zero Valgrind errors/lost bytes; rebuilt WSDL passes 11 cases/107 assertions. |

Affected suites: `ir/AOTConstructorClassContext`, `ir/AOTConstructorCallRelocations`, `ir/AOTConstructorParameterReceiverLifetime`, `ir/AOTPrivateStaticClosureContext`, `ir/AOTModulePrivateClassStatic`, `ir/AOTGenericStaticMethodFastEntry`, `ir/AOTContextExpressionSlots`, `ir/AOTSplitStaticCallSlot`, `ir/AOTNativeClosure`, `ir/AOTStoredCapturedClosure`, `ir/AOTGenericCrossObjectFastEntry`, `ir/AOTDeferredConstRefTypedContainer`, `qore/vars/type_narrowing`.
