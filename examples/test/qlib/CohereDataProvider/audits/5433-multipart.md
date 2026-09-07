# Cohere multipart and live verification audit for issue 5433

Copyright 2026 Qore Technologies, s.r.o.

The subsequent [production verification](5433-production.md) updates the model-list and batch
coverage and corrects the classification availability guidance recorded in this earlier audit.

Applied `/home/david/.codex/skills/audit-changes/SKILL.md` to the multipart follow-up after the
24-action implementation. The upstream regex merge is audited separately. This report covers
all 62 checklist items for the Qore modules, tests, and documentation in this commit.

|!Check|!Status|!Evidence
|1. Module index|N/A|No new module index entry is required.
|2. Release notes|Pass|Mime, OpenApi3, RestSchemaActions, RestSchemaDataProvider, and Cohere release notes describe named multipart uploads and boundary quoting.
|3. CMake registration|N/A|No new module or packaged resource; existing build targets apply.
|4. Qmod registration|N/A|No new module; existing qmod targets are rebuilt.
|5. Introduction section|Pass|Existing module introduction sections and entry points are retained.
|6. Modern directives|Pass|All edited module entry points use %modern; no redundant directives added.
|7. Split-file directives|Pass|The two edited split .qc files contain no parse directives.
|8. No module includes|Pass|No %include added.
|9. Copyright|Pass|New tests, README, and audit use copyright 2026; updated Mime test now carries it explicitly.
|10. Directory layout|Pass|Existing standalone Mime/OpenApi3 modules and split REST/Cohere directories retain their layout.
|11. Single entry point|Pass|No duplicate module entry point.
|12. QPP namespace|N/A|No QPP change in this commit.
|13. Modern tests|Pass|All three updated/new qtest files use %modern.
|14. Executable tests|Pass|All three qtest files are mode 0755.
|15. Local test dependencies|Pass|All three qtest files prepend qlib and require repository-relative local modules.
|16. External dependencies|Pass|External YAML remains guarded by %try-module; bundled JSON and Qore modules are hard dependencies.
|17. C++ filesystem sandbox|N/A|No C++ I/O change.
|18. C++ network sandbox|N/A|No C++ I/O change.
|19. Sandbox manager|N/A|No new low-level I/O.
|20. Qore I/O justification|Pass|Runtime serialization performs no file/network I/O. Opt-in live tests read caller-supplied fixtures and use the existing sandbox-aware REST client; dataset cleanup runs on_exit.
|21. C++ loop cancellation|N/A|No C++ change in this commit.
|22. Cancellation API|N/A|No C++ change in this commit.
|23. Cancellation frequency|N/A|No C++ change in this commit.
|24. Blocking operations|N/A|No blocking runtime operation is added.
|25. Action presentation|Pass|All 24 existing action names and descriptions remain valid; the presentation tests pass.
|26. Action options|Pass|Generated actions and raw schema providers derive the same multipart-aware request field; wire tests exercise the real action path.
|27. Action output types|Pass|Schema response types remain unchanged; the unit test confirms cached binary wire types are not widened by request field construction.
|28. API execution|Pass|Existing request execution delegates to RestSchemaRequestDataProvider; named file requests pass through normal schema validation.
|29. Find actions|N/A|No record-search action changed.
|30. Scheme paths|Pass|Cohere retains scheme-based action paths; existing registration tests pass.
|31. Hash slices|Pass|No erroneous single-key hash slices added.
|32. Typed contracts|Pass|FileDataType and SoftBinaryType form an explicit union for multipart binary request properties; enclosing HashDataType and field copies preserve cached contracts.
|33. Input Fields access|N/A|No new hand-written input type; operation-generated request fields are public through getFields().
|34. Output Fields access|N/A|No output type implementation changed.
|35. Field presentation|Pass|Existing field help and enums are preserved; native Cohere catalog validation and full repository presentation validation pass without catalog changes.
|36. Examples|Pass|The Cohere module includes a realistic WAV upload example; the test README documents live fixture inputs.
|37. Allowed values|Pass|No finite allowed values changed; original action enum validation remains covered.
|38. Sensitive fields|Pass|Live credentials are read only through COHERE_APIKEY or the saved cohere connection.
|39. App groups|N/A|No app group change.
|40. Logo resource|N/A|No logo change.
|41. App description|N/A|No app description change.
|42. App name|N/A|No app name change.
|43. Short descriptions|Pass|No action short description changed; full presentation checks remain valid.
|44. Markdown descriptions|Pass|New prose quotes field names and file examples with Markdown code spans.
|45. Business language|Pass|The example transcribes a delivery note; tests parse and embed invoice data.
|46. Boolean formatting|Pass|No bare boolean/NOTHING literal introduced in public prose.
|47. Field-name formatting|Pass|New public field references use code spans.
|48. Long descriptions|N/A|No long public action descriptions added.
|49. Factory map|N/A|No factory registration change.
|50. Record type signature|N/A|No record type signature change.
|51. JNI JARs|N/A|No JNI dependency.
|52. JAR installation|N/A|No JNI installation change.
|53. No workarounds|Pass|The shared MIME layer quotes RFC 2046 boundary parameters. The OpenAPI layer retains named-file metadata and separates ordinary form fields from uploads; no provider-specific header or extension workaround.
|54. Exception safety|Pass|Validation operates on local Qore value copies and commits the checked value only after success. Field/type copies preserve interned schema state. Live dataset cleanup is registered before creation.
|55. Thread safety|Pass|New mutable values belong to one serialization or type-construction call; no new shared state or unlocked cache is introduced.
|56. Type safety|Pass|Public signatures use AbstractDataField, PathItemObject, MessageInfo, and FormDataMessageInfo. Polymorphic JSON remains auto; named values use FileDataType validation.
|57. Performance|Pass|Multipart assembly and property inspection are linear. Qore copy-on-write preserves caller values and avoids mutating shared types; files are processed with the existing binary/base64 conventions.
|58. Error handling|Pass|Tests cover boundary length/characters, CRLF and invalid Unicode boundaries, requiredness, missing filename/content, empty filename, invalid content values, unknown file fields, empty files, and invalid requests before network I/O.
|59. API documentation|Pass|Public methods document inputs/outputs and relevant errors. Durable design, module release notes, realistic example, and opt-in live test setup are updated; qdx strict extraction passes.
|60. QPP flags|N/A|No QPP change.
|61. Security|Pass|Boundary validation rejects CRLF and other invalid characters before header assembly. Binary data remains byte-exact. No credential or signed download URL is committed.
|62. Correctness|Pass|Unit, local HTTP, and live Cohere tests verify serialization and file metadata. An independent Go mime/multipart parser verifies the emitted request. Trial-key production restrictions are documented below.

