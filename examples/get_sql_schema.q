#!/usr/bin/env qore
%new-style
%exec-class Main
%requires SchemaReverse
%requires SqlUtil

%try-module QorusClientCore
%define NO_QORUS
%endtry

const VERSION = "0.1";

const OBJECT_OPTIONS = (
        "sequences" : "sequences,q:s",
        "tables" : "tables,t:s",
        "types" : "types,y:s",
        "views" : "views,v:s",
    );

const OPTIONS = (
        "help" : "help,h",
        "version" : "version,V",
        "verbose" : "verbose",
        "schema" : "schema,s=s",
    ) + OBJECT_OPTIONS;

sub help(int exitCode) {
    printf("Usage:
%s <options> <connection_string>

Options:
 -h --help              Display help and exits
 -V --version           Show version of this script
    --verbose           Show additional verbose info
 -s --schema=<name>     Export full (supported objects) DB schema.
                        The class of the qsm output will be named <name>
 -t --tables=<val>      Export specified table(s). See <val> section below.
 -s --sequences=<val>   Export specified sequence(s). See <val> section below.
 -y --types=<val>       Export specified type(s). See <val> section below.
 -v --views=<val>       Export specified view(s). See <val> section below.


Object <val> filtering:
    Any <val> can be exact name of the object, a list (separated by ','),
    or a regular expression to match object name.

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

        bool userObjects = False;
        foreach string i in (OBJECT_OPTIONS.keyIterator()) {
            if (m_opts{i}.typeCode() == NT_BOOLEAN && m_opts{i}) {
                m_opts{i} = ".*";
            }
            if (m_opts{i}.size()) {
                userObjects = True;
            }
        }

        if (!m_opts.schema && !userObjects) {
            error("NOT-ALLOWED-OPTS", sprintf("At least one option must be set: \"schema\" or %y", OBJECT_OPTIONS.keys()));
        }

        if (m_opts.schema && userObjects) {
            error("NOT-ALLOWED-OPTS", sprintf("Cannot combine \"schema\" and %y together", OBJECT_OPTIONS.keys()));
        }

        *string connstr = shift ARGV;
        if (!connstr) {
            error("NO-CONNECTION-STRING", "Please specify qore DB connection string");
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
            HashIterator it(OBJECT_OPTIONS);
            while (it.next()) {
                string key = it.getKey();
                string classKey = substr(key, 0, 1).upr() + substr(key,1); # TODO/FIXME: <string>.capitalize() one day...
                if (exists m_opts{key}) {
                    exportObjects(m_opts{key}, sprintf("%sReverse", classKey));
                }
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
    
    static error(string errcode, string msg) {
        stderr.printf("%s: %s\n", errcode, msg);
        exit(1);
    }

} # class Main
