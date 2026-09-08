# Indexed container deserialization audit

Copyright (C) 2026 Qore Technologies, s.r.o.

Scope: `lib/QoreSerializable.cpp`, `lib/TypedHashDecl.cpp`,
`IndexedContainerErrors.qtest`, the ownership design and the Qore 3.0 release note.
All 62 items from `/home/david/.codex/skills/audit-changes/SKILL.md` are recorded
below. Module structure, sandboxing and cancellation guides were consulted;
DataProvider checks are inapplicable to this core-only change.

The index owns each preallocated hash/list. Helpers previously adopted this
borrowed reference and could release it on a failed member conversion, leaving
the index cleanup to dereference freed storage. Helpers now acquire a separate
reference, and indexed callers consume the owned result with `ValueHolder`.
The typed-hash helper has only one caller supplying an in-place target: the
serializer. Other callers retain their existing allocation behavior.

Validation used the isolated `/tmp/wsdl-core-date/build-debug` runtime, configured
as Debug with `/usr` prefix and jemalloc disabled. Both changed C++ files in the
main checkout are byte-identical to the compiled files. Library SHA-256:
`fc3a60454896701c17913c4ed768678959f9788c059be20c334d72a43ab2ac80`.
No installation or push was performed.

- Baseline regression: exit 139. Fixed regression: 4 cases / 17 assertions;
  existing Serializable: 8 cases / 132 assertions, each passing in AST, IR, JIT
  and tiered modes with debugging enabled (48 cases / 596 assertions total).
- Existing hashdecl suite: 10 cases / 220 assertions. The copied regression also
  passes from the main checkout against the tested runtime.
- New and existing Serializable suites and the affected XML union suite pass
  Valgrind with `qore -b --enable-debug --exec-mode=ast` and
  `QORE_PCRE2_NO_JIT=1`: zero errors, zero definite/indirect/possible losses,
  no suppressions. Valgrind's previously tracked `DW_AT_abstract_origin` reader
  warning remains an environment finding; reachable library state is reported.
- All 57 affected XML suites pass (668 cases / 10983 assertions). The expanded
  union regression passes 10 cases / 293 assertions, including malformed provider
  metadata, unexpected-error propagation and recovery.
- Both independent XML union matrix methods pass with libxml2 and Xerces.
  SOAP 1.1/1.2 survey and strict coverage reports are identical to P3-13 before
  this core fix, excluding version metadata. Strict selection retains 89 WSDLs,
  756 message directions and zero selected failures. The 220 tracked later-phase
  diagnostic failures remain visible.
- Incremental native build and ordinary tests emit no compiler/runtime warnings
  or errors. `git diff --check` passes. The already recorded unrelated dependency
  build and debugger-reader diagnostics are not claimed to be resolved here.

Evidence is in `/tmp/wsdl-indexed-errors-{baseline,build,checks,valgrind,hashdecl,
main-test,xml,independent,survey,coverage,xml-valgrind}.log`, per-mode/test logs,
and the survey/coverage JSON files with the same prefix. The XML implementation
remains in progress separately; this audit covers only its core prerequisite.

