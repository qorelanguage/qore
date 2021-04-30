# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore DefaultRecordIterator class definition

/** DefaultRecordIterator.qc Copyright 2019 - 2021 Qore Technologies, s.r.o.

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

#! contains all public definitions in the DataProvider module
public namespace DataProvider {
#! Default record iterator class for data providers that do not support custom read / search APIs
/** @note This class supports the \c limit search option
*/
public class DefaultRecordIterator inherits AbstractDataProviderRecordIterator {
    private {
        AbstractIterator i;
        *hash<auto> where_cond;
        *hash<auto> search_options;
        *hash<string, AbstractDataField> record_type;
        int count = 0;
    }

    #! Returns an iterator for zero or more records matching the search options
    /** @param i the iterator; must return a record hash in the getValue() method
        @param where_cond the search criteria
        @param search_options the search options after processing by validateSearchOptions()
    */
    constructor(AbstractIterator i, *hash<auto> where_cond, *hash<auto> search_options, *hash<string, AbstractDataField> record_type) {
        self.i = i;
        self.where_cond = where_cond;
        self.search_options = search_options;
        self.record_type = record_type;
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
            if (!where_cond || (where_cond && matchGeneric(i.getValue(), where_cond))) {
                if (exists search_options.limit && search_options.limit >= 0 && count == search_options.limit) {
                    return False;
                }
                ++count;
                return True;
            }
        }
        return False;
    }

    #! returns @ref True "True" if the iterator is currently pointing at a valid element, @ref False "False" if not
    /** @return @ref True "True" if the iterator is currently pointing at a valid element, @ref False "False" if not
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
        return doMemberGate(key);
    }

    #! Returns the record description, if available
    /** @return the record type of the iterator
    */
    *hash<string, AbstractDataField> getRecordType() {
        return record_type;
    }
}
}
