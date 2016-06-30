#!/usr/bin/env qore
%new-style
%exec-class Main
%requires SchemaReverse
%requires SqlUtil

%try-module QorusClientCore
%define NO_QORUS
%endtry

const VERSION = "0.1";

const OPTIONS = (
        "help" : "help,h",
        "version" : "version,V",
        "verbose" : "verbose,v",
        "schema" : "schema,s=s",
        "sequences" : "sequences,q=s",
        "tables" : "tables,t=s",
        "types" : "types,y=s",
    );

sub help(int exitCode) {
    printf("Usage:
%s <options> <connection_string>

Options:
 -h --help              Display help and exits
 -V --version           Show version of this script
 -v --verbose           Show additional verbose info
 -s --schema=<name>     Export full (supported objects) DB schema.
                        The class of the qsm output will be named <name>
 -t --tables=<tables>   Export specified table(s). <tables> can be exact
                        name of the table, table list (separated by ','),
                        or a regular expression to match table name
 -s --sequences=<seqs>  Export specified table(s). <seqs> can be exact
                        name of the table, table list (separated by ','),
                        or a regular expression to match table name
 -y --types=<types>     Export specified type(s). <types> can be exact
                        name of the table, table list (separated by ','),
                        or a regular expression to match table name

Note:
    --schema and (--tables or --sequences or --types) cannot be
    used together

Example:
    %s -s=OmqXbox oracle:omq/omq@xbox
        export full OMQ schema

    %s -t=\"workflows,.*step.*\" oracle:omq/omq@xbox
        export only tables whch matches exact table name 'workflows',
        and any of tables with 'step' in name

",
    get_script_name(), get_script_name(), get_script_name()
    );
    exit(exitCode);
}

class Main {
    private {
        hash m_opts;
        Datasource m_ds;
    }
    
    constructor() {
        GetOpt go(OPTIONS);
        m_opts = go.parse2(\ARGV);
        
        if (m_opts.help) {
            help(0);
        }

        if (m_opts.version) {
            printf(VERSION + "\n");
            exit(0);
        }

        if (!m_opts.schema && !m_opts.sequences && !m_opts.tables && !m_opts.types) {
            throw "NOT-ALLOWED-OPTS", "At least one option must be set: schema, sequences, tables";
        }

        if (m_opts.schema && (m_opts.sequences || m_opts.tables || m_opts.types)) {
            throw "NOT-ALLOWED-OPTS", "Cannot combine schema and sequences or tables together";
        }

        *string connstr = shift ARGV;
        if (!connstr) {
            throw "NO-CONNECTION-STRING";
        }

%ifndef NO_QORUS
        QorusClient::init2();
        try {
            m_ds = omqclient.getDatasource(connstr);
        }
        catch (ex) {
#            printf("Qorus client found, but dbparams does not contain: %n\n", connstr);
#            printf("    Continuing with regular connection\n");
        }
%endif

        if (!m_ds) {
            m_ds = new Datasource(connstr);
        }

        if (m_opts.schema) {
            cout("Exporting full schema: %s", m_opts.schema);
            SchemaReverse sr(m_ds, m_opts.schema, connstr);
            printf("%s\n", sr.toString());
        }
        else {
            if (m_opts.sequences) {
                exportObjects(m_opts.sequences, "SequencesReverse");
            }
            if (m_opts.tables) {
                exportObjects(m_opts.tables, "TablesReverse");
            }
            if (m_opts.types) {
                exportObjects(m_opts.types, "TypesReverse");
            }
        }
    } # constructor

    private string makeRxString(string val) {
        list ret = val.split(",");
        ret = map trim($1), ret;
        return ret.join("|");
    }

    private exportObjects(string input, string className) {
        cout("Exporting objects: %s, for: %s", className, input);
        string rx = makeRxString(input);
        object o = create_object(className, m_ds, rx);
        printf("%s\n", call_object_method(o, "toString"));
    }

    private cout(string msg) {
        if (!m_opts.verbose) {
            return;
        }

        printf("# %s\n", vsprintf(msg, argv));
    }

} # class Main
