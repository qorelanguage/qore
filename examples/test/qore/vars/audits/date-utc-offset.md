# Audit: date UTC offsets

Copyright (C) 2026 Qore Technologies, s.r.o.

The complete audit-changes skill and applicable module structure, sandboxing,
cooperative cancellation and DataProvider guides were reviewed for this exact
eight-file delta. Result: **23 Pass / 39 N/A / 0 Fail**.

The isolated Debug build uses prefix /usr, matching /usr/bin/qore. Main source
and the tested native/test files match exactly. No main build, installation or
push is performed. Debug libqore SHA-256:
`bc3501b6e25140bad2034d68f13f0c37aa5f7b3feb373e39d2ec4bd9dceae536`.

The initial new tests exposed standard/actual offset confusion and the -1
sentinel collision. Review corrected test fixture paths for relative invocation
and gave the interrupting Program explicit fixture access. A bounded batch crosses
the cooperative check interval even with a cached zone. The final new test passes
5 cases/611 assertions in AST, IR, JIT, tiered and AOT; the main-source copy also
passes with the isolated runtime. Existing date, extended-year, timezone-parser
and date-format suites pass in all four modes. Their 16 unchanged results plus
the four reviewed new-suite results are recorded in
`/tmp/wsdl-p3-44-core-final-verification.json`: 108 cases/42,416 reported assertions.
Initial failed logs remain separate from that final tested union.

Valgrind uses QORE_PCRE2_NO_JIT=1 and qore -b --enable-debug --exec-mode=ast.
Both the new suite and existing date suite report zero errors and zero definitely,
indirectly or possibly lost bytes. The previously recorded core DWARF-reader
warning remains visible in both logs; these instrumentation runs are not claimed
warning-free. It is not suppressed or newly introduced by this delta.
Logs: `/tmp/wsdl-p3-44-core-valgrind-new.log` and
`/tmp/wsdl-p3-44-core-valgrind-date.log`. Focused QPP/native Doxygen logs are empty;
the example and independent Python zoneinfo check pass. Fixture SHA-256:
`68539b62935c6205af0f2ae098c79e1007477784416eddd6aaca59f05d81fd67`.
The exact source/test/log manifest is `/tmp/wsdl-p3-44-core-final-manifest.json`.

The XML native-time formatter is a separate uncommitted prototype. Its supplied
native values pass the integration matrix, but review exposed its dependency on
preserving absent XML timezones. No complete XML temporal claim is made here.

