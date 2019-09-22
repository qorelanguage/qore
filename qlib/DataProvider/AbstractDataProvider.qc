# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore AbstractDataProvider class definition

/** AbstractDataProvider.qc Copyright 2019 Qore Technologies, s.r.o.

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

#! contains all public definitions in the DataProvider module
public namespace DataProvider {
#! Data provider search option info
public hashdecl DataProviderOptionInfo {
    #! The option value type
    Type type;

    #! Required flag
    bool required;

    #! The option description
    string desc;
}

#! Data provider info
public hashdecl DataProviderInfo {
    #! Data provider name
    string name;

    #! Does the data provider support searching?
    bool supports_searching;

    #! Does the data provider support writing?
    bool supports_writing;

    #! Does the data provider support fast bulk writing?
    /** if this is @ref False and \a supports_writing is @ref True, the data provider will still support bulk write
        APIs but without any performance improvements for bulk data
    */
    bool supports_bulk_writing;

    #! Search options supported by the data provider
    *hash<string, hash<DataProviderOptionInfo>> search_options;

    #! Does the data provide require transaction management?
    bool transaction_management;
}

#! The AbstractDataProvider class
public class AbstractDataProvider {
    #! Returns a single record matching the search options
    *hash<auto> searchSingleRecord(*hash<auto> search_options) {
        search_options = validateSearchOptions(search_options);
        return searchSingleRecordImpl(search_options);
    }

    #! Returns an iterator for zero or more records matching the search options
    AbstractIterator searchRecords(*hash<auto> search_options) {
        search_options = validateSearchOptions(search_options);
        return searchRecordsImpl(search_options);
    }

    #! Returns info about the data provider
    hash<DataProviderInfo> getInfo() {
        *hash<string, hash<DataProviderOptionInfo>> search_opts = getSearchOptions();
        return <DataProviderInfo>{
            "name": getName(),
            "supports_searching": exists search_opts,
            "supports_writing": supportsWrite(),
            "supports_bulk_writing": supportsBulkWrite(),
            "search_options": search_opts,
            "transaction_management": requiresTransactionManagement(),
        };
    }

    #! Returns @ref True if the data provider supports transaction management
    /** @return @ref True if the data provider supports transaction management, in which case commit() or rollback()
        must be called to flush or discard data written to the data provider
    */
    bool requiresTransactionManagement() {
        return False;
    }

    #! flushes any remaining queued data to the data provider; has no effect if the data provider does not support bulk writes
    *hash<auto> flush() {
    }

    #! discards any remaining queued data; has no effect if the data provider does not support bulk writes
    discard() {
    }

    #! Commits data written to the data provider
    /** Has no effect if the data provider does not support transaction management
    */
    commit() {
    }

    #! Rolls back data written to the data provider
    /** Has no effect if the data provider does not support transaction management
    */
    rollback() {
    }

    #! Returns True if the data provider supports writing output
    bool supportsBulkWrite() {
        return False;
    }

    #! Queues the given record for writing
    /** @param rec a hash representing a single input record

        @return if data was inserted (flushed) then a hash (columns) of lists (row data) of all data written
        (including generated /default data, if applicable); if data was only queued and not written, @ref nothing is
        returned.

        @note if bulk writing is not supported then this method is identical to writeRecord()
    */
    *hash<auto> queueWriteRecord(hash<auto> rec) {
        return writeRecord(rec);
    }

    #! validates search options
    /** @throw SEARCH-OPTION-ERROR invalid or unsupported search option
    */
    private *hash<auto> validateSearchOptions(*hash<auto> search_options) {
        return checkOptions("SEARCH-OPTION-ERROR", getSearchOptions(), search_options);
    }

    #! processes options passed to the constructor
    private processConstructorOptions(*hash<string, hash<DataProviderOptionInfo>> option_desc, *hash<auto> options) {
        self += checkOptions("CONSTRUCTOR-OPTION-ERROR", option_desc, options);
    }

    #! verifies options according to the option
    private *hash<auto> checkOptions(string err, *hash<string, hash<DataProviderOptionInfo>> option_desc, *hash<auto> options) {
        if (!option_desc && options) {
            error(err, "invalid options: %y; no options are supported",
                keys options);
        }
        if (*list<string> invalid_keys = keys (options - (keys option_desc))) {
            error(err, "invalid options: %y; supported options: %y", invalid_keys, keys option_desc);
        }
        # check types and required keys
        foreach hash<auto> i in (option_desc.pairIterator()) {
            bool present = options.hasKey(i.key);
            if (i.value.required && !present) {
                error(err, "missing required search option %y", i.key);
            }
            if (!present) {
                continue;
            }
            # process value
            try {
                options{i.key} = i.value.type.acceptsValue(options{i.key});
            } catch (hash<ExceptionInfo> ex) {
                if (ex.err == "RUNTIME-TYPE-ERROR") {
                    error(err, "option %y of type %y cannot be assigned a value of type %y: %s: %s", i.key,
                        i.value.type.getName(), options{i.key}.type(), ex.err, ex.desc);
                }
                rethrow;
            }
        }
        return options;
    }

    #! thrown an exception
    private error(string err, string fmt) {
        throw err, sprintf("data provider %y: %s", self.className(), vsprintf(fmt, argv));
    }

    #! Returns the type name
    abstract string getName();

    #! Returns the description of the record type, if any
    abstract *hash<string, AbstractDataField> getRecordType();

    #! Returns options that can be used for searching
    /** @return a hash of options that can be used for searching; keys are search option names, values describe the
        search option; if @ref nothing is returned, then the provider does not support searching
    */
    abstract *hash<string, hash<DataProviderOptionInfo>> getSearchOptions();

    #! Returns child providers; return @ref nothing if there are no child providers
    abstract *AbstractDataProvider getChildProviders();

    #! Returns True if the data provider supports writing output
    abstract bool supportsWrite();

    #! Writes the given record to the data provider
    /** @param rec a hash representing a single input record

        @return the data written to the data provider
    */
    abstract *hash<auto> writeRecord(hash<auto> rec);

    #! Returns a single record matching the search options
    /** @param search_options the search options after processing by validateSearchOptions()
    */
    private abstract *hash<auto> searchSingleRecordImpl(*hash<auto> search_options);

    #! Returns an iterator for zero or more records matching the search options
    /** @param search_options the search options after processing by validateSearchOptions()
    */
    private abstract AbstractIterator searchRecordsImpl(*hash<auto> search_options);
}
}
