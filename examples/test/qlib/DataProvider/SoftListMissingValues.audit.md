# Audit: missing optional soft-list values

Copyright (C) 2026 Qore Technologies, s.r.o.

Date: 2026-09-10. Scope: `qlib/DataProvider/SoftListDataType.qc`, the DataProvider
3.8 release notes, `SoftListMissingValues.qtest`, the implemented
`design/data-provider-soft-list-values.md`, and this audit.

The full `/home/david/.codex/skills/audit-changes/SKILL.md` was read and applied.
Its module structure, sandboxing, cancellation and DataProvider checklist/development
references were checked for the affected collection API. All 62 checks are resolved:
**20 Pass / 42 N/A / 0 Fail**.

The defect was dispatching absent optional collections to the element validator
as one item. Missing input now chooses a configured collection default first,
validates its items, or returns NOTHING when the collection is optional.
Explicit NULL and explicit missing-valued items retain item validation.
No C++ source is changed; Valgrind is not required for this Qore-only fix.

Validation uses `/tmp/wsdl-core-date/build-debug` with the `/usr` installation
prefix and local library/module paths. The isolated DataProvider-qmod target was
rebuilt after copying the exact changed class. The new test and the six directly
related provider class sources match the main checkout byte for byte; all five
existing provider regression files also match. The main build is left to concurrent
development. The rebuilt compiled module contains 2117 variants and its direct
artifact test passes. Qore signals are disabled and debugging enabled; normal
PCRE2 JIT stays enabled. No system installation or push was performed.

Core logs and complete mode rows are `/tmp/wsdl-p4-09-core-gate.json` and
`/tmp/wsdl-p4-09-core-*.log`; AOT, build and documentation logs use
`/tmp/wsdl-p4-09-softlist-`. XML integration evidence is recorded separately in
the module-xml P4-09 inventory. All affected checks pass without warnings.

