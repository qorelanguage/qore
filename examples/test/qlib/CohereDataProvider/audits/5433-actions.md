# Cohere actions audit for issue #5433

Copyright 2026 Qore Technologies, s.r.o.

The subsequent [production verification](5433-production.md) updates the model-list and batch
coverage and corrects the classification availability guidance recorded in this earlier audit.

Reviewed 2026-09-07 using `/home/david/.codex/skills/audit-changes/SKILL.md` after merging origin/develop through 20517a962. This report covers the Cohere action expansion and the shared fixes it exposed; the authentication commit has its own connection audit.

|!Check|!Status|!Evidence
|1. Module index|Pass|Existing Cohere module index entry now describes the expanded action surface.
|2. Release notes|Pass|Cohere, DataProvider, OpenApi3, RestSchemaActions, RestSchemaDataProvider, and Paddle release notes updated.
|3. CMake registration|Pass|Cohere registration includes the schema resource; the existing macro installs YAML assets.
|4. Qmod registration|Pass|Existing qore_user_module calls create qmod targets; all affected modules rebuilt.
|5. Introduction section|Pass|Cohere retains coheredataproviderintro as its first documentation section.
|6. Modern directives|Pass|Module entry points retain %modern without redundant directives.
|7. Split-file directives|Pass|The new CohereManifest and CohereSchema .qc files contain no parse directives.
|8. No module includes|Pass|No module %include directives introduced.
|9. Copyright|Pass|New Qore/test/import/audit files use copyright 2026; vendor schema retains upstream metadata.
|10. Directory layout|Pass|CohereDataProvider remains a single split module under qlib/CohereDataProvider.
|11. Single entry point|Pass|No duplicate CohereDataProvider.qm entry point.
|12. QPP namespace|N/A|No new C++, QPP, blocking primitive, record search, or JNI implementation in this change.
|13. Modern tests|Pass|All changed and new tests use %modern.
|14. Executable tests|Pass|All changed and new qtest files have mode 0755.
|15. Local test dependencies|Pass|Tests use repository-relative %requires, as required by the user; split modules use directory paths.
|16. External dependencies|Pass|Cohere/importer YAML-dependent tests use %try-module; bundled json and Qore modules are hard dependencies.
|17. C++ filesystem sandbox|N/A|No new C++, QPP, blocking primitive, record search, or JNI implementation in this change.
|18. C++ network sandbox|N/A|No new C++, QPP, blocking primitive, record search, or JNI implementation in this change.
|19. Sandbox manager|N/A|No new C++, QPP, blocking primitive, record search, or JNI implementation in this change.
|20. Qore I/O justification|Pass|File reads load pinned module assets; import-tool file/network I/O is its purpose; HTTP/Socket tests use local fixtures and bounded readiness. No sandbox bypass introduced.
|21. C++ loop cancellation|N/A|No new C++, QPP, blocking primitive, record search, or JNI implementation in this change.
|22. Cancellation API|N/A|No new C++, QPP, blocking primitive, record search, or JNI implementation in this change.
|23. Cancellation frequency|N/A|No new C++, QPP, blocking primitive, record search, or JNI implementation in this change.
|24. Blocking operations|N/A|No new C++, QPP, blocking primitive, record search, or JNI implementation in this change.
|25. Action presentation|Pass|All 24 actions have reviewed names and descriptions; tests require nonempty plain short descriptions below 80 characters.
|26. Action options|Pass|RestSchemaActionSet generates catalog options with getActionOptionFromFields; tests compare options with every request contract. Parameterless actions need no options.
|27. Action output types|Pass|23 actions have schema-derived output contracts; cancel-embed-job has no response body by the upstream contract and intentionally no output type (covered by tests).
|28. API execution|Pass|All 24 child paths resolve and support requests; native four providers delegate to the same generated execution path.
|29. Find actions|N/A|No new C++, QPP, blocking primitive, record search, or JNI implementation in this change.
|30. Scheme paths|Pass|Cohere uses scheme cohere and action paths; tests reject a cls property.
|31. Hash slices|Pass|No erroneous single-key field-hash slices introduced; keyed request lookups intentionally select values.
|32. Typed contracts|Pass|Schema-derived HashDataType contracts follow design/data-provider-rest-schema-apps.md; legacy public type classes/constants remain available and expose the complete generated fields.
|33. Input Fields access|Pass|Legacy request Fields remain public for source compatibility; current contracts come from getFields().
|34. Output Fields access|Pass|Legacy response Fields remain private; output types come from the vendored schema.
|35. Field presentation|Pass|Schema fields supply types/descriptions; common field help and enum labels are curated. The 570-message root and all 12 complete sibling locale catalogs pass owner-scoped validation; the full repository presentation test also passes.
|36. Examples|Pass|Chat messages, embedding texts, parsing inputs, token counts, and transcription language include useful examples.
|37. Allowed values|Pass|All 13 top-level finite option enums use readable AllowedValueInfo labels without changing API values; embedding numeric formats use element_allowed_values. Model/object references permit custom values.
|38. Sensitive fields|Pass|API-key constructor/connection options stay sensitive; live keys are read only from environment or the saved connection.
|39. App groups|Pass|Cohere uses AppGroup::AiLlm.
|40. Logo resource|Pass|Existing SVG is loaded in Priv; the public logo alias is retained for compatibility.
|41. App description|Pass|App description links Cohere and lists business uses in Markdown.
|42. App name|Pass|App display name remains Cohere.
|43. Short descriptions|Pass|Catalog contract tests check every action short description; curated descriptions are plain sentences.
|44. Markdown descriptions|Pass|Curated prose uses Markdown code spans for API fields and bold deprecation guidance.
|45. Business language|Pass|Actions explain search, customer requests, document extraction, storage usage, and job management.
|46. Boolean formatting|Pass|No bare boolean/NOTHING literals introduced in curated descriptions.
|47. Field-name formatting|Pass|Curated option references use backticks.
|48. Long descriptions|Pass|Curated action descriptions are short; app description uses a capability list. Upstream schema prose is retained as vendor documentation.
|49. Factory map|Pass|Existing cohere factory and connection-scheme maps point to CohereDataProvider and CohereRestClient; registration tests pass.
|50. Record type signature|N/A|No new C++, QPP, blocking primitive, record search, or JNI implementation in this change.
|51. JNI JARs|N/A|No new C++, QPP, blocking primitive, record search, or JNI implementation in this change.
|52. JAR installation|N/A|No new C++, QPP, blocking primitive, record search, or JNI implementation in this change.
|53. No workarounds|Pass|Root causes fixed in the shared routing/action/response layers. Vendor required-list defects have explicit checked, documented import repairs. No TODO or stub introduced.
|54. Exception safety|Pass|Schema repairs and request changes use Qore value copies. Cached schema is assigned only after construction; AutoLock and on_exit protect resources. Supplied REST clients are copied before path changes.
|55. Thread safety|Pass|Schema/action-set initialization and custom child registration are mutex protected; action contracts are immutable after construction; fixtures guard shared requests.
|56. Type safety|Pass|Manifest, overlays, reference data, provenance, and repairs use typed hashdecls. Auto is limited to JSON/schema polymorphism. Enum metadata and negative-input tests cover contract checks.
|57. Performance|Pass|Schemas/action sets are cached once; literal routes retain hash lookup, partial templates scan only siblings; option constraints run linearly in fields. No runtime schema downloads. Short-description masking scans quoted spans without global replacement or repeated newline searches.
|58. Error handling|Pass|Tests cover unknown actions, missing keys/options, invalid enums, mutually exclusive inputs, malformed/stale repairs, escaped JSON pointers, and empty response values.
|59. API documentation|Pass|Module docs include a business example, supported action scope, legacy behavior, schema update process, and public helper contracts; qdx strict-table extraction passes.
|60. QPP flags|N/A|No new C++, QPP, blocking primitive, record search, or JNI implementation in this change.
|61. Security|Pass|No credentials embedded or logged; URI values are percent-encoded; literals in path patterns are escaped; TLS/authentication stay in the existing transport.
|62. Correctness|Pass|All 24 request fixtures, local HTTP routes/uploads, live authentication and core inference, shared schema suites, and reproducible import/no-drift checks validate the implementation.

