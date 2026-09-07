# -*- mode: qore; indent-tabs-mode: nil -*-
/** PineconeUsage.qc Copyright 2026 Qore Technologies, s.r.o.

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

/** @page pineconerecords Pinecone Record Tables

    Use the `tables` child of a Pinecone data provider for record-based reads, creates, updates, and
    deletes. Each table also appears in the generic **Search**, **Search Single**, **Create**,
    **Update**, and **Delete** action forms when it supports that operation.

    |!Table|!Identity|!Scope|!Operations
    |indexes|name|project connection|read, create, update, delete
    |collections|name|project connection|read, create, delete (legacy pod indexes)
    |backups|backup_id|project-wide reads; index_name for create|read, create, delete
    |backup-schedules|schedule_id|index_name for list/create; global ID for other operations|read, create, update, delete
    |restore-jobs|restore_job_id|project connection|read only
    |models|model|project connection|read only; type and vector_type filters
    |namespaces|name|index|read, create, delete
    |imports|id|index|read, create, cancel through delete
    |vectors|id|index, optional namespace (default namespace when empty)|read, upsert through create, update, delete
    |documents|_id|index, namespace|read, upsert through create, update, delete

    @section pineconerecordsread Complete Reads

    Unfiltered vector and document scans list IDs, then fetch each page's complete records in a single
    batch. They retain listing order and skip IDs concurrently deleted between list and fetch. A
    continuation token advances the scan even when a short page, or an entirely deleted page, returns
    no records. Filtered scans use the paginated metadata fetch endpoints. Ranked similarity search
    remains available through the API actions; `top_k` results cannot enumerate a table.

    Vector records contain `id`, optional `values`, optional `sparseValues`, and an open `metadata`
    hash. Documents contain `_id` and an open `fields` hash. Write forms come from the operation's
    request schema and exclude response-only fields. Document writes flatten `fields` into Pinecone's
    wire format; an `_id` inside `fields` is rejected to prevent overriding the record identity.

    @code{.py}
AbstractDataProvider pinecone = conn.getDataProvider();
AbstractDataProvider vectors = pinecone.getChildProviderPath("tables/vectors");
hash<auto> scope = {"index": "products", "namespace": "europe"};
foreach hash<auto> record in (vectors.searchRecords({"metadata.category": "books"}, scope)) {
    printf("%s: %y\n", record.id, record.metadata);
}
AbstractDataProvider documents = pinecone.getChildProviderPath("tables/documents");
hash<auto> doc_scope = {"index": "reports", "namespace": "finance"};
documents.createRecord({"_id": "report-2026-q3", "fields": {
    "title": "Quarterly report", "year": 2026,
}}, doc_scope);
# After the write is visible, update the existing document by its ID.
documents.updateRecords({"fields": {"title": "Revised quarterly report"}},
    {"_id": "report-2026-q3"}, doc_scope);
    @endcode

    @section pineconerecordsfilters Metadata Filters

    Predicates on `metadata.category` or `fields.category` use Pinecone's `$eq`, `$ne`, `$gt`, `$gte`,
    `$lt`, `$lte`, `$in`, `$nin`, and `$exists` operators, with `$and` and `$or`. Native comparisons
    follow Pinecone's metadata semantics. `is_null` tests field absence, since metadata does not store
    null values. Inequalities require numeric operands. Membership lists must contain at most 10,000
    strings or numbers; boolean, mixed-type, or larger lists are evaluated locally. Literal dots in field names use the data
    provider's escaped-dot notation. Nested object traversal, logical NOT, and unsupported expressions
    are evaluated locally against full records. The requested result limit is counted after residual
    filtering, so a nonmatching first page does not hide matches on later pages.

    @section pineconerecordswrites Write Semantics and Consistency

    **Pinecone is eventually consistent.** A successful write acknowledges acceptance; an immediately
    following read may still show the preceding state. Record operations do not add sleeps, retries,
    transactions, or a read-after-write guarantee. See
    [Check data freshness](https://docs.pinecone.io/guides/index-data/check-data-freshness).

    Vector/document create returns the submitted record after the service acknowledges the upsert.
    Updates patch supplied values; vector metadata becomes `setMetadata`. Updates and deletes check
    the selected identity first because Pinecone also acknowledges writes to missing IDs. The reported
    count is the number of matched identities whose writes were acknowledged, subject to concurrent
    modification and eventual consistency. These operations cannot provide atomic affected-row counts.

    Collection writes require a predicate. A non-identity predicate is resolved completely before the
    first write; `max_records` can bound the affected set. Deleting a namespace removes its contents.
    Deleting an import cancels the job and does not undo records already imported.

    @section pineconerecordstesting Live Integration Stages

    The offline tests run without credentials. The live test in `PineconeRecordTables.qtest` uses
    `PINECONE_APIKEY`, `PINECONE_TABLE_STATE` (a fixture JSON path), and `PINECONE_TABLE_PHASE`.
    Run `prepare`, `read`, `updated`, `deleted`, and `cleaned` as separate stages after preceding writes are visible.
    Each stage asserts once and never sleeps or polls. `prepare` creates uniquely named test indexes;
    `deleted` verifies record deletions and requests index deletion; `cleaned` verifies index removal. `cleanup` explicitly removes the same owned
    fixtures. Failed stages also attempt cleanup. The fixture file contains no credentials.
*/

