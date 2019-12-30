# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore SalesforceRestRecordIterator class definition

/** SalesforceRestRecordIterator.qc Copyright 2019 Qore Technologies, s.r.o.

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

#! contains all public definitions in the SalesforceRestDataProvider module
public namespace SalesforceRestDataProvider {
 #! SOQL operator info hash as returned by all @ref soql_op_funcs "operator functions"
public hashdecl SoqlOperatorInfo {
    string op;   #!< the operator string code
    auto arg;    #!< optional argument
}

/** @defgroup soql_ops SQL Operators
    These are the operators that can be used in where clauses
*/
#@{
#! like operator
const SOQL_OP_LIKE = "like";

#! the SOQL in operator for use in where clauses
/** @see soql_op_in()
*/
public const SOQL_OP_IN = "in";

#! the SOQL less than (<) operator for use in where clauses
/** @see soql_op_lt()
*/
public const SOQL_OP_LT = "<";

#! the SOQL less than or equals (<=) operator for use in where clauses
/** @see soql_op_le()
*/
public const SOQL_OP_LE = "<=";

#! the SOQL greater than operator (>) for use in where clauses
/** @see soql_op_gt()
*/
public const SOQL_OP_GT = ">";

#! the SOQL greater than or equals operator (>=) for use in where clauses
/** @see soql_op_ge()
*/
public const SOQL_OP_GE = ">=";

#! the SOQL not equals operator (!= or <>) for use in where clauses
/** @see soql_op_ne()
*/
public const SOQL_OP_NE = "!=";

#! the SOQL equals operator (=) for use in where clauses
/** @see soql_op_eq()
*/
public const SOQL_OP_EQ = "=";

#! the SQL \c "not" operator for use in @ref where_clauses
/** @see soql_op_not()
*/
public const SOQL_OP_NOT = "not";

#! to combine SQL expressions with \c "or" for use in @ref where_clauses
/** @see soql_wsoql_op_or()
*/
public const SOQL_OP_OR = "or";
#@}

/** @defgroup soql_op_funcs SOQL Operator Functions
    These are the operators that can be used in Salesforce where clauses
    - soql_op_in(): for the \c "in" operator
    - soql_op_like(): for the \c "like" operator
    - soql_op_ge(): for the \c ">=" operator when comparing column values to immediate values
    - soql_op_gt(): for the \c ">" operator when comparing column values to immediate values
    - soql_op_le(): for the \c "<=" operator when comparing column values to immediate values
    - soql_op_lt(): for the \c "<" operator when comparing column values to immediate values
    - soql_op_ne(): for the \c "!=" or \c "<>" operator when comparing column values to immediate values
    - soql_op_eq(): for the \c "=" operator when comparing column values to immediate values
    - soql_op_not(): for negating other operators
    - soql_wop_or(): for combining SQL expressions with \c "or"
*/
#@{
#! returns an @ref SoqlOperatorInfo hash
public hash<SoqlOperatorInfo> sub soql_make_op(string op, auto arg) {
    return new hash<SoqlOperatorInfo>({"op": op, "arg": arg});
}

#! returns an @ref SoqlOperatorInfo hash for the \c "like" operator with the given argument for use in salesforce where clauses
/** @par Example:
    @code{.py}
*list<auto> rows = provider.searchRecords({"name": soql_op_like("%smith%")});
    @endcode

    @param str the argument for the operator

    @return an @ref SoqlOperatorInfo hash for use in salesforce where clauses
*/
public hash<SoqlOperatorInfo> sub soql_op_like(string str) {
    return soql_make_op(SOQL_OP_LIKE, str);
}

#! returns an @ref SoqlOperatorInfo hash for the \c "<" operator with the given argument for use in salesforce where clauses when comparing column values to immediate values
/** @par Example:
    @code{.py}
*list<auto> rows = provider.searchRecords({"name": soql_op_lt("Zebra")});
    @endcode

    @param arg the argument for the operator

    @return an @ref SoqlOperatorInfo hash for use in salesforce where clauses
*/
public hash<SoqlOperatorInfo> sub soql_op_lt(auto arg) {
    return soql_make_op(SOQL_OP_LT, arg);
}

#! returns an @ref SoqlOperatorInfo hash for the \c "<=" operator with the given argument for use in salesforce where clauses when comparing column values to immediate values
/** @par Example:
    @code{.py}
*list<auto> rows = provider.searchRecords({"name": soql_op_le("Zebra")});
    @endcode

    @param arg the argument for the operator

    @return an @ref SoqlOperatorInfo hash for use in salesforce where clauses
*/
public hash<SoqlOperatorInfo> sub op_le(auto arg) {
    return soql_make_op(SOQL_OP_LE, arg);
}

