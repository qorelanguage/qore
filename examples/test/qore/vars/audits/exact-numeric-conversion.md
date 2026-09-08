# Exact numeric conversion and ownership audit

Copyright (C) 2026 Qore Technologies, s.r.o.

Scope: lib/QoreNumberNode.cpp, lib/Variable.cpp, lib/QoreIRInterpreter.cpp,
three new number-raw-integer/remove-boxed-integer/foreach-boxed-numeric tests,
the basic-types page, only the numeric 3.0 release-note bullet, and
design/exact-scalar-conversion.md. All 62 audit-changes checks were applied
against these final changes. Referenced module-structure, sandboxing,
cooperative-cancellation and DataProvider development/checklist guides were read.
Concurrent CMake, qlib, Mistral/OpenApi3/RestSchema work is excluded.

MPFR's automatic decimal digit count preserves a value only when parsed back
at its original binary precision. Raw integral output now requests enough
digits to preserve the integer even for a higher-precision consumer. See the
[MPFR conversion API](https://www.mpfr.org/mpfr-current/mpfr.html#Conversion-Functions).
Zero and special values bypass the new exponent use. Default/scientific and
fractional formatting, and native float-to-number precision policy, are unchanged.

The SOAP boundary matrix exposed a separate assertion: assignInitial() returns
a redundant boxed numeric node even for a generic lvalue. Every removal path
now discards that returned storage. Valgrind then exposed typed foreach
conversions allocating boxes without cleanup; those results now use the same
owned-slot setter as numeric constants. No underlying value is discarded.

Validation:

- Existing build-debug is Debug with /usr prefix, matching /usr/bin/qore.
  `cmake --build build-debug --target libqore -j4` succeeds without compiler
  warnings. No astparser target, installation or push was used.
- `LD_LIBRARY_PATH=build-debug build-debug/qore -b --enable-debug
  --exec-mode=ir` and `--exec-mode=ast` run all eleven affected suites:
  116 cases / 1664 assertions, with no warning/error. The three new suites
  account for 13 cases / 153 assertions in each mode.
- Each new suite runs under `QORE_PCRE2_NO_JIT=1 valgrind --leak-check=full
  --show-leak-kinds=all --errors-for-leak-kinds=definite,indirect,possible
  --error-exitcode=97 ... qore -b --enable-debug --exec-mode=ir`:
  zero errors, zero definite/indirect/possible loss, no suppressions.
  The independently reproduced Valgrind DW_AT_abstract_origin reader warning
  remains a P9 environment finding; this is a native memory pass, not a claim
  that the tool-warning acceptance criterion is closed.
- The typed-loop regression includes queue-event cancellation while a boxed
  integer result is live, exception unwinding, return/break/continue, empty
  lists, 48/64-bit boundaries and negative NaNs.
- The pre-fix typed-loop regression leaks 264 bytes in 11 blocks (both numeric
  instructions); removal with the explicit AST Program aborts. The raw-number
  parent emits different low integer digits. Exact reference digits were
  calculated with independent integer arithmetic, not a self-round-trip alone.
- An AOT executable compiled from the 128-bit 10^100 number constant retains
  every raw digit. The basic-types and release-note templates regenerate via
  QORE_DOX_FILES without warnings/errors.
- Downstream XML: 36 suites pass 478 cases; the independent integer matrix
  checks 1200 SOAP documents. Both-version survey changes only 16 previously
  clamped large-value output verdicts to the retained libxml2 oracle rejection.
  Xerces and exact integer comparisons pass; both-direction coverage resolves
  32 value-loss failures with no new failures, and its expanded strict gate
  passes 60 descriptions / 536 message-direction cases.

Logs: /tmp/wsdl-p3-core-native-checks.log, /tmp/wsdl-p3-core-valgrind-checks.log,
/tmp/wsdl-p3-native-{ir,ast,valgrind}-*.log,
/tmp/wsdl-p3-core-foreach-{build,valgrind-before}.log,
/tmp/wsdl-p3-core-remove-before.log, /tmp/wsdl-p3-core-number-*.log,
/tmp/wsdl-p3-native-docs.log and the XML P3-02 logs recorded in EXECUTION.md.

| Check | Status | Evidence |
| --- | --- | --- |
| 1. Entry exists in `doxygen/lang/120_modules.dox.tmpl` (for modules in the Qore repo; N/A for external module repos) | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 2. Entry exists in `doxygen/lang/900_release_notes.dox.tmpl` (for modules in the Qore repo; external modules have release notes in their .qm) | Pass | The scoped 3.0 release-note bullet describes exact raw integers and numeric removal/iteration cleanup; the unrelated Mistral hunk is excluded. |
| 3. `qore_user_module()` or `qore_external_user_module()` call in `CMakeLists.txt` | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 4. Module added to QMOD list in `CMakeLists.txt` | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 5. `.qm` file has `@section <lowercasemodname>intro` as first doc section — **must be all lowercase** (e.g., `avrodataproviderintro`, not `AvroDataProviderintro`) | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 6. `%modern` in `.qm` file — no redundant `%new-style`, `%require-types`, `%strict-args`, `%enable-all-warnings` | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 7. No parse directives (`%requires`, `%modern`, `%new-style`) in separated `.qc` files (check OUTSIDE of `@code` blocks only) | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 8. No `%include` usage (deprecated for modules) | Pass | No deprecated include directive or module layout change. |
| 9. Copyright 2026 on all new files | Pass | All three new executable tests, the design document and this audit carry 2026 notices. |
| 10. Directory layout: `.qm` inside `qlib/<ModuleName>/` directory (not at `qlib/<ModuleName>.qm` for multi-file modules) | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 11. No second `.qm` for the same module at `qlib/<ModuleName>.qm` | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 12. `ns=Qore::XX` matches the QoreNamespace constructor path | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 13. `%modern` directive present | Pass | All three new tests use %modern. |
| 14. Executable permission set (`chmod +x`) | Pass | All three new tests have mode 0755. |
| 15. Uses %prepend-module-path  before %requires for in-repo modules (Qore and Qore modules only; not Qorus) | Pass | Each new test prepends ../../../../qlib and requires ../../../../qlib/QUnit.qm. |
| 16. External module dependencies use `%try-module` — except modules delivered with the project itself (Qore ex: DataProvider, ConnectionProvider, QUnit, etc.) which use hard `%requires` | Pass | Only builtin facilities and the hard in-repository QUnit dependency are used. |
| 17. No filesystem operations (fopen, open, creat, unlink, remove, rename, mkdir, rmdir, stat, chmod) without sandbox checks | Pass | Changed native statements perform numeric conversion, lvalue assignment and reference cleanup; no filesystem operation. |
| 18. No network operations (connect, bind, socket, getaddrinfo, gethostbyname) without sandbox checks | Pass | No network operation. |
| 19. If filesystem/network ops exist, verify `QoreSandboxManagerHelper` usage | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 20. No `File::`, `Dir::`, `Socket::`, `HTTPClient::` usage without justification | Pass | New Qore tests use builtin numeric/container/Program/Queue APIs; no filesystem or network access. |
| 21. All `for`/`while` loops that could iterate >100 times have `qore_check_cancel()` checks | Pass | No new native loop. Typed foreach retains its per-back-edge qore_check_cancel call; raw formatting does not enter the rounding-heuristic loop. |
| 22. Uses `qore_check_cancel()` (NOT deprecated `qore_check_io_interrupt()`) | Pass | IR back edges use qore_check_cancel, with no deprecated interrupt call introduced. |
| 23. Check frequency: every 100 iterations for tight loops, every 10 for expensive iterations | Pass | The existing IR loop checkpoint checks every back edge, stronger than the 100-iteration requirement. |
| 24. No blocking operations without cancellation support | Pass | No blocking native operation is added. The cancellation regression uses Queue ready/completion events and bounded 10-second deadlines, without sleeps or polling. |
| 25. Every action has `display_name`, `short_desc` (plain text, <80 chars), `desc` (markdown) | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 26. Every action has `options` populated via `getActionOptionFromFields()` — without this, the action shows an empty, unusable form | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 27. Every action has `output_type` set to a typed data type constant (e.g., `MyResponseDataType`) — not omitted | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 28. DPAT_API actions: provider has `"supports_request": True` and implements `doRequestImpl()` | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 29. DPAT_FIND actions: every option exists in `SearchOptions`, `getRecordTypeImpl()` returns `*hash<string, AbstractDataField>` | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 30. Scheme-based apps (with `"scheme"` in registerApp): actions use `"path"` and do NOT use `"cls"` — having both `scheme` and `cls` causes a runtime error | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 31. Single-key hash slices use trailing comma: `Fields{"key",}` (without trailing comma, `Fields{"key"}` returns the value, not a hash) | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 32. **Typed data type classes exist** for request and response types — inherit `HashDataType`, have `const Fields` hash, call `addQoreFields(Fields)` in constructor, export public constant at bottom (e.g., `public const MyDataType = new MyDataType();`) | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 33. Request/input types use `public` Fields (enables `ClassName::Fields` in action registration) | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 34. Response/output types use `private` Fields | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 35. Each field in data types has `display_name`, `type`, and `desc` (markdown-formatted) | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 36. Input fields have `example_value` where useful (string fields, endpoint URIs, SQL queries, etc.) | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 37. Fields with finite allowed values use `allowed_values` with `AllowedValueInfo` containing both `value` and `display_name` (Title Case, human-readable) — never bare values, never described only in text | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 38. Password/secret fields have `"sensitive": True` | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 39. `groups` uses `AppGroup` enum values from `qlib/DataProvider/AppGroup.qc` | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 40. App `logo` stored as separate file, loaded at module level in `Priv` namespace | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 41. App `desc` uses markdown: bullet list of capabilities, links to project website, business-language explanation of value | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 42. `display_name` is user-friendly ("Apache Avro" not "avro") | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 43. `short_desc` is plain text, under 80 chars, single sentence — no markdown | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 44. `desc` uses markdown: backticks for code/field refs (`` `field_name` ``, `` `True` ``, `` `pdf` ``), `\n\n` for paragraphs, `- ` bullet lists for enumerations, `**bold**` for caveats | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 45. Descriptions use plain business language relating to common challenges — not just technical "what" but "why" and "when to use" | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 46. No bare `True`/`False`/`NOTHING` — must be backtick-wrapped in `desc` | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 47. No bare field/option names in prose — must use backticks | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 48. Long descriptions (>500 chars) use bold section headers and bullet lists | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 49. **Factory registration in Qore repo**: every factory name registered in `qlib/DataProvider/DataProvider.qc` → `FactoryMap` (without this, module loads but doesn't appear in Qorus apps) | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 50. **`getRecordTypeImpl()` signature**: must be `private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options)` — NOT returning `*AbstractDataProviderType` | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 51. **Dependency JARs committed** (for JNI modules): JAR files in `qlib/*/jar/` may be gitignored — use `git add -f` to ensure they're tracked, otherwise CI compilation fails | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 52. JAR install rules in CMakeLists.txt for all dependency JARs | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 53. **No workarounds**: No TODOs, FIXMEs, stubs, or partially-implemented features | Pass | Fixes insufficient MPFR output digits, redundant removal boxes and unowned typed-loop results at their allocation/ownership boundaries. No fixture branches, suppression or workaround. |
| 54. **Exception safety**: C++ uses `ReferenceHolder` for Qore allocations, `std::unique_ptr` for C++ allocations, `*xsink` checked after every fallible operation | Pass | The MPFR buffer retains ON_BLOCK_EXIT(mpfr_free_str) and null-result handling. All twelve removal assignments discard returned redundant storage through xsink. Typed numeric results use the existing owned-slot setter and deduplicated cleanup ledger. Numeric node release cannot invoke user destructors. Exception/return/cancellation and Valgrind checks pass. |
| 55. **Thread safety**: All mutable shared state protected by `std::lock_guard<std::mutex>` or documented as immutable-after-construction | Pass | Numeric text conversion and IR slots are per-call/per-execution state. No new mutable shared state. The cancellation regression exchanges values through synchronized queues. |
| 56. **Type safety**: Strongly-typed `code<return(args)>` instead of untyped `code`; `static_cast` instead of C casts; typed hashdecls for results; enums where appropriate | Pass | Digit count is size_t with explicit static_cast from the positive exponent of a nonzero integral value. Native values and slot IDs retain their existing types; tests use typed integer/float lists and ExceptionInfo. |
| 57. **Performance**: No O(n²) where O(n) is possible; no unnecessary copies; coordinate descent uses incremental residuals not full matrix multiply | Pass | Exact raw output storage is proportional to the required integer text. Binary exponent is an upper bound for supported radix digit counts. Owned-slot replacement releases each prior box; the cleanup ledger deduplicates slot IDs, so loop iterations do not grow its storage. |
| 58. **Error handling**: All inputs validated (dimensions, empty data, unfitted models); C++ I/O handles EAGAIN/EINTR if applicable | Pass | Zero/special/fractional formatting keeps its existing path. Tests cover signed 48-bit and 64-bit bounds, missing removed members, list holes/slices, empty iteration, inline/boxed transitions, negative NaNs and control-flow exits. |
| 59. **Documentation**: Doxygen `@param`, `@return`, `@throw` on all public methods; `@par Example` with realistic business scenarios; `@note` for important caveats | Pass | The basic-types page provides the exact integer export example, release notes explain the changes, and design/exact-scalar-conversion.md records the implemented conversion/ownership contract. No public API signature changes. QORE_DOX_FILES regenerates the edited templates. |
| 60. **QPP flags**: `[flags=CONSTANT]` on methods that never throw; `[flags=RET_VALUE_ONLY]` on methods that throw but have no side effects | N/A | No new user module, separated source, QPP class, provider/action/app/field registration, factory, JAR or public QPP method in this scoped native change. |
| 61. **Security**: No user-controlled format strings; no buffer overflows; bounds checking on array indices; no credentials in code | Pass | No new pointer arithmetic, input-index access, user-controlled format string or credentials. IR setters retain slot-bound assertions; list bounds and absent-entry checks remain before conversion. |
| 62. **Correctness**: Algorithms verified against reference implementations; edge cases tested (empty data, single sample, all-zero features) | Pass | Eleven suites pass in both IR/AST modes: 116 cases / 1664 assertions. All three focused tests pass Valgrind with zero errors and zero definite/indirect/possible loss. Parent raw output differs, AST removal aborts and typed foreach leaks 264 bytes in 11 blocks. The downstream integer matrix verifies 1200 documents independently; the corpus has no new failure. |
