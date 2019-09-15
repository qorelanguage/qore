# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore QoreDataType class definition

/** QoreDataType.qc Copyright 2019 Qore Technologies, s.r.o.

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
#! describes a data type
public class QoreDataType inherits AbstractDataProviderType {
    private {
        #! the Qore type
        Type type;

        #! hash of base types where no translations are performed
        hash<string, bool> base_type_hash;
    }

    #! creates the type
    constructor(Type type, *hash<auto> options) : AbstractDataProviderType(options) {
        self.type = type;
        base_type_hash{type.getBaseTypeCode()} = True;
        if (type.isOrNothingType()) {
            base_type_hash{NT_NOTHING} = True;
        }
    }

    #! returns the type name
    string getName() {
        return type.getName();
    }

    #! returns the base type for the type, if any
    *Type getValueType() {
        return type;
    }

    #! returns a hash of native base type code where no translations are performed
    *hash<string, bool> getDirectTypeHash() {
        return base_type_hash;
    }

    #! returns the subtype (for lists or hashes) if there is only one
    *AbstractDataProviderType getElementType() {
        if (*Type elementType = type.getElementType()) {
            return AbstractDataProviderType::get(elementType);
        }
    }

    #! returns the value if the value can be assigned to the type
    /** @param value the value to assign to the type

        @return the value to be assigned; can be converted by the type

        @throw RUNTIME-TYPE-ERROR value cannot be assigned to type
    */
    auto acceptsValue(auto value) {
        return type.acceptsValue(value);
    }

    #! returns the fields of the data structure; if any
    *hash<string, AbstractDataField> getFields() {
        # this method intentionally left blank
    }
}
}
