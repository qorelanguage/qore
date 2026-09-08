# Indexed container deserialization ownership

Copyright (C) 2026 Qore Technologies, s.r.o.

`QoreSerializable::deserialize()` allocates indexed objects, hashes and lists before
initializing their contents. The index owns one reference to each target while
references between serialized values are resolved. Initialization fills those same
containers so shared references continue to identify the same objects.

An in-place target passed to `deserializeHashData()`, `deserializeListData()` or
`typed_hash_decl_private::newHash()` is borrowed. Each helper acquires a separate
reference before placing it in a `ReferenceHolder`. Successful helpers return an
owned reference, which the indexed caller consumes with `ValueHolder`. On failure,
the helper releases only its own reference; the index retains its target until
context cleanup. Early type and member checks do not consume the borrowed target.

This ownership rule also applies to empty lists. Calls without an in-place target
continue to allocate and return a newly owned container. The change does not copy
indexed targets or alter reference identity, accepted values or error categories.

For example, corrupting a serialized hashdecl's boolean member to a string raises
`RUNTIME-TYPE-ERROR`. A missing indexed reference in a hash or list raises
`DESERIALIZATION-ERROR`. Both failures release partially initialized contents without
invalidating the index's reference, and a subsequent valid deserialization succeeds.

`examples/test/qore/classes/Serializable/IndexedContainerErrors.qtest` covers typed
hash member errors, early unknown-member rejection, partial generic hash/list
initialization, empty lists, recovery and shared object identity. The existing
`Serializable.qtest` covers the broader serialization interfaces and reference graph.
