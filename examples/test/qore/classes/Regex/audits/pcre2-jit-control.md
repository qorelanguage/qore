# PCRE2 JIT test control audit

Copyright (C) 2026 Qore Technologies, s.r.o.

Scope: `lib/QoreRegexBase.cpp`, `include/qore/intern/QoreRegexBase.h`,
`examples/test/qore/classes/Regex/regex-jit-control.qtest`, and the environment
variable / 3.0 release notes. Unrelated active JinaDataProvider work and astparser
are excluded. The user explicitly requested committing these Qore changes to
`develop` on 2026-09-08, without pushing.

Implementation: exactly `QORE_PCRE2_NO_JIT=1` before first regex use skips PCRE2
JIT compilation and forces `PCRE2_NO_JIT` at match time with the installed library.
The cached bool is initialized once using C++'s synchronized local-static rules,
retains no environment pointer, and never changes after initialization. The
interpreter path does not allocate the optional JIT context or stack. Ordinary
execution and Qore's separate LLVM JIT retain their existing behavior.

Validation against the final source:

- Debug build: `which qore` is `/usr/bin/qore`; the existing `build-debug` cache
  uses Debug and prefix `/usr`. `cmake --build build-debug --target qore -j4`
  completes; no compiler warnings/errors. The generated documentation templates
  include the environment and release-note additions. Configuration separately
  reports unavailable optional ngtcp2 quictls/LibreSSL backends; Qore uses its
  configured OpenSSL backend, and this change does not alter that configuration.
- With `LD_LIBRARY_PATH=build-debug`, 137 test cases pass across Regex,
  RegexExtract, RegexSubst, RegexMatchIterator, regex_extract and regex operators.
  The class/function suites run in JIT and interpreter modes; the 19-case large
  subject performance/cancellation operator suite runs with production JIT.
- The new four-case test passes with unset, empty, 0, true and 01 values using
  JIT (12 assertions each), and exact 1 using the interpreter (17 assertions).
  The test distinguishes engines through PCRE2's interpreter-only depth limit,
  checks Unicode/negative/error/substitution/iteration paths, and verifies that
  changing the environment later cannot change the cached mode.
- `QORE_PCRE2_NO_JIT=1 QORE_TEST_REGEX_ENGINE=interpreter valgrind
  --error-exitcode=99 --leak-check=full --show-leak-kinds=all build-debug/qore
  -b --enable-debug examples/test/qore/classes/Regex/regex-jit-control.qtest`
  passes all four cases with zero memory errors and zero definite/indirect/
  possible loss, without suppressions. Valgrind's Debug-info reader emits one
  missing-DW_AT_abstract_origin warning for this GCC-built libqore; this is
  recorded separately from the clean memory result, not hidden or counted as a
  compiler/test warning. Regex semantics and memory checks complete normally.
- The installed-PCRE2 standalone reproduction also has zero valgrind errors and
  no allocations left after forcing PCRE2_NO_JIT (140 allocations/frees).
- Downstream XML literal, retained-value and XML-context tests pass with zero
  memory errors or lost allocations. The HTTP consumer still reports the
  previously isolated 384-byte possibly-lost glibc TLS allocation from core
  AsyncIoController/ThreadPool. That independent HTTP-lifetime investigation
  remains open in module-xml P2-23; this commit neither changes those files nor
  claims to clear the XML/HTTP memory acceptance gate.
- Full affected XML functional run: 482 cases pass. Catalog-backed WSDL survey
  and strict coverage are exactly unchanged (0 strict / 360 broad failures).

Logs: `/tmp/wsdl-p2-23-qore-build.log`, `/tmp/wsdl-p2-23-regex-tests.log`,
`/tmp/wsdl-p2-23-vg-regex-control{,-test}.log` and the module-xml P2-23 audit.

The full audit-changes skill was read and every item reviewed. Applicable design
references: `design/qore-module-structure.md`, `design/module-sandboxing-audit-guide.md`,
and `design/cooperative-cancellation.md`. No provider metadata is changed.

