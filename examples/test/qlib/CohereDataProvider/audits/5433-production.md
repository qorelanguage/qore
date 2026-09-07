# Cohere production verification for issue 5433

Copyright 2026 Qore Technologies, s.r.o.

Applied `/home/david/.codex/skills/audit-changes/SKILL.md` to this follow-up on `develop`.
Scope: corrected classification help, its 12 translations, module release notes, test documentation,
and this production evidence. There is no runtime algorithm, public contract, or test-code change.
All 62 checklist items are Pass or N/A after the verification recorded below.

|!Check|!Status|!Evidence
|1. Module index|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|2. Release notes|Pass|Cohere v1.1 release notes document the corrected classification guidance; the test README records production validation.
|3. CMake registration|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|4. Qmod registration|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|5. Introduction section|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|6. Modern directives|Pass|The existing CohereDataProvider entry point retains %modern.
|7. Split-file directives|Pass|CohereManifest.qc contains no parse directives; no split-file structure change.
|8. No module includes|Pass|No %include added.
|9. Copyright|Pass|This new audit carries copyright 2026; the module and manifest retain their 2026 notices.
|10. Directory layout|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|11. Single entry point|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|12. QPP namespace|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|13. Modern tests|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|14. Executable tests|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|15. Local test dependencies|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|16. External dependencies|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|17. C++ filesystem sandbox|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|18. C++ network sandbox|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|19. Sandbox manager|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|20. Qore I/O justification|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|21. C++ loop cancellation|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|22. Cancellation API|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|23. Cancellation frequency|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|24. Blocking operations|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|25. Action presentation|Pass|All 24 action contracts remain covered by the existing Cohere suite; only the legacy classification help changes.
|26. Action options|Pass|Action options remain populated through the schema action set; no option schema change.
|27. Action output types|Pass|All typed response contracts remain unchanged and covered by contract/transport tests.
|28. API execution|Pass|Production model and batch actions execute through the real Cohere provider and REST transport.
|29. Find actions|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|30. Scheme paths|Pass|Cohere retains scheme-based action paths; no cls registration is added.
|31. Hash slices|Pass|No hash slice changes.
|32. Typed contracts|Pass|Existing schema-derived request/response types are unchanged; the batch response was validated against real server data.
|33. Input Fields access|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|34. Output Fields access|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|35. Field presentation|Pass|Classification help and all 12 translations share the updated source; complete native-catalog validation verifies the catalogs.
|36. Examples|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|37. Allowed values|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|38. Sensitive fields|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|39. App groups|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|40. Logo resource|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|41. App description|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|42. App name|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|43. Short descriptions|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|44. Markdown descriptions|Pass|The updated action description uses Markdown bold for the retirement caveat; the complete source/locale catalogs preserve it.
|45. Business language|Pass|The help tells users to use Chat with structured output for new classification workflows.
|46. Boolean formatting|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|47. Field-name formatting|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|48. Long descriptions|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|49. Factory map|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|50. Record type signature|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|51. JNI JARs|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|52. JAR installation|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|53. No workarounds|Pass|No runtime workaround or stub added. The obsolete fine-tuned-model advice is corrected using the current vendor retirement notice and live model metadata.
|54. Exception safety|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|55. Thread safety|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|56. Type safety|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|57. Performance|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|58. Error handling|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|59. API documentation|Pass|The module links the vendor retirement notice; README and audit document actual production coverage, token usage, and remaining limits.
|60. QPP flags|N/A|No corresponding module, runtime, public type, test, I/O, or dependency change in this documentation follow-up.
|61. Security|Pass|Credentials remain environment variables; no keys, signed download URLs, or account identifiers are added to repository files.
|62. Correctness|Pass|Production listing, four task dropdowns, one-record batch creation/cancellation/retrieval/output checks, and cleanup are verified; local regression and documentation checks are recorded below.

## Production results

