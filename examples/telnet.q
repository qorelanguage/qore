#!/usr/bin/env qore
# -*- mode: qore; indent-tabs-mode: nil -*-

# @file telnet.q example program using the TelnetClient module

/*  telnet.q Copyright 2012 - 2016 Qore Technologies, s.r.o.

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
    * 1.0: initial example program showing usage of the TelnetClient user module

    This is a very simple telnet client program using the TelnetClient user module.
    To exit the client, just type ^] (ie ctrl-])

    Note that there is a bug on Darwin where SIGWINCH is not delivered to
    Qore's signal handling thread; window resizing does not work on Darwin.
*/

%new-style
%enable-all-warnings
%require-types
%strict-args

# execute the telnet class as the application object
%exec-class telnet

# use the TelnetClient module
%requires TelnetClient >= 1.0

%requires ConnectionProvider

class telnet {
    private {
        # command-line options for GetOpt
        const opts = (
            "help": "h,help",
            "timeout": "t,timeout=i",
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

        TelnetClient telnet;
        # thread counter
        Counter cnt();
    }

    # no public members
    public {}

    constructor() {
        # parse the command-line options
        GetOpt g(opts);
        # NOTE: by passing a reference to the list, the arguments parsed will be removed from the list
        # NOTE: calling GetOpt::parse3() means that errors will cause the script to exit immediately
        #       with an informative message
        opt = g.parse3(\ARGV);

        # show help text if necessary
        if (opt.help || ARGV.empty())
            usage();

        # get the connect string for the server (format: "host" or "host:port")
        server = shift ARGV;

        {
            string orig_server = server;
            server = get_connection_url(server);
            if (server != orig_server)
                printf("using connection: %y url: %y\n", orig_server, server);
        }

        try {
            # set up the terminal in the mode we want (Term class defined below)
            Term term();

            # create the telnet client object
            #TelnetClient telnet(server, \log(), opt.verbose ? \log() : NOTHING);
            telnet = new TelnetClient(server, \log(), opt.verbose ? \log() : NOTHING);

            # set a username for the connection, if any
            if (opt.user.val())
                telnet.setUser(opt.user);

            # connect to the server - if no timeout was set on the command line, then the default timeout is used
            telnet.connect(opt.timeout);

            # do not process special keys locally like ^C, etc once connected
            term.unsetSpecial();

            # tell server we are willing to send terminal size info
            telnet.sendData((IAC,WILL,TOPT_NAWS));

            # update the window size on the server when the SIGWINCH signal arrives
            if (SIGWINCH) {
                # note: it seems that Darwin will not deliver SIGINFO and SIGWINCH to Qore's background
                #       signal-handling thread, so window resizing will not work with Darwin
                set_signal_handler(SIGWINCH, sub () { telnet.windowSizeUpdated(); });
            }

            # we start a background thread for reading from the Telnet session
            cnt.inc();
            background startReceive(telnet);
            on_exit cnt.waitForZero();

            # while we read from stdin and write to the Telnet session in the current session
            while (!quit) {
                if (stdin.isDataAvailable(PollInterval)) {
                    string c = stdin.read(1);
                    # if ^] is typed, then exit
                    if (c == chr(0x1d)) {
                        quit = True;
                        break;
                    }
                    telnet.sendTextData(c);
                }

                if (!telnet.isConnected())
                    quit = True;
            }

            if (opt.verbose)
                printf("TID %d input thread terminated\n", gettid());
        }
        catch (hash ex) {
            # ignore NOT-CONNECTED-EXCEPTION as this happens when the server disconnects in the background
            if (ex.err != "NOT-CONNECTED-EXCEPTION") {
                printf("%s:%d: %s: %s\n", ex.file, ex.line, ex.err, ex.desc);
                exit(2);
            }
        }
    }

    # this method will read in data and print it to the screen
    private startReceive(TelnetClient telnet) {
        on_exit cnt.dec();
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
        printf("usage: %s [options] <server>\n"
               "  where <server> is a telnet server address (port optional, ex: telnet.com:23)\n"
               "options:\n"
               " -h,--help         this help text\n"
               " -t,--timeout=ARG  gives a connect timeout in ms (default: %y)\n"
               " -u,--user=ARG     set username\n"
               " -v,--verbose      show protocol negotiation messages\n",
               get_script_name(), TelnetClient::DefaultConnTimeout);
        exit(1);
    }

    # log to the screen
    static log(string str) {
        stdout.printf("%y: %s\n", now_us(), str);
    }
}

# a class for handling, saving, and restoring the terminal settings for the telnet example
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
