# Stable provider-index content and module dependencies

Copyright 2026 Qore Technologies, s.r.o.

## Problem and authority

A provider index is a derived description of installed provider capabilities.
Its dependencies identify modules, not the source or AOT artifact selected by
the loader. `SftpPoller.qm` and `SftpPoller.qmod` implement the same module; a
packaging change alone must not change the dependency. A generated date example
must not change the description merely because the clock advanced.

The observed SFTP app and seven action contracts are identical. Normal loading
prefers the compiled artifact. An old index can explicitly select source, but
this does not justify changing loader precedence or introducing a new global
discovery-isolation mode.

## Design

1. Keep `DataProviderActionCatalog.getModulePathsForApp()` unchanged as physical
   registration provenance. During index generation, resolve these exact paths
   against one snapshot of the runtime module registry, taken after deferred
   registration completes. Persist the registry's exact module names in
   `factorymap`, `appmap`, and `schememap`. Do not derive names from filenames,
   presentation strings, suffix removal, or locale. Unknown explicit paths fail
   before publication rather than inventing ownership.
2. Seeded routing is supported. Already-logical names, including optional known
   factories not installed on this host, remain names. Exact loaded artifact
   paths are migrated through the registry. Unresolved explicit paths are rejected.
   Qorus will seed its explicitly loaded providers by their declared module names.
3. Existing index readers already accept names through `load_module()`. Preserve
   legacy path loading for compatibility; rebuilding migrates those references.
   Loaded-module filtering recognizes exact registered names and physical paths,
   without the old `.qm`-only basename heuristic.
4. Default date examples use `2000-01-01T00:00:00Z`, just as default integer and
   string examples already use fixed samples. This applies recursively through
   the existing example traversal, without altering its cycle/memoization rules.
   Explicit examples, defaults, allowed values, and provider overrides retain
   their existing precedence and exact values. Examples are illustrative, not
   current-time defaults for executing an action.
5. Preserve native serialization and atomic file publication. No timestamps,
   examples, schemas, or dependency identities are omitted by downstream equality
   checks. A genuinely different named dependency or authored value remains drift.

## Implementation and acceptance sequence

- Qore: native dependency generation and loaded filtering; stable fallback date;
  module release notes and regressions for native names, legacy paths, unresolved
  ownership, repeated builds, failure-before-publication, nested dates and authored
  values. Rebuild the affected AOT module targets before testing.
- Exercise source and compiled provider forms in independent processes. Verify
  name-based loading prefers the compiled form but both generate identical index
  dependencies and semantic metadata. Verify the normal lazy reader loads named
  dependencies and still accepts old explicit-path indexes.
- Audit all changed files with `audit-changes`, commit to primary `develop`, and
  push Qore with CI enabled. Monitor the exact pushed commit's pipeline to green;
  any corrective commit receives its own audit.
- Qorus: use named seeds, require the new native builder, retain strict comparison,
  and test named dependency changes plus authored date/example changes. Rebuild,
  install, restart, reconcile the live index, and verify independent discovery
  twice. No Qorus test changes module search paths.
- Audit Qorus, fetch/merge `origin/develop` after committing, and push with
  `git push -o ci.skip`. Preserve unrelated files in both primary repositories.

Reference population follows successful native agreement: compare each producer's
actual inputs, stage only affected collections, verify content/retrieval and swap
aliases atomically with retained rollback generations. This work does not claim
the separate architecture/docs provenance, mandatory grounding, or full Vertex
hierarchical scenario acceptance is complete.
