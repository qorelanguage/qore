# Failed deserialization graph cleanup audit

Copyright (C) 2026 Qore Technologies, s.r.o.

Audited against parent `35dc29f31` on 2026-09-11 using the complete
`/home/david/.codex/skills/audit-changes/SKILL.md` and its five referenced design
guides. All 62 individual items: 19 Pass, 43 N/A, 0 Fail.

Scope: `lib/QoreSerializable.cpp`, `lib/QoreObject.cpp`,
`include/qore/intern/QoreObjectIntern.h`, `FailedObjectGraphs.qtest`, the ownership
design, release notes and this audit. Tests ran with debugging against the
isolated Debug build at `/tmp/wsdl-core-date/build-debug`, using the installed
`/usr` prefix with jemalloc disabled. The three changed C++ files are byte-identical
between the main checkout and compiled source. Library SHA-256:
`560a59208ae68df3713b20f1e07c8e8ae364d21d5c5506bbffa106e0293337a1`.

The old minimal self-cycle leaked 1232 definite and 14176 indirect bytes; its
acyclic control was clean. The initial regression also fails against the
old runtime because an escaped rejected object remains usable. The final
regression passes 94 assertions in all four execution modes. Cancellation is
requested deterministically by the hook; no sleeps, polling or test suppressions.

All 14 Valgrind commands have zero memory errors and zero definite, indirect or
possible lost blocks. They include the minimal reproducer, four serialization
suites in AST/tiered modes, the full XML wildcard consumer, HTTP and registry
suites, and existing object/class regressions. Valgrind 3.27.1 still emits the
previously recorded `DW_AT_abstract_origin` reader warning from the Debug library;
this is an explicit environment finding, not a clean diagnostic claim. Native
build and ordinary tests have no compiler/runtime warnings or failures.

The full affected XML gate passes 127 suites/1274 cases/63311 reported assertions.
The legacy SOAP test retains its three intentional caught comparator negatives. All 19
AOT, consumer, independent matrix, harness and example supplements pass. Both SOAP
versions retain byte-identical survey and strict coverage reports to XML P5-11;
tracked later-phase failures remain visible. This core prerequisite does not
close the remaining XML P5-P9 requirements.

Commands, per-run summaries and source hashes are in
`/tmp/wsdl-p5-12-{core,valgrind,object-valgrind,acceptance-gate,supplement,final-corpus}.json`;
per-test logs share the corresponding prefixes. The durable XML inventory is
`module-xml/test/wsdl-interop/P5-12-validation.json`. No install or push performed.

