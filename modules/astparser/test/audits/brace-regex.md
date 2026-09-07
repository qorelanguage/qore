# Astparser brace-regex audit

Copyright (C) 2026 Qore Technologies, s.r.o.

Scope: the tree-sitter grammar and stateless C scanner, generated artifacts,
CMake compilation/installation and standalone test target, Qore regression tests,
module/core release notes and `design/astparser-brace-regex.md`.

Review found a lexical-context regression in the supplied implementation:
`m{"abc"}` and `x{"abc"}` were parsed as regexes, while `s{"abc"}` and
`tr{"abc"}` produced errors. Regex forms now appear only after their operators
and in switch cases, following the runtime scanner. Negative matching and switch
cases accept match expressions. Named regex-operation nodes and their complete
source text are preserved.

Expanded tests also exposed a core substring-view crash: `QoreString::substr()`
read the null owned buffer of a view. Prerequisite commit `18228a80e` makes the
four substring helpers read `effective_buf()` and tests UTF-8/single-byte views,
negative/out-of-range offsets, nested views and parent lifetimes.

Validation:
- Tree-sitter 0.26.8 regeneration with Node 24 completes without warnings.
- The standalone grammar integration suite passes 50 checks under Valgrind;
  all 11,542 allocations are freed, with zero errors and zero suppressions.
- Debug Qore with `-b --enable-debug`: brace-regex.qtest passes 4 cases / 215 assertions;
  astparser.qtest passes 111 cases / 893 assertions; Qdx/AstProcessor.qtest passes
  23 cases / 61 assertions.
- Prerequisite Qore tests pass 2 cases / 18 assertions for substring views and
  33 cases / 971 assertions for strings. The C++ substring-view test reports zero
  Valgrind errors and zero definitely/indirectly/possibly lost bytes.
- The full Qore brace suite also passes all 215 assertions under Valgrind, with
  zero lost bytes and two PCRE2 JIT uninitialized-condition diagnostics. Valgrind
  exits 99 with `--error-exitcode=99`; this is not a clean full-process Memcheck run.
  Its debug-information reader also warns about DW_AT_abstract_origin in libqore.
- Module documentation builds with Doxygen `WARN_AS_ERROR=YES`; strict qpp
  preprocessing of the module and core release notes passes without warnings.

