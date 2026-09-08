# Round-trip numeric formatting audit

Copyright (C) 2026 Qore Technologies, s.r.o.

Scope: QoreNumberNode public/private headers and implementation, Float/Number
pseudo-methods, the native CMake test target, three new test files, implemented
scalar-conversion design and the numeric release-note bullet. Concurrent Pgsql,
RestHandler and other developers' work is excluded. No astparser source, audit,
documentation or build target is included.

The full audit-changes skill was read. The module-structure, sandboxing and
cooperative-cancellation guides were consulted; DataProvider registration checks
are inapplicable to this native core change. Every checklist item is recorded below.

The method is an explicit shortest-significand formatter. MPFR's automatic digit
count bounds a binary search that tests nearest and both directed decimal
neighbors. This covers asymmetric binary rounding intervals at powers of two.
An independent Python Fraction oracle computes the exact interval and decimal
integer-grid solution; it does not parse formatter output back through MPFR as
its expected-value oracle. Native reconstruction is an additional assertion.
Float formatting uses C++ scientific to_chars, followed by optional plain expansion.
Neither existing display heuristics nor global rounding or precision policy changes.
See [MPFR conversions](https://www.mpfr.org/mpfr-current/mpfr.html#Conversion-Functions)
and [C++ character conversion](https://eel.is/c++draft/charconv.to.chars).

Validation:

- /usr/bin/qore matches the retained /usr prefix; build-debug is Debug. The explicit
  qore-number-round-trip-test target builds successfully, without compiler warnings.
  QPP generation and focused Doxygen with parameter warnings enabled pass.
- Seven affected Qore suites pass in both IR and AST: 84 cases / 1152 assertions.
  The new public-method suite accounts for 5 cases / 112 assertions per mode.
- The exact rational oracle checks 320 signed MPFR values at 128–8192 bits in plain
  and scientific formats. The binary64 oracle checks 1050 boundary/seeded values.
  Seeds are 0xDEC1A12026 and 0xB1642026. Native tests also exercise signed-zero and
  nonfinite cancellation entry, unchanged destinations and recovery.
- Native oracle and new Qore suite pass Valgrind, zero errors and zero definite,
  indirect or possible loss, no suppressions. The affected XML decimal suite also
  passes 5/380 under Valgrind. Qore runs use -b --enable-debug and the local Debug
  library. The already reproduced Valgrind DW_AT_abstract_origin reader warning
  remains a P9 environment finding; no new claim of tool-warning closure is made.
- Downstream XML: all 38 affected suites pass (489 cases), and the new independent
  decimal matrix validates 784 documents in actual SOAP 1.1/1.2 bindings and both
  directions. The both-version survey resolves 12 adjudicated failure rows, including
  all eight recorded decimal value losses. No new corpus failure appears.

Logs: /tmp/wsdl-decimal-core-final-build.log,
/tmp/wsdl-decimal-core-configure.log, /tmp/wsdl-decimal-core-checks.log,
/tmp/wsdl-decimal-round-trip-oracle-final.log, /tmp/wsdl-decimal-core-docs.log,
/tmp/wsdl-decimal-core-native-valgrind.log, /tmp/wsdl-decimal-core-qore-valgrind.log,
/tmp/wsdl-p3-04-valgrind.log, /tmp/wsdl-p3-04-affected.log,
/tmp/wsdl-p3-04-independent.log, /tmp/wsdl-p3-04-survey.json and
/tmp/wsdl-p3-04-coverage.json. No installation or push was performed.

| Check | Status | Evidence |
| --- | --- | --- |
| 1. Entry exists in `doxygen/lang/120_modules.dox.tmpl` (for modules in the Qore repo; N/A for external module repos) | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 2. Entry exists in `doxygen/lang/900_release_notes.dox.tmpl` (for modules in the Qore repo; external modules have release notes in their .qm) | Pass | The Qore 3.0 release notes name both round-trip pseudo-methods and their value contract. |
| 3. `qore_user_module()` or `qore_external_user_module()` call in `CMakeLists.txt` | Pass | No module registration changes; CMake adds the explicit native oracle test target and links it to the local libqore. |
| 4. Module added to QMOD list in `CMakeLists.txt` | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 5. `.qm` file has `@section <lowercasemodname>intro` as first doc section — **must be all lowercase** (e.g., `avrodataproviderintro`, not `AvroDataProviderintro`) | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 6. `%modern` in `.qm` file — no redundant `%new-style`, `%require-types`, `%strict-args`, `%enable-all-warnings` | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 7. No parse directives (`%requires`, `%modern`, `%new-style`) in separated `.qc` files (check OUTSIDE of `@code` blocks only) | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 8. No `%include` usage (deprecated for modules) | Pass | No deprecated module include directive is introduced. |
| 9. Copyright 2026 on all new files | Pass | New tests/audit and every touched native file carry 2026 notices. |
| 10. Directory layout: `.qm` inside `qlib/<ModuleName>/` directory (not at `qlib/<ModuleName>.qm` for multi-file modules) | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 11. No second `.qm` for the same module at `qlib/<ModuleName>.qm` | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 12. `ns=Qore::XX` matches the QoreNamespace constructor path | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 13. `%modern` directive present | Pass | The qtest and embedded Python-driven Qore worker use %modern. |
| 14. Executable permission set (`chmod +x`) | Pass | number-round-trip.qtest has mode 0755. |
| 15. Uses %prepend-module-path  before %requires for in-repo modules (Qore and Qore modules only; not Qorus) | Pass | The qtest prepends local qlib before requiring ../../../../qlib/QUnit.qm. |
| 16. External module dependencies use `%try-module` — except modules delivered with the project itself (Qore ex: DataProvider, ConnectionProvider, QUnit, etc.) which use hard `%requires` | Pass | Only builtin facilities and in-repository QUnit are required; no external binary module in core tests. |
| 17. No filesystem operations (fopen, open, creat, unlink, remove, rename, mkdir, rmdir, stat, chmod) without sandbox checks | Pass | Formatter code has no filesystem operations. The native oracle reads supplied stdin only as a bounded test protocol. |
| 18. No network operations (connect, bind, socket, getaddrinfo, gethostbyname) without sandbox checks | Pass | No network operations in the formatter or native tests. |
| 19. If filesystem/network ops exist, verify `QoreSandboxManagerHelper` usage | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 20. No `File::`, `Dir::`, `Socket::`, `HTTPClient::` usage without justification | Pass | Core qtest has no I/O. Python oracle drives a finite stdin protocol using argument-list subprocesses and 60/180-second deadlines. |
| 21. All `for`/`while` loops that could iterate >100 times have `qore_check_cancel()` checks | Pass | Trailing-zero scanning checks every 100 iterations; exponent expansion checks every 64 KiB chunk; every MPFR candidate conversion checks cancellation. The native oracle calls the checked formatter on every input row. |
| 22. Uses `qore_check_cancel()` (NOT deprecated `qore_check_io_interrupt()`) | Pass | All new cancellation checks use qore_check_cancel; no deprecated I/O interrupt API. |
| 23. Check frequency: every 100 iterations for tight loops, every 10 for expensive iterations | Pass | One check per MPFR conversion/chunk and every 100 trailing zeros meets both expensive/tight-loop frequencies. |
| 24. No blocking operations without cancellation support | Pass | No production blocking operation; test subprocesses use finite input/EOF and bounded completion, with no sleeps or polling. |
| 25. Every action has `display_name`, `short_desc` (plain text, <80 chars), `desc` (markdown) | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 26. Every action has `options` populated via `getActionOptionFromFields()` — without this, the action shows an empty, unusable form | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 27. Every action has `output_type` set to a typed data type constant (e.g., `MyResponseDataType`) — not omitted | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 28. DPAT_API actions: provider has `"supports_request": True` and implements `doRequestImpl()` | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 29. DPAT_FIND actions: every option exists in `SearchOptions`, `getRecordTypeImpl()` returns `*hash<string, AbstractDataField>` | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 30. Scheme-based apps (with `"scheme"` in registerApp): actions use `"path"` and do NOT use `"cls"` — having both `scheme` and `cls` causes a runtime error | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 31. Single-key hash slices use trailing comma: `Fields{"key",}` (without trailing comma, `Fields{"key"}` returns the value, not a hash) | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 32. **Typed data type classes exist** for request and response types — inherit `HashDataType`, have `const Fields` hash, call `addQoreFields(Fields)` in constructor, export public constant at bottom (e.g., `public const MyDataType = new MyDataType();`) | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 33. Request/input types use `public` Fields (enables `ClassName::Fields` in action registration) | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 34. Response/output types use `private` Fields | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 35. Each field in data types has `display_name`, `type`, and `desc` (markdown-formatted) | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 36. Input fields have `example_value` where useful (string fields, endpoint URIs, SQL queries, etc.) | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 37. Fields with finite allowed values use `allowed_values` with `AllowedValueInfo` containing both `value` and `display_name` (Title Case, human-readable) — never bare values, never described only in text | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 38. Password/secret fields have `"sensitive": True` | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 39. `groups` uses `AppGroup` enum values from `qlib/DataProvider/AppGroup.qc` | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 40. App `logo` stored as separate file, loaded at module level in `Priv` namespace | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 41. App `desc` uses markdown: bullet list of capabilities, links to project website, business-language explanation of value | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 42. `display_name` is user-friendly ("Apache Avro" not "avro") | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 43. `short_desc` is plain text, under 80 chars, single sentence — no markdown | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 44. `desc` uses markdown: backticks for code/field refs (`` `field_name` ``, `` `True` ``, `` `pdf` ``), `\n\n` for paragraphs, `- ` bullet lists for enumerations, `**bold**` for caveats | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 45. Descriptions use plain business language relating to common challenges — not just technical "what" but "why" and "when to use" | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 46. No bare `True`/`False`/`NOTHING` — must be backtick-wrapped in `desc` | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 47. No bare field/option names in prose — must use backticks | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 48. Long descriptions (>500 chars) use bold section headers and bullet lists | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 49. **Factory registration in Qore repo**: every factory name registered in `qlib/DataProvider/DataProvider.qc` → `FactoryMap` (without this, module loads but doesn't appear in Qorus apps) | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 50. **`getRecordTypeImpl()` signature**: must be `private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options)` — NOT returning `*AbstractDataProviderType` | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 51. **Dependency JARs committed** (for JNI modules): JAR files in `qlib/*/jar/` may be gitignored — use `git add -f` to ensure they're tracked, otherwise CI compilation fails | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 52. JAR install rules in CMakeLists.txt for all dependency JARs | N/A | No new module, QPP class/namespace, DataProvider app/action/record/field, factory or dependency JAR is introduced. |
| 53. **No workarounds**: No TODOs, FIXMEs, stubs, or partially-implemented features | Pass | Explicit source-precision round-trip conversion is complete for finite/nonfinite numbers, signs and both formats. Existing display heuristic/raw/arithmetic policies are unchanged; no fixture workaround or scope reduction. |
| 54. **Exception safety**: C++ uses `ReferenceHolder` for Qore allocations, `std::unique_ptr` for C++ allocations, `*xsink` checked after every fallible operation | Pass | Qore output allocations use ReferenceHolder; MPFR strings use unique_ptr with mpfr_free_str; the restored MPFR value is scoped. Every error result/sink is checked, and plain output is committed only after successful formatting. Native tests verify unchanged destinations on cancellation and recovery. |
| 55. **Thread safety**: All mutable shared state protected by `std::lock_guard<std::mutex>` or documented as immutable-after-construction | Pass | All new state is call-local. Source numbers are read only, and no formatting mode/global precision is changed. |
| 56. **Type safety**: Strongly-typed `code<return(args)>` instead of untyped `code`; `static_cast` instead of C casts; typed hashdecls for results; enums where appropriate | Pass | Native helpers have explicit types, mpfr_rnd_t rounding modes, static_cast conversions, bounded charconv buffers and typed lambdas. Heterogeneous Qore test values deliberately test both pseudo-types. |
| 57. **Performance**: No O(n²) where O(n) is possible; no unnecessary copies; coordinate descent uses incremental residuals not full matrix multiply | Pass | A logarithmic search over at most the 8192-bit precision digit bound replaces linear candidate enumeration. Decimal padding uses chunks and QoreString geometric capacity growth; output construction is linear in output size. |
| 58. **Error handling**: All inputs validated (dimensions, empty data, unfitted models); C++ I/O handles EAGAIN/EINTR if applicable | Pass | Signed zero/nonfinite values precede ordinary conversion; charconv and MPFR errors have explicit categories. Invalid scientific arguments raise RUNTIME-OVERLOAD-ERROR; pending cancellation and subsequent recovery pass. |
| 59. **Documentation**: Doxygen `@param`, `@return`, `@throw` on all public methods; `@par Example` with realistic business scenarios; `@note` for important caveats | Pass | Both public pseudo-methods and the C++ API document arguments, returns, errors, precision caveats and examples. Durable design and release notes are updated. QPP generation and focused Doxygen with parameter warnings enabled pass without warnings. |
| 60. **QPP flags**: `[flags=CONSTANT]` on methods that never throw; `[flags=RET_VALUE_ONLY]` on methods that throw but have no side effects | Pass | Both methods are const with RET_VALUE_ONLY,NAMED_ARGS because cancellation can throw. They are not marked CONSTANT and do not change source values. |
| 61. **Security**: No user-controlled format strings; no buffer overflows; bounds checking on array indices; no credentials in code | Pass | Format strings are constant. Scientific charconv uses a 64-byte buffer with checked status; digit offsets are guarded/asserted. Test workers use no shell interpolation or external network services. |
| 62. **Correctness**: Algorithms verified against reference implementations; edge cases tested (empty data, single sample, all-zero features) | Pass | The rational oracle checks 320 numbers at 128/129/200/512/8192 bits in two formats, native source-precision reconstruction and append semantics; Python checks 1050 binary64 values. Seven Qore suites pass in IR and AST (84 cases, 1152 assertions); new native/Qore tests pass Valgrind with zero errors/lost memory. |
