# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore SalesforceRestDataProviderDefs definitions

/** SalesforceRestDataProviderDefs.qc Copyright 2019 - 2020 Qore Technologies, s.r.o.

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

#! column operator info hash as returned by all @ref soql_soql_cop_funcs "column operator functions"
public hashdecl SoqlColumnOperatorInfo {
    string cop;   #!< the column operator string code
    auto column;  #!< column sopecifier, may be a string or a complex hash
    auto arg;     #!< optional argument
}

/** @defgroup soql_cops SOQL Column Operators
    These are the operators that can be used in the \c "columns" argument for select statements
*/
#@{
#! to return the minimum value
/** @see soql_cop_min
*/
public const SOQL_COP_MIN = "min";

#! to return the maximum value
/** @see soql_cop_max
*/
public const SOQL_COP_MAX = "max";

#! to return the average value
/** @see soql_cop_avg
*/
public const SOQL_COP_AVG = "avg";

#! to return the sum value
/** @see soql_cop_sum
*/
public const SOQL_COP_SUM = "sum";

#! to return the row count
/** @see soql_cop_count
*/
public const SOQL_COP_COUNT = "count";

#! to return a count of distinct values
/** @see soql_cop_count_distinct
*/
public const SOQL_COP_COUNT_DISTINCT = "count_distinct";

#! to return the given column value in the default locale for number, date, time, and currency fields
/** @see soql_cop_format
*/
public const SOQL_COP_FORMAT = "format";

#! a hash of default column operator descriptions
public const DefaultSoqlCopMap = {
    SOQL_COP_MIN: (
        "code": string sub (string cve, auto arg) {
            return sprintf("min(%s)", cve);
        },
        "group": True,
    ),
    SOQL_COP_MAX: (
        "code": string sub (string cve, auto arg) {
            return sprintf("max(%s)", cve);
        },
        "group": True,
    ),
    SOQL_COP_AVG: (
        "code": string sub (string cve, auto arg) {
            return sprintf("avg(%s)", cve);
        },
        "group": True,
    ),
    SOQL_COP_SUM: (
        "code": string sub (string cve, auto arg) {
            return sprintf("sum(%s)", cve);
        },
    ),
    SOQL_COP_COUNT: (
        "nocolumn": True,
        "code": string sub (*string cve, auto arg) {
            return sprintf("count(%s)", cve);
        },
    ),
    SOQL_COP_COUNT_DISTINCT: (
        "code": string sub (string cve, auto arg) {
            return sprintf("count_distinct %s", cve);
        },
    ),
    SOQL_COP_FORMAT: (
        "dostring": True,
        "code": string sub (string cve, auto arg) {
            return sprintf("format(%s)", cve);
        },
    ),
};
#@}

/** @defgroup soql_ops SOQL Operators
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

#! the SOQL \c "not" operator for use in @ref where_clauses
/** @see soql_op_not()
*/
public const SOQL_OP_NOT = "not";

#! to combine SOQL expressions with \c "or" for use in @ref where_clauses
/** @see soql_wsoql_op_or()
*/
public const SOQL_OP_OR = "or";
#@}

/** @defgroup soql_soql_cop_funcs SOQL Column Operator Functions
    These are functions that can be used in the \c "columns" argument for select statements:
    - @ref soql_cop_avg(): return the averge value for the given column when grouping
    - @ref soql_cop_max(): return the maximum value for the given column when grouping
    - @ref soql_cop_min(): return the minimum value for the given column when grouping
    - @ref soql_cop_sum(): return the sum of all values for the given column when grouping
    - @ref soql_cop_count(): use the \c COUNT() function in the query
    - @ref soql_cop_count_distinct(): use the \c COUNT_DISTINCT() function in the query
    - @ref soql_cop_format(): use the FORMAT() on the given column
    */
