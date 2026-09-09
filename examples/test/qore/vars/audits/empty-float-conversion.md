# Empty string-to-float conversion audit

Copyright (C) 2026 Qore Technologies, s.r.o.

Scope: the local result initialization in `lib/QoreLib.cpp::q_strtod()`, the
Qore 3.0 release-note bullet and the new executable conversion regression.
All 62 audit-changes checklist items were reviewed, including the applicable
module-structure, sandboxing, cancellation and provider guide sections.
No module, public API, parser or astparser source changes.

`std::istringstream` constructs a sentry before numeric extraction. On empty or
whitespace-only input that sentry reaches EOF and can return before assigning
the destination. The converter previously returned an uninitialized local
double. The new regression fails on the baseline with a softfloat argument
value of approximately `6.95e-310` where zero is required. A separate core-only
Valgrind probe traces undefined reads to the local allocation in `q_strtod()`.
Initializing the result to `0.0` fixes that path while preserving numeric-prefix,
malformed nonempty input, locale and signed-zero behavior.

Validation uses `/tmp/wsdl-core-date/build-debug`, a Debug build with `/usr`
prefix matching `/usr/bin/qore`. Its `QoreLib.cpp` is byte-identical to the main
checkout's tested source. The build has no compiler warnings/errors; no main
checkout build, installation or push was performed.

- Five affected suites (`float-empty-conversion`, `float`, `infnan`, `numbers`,
  `number-round-trip`) pass in AST, IR, JIT and tiered execution: 20 suite runs,
  116 cases and 3628 assertions. The new regression has 3 cases / 322 assertions
  per mode, covering explicit casts and softfloat argument coercion.
- Valgrind runs the new AST and IR regression plus the existing AST float suite,
  with `qore -b --enable-debug`: 10 cases / 703 assertions, zero errors and zero
  definite, indirect or possible loss; no suppressions.
- Valgrind uses the previously authorized `QORE_PCRE2_NO_JIT=1` setting. The initial
  JIT-enabled run reproduced the already source-adjudicated PCRE2 call-stack-regex
  conditional read. The existing Valgrind DW_AT_abstract_origin reader warning
  remains a P9 environment finding; this audit does not claim it is resolved.
- All 64 affected XML suites pass 742 cases / 15319 assertions with the rebuilt
  runtime. Both-version survey and strict coverage match P3-19 outside version
  metadata. Full XML Python discovery remains the separate XML commit gate.

Evidence: `/tmp/wsdl-p3-21-core-empty-{baseline,build,tests}.log`,
`/tmp/wsdl-p3-21-empty-float-valgrind.log` (baseline), and
`/tmp/wsdl-p3-21-core-empty-valgrind-final-*.log` (fixed).
The new runtime SHA-256 is
`c8b0739f796b93c0f056cbf2a37050d6902de45c3e9361ed3565754ac5549afb`.

