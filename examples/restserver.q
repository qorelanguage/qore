#!/usr/bin/env qore
# -*- mode: qore; indent-tabs-mode: nil -*-

# @file restserver.q example program using the RestHandler and HttpServer modules

/*  restserver.q Copyright (C) 2013 - 2022 David Nichols

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
    SIGUSR1, SIGUSR2), interrupt it on the console (which sends the process a
    SIGINT which is handled like other signals), or send a shutdown request
    (PUT /system?action=shutdown)

    it accepts bind and path argument and serves simple functionality on the
    files and directories in the path given on the command line.

    for example:
    GET /files

    will return a list of files, and

    GET /dirs

    will return a list of directories

    if there is a file name "temp.txt" in the current directory, then

    GET /files/temp.txt

    will return a hash of stat information as returned from hstat()

    the following actions can be performed on the files and directories

    PUT /files/temp.txt
    {'action': 'chown', 'user': 1001}

    PUT /files/temp.txt
    {'action': 'chmod', 'mode': 0600}

    PUT /files/temp.txt
    {'action': 'rename', 'newPath': 'temp1.txt'}

    DELETE /files/temp.txt

    POST /files/
    {'name': 'newfile.txt', 'mode': 0644, 'contents': 'text for the new file'}

    POST /dirs/
    {'name': 'subdir', 'mode': 0755}
*/

%new-style
%enable-all-warnings
%require-types
%strict-args

# execute the restServer class as the application object
%exec-class restServer

# ensure minimum qore version
%requires qore >= 0.8.8

# use the HttpServer module
%requires HttpServer >= 0.3.11

# use the HttpServerUtil module
%requires HttpServerUtil >= 0.3.11

# use the Mime module
%requires Mime >= 1.3

# use the RestHandler module
%requires RestHandler >= 0.1

our restServer app;

class AbstractExampleInstanceBaseClass inherits AbstractRestClass {
    private {
        hash h;
    }

    constructor(hash n_h) {
        h = n_h;
    }

    hash get(hash cx, *hash ah) {
        return RestHandler::makeResponse(200, h);
    }

    hash putChown(hash cx, *hash ah) {
        if (!exists cx.body.user && !exists cx.body.group)
            return RestHandler::make400("missing 'user' and/or 'group' parameters for chown(%y)", h.path);
        int rc = chown(h.path, cx.body.user, cx.body.group);
        return rc ? RestHandler::make400("ERROR chown(%y): %s", h.path, strerror()) : RestHandler::makeResponse(200, "OK");
    }

    hash putChmod(hash cx, *hash ah) {
        if (!exists cx.body.mode)
            return RestHandler::make400("missing 'mode' parameter for chmod(%y)", h.path);
        int rc = chmod(h.path, cx.body.mode);
        return rc ? RestHandler::make400("ERROR chmod(%y): %s", h.path, strerror()) : RestHandler::makeResponse(200, "OK");
    }

    hash putRename(hash cx, *hash ah) {
        if (!exists cx.body.newPath)
            return RestHandler::make400("missing 'newPath' parameter for rename(%y)", h.path);
        try {
            rename(h.path, cx.body.newPath);
        }
        catch (hash ex) {
            return RestHandler::make400("rename (%y): %s: %s", h.path, ex.err, ex.desc);
        }
        return RestHandler::makeResponse(200, "OK");
    }
}

class ExampleFileInstanceClass inherits AbstractExampleInstanceBaseClass {
    constructor(hash n_h) : AbstractExampleInstanceBaseClass(n_h) {
    }

    string name() {
        return "file";
    }

    hash del(hash cx, *hash ah) {
        int rc = unlink(h.path);
        return rc ? RestHandler::make400("ERROR unlink(%y): %s", h.path, strerror()) : RestHandler::makeResponse(200, "OK");
    }
}

class ExampleFilesClass inherits AbstractRestClass {
    *AbstractRestClass subClass(string name, hash cx, *hash args) {
        string path = ExampleRestHandler::Dir.path() + "/" + name;
        *hash h = hstat(path);
        if (!h)
            return;

        # make sure it's a file
        if (h.type == "DIRECTORY")
            throw "FILE-ERROR", sprintf("%s: is a directory", path);
        return new ExampleFileInstanceClass(("path": path) + h);
    }

