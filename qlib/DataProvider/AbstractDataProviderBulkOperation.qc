# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore AbstractDataProviderBulkOperation class definition

/** AbstractDataProviderBulkOperation.qc Copyright 2023 - 2024 Qore Technologies, s.r.o.

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
#! Abstract bulk data operation class
public class AbstractDataProviderBulkOperation {
    private {
        #! The data provider
        AbstractDataProvider provider;
    }

    #! Creates the object
    constructor(AbstractDataProvider provider) {
        self.provider = provider;
    }

    #! Commits data written to the data provider
    /** Has no effect if the data provider does not support transaction management
    */
    commit() {
        provider.commit();
    }

    #! Rolls back data written to the data provider
    /** Has no effect if the data provider does not support transaction management
    */
    rollback() {
        provider.rollback();
    }

    #! Queues data in the buffer
    abstract queueData(hash<auto> record);

    #! Queues data in the buffer
    abstract queueData(list<hash<auto>> records);

    #! Flushes any remaining data to the data provider
    /** This method should always be called before committing the transaction (if the data provider supports
        transaction management) or destroying the object
    */
    abstract flush();

    #! Discards any buffered data
    /** This method should be called before rolling back the transaction (if the data provider supports transaction
        management) or destroying the object if an error occurs
    */
    abstract discard();
}
}
