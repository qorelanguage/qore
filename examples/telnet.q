#!/usr/bin/env qore

# do not use "$" signs for vars, etc
%new-style

# execute the telnet class as the application object
%exec-class telnet

# use the Telnet module
%requires Telnet >= 1.0

class telnet {
    private {
	# command-line options for GetOpt
	const opts = (
	    "help": "h,help",
	    "user": "u,user=s",
	    "verbose": "v,verbose",
	    );

	# command-line options
	hash opt;

	# Telnet server
	string server;

	# exit flag
	bool quit = False;

	# default poll interval
	const PollInterval = 10ms;
    }

    # no public members
    public {}

    constructor() {
	# parse the command-line options
	GetOpt g(opts);
	opt = g.parse3(\ARGV);

	# show help text if necessary
	if (opt.help || ARGV.empty())
	    usage();

	# get the connect string for the server (format: "host" or "host:port")
	server = shift ARGV;

	try {
	    # set up the terminal in the mode we want
	    Term term();

	    # create the telnet client object
	    Telnet telnet(server, \log(), opt.verbose ? \log() : NOTHING);

	    # set a username for the connection, if any
	    if (opt.user.val())
		telnet.setUser(opt.user);

	    # connect to the server
	    telnet.connect();

	    # do not process special keys once connected
	    term.unsetSpecial();

	    # tell server we are willing to send terminal size info
	    telnet.sendData((IAC,WILL,TOPT_NAWS));

	    # update the window size on the server when the SIGWINCH signal arrives
	    if (SIGWINCH)
		set_signal_handler(SIGWINCH, sub () { printf("sigwinch\n");telnet.windowSizeUpdated(); });

	    # we start a background thread for reading from the Telnet session
	    background startReceive(telnet);

	    # while we read from stdin and write to the Telnet session in the current session
	    while (!quit) {
		if (stdin.isDataAvailable(PollInterval)) {
		    string c = stdin.read(1);
		    if (c == chr(0x1d)) {
			quit = True;
			break;
		    }
		    telnet.sendTextData(c);
		}
	    }  

            if (opt.verbose)
		printf("TID %d input thread terminated\n", gettid());
	}
	catch (hash ex) {
	    # ignore NOT-CONNECTED-EXCEPTION as this happens when the server disconnects in the background
	    if (ex.err != "NOT-CONNECTED-EXCEPTION")
		printf("%s:%d: %s: %s\n", ex.file, ex.line, ex.err, ex.desc);
	    exit(2);
	}
    }

    # this method will read in data and print it to the screen
    private startReceive(Telnet telnet) {
	while (!quit) {
	    *string str = telnet.getAvailableData(PollInterval);
	    # if the remote end closed the connection, then exit
	    if (!exists str) {
		quit = True;
		break;
	    }

	    # print the output
	    stdout.printf("%s", str);
	    # flush the output
	    stdout.sync();
	}

	if (opt.verbose)
	    printf("TID %d output thread terminated\n", gettid());
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
	       " -u,--user=ARG     set username\n"
	       " -v,--verbose      show protocol negotiation messages\n",
	       get_script_name());
	exit(1);
    }

    # log to the screen
    static log(string str) {
	stdout.printf("%y: %s\n", now_us(), str);
    }
}

class Term {
    private {
	# original terminal settings to restore on exit
	TermIOS orig;

	# current terminal settings
	TermIOS t();
    }

    public {}

    constructor() {
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

        # set the new local flags
        t.setLFlag(lflag);

        # set minimum characters to return on a read
        t.setCC(VMIN, 1);

        # set character input timer in 0.1 second increments (= no timer)
	t.setCC(VTIME, 0);

        # make these terminal attributes active
        stdin.setTerminalAttributes(TCSADRAIN, t);
    }

    unsetSpecial() {
        # get local flags
        int lflag = t.getLFlag();

        # do not check for special input characters (INTR, QUIT, and SUSP)
        lflag &= ~ISIG;

        # set the new local flags
        t.setLFlag(lflag);

        # make these terminal attributes active
        stdin.setTerminalAttributes(TCSADRAIN, t);
    }

    destructor() {
        # restore terminal attributes on exit
        restore();
    }

    restore() {
        # restore terminal attributes
        stdin.setTerminalAttributes(TCSADRAIN, orig);
    }
}
