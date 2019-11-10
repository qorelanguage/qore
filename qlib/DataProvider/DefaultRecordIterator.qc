# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore DefaultRecordIterator class definition

/** DefaultRecordIterator.qc Copyright 2019 Qore Technologies, s.r.o.

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
#! Default record iterator class for data providers that do not support custom read / search APIs
public class DefaultRecordIterator inherits AbstractDataProviderRecordIterator {
    private {
        AbstractIterator i;
        *hash<auto> where_cond;
        *hash<auto> search_options;
    }

    #! Returns an iterator for zero or more records matching the search options
    /** @param i the iterator; must return a record hash in the getValue() method
        @param where_cond the search criteria
        @param search_options the search options after processing by validateSearchOptions()
    */
    constructor(AbstractIterator i, *hash<auto> where_cond, *hash<auto> search_options) {
        self.i = i;
        self.where_cond = where_cond;
        self.search_options = search_options;
    }

    #! Moves the current position to the next element; returns @ref False if there are no more elements
    /** This method will return @ref True again after it returns @ref False once if the object being iterated is not
        empty, otherwise it will always return @ref False.  The iterator object should not be used after this method
        returns @ref False

        @return @ref False if there are no more elements (in which case the iterator object is invalid and should not
        be used); @ref True if successful (meaning that the iterator object is valid)
    */
    bool next() {
        while (i.next()) {
            if (match()) {
                return True;
            }
        }
        return False;
    }

    #! returns @ref Qore::True "True" if the iterator is currently pointing at a valid element, @ref Qore::False "False" if not
    /** @return @ref Qore::True "True" if the iterator is currently pointing at a valid element,
        @ref Qore::False "False" if not
    */
    bool valid() {
        return i.valid();
    }

    #! returns a single record if the iterator is valid
    /** @throw INVALID-ITERATOR the iterator is not pointing at a valid element
    */
    hash<auto> getValue() {
        return i.getValue();
    }

    #! Returns the value of the given field in the current record, if the iterator is valid
    /** @param key the name of the field

        @return the value of the given field in the current record, if the iterator is valid

        @throw FIELD-ERROR invalid or unknown field name
    */
    auto memberGate(string key) {
        hash<auto> record = i.getValue();
        auto rv = record{key};
        if (!exists rv && !record.hasKey(key)) {
            throw "FIELD-ERROR", sprintf("the current record does not have field %y; valid fields: %y", key,
                keys record);
        }
        return rv;
    }

    #! Checks if the current record matches the search criteria
    /**
    */
    private bool match() {
        hash<auto> record = i.getValue();
        foreach hash<auto> elem in (where_cond.pairIterator()) {
            if (record{elem.key} != elem.value) {
                return False;
            }
        }
        return True;
    }
}
}