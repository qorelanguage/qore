# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore HashDataType class definition

/** HashDataType.qc Copyright 2019 Qore Technologies, s.r.o.

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
#! describes a data type based on a hash
public class HashDataType inherits QoreDataType {
    private {
        #! the name of the type
        string name;

        #! fields
        hash<string, AbstractDataField> fields;
    }

    #! creates the object and assigns the name as the type
    constructor(string name = AutoHashType.getName(), *hash<auto> options) : QoreDataType(AutoHashType, options) {
        self.name = name;
    }

    #! creates the object and assigns the name as the given name or the base type's name
    constructor(Type base_type, *string name, *hash<auto> options) : QoreDataType(base_type, options) {
        self.name = name ?? base_type.getName();
    }

    #! returns the type name
    string getName() {
        return name;
    }

    #! adds a field to the type
    addField(AbstractDataField field) {
        fields{field.getName()} = field;
    }

    #! returns the given field, if present, or @ref nothing if not
    *AbstractDataField getField(string name) {
        return fields{name};
    }

    #! returns the fields of the data structure; if any
    *hash<string, AbstractDataField> getFields() {
        return fields;
    }

    #! returns a "soft" type equivalent to the current type
    /** @return a "soft" type equivalent to the current type
    */
    AbstractDataProviderType getSoftType() {
        if (soft_type) {
            return self;
        }
        HashDataType rv(name);
        map rv.addField($1.getSoftType()), getFields().iterator();
        rv.soft_type = True;
        return rv;
    }
}
}