## Root causes and live results

The generated boundary includes the timezone offset, such as `+02:00`, but the MIME serializer
previously emitted it without quotes. Cohere's parser rejected the whole Content-Type header.
[RFC 2046 section 5.1.1](https://www.rfc-editor.org/rfc/rfc2046#section-5.1.1) explicitly requires
quoting boundaries containing a colon. Quoting fixes this in the shared MIME implementation.

The OpenAPI request serializer also replaced file names with field names and assigned filenames
to ordinary text parameters. Transcription rejected the missing audio extension; dataset upload
succeeded but asynchronous validation rejected the extensionless data file. Named `FileDataType`
inputs now retain the original filename and media type, while text parameters precede files.
Existing binary inputs continue to use their field name as the legacy filename.

Live checks with the supplied trial key on 2026-09-07:

|!Action or operation|!Result
|Key validation and saved connection ping|Pass, including rejection of an invalid key
|Individual model lookup|Pass for command-a-03-2025
|Document parsing|Pass; the synthetic invoice returns QORE-5433 and Total: 42 EUR
|Audio transcription|Pass; the spoken invoice sentence returns the expected 42 euros
|Dataset create/get/delete|Pass; JSONL input validates; deleted IDs return HTTP 404
|Embedding job create/get|Pass; a one-record job completes and exposes an embed-result dataset
|Embedding job cancellation|Pass; immediate cancellation of a newly created job transitions through cancelling to cancelled
|Batch input upload|Pass; a one-record batch-chat-v2-input dataset validates
|Batch create/get/cancel lifecycle|Blocked: Cohere rejects batch creation with HTTP 400 stating that a production API key is required; no batch was created
|Model listing|Blocked: HTTP 429 reports the trial key's monthly request limit; individual lookup and the other listed actions still work

The batch JSONL fixture has `custom_id` and `body`, with messages represented as typed content
blocks. The server's returned validation schema confirmed this layout. Every test-created input
and output dataset was deleted. Completed and cancelled embedding-job history is retained by Cohere; there is
no job-deletion action. No existing account dataset or job was modified.

The live media and dataset CRUD checks are reproducible through the documented opt-in environment
variables in the test README. Live production-only checks remain pending a suitable credential.

## Validation

Before and after merging upstream through `1fa5ce84c`: 238 cases / 3,710 assertions across 14 suites passed with
`qore --enable-debug`, including 15 Cohere app cases / 369 assertions with live media and resource
checks, and 15 Cohere connection cases / 268 assertions through the saved connection.

Cohere native catalogs and all 12 locales pass complete-locale validation. The full repository
Presentation suite passes (its deliberate negative fixtures print diagnostic errors as expected).
Strict qdx documentation extraction passes for all five affected module entry points. Go's standard
MIME parser rejects the old unquoted header and accepts the corrected envelope, ordinary fields,
filename, media type, exact binary bytes, and final multipart boundary.

All 62 audit items above are Pass or N/A after post-merge verification. No C++ changes are included
in this multipart follow-up, so Valgrind is not required for this commit.