| Check | Status | Evidence |
| --- | --- | --- |
| 1. Entry exists in `doxygen/lang/120_modules.dox.tmpl` (for modules in the Qore repo; N/A for external module repos) | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 2. Entry exists in `doxygen/lang/900_release_notes.dox.tmpl` (for modules in the Qore repo; external modules have release notes in their .qm) | Pass | Qore 3.0 release notes describe deterministic zero conversion for empty and whitespace-only input. |
| 3. `qore_user_module()` or `qore_external_user_module()` call in `CMakeLists.txt` | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 4. Module added to QMOD list in `CMakeLists.txt` | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 5. `.qm` file has `@section <lowercasemodname>intro` as first doc section — **must be all lowercase** (e.g., `avrodataproviderintro`, not `AvroDataProviderintro`) | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 6. `%modern` in `.qm` file — no redundant `%new-style`, `%require-types`, `%strict-args`, `%enable-all-warnings` | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 7. No parse directives (`%requires`, `%modern`, `%new-style`) in separated `.qc` files (check OUTSIDE of `@code` blocks only) | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 8. No `%include` usage (deprecated for modules) | Pass | No deprecated module include directive is introduced. |
| 9. Copyright 2026 on all new files | Pass | The new test and audit carry 2026 copyright; QoreLib.cpp already carries the current notice. |
| 10. Directory layout: `.qm` inside `qlib/<ModuleName>/` directory (not at `qlib/<ModuleName>.qm` for multi-file modules) | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 11. No second `.qm` for the same module at `qlib/<ModuleName>.qm` | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 12. `ns=Qore::XX` matches the QoreNamespace constructor path | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 13. `%modern` directive present | Pass | The new qtest uses %modern. |
| 14. Executable permission set (`chmod +x`) | Pass | float-empty-conversion.qtest is executable (0755). |
| 15. Uses %prepend-module-path  before %requires for in-repo modules (Qore and Qore modules only; not Qorus) | Pass | The test prepends local qlib before the explicit relative QUnit.qm requirement. |
| 16. External module dependencies use `%try-module` — except modules delivered with the project itself (Qore ex: DataProvider, ConnectionProvider, QUnit, etc.) which use hard `%requires` | Pass | The test uses only builtin facilities and repository QUnit; no external dependency. |
| 17. No filesystem operations (fopen, open, creat, unlink, remove, rename, mkdir, rmdir, stat, chmod) without sandbox checks | Pass | The changed converter uses an in-memory string stream and adds only local scalar initialization; no filesystem operation. |
| 18. No network operations (connect, bind, socket, getaddrinfo, gethostbyname) without sandbox checks | Pass | No network operation is present in the changed converter or regression. |
| 19. If filesystem/network ops exist, verify `QoreSandboxManagerHelper` usage | N/A | No native filesystem/network operation requiring a sandbox helper. |
| 20. No `File::`, `Dir::`, `Socket::`, `HTTPClient::` usage without justification | Pass | The Qore regression uses no file, directory, socket or HTTP client API. |
| 21. All `for`/`while` loops that could iterate >100 times have `qore_check_cancel()` checks | N/A | No C++ loop or cancellation call is added or changed. |
| 22. Uses `qore_check_cancel()` (NOT deprecated `qore_check_io_interrupt()`) | N/A | No C++ loop or cancellation call is added or changed. |
| 23. Check frequency: every 100 iterations for tight loops, every 10 for expensive iterations | N/A | No C++ loop or cancellation call is added or changed. |
| 24. No blocking operations without cancellation support | Pass | No blocking operation or new loop is introduced. |
| 25. Every action has `display_name`, `short_desc` (plain text, <80 chars), `desc` (markdown) | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 26. Every action has `options` populated via `getActionOptionFromFields()` — without this, the action shows an empty, unusable form | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 27. Every action has `output_type` set to a typed data type constant (e.g., `MyResponseDataType`) — not omitted | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 28. DPAT_API actions: provider has `"supports_request": True` and implements `doRequestImpl()` | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 29. DPAT_FIND actions: every option exists in `SearchOptions`, `getRecordTypeImpl()` returns `*hash<string, AbstractDataField>` | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 30. Scheme-based apps (with `"scheme"` in registerApp): actions use `"path"` and do NOT use `"cls"` — having both `scheme` and `cls` causes a runtime error | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 31. Single-key hash slices use trailing comma: `Fields{"key",}` (without trailing comma, `Fields{"key"}` returns the value, not a hash) | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 32. **Typed data type classes exist** for request and response types — inherit `HashDataType`, have `const Fields` hash, call `addQoreFields(Fields)` in constructor, export public constant at bottom (e.g., `public const MyDataType = new MyDataType();`) | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 33. Request/input types use `public` Fields (enables `ClassName::Fields` in action registration) | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 34. Response/output types use `private` Fields | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 35. Each field in data types has `display_name`, `type`, and `desc` (markdown-formatted) | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 36. Input fields have `example_value` where useful (string fields, endpoint URIs, SQL queries, etc.) | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 37. Fields with finite allowed values use `allowed_values` with `AllowedValueInfo` containing both `value` and `display_name` (Title Case, human-readable) — never bare values, never described only in text | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 38. Password/secret fields have `"sensitive": True` | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 39. `groups` uses `AppGroup` enum values from `qlib/DataProvider/AppGroup.qc` | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 40. App `logo` stored as separate file, loaded at module level in `Priv` namespace | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 41. App `desc` uses markdown: bullet list of capabilities, links to project website, business-language explanation of value | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 42. `display_name` is user-friendly ("Apache Avro" not "avro") | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 43. `short_desc` is plain text, under 80 chars, single sentence — no markdown | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 44. `desc` uses markdown: backticks for code/field refs (`` `field_name` ``, `` `True` ``, `` `pdf` ``), `\n\n` for paragraphs, `- ` bullet lists for enumerations, `**bold**` for caveats | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 45. Descriptions use plain business language relating to common challenges — not just technical "what" but "why" and "when to use" | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 46. No bare `True`/`False`/`NOTHING` — must be backtick-wrapped in `desc` | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 47. No bare field/option names in prose — must use backticks | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 48. Long descriptions (>500 chars) use bold section headers and bullet lists | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 49. **Factory registration in Qore repo**: every factory name registered in `qlib/DataProvider/DataProvider.qc` → `FactoryMap` (without this, module loads but doesn't appear in Qorus apps) | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 50. **`getRecordTypeImpl()` signature**: must be `private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options)` — NOT returning `*AbstractDataProviderType` | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 51. **Dependency JARs committed** (for JNI modules): JAR files in `qlib/*/jar/` may be gitignored — use `git add -f` to ensure they're tracked, otherwise CI compilation fails | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 52. JAR install rules in CMakeLists.txt for all dependency JARs | N/A | No new Qore module, separated source, QPP class, provider registration, business type, factory or dependency JAR. |
| 53. **No workarounds**: No TODOs, FIXMEs, stubs, or partially-implemented features | Pass | The root cause is the uninitialized extraction destination when the stream sentry reaches EOF. Initializing it defines the existing zero fallback; parsing rules and nonempty numeric behavior are unchanged. |
| 54. **Exception safety**: C++ uses `ReferenceHolder` for Qore allocations, `std::unique_ptr` for C++ allocations, `*xsink` checked after every fallible operation | Pass | Local double initialization adds no allocation or fallible operation. Existing stream/locale objects retain automatic ownership. There is no new Qore value ownership or exception path. |
| 55. **Thread safety**: All mutable shared state protected by `std::lock_guard<std::mutex>` or documented as immutable-after-construction | Pass | The result is a call-local double; no shared mutable state is added. |
| 56. **Type safety**: Strongly-typed `code<return(args)>` instead of untyped `code`; `static_cast` instead of C casts; typed hashdecls for results; enums where appropriate | Pass | The native result remains double. Test inputs, expected results and softfloat coercion helper are explicitly typed. |
| 57. **Performance**: No O(n²) where O(n) is possible; no unnecessary copies; coordinate descent uses incremental residuals not full matrix multiply | Pass | A single scalar initialization adds constant work and no allocation, copy or traversal. |
| 58. **Error handling**: All inputs validated (dimensions, empty data, unfitted models); C++ I/O handles EAGAIN/EINTR if applicable | Pass | Empty and every classic-locale ASCII whitespace kind, long whitespace, malformed inputs, numeric prefixes, finite values and negative zero have explicit conversion/coercion assertions. The baseline new test fails on an empty softfloat argument; all final execution modes pass. |
| 59. **Documentation**: Doxygen `@param`, `@return`, `@throw` on all public methods; `@par Example` with realistic business scenarios; `@note` for important caveats | Pass | The code comment explains the stream sentry failure; release notes describe the behavior, and the executable regression supplies input examples. No public API signature or new method needs documentation. |
| 60. **QPP flags**: `[flags=CONSTANT]` on methods that never throw; `[flags=RET_VALUE_ONLY]` on methods that throw but have no side effects | N/A | No QPP code or method flag changed. |
| 61. **Security**: No user-controlled format strings; no buffer overflows; bounds checking on array indices; no credentials in code | Pass | The fix removes an uninitialized read; there is no new buffer, format string, external input channel or credential. Valgrind evidence verifies initialized use. |
| 62. **Correctness**: Algorithms verified against reference implementations; edge cases tested (empty data, single sample, all-zero features) | Pass | Five suites pass in AST, IR, JIT and tiered modes: 116 cases / 3628 assertions. Valgrind passes the new AST/IR regression and existing AST float suite: zero errors and zero definite/indirect/possible loss. Both XML corpus reports are unchanged outside runtime versions; all 64 affected XML suites pass. |

All 62 checks are classified: 20 Pass, 42 N/A, no failures.
