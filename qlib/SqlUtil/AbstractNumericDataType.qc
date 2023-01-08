# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore AbstractNumericDataType class definition

/** AbstractNumericDataType.qc Copyright 2023 Qore Technologies, s.r.o.

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

#! contains all public definitions in the SqlUtil module
public namespace SqlUtil {
#! the data type for Oracle NUMBER columns
/**
    @since SqlUtil 1.6
*/
public class AbstractNumericDataType inherits DataProvider::QoreNumberDataTypeBase {
    private {
        #! the type name
        string name;
    }

    #! creates the object
    constructor(string native_type, bool nullable, *hash<auto> options)
        : QoreNumberDataTypeBase(nullable ? SoftNumberOrNothingType : SoftNumberType,
            options + {"qore.no_null": True}) {
        name = native_type;
        # make sure compatible values are otherwise passed straight through without conversions; we let the DB do
        # conversions for performance reasons
        base_type_hash += {
            NT_INT: True,
            NT_FLOAT: True,
            NT_STRING: True,
        };
        if (or_nothing_type) {
            base_type_hash{NT_NULL} = True;
        }
    }

    #! returns the type name
    string getName() {
        return name;
    }

    #! returns the value if the value can be assigned to the type
    /** @param value the value to assign to the type

        @return the value to be assigned; can be converted by the type
    */
    auto acceptsValue(auto value) {
        if (base_type_hash{value.typeCode()}) {
            return value;
        }
        return QoreNumberDataTypeBase::acceptsValue(value);
    }
}
}