| Check | Status | Evidence |
| --- | --- | --- |
| 1. Entry exists in `doxygen/lang/120_modules.dox.tmpl` (for modules in the Qore repo; N/A for external module repos) | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 2. Entry exists in `doxygen/lang/900_release_notes.dox.tmpl` (for modules in the Qore repo; external modules have release notes in their .qm) | Pass | Qore 3.0 release notes describe safe cleanup and unchanged errors/reference identity. |
| 3. `qore_user_module()` or `qore_external_user_module()` call in `CMakeLists.txt` | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 4. Module added to QMOD list in `CMakeLists.txt` | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 5. `.qm` file has `@section <lowercasemodname>intro` as first doc section — **must be all lowercase** (e.g., `avrodataproviderintro`, not `AvroDataProviderintro`) | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 6. `%modern` in `.qm` file — no redundant `%new-style`, `%require-types`, `%strict-args`, `%enable-all-warnings` | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 7. No parse directives (`%requires`, `%modern`, `%new-style`) in separated `.qc` files (check OUTSIDE of `@code` blocks only) | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 8. No `%include` usage (deprecated for modules) | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 9. Copyright 2026 on all new files | Pass | New test, design and audit files and both touched native source notices use 2026. |
| 10. Directory layout: `.qm` inside `qlib/<ModuleName>/` directory (not at `qlib/<ModuleName>.qm` for multi-file modules) | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 11. No second `.qm` for the same module at `qlib/<ModuleName>.qm` | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 12. `ns=Qore::XX` matches the QoreNamespace constructor path | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 13. `%modern` directive present | Pass | IndexedContainerErrors.qtest declares %modern. |
| 14. Executable permission set (`chmod +x`) | Pass | The new qtest is executable (0755). |
| 15. Uses %prepend-module-path  before %requires for in-repo modules (Qore and Qore modules only; not Qorus) | Pass | The local qlib prepend precedes the relative hard requirement ../../../../../qlib/QUnit.qm. |
| 16. External module dependencies use `%try-module` — except modules delivered with the project itself (Qore ex: DataProvider, ConnectionProvider, QUnit, etc.) which use hard `%requires` | N/A | The only dependency is in-repository QUnit; no external binary module is required. |
| 17. No filesystem operations (fopen, open, creat, unlink, remove, rename, mkdir, rmdir, stat, chmod) without sandbox checks | Pass | The ownership changes perform no filesystem operation. |
| 18. No network operations (connect, bind, socket, getaddrinfo, gethostbyname) without sandbox checks | Pass | The ownership changes perform no network operation. |
| 19. If filesystem/network ops exist, verify `QoreSandboxManagerHelper` usage | N/A | No filesystem or network operation is introduced. |
| 20. No `File::`, `Dir::`, `Socket::`, `HTTPClient::` usage without justification | N/A | No Qore module implementation changes; the new test uses no direct file/network operations. |
| 21. All `for`/`while` loops that could iterate >100 times have `qore_check_cancel()` checks | N/A | No native loops or traversal logic are added or changed; this change adjusts reference ownership only. |
| 22. Uses `qore_check_cancel()` (NOT deprecated `qore_check_io_interrupt()`) | N/A | No cancellation API or long-running operation is introduced. |
| 23. Check frequency: every 100 iterations for tight loops, every 10 for expensive iterations | N/A | No new or modified native loop requires a check interval. |
| 24. No blocking operations without cancellation support | Pass | Only constant-time reference acquisition/release and RAII management are added; no blocking operation. |
| 25. Every action has `display_name`, `short_desc` (plain text, <80 chars), `desc` (markdown) | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 26. Every action has `options` populated via `getActionOptionFromFields()` — without this, the action shows an empty, unusable form | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 27. Every action has `output_type` set to a typed data type constant (e.g., `MyResponseDataType`) — not omitted | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 28. DPAT_API actions: provider has `"supports_request": True` and implements `doRequestImpl()` | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 29. DPAT_FIND actions: every option exists in `SearchOptions`, `getRecordTypeImpl()` returns `*hash<string, AbstractDataField>` | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 30. Scheme-based apps (with `"scheme"` in registerApp): actions use `"path"` and do NOT use `"cls"` — having both `scheme` and `cls` causes a runtime error | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 31. Single-key hash slices use trailing comma: `Fields{"key",}` (without trailing comma, `Fields{"key"}` returns the value, not a hash) | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 32. **Typed data type classes exist** for request and response types — inherit `HashDataType`, have `const Fields` hash, call `addQoreFields(Fields)` in constructor, export public constant at bottom (e.g., `public const MyDataType = new MyDataType();`) | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 33. Request/input types use `public` Fields (enables `ClassName::Fields` in action registration) | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 34. Response/output types use `private` Fields | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 35. Each field in data types has `display_name`, `type`, and `desc` (markdown-formatted) | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 36. Input fields have `example_value` where useful (string fields, endpoint URIs, SQL queries, etc.) | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 37. Fields with finite allowed values use `allowed_values` with `AllowedValueInfo` containing both `value` and `display_name` (Title Case, human-readable) — never bare values, never described only in text | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 38. Password/secret fields have `"sensitive": True` | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 39. `groups` uses `AppGroup` enum values from `qlib/DataProvider/AppGroup.qc` | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 40. App `logo` stored as separate file, loaded at module level in `Priv` namespace | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 41. App `desc` uses markdown: bullet list of capabilities, links to project website, business-language explanation of value | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 42. `display_name` is user-friendly ("Apache Avro" not "avro") | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 43. `short_desc` is plain text, under 80 chars, single sentence — no markdown | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 44. `desc` uses markdown: backticks for code/field refs (`` `field_name` ``, `` `True` ``, `` `pdf` ``), `\n\n` for paragraphs, `- ` bullet lists for enumerations, `**bold**` for caveats | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 45. Descriptions use plain business language relating to common challenges — not just technical "what" but "why" and "when to use" | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 46. No bare `True`/`False`/`NOTHING` — must be backtick-wrapped in `desc` | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 47. No bare field/option names in prose — must use backticks | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 48. Long descriptions (>500 chars) use bold section headers and bullet lists | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 49. **Factory registration in Qore repo**: every factory name registered in `qlib/DataProvider/DataProvider.qc` → `FactoryMap` (without this, module loads but doesn't appear in Qorus apps) | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 50. **`getRecordTypeImpl()` signature**: must be `private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options)` — NOT returning `*AbstractDataProviderType` | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 51. **Dependency JARs committed** (for JNI modules): JAR files in `qlib/*/jar/` may be gitignored — use `git add -f` to ensure they're tracked, otherwise CI compilation fails | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 52. JAR install rules in CMakeLists.txt for all dependency JARs | N/A | No module entry point, QPP class, provider registration, field metadata, factory or JAR dependency changes. |
| 53. **No workarounds**: No TODOs, FIXMEs, stubs, or partially-implemented features | Pass | Fixes borrowed-reference adoption at the root; no validation bypass, workaround, TODO or stub. |
| 54. **Exception safety**: C++ uses `ReferenceHolder` for Qore allocations, `std::unique_ptr` for C++ allocations, `*xsink` checked after every fallible operation | Pass | Each helper independently owns its RAII reference; indexed callers own returned values with ValueHolder and inspect the sink. Early validation never consumes the borrowed target. Success/error paths and all supplied-target callers were traced. |
| 55. **Thread safety**: All mutable shared state protected by `std::lock_guard<std::mutex>` or documented as immutable-after-construction | Pass | No shared mutable state is added. Each deserialization context owns its index and uses existing node reference counting. |
| 56. **Type safety**: Strongly-typed `code<return(args)>` instead of untyped `code`; `static_cast` instead of C casts; typed hashdecls for results; enums where appropriate | Pass | Typed ReferenceHolder/ValueHolder and node-specific refSelf methods; no casts, raw unmanaged allocation or untyped callback added. Regression uses typed SerializationInfo and a hashdecl. |
| 57. **Performance**: No O(n²) where O(n) is possible; no unnecessary copies; coordinate descent uses incremental residuals not full matrix multiply | Pass | Constant additional reference-count operations per indexed container; no copies or changes to traversal complexity. |
| 58. **Error handling**: All inputs validated (dimensions, empty data, unfitted models); C++ I/O handles EAGAIN/EINTR if applicable | Pass | Original errors are preserved for invalid typed members, unknown hashdecl keys and missing hash/list references. Recovery after failures, empty lists and shared object identity are checked. No native I/O. |
| 59. **Documentation**: Doxygen `@param`, `@return`, `@throw` on all public methods; `@par Example` with realistic business scenarios; `@note` for important caveats | Pass | No public signature changes. Inline comments and the durable ownership design specify borrowed arguments, owned returns, early failures and empty lists; release notes explain the visible fix. The documented malformed-member example is exercised by the new test. |
| 60. **QPP flags**: `[flags=CONSTANT]` on methods that never throw; `[flags=RET_VALUE_ONLY]` on methods that throw but have no side effects | N/A | No QPP method or flag changes. |
| 61. **Security**: No user-controlled format strings; no buffer overflows; bounds checking on array indices; no credentials in code | Pass | No new format string, buffer arithmetic, index lookup, external I/O or credentials. Malformed serialization now unwinds safely. |
| 62. **Correctness**: Algorithms verified against reference implementations; edge cases tested (empty data, single sample, all-zero features) | Pass | The baseline new regression exits 139; the final regression and existing Serializable pass in all four execution modes, hashdecl tests pass, and three affected suites pass Valgrind. All 57 XML suites and both independent matrix methods pass; corpus reports are unchanged outside versions. |