#@{
#! returns a @ref SoqlColumnOperatorInfo hash
/** @param cop the column operator (one of @ref soql_cops)
    @param column the column name
    @param arg the argument to the operator

    @return a @ref SoqlColumnOperatorInfo hash corresponding to the arguments for use in the
    @ref select_option_columns "columns" argument of a @ref select_option_hash "select option hash"

    @note Normally this function is not called directly, but rather by the other column operator functions
*/
public hash<SoqlColumnOperatorInfo> sub soql_make_cop(string cop, auto column, auto arg) {
    switch (column.typeCode()) {
        case NT_HASH:
        case NT_STRING: return new hash<SoqlColumnOperatorInfo>({"cop": cop, "column": column, "arg": arg});
        default: throw "MAKE-COLUMN-OPERATOR-ERROR", sprintf("invalid column value passed to soql_make_cop(); got type "
            "%y, expecting \"string\" or \"hash\"", column.type());
    }
}

#! returns a @ref SoqlColumnOperatorInfo hash for the \c "min" operator; returns minimum column values
/** @par Example:
    @code{.py}
AbstractDataProviderRecordIterator i = provider.searchRecords(("columns": (soql_cop_min("Id")), "where": {"Type": "user"}));
    @endcode

    @param column the column specification for the column

    @return a @ref SoqlColumnOperatorInfo hash corresponding to the arguments for use in the \c "columns" argument of
    a @ref select_option_hash "select option hash"
*/
public hash<SoqlColumnOperatorInfo> sub soql_cop_min(auto column) {
    return soql_make_cop(SOQL_COP_MIN, column);
}

#! returns a @ref SoqlColumnOperatorInfo hash for the \c "max" operator; returns maximum column values
/** @par Example:
    @code{.py}
AbstractDataProviderRecordIterator i = provider.searchRecords(("columns": (soql_cop_max("id")), "where": {"Type": "user"}));
    @endcode

    @param column the column specification for the column

    @return a @ref SoqlColumnOperatorInfo hash corresponding to the arguments for use in the \c "columns" argument of
    a @ref select_option_hash "select option hash"
*/
public hash<SoqlColumnOperatorInfo> sub soql_cop_max(auto column) {
    return soql_make_cop(SOQL_COP_MAX, column);
}

#! returns a @ref SoqlColumnOperatorInfo hash for the \c "avg" operator; returns average column values
/** @par Example:
    @code{.py}
AbstractDataProviderRecordIterator i = provider.searchRecords(("columns": (soql_cop_avg("quantity")), "where": ("order_type": "wholesale")));
    @endcode

    @param column the column specification for the column

    @return a @ref SoqlColumnOperatorInfo hash corresponding to the arguments for use in the \c "columns" argument of
    a @ref select_option_hash "select option hash"
*/
public hash<SoqlColumnOperatorInfo> sub soql_cop_avg(auto column) {
    return soql_make_cop(SOQL_COP_AVG, column);
}

#! returns a @ref SoqlColumnOperatorInfo hash for the \c "sum" operator; returns the total sum of a numeric column.
/** @par Example:
    @code{.py}
AbstractDataProviderRecordIterator i = provider.searchRecords(("columns": (soql_cop_sum("quantity")), "where": ("order_type": "wholesale")));
    @endcode

    @param column the column specification for the column

    @return a @ref SoqlColumnOperatorInfo hash corresponding to the arguments for use in the \c "columns" argument of
    a @ref select_option_hash "select option hash"
*/
public hash<SoqlColumnOperatorInfo> sub soql_cop_sum(auto column) {
    return soql_make_cop(SOQL_COP_SUM, column);
}

#! returns a @ref SoqlColumnOperatorInfo hash for the \c "count_distinct" aggregate function; returns row counts
/** @par Example:
    @code{.py}
AbstractDataProviderRecordIterator i = provider.searchRecords({"columns": {"AccountType", soql_cop_count()}, "where": {"Type": "user"}, "groupby": "account_type"});
    @endcode

    @return a @ref SoqlColumnOperatorInfo hash corresponding to the arguments for use in the \c "columns" argument of
    a @ref select_option_hash "select option hash"
*/
public hash<SoqlColumnOperatorInfo> sub soql_cop_count_distinct(auto column = "") {
    return soql_make_cop(SOQL_COP_COUNT_DISTINCT, column);
}

