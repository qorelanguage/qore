# Audit: allowed list-element values

Copyright (C) 2026 Qore Technologies, s.r.o.

This commit contains AbstractDataField.qc, its DataProvider release-note entry,
ElementAllowedValues.qtest, and this audit. The full audit-changes skill was read
and all 62 checks applied to the final diff. The referenced module structure,
sandboxing, cooperative cancellation and DataProvider checklist/development guides
were consulted. Concurrent Together, OpenApi3, RestSchemaActions and build-registration
changes remain with their current owner and are excluded.

The XML string-enumeration regression exposed rejection of a repeated field whose
value was visibly in its allowed list. The setter selected a membership strategy
from the raw caller input instead of normalized AllowedValueInfo values; its
structured fallback compared each item to the metadata records. The setter now
converts and validates all choices locally, chooses the strategy from the converted
values, and publishes the complete result. Replacing or clearing choices resets
stale membership and custom-value flags. Failed conversion retains the old state.

Verification uses the immutable Debug Qore runtime in /tmp/wsdl-core-deps/runtime,
with LD_LIBRARY_PATH pointing there and QORE_MODULE_DIR=qlib in the core checkout.
The required DataProvider-qmod target was rebuilt in build/ (Release, /usr prefix)
and tests loaded its new AOT module. No installation, native/astparser change,
Valgrind run or push is part of this Qore-only change.

Eight DataProvider suites pass 87 cases and 2432 assertions without warnings or
errors. The focused regression also passes AST execution mode (5 cases/40 assertions).
Full DataProvider Qdx documentation generation uses --strict-tables and the already
built astparser module; no astparser target is built. Downstream XML's in-progress
P3-07 tree passes all 41 affected Qore suites (519 cases), including repeated
string-enumeration fields. Both-version corpus counts and failure identities match
the previous committed report: 279 successful/14 failed parses, 992/124 decodes,
990/2 serializations, and 942/48 independently validated/rejected outputs. These
known XML phase failures remain visible; this is not completion of P3 or P9.

Logs: /tmp/qore-element-choices-before.log, /tmp/qore-element-choices-build-final.log,
/tmp/qore-element-choices-tests.log and its named suite logs,
/tmp/qore-element-choices-ast.log, /tmp/qore-element-choices-docs.log,
/tmp/wsdl-p3-07-core-dependency-affected.log and
/tmp/wsdl-p3-07-core-dependency-survey.json.

Final source SHA-256:

- qlib/DataProvider/AbstractDataField.qc: `9f7c21fe0150db54ee11aa74d4f7f1501510a9cc5faeba36bc17081b6843bbcc`
- qlib/DataProvider/DataProvider.qm: `fbc324ef8fd8d7aa91d3973d46354780b7b6360aade33c631092ecd7ce6b612e`
- examples/test/qlib/DataProvider/ElementAllowedValues.qtest: `ef824962df732c66884068f099b624a25a8c7b4eb01df74f9d600ae8cf0207ae`

