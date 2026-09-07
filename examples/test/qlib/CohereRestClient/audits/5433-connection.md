# Cohere connection audit — issue 5433

Copyright (C) 2026 Qore Technologies, s.r.o.

Audit date: 2026-09-07. Applied `/home/david/.codex/skills/audit-changes/SKILL.md` to this commit's scope before committing.
The broader action coverage audit is a separate part of issue 5433.

The trial key was valid but its monthly quota was exhausted: both `/v2/models` and the documented
`/v1/models` returned HTTP 429 with the monthly-limit message. `POST /v1/check-api-key` returned
`{valid: true}` for the trial key and `{valid: false}` for an intentionally invalid key, both with HTTP 200.
A status-only ping therefore cannot authenticate this endpoint. Cohere's reference currently categorizes
it as deprecated, while the published OpenAPI schema still includes it without a deprecation flag.

Sources:
- https://docs.cohere.com/reference/check-api-key
- https://docs.cohere.com/reference/list-models
- https://docs.cohere.com/docs/rate-limits

|!Check|!Status|!Evidence
|Changed-file scope|Pass|CohereRestClient.qm, RestClient.qm, RestClientIo.qm, CohereRestClient.qtest, release notes, and this report. Existing astparser edits are excluded.
|Module index|Pass|All three existing modules remain listed in 120_modules.dox.tmpl; no new module.
|Release notes|Pass|Cohere and RestClient module notes plus the Qore 3.0 Cohere entry describe the change.
|CMake registration and QMOD list|Pass|Existing qore_user_module registrations build all affected qmods; no new modules or assets.
|Intro section, modern directives, layout|Pass|Existing single-file modules retain lowercase intro sections and %modern.
|Separated source directives, includes, duplicate modules|N/A|No separated Qore source files or new modules in this commit.
|Copyright|Pass|Updated files carry 2026; test copyright added.
|QPP namespace paths|N/A|No C++ or QPP changes.
|Test modern directive and executable bit|Pass|CohereRestClient.qtest uses %modern and retains mode 100755.
|Test local module resolution|Pass|Prepend local qlib before explicit relative requirements; directory requirements for split modules.
|Test external dependencies|Pass|Only Qore-delivered dependencies are required, including the bundled json module.
|C++ filesystem/network sandbox checks|N/A|No C++ changes.
|Qore sandboxing|Pass|Production code uses the existing REST transport. Test listener is local; Socket::poll drives the public I/O API with a bounded readiness deadline.
|Cancellation loops, frequency and blocking calls|N/A|No C++ changes; no new production loops or blocking operations.
|DP action names, descriptions, options and output types|N/A|No action registrations changed in this connection commit.
|DP API/find contracts, action paths and hash slices|N/A|No action providers changed.
|DP typed request/response classes and fields|N/A|No data types changed.
|DP allowed values and sensitive fields|N/A|No DP fields changed; connection API versions have AllowedValueInfo entries and apikey remains sensitive and required.
|DP app groups, logo and description|N/A|No app registrations changed; getAppName now returns the existing Cohere identity.
|DP descriptions, examples, finite values and long-form formatting|N/A|No DP presentation changes.
|FactoryMap and record type signature|Pass|Existing cohere factory and scheme map entries verified; no record types changed.
|JAR packaging and install rules|N/A|No JNI or JAR changes.
|No workarounds or stubs|Pass|Correct endpoint plus mandatory response validation; the default no-op hook is the intentional base contract, not an unfinished implementation.
|Exception safety|Pass|Validation errors call authFailure before propagating; parse failures do not report response contents; test operations abort on exit and the HTTP server is torn down.
|Thread safety|Pass|No new shared production state; fake server state is protected by AutoLock.
|Type safety|Pass|Connection options use typed hashdecls; valid must have NT_BOOLEAN type and be true, rejecting truthy strings and numbers.
|Performance|Pass|One small authentication request; no inference call, retry loop, extra connection or shared cache.
|Error handling|Pass|Invalid, absent and malformed validity plus HTTP 401, 429 and 500 fail on synchronous and both polling transports.
|Public API documentation|Pass|New hook documents input representations and errors; Cohere documents trial behavior and includes a usable example.
|QPP flags|N/A|No QPP changes.
|Security|Pass|Credentials come from the environment or existing connection; no secrets in tracked files or reports. Invalid credentials fail even with HTTP 200.
|Correctness and corner cases|Pass|Four transport paths verify POST /v1/check-api-key on the wire; tests cover saved defaults, URL canonicalization, missing key, invalid responses and recovery.
|Valgrind|N/A|No C++ source changes.

Validation:

- `cmake --build build --target CohereRestClient-qmod -j4` rebuilt all three affected modules successfully.
- `build/qore --enable-debug examples/test/qlib/CohereRestClient/CohereRestClient.qtest`: 15 registered cases,
  259 assertions passed; the connection-based inference case skipped because no connection was requested.
  The separate live authentication case ran against `COHERE_APIKEY`, including intentionally invalid credentials.
- `RestClient.qtest`: 19 cases, 129 assertions passed.
- `RestClientIo.qtest`: 163 cases, 601 assertions passed.
- `RestClientAsync.qtest`: 10 cases, 45 assertions passed.
- Tests used `LD_LIBRARY_PATH=build` and local qmods/modules via `QORE_MODULE_DIR`; debugging was enabled.
- `rest -G cohere` with the local modules and the existing Qorus module path returned `ping result: "OK"`.
- `qdx --strict-tables qlib/CohereRestClient.qm /tmp/cohere-restclient.dox.h` succeeded.
- The Cohere presentation catalog passed the owner-scoped `qore-data-provider-i18n -C --check` check.
- `git diff --check` passed.

Live inference remains unavailable with this key until its monthly quota resets or another key is supplied.
No quota errors are treated as successful inference calls or suppressed by the connection.
