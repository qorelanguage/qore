# Quoted multipart boundary fixture audit

Copyright (C) 2026 Qore Technologies, s.r.o.

Scope: `examples/test/qlib/HttpServer/HttpServer.qtest` and this audit only.
The test handler's `(.+)` captured the closing quote after Mime began emitting
quoted boundary parameters. The parser then searched for a delimiter containing
that quote and returned HTTP 500. Exclude quotes and subsequent MIME parameters
from the capture and assert that the generated boundary is quoted. The rebase
retains the upstream semicolon exclusion together with the local regression
assertion and relative imports. This restores the existing binary upload
regression without changing Mime or production HTTP code.

All imports in the changed test are now explicitly relative. Separated modules
use the directory path so their .qc components are included; naming only their
entry .qm file would load an incomplete module.

Verification after conflict resolution (2026-09-08): 63 cases / 480 assertions
pass with debugging enabled and no warnings or errors. The first run exposed a
stale RestClient qmod after the upstream source update; rebuilding RestClient's
qmod and rerunning the complete HTTP suite removed that warning.

```bash
QORE_MODULE_DIR=build/modules/json:build/modules/reflection:qlib \
LD_LIBRARY_PATH=build-debug build-debug/qore --enable-debug \
examples/test/qlib/HttpServer/HttpServer.qtest
```

Log: `/tmp/qore-pull-rebase/http-server.log`.
No C++ code changes in this resolution, so a new Valgrind run is not applicable.
The remaining native-thread and AOT commits retain their previously audited
patches; their implementation files have no overlapping upstream edits.

Applied the full audit-changes skill and relevant module structure, sandboxing
and cancellation design checks. No new module/provider metadata is introduced.

