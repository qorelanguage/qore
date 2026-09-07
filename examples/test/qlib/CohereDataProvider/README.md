# Cohere integration tests

Copyright 2026 Qore Technologies, s.r.o.

Run against the local modules after rebuilding `CohereDataProvider-qmod`:

```sh
QORE_MODULE_DIR=build/qlib-qmod:build/modules/json:build/modules/reflection:qlib \
LD_LIBRARY_PATH=build \
build/qore --enable-debug examples/test/qlib/CohereDataProvider/CohereDataProvider.qtest
```

The suite checks all 24 action contracts, request routes, validation, dropdowns, and real HTTP multipart
serialization using a local server. When `COHERE_APIKEY` is set, it also checks live key validation.
Use `-c cohere` to test embedding through the saved connection; include the local Qorus module paths
in `QORE_MODULE_DIR` when the connection registry requires them.

Optional live checks:

- Set `COHERE_TEST_IMAGE` to a PNG containing text and `COHERE_TEST_AUDIO` to a short English audio
  file with a supported extension, such as `.wav`. Both are required to enable the media case.
- Set `COHERE_TEST_IMAGE_TEXT` and `COHERE_TEST_AUDIO_TEXT` to expected output substrings for stronger
  assertions. The issue 5433 fixtures contain `QORE-5433` and speak an invoice total of `42 euros`.
- Set `COHERE_TEST_RESOURCES=1` to create a one-record JSONL dataset, retrieve it, delete it, and
  verify the deleted ID returns HTTP 404. Cleanup also runs if an assertion fails after creation.

The resource test does not wait for asynchronous validation. Live embedding-job completion and
batch lifecycle results are recorded in the accompanying audit. Cohere requires a production API
key to create v2 batches. A trial key may reject model listing with HTTP 429 while still allowing
individual model lookup, inference, parsing, transcription, and v1 embedding jobs.

Production verification for issue 5433 used `COHERE_PROD_APIKEY` from the local profile, without
changing the saved trial-key connection. Model listings and task dropdowns succeeded. A single
batch record using `command-r7b-12-2024`, the prompt `Reply OK.`, and `max_tokens: 4` exercised
creation, cancellation, retrieval, and output validation. Cancellation was accepted while queued,
but the record completed before cancellation took effect. Both test datasets were deleted.
See [the production audit](audits/5433-production.md) for results and the exact scope of validation.

Cohere's [September 2025 retirement notice](https://docs.cohere.com/changelog/2025-09-15-major-command-deprecations)
covers classification fine-tuning and previously fine-tuned models. The production account lists
no classification models, and the classification dropdown correctly returns no options. A production
key alone cannot enable a successful legacy classification check. Use Chat with structured output
for new classification workflows.