#! returns an @ref SoqlOperatorInfo hash for the \c ">" operator with the given argument for use in salesforce where clauses when comparing column values to immediate values
/** @par Example:
    @code{.py}
*list<auto> rows = provider.searchRecords({"name": op_gt("Apple")});
    @endcode

    @param arg the argument for the operator

    @return an @ref SoqlOperatorInfo hash for use in salesforce where clauses

    @see op_cgt() for a function to be used when comparing two column values
*/
public hash<SoqlOperatorInfo> sub op_gt(auto arg) {
    return soql_make_op(SOQL_OP_GT, arg);
}

#! returns an @ref SoqlOperatorInfo hash for the \c ">=" operator with the given argument for use in salesforce where clauses when comparing column values to immediate values
/** @par Example:
    @code{.py}
*list<auto> rows = provider.searchRecords({"name": op_ge("Apple")});
    @endcode

    @param arg the argument for the operator

    @return an @ref SoqlOperatorInfo hash for use in salesforce where clauses

    @see op_cge() for a function to be used when comparing two column values
*/
public hash<SoqlOperatorInfo> sub op_ge(auto arg) {
    return soql_make_op(SOQL_OP_GE, arg);
}

#! returns an @ref SoqlOperatorInfo hash for the \c "!=" or \c "<>" operator with the given argument for use in salesforce where clauses when comparing column values to immediate values
/** @par Example:
    @code{.py}
*list<auto> rows = provider.searchRecords({"name": op_ne("Smith")});
    @endcode

    @param arg the argument for the operator

    @return an @ref SoqlOperatorInfo hash for use in salesforce where clauses

    @see op_cne() for a function to be used when comparing two column values

    This operator also handles Qore::NULL as SQL NULL values expanding them into <tt>IS NOT NULL</tt> clause.
*/
public hash<SoqlOperatorInfo> sub op_ne(auto arg) {
    return soql_make_op(SOQL_OP_NE, arg);
}

#! returns an @ref SoqlOperatorInfo hash for the \c "=" operator with the given argument for use in salesforce where clauses when comparing column values to immediate values
/** @par Example:
    @code{.py}
*list<auto> rows = provider.searchRecords({"name": op_eq("Smith")});
    @endcode

    @param arg the argument for the operator

    @return an @ref SoqlOperatorInfo hash for use in salesforce where clauses

    @see op_ceq() for a function to be used when comparing two column values

    This operator also handles Qore::NULL as SQL NULL values expanding them into <tt>IS NULL</tt> clause.
*/
public hash<SoqlOperatorInfo> sub op_eq(auto arg) {
    return soql_make_op(SOQL_OP_EQ, arg);
}

#! returns an @ref SoqlOperatorInfo hash for the \c "in" operator with all arguments passed to the function; for use in salesforce where clauses
/** @par Example:
    @code{.py}
*list<auto> rows = provider.searchRecords({"name": op_in(200, 300, 500, 9)});
    @endcode

    @return an @ref SoqlOperatorInfo hash for use in salesforce where clauses

    @note The argument list size may be constrained depending on the database server / driver used; passing a large number of arguments to this function may be a sign of an improper application or query design; consider using a join with a temporary table instead of passing a large number of arguments to this function
*/
public hash<SoqlOperatorInfo> sub op_in() {
    return soql_make_op(SOQL_OP_IN, argv);
}

#! returns an @ref SoqlOperatorInfo hash for the \c "in" operator with the given argument list as the first argument; for use in salesforce where clauses
/** @par Example:
    @code{.py}
*list<auto> rows = provider.searchRecords({"name": op_in(idlist)});
    @endcode

    @param args a list of values for the \c "in" operator

    @return an @ref SoqlOperatorInfo hash for use in salesforce where clauses

    @note The argument list size may be constrained depending on the database server / driver used; passing a large number of arguments to this function may be a sign of an improper application or query design; consider using a join with a temporary table instead of passing a large number of arguments to this function
*/
public hash<SoqlOperatorInfo> sub op_in(list<auto> args) {
    return soql_make_op(SOQL_OP_IN, args);
}

#! returns an @ref SoqlOperatorInfo hash for the \c "not" operator; for use in salesforce where clauses
/** @par Example:
    @code{.py}
*list<auto> rows = provider.searchRecords({"name": opt_not(op_in(200, 300, 500, 9))))_;
    @endcode

    @return an @ref SoqlOperatorInfo hash for use in salesforce where clauses
*/
public hash<SoqlOperatorInfo> sub op_not(hash arg) {
    return soql_make_op(SOQL_OP_NOT, arg);
}

