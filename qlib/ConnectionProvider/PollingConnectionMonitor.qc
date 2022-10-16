# -*- mode: qore; indent-tabs-mode: nil -*-
# Qore PollingConnectionMonitor class definition

/*  PollingConnectionMonitor.qc Copyright 2016 - 2022 Qore Technologies, s.r.o.

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

#! The ConnectionProvider namespace
/** All public declarations in the %ConnectionProvider module are defined in this namespace
*/
public namespace ConnectionProvider {
#! Class for monitoring connection supporting the polling API
/** @since ConnectionProvider 1.7
*/
public class PollingConnectionMonitor {
    public {}

    private {
        #! Lock for atomic actions
        Mutex m();

        #! Connection cache; unique name -> connection object wrapper
        hash<string, hash<PollInfo>> cache;

        #! I/O thread counter
        Counter cnt();

        #! I/O thread TID
        int tid;

        #! I/O thread command queue
        Queue cmdq();

        #! Logger
        *Logger logger;

        #! ping timeout duration
        date ping_timeout = DefaultPingTimeout;

        #! ping repeat duration
        date ping_repeat = DefaultPingRepeat;

        #! I/O thread command: add
        const IO_ADD = "add";

        #! I/O thread command: update
        const IO_UPDATE = "update";

        #! I/O thread command: remove
        const IO_REMOVE = "remove";

        #! I/O thread command: quit
        const IO_QUIT = "quit";

        #! Default ping timeout duration
        const DefaultPingTimeout = 30s;

        #! Default ping repeat duration
        const DefaultPingRepeat = 1m;
    }

    private:internal {
        #! File to signal the I/O thread
        File sem_write;

        #! File to read in the I/O thread
        ReadOnlyFile sem_read;

        #! Semaphore descriptor info
        hash<SocketPollInfo> sem_info;
    }

    #! Creates the object
    constructor(*Logger logger) {
        self.logger = logger;
        hash<PipeInfo> info = File::getPipe();
        sem_read = info.read;
        sem_write = info.write;
        sem_info = <SocketPollInfo>{
            "events": SOCK_POLLIN,
            "socket": sem_read,
        };
    }

    #! Stops the monitoring thread and destroys the object
    destructor() {
        stop();
    }

    #! Sets the logger
    setLogger(Logger logger) {
        self.logger = logger;
    }

    #! Starts monitoring
    /** @throw MONITOR-START-ERROR if the I/O thread is already running
    */
    start() {
        AutoLock al(m);

        if (cnt.getCount()) {
            throw "MONITOR-START-ERROR", sprintf("I/O thread is already running in TID %d", tid);
        }

        cnt.inc();
        on_error cnt.dec();
        tid = background ioThread();
    }

    #! Stops monitoring
    stop() {
        {
            AutoLock al(m);
            if (tid) {
                sendCmd(IO_QUIT);
            }
        }
        cnt.waitForZero();
    }

    #! Returns @ref True if the I/O thread is running
    bool running() {
        return cnt.getCount() > 0;
    }

    #! Adds a connection to be monitored
    /** @param conn the connection to be monitored; must support the connection polling API

        @throw MONITOR-CONNECTION-ERROR the connection does not support the connection polling API
        @throw MONITOR-ADD-ERROR a different connection with this name is already being monitored
    */
    add(AbstractConnection conn) {
        string name = conn.getName();
        if (!conn.supportsPollingApi()) {
            throw "MONITOR-CONNECTION-ERROR", sprintf("connection %y does not support the polling API and cannot be added to "
                "the polling connection monitor", name);
        }
        AutoLock al(m);
        if (*hash<PollInfo> info = cache{name}) {
            # if the connection has already been added, then ignore
            if (info.conn == conn) {
                return;
            }
            # otherwise throw an exception
            throw "MONITOR-ADD-ERROR", sprintf("connection %y with URL %y cannot be added, as connection %y with URL %y "
                "is already being monitored; update the connection if this definition should replace the existing "
                "one", name, conn.getSafeUrl(), name, info.conn.getSafeUrl());
        }
        cache{name} = <PollInfo>{
            "conn": conn,
        };
        restartPing(name, True);
        sendCmd(IO_ADD);
    }

    #! Adds or updates an existing connection that is already being monitored
    /** @param conn the connection to be monitored; must support the connection polling API

        @throw MONITOR-CONNECTION-ERROR the connection does not support the connection polling API
    */
    addOrUpdate(AbstractConnection conn) {
        string name = conn.getName();
        if (!conn.supportsPollingApi()) {
            throw "MONITOR-CONNECTION-ERROR", sprintf("connection %y does not support the polling API and cannot be added to "
                "the polling connection monitor", name);
        }
        string cmd;
        AutoLock al(m);
        if (*hash<PollInfo> info = cache{name}) {
            # if the connection has already been added, then ignore
            if (info.conn == conn) {
                return;
            }
            # otherwise we replace it in any case
            cmd = IO_UPDATE;
        } else {
            cmd = IO_ADD;
        }
        cache{name} = <PollInfo>{
            "conn": conn,
        };
        restartPing(name, True);
        sendCmd(cmd);
    }

    #! Removes the given connection
    /** @param name the connection to be removed

        @return @ref True if the connection was removed, @ref False if no such connection is being monitored

        @see removeConnectionEx()
    */
    bool removeConnection(string name) {
        bool rv;

        AutoLock al(m);
        if (remove cache{name}) {
            rv = True;
            sendCmd(IO_REMOVE);
        } else {
            rv = False;
        }
        return rv;
    }