## Scope and live evidence

The manifest exports 24 operations across chat, embeddings, reranking, legacy classification, models, tokenization, document image parsing, transcription, datasets, embedding jobs, batches, and key validation. The original four action IDs and public provider/type classes remain available. Deprecated generation, summarization, connector, and fine-tuning administration endpoints are excluded; completed chat does not expose streaming. Model/object dropdowns fetch one listing page and accept custom IDs.

Using the supplied trial key and saved `cohere` connection, valid and invalid authentication checks, chat, embedding, reranking, tokenization, dataset listing/usage, embedding-job listing, and batch listing were exercised successfully. Model listing returned HTTP 429 with Cohere’s monthly-limit message; another batch-list request returned HTTP 502 after an earlier success. These remote responses are not treated as successful pings or hidden. Media actions, legacy classification, and resource mutation actions have local schema/wire tests but have not all been verified against the live service. This audit does not claim the full issue is closed.

The shared short-description formatter now masks complete quoted spans at their original character offsets. Its old numbered-marker replacement could change ordinary words containing a quoted parameter name and overwrite markers when another quoted value was a number. Regressions cover mixed quotes, literal braces, empty/unmatched quotes, apostrophes, Unicode, and the original Cohere token-sampling help.

Cohere’s source catalog contains 570 messages, fully translated in all 12 required locales. Current translations were retained and identical source translations reused where available. New help was translated with protected API literals, reviewed for action labels and terminology, and checked against exact source text. Removed contracts no longer have stale translations. The full repository presentation test enforces both source synchronization and complete standard locales.