/** @page pineconeadmin Pinecone Organization Administration

    The **Pinecone Admin** app provisions projects and manages organization access. It uses a separate
    `pinecone-admin` connection and the pinned Admin API schema. The database/inference app continues
    to use a project API key through `pinecone`.

    |!Resource|!Actions
    |Projects|list, create, get, update, delete
    |Organizations|list, get, update, delete
    |API keys|list for project, create, get, update, delete
    |Service accounts|list, create, get, update, delete, rotate secret
    |Users|list, get, remove
    |Invites|list, create, get, delete, resend
    |Role bindings|list, create, get, delete

    @section pineconeadminconnection Service-Account Connection

    Create a service account in Pinecone and grant the roles its workflows require. Configure its
    **Client ID** and **Client Secret**, both sensitive options. The service account's resource `id`
    is a different value from its OAuth2 `client_id`.

    @code{.py}
PineconeAdminRestConnection conn("pinecone-admin", "Organization provisioning",
    PineconeAdminRestConnection::DefaultUrl, {}, {
        "oauth2_client_id": ENV.PINECONE_CLIENT_ID,
        "oauth2_client_secret": ENV.PINECONE_CLIENT_SECRET,
    });
conn.ping(True);
AbstractDataProvider admin = conn.getDataProvider();
hash<auto> project = admin.getChildProviderEx("create-project").doRequest({"name": "Support search"});
hash<auto> key = admin.getChildProviderEx("create-api-key").doRequest({
    "project_id": project.id, "name": "Search reader", "roles": ("DataPlaneViewer",),
});
# Store key.value securely in the project's Pinecone connection; do not log it.
    @endcode

    Tokens use `client_credentials`, `https://login.pinecone.io/oauth/token`, and the fixed audience
    `https://api.pinecone.io/`. Both REST clients renew an expiring access token by acquiring another
    client-credentials token. The app has no interactive authorization step. Ping authenticates and
    lists accessible projects, including for organizations whose project list is empty.

    @section pineconeadminforms Action Forms and References

    All 32 actions publish typed request and response fields from the vendored `2026-07` schema.
    Vendor `x-enum` values become named choices, including roles inside role bindings. ID fields offer
    live references where the API can enumerate them and accept custom IDs. API-key references first
    list projects, then list their keys. Polymorphic principal IDs identify users, service accounts,
    API keys, or invites according to `principal_type`; use the resource ID returned by the matching
    listing. Paginated reference lists offer a first page of choices and always accept a custom ID;
    use the corresponding list action's cursor options to inspect subsequent pages.

    **Issued secrets are returned only at creation or rotation.** API-key `value` and service-account
    credentials are marked sensitive. Store them securely when returned. Rotation invalidates the
    previous secret; update the connections that use the service account. **Create Invite** and
    **Resend Invite** send email to the specified user.

    @section pineconeadminvalidation Validation

    `PineconeAdminRestClient.qtest` exercises real local HTTP token and resource endpoints, including
    invalid credentials, fixed audience, token expiry and authenticated ping. Optional live project
    creation/deletion in `PineconeAdminDataProvider.qtest` requires `PINECONE_ADMIN_LIVE_TEST=1` and
    either `PINECONE_ADMIN_CONNECTION` or both `PINECONE_CLIENT_ID` and `PINECONE_CLIENT_SECRET`.
    A project API key cannot substitute for Admin credentials.
*/
