# Audit: HashDataType required fields followed by defaults

Copyright (C) 2026 Qore Technologies, s.r.o.

Full audit-changes skill read and applied before this commit. Applicable Qore AGENTS.md
and all five referenced design documents were consulted; table format follows
`design/doc-tables.md`. Parent reviewed: ceabb7dbcc11a2f0db7060abfe8d779dd3cc0e7d.
Only the three files below and this audit belong to this change. Unrelated RestClient,
AzureOpenAiRestClient and Sap4HanaRestClient work is preserved.

## Root cause and verification

postProcessAddedField only recorded required_fields when the hash had already become
optional. A required field added first was therefore forgotten when an optional field
with a default was added afterward. The fix records required fields regardless of the
current automatic hash type, while preserving explicitly optional containers and
fully defaulted records.

The new regression failed only the required-first case before the fix; all four cases
now pass (21 assertions). Existing DataProvider (33 cases/2203 assertions), required
groups (8/58), and type-graph (4/29) suites pass: 49 core cases in total. All run with
qore --enable-debug. Logs: /tmp/wsdl-p2-07-core-{required,dataprovider,groups,typegraph}.log.

The required DataProvider-qmod target rebuilt successfully in build/ with the existing
/usr prefix and Release configuration. The first attempt detected a source change made
during compilation and stopped with its explicit retry diagnostic; the retry completed.
Its CMake reconfiguration reports unavailable optional quictls/LibreSSL backends.
Those optional-backend configuration messages are not test warnings or failures. Final build log: /tmp/wsdl-p2-07-core-qmod-final.log.

WSDL integration runs use QORE_MODULE_DIR=/home/david/src/qore/git/qore/qlib, so they
load this core change without installation. All 322 affected WSDL Qore cases pass.
The 69-test Python suite retains only the two explicitly routed P6 selected-binding
subtest failures; it is not a green full Python suite. Twelve generated payloads,
including attributed simple content, use provider values and validate independently.
Both-version survey and strict coverage are unchanged except the WSDL module digest;
38 selected descriptions/140 directions pass, while 384 diagnostic failures remain.
No WSDL phase-boundary completion or complete protocol conformance is claimed.

Production/test hashes:

- qlib/DataProvider/HashDataType.qc: `d6908569310d036efe37993acbbf1c44ca4c52b65b12f5d106c0e609d6bc35e1`
- qlib/DataProvider/DataProvider.qm: `a2a874ae0c1aa717f3b2ad50020957e34b2d1de3f5226858fcab0e0d9bb1a590`
- examples/test/qlib/DataProvider/HashDataTypeRequiredFields.qtest: `8963c019809cc7c9f059576c8d9e30a351ba6363132a2fb30cb81247c3739c8e`

## Complete checklist

