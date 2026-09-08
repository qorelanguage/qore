# Issue 5432 follow-up: audit-changes

Copyright 2026 Qore Technologies, s.r.o.

Applied `/home/david/.codex/skills/audit-changes/SKILL.md` before the follow-up commit on develop.
Scope: JinaDataProvider source and catalogs, its qtest, README and verification records.
Concurrent C++/regex and unrelated release-note edits in the shared workspace are outside this commit.
No native code is changed by this work, so native sandbox/cancellation/QPP checks and valgrind are N/A.

The table audits local implementation changes. External live acceptance failures are recorded separately
as Fail in [the full data-provider checklist](5432-verification.md); they are not concealed as successes.

|!Item|!Check|!Status|!Evidence
|1|Entry exists in `doxygen/lang/120_modules.dox.tmpl` (for modules in the Qore repo; N/A for external module repos)|N/A|No new module, QPP class, native code, record-search provider, JNI dependency, or packaging registration in this follow-up, as applicable to this check.
|2|Entry exists in `doxygen/lang/900_release_notes.dox.tmpl` (for modules in the Qore repo; external modules have release notes in their .qm)|Pass|Existing central Jina 1.1 entry remains; module release notes now describe form fixes, labels, and direct JSON dependency.
|3|`qore_user_module()` or `qore_external_user_module()` call in `CMakeLists.txt`|N/A|No new module, QPP class, native code, record-search provider, JNI dependency, or packaging registration in this follow-up, as applicable to this check.
|4|Module added to QMOD list in `CMakeLists.txt`|N/A|No new module, QPP class, native code, record-search provider, JNI dependency, or packaging registration in this follow-up, as applicable to this check.
|5|`.qm` file has `@section <lowercasemodname>intro` as first doc section — **must be all lowercase** (e.g., `avrodataproviderintro`, not `AvroDataProviderintro`)|N/A|No new module, QPP class, native code, record-search provider, JNI dependency, or packaging registration in this follow-up, as applicable to this check.
|6|`%modern` in `.qm` file — no redundant `%new-style`, `%require-types`, `%strict-args`, `%enable-all-warnings`|Pass|JinaDataProvider.qm retains %modern; no redundant modes added.
|7|No parse directives (`%requires`, `%modern`, `%new-style`) in separated `.qc` files (check OUTSIDE of `@code` blocks only)|Pass|No parse directives occur in separated .qc source.
|8|No `%include` usage (deprecated for modules)|Pass|No %include directives.
|9|Copyright 2026 on all new files|Pass|All changed Qore files and new test documentation carry copyright 2026; catalogs remain generated JSON data.
|10|Directory layout: `.qm` inside `qlib/<ModuleName>/` directory (not at `qlib/<ModuleName>.qm` for multi-file modules)|N/A|No new module, QPP class, native code, record-search provider, JNI dependency, or packaging registration in this follow-up, as applicable to this check.
|11|No second `.qm` for the same module at `qlib/<ModuleName>.qm`|N/A|No new module, QPP class, native code, record-search provider, JNI dependency, or packaging registration in this follow-up, as applicable to this check.
|12|`ns=Qore::XX` matches the QoreNamespace constructor path|N/A|No new module, QPP class, native code, record-search provider, JNI dependency, or packaging registration in this follow-up, as applicable to this check.
|13|`%modern` directive present|Pass|JinaDataProvider.qtest uses %modern.
|14|Executable permission set (`chmod +x`)|Pass|JinaDataProvider.qtest retains executable mode.
|15|Uses %prepend-module-path  before %requires for in-repo modules (Qore and Qore modules only; not Qorus)|Pass|Test prepends local qlib and uses relative requires for Util, QUnit, and JinaDataProvider.
|16|External module dependencies use `%try-module` — except modules delivered with the project itself (Qore ex: DataProvider, ConnectionProvider, QUnit, etc.) which use hard `%requires`|Pass|json is delivered by this repository; hard requires is correct. The provider now directly declares the json dependency used by parse_json rather than relying on an indirect import.
|17|No filesystem operations (fopen, open, creat, unlink, remove, rename, mkdir, rmdir, stat, chmod) without sandbox checks|N/A|No new module, QPP class, native code, record-search provider, JNI dependency, or packaging registration in this follow-up, as applicable to this check.
|18|No network operations (connect, bind, socket, getaddrinfo, gethostbyname) without sandbox checks|N/A|No new module, QPP class, native code, record-search provider, JNI dependency, or packaging registration in this follow-up, as applicable to this check.
|19|If filesystem/network ops exist, verify `QoreSandboxManagerHelper` usage|N/A|No new module, QPP class, native code, record-search provider, JNI dependency, or packaging registration in this follow-up, as applicable to this check.
|20|No `File::`, `Dir::`, `Socket::`, `HTTPClient::` usage without justification|Pass|No new direct filesystem/socket/HTTP implementation. Existing module initialization reads packaged logo/schema; runtime requests use the REST transport. Live fixture cleanup uses existing action APIs.
|21|All `for`/`while` loops that could iterate >100 times have `qore_check_cancel()` checks|N/A|No new module, QPP class, native code, record-search provider, JNI dependency, or packaging registration in this follow-up, as applicable to this check.
|22|Uses `qore_check_cancel()` (NOT deprecated `qore_check_io_interrupt()`)|N/A|No new module, QPP class, native code, record-search provider, JNI dependency, or packaging registration in this follow-up, as applicable to this check.
|23|Check frequency: every 100 iterations for tight loops, every 10 for expensive iterations|N/A|No new module, QPP class, native code, record-search provider, JNI dependency, or packaging registration in this follow-up, as applicable to this check.
|24|No blocking operations without cancellation support|N/A|No new module, QPP class, native code, record-search provider, JNI dependency, or packaging registration in this follow-up, as applicable to this check.
|25|Every action has `display_name`, `short_desc` (plain text, <80 chars), `desc` (markdown)|Pass|Registration test checks every one of the 19 actions for display name, short description under 80 characters, description, type, and path.
|26|Every action has `options` populated via `getActionOptionFromFields()` — without this, the action shows an empty, unusable form|Pass|Manual options derive from public Fields; schema actions derive through RestSchemaActions. Parameterless model/classifier lists have intentionally empty option hashes.
|27|Every action has `output_type` set to a typed data type constant (e.g., `MyResponseDataType`) — not omitted|Pass|All actions expose typed outputs; no type removed by the follow-up.
|28|DPAT_API actions: provider has `"supports_request": True` and implements `doRequestImpl()`|Pass|All action children support requests and implement doRequestImpl; offline wire tests and working live endpoints verify dispatch.
|29|DPAT_FIND actions: every option exists in `SearchOptions`, `getRecordTypeImpl()` returns `*hash<string, AbstractDataField>`|N/A|No new module, QPP class, native code, record-search provider, JNI dependency, or packaging registration in this follow-up, as applicable to this check.
|30|Scheme-based apps (with `"scheme"` in registerApp): actions use `"path"` and do NOT use `"cls"` — having both `scheme` and `cls` causes a runtime error|Pass|JinaAi retains scheme jina and path-only action registration; registrationTest rejects cls on all 19.
|31|Single-key hash slices use trailing comma: `Fields{"key",}` (without trailing comma, `Fields{"key"}` returns the value, not a hash)|Pass|No incorrect single-key hash slices introduced.
|32|**Typed data type classes exist** for request and response types — inherit `HashDataType`, have `const Fields` hash, call `addQoreFields(Fields)` in constructor, export public constant at bottom (e.g., `public const MyDataType = new MyDataType();`)|Pass|Manual HashDataType contracts retain Fields and addQoreFields; schema-backed types follow the manifest-driven architecture.
|33|Request/input types use `public` Fields (enables `ClassName::Fields` in action registration)|Pass|Reader/Search request Fields remain public; reordered source inputs are consumed by the action catalog.
|34|Response/output types use `private` Fields|Pass|Response Fields remain private; no output structure changes.
|35|Each field in data types has `display_name`, `type`, and `desc` (markdown-formatted)|Pass|Every action option has display_name, short_desc, desc and type. Required-first ordering, API literals and readable model names are checked at runtime.
|36|Input fields have `example_value` where useful (string fields, endpoint URIs, SQL queries, etc.)|Pass|Existing structured/text examples retained; added numeric dimensions=32, top_n=3 and limit=10.
|37|Fields with finite allowed values use `allowed_values` with `AllowedValueInfo` containing both `value` and `display_name` (Title Case, human-readable) — never bare values, never described only in text|Pass|Model subsets reuse client AllowedValueInfo labels with schema-compatible values. Image retention labels explain alternative text/generated captions. All choice values and wire contracts stay stable.
|38|Password/secret fields have `"sensitive": True`|Pass|API key, cookies and proxy options retain sensitive metadata; no new credential field.
|39|`groups` uses `AppGroup` enum values from `qlib/DataProvider/AppGroup.qc`|Pass|AppGroup::AiLlm remains the registered group.
|40|App `logo` stored as separate file, loaded at module level in `Priv` namespace|Pass|Separate square SVG is loaded once in Priv::JinaLogo, consistent with this skill.
|41|App `desc` uses markdown: bullet list of capabilities, links to project website, business-language explanation of value|Pass|App description retains Markdown capability bullets and Jina website link.
|42|`display_name` is user-friendly ("Apache Avro" not "avro")|Pass|App display name remains Jina AI; model and image labels now read naturally.
|43|`short_desc` is plain text, under 80 chars, single sentence — no markdown|Pass|Generated short descriptions remain plain; tests and catalog checks verify source metadata.
|44|`desc` uses markdown: backticks for code/field refs (`` `field_name` ``, `` `True` ``, `` `pdf` ``), `\n\n` for paragraphs, `- ` bullet lists for enumerations, `**bold**` for caveats|Pass|Bare true and batch field references now use code spans; app capabilities use paragraphs and bullets, service caveats use bold.
|45|Descriptions use plain business language relating to common challenges — not just technical "what" but "why" and "when to use"|Pass|Examples and help describe customer support, delivery search, content processing, and classifier privacy.
|46|No bare `True`/`False`/`NOTHING` — must be backtick-wrapped in `desc`|Pass|All 19 action option descriptions checked for bare booleans/null; six offending descriptions fixed and translated.
|47|No bare field/option names in prose — must use backticks|Pass|Batch input/input_url references now use backticks; remaining Jina-owned catalog descriptions reviewed for bare field names.
|48|Long descriptions (>500 chars) use bold section headers and bullet lists|Pass|No Jina-owned action/option description exceeds 500 characters; app description already has structured capability bullets.
|49|**Factory registration in Qore repo**: every factory name registered in `qlib/DataProvider/DataProvider.qc` → `FactoryMap` (without this, module loads but doesn't appear in Qorus apps)|Pass|FactoryMap maps jina to JinaDataProvider; installed app/index verification checks actual visibility.
|50|**`getRecordTypeImpl()` signature**: must be `private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options)` — NOT returning `*AbstractDataProviderType`|N/A|No new module, QPP class, native code, record-search provider, JNI dependency, or packaging registration in this follow-up, as applicable to this check.
|51|**Dependency JARs committed** (for JNI modules): JAR files in `qlib/*/jar/` may be gitignored — use `git add -f` to ensure they're tracked, otherwise CI compilation fails|N/A|No new module, QPP class, native code, record-search provider, JNI dependency, or packaging registration in this follow-up, as applicable to this check.
|52|JAR install rules in CMakeLists.txt for all dependency JARs|N/A|No new module, QPP class, native code, record-search provider, JNI dependency, or packaging registration in this follow-up, as applicable to this check.
|53|**No workarounds**: No TODOs, FIXMEs, stubs, or partially-implemented features|Pass|No TODO/FIXME/stub or workaround. Source import fixed at the dependency declaration; form order fixed in Fields/manifest. Vendor failures remain explicit; no fallback endpoint or polling loop.
|54|**Exception safety**: C++ uses `ReferenceHolder` for Qore allocations, `std::unique_ptr` for C++ allocations, `*xsink` checked after every fallible operation|Pass|Qore values are managed. New lifecycle test registers cleanup before assertions and verifies deletion explicitly; per-call request handling retains existing exception-safe transport copying.
|55|**Thread safety**: All mutable shared state protected by `std::lock_guard<std::mutex>` or documented as immutable-after-construction|Pass|New model-choice constants are immutable. Existing schema/child caches retain locks; no mutable shared state added.
|56|**Type safety**: Strongly-typed `code<return(args)>` instead of untyped `code`; `static_cast` instead of C casts; typed hashdecls for results; enums where appropriate|Pass|Typed RestSchemaFieldOverlayInfo, AllowedValueInfo, list/hash results, and bool/int lifecycle state. No untyped callbacks or native casts added.
|57|**Performance**: No O(n²) where O(n) is possible; no unnecessary copies; coordinate descent uses incremental residuals not full matrix multiply|Pass|Model subsets are computed once at module initialization; existing cached schema/action construction retained. No per-request lookup or redundant model API request added.
|58|**Error handling**: All inputs validated (dimensions, empty data, unfitted models); C++ I/O handles EAGAIN/EINTR if applicable|Pass|Negative tests cover validation before transport, HTTP errors without retries, malformed JSONL and auth failures. Direct vendor probes distinguish authentication, model selection, and processing failures.
|59|**Documentation**: Doxygen `@param`, `@return`, `@throw` on all public methods; `@par Example` with realistic business scenarios; `@note` for important caveats|Pass|Module docs explain full classifier lifecycle, proven worker error, unresolved classifier internals and EU DNS failure. README documents reproducible tests; verification record accounts for all 210 data-provider checklist items.
|60|**QPP flags**: `[flags=CONSTANT]` on methods that never throw; `[flags=RET_VALUE_ONLY]` on methods that throw but have no side effects|N/A|No new module, QPP class, native code, record-search provider, JNI dependency, or packaging registration in this follow-up, as applicable to this check.
|61|**Security**: No user-controlled format strings; no buffer overflows; bounds checking on array indices; no credentials in code|Pass|No credentials or signed download URLs in the commit. Diagnostic request IDs are nonsecret; key comparison only prints a boolean. Live probes use the existing authorized key.
|62|**Correctness**: Algorithms verified against reference implementations; edge cases tested (empty data, single sample, all-zero features)|Pass|Local action, translation, source-loading, docs, and installed tests are recorded in 5432-verification.md. Successful saved-classifier lifecycle and completed batch output cannot be verified while vendor endpoints fail; no claim that these live acceptance criteria pass.

## Summary

|!Check|!Status|!Findings
|Module docs|N/A|Existing module index entries unchanged.
|Release notes|Pass|Module notes updated.
|CMake registration|N/A|Existing module and resource registration unchanged.
|Module structure|Pass|Modern entry point, separated source, explicit json dependency.
|Sandboxing|Pass|Existing sandbox-aware transports; no new direct I/O.
|Cooperative cancellation|N/A|No native code.
|DP action registration|Pass|All 19 action paths, options and outputs checked.
|DP data types and examples|Pass|Typed Fields retained; numeric examples improved.
|DP allowed values|Pass|Readable model/image labels; values preserved.
|DP description quality|Pass|API code spans and 12 locale updates.
|DP scheme actions|Pass|Path-only registration with jina scheme.
|FactoryMap|Pass|Installed catalog checked after index rebuild.
|Dependency JARs|N/A|No JNI changes.
|Workarounds/stubs|Pass|No endpoint fallback or hidden vendor error.
|Exception safety|Pass|Managed values and lifecycle cleanup.
|Thread safety|Pass|Immutable choices and existing cache locks.
|Type safety|Pass|Typed metadata and lifecycle assertions.
|Performance|Pass|Choices computed once, schema cached.
|Error handling|Pass|Local negative tests and direct vendor diagnostics.
|Documentation|Pass|Module help, test README, complete checklist and evidence.
|QPP flags|N/A|No QPP changes.
|Security|Pass|No credentials or signed URLs committed.
|Correctness|Pass|Local validation passes; vendor lifecycle/output limitations stated explicitly.
