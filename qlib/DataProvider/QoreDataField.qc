# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore QoreDataField class definition

/** QoreDataField.qc Copyright 2019 Qore Technologies, s.r.o.

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

# minimum required Qore version
%requires qore >= 0.9.4
# assume local scope for variables, do not use "$" signs
%new-style
# require type definitions everywhere
%require-types
#! strict argument handling
%strict-args
# enable all warnings
%enable-all-warnings

%requires reflection

#! contains all public definitions in the DataProvider module
public namespace DataProvider {
#! describes a data type based on a hashdecl
public class QoreDataField inherits AbstractDataField {
    private {
        #! name of the field
        string name;

        #! field type
        AbstractDataProviderType type;

        #! the default value for the field, if any
        auto def_value;
    }

    #! creates the field from the arguments
    constructor(string name, Type type, auto def_value) {
        self.name = name;
        self.type = AbstractDataProviderType::get(type),
        self.def_value = def_value;
    }

    #! returns the name of the field
    string getName() {
        return name;
    }

    #! returns the type of the field
    AbstractDataProviderType getType() {
        return type;
    }

    #! get default value, if any
    auto getDefaultValue() {
        return def_value;
    }
}
}
