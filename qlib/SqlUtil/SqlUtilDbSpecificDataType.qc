# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore SqlUtilDbSpecificDataType class definition

/** SqlUtilDbSpecificDataType.qc Copyright 2023 Qore Technologies, s.r.o.

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

#! contains all public definitions in the SqlUtil module
public namespace SqlUtil {
#! data type for DB-specific types that are not convertible to other types
/** by default any DB-specific type without a known mapping to a Qore type and without a specific type
    implementation will be assigned this type

    @since SqlUtil 1.6
*/
public class SqlUtilDbSpecificDataType inherits DataProvider::AbstractDataProviderType {
    private {
        #! the type name
        string name;

        #! nullable flag
        bool nullable;
    }

    #! creates the object from the given parameters
    constructor(string native_type, bool nullable, *hash<auto> options)
            : DataProvider::AbstractDataProviderType(options + {"qore.no_null": True}) {
        name = native_type;
        self.nullable = nullable;
    }

    #! returns the type name
    string getName() {
        return name;
    }

    #! Returns the description
    string getDesc() {
        return sprintf("SqlUtil type %y (%sNULL)", name, nullable ? "" : "NOT ");
    }

    #! returns the base type for the type, if any
    *Type getValueType() {
        # this method intentionally left empty
    }

    #! returns the subtype (for lists or hashes) if there is only one
    *AbstractDataProviderType getElementType() {
        # this method intentionally left empty
    }

    #! returns the fields of the data structure; if any
    *hash<string, AbstractDataField> getFields() {
        # this method intentionally left empty
    }

    #! returns the value if the value can be assigned to the type
    /** @param value the value to assign to the type

        @return the value to be assigned; can be converted by the type
    */
    auto acceptsValue(auto value) {
        return value;
    }

    #! returns a hash of types accepted by this type
    hash<string, bool> getAcceptTypeHash(*bool simple) {
        return {"all": True};
    }

    #! returns a hash of types returned by this type
    hash<string, bool> getReturnTypeHash(*bool simple) {
        return {"all": True};
    }
}
}
