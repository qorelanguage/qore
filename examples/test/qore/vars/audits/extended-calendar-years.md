# Extended calendar years audit

Copyright (C) 2026 Qore Technologies, s.r.o.

Scope: seven native/QPP source files, Qore 3.0 release notes, the implemented
`design/extended-calendar-years.md`, an executable QUnit suite and a deterministic
native regression. The final patch was reviewed against the main `develop` tree.
The full `audit-changes` skill and its referenced module structure, sandboxing,
cooperative cancellation and DataProvider guides were reviewed; provider/module
registration checks do not apply to this native date change.

Result: **21 Pass, 41 N/A, 0 Fail**. No workaround or scope reduction is introduced.

|!Check|!Status|!Evidence
|1. Entry exists in `doxygen/lang/120_modules.dox.tmpl` (for modules in the Qore repo; N/A for external module repos)|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|2. Entry exists in `doxygen/lang/900_release_notes.dox.tmpl` (for modules in the Qore repo; external modules have release notes in their .qm)|Pass|Qore 3.0 release notes document extended years, corrected calendar arithmetic, full Qore year accessors, wide ISO-week results and compact-buffer safety.
|3. `qore_user_module()` or `qore_external_user_module()` call in `CMakeLists.txt`|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|4. Module added to QMOD list in `CMakeLists.txt`|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|5. `.qm` file has `@section <lowercasemodname>intro` as first doc section — **must be all lowercase** (e.g., `avrodataproviderintro`, not `AvroDataProviderintro`)|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|6. `%modern` in `.qm` file — no redundant `%new-style`, `%require-types`, `%strict-args`, `%enable-all-warnings`|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|7. No parse directives (`%requires`, `%modern`, `%new-style`) in separated `.qc` files (check OUTSIDE of `@code` blocks only)|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|8. No `%include` usage (deprecated for modules)|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|9. Copyright 2026 on all new files|Pass|All new and changed authored source notices include 2026. The final native build includes the four updated legacy notices.
|10. Directory layout: `.qm` inside `qlib/<ModuleName>/` directory (not at `qlib/<ModuleName>.qm` for multi-file modules)|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|11. No second `.qm` for the same module at `qlib/<ModuleName>.qm`|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|12. `ns=Qore::XX` matches the QoreNamespace constructor path|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|13. `%modern` directive present|Pass|The new Qore suite uses %modern; the native helper is a standalone C++ regression.
|14. Executable permission set (`chmod +x`)|Pass|extended-years.qtest is executable (0755).
|15. Uses %prepend-module-path  before %requires for in-repo modules (Qore and Qore modules only; not Qorus)|Pass|The local qlib prepend precedes the relative hard requirement for ../../../../qlib/QUnit.qm.
|16. External module dependencies use `%try-module` — except modules delivered with the project itself (Qore ex: DataProvider, ConnectionProvider, QUnit, etc.) which use hard `%requires`|N/A|Only in-repository QUnit is required; no external binary test dependency.
|17. No filesystem operations (fopen, open, creat, unlink, remove, rename, mkdir, rmdir, stat, chmod) without sandbox checks|Pass|The parser, calendar arithmetic and date-string formatting perform no filesystem operations.
|18. No network operations (connect, bind, socket, getaddrinfo, gethostbyname) without sandbox checks|Pass|No network operations are introduced.
|19. If filesystem/network ops exist, verify `QoreSandboxManagerHelper` usage|N/A|No filesystem or network operation is added by the date conversion change.
|20. No `File::`, `Dir::`, `Socket::`, `HTTPClient::` usage without justification|N/A|No Qore module changes; the test uses no direct file or network operations.
|21. All `for`/`while` loops that could iterate >100 times have `qore_check_cancel()` checks|Pass|The new year scanner checks cancellation every 100 digits. Other added native loops have six fixed entries or no iteration.
|22. Uses `qore_check_cancel()` (NOT deprecated `qore_check_io_interrupt()`)|Pass|The scanner uses qore_check_cancel with its existing exception sink and an operation description.
|23. Check frequency: every 100 iterations for tight loops, every 10 for expensive iterations|Pass|The year scanner checks every 100 digits without a lexical length cap.
|24. No blocking operations without cancellation support|Pass|Production operations are in-memory conversions; the native helper uses deterministic pre-requested cancellation and owns its temporary program context.
|25. Every action has `display_name`, `short_desc` (plain text, <80 chars), `desc` (markdown)|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|26. Every action has `options` populated via `getActionOptionFromFields()` — without this, the action shows an empty, unusable form|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|27. Every action has `output_type` set to a typed data type constant (e.g., `MyResponseDataType`) — not omitted|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|28. DPAT_API actions: provider has `"supports_request": True` and implements `doRequestImpl()`|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|29. DPAT_FIND actions: every option exists in `SearchOptions`, `getRecordTypeImpl()` returns `*hash<string, AbstractDataField>`|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|30. Scheme-based apps (with `"scheme"` in registerApp): actions use `"path"` and do NOT use `"cls"` — having both `scheme` and `cls` causes a runtime error|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|31. Single-key hash slices use trailing comma: `Fields{"key",}` (without trailing comma, `Fields{"key"}` returns the value, not a hash)|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|32. **Typed data type classes exist** for request and response types — inherit `HashDataType`, have `const Fields` hash, call `addQoreFields(Fields)` in constructor, export public constant at bottom (e.g., `public const MyDataType = new MyDataType();`)|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|33. Request/input types use `public` Fields (enables `ClassName::Fields` in action registration)|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|34. Response/output types use `private` Fields|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|35. Each field in data types has `display_name`, `type`, and `desc` (markdown-formatted)|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|36. Input fields have `example_value` where useful (string fields, endpoint URIs, SQL queries, etc.)|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|37. Fields with finite allowed values use `allowed_values` with `AllowedValueInfo` containing both `value` and `display_name` (Title Case, human-readable) — never bare values, never described only in text|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|38. Password/secret fields have `"sensitive": True`|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|39. `groups` uses `AppGroup` enum values from `qlib/DataProvider/AppGroup.qc`|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|40. App `logo` stored as separate file, loaded at module level in `Priv` namespace|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|41. App `desc` uses markdown: bullet list of capabilities, links to project website, business-language explanation of value|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|42. `display_name` is user-friendly ("Apache Avro" not "avro")|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|43. `short_desc` is plain text, under 80 chars, single sentence — no markdown|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|44. `desc` uses markdown: backticks for code/field refs (`` `field_name` ``, `` `True` ``, `` `pdf` ``), `\n\n` for paragraphs, `- ` bullet lists for enumerations, `**bold**` for caveats|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|45. Descriptions use plain business language relating to common challenges — not just technical "what" but "why" and "when to use"|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|46. No bare `True`/`False`/`NOTHING` — must be backtick-wrapped in `desc`|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|47. No bare field/option names in prose — must use backticks|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|48. Long descriptions (>500 chars) use bold section headers and bullet lists|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|49. **Factory registration in Qore repo**: every factory name registered in `qlib/DataProvider/DataProvider.qc` → `FactoryMap` (without this, module loads but doesn't appear in Qorus apps)|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|50. **`getRecordTypeImpl()` signature**: must be `private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options)` — NOT returning `*AbstractDataProviderType`|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|51. **Dependency JARs committed** (for JNI modules): JAR files in `qlib/*/jar/` may be gitignored — use `git add -f` to ensure they're tracked, otherwise CI compilation fails|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|52. JAR install rules in CMakeLists.txt for all dependency JARs|N/A|No new module, QPP class/namespace, provider action/app/type/field, factory or JAR dependency.
|53. **No workarounds**: No TODOs, FIXMEs, stubs, or partially-implemented features|Pass|Root causes are corrected at the year parser, arithmetic, accessor and buffer allocation boundaries. No fixture-specific behavior, heuristic rounding or tolerance is introduced. Existing native calendar and legacy C++ API ranges remain documented.
|54. **Exception safety**: C++ uses `ReferenceHolder` for Qore allocations, `std::unique_ptr` for C++ allocations, `*xsink` checked after every fallible operation|Pass|The compact string constructor delegates to an initialized growing string; parsed dates and the temporary native program use RAII. Cancellation returns without overwriting its error, and checked TimeZone construction retains ownership until success.
|55. **Thread safety**: All mutable shared state protected by `std::lock_guard<std::mutex>` or documented as immutable-after-construction|Pass|All added conversion state is per-call; immutable timezone data and existing thread-local program/cancellation contexts retain their synchronization contracts.
|56. **Type safety**: Strongly-typed `code<return(args)>` instead of untyped `code`; `static_cast` instead of C casts; typed hashdecls for results; enums where appropriate|Pass|Native magnitude arithmetic widens before subtraction/multiplication; calendar/ISO years have explicit widths, typed qore_tm metadata and an additive int64-reference overload. Legacy exported signatures remain present.
|57. **Performance**: No O(n²) where O(n) is possible; no unnecessary copies; coordinate descent uses incremental residuals not full matrix multiply|Pass|Parsing is linear in input length with constant extra state. Calendar conversion uses fixed arithmetic and the Gregorian 400-year cycle; growing string formatting replaces an undersized fixed buffer.
|58. **Error handling**: All inputs validated (dimensions, empty data, unfitted models); C++ I/O handles EAGAIN/EINTR if applicable|Pass|Regressions cover malformed/overflowing years, native limits, negative leaps, compact syntax, explicit offsets, failed-parse recovery, cancellation and both accessor families.
|59. **Documentation**: Doxygen `@param`, `@return`, `@throw` on all public methods; `@par Example` with realistic business scenarios; `@note` for important caveats|Pass|The public wide ISO-week overload documents each parameter and its boundary purpose; the legacy short-year limitation is explicit. Design/release notes include a runnable historical timestamp example. Focused native/QPP Doxygen logs are empty.
|60. **QPP flags**: `[flags=CONSTANT]` on methods that never throw; `[flags=RET_VALUE_ONLY]` on methods that throw but have no side effects|Pass|Existing side-effect-free year/week getters retain CONSTANT flags. No new QPP class or effectful method is added.
|61. **Security**: No user-controlled format strings; no buffer overflows; bounds checking on array indices; no credentials in code|Pass|The fixed 15-byte date-string allocation is removed, preserving dynamic bounds checking. Magnitude scanning checks overflow before accumulation; negative years cannot produce negative weekday array indexes. No user-controlled format string or secret is introduced.
|62. **Correctness**: Algorithms verified against reference implementations; edge cases tested (empty data, single sample, all-zero features)|Pass|The frozen implementation passes AST, IR, JIT and tiered tests in UTC and Prague, AOT, existing date suites, the native cancellation/string regression and affected XML suites. Corpus failure comparison adds no regressions; Valgrind reports zero memory errors or lost allocations. Exact evidence and the retained instrumentation warning are recorded below.

## Root causes and verification

The four-digit parser rejected separated extended years. Truncated division and
narrow intermediate arithmetic corrupted negative/boundary calendar calculations;
Qore year accessors narrowed the stored year through a legacy `short` C++ return.
The ISO week rule incorrectly excluded leap years starting on Thursday. Separately,
the compact `QoreString(DateTime*)` constructor used a 15-byte allocation with an
unbounded formatted write; the reduced pre-fix native reproducer reported six
Valgrind errors in five contexts. Each defect is corrected at its implementation
boundary and covered by committed regressions.

The isolated build uses `build-debug/`, Debug, prefix `/usr` matching `/usr/bin/qore`,
and its local `LD_LIBRARY_PATH` and module paths. Nothing was installed. The final
runtime SHA-256 is
`80078ec30d71bc618b7bb40991bad63604303f379c3e47e6dd41d58d0dbc56fd`.
The main-tree patch is checked against the exact tested source hashes; the full
runtime matrix preceded four copyright-only updates, which were verified as exact
notice substitutions and rebuilt. Final native/QUnit, AOT, Valgrind and downstream
XML checks use the final runtime.

- New QUnit suite: seven cases, 9,481 assertions per run, AST/IR/JIT/tiered in both
  UTC and Europe/Prague. The complete core matrix, including five existing suites
  in AST and IR, passes 92 cases and 76,940 assertions across 18 invocations.
- The final runtime also passes the seven-case suite, the standalone AOT executable
  compiled with `qcc -O3 --strip-source`, and the deterministic native helper.
- Valgrind 3.27.1 runs the new suite, existing `date.qtest`, and native helper:
  zero errors; zero definitely, indirectly or possibly lost bytes; no suppressions.
  Each run retains the previously tracked Valgrind DWARF-reader warning
  `zero subprog, missing DW_AT_abstract_origin`. This instrumentation issue is
  recorded with the XML P9 toolchain findings; these runs are not called warning-free.
  The existing Windows-only date test is skipped by its original platform guard.
- All 66 affected XML suites pass: 761 cases, 20,957 assertions. Both-version corpus
  comparison has zero added failure signatures and resolves 16 date/dateTime
  decode-direction failures, reducing the broad report from 196 to 180 failures.
  Strict selection remains 97 WSDLs / 828 directions, zero selected failures.
  Original corpus and catalog hashes are unchanged. Typed temporal preservation
  beyond the existing explicit assertions remains XML P3 work; these counts are
  not a claim of full XML or SOAP conformance.
- Focused native and QPP Doxygen checks produce empty warning logs; the historical
  timestamp example runs successfully. Both old and new `getISOWeek()` symbols
  and the existing `getYear()` symbol are present in the final library.
- The final changed-source build log contains no compiler warning or error.

Reproduce the native test from a configured Debug tree without installing:

```sh
c++ -std=c++20 -g -Iinclude -Ibuild-debug/include \
  examples/test/qore/vars/extended_year_cancellation.cpp \
  -Lbuild-debug -lqore -o build-debug/extended-year-cancellation
LD_LIBRARY_PATH=build-debug build-debug/extended-year-cancellation
QORE_PCRE2_NO_JIT=1 LD_LIBRARY_PATH=build-debug valgrind \
  --error-exitcode=97 --leak-check=full \
  --show-leak-kinds=definite,indirect,possible \
  --errors-for-leak-kinds=definite,indirect,possible \
  build-debug/extended-year-cancellation
LD_LIBRARY_PATH=build-debug build-debug/qore -b --enable-debug \
  examples/test/qore/vars/extended-years.qtest
```

Execution evidence is retained in `/tmp/wsdl-p3-24-core-final-hashes.json`,
`/tmp/wsdl-p3-24-core-buffer-verify.log`, the per-mode core logs,
`/tmp/wsdl-p3-24-core-valgrind-{new,existing,native}.log`,
`/tmp/wsdl-p3-24-core-aot-{build,run}.log`,
`/tmp/wsdl-p3-24-core-report-comparison.json`, and the corresponding XML corpus
reports. Committed regression sources and the commands above reproduce the native
checks without depending on those temporary logs.