|!Check|!Status|!Evidence
|1. Entry exists in `doxygen/lang/120_modules.dox.tmpl` (for modules in the Qore repo; N/A for external module repos)|N/A|Existing module; no module registration or dependency added. Its release notes are updated.
|2. Entry exists in `doxygen/lang/900_release_notes.dox.tmpl` (for modules in the Qore repo; external modules have release notes in their .qm)|N/A|Existing module; no module registration or dependency added. Its release notes are updated.
|3. `qore_user_module()` or `qore_external_user_module()` call in `CMakeLists.txt`|N/A|Existing module; no module registration or dependency added. Its release notes are updated.
|4. Module added to QMOD list in `CMakeLists.txt`|N/A|Existing module; no module registration or dependency added. Its release notes are updated.
|5. `.qm` file has `@section <lowercasemodname>intro` as first doc section — **must be all lowercase** (e.g., `avrodataproviderintro`, not `AvroDataProviderintro`)|Pass|Existing dataproviderintro section remains lowercase.
|6. `%modern` in `.qm` file — no redundant `%new-style`, `%require-types`, `%strict-args`, `%enable-all-warnings`|Pass|DataProvider.qm retains %modern without new parse directives.
|7. No parse directives (`%requires`, `%modern`, `%new-style`) in separated `.qc` files (check OUTSIDE of `@code` blocks only)|Pass|AbstractDataField.qc has no parse directives outside documentation.
|8. No `%include` usage (deprecated for modules)|Pass|No include or module layout changes.
|9. Copyright 2026 on all new files|Pass|Changed source, new executable test and this audit carry 2026 notices.
|10. Directory layout: `.qm` inside `qlib/<ModuleName>/` directory (not at `qlib/<ModuleName>.qm` for multi-file modules)|Pass|Existing directory module remains qlib/DataProvider/DataProvider.qm.
|11. No second `.qm` for the same module at `qlib/<ModuleName>.qm`|Pass|No duplicate entry point.
|12. `ns=Qore::XX` matches the QoreNamespace constructor path|N/A|No C++/QPP change or new blocking operation. Qore owns allocations and loop cancellation.
|13. `%modern` directive present|Pass|ElementAllowedValues.qtest uses %modern.
|14. Executable permission set (`chmod +x`)|Pass|Executable mode 0755 verified.
|15. Uses %prepend-module-path  before %requires for in-repo modules (Qore and Qore modules only; not Qorus)|Pass|Local module path precedes relative QUnit.qm and DataProvider requires.
|16. External module dependencies use `%try-module` — except modules delivered with the project itself (Qore ex: DataProvider, ConnectionProvider, QUnit, etc.) which use hard `%requires`|Pass|Only repository-delivered QUnit and DataProvider dependencies.
|17. No filesystem operations (fopen, open, creat, unlink, remove, rename, mkdir, rmdir, stat, chmod) without sandbox checks|N/A|No C++/QPP change or new blocking operation. Qore owns allocations and loop cancellation.
|18. No network operations (connect, bind, socket, getaddrinfo, gethostbyname) without sandbox checks|N/A|No C++/QPP change or new blocking operation. Qore owns allocations and loop cancellation.
|19. If filesystem/network ops exist, verify `QoreSandboxManagerHelper` usage|N/A|No C++/QPP change or new blocking operation. Qore owns allocations and loop cancellation.
|20. No `File::`, `Dir::`, `Socket::`, `HTTPClient::` usage without justification|Pass|No file, directory, socket or HTTP access added.
|21. All `for`/`while` loops that could iterate >100 times have `qore_check_cancel()` checks|N/A|No C++/QPP change or new blocking operation. Qore owns allocations and loop cancellation.
|22. Uses `qore_check_cancel()` (NOT deprecated `qore_check_io_interrupt()`)|N/A|No C++/QPP change or new blocking operation. Qore owns allocations and loop cancellation.
|23. Check frequency: every 100 iterations for tight loops, every 10 for expensive iterations|N/A|No C++/QPP change or new blocking operation. Qore owns allocations and loop cancellation.
|24. No blocking operations without cancellation support|N/A|No C++/QPP change or new blocking operation. Qore owns allocations and loop cancellation.
|25. Every action has `display_name`, `short_desc` (plain text, <80 chars), `desc` (markdown)|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|26. Every action has `options` populated via `getActionOptionFromFields()` — without this, the action shows an empty, unusable form|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|27. Every action has `output_type` set to a typed data type constant (e.g., `MyResponseDataType`) — not omitted|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|28. DPAT_API actions: provider has `"supports_request": True` and implements `doRequestImpl()`|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|29. DPAT_FIND actions: every option exists in `SearchOptions`, `getRecordTypeImpl()` returns `*hash<string, AbstractDataField>`|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|30. Scheme-based apps (with `"scheme"` in registerApp): actions use `"path"` and do NOT use `"cls"` — having both `scheme` and `cls` causes a runtime error|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|31. Single-key hash slices use trailing comma: `Fields{"key",}` (without trailing comma, `Fields{"key"}` returns the value, not a hash)|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|32. **Typed data type classes exist** for request and response types — inherit `HashDataType`, have `const Fields` hash, call `addQoreFields(Fields)` in constructor, export public constant at bottom (e.g., `public const MyDataType = new MyDataType();`)|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|33. Request/input types use `public` Fields (enables `ClassName::Fields` in action registration)|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|34. Response/output types use `private` Fields|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|35. Each field in data types has `display_name`, `type`, and `desc` (markdown-formatted)|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|36. Input fields have `example_value` where useful (string fields, endpoint URIs, SQL queries, etc.)|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|37. Fields with finite allowed values use `allowed_values` with `AllowedValueInfo` containing both `value` and `display_name` (Title Case, human-readable) — never bare values, never described only in text|Pass|Resolved choices retain typed AllowedValueInfo records, including display_name; bare input values are normalized by the existing public API.
|38. Password/secret fields have `"sensitive": True`|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|39. `groups` uses `AppGroup` enum values from `qlib/DataProvider/AppGroup.qc`|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|40. App `logo` stored as separate file, loaded at module level in `Priv` namespace|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|41. App `desc` uses markdown: bullet list of capabilities, links to project website, business-language explanation of value|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|42. `display_name` is user-friendly ("Apache Avro" not "avro")|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|43. `short_desc` is plain text, under 80 chars, single sentence — no markdown|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|44. `desc` uses markdown: backticks for code/field refs (`` `field_name` ``, `` `True` ``, `` `pdf` ``), `\n\n` for paragraphs, `- ` bullet lists for enumerations, `**bold**` for caveats|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|45. Descriptions use plain business language relating to common challenges — not just technical "what" but "why" and "when to use"|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|46. No bare `True`/`False`/`NOTHING` — must be backtick-wrapped in `desc`|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|47. No bare field/option names in prose — must use backticks|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|48. Long descriptions (>500 chars) use bold section headers and bullet lists|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|49. **Factory registration in Qore repo**: every factory name registered in `qlib/DataProvider/DataProvider.qc` → `FactoryMap` (without this, module loads but doesn't appear in Qorus apps)|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|50. **`getRecordTypeImpl()` signature**: must be `private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options)` — NOT returning `*AbstractDataProviderType`|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|51. **Dependency JARs committed** (for JNI modules): JAR files in `qlib/*/jar/` may be gitignored — use `git add -f` to ensure they're tracked, otherwise CI compilation fails|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|52. JAR install rules in CMakeLists.txt for all dependency JARs|N/A|Generic field-choice implementation only; no app, action, production field declaration, presentation catalog, factory or JAR registration changed.
|53. **No workarounds**: No TODOs, FIXMEs, stubs, or partially-implemented features|Pass|Root fixes: membership used raw metadata instead of converted values; structured comparisons targeted metadata records; replacement retained stale maps or published partial data. No workaround, suppression or stub.
|54. **Exception safety**: C++ uses `ReferenceHolder` for Qore allocations, `std::unique_ptr` for C++ allocations, `*xsink` checked after every fallible operation|Pass|All choice normalization, provider conversion and membership construction complete in local typed values before assigning fields. A conversion failure preserves previous choices and flags; rollback regression passes. No C++ allocations.
|55. **Thread safety**: All mutable shared state protected by `std::lock_guard<std::mutex>` or documented as immutable-after-construction|Pass|No new static, global or concurrently shared state. Fields retain their existing instance mutation contract; new conversion state is local to each call and tests reconstruct independent field instances.
|56. **Type safety**: Strongly-typed `code<return(args)>` instead of untyped `code`; `static_cast` instead of C casts; typed hashdecls for results; enums where appropriate|Pass|Local choices use list<hash<AllowedValueInfo>>, membership is hash<string, bool>, and the element provider is typed. Actual record types disambiguate raw records containing a value field from metadata.
|57. **Performance**: No O(n²) where O(n) is possible; no unnecessary copies; coordinate descent uses incremental residuals not full matrix multiply|Pass|Scalar membership remains linear construction plus constant-time lookups. Structured fallback extracts values once per acceptsValue call, then uses existing structural comparisons. No per-item metadata-list copies.
|58. **Error handling**: All inputs validated (dimensions, empty data, unfitted models); C++ I/O handles EAGAIN/EINTR if applicable|Pass|Tests cover raw/described strings, converted integers, structured and heterogeneous values, a record named value, invalid items, repeated/empty input, replacement, custom choices, clearing, rollback and serialization reconstruction.
|59. **Documentation**: Doxygen `@param`, `@return`, `@throw` on all public methods; `@par Example` with realistic business scenarios; `@note` for important caveats|Pass|Method documents parameters, return, failure behavior, exception safety and a currency-list example. DataProvider v3.8 release notes updated. Full module Qdx strict-table generation passes without warnings/errors.
|60. **QPP flags**: `[flags=CONSTANT]` on methods that never throw; `[flags=RET_VALUE_ONLY]` on methods that throw but have no side effects|N/A|No C++/QPP change or new blocking operation. Qore owns allocations and loop cancellation.
|61. **Security**: No user-controlled format strings; no buffer overflows; bounds checking on array indices; no credentials in code|Pass|No credentials, user-controlled format strings or unchecked native indexing. Qore empty-list indexing has defined NOTHING behavior; no new I/O.
|62. **Correctness**: Algorithms verified against reference implementations; edge cases tested (empty data, single sample, all-zero features)|Pass|Before fix all five new cases failed. Final eight suites pass 87 cases/2432 assertions; new suite also passes AST mode (5/40). Downstream XML passes 41 suites/519 cases. Both-version corpus counts and failure identities are unchanged.