|!Check|!Status|!Evidence
|1. Entry exists in `doxygen/lang/120_modules.dox.tmpl` (for modules in the Qore repo; N/A for external module repos)|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|2. Entry exists in `doxygen/lang/900_release_notes.dox.tmpl` (for modules in the Qore repo; external modules have release notes in their .qm)|Pass|Qore 3.0 notes document instant-specific date offsets, relative-date behavior and exact offset serialization.
|3. `qore_user_module()` or `qore_external_user_module()` call in `CMakeLists.txt`|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|4. Module added to QMOD list in `CMakeLists.txt`|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|5. `.qm` file has `@section <lowercasemodname>intro` as first doc section — **must be all lowercase** (e.g., `avrodataproviderintro`, not `AvroDataProviderintro`)|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|6. `%modern` in `.qm` file — no redundant `%new-style`, `%require-types`, `%strict-args`, `%enable-all-warnings`|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|7. No parse directives (`%requires`, `%modern`, `%new-style`) in separated `.qc` files (check OUTSIDE of `@code` blocks only)|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|8. No `%include` usage (deprecated for modules)|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|9. Copyright 2026 on all new files|Pass|New test/design/audit have 2026 notices; changed native notices include 2026. The authored binary fixture is described with provenance in the design.
|10. Directory layout: `.qm` inside `qlib/<ModuleName>/` directory (not at `qlib/<ModuleName>.qm` for multi-file modules)|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|11. No second `.qm` for the same module at `qlib/<ModuleName>.qm`|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|12. `ns=Qore::XX` matches the QoreNamespace constructor path|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|13. `%modern` directive present|Pass|date-utc-offset.qtest uses %modern.
|14. Executable permission set (`chmod +x`)|Pass|New qtest is executable (0755).
|15. Uses %prepend-module-path  before %requires for in-repo modules (Qore and Qore modules only; not Qorus)|Pass|Local qlib prepend precedes the relative ../../../../qlib/QUnit.qm requirement. Fixture paths are canonicalized so absolute and relative test invocation both work.
|16. External module dependencies use `%try-module` — except modules delivered with the project itself (Qore ex: DataProvider, ConnectionProvider, QUnit, etc.) which use hard `%requires`|Pass|Only in-repository QUnit and core builtins are required; no optional external module.
|17. No filesystem operations (fopen, open, creat, unlink, remove, rename, mkdir, rmdir, stat, chmod) without sandbox checks|Pass|No new native filesystem operation. The existing TimeZone constructor retains its sandbox-aware path loading; the offset selector reads already loaded type records.
|18. No network operations (connect, bind, socket, getaddrinfo, gethostbyname) without sandbox checks|Pass|No network operation added.
|19. If filesystem/network ops exist, verify `QoreSandboxManagerHelper` usage|N/A|No new I/O boundary or sandbox helper is needed for the in-memory offset selection/accessor.
|20. No `File::`, `Dir::`, `Socket::`, `HTTPClient::` usage without justification|Pass|The test reads an authored local TZif through TimeZone; its sandbox explicitly permits that exact path. No direct File/Dir/Socket/HTTPClient operations added.
|21. All `for`/`while` loops that could iterate >100 times have `qore_check_cancel()` checks|Pass|The modified standard-type scan checks qore_check_cancel every 100 iterations; the accessor has no loop.
|22. Uses `qore_check_cancel()` (NOT deprecated `qore_check_io_interrupt()`)|Pass|Uses qore_check_cancel, with no deprecated interrupt API.
|23. Check frequency: every 100 iterations for tight loops, every 10 for expensive iterations|Pass|One check per 100 descending type indices; no expensive operation added within the loop.
|24. No blocking operations without cancellation support|Pass|No blocking operation added; existing managed timezone loading remains responsible for I/O cancellation.
|25. Every action has `display_name`, `short_desc` (plain text, <80 chars), `desc` (markdown)|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|26. Every action has `options` populated via `getActionOptionFromFields()` — without this, the action shows an empty, unusable form|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|27. Every action has `output_type` set to a typed data type constant (e.g., `MyResponseDataType`) — not omitted|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|28. DPAT_API actions: provider has `"supports_request": True` and implements `doRequestImpl()`|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|29. DPAT_FIND actions: every option exists in `SearchOptions`, `getRecordTypeImpl()` returns `*hash<string, AbstractDataField>`|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|30. Scheme-based apps (with `"scheme"` in registerApp): actions use `"path"` and do NOT use `"cls"` — having both `scheme` and `cls` causes a runtime error|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|31. Single-key hash slices use trailing comma: `Fields{"key",}` (without trailing comma, `Fields{"key"}` returns the value, not a hash)|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|32. **Typed data type classes exist** for request and response types — inherit `HashDataType`, have `const Fields` hash, call `addQoreFields(Fields)` in constructor, export public constant at bottom (e.g., `public const MyDataType = new MyDataType();`)|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|33. Request/input types use `public` Fields (enables `ClassName::Fields` in action registration)|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|34. Response/output types use `private` Fields|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|35. Each field in data types has `display_name`, `type`, and `desc` (markdown-formatted)|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|36. Input fields have `example_value` where useful (string fields, endpoint URIs, SQL queries, etc.)|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|37. Fields with finite allowed values use `allowed_values` with `AllowedValueInfo` containing both `value` and `display_name` (Title Case, human-readable) — never bare values, never described only in text|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|38. Password/secret fields have `"sensitive": True`|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|39. `groups` uses `AppGroup` enum values from `qlib/DataProvider/AppGroup.qc`|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|40. App `logo` stored as separate file, loaded at module level in `Priv` namespace|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|41. App `desc` uses markdown: bullet list of capabilities, links to project website, business-language explanation of value|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|42. `display_name` is user-friendly ("Apache Avro" not "avro")|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|43. `short_desc` is plain text, under 80 chars, single sentence — no markdown|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|44. `desc` uses markdown: backticks for code/field refs (`` `field_name` ``, `` `True` ``, `` `pdf` ``), `\n\n` for paragraphs, `- ` bullet lists for enumerations, `**bold**` for caveats|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|45. Descriptions use plain business language relating to common challenges — not just technical "what" but "why" and "when to use"|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|46. No bare `True`/`False`/`NOTHING` — must be backtick-wrapped in `desc`|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|47. No bare field/option names in prose — must use backticks|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|48. Long descriptions (>500 chars) use bold section headers and bullet lists|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|49. **Factory registration in Qore repo**: every factory name registered in `qlib/DataProvider/DataProvider.qc` → `FactoryMap` (without this, module loads but doesn't appear in Qorus apps)|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|50. **`getRecordTypeImpl()` signature**: must be `private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options)` — NOT returning `*AbstractDataProviderType`|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|51. **Dependency JARs committed** (for JNI modules): JAR files in `qlib/*/jar/` may be gitignored — use `git add -f` to ensure they're tracked, otherwise CI compilation fails|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|52. JAR install rules in CMakeLists.txt for all dependency JARs|N/A|No new module, QPP class/namespace, DataProvider action/app/field/factory, separated module or JAR dependency.
|53. **No workarounds**: No TODOs, FIXMEs, stubs, or partially-implemented features|Pass|Root causes are corrected: actual date epoch is supplied for zone lookup, and no integer offset is reserved as an unset marker. No sentinel substitution or fixture-specific conditional.
|54. **Exception safety**: C++ uses `ReferenceHolder` for Qore allocations, `std::unique_ptr` for C++ allocations, `*xsink` checked after every fallible operation|Pass|Accessor allocates nothing. Cancelled construction returns with xsink set and valid=false, retaining existing RAII cleanup of file/type records. Program interruption/recovery and both Valgrind suites pass.
|55. **Thread safety**: All mutable shared state protected by `std::lock_guard<std::mutex>` or documented as immutable-after-construction|Pass|Zone metadata is immutable after construction and published through the existing locked manager. The accessor only reads the date and immutable zone.
|56. **Type safety**: Strongly-typed `code<return(args)>` instead of untyped `code`; `static_cast` instead of C casts; typed hashdecls for results; enums where appropriate|Pass|Existing date/epoch/int native types are retained; typed code<nothing()> test callback. No casts, layouts or signatures changed.
|57. **Performance**: No O(n²) where O(n) is possible; no unnecessary copies; coordinate descent uses incremental residuals not full matrix multiply|Pass|Standard-type scan remains linear and stops at the first eligible type. Date offset lookup reuses the existing transition lookup; no copies or allocation added.
|58. **Error handling**: All inputs validated (dimensions, empty data, unfitted models); C++ I/O handles EAGAIN/EINTR if applicable|Pass|Relative dates return the documented -1; absolute -1-second offsets stay valid. Fixed/region/serialized cases, both DST boundaries, epochs around zero and recovery are covered.
|59. **Documentation**: Doxygen `@param`, `@return`, `@throw` on all public methods; `@par Example` with realistic business scenarios; `@note` for important caveats|Pass|Accessor @return, @note and business example are updated; nonthrowing no-argument method needs no @param/@throw. Both focused Doxygen runs are diagnostic-free and the example executes.
|60. **QPP flags**: `[flags=CONSTANT]` on methods that never throw; `[flags=RET_VALUE_ONLY]` on methods that throw but have no side effects|Pass|Accessor keeps const and flags=CONSTANT; it is deterministic for the immutable date/zone and cannot throw.
|61. **Security**: No user-controlled format strings; no buffer overflows; bounds checking on array indices; no credentials in code|Pass|No format string from input, raw buffer operation, credential or new index introduced. Descending scan decrements only when its index is nonzero.
|62. **Correctness**: Algorithms verified against reference implementations; edge cases tested (empty data, single sample, all-zero features)|Pass|New suite 5 cases/611 assertions in all four modes plus AOT. Final tested union: 20 suite/mode runs, 108 cases, 42,416 reported assertions. Python zoneinfo confirms eight transition boundaries and the authored TZif; both Valgrind suites have zero errors and lost bytes.
