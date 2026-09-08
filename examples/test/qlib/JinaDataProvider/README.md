# Jina AI provider verification

Copyright 2026 Qore Technologies, s.r.o.

The tests cover all 19 Jina actions, form metadata, model choices, schema validation, wire contracts,
reference data, authentication isolation, JSONL parsing, invalid input, and remote errors.
The [verification record](audits/5432-verification.md) distinguishes successful local checks from
service failures that prevent a complete live classifier or batch-output round trip.

Build the current module before testing:

```bash
cmake --build build --target JinaDataProvider-qmod
QORE_MODULE_DIR=build/qlib-qmod:build/modules/json:qlib LD_LIBRARY_PATH=build \
  env -u JINAAI_APIKEY build/qore --enable-debug \
  examples/test/qlib/JinaDataProvider/JinaDataProvider.qtest
```

For live checks, supply `JINAAI_APIKEY` or use `--connection jinaai`. The connection option verifies
an installed Qorus connection; synchronize the modules and rebuild the provider index first.
`JINA_TEST_DEEPSEARCH=1` enables two bounded research requests. The default live batch test requests cancellation of
its pending job. Jina later changed an acknowledged cancellation to a worker failure; see the verification record. `JINA_KEEP_BATCH=1` retains it for later download verification using `JINA_BATCH_ID`;
there is no polling loop. Only use a batch created by this test, whose record ID is `qore-5432` and
whose embedding dimension is `32`.

`JINA_TEST_CLASSIFIERS=1` enables training, listing, classification, updating, re-listing, deletion,
and verification that the classifier is gone. The test uses private training data and cleans up
its classifier on failure. On September 8, 2026, Jina rejected training with HTTP 500; this optional
test fails explicitly and does not silently skip the affected lifecycle.

After installing the Qore module component, publish it with `copy-qore-modules`, run
`qctl update-index`, and restart the platform before checking `qdp @JinaAi` and installed actions.
Never overwrite a loaded `.qmod` in place; the repository installer and publishing helper install
AOT artifacts atomically.
