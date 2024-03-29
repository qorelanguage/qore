# -*- mode: qore; indent-tabs-mode: nil -*-
# Qore PollingConnectionMonitor class definition

/*  PollingConnectionMonitor.qc Copyright 2016 - 2024 Qore Technologies, s.r.o.

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
public class PollingConnectionMonitor inherits LoggerWrapper {
    public {
        #! Polling event map
        const PollEventMap = {
            SOCK_POLLIN: "read",
            SOCK_POLLOUT: "write",
            SOCK_POLLIN | SOCK_POLLOUT: "read and write",
        };
    }

    private {
        #! Lock for atomic actions
        Mutex m();

        #! Connection cache; unique name -> connection object wrapper
        hash<string, hash<PollInfo>> cache;

        #! I/O thread TID
        int tid;

        #! Autostart monitoring thread with first connection
        bool autostart = True;

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

        #! I/O thread counter
        Counter mcnt();

        #! I/O thread command queue
        Queue cmdq();
    }

    #! Creates the object
    constructor(*LoggerInterface logger) : LoggerWrapper(logger) {
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

    #! Returns information about the object
    hash<auto> getInfo() {
        return {
            "autostart": autostart,
            "connections": keys cache,
            "io_tid": tid,
        };
    }

    #! Set autostart flag
    setAutostart(bool autostart = True) {
        self.autostart = autostart;
    }

    #! Get autostart flag
    bool getAutostart() {
        return autostart;
    }

    #! Starts monitoring
    /** @throw MONITOR-START-ERROR if the I/O thread is already running
    */
    start() {
        AutoLock al(m);

        if (tid) {
            throw "MONITOR-START-ERROR", sprintf("I/O thread is already running in TID %d", tid);
        }

        startIntern();
    }

    #! Stops monitoring
    stop() {
        {
            AutoLock al(m);
            if (tid) {
                sendCmd(IO_QUIT);
            }
        }
        mcnt.waitForZero();
    }

    #! Stops monitoring and clears all connections
    stopClear() {
        int count;
        {
            AutoLock al(m);
            count = tid ? stopIntern() : 0;
        }
        mcnt.waitForZero();
        log(LoggerLevel::INFO, "removed all connections (%d connection%s removed)", count, count == 1 ? "" : "s");
    }

    #! Returns @ref True if the I/O thread is running
    bool running() {
        return mcnt.getCount() > 0;
    }

    #! Adds a connection to be monitored
    /** @param conn the connection to be monitored; must support the connection polling API
        @param other a free-form hash to be stored alongside the connection

        @throw MONITOR-CONNECTION-ERROR the connection does not support the connection polling API
        @throw MONITOR-ADD-ERROR a different connection with this name is already being monitored
    */
    add(AbstractConnection conn, *hash<auto> other) {
        string name = conn.getName();
        if (!conn.supportsPollingApi()) {
            throw "MONITOR-CONNECTION-ERROR", sprintf("connection %y does not support the polling API and cannot be "
                "added to the polling connection monitor", name);
        }
        AutoLock al(m);
        if (*hash<PollInfo> info = cache{name}) {
            # if the connection has already been added, then ignore
            if (info.conn == conn) {
                return;
            }
            # otherwise throw an exception
            throw "MONITOR-ADD-ERROR", sprintf("connection %y with URL %y cannot be added, as connection %y with "
                "URL %y is already being monitored; update the connection if this definition should replace the "
                "existing one", name, conn.getSafeUrl(), name, info.conn.getSafeUrl());
        }
        cache{name} = <PollInfo>{
            "conn": conn,
            "other": other,
        };
        if (autostart && !tid) {
            startIntern();
        }
        if (*hash<ExceptionInfo> ex = restartPing(name, True)) {
            failedToStartPing(name, ex);
        }
        sendCmd(IO_ADD);
    }

    #! Adds or updates an existing connection that is already being monitored
    /** @param conn the connection to be monitored; must support the connection polling API
        @param other a free-form hash to be stored alongside the connection

        @throw MONITOR-CONNECTION-ERROR the connection does not support the connection polling API
    */
    addOrUpdate(AbstractConnection conn, *hash<auto> other) {
        string name = conn.getName();
        if (!conn.supportsPollingApi()) {
            throw "MONITOR-CONNECTION-ERROR", sprintf("connection %y does not support the polling API and cannot be "
                "added to the polling connection monitor", name);
        }
        string cmd;
        RecursiveAutoLock al(m);
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
            "other": other,
        };
        restartPing(name, True);
        sendCmd(cmd);
    }

    #! Removes the given connection
    /** @param name the connection to be removed

        @return @ref True if the connection was removed, @ref False if no such connection is being monitored

        @note Stops the I/O thread if the last connection is removed and the \a autostart flag is enabled

        @see removeConnectionEx()
    */
    bool removeConnection(string name) {
        bool rv;
        bool stopped;

        {
            RecursiveAutoLock al(m);
            if (remove cache{name}) {
                rv = True;
                sendCmd(IO_REMOVE);
                if (autostart && !cache) {
                    stopIntern();
                    stopped = True;
                }
            } else {
                rv = False;
            }
        }
        if (stopped) {
            mcnt.waitForZero();
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

    private startIntern() {
        mcnt.inc();
        on_error mcnt.dec();
        tid = background ioThread();
    }

    private int stopIntern() {
        sendCmd(IO_QUIT);
        return (remove cache).size();
    }

    private ioThread() {
        log(LoggerLevel::INFO, "started polling connection monitor I/O thread");
        on_exit {
            {
                AutoLock al(m);
                remove tid;
            }
            log(LoggerLevel::INFO, "exiting polling connection monitor I/O thread");
            mcnt.dec();
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
                    foreach string key in (keys cache) {
                        *hash<auto> value = cache{key};
                        if (!value) {
                            # removed from cache while the lock was unlocked
                            continue;
                        }
                        date delta = value.spop ? ping_timeout : ping_repeat;
                        date trigger = value.start + delta;
                        AbstractConnection c = value.conn;
                        if (trigger <= now) {
                            c.last_check = value.start;
                            c.up = False;
                            handlePingTimeout(key, delta);
                            continue;
                        } else if (value.spop) {
                            try {
                                *hash<SocketPollInfo> info;
                                {
                                    # to avoid deadlocks with external code, such as with option update code,
                                    # we run with this unlocked
                                    m.unlock();
                                    on_exit m.lock();
                                    info = value.spop.continuePoll();
                                }
                                if (value != cache{key}) {
                                    # removed / redefined in cache, ignore
                                    continue;
                                }
                                log(LoggerLevel::DEBUG, "%y: state: %y: got ping info: %y", key,
                                    value.spop.getState(), info ? {
                                        "events": PollEventMap{info.events} ?? "unknown",
                                        "socket": sprintf("%s %s", info.socket.className(),
                                            info.socket.uniqueHash()[0..8]),
                                    } : NOTHING);
                                if (!info) {
                                    *bool oldok = c.last_check ? c.up : NOTHING;
                                    c.last_check = value.start;
                                    handlePingSuccess(key, now_us() - value.start, oldok);
                                } else {
                                    poll_list += info;
                                }
                            } catch (hash<ExceptionInfo> ex) {
                                c.last_check = value.start;
                                handlePingFailed(key, now_us() - value.start, ex);
                            }
                        }
                        if (!min || trigger < min) {
                            min = trigger;
                        }
                    }

                    timeout_ms = min ? (min - now).durationMilliseconds() : -1;
                    log(LoggerLevel::DEBUG, "timeout: %d poll_list: %y", timeout_ms, (map $1 + {
                        "events": PollEventMap{$1.events} ?? "unknown",
                        "socket": sprintf("%s %s", $1.socket.className(), $1.socket.uniqueHash()[0..8]),
                    }, poll_list));
                }
                # ignore return value, all active ping operations are sent in each poll call
                Socket::poll(poll_list, timeout_ms);

                # first process commands, if any
                if (processCommands()) {
                    break;
                }
            } catch (hash<ExceptionInfo> ex) {
               log(LoggerLevel::ERROR, "%s", get_exception_string(ex));
            }
        }
    }

    #! Could not start ping
    private failedToStartPing(string name, hash<ExceptionInfo> ex) {
        # error already logged
        cache{name}.conn.handlePingFailed(0s, ex);
    }

    #! Handles a successful ping
    private handlePingSuccess(string name, date delta, *bool oldok) {
        log(LoggerLevel::INFO, "%y (poll state %y): successful ping (duration: %y)", name,
            cache{name}.spop.getState(), delta);
        cache{name}.conn.handlePingSuccess(delta);
        restartPing(name);
    }

    #! Handles a failed ping
    private handlePingFailed(string name, date delta, hash<ExceptionInfo> ex) {
        log(LoggerLevel::ERROR, "%y (poll state %y): error in ping (duration: %y): %s", name,
            cache{name}.spop.getState(), delta, get_exception_string(ex));
        cache{name}.conn.handlePingFailed(delta, ex);
        restartPing(name);
    }

    #! Handles a poll timeout
    private handlePingTimeout(string name, date delta) {
        if (cache{name}.spop) {
            handlePingTimeoutIntern(name, delta);
            restartPing(name);
        } else {
            if (*hash<ExceptionInfo> ex = restartPing(name)) {
                failedToStartPing(name, ex);
            }
        }
    }

    #! Handles a ping timeout
    private handlePingTimeoutIntern(string name, date delta) {
        string err = sprintf("(poll state %s): %s connection ping timed out after: %y", cache{name}.spop.getState(),
            cache{name}.conn.getType(), delta);
        cache{name}.conn.handlePingFailed(delta, err);
        log(LoggerLevel::ERROR, "%y %s; restarting poll", name, err);
    }

    #! Restarts a ping operation
    /** @return an exception object if the ping could not be started
    */
    private *hash<ExceptionInfo> restartPing(string name, *bool force_restart) {
        *hash<ExceptionInfo> rv;
        if (!force_restart && cache{name}.spop) {
            delete cache{name}.spop;
        } else {
            try {
                cache{name}.spop = cache{name}.conn.startPollConnect();
                log(LoggerLevel::DEBUG, "%y: starting connect ping to %y; poll state: %y", name,
                    cache{name}.conn.getUrl(), cache{name}.spop.getState());
            } catch (hash<ExceptionInfo> ex) {
                log(LoggerLevel::ERROR, "%y: error handling ping restart: %s", name, get_exception_string(ex));
                rv = ex;
            }
        }
        cache{name}.start = now_us();
        return rv;
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
                #log(LoggerLevel::DEBUG, "I/O thread got command: %y", h);
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
        # only send the command if the I/O thread is running
        if (!tid) {
            return;
        }
        cmdq.push({
            "cmd": cmd,
            "args": argv,
        });
        #log(LoggerLevel::DEBUG, "wrote command %y args %y to the command queue", cmd, argv);
        sem_write.write(".");
        #log(LoggerLevel::DEBUG, "notified I/O thread to wake up");
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

    # other data stored alongside the connection
    *hash<auto> other;
}

class RecursiveAutoLock {
    private {
        Mutex m;
    }

    constructor(Mutex m) {
        if (!m.lockOwner()) {
            m.lock();
            self.m = m;
        }
    }

    destructor() {
        if (m) {
            m.unlock();
        }
    }
}
}