#!/usr/bin/env qore
%requires Qorize
%requires SqlUtil
%new-style

const TYPE_ATTRIBUTES = (
    # native_type : allowed list
    "number"    : ("qore_type", "native_type", "size", "scale", "default_value", "comment"),
    "numeric"   : ("qore_type", "native_type", "size", "scale", "default_value", "comment"),
    "date"      : ("qore_type", "native_type", "scale", "default_value", "comment"), 
    "varchar"   : ("qore_type", "native_type", "size", "scale", "default_value", "comment"), 
    "char"      : ("qore_type", "native_type", "size", "scale", "default_value", "comment"), 
    "timestamp" : ("qore_type", "native_type", "scale", "default_value", "comment"),
    "text"      : ("qore_type", "native_type", "scale", "default_value", "comment"), 
    "bytea"     : ("qore_type", "native_type", "scale", "default_value", "comment"), 
);

const opts = (
    "table"    : "t,table=s@",
    "sequence" : "s,sequence=s@",
    "help"     : "h,help",
);

GetOpt g(opts);
hash opt = g.parse3(\ARGV);

if (opt.help || !ARGV[0]) {
    printf("Usage: get_schema.q -t TABLES -s SEQUENCES DATASOURCE
Options:
    -t,--table     list of tables to export
    -s,--sequence  list of sequences to export
    -h,--help      prints this
");
    exit(1);
}

if (!ARGV[0]) {
    throw 'NO-DATASOURCE-GIVEN', 'Please provide datasource where the definition should be taken';
}

Datasource ds(ARGV[0]);
SqlUtil::Database db(ds);

list tables_to_export = opt.table ?* db.listTables();
list sequences_to_export = opt.sequence ?* db.listSequences();


hash data;

foreach string table in (tables_to_export) {
    Table t(ds, table);

    hash h = t.describe().getHash();
    foreach string key in (h.keys()) {
        h{key} = hash(h{key});
    }

    data{table} = h;
}

string str_header  = create_header_string();

string str_sequences       = create_sequences_string(sequences_to_export);
string str_coloptions      = create_column_options_string();
string str_generic_options = create_generic_options_string();
string str_index_options   = create_index_options_string();
string str_tables          = create_table_string(data);
string str_private         = create_private_namespace_string(str_tables, str_sequences, str_coloptions, str_generic_options, str_index_options);

string str_public = create_str_public();

string str = create_qsm(str_header, str_private, str_public);

printf("%s", str);


sub create_qsm(string header, string priv, string pub) {
    return header + priv + pub;
}

sub create_sequences_string(list sequences = ()) {
    string result = "    const SequenceList = (\n";
    foreach string sequence in (sequences) {
        result += '        "' + sequence + "\": {},\n";
    }
    result += '    );

    const Sequences = (
        "driver": (
            "oracle": SequenceList,
            "pgsql": SequenceList,
        ),
    );
';

    return result;
}

sub create_index_options_string() {
    return '    const IndexOptions = (
        "driver": (
            "oracle": (
                "compute_statistics": True,
            ),
        ),
    );
';
}

sub create_generic_options_string() {
    return '    const GenericOptions = (
        "replace": True,
    );
';
}

sub create_column_options_string() {
    return "const ColumnOptions = hash();\n";
}

sub create_private_namespace_string(string tables = '', string sequences = '', string coloptions = '', string generic_options = '', string index_options = '') {
    return '# private namespace for private schema declarations
namespace Private {
' + tables + sequences + coloptions + generic_options + index_options + '}
';
}

sub create_header_string(string desc = 'generated schema') {
    return '# -*- mode: qore; indent-tabs-mode: nil -*-

%requires qore >= 0.8.10

module MySchema {
    version = "1.0";
    desc    = "' + desc + '";
    author  = "sqlutil, Qore Technologies, sro";
    url     = "http://www.qoretechnologies.com";
}

%requires Schema
%requires SqlUtil

%new-style
%require-types
%enable-all-warnings

';
}

sub create_str_public() {
    return 'public namespace SchemaClass {
    public string sub get_datasource_name() {
        return "omquser";
    }

    public SchemaClass sub get_user_schema(AbstractDatasource ds, *string dts, *string its) {
        return new SchemaClass(ds, dts, its);
    }

    public class SchemaClass inherits AbstractSchema {
        public {
            const SchemaName = "SchemaClass";
            const SchemaVersion = "1.0";
        }

        constructor(AbstractDatasource ds, *string dts, *string its) :  AbstractSchema(ds, dts, its) {
        }

        private string getNameImpl() {
            return SchemaName;
        }

        private string getVersionImpl() {
            return SchemaVersion;
        }

        private *hash getTablesImpl() {
            return Tables;
        }

        private *hash getSequencesImpl() {
            return Sequences;
        }

        private *hash getIndexOptionsImpl() {
            return IndexOptions;
        }

        private *hash getGenericOptionsImpl() {
            return GenericOptions;
        }

        private *hash getColumnOptionsImpl() {
            return ColumnOptions;
        }
    }
}
';
}

sub create_table_string(hash specs) {
    string result = '';

    foreach string table_name in (specs.keys()) {
        hash columns_desc = specs{table_name};
        string ident_name = "T_" + table_name.upr();

        result += "    const " + ident_name + " = (\n        \"columns\": (\n";
        foreach string column_name in (columns_desc.keys()) {
            hash column_specs_all = columns_desc{column_name};
            string native_type = column_specs_all.native_type;
            if (!exists TYPE_ATTRIBUTES{native_type}) {
                throw 'COLUMN-TYPE-NOT-SUPPORTED', 'Please specify the list of allowed attributes for type ' + native_type;
            }
            list allowed_attrs = TYPE_ATTRIBUTES{native_type};
            hash column_specs = column_specs_all{allowed_attrs};

            string type = qorize(column_specs, 'VAR', True);
            type = regex_subst(type, "hash VAR = \\n", '');
            type = regex_subst(type, "\\);", '');
            result += '            "' + column_name + '": ' + type + "),\n";
        }
        result += "        ),\n    );\n\n";
    }

    result += "    const Tables = (\n";
    foreach string table_name in (specs.keys()) {
        result += '        "' + table_name + '": T_' + table_name.upr() + ",\n";
    }
    result += "    );\n";

    return result;
}
