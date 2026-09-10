# Soft-list missing values

Copyright (C) 2026 Qore Technologies, s.r.o.

`SoftListDataType::acceptsValue()` distinguishes an absent collection from an
explicit collection item. When its input is `NOTHING`, a configured collection
default takes precedence. The method validates each default item through the
declared element type. Without a default, an optional soft list returns `NOTHING`
without invoking the item validator. A mandatory soft list retains its existing
single-item conversion and validation for that input.

An explicit list is always processed item by item, including empty lists and
items whose value is `NOTHING` or `NULL`. An explicit scalar becomes one item;
`NULL` therefore passes through the element validator and never selects a
collection default. Item validation errors propagate with type context, and a
failed default conversion leaves the type reusable. Returned lists use separate
copy-on-write storage, so changing a result does not modify the configured default.

Constructors accepting reflection types or provider types, named constructors,
`getOrNothingType()`, `getMandatoryType()` and Serializable reconstruction all
retain the same collection-level optionality contract. Optionality of an item
remains independent of optionality of its collection.

For example:

```qore
SoftListDataType references(StringType, True);
@assert(references.acceptsValue(NOTHING) === NOTHING);
@assert(references.acceptsValue("INV-2026-0042") == ("INV-2026-0042",));
```

The executable `examples/test/qlib/DataProvider/SoftListMissingValues.qtest`
checks the constructor/copy forms, reconstruction, explicit missing and null
items, defaults, empty lists, callback invocation, invalid items and record fields.
It also verifies successful reuse after errors. The implementation adds no shared
mutable state or blocking operation; item conversion keeps the existing Qore
iteration and exception behavior.
