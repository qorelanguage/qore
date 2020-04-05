# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore DataProviderRecordPipeline class definition

/** DataProviderRecordPipeline.qc Copyright 2019 - 2020 Qore Technologies, s.r.o.

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

# assume local scope for variables, do not use "$" signs
%new-style
# require type definitions everywhere
%require-types
#! strict argument handling
%strict-args
# enable all warnings
%enable-all-warnings
# allow weak references
%allow-weak-references

#! contains all public definitions in the DataProvider module
public namespace DataProvider {
#! Pipeline element
class PipelineInfo {
    public {
        #! Pipeline ID
        int id;

        #! Queue condition variable
        Condition cond();

        #! Number of threads waiting on data
        int threads_waiting = 0;

        #! Data queue
        list<hash<auto>> queue;

        #! Maximum queue size
        int size;

        #! Pipeline elements
        /** elements are either:
            - AbstractDataProviderRecordProcessor objects
            - list of PipelineInfo objects
        */
        list<auto> nodes();

        #! Parent object
        DataProviderRecordPipeline parent;
    }

    #! Creates the object
    constructor(DataProviderRecordPipeline parent, *int id, int size) {
        self.size = size;
        self.parent := parent;

        parent.cnt.inc();
        on_error {
            parent.cnt.dec();
        }
        int tid = background run();
        self.id = id ?? tid;
    }

    #! Returns the pipeline ID
    int getId() {
        return id;
    }

    #! Submits data for processing
    /** @param record the record to process

        @throw PIPELINE-SUBMISSION-ABORTED cannot submit data; the pipeline is shutting down

        @note Called in the pipeline lock
    */
    submit(hash<auto> record) {
        while (!parent.stop && queue.lsize() == size) {
            ++threads_waiting;
            cond.wait(parent.lck);
            --threads_waiting;
        }

        if (parent.stop) {
            throw "PIPELINE-SUBMISSION-ABORTED", "cannot submit data; the pipeline is shutting down";
        }

        queue += record;
        cond.broadcast();
    }

    #! Processing thread
    /** @note Called in the pipeline lock
    */
    run() {
        on_exit {
            parent.cnt.dec();
        }

        #! wait for an event
        while (!parent.stop) {
            parent.lck.lock();
            on_exit parent.lck.unlock();

            if (!queue) {
                cond.wait(parent.lck);
                continue;
            }

            hash<auto> record = shift queue;
            if (threads_waiting) {
                cond.broadcast();
            }

            foreach auto elem in (nodes) {
                if (elem instanceof AbstractDataProviderRecordProcessor) {
                    record = elem.submit(record);
                } else {
                    # must be the last entry in the list
                    map $1.submit(record), elem;
                }
            }
        }
    }

    #! Wait for the queue to be empty
    /** @note Called in the pipeline lock
    */
    waitDone() {
        while (queue) {
            ++threads_waiting;
            cond.wait(parent.lck);
            --threads_waiting;
        }
    }
}

#! Defines a class for passing data through record processors
public class DataProviderRecordPipeline {
    public {
        #! Atomic lock
        Mutex lck();

        #! Stop flag
        bool stop;

        #! Thread counter
        Counter cnt();
    }

    private {
        #! The input iterator
        AbstractDataProviderRecordIterator input;

        #! Hash of pipelines keyed by pipeline ID
        hash<string, PipelineInfo> pmap;

        #! Bulk flag
        bool do_bulk = True;

        #! Locked flag
        bool locked = False;

        #! Info log closure; takes a single format string and then arguments for format placeholders
        *code info_log;

        #! Error log closure; takes a single format string and then arguments for format placeholders
        *code error_log;

        #! Debug log closure; takes a single format string and then arguments for format placeholders
        *code debug_log;
    }

    #! Creates the object with the given input iterator
    /** @param input the input iterator
        @param opts any options for the pipeline; known options are:
        - \c info_log: a closure or call reference that takes a single format string and then optional arguments
          corresponding to the placeholders in the format string
        - \c error_log: a closure or call reference that takes a single format string and then optional arguments
          corresponding to the placeholders in the format string
        - \c debug_log: a closure or call reference that takes a single format string and then optional arguments
          corresponding to the placeholders in the format string

        @Note The object is created with an initial pipeline with ID 0
    */
    constructor(AbstractDataProviderRecordIterator input, *hash<auto> opts) {
        self.input = input;
        info_log = opts.info_log;
        error_log = opts.error_log;
        debug_log = opts.debug_log;

        lck.lock();
        on_exit lck.unlock();

        createPipelineIntern(0);
    }

    #! Destroys the object
    destructor() {
        stop = True;
        {
            lck.lock();
            on_exit lck.unlock();

            map $1.cond.broadcast(), pmap.iterator();
        }

        cnt.waitForZero();
    }

