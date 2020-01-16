# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore AbstractDataProviderRecordIterator class definition

/** AbstractDataProviderRecordIterator.qc Copyright 2019 - 2020 Qore Technologies, s.r.o.

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

#! contains all public definitions in the DataProvider module
public namespace DataProvider {
#! Defines the abstract class for data provider iterators; the destructor releases the iterator
public class AbstractDataProviderRecordIterator inherits AbstractIterator {
    #! returns a single record if the iterator is valid
    /** @throw INVALID-ITERATOR the iterator is not pointing at a valid element
    */
    abstract hash<auto> getValue();

    #! Returns the value of the given field in the current record, if the iterator is valid
    /** @param key the name of the field

        @return the value of the given field in the current record, if the iterator is valid

        @throw FIELD-ERROR invalid or unknown field name
    */
    private auto doMemberGate(string key) {
        hash<auto> record = getValue();
        auto rv = record{key};
        if (!exists rv && !record.hasKey(key)) {
            throw "FIELD-ERROR", sprintf("the current record does not have field %y; valid fields: %y", key,
                keys record);
        }
        return rv;
    }

    #! Checks if the current record matches the search criteria
    /** in case of a hash value, a recursive partial match with only the keys given in the value to be checked is
        performed
    */
    private static bool matchGeneric(hash<auto> record, *hash<auto> where_cond) {
        foreach hash<auto> elem in (where_cond.pairIterator()) {
            if (!AbstractDataProviderRecordIterator::matchGenericValue(record{elem.key}, elem.value)) {
                return False;
            }
        }
        return True;
    }

    #! Match a single value
    /** in case of a hash value, a recursive partial match with only the keys given in the value to be checked is
        performed
    */
    private static bool matchGenericValue(auto expects, auto val) {
        if (val.typeCode() == NT_HASH) {
            # in case of a hash, do a recursive partial match with only the given keys
            foreach hash<auto> i in (val.pairIterator()) {
                if (!AbstractDataProviderRecordIterator::matchGenericValue(expects{i.key}, i.value)) {
                    return False;
                }
            }
            return True;
        }
        return expects == val;
    }
}
}