Cohere returns an empty JSON object for empty dataset listing/usage; the shared request provider now preserves that object, along with empty lists, zero, false, empty text and binary data.

The schema is imported from the pinned [Cohere developer-experience revision](https://github.com/cohere-ai/cohere-developer-experience/blob/f19a6485639746f919d2b7fc2b63b476e2f39490/cohere-openapi.yaml). Its unmodified SHA-256 is `6f64ec87c66df5857f14283d43c816da5d6a729ca98edd25f23807cd4989a599`. Source counts: 32 paths, 42 operations, 189 schemas; pruned counts: 20 paths and 24 operations. Two upstream required-list defects are corrected explicitly in `cohere-import.yaml` and recorded in the generated artifact; source bytes are not edited manually.

See [key validation](https://docs.cohere.com/reference/check-api-key), [model listing](https://docs.cohere.com/reference/list-models), [trial limits](https://docs.cohere.com/docs/rate-limits), and [OpenAPI path templating](https://spec.openapis.org/oas/v3.1.0.html#path-templating) for the relevant vendor contracts.

## Validation

All Qore tests run with `--enable-debug`, the Release runtime, and local modules. Final affected suites pass 282 cases and 10,043 assertions:

|!Suite|!Cases|!Assertions
|CohereDataProvider (saved connection and live key)|13|344
|CohereRestClient (saved connection and all ping transports)|15|268
|DataProvider|33|2209
|OpenApi3|57|1044
|OpenApi3PathTemplates|3|25
|RestSchemaActionSet|37|415
|RestSchemaImporter|13|137
|RestSchemaPruner|13|72
|RestSchemaDataProvider|11|104
|RestSchemaResponseValues|1|30
|RestSchemaQueryArrays|3|31
|PaddleDataProvider|25|1657
|XeroDataProvider|19|413
|GitLabDataProvider|16|938
|PineconeDataProvider|8|2190
|DataProvider Presentation|15|166

Validation commands (from the repository root):

- `cmake --build build --target CohereDataProvider-qmod -j4` rebuilds the app and its affected dependency qmods; `PaddleDataProvider-qmod` is rebuilt for its corrected manifest; `DataProvider-qmod` is also rebuilt before its regression test.
- `LD_LIBRARY_PATH=build QORE_MODULE_DIR=build/qlib-qmod:build/modules/json:build/modules/reflection:qlib build/qore --enable-debug examples/test/qlib/<Module>/<Suite>.qtest` runs the suites above. For `-c cohere`, append the existing Qorus `qlib` and `user/modules` directories to the module path.
- The saved connection command `rest -G cohere`, executed with the rebuilt runtime and modules, returns `ping result: "OK"`.
- `tools/rest-schema-import.qr --spec qlib/CohereDataProvider/cohere-import.yaml --drift` reproduces the pinned artifact and reports no drift.
- `bin/qore-data-provider-i18n -C --check --require-standard-locales --require-complete-locales -o qlib/CohereDataProvider/i18n -m CohereDataProvider --owner CohereDataProvider`, with the local `i18n` binary module added, verifies the root and all sibling translations.
- `doxygen/qdx --strict-tables` extracts documentation for the affected module sources without warnings or errors.
- `git diff --check` passes. No C++ is changed in this feature commit; the preceding upstream merge was separately tested under Valgrind.

The Cohere/DataProvider presentation edits were followed by rerunning their suites, path-template regressions, the native catalog check, documentation extraction, and the saved connection ping.


## Findings from repository-wide verification

Read-only request properties remain addressable by an explicit manifest overlay so existing ignored-field declarations still validate. Tests cover ordinary and unwrapped bodies, qualified names, and both explicit ignore values; a read-only property never becomes an editable request option. The Paddle create-customer manifest incorrectly described its read-only ID as an import input. That field was removed from the form and ordering, with a negative request test. [Paddle’s request contract](https://developer.paddle.com/api-reference/customers/create-customer/) lists the ID only in the response.

The shared fixes also required source/translation updates in GitLab (four short descriptions), Paddle (27 removed messages and two corrected summaries), and Xero (11 removed messages). Source-tree verification found a previously missing Pinecone Admin catalog and 30 missing record-action messages from the existing Pinecone implementation. Those catalogs are generated and fully translated as well. Existing catalog indentation is preserved.

The final Presentation test passes all 15 cases, including its repository-wide source-tree check with complete standard locales. Its deliberate invalid-catalog fixtures print expected diagnostics. API literals and URLs are preserved across all 10,332 new or changed translation entries. The additional provider suites pass without live credentials; live coverage of the feature remains as stated above.
