# Recursive closure-local JIT audit

Copyright (C) 2026 Qore Technologies, s.r.o.

Scope: `lib/QoreIRToLLVM.cpp`, `RecursiveClosureLocals.qtest`, its Qore scenario
and native driver, the implemented design and release notes. The full
`audit-changes` skill and its module-structure, sandboxing, cancellation and
provider guides were reviewed. No module, provider, QPP class or dependency
registration changes apply.

The native frame wrapper skips closure-use body locals. JIT closure load/store
lowering previously omitted their instantiation, allowing a recursive callee to
reuse an outer frame's binding. The fix mirrors the AOT ownership check and
calls the existing frame-aware helper before accessing an owned body/block local.
Captured outer locals and parameters remain excluded.

Validation used the isolated `/tmp/wsdl-core-date/build-debug`, configured Debug
with the installed `/usr` prefix. Native SHA256:
`745c1da619df1407bda9d75442c9012551a67c91ca4e3c4ab4a6834da5be0b8f`.
The exact modified compiler and new tests/design were copied to the main develop
checkout and byte-compared. The main-copy QUnit runner also passes against this
library. Existing concurrent development and history were preserved; no install
or push was performed.

- The new suite passes **3 cases / 28 assertions**, covering AST/IR/JIT/tiered,
  deterministic native promotion and debug-enabled source-stripped AOT. Its
  scenario asserts nested/sibling/empty traversal, captures, parameters, escaped
  references, unassigned locals, exception cleanup and reuse.
- The native regression fails against original library SHA256
  `80078ec30d71bc618b7bb40991bad63604303f379c3e47e6dd41d58d0dbc56fd`.
  Corrected native checks pass at default optimization and O0, and with
  interprocedural rewriting disabled as an additional diagnostic configuration.
- Six affected existing suites pass: JITSmoke, IRTypedForeach,
  IRWeakReferenceLocalStore, ClosureBlockScopedCapture, vars/closures and
  closure_destruction_order. Five QUnit summaries total **194 cases / 5,237
  assertions**; the standalone typed-foreach matrix also exits successfully.
  No new warnings, failures or skips were added.
- **79 XML suites / 853 cases / 31,947 assertions** pass with the corrected core.
  The pre-existing soap suite accounting remains 1,031 total / 1,028 succeeded
  assertions, with all cases passing. QName mode/zone checks pass **248 cases /
  5,816 assertions**; compiled modules pass **31 cases / 727 assertions**.
- The both-version survey and strict coverage reports differ from the previous
  XML commit only in the WSDL source hash. Strict coverage retains **120 WSDLs /
  1,148 message directions**, zero selected failures, and the same **168** broad
  failures assigned to remaining XML phases.
- The native test host and the Qore scenario under `qore -b --enable-debug
  --exec-mode=jit` both pass Valgrind: **zero errors and zero definite, indirect
  or possible loss**, with no suppressions. PCRE2 JIT is disabled using the
  previously authorized environment setting. The known core DWARF-reader warning
  remains tracked for XML P9; reachable library allocations are reported separately.
- Debug qore/qcc rebuilding emits no compiler warnings or errors. The complete
  design example runs without output. All final diffs pass whitespace checks.

Evidence uses the `/tmp/wsdl-p3-31-` prefix: `core-jit-build.log`,
`recursive-final-baseline.log`, `recursive-qtest-final.log`, `core-main-qtest.log`,
`recursive-o0.log`, `recursive-no-rewrite.log`, `core-regression.json`,
`recursive-valgrind-native.log`, `recursive-valgrind-qore.log`,
`core-design-example-final.log`, `declaration-gate-core-fixed.log`,
`declaration-modes.json`, `declaration-aot.json`, and
`declaration-report-comparison-core-fixed.json`.
The separate full XML Valgrind run first reached its 240-second deadline before
completion; its forced-shutdown leak report is not a completed memory check.
The native/Qore evidence above is complete; the extended XML rerun belongs to
that increment's execution record.

Full checklist: **19 Pass / 43 N/A / 0 Fail**.