|!Check|!Status|!Evidence
|1. Entry exists in `doxygen/lang/120_modules.dox.tmpl` (for modules in the Qore repo; N/A for external module repos)|N/A|Existing DataProvider module; no new module, QPP class or registration. Module release notes updated for this fix.
|2. Entry exists in `doxygen/lang/900_release_notes.dox.tmpl` (for modules in the Qore repo; external modules have release notes in their .qm)|N/A|Existing DataProvider module; no new module, QPP class or registration. Module release notes updated for this fix.
|3. `qore_user_module()` or `qore_external_user_module()` call in `CMakeLists.txt`|N/A|Existing DataProvider module; no new module, QPP class or registration. Module release notes updated for this fix.
|4. Module added to QMOD list in `CMakeLists.txt`|N/A|Existing DataProvider module; no new module, QPP class or registration. Module release notes updated for this fix.
|5. `.qm` file has `@section <lowercasemodname>intro` as first doc section — **must be all lowercase** (e.g., `avrodataproviderintro`, not `AvroDataProviderintro`)|Pass|DataProvider retains its lowercase dataproviderintro section.
|6. `%modern` in `.qm` file — no redundant `%new-style`, `%require-types`, `%strict-args`, `%enable-all-warnings`|Pass|DataProvider.qm retains %modern; no redundant directives introduced.
|7. No parse directives (`%requires`, `%modern`, `%new-style`) in separated `.qc` files (check OUTSIDE of `@code` blocks only)|Pass|HashDataType.qc has no parse directives outside documentation code examples.
|8. No `%include` usage (deprecated for modules)|Pass|No %include introduced.
|9. Copyright 2026 on all new files|Pass|HashDataType copyright updated to 2026; main module and new test/audit also carry 2026 notices.
|10. Directory layout: `.qm` inside `qlib/<ModuleName>/` directory (not at `qlib/<ModuleName>.qm` for multi-file modules)|Pass|Existing main module stays in qlib/DataProvider/DataProvider.qm.
|11. No second `.qm` for the same module at `qlib/<ModuleName>.qm`|Pass|No second main module added.
|12. `ns=Qore::XX` matches the QoreNamespace constructor path|N/A|Existing DataProvider module; no new module, QPP class or registration. Module release notes updated for this fix.
|13. `%modern` directive present|Pass|New regression uses %modern.
|14. Executable permission set (`chmod +x`)|Pass|New regression executable permission verified.
|15. Uses %prepend-module-path  before %requires for in-repo modules (Qore and Qore modules only; not Qorus)|Pass|Local qlib path is prepended before relative QUnit and DataProvider requires.
|16. External module dependencies use `%try-module` — except modules delivered with the project itself (Qore ex: DataProvider, ConnectionProvider, QUnit, etc.) which use hard `%requires`|Pass|New test uses only Qore-delivered QUnit and DataProvider, with relative requires.
|17. No filesystem operations (fopen, open, creat, unlink, remove, rename, mkdir, rmdir, stat, chmod) without sandbox checks|N/A|No C++/QPP change or new blocking operation; Qore owns memory and cancellation. No valgrind required.
|18. No network operations (connect, bind, socket, getaddrinfo, gethostbyname) without sandbox checks|N/A|No C++/QPP change or new blocking operation; Qore owns memory and cancellation. No valgrind required.
|19. If filesystem/network ops exist, verify `QoreSandboxManagerHelper` usage|N/A|No C++/QPP change or new blocking operation; Qore owns memory and cancellation. No valgrind required.
|20. No `File::`, `Dir::`, `Socket::`, `HTTPClient::` usage without justification|Pass|No Qore file/directory/socket/HTTP operations added.
|21. All `for`/`while` loops that could iterate >100 times have `qore_check_cancel()` checks|N/A|No C++/QPP change or new blocking operation; Qore owns memory and cancellation. No valgrind required.
|22. Uses `qore_check_cancel()` (NOT deprecated `qore_check_io_interrupt()`)|N/A|No C++/QPP change or new blocking operation; Qore owns memory and cancellation. No valgrind required.
|23. Check frequency: every 100 iterations for tight loops, every 10 for expensive iterations|N/A|No C++/QPP change or new blocking operation; Qore owns memory and cancellation. No valgrind required.
|24. No blocking operations without cancellation support|N/A|No C++/QPP change or new blocking operation; Qore owns memory and cancellation. No valgrind required.
|25. Every action has `display_name`, `short_desc` (plain text, <80 chars), `desc` (markdown)|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|26. Every action has `options` populated via `getActionOptionFromFields()` — without this, the action shows an empty, unusable form|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|27. Every action has `output_type` set to a typed data type constant (e.g., `MyResponseDataType`) — not omitted|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|28. DPAT_API actions: provider has `"supports_request": True` and implements `doRequestImpl()`|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|29. DPAT_FIND actions: every option exists in `SearchOptions`, `getRecordTypeImpl()` returns `*hash<string, AbstractDataField>`|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|30. Scheme-based apps (with `"scheme"` in registerApp): actions use `"path"` and do NOT use `"cls"` — having both `scheme` and `cls` causes a runtime error|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|31. Single-key hash slices use trailing comma: `Fields{"key",}` (without trailing comma, `Fields{"key"}` returns the value, not a hash)|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|32. **Typed data type classes exist** for request and response types — inherit `HashDataType`, have `const Fields` hash, call `addQoreFields(Fields)` in constructor, export public constant at bottom (e.g., `public const MyDataType = new MyDataType();`)|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|33. Request/input types use `public` Fields (enables `ClassName::Fields` in action registration)|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|34. Response/output types use `private` Fields|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|35. Each field in data types has `display_name`, `type`, and `desc` (markdown-formatted)|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|36. Input fields have `example_value` where useful (string fields, endpoint URIs, SQL queries, etc.)|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|37. Fields with finite allowed values use `allowed_values` with `AllowedValueInfo` containing both `value` and `display_name` (Title Case, human-readable) — never bare values, never described only in text|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|38. Password/secret fields have `"sensitive": True`|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|39. `groups` uses `AppGroup` enum values from `qlib/DataProvider/AppGroup.qc`|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|40. App `logo` stored as separate file, loaded at module level in `Priv` namespace|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|41. App `desc` uses markdown: bullet list of capabilities, links to project website, business-language explanation of value|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|42. `display_name` is user-friendly ("Apache Avro" not "avro")|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|43. `short_desc` is plain text, under 80 chars, single sentence — no markdown|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|44. `desc` uses markdown: backticks for code/field refs (`` `field_name` ``, `` `True` ``, `` `pdf` ``), `\n\n` for paragraphs, `- ` bullet lists for enumerations, `**bold**` for caveats|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|45. Descriptions use plain business language relating to common challenges — not just technical "what" but "why" and "when to use"|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|46. No bare `True`/`False`/`NOTHING` — must be backtick-wrapped in `desc`|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|47. No bare field/option names in prose — must use backticks|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|48. Long descriptions (>500 chars) use bold section headers and bullet lists|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|49. **Factory registration in Qore repo**: every factory name registered in `qlib/DataProvider/DataProvider.qc` → `FactoryMap` (without this, module loads but doesn't appear in Qorus apps)|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|50. **`getRecordTypeImpl()` signature**: must be `private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options)` — NOT returning `*AbstractDataProviderType`|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|51. **Dependency JARs committed** (for JNI modules): JAR files in `qlib/*/jar/` may be gitignored — use `git add -f` to ensure they're tracked, otherwise CI compilation fails|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|52. JAR install rules in CMakeLists.txt for all dependency JARs|N/A|No production action, app, field declaration, factory or dependency registration changed; generic hash field bookkeeping only.
|53. **No workarounds**: No TODOs, FIXMEs, stubs, or partially-implemented features|Pass|Directly records the required field before later defaults can change the hash type. No workaround, stub, skip or compatibility suppression.
|54. **Exception safety**: C++ uses `ReferenceHolder` for Qore allocations, `std::unique_ptr` for C++ allocations, `*xsink` checked after every fallible operation|Pass|The new branch performs the existing type/boolean assignments after checking field metadata. It adds no owned resources or fallible operation after those assignments. Existing conversion failures propagate.
|55. **Thread safety**: All mutable shared state protected by `std::lock_guard<std::mutex>` or documented as immutable-after-construction|Pass|Only per-instance field-construction bookkeeping changes. Hash type construction remains complete before sharing; no globals or concurrent mutation introduced.
|56. **Type safety**: Strongly-typed `code<return(args)>` instead of untyped `code`; `static_cast` instead of C casts; typed hashdecls for results; enums where appropriate|Pass|Existing AbstractDataField, Type and bool members retained; new tests use typed fields/hash types and exact values.
|57. **Performance**: No O(n²) where O(n) is possible; no unnecessary copies; coordinate descent uses incremental residuals not full matrix multiply|Pass|Constant work per field; no repeated field scans or graph copies.
|58. **Error handling**: All inputs validated (dimensions, empty data, unfitted models); C++ I/O handles EAGAIN/EINTR if applicable|Pass|Both insertion orders, missing records/fields, unsupported keys, explicit optional containers and fully defaulted records have specific rejection and value assertions.
|59. **Documentation**: Doxygen `@param`, `@return`, `@throw` on all public methods; `@par Example` with realistic business scenarios; `@note` for important caveats|Pass|addField documents parameters, return, required/default ordering, explicit optional containers and an invoice example. DataProvider 3.7 release notes updated.
|60. **QPP flags**: `[flags=CONSTANT]` on methods that never throw; `[flags=RET_VALUE_ONLY]` on methods that throw but have no side effects|N/A|No C++/QPP change or new blocking operation; Qore owns memory and cancellation. No valgrind required.
|61. **Security**: No user-controlled format strings; no buffer overflows; bounds checking on array indices; no credentials in code|Pass|No I/O, credentials, native buffers or user-controlled formatting added.
|62. **Correctness**: Algorithms verified against reference implementations; edge cases tested (empty data, single sample, all-zero features)|Pass|49 core Qore cases pass, including four new cases/21 assertions. 322 WSDL cases and independent provider examples pass with the rebuilt local DataProvider; corpus results unchanged apart from WSDL digest. Known P6 subtests remain visible.

All applicable checks for this core change pass. No push performed.