#! returns a @ref SoqlColumnOperatorInfo hash for the \c "format" operator; returns the given column value in the default locale for number, date, time, and currency fields.
/** @par Example:
    @code{.py}
AbstractDataProviderRecordIterator i = provider.searchRecords({"columns": {soql_cop_format("Quantity")}, "where": {"OrderType": "wholesale"}});
    @endcode

    @param column the column specification for the column

    @return a @ref SoqlColumnOperatorInfo hash corresponding to the arguments for use in the \c "columns" argument of
    a @ref select_option_hash "select option hash"

    @note using this function on a date or date/time field will cause it to be returned as a string instead of a
    date/time value
*/
public hash<SoqlColumnOperatorInfo> sub soql_cop_format(auto column) {
    return soql_make_cop(SOQL_COP_FORMAT, column);
}
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
    - soql_wop_or(): for combining SOQL expressions with \c "or"
*/
#@{
#! returns an @ref SoqlOperatorInfo hash
public hash<SoqlOperatorInfo> sub soql_make_op(string op, auto arg) {
    return new hash<SoqlOperatorInfo>({"op": op, "arg": arg});
}

#! returns an @ref SoqlOperatorInfo hash for the \c "like" operator with the given argument for use in salesforce where clauses
/** @par Example:
    @code{.py}
AbstractDataProviderRecordIterator i = provider.searchRecords({"Name": soql_op_like("%smith%")});
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
AbstractDataProviderRecordIterator i = provider.searchRecords({"Name": soql_op_lt("Zebra")});
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
AbstractDataProviderRecordIterator i = provider.searchRecords({"Name": soql_op_le("Zebra")});
    @endcode

    @param arg the argument for the operator

    @return an @ref SoqlOperatorInfo hash for use in salesforce where clauses
*/
public hash<SoqlOperatorInfo> sub soql_op_le(auto arg) {
    return soql_make_op(SOQL_OP_LE, arg);
}

#! returns an @ref SoqlOperatorInfo hash for the \c ">" operator with the given argument for use in salesforce where clauses when comparing column values to immediate values
/** @par Example:
    @code{.py}
AbstractDataProviderRecordIterator i = provider.searchRecords({"Name": soql_op_gt("Apple")});
    @endcode

    @param arg the argument for the operator

    @return an @ref SoqlOperatorInfo hash for use in salesforce where clauses
*/
public hash<SoqlOperatorInfo> sub soql_op_gt(auto arg) {
    return soql_make_op(SOQL_OP_GT, arg);
}

#! returns an @ref SoqlOperatorInfo hash for the \c ">=" operator with the given argument for use in salesforce where clauses when comparing column values to immediate values
/** @par Example:
    @code{.py}
AbstractDataProviderRecordIterator i = provider.searchRecords({"Name": soql_op_ge("Apple")});
    @endcode

    @param arg the argument for the operator

    @return an @ref SoqlOperatorInfo hash for use in salesforce where clauses
*/
public hash<SoqlOperatorInfo> sub soql_op_ge(auto arg) {
    return soql_make_op(SOQL_OP_GE, arg);
}

#! returns an @ref SoqlOperatorInfo hash for the \c "!=" or \c "<>" operator with the given argument for use in salesforce where clauses when comparing column values to immediate values
/** @par Example:
    @code{.py}
AbstractDataProviderRecordIterator i = provider.searchRecords({"Name": soql_op_ne("Smith")});
    @endcode

    @param arg the argument for the operator
*/
public hash<SoqlOperatorInfo> sub soql_op_ne(auto arg) {
    return soql_make_op(SOQL_OP_NE, arg);
}