| Check | Status | Evidence |
| --- | --- | --- |
| 1. Entry exists in `doxygen/lang/120_modules.dox.tmpl` (for modules in the Qore repo; N/A for external module repos) | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 2. Entry exists in `doxygen/lang/900_release_notes.dox.tmpl` (for modules in the Qore repo; external modules have release notes in their .qm) | Pass | The Qore 3.0 release notes describe recursive reference/closure local preservation and cleanup. |
| 3. `qore_user_module()` or `qore_external_user_module()` call in `CMakeLists.txt` | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 4. Module added to QMOD list in `CMakeLists.txt` | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 5. `.qm` file has `@section <lowercasemodname>intro` as first doc section — **must be all lowercase** (e.g., `avrodataproviderintro`, not `AvroDataProviderintro`) | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 6. `%modern` in `.qm` file — no redundant `%new-style`, `%require-types`, `%strict-args`, `%enable-all-warnings` | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 7. No parse directives (`%requires`, `%modern`, `%new-style`) in separated `.qc` files (check OUTSIDE of `@code` blocks only) | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 8. No `%include` usage (deprecated for modules) | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 9. Copyright 2026 on all new files | Pass | All new test, design and audit files use Copyright 2026; the modified compiler already has 2026. |
| 10. Directory layout: `.qm` inside `qlib/<ModuleName>/` directory (not at `qlib/<ModuleName>.qm` for multi-file modules) | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 11. No second `.qm` for the same module at `qlib/<ModuleName>.qm` | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 12. `ns=Qore::XX` matches the QoreNamespace constructor path | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 13. `%modern` directive present | Pass | The executable QUnit runner and scenario use %modern; invocations enable debug, including the native host and AOT compile option. |
| 14. Executable permission set (`chmod +x`) | Pass | The .qtest and .qr files have executable mode 0755. |
| 15. Uses %prepend-module-path  before %requires for in-repo modules (Qore and Qore modules only; not Qorus) | Pass | The runner prepends the local qlib path before relative requirements for QUnit.qm and FsUtil.qm. |
| 16. External module dependencies use `%try-module` — except modules delivered with the project itself (Qore ex: DataProvider, ConnectionProvider, QUnit, etc.) which use hard `%requires` | N/A | Only in-repository Qore modules are required; there is no external binary module dependency. |
| 17. No filesystem operations (fopen, open, creat, unlink, remove, rename, mkdir, rmdir, stat, chmod) without sandbox checks | Pass | Compiler lowering performs no filesystem I/O. The native test loads only its explicitly supplied fixture through QoreProgram::parseFile. |
| 18. No network operations (connect, bind, socket, getaddrinfo, gethostbyname) without sandbox checks | Pass | No network operations were added in production or test code. |
| 19. If filesystem/network ops exist, verify `QoreSandboxManagerHelper` usage | N/A | No production filesystem or network operation was added. |
| 20. No `File::`, `Dir::`, `Socket::`, `HTTPClient::` usage without justification | Pass | The test reads its own temporary command log and uses FsUtil TmpDir for deterministic resource cleanup; no production Qore I/O changes. |
| 21. All `for`/`while` loops that could iterate >100 times have `qore_check_cancel()` checks | Pass | No new production loops. Both native test loops have exactly three fixed phases, below the cancellation-check threshold. |
| 22. Uses `qore_check_cancel()` (NOT deprecated `qore_check_io_interrupt()`) | N/A | No new production loop or cancellation call; native test loops have only three phases. |
| 23. Check frequency: every 100 iterations for tight loops, every 10 for expensive iterations | N/A | No new production loop or cancellation call; native test loops have only three phases. |
| 24. No blocking operations without cancellation support | Pass | Production only calls the existing nonblocking local-instantiation helper. The native test uses the compiler completion event at fixed stages, without sleeps or polling. |
| 25. Every action has `display_name`, `short_desc` (plain text, <80 chars), `desc` (markdown) | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 26. Every action has `options` populated via `getActionOptionFromFields()` — without this, the action shows an empty, unusable form | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 27. Every action has `output_type` set to a typed data type constant (e.g., `MyResponseDataType`) — not omitted | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 28. DPAT_API actions: provider has `"supports_request": True` and implements `doRequestImpl()` | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 29. DPAT_FIND actions: every option exists in `SearchOptions`, `getRecordTypeImpl()` returns `*hash<string, AbstractDataField>` | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 30. Scheme-based apps (with `"scheme"` in registerApp): actions use `"path"` and do NOT use `"cls"` — having both `scheme` and `cls` causes a runtime error | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 31. Single-key hash slices use trailing comma: `Fields{"key",}` (without trailing comma, `Fields{"key"}` returns the value, not a hash) | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 32. **Typed data type classes exist** for request and response types — inherit `HashDataType`, have `const Fields` hash, call `addQoreFields(Fields)` in constructor, export public constant at bottom (e.g., `public const MyDataType = new MyDataType();`) | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 33. Request/input types use `public` Fields (enables `ClassName::Fields` in action registration) | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 34. Response/output types use `private` Fields | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 35. Each field in data types has `display_name`, `type`, and `desc` (markdown-formatted) | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 36. Input fields have `example_value` where useful (string fields, endpoint URIs, SQL queries, etc.) | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 37. Fields with finite allowed values use `allowed_values` with `AllowedValueInfo` containing both `value` and `display_name` (Title Case, human-readable) — never bare values, never described only in text | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 38. Password/secret fields have `"sensitive": True` | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 39. `groups` uses `AppGroup` enum values from `qlib/DataProvider/AppGroup.qc` | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 40. App `logo` stored as separate file, loaded at module level in `Priv` namespace | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 41. App `desc` uses markdown: bullet list of capabilities, links to project website, business-language explanation of value | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 42. `display_name` is user-friendly ("Apache Avro" not "avro") | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 43. `short_desc` is plain text, under 80 chars, single sentence — no markdown | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 44. `desc` uses markdown: backticks for code/field refs (`` `field_name` ``, `` `True` ``, `` `pdf` ``), `\n\n` for paragraphs, `- ` bullet lists for enumerations, `**bold**` for caveats | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 45. Descriptions use plain business language relating to common challenges — not just technical "what" but "why" and "when to use" | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 46. No bare `True`/`False`/`NOTHING` — must be backtick-wrapped in `desc` | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 47. No bare field/option names in prose — must use backticks | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 48. Long descriptions (>500 chars) use bold section headers and bullet lists | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 49. **Factory registration in Qore repo**: every factory name registered in `qlib/DataProvider/DataProvider.qc` → `FactoryMap` (without this, module loads but doesn't appear in Qorus apps) | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 50. **`getRecordTypeImpl()` signature**: must be `private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options)` — NOT returning `*AbstractDataProviderType` | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 51. **Dependency JARs committed** (for JNI modules): JAR files in `qlib/*/jar/` may be gitignored — use `git add -f` to ensure they're tracked, otherwise CI compilation fails | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 52. JAR install rules in CMakeLists.txt for all dependency JARs | N/A | No new module, QPP class, provider action/app/field/factory or JAR dependency in this change. |
| 53. **No workarounds**: No TODOs, FIXMEs, stubs, or partially-implemented features | Pass | The JIT lowering now creates callee-owned closure storage at load/store sites. No test reordering, JIT disabling, placeholder or compatibility workaround. |
| 54. **Exception safety**: C++ uses `ReferenceHolder` for Qore allocations, `std::unique_ptr` for C++ allocations, `*xsink` checked after every fallible operation | Pass | Existing frame-aware allocation and scope-exit release remain paired. Native test QoreProgramHelper and ValueHolder own all Qore values; every parse/call error is inspected. Exception/reuse and Valgrind checks pass. |
| 55. **Thread safety**: All mutable shared state protected by `std::lock_guard<std::mutex>` or documented as immutable-after-construction | Pass | Lowering ownership sets belong to one compiler instance; runtime bindings remain thread-local. No mutable global state added. Existing closure concurrency and XML concurrent-copy suites pass. |
| 56. **Type safety**: Strongly-typed `code<return(args)>` instead of untyped `code`; `static_cast` instead of C casts; typed hashdecls for results; enums where appropriate | Pass | LocalVar ownership is explicit; test closures and references have concrete types. Native host uses Qore enums and typed RAII holders; no C-style casts or raw owned allocations. |
| 57. **Performance**: No O(n²) where O(n) is possible; no unnecessary copies; coordinate descent uses incremental residuals not full matrix multiply | Pass | Two ownership membership checks and the existing idempotent helper are added only for callee-owned closure accesses. No graph copy or new traversal loop. |
| 58. **Error handling**: All inputs validated (dimensions, empty data, unfitted models); C++ I/O handles EAGAIN/EINTR if applicable | Pass | Tests cover absent/empty input, nested and sibling nodes, uninitialized locals, expected exceptions and repeated use. Native test rejects missing arguments and parse/call errors. |
| 59. **Documentation**: Doxygen `@param`, `@return`, `@throw` on all public methods; `@par Example` with realistic business scenarios; `@note` for important caveats | Pass | Implemented design explains storage ownership and contains an executable example, verified with debug-enabled JIT. No new public runtime API requires parameter documentation. |
| 60. **QPP flags**: `[flags=CONSTANT]` on methods that never throw; `[flags=RET_VALUE_ONLY]` on methods that throw but have no side effects | N/A | No QPP method or public runtime method was added or changed. |
| 61. **Security**: No user-controlled format strings; no buffer overflows; bounds checking on array indices; no credentials in code | Pass | No credentials, unchecked buffer indexing or user-controlled format strings. Test subprocess arguments are shell-quoted individually. |
| 62. **Correctness**: Algorithms verified against reference implementations; edge cases tested (empty data, single sample, all-zero features) | Pass | The original native library fails the exact recursive regression; the corrected Debug library passes the new test, six existing suites, 79 XML suites, all mode/zone checks and unchanged corpus results. Native and Qore Valgrind checks report zero errors/lost bytes. |
