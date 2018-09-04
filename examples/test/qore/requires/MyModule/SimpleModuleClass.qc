# -*- mode: qore; indent-tabs-mode: nil -*-
# Qorus ThreadLocalData class definition

%new-style

public namespace EXAMPLE_C;

/**
 * global thread-local instance of @ref ThreadLocalData. Use one of @ref create_tld
 * or @ref ensure_tld before using this variable.
 */

/**
 * contains the thread-local context information.
 */
public class EXAMPLE_C::SomeClass {

    public string cfunc(string str) {
        return str + "EXAMPLE_C::SomeClass::cfunc\n";
    }
}

/**
 * returns @ref True if @ref tld exists and its members (given in \c names) are not @ref NOTHING.
 * Otherwise returns @ref False.
 *
 * Typical use is like
 *     QDBG_ASSERT(ensure_tld());
 */
string sub EXAMPLE_C::func(string str) {
    return str + "EXAMPLE_C::func\n";
}
