#!/usr/bin/env qore

%requires ../../../../qlib/TextWrap.qm
%requires ../../../../qlib/QUnit.qm

%new-style
%require-types
%enable-all-warnings

#! Parent class with utility methods for TextWrap tests.
class BaseTest inherits QUnit::Test {
    constructor (string name, string version) : Test (name, version) {
    }

    constructor () : Test ("BaseTest", "1.0") {
        set_return_value (main());
    }

    string show (list text) {
        list result = ();
        for (int i=0; i < text.size(); i++)
            result += sprintf ("  %d: %n", i, text[i]);
        return result ? result.join("\n") : "  no lines";
    }

    string show (string text) {
        return sprintf ("  %n\n", text);
    }

    check (any result, any expect) {
        assertEq (expect, result,
                sprintf ("expected:\n%s\nbut got:\n%s", self.show(expect), self.show(result))
                );
    }
}