|!Check|!Status|!Evidence
|1. Entry exists in `doxygen/lang/120_modules.dox.tmpl` (for modules in the Qore repo; N/A for external module repos)|N/A|Existing DataProvider module; no module catalog entry is added.
|2. Entry exists in `doxygen/lang/900_release_notes.dox.tmpl` (for modules in the Qore repo; external modules have release notes in their .qm)|Pass|DataProvider 3.8 module release notes describe missing collections, defaults, and explicit item validation.
|3. `qore_user_module()` or `qore_external_user_module()` call in `CMakeLists.txt`|N/A|Existing DataProvider-qmod target; no new module registration.
|4. Module added to QMOD list in `CMakeLists.txt`|N/A|No new QMOD target.
|5. `.qm` file has `@section <lowercasemodname>intro` as first doc section — **must be all lowercase** (e.g., `avrodataproviderintro`, not `AvroDataProviderintro`)|N/A|No new module mainpage; existing dataproviderintro is unchanged.
|6. `%modern` in `.qm` file — no redundant `%new-style`, `%require-types`, `%strict-args`, `%enable-all-warnings`|Pass|Main module and executable regression use %modern; redundant separated-file directives were removed.
|7. No parse directives (`%requires`, `%modern`, `%new-style`) in separated `.qc` files (check OUTSIDE of `@code` blocks only)|Pass|SoftListDataType.qc now inherits %modern and reflection from DataProvider.qm; no file-level parse directives remain.
|8. No `%include` usage (deprecated for modules)|Pass|No deprecated %include is introduced.
|9. Copyright 2026 on all new files|Pass|Changed class notice ends in 2026; new test, design and audit have 2026 notices.
|10. Directory layout: `.qm` inside `qlib/<ModuleName>/` directory (not at `qlib/<ModuleName>.qm` for multi-file modules)|N/A|Existing qlib/DataProvider directory layout is unchanged.
|11. No second `.qm` for the same module at `qlib/<ModuleName>.qm`|N/A|No duplicate module entry is added.
|12. `ns=Qore::XX` matches the QoreNamespace constructor path|N/A|No QPP class is added.
|13. `%modern` directive present|Pass|SoftListMissingValues.qtest uses %modern.
|14. Executable permission set (`chmod +x`)|Pass|The new qtest is executable (0755).
|15. Uses %prepend-module-path  before %requires for in-repo modules (Qore and Qore modules only; not Qorus)|Pass|The test prepends local qlib before relative QUnit/DataProvider requirements. An exact copied test runs against the isolated rebuilt module.
|16. External module dependencies use `%try-module` — except modules delivered with the project itself (Qore ex: DataProvider, ConnectionProvider, QUnit, etc.) which use hard `%requires`|Pass|Only Qore-delivered QUnit/DataProvider are required; no external binary dependency is added.
|17. No filesystem operations (fopen, open, creat, unlink, remove, rename, mkdir, rmdir, stat, chmod) without sandbox checks|N/A|No C++ or filesystem operation is changed.
|18. No network operations (connect, bind, socket, getaddrinfo, gethostbyname) without sandbox checks|N/A|No C++ or network operation is changed.
|19. If filesystem/network ops exist, verify `QoreSandboxManagerHelper` usage|N/A|No filesystem/network operation requires a new sandbox helper.
|20. No `File::`, `Dir::`, `Socket::`, `HTTPClient::` usage without justification|Pass|No new File/Dir/Socket/HTTPClient operation; only value and provider APIs.
|21. All `for`/`while` loops that could iterate >100 times have `qore_check_cancel()` checks|N/A|No C++ loop is changed.
|22. Uses `qore_check_cancel()` (NOT deprecated `qore_check_io_interrupt()`)|N/A|No native cancellation API is changed.
|23. Check frequency: every 100 iterations for tight loops, every 10 for expensive iterations|N/A|No new native loop requires a check interval.
|24. No blocking operations without cancellation support|Pass|No blocking operation is added; the existing item map retains runtime cancellation and exception behavior.
|25. Every action has `display_name`, `short_desc` (plain text, <80 chars), `desc` (markdown)|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|26. Every action has `options` populated via `getActionOptionFromFields()` — without this, the action shows an empty, unusable form|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|27. Every action has `output_type` set to a typed data type constant (e.g., `MyResponseDataType`) — not omitted|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|28. DPAT_API actions: provider has `"supports_request": True` and implements `doRequestImpl()`|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|29. DPAT_FIND actions: every option exists in `SearchOptions`, `getRecordTypeImpl()` returns `*hash<string, AbstractDataField>`|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|30. Scheme-based apps (with `"scheme"` in registerApp): actions use `"path"` and do NOT use `"cls"` — having both `scheme` and `cls` causes a runtime error|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|31. Single-key hash slices use trailing comma: `Fields{"key",}` (without trailing comma, `Fields{"key"}` returns the value, not a hash)|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|32. **Typed data type classes exist** for request and response types — inherit `HashDataType`, have `const Fields` hash, call `addQoreFields(Fields)` in constructor, export public constant at bottom (e.g., `public const MyDataType = new MyDataType();`)|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|33. Request/input types use `public` Fields (enables `ClassName::Fields` in action registration)|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|34. Response/output types use `private` Fields|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|35. Each field in data types has `display_name`, `type`, and `desc` (markdown-formatted)|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|36. Input fields have `example_value` where useful (string fields, endpoint URIs, SQL queries, etc.)|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|37. Fields with finite allowed values use `allowed_values` with `AllowedValueInfo` containing both `value` and `display_name` (Title Case, human-readable) — never bare values, never described only in text|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|38. Password/secret fields have `"sensitive": True`|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|39. `groups` uses `AppGroup` enum values from `qlib/DataProvider/AppGroup.qc`|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|40. App `logo` stored as separate file, loaded at module level in `Priv` namespace|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|41. App `desc` uses markdown: bullet list of capabilities, links to project website, business-language explanation of value|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|42. `display_name` is user-friendly ("Apache Avro" not "avro")|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|43. `short_desc` is plain text, under 80 chars, single sentence — no markdown|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|44. `desc` uses markdown: backticks for code/field refs (`` `field_name` ``, `` `True` ``, `` `pdf` ``), `\n\n` for paragraphs, `- ` bullet lists for enumerations, `**bold**` for caveats|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|45. Descriptions use plain business language relating to common challenges — not just technical "what" but "why" and "when to use"|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|46. No bare `True`/`False`/`NOTHING` — must be backtick-wrapped in `desc`|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|47. No bare field/option names in prose — must use backticks|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|48. Long descriptions (>500 chars) use bold section headers and bullet lists|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|49. **Factory registration in Qore repo**: every factory name registered in `qlib/DataProvider/DataProvider.qc` → `FactoryMap` (without this, module loads but doesn't appear in Qorus apps)|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|50. **`getRecordTypeImpl()` signature**: must be `private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options)` — NOT returning `*AbstractDataProviderType`|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|51. **Dependency JARs committed** (for JNI modules): JAR files in `qlib/*/jar/` may be gitignored — use `git add -f` to ensure they're tracked, otherwise CI compilation fails|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|52. JAR install rules in CMakeLists.txt for all dependency JARs|N/A|Collection value conversion only; no action/app registration, request/response schema class, catalog, factory, secret, or JNI dependency is introduced.
|53. **No workarounds**: No TODOs, FIXMEs, stubs, or partially-implemented features|Pass|The root missing-collection dispatch is corrected directly. No feature flags, fixture-specific branch, suppressed error, skipped case or stub is introduced.
|54. **Exception safety**: C++ uses `ReferenceHolder` for Qore allocations, `std::unique_ptr` for C++ allocations, `*xsink` checked after every fallible operation|Pass|Default conversion only changes the call-local input. Invalid item/default errors propagate and leave the provider reusable; result mutation does not alter the stored default.
|55. **Thread safety**: All mutable shared state protected by `std::lock_guard<std::mutex>` or documented as immutable-after-construction|Pass|No shared state is added or modified during conversion. Collection defaults are read with copy-on-write value semantics; test counters are local to a test case.
|56. **Type safety**: Strongly-typed `code<return(args)>` instead of untyped `code`; `static_cast` instead of C casts; typed hashdecls for results; enums where appropriate|Pass|Existing Type/AbstractDataProviderType constructors and strongly typed provider classes are retained. An explicit list still converts each item through its declared type.
|57. **Performance**: No O(n²) where O(n) is possible; no unnecessary copies; coordinate descent uses incremental residuals not full matrix multiply|Pass|Missing optional input returns in constant time. Default lists take the same single item-validation pass as explicit lists, without copying or scanning them twice.
|58. **Error handling**: All inputs validated (dimensions, empty data, unfitted models); C++ I/O handles EAGAIN/EINTR if applicable|Pass|Tests distinguish absent, empty, explicit NULL, explicit NOTHING items, mandatory copies, invalid defaults and invalid items; HashDataType wrapper categories are asserted.
|59. **Documentation**: Doxygen `@param`, `@return`, `@throw` on all public methods; `@par Example` with realistic business scenarios; `@note` for important caveats|Pass|Public method parameters/return/error docs include missing/default/item semantics, caveats and an executed invoice-reference example. Durable design and release notes are updated; qdx extraction is warning-free.
|60. **QPP flags**: `[flags=CONSTANT]` on methods that never throw; `[flags=RET_VALUE_ONLY]` on methods that throw but have no side effects|N/A|No QPP method or flag is changed.
|61. **Security**: No user-controlled format strings; no buffer overflows; bounds checking on array indices; no credentials in code|Pass|No new I/O, credentials, untrusted format string or unchecked index. Error formatting uses the existing fixed format string.
|62. **Correctness**: Algorithms verified against reference implementations; edge cases tested (empty data, single sample, all-zero features)|Pass|Nine checks pass: 75 cases/2875 assertions, including the new 5-case/129-assertion suite in AST/IR/JIT/tiered. Explicit compiled-module loading also passes 5/129. The XML gate passes 109 suites/1098 cases/54884 assertions, and all 2443 both-version corpus stage verdicts remain unchanged.