|!Check|!Status|!Evidence
|1. Entry exists in `doxygen/lang/120_modules.dox.tmpl` (for modules in the Qore repo; N/A for external module repos)|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|2. Entry exists in `doxygen/lang/900_release_notes.dox.tmpl` (for modules in the Qore repo; external modules have release notes in their .qm)|Pass|Qore 3.0 release notes describe cycle reclamation, escaped-object invalidation and interruption cleanup.
|3. `qore_user_module()` or `qore_external_user_module()` call in `CMakeLists.txt`|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|4. Module added to QMOD list in `CMakeLists.txt`|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|5. `.qm` file has `@section <lowercasemodname>intro` as first doc section — **must be all lowercase** (e.g., `avrodataproviderintro`, not `AvroDataProviderintro`)|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|6. `%modern` in `.qm` file — no redundant `%new-style`, `%require-types`, `%strict-args`, `%enable-all-warnings`|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|7. No parse directives (`%requires`, `%modern`, `%new-style`) in separated `.qc` files (check OUTSIDE of `@code` blocks only)|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|8. No `%include` usage (deprecated for modules)|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|9. Copyright 2026 on all new files|Pass|New test/audit and changed native source notices use 2026; the ownership design already has its 2026 notice.
|10. Directory layout: `.qm` inside `qlib/<ModuleName>/` directory (not at `qlib/<ModuleName>.qm` for multi-file modules)|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|11. No second `.qm` for the same module at `qlib/<ModuleName>.qm`|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|12. `ns=Qore::XX` matches the QoreNamespace constructor path|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|13. `%modern` directive present|Pass|FailedObjectGraphs.qtest and its embedded Program declare %modern.
|14. Executable permission set (`chmod +x`)|Pass|FailedObjectGraphs.qtest is executable, mode 0755.
|15. Uses %prepend-module-path  before %requires for in-repo modules (Qore and Qore modules only; not Qorus)|Pass|Local qlib prepend precedes the relative ../../../../../qlib/QUnit.qm requirement.
|16. External module dependencies use `%try-module` — except modules delivered with the project itself (Qore ex: DataProvider, ConnectionProvider, QUnit, etc.) which use hard `%requires`|N/A|Only project-owned QUnit is required; no external module is needed.
|17. No filesystem operations (fopen, open, creat, unlink, remove, rename, mkdir, rmdir, stat, chmod) without sandbox checks|Pass|Member invalidation and reference cleanup introduce no filesystem operations.
|18. No network operations (connect, bind, socket, getaddrinfo, gethostbyname) without sandbox checks|Pass|No native network operation is introduced.
|19. If filesystem/network ops exist, verify `QoreSandboxManagerHelper` usage|N/A|No filesystem/network operation needs a sandbox helper.
|20. No `File::`, `Dir::`, `Socket::`, `HTTPClient::` usage without justification|Pass|The new Qore regression performs no file or network I/O; Program/SandboxManager exercise cancellation.
|21. All `for`/`while` loops that could iterate >100 times have `qore_check_cancel()` checks|N/A|The only new traversal is destructor unwinding. It must visit every retained owner even with cancellation pending; an early return would leak references. Entry and post-hook cancellation checks transfer execution into this cleanup path.
|22. Uses `qore_check_cancel()` (NOT deprecated `qore_check_io_interrupt()`)|Pass|Uses qore_check_cancel on deserialization entry and after each native/custom hook, preserving an existing hook exception with short-circuit evaluation.
|23. Check frequency: every 100 iterations for tight loops, every 10 for expensive iterations|N/A|No new ordinary long-running traversal; exception-cleanup passes intentionally finish rather than abandoning owners.
|24. No blocking operations without cancellation support|Pass|No new blocking I/O. Existing object member locking is retained, and member/native destruction occurs after unlocking.
|25. Every action has `display_name`, `short_desc` (plain text, <80 chars), `desc` (markdown)|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|26. Every action has `options` populated via `getActionOptionFromFields()` — without this, the action shows an empty, unusable form|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|27. Every action has `output_type` set to a typed data type constant (e.g., `MyResponseDataType`) — not omitted|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|28. DPAT_API actions: provider has `"supports_request": True` and implements `doRequestImpl()`|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|29. DPAT_FIND actions: every option exists in `SearchOptions`, `getRecordTypeImpl()` returns `*hash<string, AbstractDataField>`|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|30. Scheme-based apps (with `"scheme"` in registerApp): actions use `"path"` and do NOT use `"cls"` — having both `scheme` and `cls` causes a runtime error|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|31. Single-key hash slices use trailing comma: `Fields{"key",}` (without trailing comma, `Fields{"key"}` returns the value, not a hash)|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|32. **Typed data type classes exist** for request and response types — inherit `HashDataType`, have `const Fields` hash, call `addQoreFields(Fields)` in constructor, export public constant at bottom (e.g., `public const MyDataType = new MyDataType();`)|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|33. Request/input types use `public` Fields (enables `ClassName::Fields` in action registration)|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|34. Response/output types use `private` Fields|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|35. Each field in data types has `display_name`, `type`, and `desc` (markdown-formatted)|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|36. Input fields have `example_value` where useful (string fields, endpoint URIs, SQL queries, etc.)|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|37. Fields with finite allowed values use `allowed_values` with `AllowedValueInfo` containing both `value` and `display_name` (Title Case, human-readable) — never bare values, never described only in text|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|38. Password/secret fields have `"sensitive": True`|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|39. `groups` uses `AppGroup` enum values from `qlib/DataProvider/AppGroup.qc`|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|40. App `logo` stored as separate file, loaded at module level in `Priv` namespace|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|41. App `desc` uses markdown: bullet list of capabilities, links to project website, business-language explanation of value|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|42. `display_name` is user-friendly ("Apache Avro" not "avro")|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|43. `short_desc` is plain text, under 80 chars, single sentence — no markdown|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|44. `desc` uses markdown: backticks for code/field refs (`` `field_name` ``, `` `True` ``, `` `pdf` ``), `\n\n` for paragraphs, `- ` bullet lists for enumerations, `**bold**` for caveats|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|45. Descriptions use plain business language relating to common challenges — not just technical "what" but "why" and "when to use"|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|46. No bare `True`/`False`/`NOTHING` — must be backtick-wrapped in `desc`|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|47. No bare field/option names in prose — must use backticks|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|48. Long descriptions (>500 chars) use bold section headers and bullet lists|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|49. **Factory registration in Qore repo**: every factory name registered in `qlib/DataProvider/DataProvider.qc` → `FactoryMap` (without this, module loads but doesn't appear in Qorus apps)|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|50. **`getRecordTypeImpl()` signature**: must be `private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options)` — NOT returning `*AbstractDataProviderType`|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|51. **Dependency JARs committed** (for JNI modules): JAR files in `qlib/*/jar/` may be gitignored — use `git add -f` to ensure they're tracked, otherwise CI compilation fails|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|52. JAR install rules in CMakeLists.txt for all dependency JARs|N/A|No module, QPP, DataProvider registration, factory, or JNI dependency changes.
|53. **No workarounds**: No TODOs, FIXMEs, stubs, or partially-implemented features|Pass|Fixes the root reference-count cycle: invalidate all incomplete indexed objects before releasing any indexed owner. No skipped case or validation bypass.
|54. **Exception safety**: C++ uses `ReferenceHolder` for Qore allocations, `std::unique_ptr` for C++ allocations, `*xsink` checked after every fallible operation|Pass|Index references retain every target through the first pass. The invalidation primitive consumes no reference and adds no allocations. Storage is detached under lock and released after unlock; second pass consumes exactly one reference per index entry. Original errors and all cleanup paths pass memory checks.
|55. **Thread safety**: All mutable shared state protected by `std::lock_guard<std::mutex>` or documented as immutable-after-construction|Pass|Each index is invocation-local. Existing QoreSafeVarRWWriteLocker protects status/member detachment and recursive-set invalidation; reference counts retain their existing locks. No new mutable global state.
|56. **Type safety**: Strongly-typed `code<return(args)>` instead of untyped `code`; `static_cast` instead of C casts; typed hashdecls for results; enums where appropriate|Pass|Node type is checked before QoreObject access; no new casts or untyped callbacks. Tests use SerializationInfo, ExceptionInfo, typed graph links and typed helper parameters.
|57. **Performance**: No O(n²) where O(n) is possible; no unnecessary copies; coordinate descent uses incremental residuals not full matrix multiply|Pass|Two linear index passes with constant additional storage. Objects are not copied and no quadratic graph traversal is introduced; no benchmark claim.
|58. **Error handling**: All inputs validated (dimensions, empty data, unfitted models); C++ I/O handles EAGAIN/EINTR if applicable|Pass|Original graph error kind/description/argument survive. Rejected automatic members, inherited/custom hooks, late failures after native restoration and cancellation are checked; successful reuse preserves cycles and shared identity.
|59. **Documentation**: Doxygen `@param`, `@return`, `@throw` on all public methods; `@par Example` with realistic business scenarios; `@note` for important caveats|Pass|No public QPP signature changes. The internal primitive documents ownership and destructor exclusion. Durable design includes a self-linked custom-hook example, escaped-object behavior and cancellation boundaries; release notes describe the visible changes.
|60. **QPP flags**: `[flags=CONSTANT]` on methods that never throw; `[flags=RET_VALUE_ONLY]` on methods that throw but have no side effects|N/A|No QPP methods or flags changed.
|61. **Security**: No user-controlled format strings; no buffer overflows; bounds checking on array indices; no credentials in code|Pass|No user-controlled format string, buffer arithmetic, new array indexing, network operation or credential. Malformed serialized graphs cannot retain usable partial objects.
|62. **Correctness**: Algorithms verified against reference implementations; edge cases tested (empty data, single sample, all-zero features)|Pass|Core gate: 20 runs, 130 cases, 1484 assertions, including four execution modes. XML gate: 127 suites, 1274 cases, 63311 assertions. Nineteen AOT/consumer/matrix/harness supplements and byte-identical both-version survey/strict coverage. Fourteen Valgrind runs have zero errors and no lost blocks; the known DWARF-reader warning is retained explicitly.