Verified on 2026-09-07. The production key was supplied through `COHERE_PROD_APIKEY` in the user's local profile.
The saved `cohere` connection remains configured with its trial key. All Qore scripts ran with
`--enable-debug` and the local Release build/modules. Remote `develop` matched the local checkout
at the start of verification.

|!Operation|!Result
|Check API key|Pass; the production key validates.
|List models|Pass; one complete page contains 32 models and no continuation token.
|Chat model dropdown|Pass; 16 names match the chat-capable models in the complete listing.
|Embedding model dropdown|Pass; 5 names match the embedding-capable models in the complete listing.
|Rerank model dropdown|Pass; 5 names match the reranking-capable models in the complete listing.
|Classification model dropdown|Pass; the service returns no models and the provider returns no options.
|Upload batch input|Pass; a single JSONL record validates as batch-chat-v2-input.
|Create batch|Pass; command-r7b-12-2024 accepts the uploaded dataset and returns BATCH_STATUS_QUEUED.
|Cancel batch|Pass for the cancellation API; the request succeeds and retrieval observes BATCH_STATUS_CANCELING.
|Retrieve completed batch|Pass; the single record finishes before cancellation takes effect and the final state is BATCH_STATUS_COMPLETED, with one successful record and zero failed records.
|Read batch output|Pass; the output dataset contains one Avro record with the original custom_id, a nonempty assistant response, no record error, and finish_reason MAX_TOKENS.
|Cleanup|Both the test input dataset and the generated output dataset are deleted. Cohere retains completed batch history; there is no batch-deletion action.

The input was `Reply OK.` with `max_tokens: 4`. Exactly one batch record was submitted.
Both batch statistics and the decoded output report **3 billed input tokens and 3 billed output
tokens**. The response is truncated by the deliberately small output limit; output validation checks
transport/content integrity, not instruction-following quality. No additional inference job was
submitted to force a terminal cancellation state. The cancellation endpoint and intermediate state
are verified; a terminal `BATCH_STATUS_CANCELED` transition is not claimed.

Model checks use metadata requests. No fine-tuning or classification inference was initiated.
This keeps paid inference to the one tiny batch record requested for verification. API keys and
signed URLs are not stored in the repository.

## Classification availability correction

The previous audits described positive classification verification as requiring a suitable fine-tuned
model. That reflected the older January 2025 guidance and was incomplete. Cohere's later
[September 2025 retirement notice](https://docs.cohere.com/changelog/2025-09-15-major-command-deprecations)
retires classification fine-tuning and previously fine-tuned models and marks `/v1/classify` as
deprecated. The production account's complete model list has no classification-capable model;
the task-filtered dropdown also returns no options.

A production key therefore does not supply the missing legacy classification model. Positive live
classification remains unavailable for this account. The compatibility action remains registered,
but its help now states the retirement and directs new workflows to Chat with structured output.
All 12 localized descriptions carry the same guidance.

## Local verification

Rebuilt `CohereDataProvider-qmod` before running the checks. Credentials were removed from regression-test
environments to avoid repeating paid API requests.

|!Check|!Result
|CohereDataProvider.qtest|14 cases / 354 assertions pass; live inference cases are skipped explicitly without credentials.
|Presentation.qtest|15 cases / 166 assertions pass, including the complete source-tree catalog check. Deliberate invalid-catalog fixtures emit their expected diagnostics.
|Native Cohere catalog validation|The app and catalog domain pass --require-standard-locales and --require-complete-locales for all 12 translations.
|Strict documentation extraction|qdx --strict-tables succeeds for CohereDataProvider.
|Whitespace and secret checks|git diff --check succeeds; no API keys, signed URLs, or account identifiers are included.
|Valgrind|N/A: no C++ change.

All Qore tests ran with `--enable-debug`. The production request fixtures, decoded batch output, and
local logs are retained under `/tmp/cohere-production-5433`; credential values are not written there.