| Check | Status | Evidence |
| --- | --- | --- |
| 1. Entry exists in `doxygen/lang/120_modules.dox.tmpl` (for modules in the Qore repo; N/A for external module repos) | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 2. Entry exists in `doxygen/lang/900_release_notes.dox.tmpl` (for modules in the Qore repo; external modules have release notes in their .qm) | Pass | Environment and 3.0 release notes document scope, usage and defaults. |
| 3. `qore_user_module()` or `qore_external_user_module()` call in `CMakeLists.txt` | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 4. Module added to QMOD list in `CMakeLists.txt` | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 5. `.qm` file has `@section <lowercasemodname>intro` as first doc section — **must be all lowercase** (e.g., `avrodataproviderintro`, not `AvroDataProviderintro`) | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 6. `%modern` in `.qm` file — no redundant `%new-style`, `%require-types`, `%strict-args`, `%enable-all-warnings` | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 7. No parse directives (`%requires`, `%modern`, `%new-style`) in separated `.qc` files (check OUTSIDE of `@code` blocks only) | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 8. No `%include` usage (deprecated for modules) | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 9. Copyright 2026 on all new files | Pass | Runtime/header copyright already covers 2026; new test and audit have 2026 notices. |
| 10. Directory layout: `.qm` inside `qlib/<ModuleName>/` directory (not at `qlib/<ModuleName>.qm` for multi-file modules) | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 11. No second `.qm` for the same module at `qlib/<ModuleName>.qm` | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 12. `ns=Qore::XX` matches the QoreNamespace constructor path | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 13. `%modern` directive present | Pass | New qtest is executable, uses %modern, prepends local qlib and requires QUnit by a relative source path; no external binary dependencies. |
| 14. Executable permission set (`chmod +x`) | Pass | New qtest is executable, uses %modern, prepends local qlib and requires QUnit by a relative source path; no external binary dependencies. |
| 15. Uses %prepend-module-path  before %requires for in-repo modules (Qore and Qore modules only; not Qorus) | Pass | New qtest is executable, uses %modern, prepends local qlib and requires QUnit by a relative source path; no external binary dependencies. |
| 16. External module dependencies use `%try-module` — except modules delivered with the project itself (Qore ex: DataProvider, ConnectionProvider, QUnit, etc.) which use hard `%requires` | Pass | New qtest is executable, uses %modern, prepends local qlib and requires QUnit by a relative source path; no external binary dependencies. |
| 17. No filesystem operations (fopen, open, creat, unlink, remove, rename, mkdir, rmdir, stat, chmod) without sandbox checks | Pass | No new filesystem/network runtime operation. Internal environment read occurs once; test setenv/unsetenv use existing PROCESS-domain APIs. |
| 18. No network operations (connect, bind, socket, getaddrinfo, gethostbyname) without sandbox checks | Pass | No new filesystem/network runtime operation. Internal environment read occurs once; test setenv/unsetenv use existing PROCESS-domain APIs. |
| 19. If filesystem/network ops exist, verify `QoreSandboxManagerHelper` usage | Pass | No new filesystem/network runtime operation. Internal environment read occurs once; test setenv/unsetenv use existing PROCESS-domain APIs. |
| 20. No `File::`, `Dir::`, `Socket::`, `HTTPClient::` usage without justification | Pass | No new filesystem/network runtime operation. Internal environment read occurs once; test setenv/unsetenv use existing PROCESS-domain APIs. |
| 21. All `for`/`while` loops that could iterate >100 times have `qore_check_cancel()` checks | Pass | No new loop or blocking I/O; existing regex callers retain cancellation checks, tested by RegexSubst and production operator regressions. |
| 22. Uses `qore_check_cancel()` (NOT deprecated `qore_check_io_interrupt()`) | Pass | No new loop or blocking I/O; existing regex callers retain cancellation checks, tested by RegexSubst and production operator regressions. |
| 23. Check frequency: every 100 iterations for tight loops, every 10 for expensive iterations | Pass | No new loop or blocking I/O; existing regex callers retain cancellation checks, tested by RegexSubst and production operator regressions. |
| 24. No blocking operations without cancellation support | Pass | No new loop or blocking I/O; existing regex callers retain cancellation checks, tested by RegexSubst and production operator regressions. |
| 25. Every action has `display_name`, `short_desc` (plain text, <80 chars), `desc` (markdown) | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 26. Every action has `options` populated via `getActionOptionFromFields()` — without this, the action shows an empty, unusable form | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 27. Every action has `output_type` set to a typed data type constant (e.g., `MyResponseDataType`) — not omitted | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 28. DPAT_API actions: provider has `"supports_request": True` and implements `doRequestImpl()` | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 29. DPAT_FIND actions: every option exists in `SearchOptions`, `getRecordTypeImpl()` returns `*hash<string, AbstractDataField>` | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 30. Scheme-based apps (with `"scheme"` in registerApp): actions use `"path"` and do NOT use `"cls"` — having both `scheme` and `cls` causes a runtime error | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 31. Single-key hash slices use trailing comma: `Fields{"key",}` (without trailing comma, `Fields{"key"}` returns the value, not a hash) | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 32. **Typed data type classes exist** for request and response types — inherit `HashDataType`, have `const Fields` hash, call `addQoreFields(Fields)` in constructor, export public constant at bottom (e.g., `public const MyDataType = new MyDataType();`) | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 33. Request/input types use `public` Fields (enables `ClassName::Fields` in action registration) | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 34. Response/output types use `private` Fields | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 35. Each field in data types has `display_name`, `type`, and `desc` (markdown-formatted) | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 36. Input fields have `example_value` where useful (string fields, endpoint URIs, SQL queries, etc.) | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 37. Fields with finite allowed values use `allowed_values` with `AllowedValueInfo` containing both `value` and `display_name` (Title Case, human-readable) — never bare values, never described only in text | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 38. Password/secret fields have `"sensitive": True` | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 39. `groups` uses `AppGroup` enum values from `qlib/DataProvider/AppGroup.qc` | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 40. App `logo` stored as separate file, loaded at module level in `Priv` namespace | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 41. App `desc` uses markdown: bullet list of capabilities, links to project website, business-language explanation of value | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 42. `display_name` is user-friendly ("Apache Avro" not "avro") | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 43. `short_desc` is plain text, under 80 chars, single sentence — no markdown | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 44. `desc` uses markdown: backticks for code/field refs (`` `field_name` ``, `` `True` ``, `` `pdf` ``), `\n\n` for paragraphs, `- ` bullet lists for enumerations, `**bold**` for caveats | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 45. Descriptions use plain business language relating to common challenges — not just technical "what" but "why" and "when to use" | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 46. No bare `True`/`False`/`NOTHING` — must be backtick-wrapped in `desc` | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 47. No bare field/option names in prose — must use backticks | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 48. Long descriptions (>500 chars) use bold section headers and bullet lists | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 49. **Factory registration in Qore repo**: every factory name registered in `qlib/DataProvider/DataProvider.qc` → `FactoryMap` (without this, module loads but doesn't appear in Qorus apps) | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 50. **`getRecordTypeImpl()` signature**: must be `private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options)` — NOT returning `*AbstractDataProviderType` | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 51. **Dependency JARs committed** (for JNI modules): JAR files in `qlib/*/jar/` may be gitignored — use `git add -f` to ensure they're tracked, otherwise CI compilation fails | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 52. JAR install rules in CMakeLists.txt for all dependency JARs | N/A | No new module, QPP class, provider, app, factory, field metadata, or dependency packaging. |
| 53. **No workarounds**: No TODOs, FIXMEs, stubs, or partially-implemented features | Pass | The user requested programmatic PCRE2 JIT control. Uses the documented PCRE2_NO_JIT API; no alternate library build, suppression or stub. |
| 54. **Exception safety**: C++ uses `ReferenceHolder` for Qore allocations, `std::unique_ptr` for C++ allocations, `*xsink` checked after every fallible operation | Pass | New helper allocates nothing and stores only a bool; existing PCRE2 match errors return through the unchanged exception mapping. Negative tests and valgrind pass. |
| 55. **Thread safety**: All mutable shared state protected by `std::lock_guard<std::mutex>` or documented as immutable-after-construction | Pass | Thread-safe static initialization, immutable bool afterward, no environment pointer retained; environment-mutation test verifies lifetime semantics. |
| 56. **Type safety**: Strongly-typed `code<return(args)>` instead of untyped `code`; `static_cast` instead of C casts; typed hashdecls for results; enums where appropriate | Pass | Boolean configuration and existing typed PCRE2 flags. Typed Qore lists/hashes and exception info; no new casts or untyped callbacks. |
| 57. **Performance**: No O(n²) where O(n) is possible; no unnecessary copies; coordinate descent uses incremental residuals not full matrix multiply | Pass | Cached branch only; normal JIT still enabled. Interpreter mode has explicitly documented resource/performance differences; production large-input tests pass. |
| 58. **Error handling**: All inputs validated (dimensions, empty data, unfitted models); C++ I/O handles EAGAIN/EINTR if applicable | Pass | Only exact 1 disables JIT; unset/empty/0/true/01 tested. Existing invalid-pattern and match-error paths remain correct. |
| 59. **Documentation**: Doxygen `@param`, `@return`, `@throw` on all public methods; `@par Example` with realistic business scenarios; `@note` for important caveats | Pass | Environment table, internal helper contract, invocation example, scope and release notes are updated; generated templates build. |
| 60. **QPP flags**: `[flags=CONSTANT]` on methods that never throw; `[flags=RET_VALUE_ONLY]` on methods that throw but have no side effects | N/A | No QPP method signature, functional domain or flags changed. |
| 61. **Security**: No user-controlled format strings; no buffer overflows; bounds checking on array indices; no credentials in code | Pass | No secret output, user format strings or buffers added. Test restores the original process environment setting. |
| 62. **Correctness**: Algorithms verified against reference implementations; edge cases tested (empty data, single sample, all-zero features) | Pass | Engine selection is independently observable via PCRE2 depth limits; six configuration values, matching variants, errors, immutable configuration and downstream use pass. Separate HTTP/TLS finding remains explicitly outside this commit. |

Reviewed source SHA-256:

- `lib/QoreRegexBase.cpp`: `392be57a6ef69285c2cfc47231a55659657adfa0adbc0c82e78befbc5d974383`
- `include/qore/intern/QoreRegexBase.h`: `2fce4e42c1d62b8517e0fdaeb42cae93eeee65148c932ae357a23a741c710ef4`
- `examples/test/qore/classes/Regex/regex-jit-control.qtest`: `f4e1be24211bd5a47b50c877731b0b0b76df9536649993bf192cd2492ad69979`
- `doxygen/lang/110_environment_variables.dox.tmpl`: `e8cf8602fe40572655e8043c5810d730fa4a5d27d58173fa19b04ee64a49e7f4`
- `doxygen/lang/900_release_notes.dox.tmpl`: `7ff33574d3d1841a1a97e2ce1c8e5bab6c34a60204d13c2126795b81e8caf08b`