    #! Appends a record processor to the default pipeline
    /**
        @note the initial pipeline is pipeline 0
    */
    append(AbstractDataProviderRecordProcessor processor) {
        append(0, processor);
    }

    #! Appends a record processor to a pipeline
    /** @param id the pipeline ID as returned from appendPipeline()
        @param processor the record processor to append to the pipeline

        @note the initial pipeline is pipeline 0

        @see appendPipeline()
    */
    append(int id, AbstractDataProviderRecordProcessor processor) {
        lck.lock();
        on_exit lck.unlock();

        checkUpdatePipelineIntern(id);

        if (pmap{id}.nodes.last() instanceof list<PipelineInfo>) {
            throw "PIPELINE-ERROR", sprintf("pipeline %d already terminated in a pipeline; no more elements can be "
                "added after a pipeline terminates in a pipeline", id);
        }

        pmap{id}.nodes += processor;

        if (do_bulk && !processor.supportsBulkApi()) {
            do_bulk = False;
        }
    }

    #! Appends a new pipeline to an existing pipeline and returns the new pipeline ID
    /** @param id the pipeline to which the new pipeline will be appended

        @return the new pipeline ID

        @throw PIPELINE-ERROR the pipeline is locked, or the given pipeline does not exist or already ends in a pipeline

        @note the initial pipeline is pipeline 0

        @see append(int, AbstractDataProviderRecordProcessor)
    */
    int appendPipeline(int id) {
        lck.lock();
        on_exit lck.unlock();

        checkUpdatePipelineIntern(id);

        PipelineInfo info = createPipelineIntern();

        if (!pmap{id}.nodes || (pmap{id}.nodes.last() instanceof AbstractDataProviderRecordProcessor)) {
            list<PipelineInfo> pipeline_list();
            push pmap{id}.nodes, pipeline_list;
        }

        # add pipeline to the final element list
        pmap{id}.nodes[pmap{id}.nodes.size() - 1] += info;
        return info.getId();
    }

    #! Runs the pipeline until complete or an error occurs
    /** Returns when all pipelines have completed processing

        @throw PIPELINE-ERROR pipeline is already running or has no elements

        @see
        - runAsync()
        - waitDone();
    */
    run() {
        runAsync();
        waitDone();
    }

    #! Runs the pipeline until complete or an error occurs
    /** Returns when all data has been submitted, but not necessarily before ipelines have completed processing;
        call @ref waitDone() after this method to wait until all pipelines have finished processing
    */
    runAsync() {
        {
            lck.lock();
            on_exit lck.unlock();

            if (locked) {
                throw "PIPELINE-ERROR", "pipeline is already running";
            }

            if (!pmap."0".nodes) {
                throw "PIPELINE-ERROR", "pipeline has no elements";
            }

            locked = True;
        }
        # unlock on exit
        on_exit {
            lck.lock();
            on_exit lck.unlock();

            locked = False;
        }

        if (do_bulk) {
            AbstractDataProviderBulkRecordInterface bulk = input.getBulkApi();
            while (*hash<auto> recs = bulk.getValue()) {
                pmap."0".submit(recs);
            }
        } else {
            while (input.next()) {
                pmap."0".submit(input.getValue());
            }
        }
    }

    #! Waits for all queues in all pipelines to be empty
    waitDone() {
        lck.lock();
        on_exit lck.unlock();

        map $1.waitDone(), pmap.iterator();
    }

    #! Logs to the info log, if set
    logInfo(string fmt) {
        if (info_log) {
            softlist<auto> args += fmt;
            if (argv) {
                args += argv;
            }
            call_function_args(info_log, args);
        }
    }

    #! Logs to the error log, if set
    logError(string fmt) {
        if (error_log) {
            softlist<auto> args += fmt;
            if (argv) {
                args += argv;
            }
            call_function_args(error_log, args);
        }
    }

    #! Logs to the debug log, if set
    logDebug(string fmt) {
        if (debug_log) {
            softlist<auto> args += fmt;
            if (argv) {
                args += argv;
            }
            call_function_args(debug_log, args);
        }
    }

    #! Check if the given pipeline exists
    private checkUpdatePipelineIntern(int id) {
        checkLockedIntern();
        if (!pmap{id}) {
            throw "PIPELINE-ERROR", sprintf("pipeline %d does not exist; known pipelines: %y", id, (map $1.toInt(), keys pmap));
        }
    }

    #! Throws an exception if the pipeline is locked
    private checkLockedIntern() {
        if (locked) {
            throw "PIPELINE-ERROR", "no changes can be made to a locked pipeline";
        }
    }

    #! Creates a pipeline and returns its ID
    /** @throw PIPELINE-ERROR the pipeline is locked

        must be called with the pipeline lock held
    */
    private PipelineInfo createPipelineIntern(*int id) {
        PipelineInfo info(self, id, 1);
        int id = info.getId();
        pmap{id} = info;
        return info;
    }
}
}
