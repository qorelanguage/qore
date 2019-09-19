# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore QoreStringTypeBase class definition

/** QoreStringTypeBase.qc Copyright 2019 Qore Technologies, s.r.o.

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
#! describes a data type based on softstring with a target encoding
public class QoreStringTypeBase inherits QoreDataType {
    private {
        #! supported options
        const SupportedOptions = {
            "string.encoding": <DataProviderTypeOptionInfo>{
                "type": Type::String,
                "desc": "the output encoding when writing to the type",
            },
            # the max_size_chars option is to be enforced externally
            "string.max_size_chars": <DataProviderTypeOptionInfo>{
                "type": Type::Int,
                "desc": "the maximum length of the string in characters",
            },
            # only used with "or nothing" types
            "string.empty_to_nothing": <DataProviderTypeOptionInfo>{
                "type": Type::Boolean,
                "desc": "if an empty string should be converted to no value",
            },
        };
    }

    #! creates the object with the given target encoding
    /** @throw STRING-TYPE-ERROR invalid type; must be a string type
    */
    constructor(Type type, *hash<auto> options) : QoreDataType(type, options) {
        if (type.getBaseTypeCode() != NT_STRING) {
            throw "STRING-TYPE-ERROR", sprintf("cannot create a %s object from type %y", self.className(),
                type.getName());
        }

        # translations are done on string types if there are options that require string processing
        if (options."string.encoding" || options."string.empty_to_nothing") {
            remove base_type_hash{NT_STRING};
        }
    }

    #! returns the value if the value can be assigned to the type
    /** @param value the value to assign to the type

        @return the value to be assigned; can be converted by the type

        @throw RUNTIME-TYPE-ERROR value cannot be assigned to type
    */
    auto acceptsValue(auto value) {
        if (exists value && value !== NULL) {
            if ((value.typeCode() != NT_STRING)) {
                if (!value.strp()) {
                    throw "RUNTIME-TYPE-ERROR", sprintf("value of type %y cannot be converted to type %y",
                        value.type(), getName());
                }
                value = string(value, options."string.encoding");
            } else if (options."string.encoding" && (value.encoding() != options."string.encoding")) {
                value = string(value, options."string.encoding");
            }
            if (options."string.empty_to_nothing" && !value.val() && return_type_hash."nothing") {
                return;
            }
            return value;
        }
        return type.acceptsValue(value);
    }

    #! returns supported options
    *hash<string, hash<DataProviderTypeOptionInfo>> getSupportedOptions() {
        return SupportedOptions;
    }

    #! sets the given option without any validation of the option
    private setOptionInternal(string opt, auto value) {
        if (opt == "string.encoding") {
            # make sure the encoding is recognized and is the internal target encoding
            options."string.encoding" = string("", options."string.encoding").encoding();
        } else {
            options{opt} = value;
        }
    }
}
}
