# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore MewsRestDataProvider module definition

/** MewsRestOrderItemsTableDataProvider.qc Copyright 2023 Qore Technologies, s.r.o.

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

#! contains all public definitions in the MewsRestDataProvider module
public namespace MewsRestDataProvider {
#! The MewsRestOrderItemsTableDataProvider data provider class
public class MewsRestOrderItemsTableDataProvider inherits MewsRestTableDataProvider {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "type": "MewsRestOrderItemsTableDataProvider",
            "desc": "Mews order items data",
            "has_record": True,
            "supports_read": True,
            "supports_bulk_read": True,
            "supports_search_expressions": True,
            "constructor_options": ConstructorOptions,
            "search_options": SearchOptions,
            "expressions": (AbstractDataProvider::GenericExpressions{
                DP_OP_AND,
                DP_SEARCH_OP_GE,
                DP_SEARCH_OP_LE,
            }),
        };

        #! Fields supporting >= and <= operators
        const ComparisonFields = {
            "CreatedUtc": True,
            "UpdatedUtc": True,
            "ChargedUtc": True,
            "ConsumedUtc": True,
            "CanceledUtc": True,
            "ClosedUtc": True,
        };

        #! Constructor options
        const ConstructorOptions = MewsRestTableDataProvider::ConstructorOptions - "table";
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) : MewsRestTableDataProvider(options + {"table": "orderitems"}) {
    }

    #! Creates the object from the arguments
    constructor(MewsRestClient rest) : MewsRestTableDataProvider(rest, "orderitems"){
    }

    #! Returns an iterator for zero or more records matching the search options
    /** @param where_cond the search criteria
        @param search_options the search options after processing by validateSearchOptions()

        @see requestSearchRecordsImpl()
    */
    private DataProvider::AbstractDataProviderRecordIterator searchRecordsImpl(*hash<auto> where_cond,
            *hash<auto> search_options) {
        AbstractDataProvider prov = sdp.getChildProviderPath("api/connector/v1/orderItems/getAll/POST");
        int limit = min(search_options.limit ?? MaxRecords, MaxRecords);
        hash<auto> req = getSearchRequest(limit, \where_cond, search_options);
        *list<auto> l;
        while (True) {
            hash<auto> res = prov.doRequest({"body": req});
            if (!res.body.OrderItems) {
                break;
            }
            l += res.body.OrderItems;
            if (res.body.OrderItems.size() < limit) {
                break;
            }

            if (search_options.limit) {
                int remaining = search_options.limit - l.size();
                if (!remaining) {
                    break;
                }
                if (remaining < limit) {
                    limit = remaining;
                }
            }
        }

        return new DefaultRecordIterator(new ListIterator(l), where_cond, search_options,
            record_info.record_type);
    }

    private AbstractDataProviderBulkRecordInterface searchRecordsBulkImpl(int block_size = 1000,
            *hash<auto> where_cond, *hash<auto> search_options) {
        AbstractDataProvider prov = sdp.getChildProviderPath("api/connector/v1/orderItems/getAll/POST");
        int limit = min(block_size, search_options.limit ?? MaxRecords, MaxRecords);
        hash<auto> req = getSearchRequest(limit, \where_cond, search_options);
        return new MewsTableBulkRecordInterface(block_size, prov, req, "OrderItems", record_info.record_type,
            where_cond, search_options);
    }

    private hash<auto> getSearchRequest(int limit, reference<*hash<auto>> where_cond, *hash<auto> search_options) {
        hash<auto> req.Limitation.Count = limit;
        if (where_cond) {
            processSearchArgs(\req, where_cond);
        }
        return rest.getMewsRequestBody(req);
    }

    private processSearchArgs(reference<hash<auto>> req, hash<auto> exp) {
        if (!exp.exp || !exp.args) {
            throw "EXPRESSION-ERROR", sprintf("cannot handle expression %y; expecting a data provider expression "
                "with \"exp\" and \"args\" keys", exp);
        }
        switch (exp.exp) {
            case DP_OP_AND:
                map processSearchArgs(\req, $1), exp.args;
                break;

            case DP_SEARCH_OP_GE:
            case DP_SEARCH_OP_LE:
                processComparisonArgs(\req, exp);
                break;

            default:
                throw "EXPRESSION-ERROR", sprintf("unsupported operator %y", exp.exp);
        }
    }

    private processComparisonArgs(reference<hash<auto>> req, hash<auto> exp) {
        *string field = exp.args[0].field;
        if (!field) {
            throw "EXPRESSION-ERROR", sprintf("expecting a field reference as the first argument to the %y "
                "operator; got %y instead", exp.exp, exp.args);
        }
        if (!ComparisonFields{field}) {
            throw "EXPRESSION-ERROR", sprintf("field %y cannot be used with the %y operator; supported fields: %y",
                field, exp.exp, keys ComparisonFields);
        }
        auto val = exp.args[1];
        switch (exp.exp) {
            case DP_SEARCH_OP_GE:
                req{field}.StartUtc = val;
                if (!req{field}.EndUtc) {
                    req{field}.EndUtc = val + 3M;
                }
                break;

            case DP_SEARCH_OP_LE:
                req{field}.EndUtc = val;
                if (!req{field}.StartUtc) {
                    req{field}.StartUtc = val - 3M;
                }
                break;
        }
    }

    #! Returns data provider static info
    private hash<DataProvider::DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }

    #! Retrieves the Swagger data provider for the table data
    private AbstractDataProvider getTableDataProvider() {
        return sdp.getChildProviderPath("api/connector/v1/orderItems/getAll/POST");
    }

    #! Retrieve the record information
    private hash<MewsRestRecordInfo> getRecordInfoInternImpl() {
        # get data provider for the call
        AbstractDataProvider prov = sdp.getChildProviderPath("api/connector/v1/orderItems/getAll/POST");

        return <MewsRestRecordInfo>{
            "record_type": prov.getResponseType()
                .getFields().body.getType()
                .getFields().OrderItems.getType()
                .getElementType().getFields(),
        };
    }
}
}
