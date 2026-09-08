# Mistral data provider

MistralDataProvider 1.1 exposes 161 completed-response operations from the official
Mistral OpenAPI document pinned at revision `2e094f7bbe1395de4a738a3483def3573143d973`.
The original `chat-completion`, `create-embeddings`, and `ocr` action IDs and public
provider classes are retained. Their request and response classes now use the
schema-derived fields; the legacy `Fields` constants remain for source compatibility.
Chat responses retain the original epoch-to-date conversion of `created`.

The committed schema determines paths, HTTP methods, requiredness, nested types,
validation, and wire formats. `MistralManifest` selects stable action names and adds
presentation, finite choices, examples, and 24 resource dropdowns. Nullable enums
and arrays retain the same choices and element lookups as their non-nullable forms.
Parent-scoped dropdowns require the selected library, agent, conversation, or dataset. Listings fetch one
page; custom identifiers remain accepted and normal action options expose pagination. Dataset-record selectors enumerate one page of records
for each dataset in the first dataset page, because records are addressed globally but listed per dataset.

The 161 actions cover models, chat, fill-in-the-middle completion, embeddings,
moderation/classification, files, batch inference, OCR, audio, agents, conversations,
libraries and documents, connectors, evaluation resources, and workflows. Twelve
upstream operations are excluded: six deprecated operations and six streaming-only
variants. Streaming chat belongs to the Mistral LLM provider. Publishing a new
upstream operation requires an explicit manifest change.

`MistralSchema` records upstream provenance and loads the vendored JSON resource
under a mutex. Module initialization builds and registers the action set; execution
does not fetch schemas. CMake installs the JSON, import specification, and logo.
The schema is deliberately stored as JSON, which the importer supports independently
of the YAML serialization bug fixed in module-yaml while importing this API.

Re-import using the local Qore modules and a YAML module for the upstream document:

```sh
QORE_MODULE_DIR=qlib tools/rest-schema-import.qr --spec qlib/MistralDataProvider/mistral-import.yaml
```

Review provenance and run the importer drift
check against `MistralSchema::getActionSet()` before adopting a newer revision.
The import records two reviewed upstream defects:

- `SharingDelete` requires an absent `level` property. The official SDK requires
  only `share_with_uuid` and `share_with_type`. The repair checks the old required list.
- Connector reference paths include documentation fragments and use different names
  for the same path parameter. The repair removes the fragments, renames the parameter
  declarations positionally, and merges disjoint HTTP methods. Collisions, stale
  sources, and changed parameter counts fail the import.

Connections identify the `MistralAi` app and use the canonical Mistral URL when built
from generated connection configuration. The API key is required and sensitive.
Both sync and async pings authenticate at `/v1/models`, regardless of the direct
client's selected API version. Action providers copy the connection client and clear
its prefix because the schema uses explicit versioned paths. The caller's client
retains its original prefix. Requests are sent once; ambiguous transport failures
must not duplicate inference charges or resource creation.

The OpenApi3 implementation handles JSON parameter content used by agent and
conversation metadata filters. Required-property presence is independent of nullability: an explicit null
filter remains valid through field caching, overlays and JSON serialization; omitting the required
property fails. Nullable scalar and composition types retain that distinction. Content objects are individual query values rather
than catch-all query objects. Parameters are validated as native values, serialized
once, and URI-escaped. Multipart file fields accept named files through nullable
binary schemas, preserving filename, content bytes, and media type.

Tests include a fixed request fixture for every action, catalog/type checks, all
resource dropdowns, missing parent scopes, negative requests, no automatic retries,
legacy classes, a local HTTP server, and optional live tests. `MistralLive.qtest` uses
`MISTRALAI_APIKEY`; OCR and transcription additionally use `MISTRAL_TEST_PDF` and
`MISTRAL_TEST_AUDIO`. It deletes test-created files, agents and libraries, and cancels
unfinished test-created batch jobs. No test polls job completion. Live account limits
are explicit skips: billing-disabled batch jobs and unavailable preview services do
not imply invalid authentication. Unit tests still exercise those request contracts.