|!Check|!Status|!Evidence
|1. Entry exists in `doxygen/lang/120_modules.dox.tmpl` (for modules in the Qore repo; N/A for external module repos)|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|2. Entry exists in `doxygen/lang/900_release_notes.dox.tmpl` (for modules in the Qore repo; external modules have release notes in their .qm)|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|3. `qore_user_module()` or `qore_external_user_module()` call in `CMakeLists.txt`|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|4. Module added to QMOD list in `CMakeLists.txt`|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|5. `.qm` file has `@section <lowercasemodname>intro` as first doc section — **must be all lowercase** (e.g., `avrodataproviderintro`, not `AvroDataProviderintro`)|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|6. `%modern` in `.qm` file — no redundant `%new-style`, `%require-types`, `%strict-args`, `%enable-all-warnings`|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|7. No parse directives (`%requires`, `%modern`, `%new-style`) in separated `.qc` files (check OUTSIDE of `@code` blocks only)|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|8. No `%include` usage (deprecated for modules)|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|9. Copyright 2026 on all new files|Pass|2026 copyright, %modern, executable mode 100755, prepend local qlib, and explicit relative imports. Separated modules use directory paths; flat modules use .qm paths. No external dependency guard is added.
|10. Directory layout: `.qm` inside `qlib/<ModuleName>/` directory (not at `qlib/<ModuleName>.qm` for multi-file modules)|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|11. No second `.qm` for the same module at `qlib/<ModuleName>.qm`|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|12. `ns=Qore::XX` matches the QoreNamespace constructor path|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|13. `%modern` directive present|Pass|2026 copyright, %modern, executable mode 100755, prepend local qlib, and explicit relative imports. Separated modules use directory paths; flat modules use .qm paths. No external dependency guard is added.
|14. Executable permission set (`chmod +x`)|Pass|2026 copyright, %modern, executable mode 100755, prepend local qlib, and explicit relative imports. Separated modules use directory paths; flat modules use .qm paths. No external dependency guard is added.
|15. Uses %prepend-module-path  before %requires for in-repo modules (Qore and Qore modules only; not Qorus)|Pass|2026 copyright, %modern, executable mode 100755, prepend local qlib, and explicit relative imports. Separated modules use directory paths; flat modules use .qm paths. No external dependency guard is added.
|16. External module dependencies use `%try-module` — except modules delivered with the project itself (Qore ex: DataProvider, ConnectionProvider, QUnit, etc.) which use hard `%requires`|Pass|2026 copyright, %modern, executable mode 100755, prepend local qlib, and explicit relative imports. Separated modules use directory paths; flat modules use .qm paths. No external dependency guard is added.
|17. No filesystem operations (fopen, open, creat, unlink, remove, rename, mkdir, rmdir, stat, chmod) without sandbox checks|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|18. No network operations (connect, bind, socket, getaddrinfo, gethostbyname) without sandbox checks|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|19. If filesystem/network ops exist, verify `QoreSandboxManagerHelper` usage|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|20. No `File::`, `Dir::`, `Socket::`, `HTTPClient::` usage without justification|Pass|Existing HTTP client/server test uses loopback ephemeral listeners; the change repairs only its multipart boundary extraction and adds a wire-header assertion.
|21. All `for`/`while` loops that could iterate >100 times have `qore_check_cancel()` checks|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|22. Uses `qore_check_cancel()` (NOT deprecated `qore_check_io_interrupt()`)|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|23. Check frequency: every 100 iterations for tight loops, every 10 for expensive iterations|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|24. No blocking operations without cancellation support|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|25. Every action has `display_name`, `short_desc` (plain text, <80 chars), `desc` (markdown)|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|26. Every action has `options` populated via `getActionOptionFromFields()` — without this, the action shows an empty, unusable form|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|27. Every action has `output_type` set to a typed data type constant (e.g., `MyResponseDataType`) — not omitted|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|28. DPAT_API actions: provider has `"supports_request": True` and implements `doRequestImpl()`|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|29. DPAT_FIND actions: every option exists in `SearchOptions`, `getRecordTypeImpl()` returns `*hash<string, AbstractDataField>`|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|30. Scheme-based apps (with `"scheme"` in registerApp): actions use `"path"` and do NOT use `"cls"` — having both `scheme` and `cls` causes a runtime error|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|31. Single-key hash slices use trailing comma: `Fields{"key",}` (without trailing comma, `Fields{"key"}` returns the value, not a hash)|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|32. **Typed data type classes exist** for request and response types — inherit `HashDataType`, have `const Fields` hash, call `addQoreFields(Fields)` in constructor, export public constant at bottom (e.g., `public const MyDataType = new MyDataType();`)|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|33. Request/input types use `public` Fields (enables `ClassName::Fields` in action registration)|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|34. Response/output types use `private` Fields|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|35. Each field in data types has `display_name`, `type`, and `desc` (markdown-formatted)|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|36. Input fields have `example_value` where useful (string fields, endpoint URIs, SQL queries, etc.)|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|37. Fields with finite allowed values use `allowed_values` with `AllowedValueInfo` containing both `value` and `display_name` (Title Case, human-readable) — never bare values, never described only in text|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|38. Password/secret fields have `"sensitive": True`|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|39. `groups` uses `AppGroup` enum values from `qlib/DataProvider/AppGroup.qc`|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|40. App `logo` stored as separate file, loaded at module level in `Priv` namespace|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|41. App `desc` uses markdown: bullet list of capabilities, links to project website, business-language explanation of value|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|42. `display_name` is user-friendly ("Apache Avro" not "avro")|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|43. `short_desc` is plain text, under 80 chars, single sentence — no markdown|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|44. `desc` uses markdown: backticks for code/field refs (`` `field_name` ``, `` `True` ``, `` `pdf` ``), `\n\n` for paragraphs, `- ` bullet lists for enumerations, `**bold**` for caveats|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|45. Descriptions use plain business language relating to common challenges — not just technical "what" but "why" and "when to use"|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|46. No bare `True`/`False`/`NOTHING` — must be backtick-wrapped in `desc`|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|47. No bare field/option names in prose — must use backticks|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|48. Long descriptions (>500 chars) use bold section headers and bullet lists|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|49. **Factory registration in Qore repo**: every factory name registered in `qlib/DataProvider/DataProvider.qc` → `FactoryMap` (without this, module loads but doesn't appear in Qorus apps)|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|50. **`getRecordTypeImpl()` signature**: must be `private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options)` — NOT returning `*AbstractDataProviderType`|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|51. **Dependency JARs committed** (for JNI modules): JAR files in `qlib/*/jar/` may be gitignored — use `git add -f` to ensure they're tracked, otherwise CI compilation fails|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|52. JAR install rules in CMakeLists.txt for all dependency JARs|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|53. **No workarounds**: No TODOs, FIXMEs, stubs, or partially-implemented features|Pass|Fixes the fixture regex that greedily captured the closing MIME parameter quote and preserves upstream semicolon handling; no runtime workaround or skipped case.
|54. **Exception safety**: C++ uses `ReferenceHolder` for Qore allocations, `std::unique_ptr` for C++ allocations, `*xsink` checked after every fallible operation|Pass|Only a regex character class and assertion change; existing server/client scope cleanup is retained. Full suite passes.
|55. **Thread safety**: All mutable shared state protected by `std::lock_guard<std::mutex>` or documented as immutable-after-construction|Pass|No shared mutable state added. Extraction uses request-local header data; assertion uses the local generated message.
|56. **Type safety**: Strongly-typed `code<return(args)>` instead of untyped `code`; `static_cast` instead of C casts; typed hashdecls for results; enums where appropriate|Pass|Existing typed string and MessageInfo hash; boolean assertion. Relative imports resolve local source components.
|57. **Performance**: No O(n²) where O(n) is possible; no unnecessary copies; coordinate descent uses incremental residuals not full matrix multiply|Pass|Single bounded header extraction; no additional request or production runtime cost.
|58. **Error handling**: All inputs validated (dimensions, empty data, unfitted models); C++ I/O handles EAGAIN/EINTR if applicable|Pass|Existing 63-case suite covers HTTP failure/timeout/shutdown paths. Multipart upload now reaches all five assertions, including status, MIME type, exact binary bytes and chunking.
|59. **Documentation**: Doxygen `@param`, `@return`, `@throw` on all public methods; `@par Example` with realistic business scenarios; `@note` for important caveats|Pass|Inline root-cause comment and this audit document the test-only correction. No public behavior/API requires a release note.
|60. **QPP flags**: `[flags=CONSTANT]` on methods that never throw; `[flags=RET_VALUE_ONLY]` on methods that throw but have no side effects|N/A|Test fixture only; no module, QPP class, native implementation, provider registration or build/dependency change.
|61. **Security**: No user-controlled format strings; no buffer overflows; bounds checking on array indices; no credentials in code|Pass|No credentials or user-supplied format string. Existing loopback-only integration and exact binary equality assertion remain.
|62. **Correctness**: Algorithms verified against reference implementations; edge cases tested (empty data, single sample, all-zero features)|Pass|All 63 cases / 480 assertions pass without warnings after resolving the conflict and rebuilding RestClient. The boundary quote and subsequent parameters are excluded; the actual generated quoted header is asserted.

Reviewed test SHA-256: `1673031721a4a7fe4e95d6deb3b5e3faad74dda0082438f1ee190071e316488f`.

## Summary

|!Check|!Status|!Findings
|Module docs (120_modules.dox.tmpl)|N/A|No new module.
|Release notes (900_release_notes.dox.tmpl)|N/A|Test fixture correction only.
|CMakeLists registration|N/A|No build registration change.
|Module structure (@section, %modern, layout)|Pass|Modern executable test; relative imports after local module path.
|Sandboxing|Pass|Existing loopback HTTP integration; no new native I/O.
|Cooperative cancellation|N/A|No native loop or blocking operation added.
|DP: Action registration (output_type, options)|N/A|No provider changes.
|DP: Data types (HashDataType, Fields, examples)|N/A|No provider changes.
|DP: Allowed values (AllowedValueInfo)|N/A|No provider changes.
|DP: Description quality (markdown, business lang)|N/A|No provider changes.
|DP: Scheme-based app actions (no cls)|N/A|No provider changes.
|FactoryMap registration (Qore repo)|N/A|No provider changes.
|Dependency JARs committed (JNI modules)|N/A|No JNI changes.
|Workarounds/stubs|Pass|Correct MIME boundary extraction; no skipped cases.
|Exception safety|Pass|Existing client/server scope cleanup retained.
|Thread safety|Pass|Request-local data only.
|Type safety|Pass|Typed boundary string, MessageInfo and boolean assertion.
|Performance|Pass|Single bounded header scan.
|Error handling|Pass|Existing negative HTTP cases pass.
|Documentation|Pass|Root-cause comment and audit updated.
|QPP flags|N/A|No QPP changes.
|Security (sensitive passwords, no credentials)|Pass|No new credentials or format strings.
|Correctness|Pass|63 cases / 480 assertions pass without warnings.