    #! Removes the given connection
    /** @param name the connection to be removed

        @throw MONITOR-REMOVE-ERROR thrown if the named connection is not being monitored

        @see removeConnection()
    */
    removeConnectionEx(string name) {
        if (!removeConnection(name)) {
            throw "MONITOR-REMOVE-ERROR", sprintf("no connection %y is being monitored", name);
        }
    }

    private ioThread() {
        logger.log(LoggerLevel::INFO, "started polling connection monitor I/O thread");
        on_exit {
            {
                AutoLock al(m);
                remove tid;
            }
            cnt.dec();
            logger.log(LoggerLevel::INFO, "exiting polling connection monitor I/O thread");
        }

        while (True) {
            try {
                int timeout_ms;
                softlist<hash<SocketPollInfo>> poll_list = sem_info;
                {
                    AutoLock al(m);
                    # scan for timed out connections
                    date now = now_us();
                    # process poll info hashes
                    *date min;
                    foreach hash<auto> i in (cache.pairIterator()) {
                        date trigger = i.value.start + (i.value.spop ? ping_timeout : ping_repeat);
                        if (trigger <= now) {
                            handlePingTimeout(i.key);
                            continue;
                        } else if (i.value.spop) {
                            try {
                                *hash<SocketPollInfo> info = i.value.spop.continuePoll();
                                logger.log(LoggerLevel::DEBUG, "%y: got ping info: %y", i.key, info);
                                if (!info) {
                                    handlePingSuccess(i.key);
                                } else {
                                    poll_list += info;
                                }
                            } catch (hash<ExceptionInfo> ex) {
                                handlePingFailed(i.key, ex);
                            }
                        }
                        if (!min || trigger < min) {
                            min = trigger;
                        }
                    }

                    timeout_ms = min ? (min - now).durationMilliseconds() : -1;
                    logger.log(LoggerLevel::DEBUG, "timeout: %d poll_list: %y", timeout_ms, poll_list);
                }
                # ignore return value, all active ping operations are sent in each poll call
                Socket::poll(poll_list, timeout_ms);

                # first process commands, if any
                if (processCommands()) {
                    break;
                }
            } catch (hash<ExceptionInfo> ex) {
               logger.log(LoggerLevel::ERROR, "%s", get_exception_string(ex));
            }
        }
    }

    #! Handles a successful ping
    private handlePingSuccess(string name) {
        logger.log(LoggerLevel::INFO, "%y (poll state %y): successful ping", name, cache{name}.spop.getState());
        restartPing(name);
    }

    #! Handles a failed ping
    private handlePingFailed(string name, hash<ExceptionInfo> ex) {
        logger.log(LoggerLevel::ERROR, "%y (poll state %y): error in ping: %s", name, cache{name}.spop.getState(),
            get_exception_string(ex));
        restartPing(name);
    }

    #! Handles a poll timeout
    private handlePingTimeout(string name) {
        if (cache{name}.spop) {
            logger.log(LoggerLevel::ERROR, "%y (poll state %y): %s connection ping timed out after: %y; restarting "
                "poll", name, cache{name}.spop.getState(), cache{name}.conn.getType(), ping_timeout);
        }
        restartPing(name);
    }

    #! Restarts a ping operation
    private restartPing(string name, *bool force_restart) {
        if (!force_restart && cache{name}.spop) {
            delete cache{name}.spop;
        } else {
            try {
                cache{name}.spop = cache{name}.conn.startPollConnect();
                logger.log(LoggerLevel::DEBUG, "%y: starting connect ping; poll state: %y", name,
                    cache{name}.spop.getState());
            } catch (hash<ExceptionInfo> ex) {
                logger.log(LoggerLevel::ERROR, "%y: error handling ping restart: %s", name, get_exception_string(ex));
            }
        }
        cache{name}.start = now_us();
    }

    private *bool processCommands() {
        # first read all data from semaphore file
        try {
            sem_read.read(-1, 0);
        } catch (hash<ExceptionInfo> ex) {
            if (ex.err != "FILE-READ-TIMEOUT") {
                rethrow;
            }
        }
        try {
            while (hash<auto> h = cmdq.get(-1)) {
                logger.log(LoggerLevel::DEBUG, "I/O thread got command: %y", h);
                switch (h.cmd) {
                    case IO_QUIT:
                        return True;

                    case IO_ADD:
                    case IO_UPDATE:
                    case IO_REMOVE:
                        # processed automatically
                        break;
                }
            }
        } catch(hash<ExceptionInfo> ex) {
            if (ex.err != "QUEUE-TIMEOUT") {
                rethrow;
            }
        }
    }

    private sendCmd(string cmd) {
        cmdq.push({
            "cmd": cmd,
            "args": argv,
        });
        logger.log(LoggerLevel::DEBUG, "wrote command %y args %y to the command queue", cmd, argv);
        sem_write.write(".");
        logger.log(LoggerLevel::DEBUG, "notified I/O thread to wake up");
    }
}
}

# private symbols
namespace Priv {
hashdecl PollInfo {
    # poll start timestamp
    date start;

    # connection
    AbstractConnection conn;

    # poll operation
    AbstractPollOperation spop;
}
}