#! returns an @ref SoqlOperatorInfo hash with a fake \c "_OR_" column name; the list of arguments to the function is combined such that each hash in the list generates SQL expressions combined with \a "and" logic, and each of those clauses is combined with \a "or" logic; this is for use in salesforce where clauses
/** @par Example:
    @code{.py}
*list<auto> rows = provider.searchRecords({"id": id} + wop_or({"type": "C", "validation_flag": op_ne("Y")}, {"expiration_flag": 'Y'}));
    @endcode
    Generates:
    @verbatim
select * from Account where id = '0012A000022K3zxQAC' and ((type = 'C' and validation flag != 'Y') or (expiration_flag = 'Y'))
    @endverbatim

    @param h1 the first hash of expressions to combine with \c "or" logic
    @param h2 the second hash of expressions to combine with \c "or" logic
    @param ... remaining expressions to combine with \c "or" logic

    @return a hash with an @ref SoqlOperatorInfo hash value keyed by a fake column name (\c "_OR_" with a numeric
    prefix for uniqueness) for use in a where operation description hash for use in salesforce where clauses

    @note a random prefix is used so that multiple uses of the function can be used to generate a single where clause
    hash
*/
public hash<string, hash<SoqlOperatorInfo>> sub wop_or(hash<auto> h1, hash<auto> h2) {
    softlist<auto> l = h1;
    l += h2;
    if (argv)
        l += argv;
    return new hash<string, hash<SoqlOperatorInfo>>((sprintf("%d:_OR_", rand() % 10000000): soql_make_op(SOQL_OP_OR, l)));
}
#@}

#! a hash of valid operators for use in where clauses
public const DefaultOpMap = {
    SOQL_OP_LIKE: (
        "code": string sub (object i, string cn, auto arg) {
            return sprintf("%s like %s", cn, arg);
        },
    ),
    SOQL_OP_LT: (
        "code": string sub (object i, string cn, auto arg) {
            return sprintf("%s < %s", cn, arg);
        },
    ),
    SOQL_OP_LE: (
        "code": string sub (object i, string cn, auto arg) {
            return sprintf("%s <= %s", cn, arg);
        },
    ),
    SOQL_OP_GT: (
        "code": string sub (object i, string cn, auto arg) {
            return sprintf("%s > %s", cn, arg);
        },
    ),
    SOQL_OP_GE: (
        "code": string sub (object i, string cn, auto arg) {
            return sprintf("%s >= %s", cn, arg);
        },
    ),
    SOQL_OP_NE: (
        "code": string sub (object i, string cn, auto arg) {
            return sprintf("%s != %s", cn, arg);
        },
    ),
    SOQL_OP_EQ: (
        "code": string sub (object i, string cn, auto arg) {
            return sprintf("%s = %s", cn, arg);
        },
    ),
    SOQL_OP_IN: (
        "code": string sub (object i, string cn, auto arg) {
            *string ins = (foldl $1 + "," + $2, (map i.getSoqlValue($1), arg));
            return sprintf("%s in (%s)", cn, ins);
        },
    ),
    SOQL_OP_NOT: (
        "recursive": True,
        "code": string sub (object i, string cn, auto arg) {
            return sprintf("not (%s)", cn);
        },
    ),
    SOQL_OP_OR: (
        "code": string sub (object i, string cn, list<auto> arg) {
            return i.getOrClause(arg);
        },
    ),
};
#@}

#! Defines the record iterator class for Table-based iterators
public class SalesforceRestRecordIterator inherits AbstractDataProviderRecordIterator {
    public {
    }

    private:internal {
        #! The REST client object for API calls
        SalesforceRestClient rest;

        #! current object name
        string name;

        #! The record info for this object
        hash<SalesforceRestRecordInfo> record_info;

        #! record iterator
        ListHashIterator i;
    }

    #! creates the iterator
    /** @param table the table to iteratate
        @param search_options search options; assumed to have already been processed for validity before this call
    */
    constructor(SalesforceRestClient rest, string name, hash<SalesforceRestRecordInfo> record_info, *hash<auto> where_cond, *hash<auto> search_options) {
        self.rest = rest;
        self.name = name;
        self.record_info = record_info;

        # make query
        string soql = makeQuery(where_cond, search_options);
        #printf("SOQL: %y\n", soql);
        hash<auto> info;
        #on_error printf("info: %N\n", info);
        i = new ListHashIterator(rest.get("query?q=" + soql, NOTHING, \info).body.records);
    }

