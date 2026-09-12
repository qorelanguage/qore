# JIT storage for recursive closure-use locals

Copyright (C) 2026 Qore Technologies, s.r.o.

A local whose address is taken, or which is captured by a closure, uses a
`ClosureVarValue` on the thread's closure-variable stack. Each recursive call
owns a distinct binding for its own locals. A closure that captures an outer
local continues to access that lexical binding.

The native frame wrapper deliberately skips closure-use body locals because
their source scopes determine when bindings are created and released. LLVM
lowering therefore instantiates a function's own local before `LoadClosure` or
`StoreClosure`, in both JIT and AOT code. Ownership comes from body-local and
explicit block-scope metadata; signature parameters and captured outer locals
are excluded. The existing instantiation helper is idempotent within the
current frame. Scope-exit instructions retain responsibility for releasing the
binding, including exception paths and repeated loop scopes.

Previously only AOT emitted this instantiation at closure loads and stores.
JIT assignment fell through to the generic local lookup, which could find the
same `LocalVar` in a caller's frame. A recursive tree walker then returned the
inner node twice instead of preserving the outer node. The downstream XML
schema parser exhibited this as a duplicate type after processing an include.

For example, recursion between taking a reference and reading the local must
preserve `node.name`:

```qore
%modern
class Walker {
    public {
        list<string> result();
    }
    private static clean(reference<*hash<auto>> node) {
        remove node.unused;
    }
    walk(*hash<auto> data) {
        foreach *hash<auto> node in (data.nodes) {
            clean(\node);
            if (node.child) {
                walk(node.child);
            }
            push result, node.name;
        }
    }
}
Walker walker();
walker.walk({"nodes": {"name": "outer", "child": {"nodes": {"name": "inner"}}}});
@assert(walker.result == ("inner", "outer"));
```

`examples/test/ir/RecursiveClosureLocals.qtest` checks this invariant, empty
input, siblings, recursive captures, captured parameters, escaped references,
uninitialized captured locals, exception unwinding and reuse. It runs all four
execution modes and a source-stripped AOT executable. Its C++ driver completes
the background compiler's queue at fixed call-graph stages before repeating
the assertions, avoiding timing-based warmup. The global completion barrier
finishes submitted work; the per-Program teardown overload cancels queued work
and is unsuitable for proving that a test reaches native code.

The pre-instantiated-local metadata excludes closure-use body locals in both
fresh lowering and restored IR. Signature captures remain pre-instantiated.
This matches the frame wrapper's actual allocation policy. Native entry code
must not load a captured body local before its lexical scope begins: lookup
can find the caller's binding or instantiate an inner binding before an outer
one, reversing the closure-stack order used by scope cleanup.

`examples/test/qore/closures/recursive-captured-scopes.qtest` exercises nested
captures behind a skipped inner scope, recursive calls, escaped closures,
exception unwinding, and early returns. It runs AST, IR, synchronous JIT,
tiered execution, and an AOT module. Each run checks 6,000 captured values.
