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


Failed object graphs are invalidated before index references are released.
`ObjectIndexMap` first calls `obliterateMembers()` for every indexed object when
an exception is pending, while retaining all index-owned references. This marks
each incomplete object deleted, detaches member/native storage under the object
lock and releases that storage after unlocking. It neither consumes the index's
reference nor invokes the object's user destructor. A second pass discards the
index references. Self-references, mutual references and references through
indexed hashes/lists therefore cannot keep rejected objects alive.

The retained index ownership also prevents a peer from disappearing while the
first pass is visiting the graph. A custom hook that has exposed a rejected
object receives a deleted object; later method calls report
`OBJECT-ALREADY-DELETED`. Successful deserialization retains its existing
identity and lifecycle behavior. The original exception remains available.
Deserialization checks pending interruption on entry and after native or custom
member hooks return. Cancellation follows the same failed-graph cleanup path.
Cleanup runs to completion when cancellation is pending, because abandoning its
remaining owners would leak the graph. Ordinary constructor-failure cleanup
shares the member invalidation primitive and preserves its reference semantics.

For example, a custom `deserializeMembers()` hook can restore `next` to the
object itself and then reject another member. Previously dropping the index's
one reference left that rejected cycle alive. The cleanup now detaches `next`
before releasing ownership. `FailedObjectGraphs.qtest` covers this case, mutual
and container links, escaped rejected objects, native members, inheritance,
automatic member rejection, cancellation and successful identity preservation.
