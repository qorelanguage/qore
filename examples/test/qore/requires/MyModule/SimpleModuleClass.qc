# -*- mode: qore; indent-tabs-mode: nil -*-
# Qorus SimpleModuleClass class definition

%new-style
%enable-all-warnings
%require-types
%strict-args

public namespace EXAMPLE_C;

public class EXAMPLE_C::SomeClass {

    public string cfunc(string str) {
        return str + "EXAMPLE_C::SomeClass::cfunc\n";
    }
}

public string sub EXAMPLE_C::func(string str) {
    return str + "EXAMPLE_C::func\n";
}

