#!/usr/bin/env qore

%new-style
%exec-class telnet

%requires Telnet >= 1.0

class telnet {
    private {
	const opts = (
	    "help": "h,help",
	    "user": "u,user=s",
	    "pass": "p,pass=s",
	    );

	# command-line options
	hash opt;

	# Telnet server
	string server;
    }

    public {}

    constructor() {
	GetOpt g(opts);
	opt = g.parse3(\ARGV);
	if (opt.help || ARGV.empty())
	    usage();

	# ensure the required options are set
	#checkOpt("user");
	#checkOpt("pass");

	server = shift ARGV;

	try {
	    Term term();

	    Telnet telnet(server, \log(), \log());
	    telnet.connect();
	    
	    while (True) {
		string str = telnet.getAvailableData();

		stdout.printf("%s", str);
		stdout.sync();

		if (stdin.isDataAvailable(10ms)) {
		    string c = stdin.read(1);
		    telnet.sendTerminalTextData(c);
		}
	    }  
	}
	catch (hash ex) {
	    printf("%s: %s\n", ex.err, ex.desc);
	    exit(2);
	}
    }

    checkOpt(string o) {
	if (opt{o}.empty()) {
	    stderr.printf("missing %y option; use %s -h for option information\n", o, get_script_name());
	    exit(1);
	}
    }

    static usage() {
	printf("usage: %s [options] <Telnet>\n"
	       "  where <Telnet> is a server address with an optional port (ex: telnet.com:25)\n"
	       "options:\n"
	       " -h,--help         this help text\n"
	       " -p,--pass=ARG     password for the connection\n"
	       " -u,--user=ARG     username for the connection\n",
	       get_script_name());
	exit(1);
    }

    static log(string str) {
	stdout.printf("%y: %s\n", now_us(), str);
    }
}

class Term {
    private {
	TermIOS orig;
    }

    public {}

    constructor() {
	TermIOS t();

        # get current terminal attributes for stdin
        stdin.getTerminalAttributes(t);

        # save a copy
        orig = t.copy();

        # get local flags
        int lflag = t.getLFlag();

        # disable canonical input mode (= turn on "raw" mode)
        lflag &= ~ICANON;

        # turn off echo mode
        lflag &= ~ECHO;

        # do not check for special input characters (INTR, QUIT, and SUSP)
        #lflag &= ~ISIG;

        # set the new local flags
        t.setLFlag(lflag);

        # set minimum characters to return on a read
        t.setCC(VMIN, 1);

        # set character input timer in 0.1 second increments (= no timer)
	t.setCC(VTIME, 0);

        # make these terminal attributes active
        stdin.setTerminalAttributes(TCSADRAIN, t);
    }

    destructor() {
        restore();
    }

    restore() {
        # restore terminal attributes
        stdin.setTerminalAttributes(TCSADRAIN, orig);
    }
}
