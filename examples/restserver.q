#!/usr/bin/env qore
# -*- mode: qore; indent-tabs-mode: nil -*-
# @file restserver.q example program using the RestHandler and HttpServer modules

/*  restserver.q Copyright 2013 David Nichols

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

/*  Version History
    * 1.0: initial example program showing usage of the RestHandler and
           HttpServer user modules

    This is a very simple REST server program using the RestHandler and
    HttpServer user modules.
    To exit the program, send it an appropriate signal (SIGTERM, SIGINT, SIGHUP,
    SIGUSR1, SIGUSR2) or interrupt it on the console (which sends the process a
    SIGINT which is handled like other signals)
*/

# do not use "$" signs for vars, etc
%new-style

# execute the restServer class as the application object
%exec-class restServer

# ensure minimum qore version
%requires qore >= 0.8.8

# use the HttpServer module
%requires HttpServer >= 0.3.7

# use the Mime module
%requires Mime >= 1.3

# use the RestHandler module
%requires RestHandler >= 0.1

class ExampleRestHandler inherits RestHandler {
    public {
        static string Dir;
    }

    constructor(*AbstractAuthenticator auth, string dir) : RestHandler(auth) {
        Dir = dir;
    }
}

class restServer {
    private {
	const Opts = (
	    "dir"    : "d,dir=s",
	    "bind"   : "b,bind=s@",
	    "help"   : "h,help",
	    "verbose": "verbose:i+" 
	    );

	# HttpServer object
	HttpServer hs;

	# command line options
	hash opt;
    }

    # no public members
    public {}

    constructor() {
	# process command line options
	GetOpt g(Opts);
        # NOTE: by passing a reference to the list, the arguments parsed will be removed from the list
        # NOTE: calling GetOpt::parse3() means that errors will cause the script to exit immediately
        #       with an informative message	
	opt = g.parse3(\ARGV);
	
	# --help: show help text and exit
	if (opt.help)
	    usage();

	# need bind argument
	if (opt.bind.empty()) {
	    stderr.print("ERROR: missing bind argument\n");
	    usage();
	}

	# need dir argument
	if (opt.dir.empty()) {
	    stderr.print("ERROR: missing directory argument\n");
	    usage();
	}

	# make sure we can read the directory
	if (!is_readable(opt.dir)) {
	    stderr.printf("ERROR: %y: is not readable\n", opt.dir);
	    exit(2);
	}

	# use default authenticator (all connections are allowed)
	AbstractAuthenticator auth();
	
	# create our example rest handler object
	ExampleRestHandler rh(auth, opt.dir);

        try {
	    # create the HttpServer object and add the RestHandler
            hs = new HttpServer(\log(), \errorLog());
	    hs.setHandler("example-rest-handler", "", "*", rh);
            hs.setDefaultHandler("example-rest-handler", rh);

            # start a listener on each bind address
            foreach *string listener in (opt.bind) {
                foreach my hash h in (hs.addListeners(listener)) {
                    log("TID %d: HTTP listener %s started on %s", hs.getListenerTID(h.id), listener, h.address_desc);
                }
            }

            # make sure at least one listener was started
            if (!hs.getListenerCount())
                throw "NO-LISTENERS", "no listeners could be started";

	    # install signal handlers
            installShutdownHandlers();
        }
        catch (hash ex) {
            stderr.printf("%s: %s\n", ex.err, ex.desc);
            stderr.printf("Please correct the above error and try again - the HTTP server was NOT started\n");
            shutdown();
        }

        /* wait for the HttpServer to stop and then exit the program
	   also we have to ensure that our Program object does not go out of scope before the HttpServer does
        */
	hs.waitStop();
    }

    private installShutdownHandlers() {
        set_signal_handler(SIGTERM, \shutdownSignalHandler());
        set_signal_handler(SIGINT,  \shutdownSignalHandler());
        set_signal_handler(SIGHUP,  \shutdownSignalHandler());
        set_signal_handler(SIGUSR1, \shutdownSignalHandler());
        set_signal_handler(SIGUSR2, \shutdownSignalHandler());
    }

    private removeShutdownHandlers() {
        remove_signal_handler(SIGTERM);
        remove_signal_handler(SIGINT);
        remove_signal_handler(SIGHUP);
        remove_signal_handler(SIGUSR1);
        remove_signal_handler(SIGUSR2);
    }

    private shutdownSignalHandler(int sig) {
        removeShutdownHandlers();
        log("%s received, starting system shutdown", SignalToName{sig});
        shutdown();
    }

    synchronized shutdown() {
        log("HTTP server pid %d shutting down now", getpid());
        hs.stopNoWait();
    }

    private log(string fmt) {
	stdout.printf("%y: %s\n", now_us(), vsprintf(fmt, argv));
    }

    private errorLog(string fmt) {
	stdout.printf("%y: ERROR: %s\n", now_us(), vsprintf(fmt, argv));
    }

    static usage() {
	stderr.printf("usage: %s [options]\n"
		      "options:\n"
		      " -b,--bind=ARG     (required) gives a bind address, if only a port number is\n"
		      "                   given, then that port is bound on all interfaces\n"
		      " -d,--dir=ARG      (required) base directory for serving files\n"
		      " -h,--help         this help text\n"
		      " -v,--verbose      show verbose operational messages\n",
		      get_script_name());
	exit(1);
    }
}