#! returns an @ref SoqlOperatorInfo hash for the \c "=" operator with the given argument for use in salesforce where clauses when comparing column values to immediate values
/** @par Example:
    @code{.py}
AbstractDataProviderRecordIterator i = provider.searchRecords({"Name": soql_op_eq("Smith")});
    @endcode

    @param arg the argument for the operator

    @return an @ref SoqlOperatorInfo hash for use in salesforce where clauses
*/
public hash<SoqlOperatorInfo> sub soql_op_eq(auto arg) {
    return soql_make_op(SOQL_OP_EQ, arg);
}

#! returns an @ref SoqlOperatorInfo hash for the \c "in" operator with all arguments passed to the function; for use in salesforce where clauses
/** @par Example:
    @code{.py}
AbstractDataProviderRecordIterator i = provider.searchRecords({"Name": soql_op_in(200, 300, 500, 9)});
    @endcode

    @return an @ref SoqlOperatorInfo hash for use in salesforce where clauses

    @note The argument list size may be constrained depending on the database server / driver used; passing a large number of arguments to this function may be a sign of an improper application or query design
*/
public hash<SoqlOperatorInfo> sub soql_op_in() {
    return soql_make_op(SOQL_OP_IN, argv);
}

#! returns an @ref SoqlOperatorInfo hash for the \c "in" operator with the given argument list as the first argument; for use in salesforce where clauses
/** @par Example:
    @code{.py}
AbstractDataProviderRecordIterator i = provider.searchRecords({"name": soql_op_in(idlist)});
    @endcode

    @param args a list of values for the \c "in" operator

    @return an @ref SoqlOperatorInfo hash for use in salesforce where clauses

    @note The argument list size may be constrained depending on the database server / driver used; passing a large number of arguments to this function may be a sign of an improper application or query design
*/
public hash<SoqlOperatorInfo> sub soql_op_in(list<auto> args) {
    return soql_make_op(SOQL_OP_IN, args);
}

#! returns an @ref SoqlOperatorInfo hash for the \c "not" operator; for use in salesforce where clauses
/** @par Example:
    @code{.py}
AbstractDataProviderRecordIterator i = provider.searchRecords({"name": soql_op_not(soql_op_in(200, 300, 500, 9))))_;
    @endcode

    @return an @ref SoqlOperatorInfo hash for use in salesforce where clauses
*/
public hash<SoqlOperatorInfo> sub soql_op_not(hash arg) {
    return soql_make_op(SOQL_OP_NOT, arg);
}

#! returns an @ref SoqlOperatorInfo hash with a fake \c "_OR_" column name; the list of arguments to the function is combined such that each hash in the list generates SQL expressions combined with \a "and" logic, and each of those clauses is combined with \a "or" logic; this is for use in salesforce where clauses
/** @par Example:
    @code{.py}
AbstractDataProviderRecordIterator i = provider.searchRecords({"Id": id} + soql_wop_or({"Type": "C", "ValidationFlag": soql_op_ne("Y")}, {"ExpirationFlag": 'Y'}));
    @endcode
    Generates:
    @verbatim
select ... from Account where Id = '0012A000022K3zxQAC' and ((type = 'C' and ValidationFlag != 'Y') or (ExpirationFlag = 'Y'))
    @endverbatim

    @param h1 the first hash of expressions to combine with \c "or" logic
    @param h2 the second hash of expressions to combine with \c "or" logic
    @param ... remaining expressions to combine with \c "or" logic

    @return a hash with an @ref SoqlOperatorInfo hash value keyed by a fake column name (\c "_OR_" with a numeric
    prefix for uniqueness) for use in a where operation description hash for use in salesforce where clauses

    @note a random prefix is used so that multiple uses of the function can be used to generate a single where clause
    hash
*/
public hash<string, hash<SoqlOperatorInfo>> sub soql_wop_or(hash<auto> h1, hash<auto> h2) {
    softlist<auto> l = h1;
    l += h2;
    if (argv)
        l += argv;
    return new hash<string, hash<SoqlOperatorInfo>>((sprintf("%d:_OR_", rand() % 10000000): soql_make_op(SOQL_OP_OR, l)));
}
#@}

#! a hash of valid operators for use in where clauses
public const DefaultSoqlOpMap = {
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
}