    string name() {
        return "files";
    }

    hash get(hash cx, *hash ah) {
        return RestHandler::makeResponse(200, ExampleRestHandler::Dir.listFiles());
    }

    hash post(hash cx, *hash ah) {
        if (!cx.body.name)
            return RestHandler::make400("missing 'name' parameter for new file");

        File f();
        f.open2(cx.body.name, O_CREAT | O_WRONLY | O_TRUNC, cx.body.mode);
        if (cx.body.content)
            f.write(cx.body.content);
        return RestHandler::makeResponse(200, "OK");
    }
}

class ExampleDirInstanceClass inherits AbstractExampleInstanceBaseClass {
    constructor(hash n_h) : AbstractExampleInstanceBaseClass(n_h) {
    }

    string name() {
        return "directory";
    }

    hash del(hash cx, *hash ah) {
        int rc = rmdir(h.path);
        return rc ? RestHandler::make400("ERROR rmdir(%y): %s", h.path, strerror()) : RestHandler::makeResponse(200, "OK");
    }
}

class ExampleDirsClass inherits AbstractRestClass {
    *AbstractRestClass subClass(string name, hash cx, *hash args) {
        string path = ExampleRestHandler::Dir.path() + "/" + name;
        *hash h = hstat(path);
        if (!h)
            return;

        # make sure it's a directory
        if (h.type != "DIRECTORY")
            throw "DIR-ERROR", sprintf("%s: is not a directory", path);
        return new ExampleFileInstanceClass(("path": path) + h);
    }

    string name() {
        return "dirs";
    }

    hash get(hash cx, *hash ah) {
        return RestHandler::makeResponse(200, ExampleRestHandler::Dir.listDirs());
    }

    hash post(hash cx, *hash ah) {
        if (!cx.body.name)
            return RestHandler::make400("missing 'name' parameter for new directory");

        try {
            ExampleRestHandler::Dir.mkdir(cx.body.name, cx.body.mode);
            return RestHandler::makeResponse(200, "OK");
        }
        catch (hash ex) {
            return RestHandler::make400("mkdir %y: %s: %s", cx.body, ex.err, ex.desc);
        }
    }
}

class ExampleSystemClass inherits AbstractRestClass {
    string name() {
        return "system";
    }

    hash putShutdown(hash cx, *hash ah) {
        app.shutdown();
        return RestHandler::makeResponse(200, "OK");
    }
}

class ExampleRestHandler inherits RestHandler {
    public {
        static Dir Dir();
        *bool verbose;
    }

    constructor(*AbstractAuthenticator auth, string dir, *softbool verb) : RestHandler(auth) {
        if (!Dir.chdir(dir))
            throw "DIRECTORY-ERROR", sprintf("%y: does not exist or is not readable", dir);
        verbose = verb;
        addClass(new ExampleSystemClass());
        addClass(new ExampleFilesClass());
        addClass(new ExampleDirsClass());
    }

    hash get(*hash cx, *hash ah) {
        return RestHandler::makeResponse(200, ("files", "dirs"));
    }

    logInfo(string fmt) {
        printf("%s: INFO: ", now_us().format("YYYY-MM-DD HH:mm:SS.uu"));
        vprintf(fmt + "\n", argv);
    }

    logError(string fmt) {
        printf("%s: ERROR: ", now_us().format("YYYY-MM-DD HH:mm:SS.uu"));
        vprintf(fmt + "\n", argv);
    }

    logDebug(string fmt) {
        if (!verbose)
            return;
        printf("%s: DEBUG: ", now_us().format("YYYY-MM-DD HH:mm:SS.uu"));
        vprintf(fmt + "\n", argv);
    }
}

class restServer {
    private {
	const Opts = (
	    "dir"    : "d,dir=s",
	    "bind"   : "b,bind=s@",
	    "help"   : "h,help",
	    "verbose": "v,verbose:i+"
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

        # setup app variable
        app = self;

	# create our example rest handler object
	ExampleRestHandler rh(auth, opt.dir, opt.verbose);

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
        log("%s received, starting system shutdown", SignalToName{sig});
        shutdown();
    }

    synchronized shutdown() {
        removeShutdownHandlers();
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
