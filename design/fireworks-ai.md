# Fireworks AI integration

The `FireworksAi` app provides 132 actions through the `fireworks` connection scheme. The original
`chat-completion` and `create-embeddings` action names remain stable. `FireworksRestClient` also supplies
the `fireworks` LLM and embedding adapters.

## Transport and authentication

The synchronous and asynchronous clients combine the connection URL path with the API version.
For the default URL, a relative `chat/completions` request addresses
`https://api.fireworks.ai/inference/v1/chat/completions`. An explicitly supplied client URL retains its
path prefix. Schema action providers use a separate client with the connection path cleared, because
each schema supplies its complete API prefix; constructing a provider does not mutate its caller's client.

Connections require a sensitive `apikey` and use Bearer authentication. Both connection constructors
migrate the previously shipped `models`, `/models`, and `/v1/models` ping paths to
`/v1/accounts?pageSize=1`. This absolute path reaches the control API outside `/inference` and verifies
authentication without generating tokens. HTTP 401 must fail on blocking and asynchronous ping paths.
The configuration-hash constructor also replaces the automatic `fireworks://localhost` fallback with
the canonical service URL. Custom ping settings remain available.

The default chat model is `accounts/fireworks/models/gpt-oss-120b`; the default embedder uses
`accounts/fireworks/models/qwen3-embedding-8b`. Explicit model selections take precedence. Model
availability can change independently of the client, so inference forms use live serverless catalogs.

## Schema ownership and coverage

Five separately published vendor documents retain their individual base paths. Merging them would lose
path-level server routing, which the schema pruner deliberately rejects.

|!Document|!Actions|!API prefix
|Gateway control API|122|`/v1/accounts/...`
|Dataset upload API|1|`/v1/accounts/...`
|Text completion API|2|`/inference/v1/...`
|Responses API|4|`/inference/v1/...`
|Anthropic-compatible Messages API|1|`/inference/v1/...`

`FireworksManifest.qc` explicitly selects operations and owns their presentation. `FireworksSchema.qc`
records the checksum, size, version, and counts of each unmodified upstream document. A mutex protects
lazy construction; the completed action-set cache is published only after every set has been built.
Inference, upload, and response sets refer to the control set for resource dropdowns.

Re-import the schemas from the repository root:

```sh
LD_LIBRARY_PATH=build QORE_MODULE_DIR=qlib build/qore --enable-debug tools/rest-schema-import.qr \
    --spec qlib/FireworksDataProvider/fireworks-import.yaml
```

Imports use `RestSchemaPruner` and update provenance in the same operation. The checked-in artifacts
are generated output and must not be edited by hand. Every schema is included in the CMake resource
list so source, compiled, and installed modules use the same files.

The exported families cover account discovery, model artifacts and versions, deployments and LoRAs,
deployment shapes, datasets and uploads, batch inference, supervised/preference/reinforcement
fine-tuning, evaluations and evaluators, routing, usage, billing, quotas, and audit logs. Session-token
exchange, internal training-session coordination, enterprise identity and secret/key administration,
developer passes, and deprecated evaluator creation are excluded. Shape and quota catalogs are read-only.
The generic REST action remains available for specialized requests.

Embeddings and reranking use handwritten types based on the published endpoint references. Their
legacy inference schema URL (`openapi.yml`) is not served; the maintained text schema does not include
these operations. The embedding action supports text or structured inputs, prompt templates, output
dimensions, logits, normalization, and the existing encoding option. Reranking supports a model, query,
documents, task instruction, result count, and optional returned text.

## Forms and reference data

Account-scoped resource dropdowns use `account_id` and declare dependencies so changing the account
refreshes the selection. The resource-ID codec accepts either a short ID or the full resource name
returned by a listing, then sends the final ID segment in the path. Listing actions expose page size,
page token, filters, and ordering wherever declared by the API.

Inference model dropdowns first list accessible accounts, then query each account's `serverlessModels`
catalog. The public `fireworks` model catalog includes offline and retired models and remains useful for
model management and training. Dropdowns fetch one page per scope and accept custom values, including
model/router names and resources omitted from a page. They never generate tokens.

Schema enums, requiredness, nested request/response types, and read-only fields determine the forms.
Overlays add reference data, date codecs, examples, and useful preselected options. Chat accepts either
`messages` or `prompt_token_ids`; completed-response actions hide streaming options. Streaming remains
available through the LLM adapter. Chat's `created` field remains a Qore date for compatibility.

The source-owned presentation catalog includes complete translations for Czech, German, Spanish,
French, Italian, Japanese, Korean, Polish, Slovak, Ukrainian, and Traditional Chinese (`zh-Hant` and
`zh-TW`). Every locale carries each root message; action and option text does not depend on English
fallback. Code examples, wire values, URLs, and product names retain their original spelling.
After changing presentation metadata, regenerate the root catalog, update every locale, and validate:

```sh
LD_LIBRARY_PATH=build QORE_MODULE_DIR=build/qlib-qmod:build/modules/i18n:qlib \
    build/qore --enable-debug bin/qore-data-provider-i18n -C --check \
    --require-standard-locales --require-complete-locales \
    -o qlib/FireworksDataProvider/i18n -m FireworksDataProvider --owner FireworksDataProvider
```

## Verification and costs

The REST-client test uses a local HTTP server to verify exact paths, Bearer headers, connection migration,
LLM/embedding adapters, and all four ping transports. Setting `FIREWORKSAI_APIKEY` enables additional
valid/invalid-key ping checks, which perform no inference.

The provider test checks all 130 schema operations against explicit request/response fixtures, schema
drift, action contracts and paths, cascading dropdowns, multipart upload serialization, reranking,
embedding options, and invalid input. Its default run does not use the network. Live verification is
explicit. Use `--live` with `FIREWORKSAI_APIKEY`, or select a named connection with `-c`:

```sh
LD_LIBRARY_PATH=build QORE_MODULE_DIR=qlib:build/modules/reflection:build/modules/astparser \
    build/qore --enable-debug examples/test/qlib/FireworksDataProvider/FireworksDataProvider.qtest --live
```

A named connection also requires its connection-provider modules on `QORE_MODULE_DIR`:

```sh
LD_LIBRARY_PATH=build QORE_MODULE_DIR="qlib:build/modules/reflection:build/modules/astparser:$QORE_MODULE_DIR" \
    build/qore --enable-debug examples/test/qlib/FireworksDataProvider/FireworksDataProvider.qtest \
    -c fireworksai
```

The live test reads account/resource catalogs and makes three small inference requests: chat capped at
64 output tokens, one embedding input, and two rerank candidates. It reports token usage. It does not
create deployments, training jobs, evaluations, or batch jobs. The handwritten inference transport does
not retry POST requests after a lost response, avoiding duplicate paid work.

Sources: [Gateway schema](https://docs.fireworks.ai/gateway.openapi.yaml),
[embeddings reference](https://docs.fireworks.ai/api-reference/creates-an-embedding-vector-representing-the-input-text),
[reranking reference](https://docs.fireworks.ai/api-reference/rerank-documents).