    #! Returns @ref True if the iterator is valid
    /**
        @return @ref True if the iterator is valid
    */
    bool valid() {
        return i.valid();
    }

    #! Increments the row pointer when retrieving rows from a select statement; returns @ref True if there is a row to retrieve, @ref False if not
    /**
        @return @ref True if there is a row to retrieve, @ref False if not (no more rows to be retrieved)

        @note Exceptions could be thrown by the DBI driver when the statement is executed; see the relevant DBI driver
        docs for more information
    */
    bool next() {
        return i.next();
    }

    #! Returns a single record if the iterator is valid
    /** @throw INVALID-ITERATOR the iterator is not pointing at a valid element
    */
    hash<auto> getValue() {
        hash<auto> rv =  i.getValue() - "attributes";
        # convert JSON strings to native Qore date/time values
        map rv{$1} = date(rv{$1}), keys record_info.date_time_fields;
        map rv{$1} = date(rv{$1}), keys record_info.date_fields;
        return rv;
    }

    #! Returns the value of the given field in the current row, if the iterator is valid
    /** @param key the name of the field

        @return the value of the given field in the current row, if the iterator is valid

        @throw FIELD-ERROR invalid or unknown field name
    */
    auto memberGate(string key) {
        hash<auto> row = getValue();
        auto rv = row{key};
        if (!exists rv && !row.hasKey(key)) {
            throw "FIELD-ERROR", sprintf("the current record does not have field %y; valid fields: %y", key,
                keys row);
        }
        return rv;
    }

    #! Returns "or" clauses
    string getOrClause(list<auto> arglist) {
        list<auto> l = map $1, (map doWhereExpression($1), arglist), $1;
        return l ? ("(" + (foldl $1 + " or " + $2, l) + ")") : "";
    }

    #! Returns the SQL for a request
    string makeQuery(*hash<auto> where_cond, *hash<auto> search_options) {
        string soql = sprintf("select %s from %s", getColumnNames(search_options.columns), name);
        if (where_cond) {
            soql += " where " + doWhereExpression(where_cond, search_options);
        }
        return soql;
    }

    private string getColumnNames(*list<string> column_names) {
        return foldl $1 + "," + $2, keys record_info.record_type;
    }

    private string doWhereExpression(hash<auto> where_cond, *hash<auto> search_options) {
        list<string> clauses;
        foreach hash<auto> i in (where_cond.pairIterator()) {
            if (i.value.typeCode()) {
                *hash<auto> op = DefaultOpMap{i.value.op};
                if (!op) {
                    throw "WHERE-ERROR", sprintf("operator hash for where cond key %y has unknown operator "
                        "%y: expecting one of: %y (operator hash: %y)", i.key, i.value.op, keys DefaultOpMap,
                        i.value);
                }
                clauses += op.code(self, i.key, getArgValue(i.key, i.value.arg));
            } else {
                clauses += sprintf("%s = '%s'", i.key, getArgValue(i.key, i.value));
            }
        }
        return foldl $1 + " and " + $2, clauses;
    }

    private string getArgValue(string key, auto value) {
        switch (record_info.field_types{key}) {
            case "datetime":
            case "date":
            case "time":
                return doDateTimeValue(key, value);

            case "int":
                return sprintf("%d", value);

            case "double":
                return sprintf("%g", value);

            case "boolean":
                return value ? "true" : "false";

            default:
                break;
        }

        return sprintf("'%s'", value);
    }

    private string doDateTimeValue(string key, auto value) {
        if (value.typeCode() == NT_STRING) {
            if (record_info.time_fields{key} && value =~ /^[0-9]+:/) {
                # add the start of the UNIX epoch to the time to parse it to a date value
                value = "1970-01-01T" + value;
            }
            value = date(value);
        } else if (value.typeCode() != NT_DATE) {
            throw "WHERE-ERROR", sprintf("cannot convert value of type %y to a date for field %y", value.type(), key);
        }

        if (record_info.date_time_fields{key}) {
            return value.format("YYYY-MM-DDTHH:mm:SSZ");
        }

        if (record_info.date_fields{key}) {
            return value.format("YYYY-MM-DD");
        }

        if (record_info.time_fields{key}) {
            return value.format("HH:mm:SS.msZ");
        }

        return sprintf("'%s'", value.format("YYYY-MM-DDTHH:mm:SS.xxZ"));
    }
}
}