The Qore/QUnit process also exercises PCRE2 JIT. The full-process run reports
the same two uninitialized-condition diagnostics from generated PCRE2 code, independently
reproduced with the system PCRE2 library without Qore or astparser. Valgrind's
[FAQ section 5.4](https://valgrind.org/docs/manual/faq.html) documents this class
of PCRE2 JIT diagnostic. The standalone test covers the changed parser/scanner
without that dependency. No suppression or JIT-disable setting is used.

The generated parser diff is mechanical. Review focuses on the grammar/scanner
and reproducible generation. Every audit-changes checklist item is assessed
below; DataProvider, new-module and QPP API checks are inapplicable to this scope.

|!Check|!Status|!Evidence
| 1. Entry exists in `doxygen/lang/120_modules.dox.tmpl` (for modules in the Qore repo; N/A for external module repos) | N/A | No new Qore module or public QPP class; existing module registration and namespaces remain in place.
| 2. Entry exists in `doxygen/lang/900_release_notes.dox.tmpl` (for modules in the Qore repo; external modules have release notes in their .qm) | Pass | Astparser module release notes describe all four brace operations; no new core module requiring a module-list entry.
| 3. `qore_user_module()` or `qore_external_user_module()` call in `CMakeLists.txt` | Pass | Existing astparser target now compiles scanner.c beside parser.c and installs both reusable grammar sources.
| 4. Module added to QMOD list in `CMakeLists.txt` | N/A | No new Qore module or public QPP class; existing module registration and namespaces remain in place.
| 5. `.qm` file has `@section <lowercasemodname>intro` as first doc section — **must be all lowercase** (e.g., `avrodataproviderintro`, not `AvroDataProviderintro`) | N/A | No new Qore module or public QPP class; existing module registration and namespaces remain in place.
| 6. `%modern` in `.qm` file — no redundant `%new-style`, `%require-types`, `%strict-args`, `%enable-all-warnings` | N/A | No Qore module source changed.
| 7. No parse directives (`%requires`, `%modern`, `%new-style`) in separated `.qc` files (check OUTSIDE of `@code` blocks only) | N/A | No separated Qore source changed.
| 8. No `%include` usage (deprecated for modules) | Pass | No %include added.
| 9. Copyright 2026 on all new files | Pass | All newly authored source, tests and review/design records have 2026 copyright notices; existing third-party notices remain intact.
| 10. Directory layout: `.qm` inside `qlib/<ModuleName>/` directory (not at `qlib/<ModuleName>.qm` for multi-file modules) | N/A | No new Qore module or public QPP class; existing module registration and namespaces remain in place.
| 11. No second `.qm` for the same module at `qlib/<ModuleName>.qm` | N/A | No new Qore module or public QPP class; existing module registration and namespaces remain in place.
| 12. `ns=Qore::XX` matches the QoreNamespace constructor path | N/A | No new Qore module or public QPP class; existing module registration and namespaces remain in place.
| 13. `%modern` directive present | Pass | The new Qore regression file explicitly declares %modern.
| 14. Executable permission set (`chmod +x`) | Pass | The new .qtest has mode 100755 (filesystem mode 755 checked).
| 15. Uses %prepend-module-path  before %requires for in-repo modules (Qore and Qore modules only; not Qorus) | Pass | Both tests prepend core qlib before relative core requires; binary artifact selection is left to QORE_MODULE_DIR so the Debug module is actually tested.
| 16. External module dependencies use `%try-module` — except modules delivered with the project itself (Qore ex: DataProvider, ConnectionProvider, QUnit, etc.) which use hard `%requires` | Pass | The tested binary module belongs to the repository and uses hard %requires; QUnit is a required core test dependency. No new optional external dependency.
| 17. No filesystem operations (fopen, open, creat, unlink, remove, rename, mkdir, rmdir, stat, chmod) without sandbox checks | Pass | External scanner consumes an existing TSLexer input; no filesystem call.
| 18. No network operations (connect, bind, socket, getaddrinfo, gethostbyname) without sandbox checks | Pass | No network call.
| 19. If filesystem/network ops exist, verify `QoreSandboxManagerHelper` usage | N/A | No external resource operation in the scanner.
| 20. No `File::`, `Dir::`, `Socket::`, `HTTPClient::` usage without justification | Pass | Tests parse/evaluate in-memory source with Program and AstParser; no production Qore I/O added.
| 21. All `for`/`while` loops that could iterate >100 times have `qore_check_cancel()` checks | N/A | This diff adds a standalone C tree-sitter scanner, not a Qore C++ execution loop. It has no ExceptionSink; source traversal is iterative, input-bounded and allocation-free.
| 22. Uses `qore_check_cancel()` (NOT deprecated `qore_check_io_interrupt()`) | N/A | No Qore C++ cancellation API changed or deprecated interrupt call introduced.
| 23. Check frequency: every 100 iterations for tight loops, every 10 for expensive iterations | N/A | The standalone C lexical callback has no Qore cancellation context; no Qore C++ loop added.
| 24. No blocking operations without cancellation support | Pass | No I/O, synchronization or other blocking operation; successful scanner iterations consume input.
| 25. Every action has `display_name`, `short_desc` (plain text, <80 chars), `desc` (markdown) | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 26. Every action has `options` populated via `getActionOptionFromFields()` — without this, the action shows an empty, unusable form | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 27. Every action has `output_type` set to a typed data type constant (e.g., `MyResponseDataType`) — not omitted | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 28. DPAT_API actions: provider has `"supports_request": True` and implements `doRequestImpl()` | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 29. DPAT_FIND actions: every option exists in `SearchOptions`, `getRecordTypeImpl()` returns `*hash<string, AbstractDataField>` | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 30. Scheme-based apps (with `"scheme"` in registerApp): actions use `"path"` and do NOT use `"cls"` — having both `scheme` and `cls` causes a runtime error | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 31. Single-key hash slices use trailing comma: `Fields{"key",}` (without trailing comma, `Fields{"key"}` returns the value, not a hash) | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 32. **Typed data type classes exist** for request and response types — inherit `HashDataType`, have `const Fields` hash, call `addQoreFields(Fields)` in constructor, export public constant at bottom (e.g., `public const MyDataType = new MyDataType();`) | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 33. Request/input types use `public` Fields (enables `ClassName::Fields` in action registration) | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 34. Response/output types use `private` Fields | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 35. Each field in data types has `display_name`, `type`, and `desc` (markdown-formatted) | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 36. Input fields have `example_value` where useful (string fields, endpoint URIs, SQL queries, etc.) | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 37. Fields with finite allowed values use `allowed_values` with `AllowedValueInfo` containing both `value` and `display_name` (Title Case, human-readable) — never bare values, never described only in text | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 38. Password/secret fields have `"sensitive": True` | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 39. `groups` uses `AppGroup` enum values from `qlib/DataProvider/AppGroup.qc` | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 40. App `logo` stored as separate file, loaded at module level in `Priv` namespace | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 41. App `desc` uses markdown: bullet list of capabilities, links to project website, business-language explanation of value | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 42. `display_name` is user-friendly ("Apache Avro" not "avro") | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 43. `short_desc` is plain text, under 80 chars, single sentence — no markdown | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 44. `desc` uses markdown: backticks for code/field refs (`` `field_name` ``, `` `True` ``, `` `pdf` ``), `\n\n` for paragraphs, `- ` bullet lists for enumerations, `**bold**` for caveats | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 45. Descriptions use plain business language relating to common challenges — not just technical "what" but "why" and "when to use" | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 46. No bare `True`/`False`/`NOTHING` — must be backtick-wrapped in `desc` | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 47. No bare field/option names in prose — must use backticks | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 48. Long descriptions (>500 chars) use bold section headers and bullet lists | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 49. **Factory registration in Qore repo**: every factory name registered in `qlib/DataProvider/DataProvider.qc` → `FactoryMap` (without this, module loads but doesn't appear in Qorus apps) | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 50. **`getRecordTypeImpl()` signature**: must be `private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options)` — NOT returning `*AbstractDataProviderType` | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 51. **Dependency JARs committed** (for JNI modules): JAR files in `qlib/*/jar/` may be gitignored — use `git add -f` to ensure they're tracked, otherwise CI compilation fails | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 52. JAR install rules in CMakeLists.txt for all dependency JARs | N/A | No DataProvider implementation, registration, field metadata, JNI dependency JAR or application packaging change in this increment.
| 53. **No workarounds**: No TODOs, FIXMEs, stubs, or partially-implemented features | Pass | Contextual grammar rules fix both missing brace syntax and the hash-access collision. No stub, suppression, JIT-disable setting or fixture-specific production branch was added.
| 54. **Exception safety**: C++ uses `ReferenceHolder` for Qore allocations, `std::unique_ptr` for C++ allocations, `*xsink` checked after every fallible operation | Pass | The scanner owns no allocations or persistent state. The standalone parser/scanner integration suite passes 50 checks under Valgrind with zero errors and all allocations freed.
| 55. **Thread safety**: All mutable shared state protected by `std::lock_guard<std::mutex>` or documented as immutable-after-construction | Pass | All scanner state is automatic; create returns NULL, serialize returns zero, and no mutable global or payload is used.
| 56. **Type safety**: Strongly-typed `code<return(args)>` instead of untyped `code`; `static_cast` instead of C casts; typed hashdecls for results; enums where appropriate | Pass | Typed C enum for four tokens; size_t nesting count, int32_t lexer characters and checked valid-symbol states. No C++ cast misuse.
| 57. **Performance**: No O(n²) where O(n) is possible; no unnecessary copies; coordinate descent uses incremental residuals not full matrix multiply | Pass | Linear source traversal with constant auxiliary storage; 1,024 nested braces and a 64 KiB replacement are tested without recursion.
| 58. **Error handling**: All inputs validated (dimensions, empty data, unfitted models); C++ I/O handles EAGAIN/EINTR if applicable | Pass | Requires one valid external token and complete balanced bodies. Rejects inter-part comments/FF/VT, illegal modifiers and non-match operations after !~; tests reuse after malformed input.
| 59. **Documentation**: Doxygen `@param`, `@return`, `@throw` on all public methods; `@par Example` with realistic business scenarios; `@note` for important caveats | Pass | No new public AST API. Durable design and module docs explain syntax, examples, artifacts, contextual recognition and the standalone memory-test target.
| 60. **QPP flags**: `[flags=CONSTANT]` on methods that never throw; `[flags=RET_VALUE_ONLY]` on methods that throw but have no side effects | N/A | No QPP function or method declaration changes in this repository increment.
| 61. **Security**: No user-controlled format strings; no buffer overflows; bounds checking on array indices; no credentials in code | Pass | TSLexer bounds source traversal; nesting cannot exceed input length, no raw buffer manipulation or user-controlled format string.
| 62. **Correctness**: Algorithms verified against reference implementations; edge cases tested (empty data, single sample, all-zero features) | Pass | Valid expressions are evaluated by the runtime and parsed by AstParser. Tests verify operation node types/text, hash lookup compatibility, invalid operators/modifiers, nested bodies, parser reuse and following declarations; see validation results above.

Reproduction commands (from the repository root):

```sh
cmake --build build-debug --target astparser astparser-brace-regex-test qore-substring-view-test -j4
LD_LIBRARY_PATH=build-debug QORE_MODULE_DIR=build-debug/modules/astparser:build-debug/modules/reflection:qlib build-debug/qore -b --enable-debug modules/astparser/test/brace-regex.qtest
LD_LIBRARY_PATH=build-debug QORE_MODULE_DIR=build-debug/modules/astparser:build-debug/modules/reflection:qlib build-debug/qore -b --enable-debug modules/astparser/test/astparser.qtest
LD_LIBRARY_PATH=build-debug QORE_MODULE_DIR=build-debug/modules/astparser:build-debug/modules/reflection:qlib build-debug/qore -b --enable-debug examples/test/qlib/Qdx/AstProcessor.qtest
valgrind --leak-check=full --error-exitcode=99 build-debug/modules/astparser/astparser-brace-regex-test
LD_LIBRARY_PATH=build-debug valgrind --leak-check=full --error-exitcode=99 build-debug/qore-substring-view-test
```
