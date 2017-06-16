#!/usr/bin/env qore
# -*- mode: qore; indent-tabs-mode: nil -*-

%new-style
%enable-all-warnings
%require-types
%strict-args
%allow-debugging

%requires ../../../../qlib/QUnit.qm

const ST_ATTACH = 1;
const ST_BLOCK = 2;
const ST_STEP = 3;
const ST_FUNC_ENTER = 4;
const ST_FUNC_EXIT = 5;
const ST_EXCEPTION = 6;
const ST_DETACH = 7;

const stResolve = (
  ST_ATTACH: "onAttach",
  ST_BLOCK: "onBlock",
  ST_STEP: "onStep",
  ST_FUNC_ENTER: "onFunctionEnter",
  ST_FUNC_EXIT: "onFunctionExit",
  ST_EXCEPTION: "onException",
  ST_DETACH: "onDetach",
);

class MyDebugProgram inherits DebugProgram {
  public {
    list traceLog;
    list actions;
    *softlist attach_only_tids;
  }

  constructor(): DebugProgram() {
    printf("MyDebugProgram::constructor()\n");
  }

  nothing setActions(list act, *softlist attach_only) {
    traceLog = ();
    actions = act;
    attach_only_tids = attach_only;
  }
  any getAction(int func) {
    if (!actions) return;
    #printf("Act: %N\n", actions);
    if (!exists actions[0].func || actions[0].func == func) {
    #printf("Act2: %N\n", actions);
        return actions[0];
    }
  }
  handle(int func, Program pgm, reference sb, *string statementId, *hash extra) {
    *hash location;
    if (statementId)
      location = pgm.getStatementIdInfo(statementId);
    #printf("%y: pgm: %y, %y, %y, %y\n", func, pgm, sb, location, extra);
    hash it = (
      'func': stResolve{func},
      'file': location.file,
      'line': location.start_line,
      'tid': gettid(),
    );
    any act = getAction(func);
    if (exists act.localVars) {
      hash v = get_local_vars(2); # get local vars from program being debugged
      hash vv;
      foreach string lv in (act.localVars) {
        vv{lv}= v{lv}.value;
      }
      it.localVars = vv;
    }
    it += extra;
    push traceLog, it;
    if (exists act) {
      shift actions;
      if (act.typeCode() == NT_HASH) {
        if (exists act.sb)
          sb = act.sb;
      } else {
        sb = act;
      }
    }
#printf("it:%y, sb:%d, act:%y\n", it, sb, act);
  }

  onAttach(Program pgm, reference sb) {
    if (attach_only_tids && !attach_only_tids.contains(gettid())) {
        #printf("Avoid attach %d\n", gettid());
        sb = DebugDetach;
    } else {
        handle(ST_ATTACH, pgm, \sb);
    }
  }

  onDetach(Program pgm, reference sb) {
    handle(ST_DETACH, pgm, \sb);
  }

  onStep(Program pgm, string blockStatementId, *string statementId, reference retCode, reference sb) {
    int st = statementId ? ST_STEP : ST_BLOCK;
    any act = getAction(st);
    if (exists act.retCode) {
        #printf("loc:%y, retCode: %y\n", statementId, act.retCode);
        retCode = act.retCode;
    }
    handle(st, pgm, \sb, statementId ?? blockStatementId);
  }

  onFunctionEnter(Program pgm, string statementId, reference sb) {
    handle(ST_FUNC_ENTER, pgm, \sb, statementId);
  }

  onFunctionExit(Program pgm, string statementId, reference returnValue, reference sb) {
    any act = getAction(ST_FUNC_EXIT);
    if (exists act.retVal) {
        returnValue = act.retVal;
    }
    handle(ST_FUNC_EXIT, pgm, \sb, statementId, ("retVal": returnValue));
  }

  onException(Program pgm, string statementId, hash ex, reference dismiss, reference sb) {
    any act = getAction(ST_EXCEPTION);
    if (exists act.dismiss) {
        dismiss = act.dismiss;
    }
    handle(ST_EXCEPTION, pgm, \sb, statementId, ("ex": ex));
  }

}

%exec-class DebugTest

class DebugTest inherits QUnit::Test {
    public {}


    private {
        hash stack;
        hash line;
        int main_tid;
        Mutex semaphore;
        Program thisProgram;
        MyDebugProgram debugProgram;
    }

    constructor() : Test("Debug Test", "1.0") {
        addTestCase("ProgramTest", \programTest());
        addTestCase("StatementTest", \statementTest());
        addTestCase("BreakpointTest", \breakpointTest());
        addTestCase("SingleThreadDebugTest", \singleThreadDebugTest());
        addTestCase("SingleThreadBreakpointTest", \singleThreadBreakpointTest());
        addTestCase("MultiThreadBreakTest", \multiThreadBreakTest());
        #addTestCase("MultiThreadDebugTest", \multiThreadDebugTest());

        thisProgram = Program::getProgram();
        debugProgram = new MyDebugProgram();
        semaphore = new Mutex();
        main_tid = gettid();
        stack = tstGetStack();
        line.tstFunction2 = stack.line+1;
        line.tstFunction = line.tstFunction2+7;
        line.tstNoRetVal = line.tstFunction+12;
        line.tstLoop = line.tstNoRetVal+2;
        line.tstProgram = line.tstLoop+9;
        line.tstBreakProgram = line.tstProgram+14;
        #printf("line: %N\n", line);
        set_return_value(main());
    }

    # defined stuff to be executed and steps inspected by debugger
    # do not  add/delete lines
    hash tstGetStack() {return get_all_thread_call_stacks(){gettid()}[0];}
    int tstFunction2(int recurseCnt) {
      if (recurseCnt > 0) {
        return tstFunction2(recurseCnt-1)+1;
      } else {
        return 0;
      }
    }
    string tstFunction(int i, string s) {
      if (i > 0) {
        s += sprintf(",i:%d", i);
      } else {
        s = "";
      }
      while (i > 0) {
        i--;
      }
      tstFunction2(3);
      return "DONE";
    }
    tstNoRetVal() {
    }
    int tstLoop() {
        int res = 0;
        int i = 3;
        while (i > 0) {
            res += i;
            i--;
        }
        return res;
    }
    any tstProgram() {
      int i = tstLoop();
      string s = i.type();
      int tid = gettid();
      string ret = tstFunction(1, "Func");
      tstNoRetVal();
      s = sprintf("%s", ret);  # just test if ret do not coredumps
      try {
        i = (tid+s.size()+ret.size()) / (tid-tid);
      } catch (hash ex) {
        s = ex.err;
      }
      return s;
    }
    bool tstBreakProgram() {
        int max_ms = 1000;
        int i=0;
        int stamp = clock_getmillis();
        while (clock_getmillis() - stamp < max_ms) {
            i++;
        }
        return clock_getmillis() - stamp < max_ms;
    }
    # end of tested program


    assertTraceLog(string name, list traceLog) {
    #printf("actuallog: %N\n", debugProgram.traceLog);
        hash l = shift debugProgram.traceLog;
        testAssertionValue(name+':method check', l.func, "onAttach");

        int i = 0;
        while (my *hash el = shift traceLog) {
            *hash al = shift debugProgram.traceLog;
            if (!exists al) {
                testAssertionValue(name+':tracelog missing steps', el + traceLog, ());
                break;
            }
            if (exists el.tid) {
                testAssertionValue(sprintf(name+':tracelog %d tid', i), al.tid, el.tid);
            }
            if (exists el.func) {
                testAssertionValue(sprintf(name+':tracelog %d func', i), al.func, stResolve{el.func});
            }
            if (exists el.line) {
                testAssertionValue(sprintf(name+':tracelog %d line', i), al.line, el.line);
            }
            if (exists el.file) {
                testAssertionValue(sprintf(name+':tracelog %d file', i), al.file, el.file);
            }
            if (exists el.retVal) {
                testAssertionValue(sprintf(name+':tracelog %d retval', i), al.retVal, el.retVal);
            }
            if (exists el.localVars) {
                testAssertionValue(sprintf(name+':tracelog %d local vars', i), al.localVars, el.localVars);
            }
            if (exists el.ex) {
                testAssertionValue(sprintf(name+':tracelog %d exception err', i), al.ex.err, el.ex);
            }
            i++;
        }
        testAssertionValue(sprintf(name+':tracelog end check, i:%d', i), debugProgram.traceLog, ());
    }
    checkProgram(string name, list actions, list traceLog, any expectedRet) {
        printf("checkProgram(%y)\n", name);
        debugProgram.setActions(actions);
#printf("Actions: %N\n", debugProgram.actions);
        debugProgram.addProgram(thisProgram);
        any ret = tstProgram();
        debugProgram.removeProgram(thisProgram);
#printf("stack: %N\n", stack);
#printf("expected log: %N\n", traceLog);
        assertTraceLog(name, traceLog);
        testAssertionValue(sprintf(name+':return value'), ret, expectedRet);
        list l = debugProgram.getAllPrograms();
        testAssertionValue(sprintf(name+':getAllPrograms size 0'), l.size(), 0);
        debugProgram.addProgram(thisProgram);
        l = debugProgram.getAllPrograms();
        testAssertionValue(sprintf(name+':getAllPrograms size 1'), l.size(), 1);
        testAssertionValue(sprintf(name+':getAllPrograms programid'), l[0].getProgramId(), thisProgram.getProgramId());
        debugProgram.removeProgram(thisProgram);
        l = debugProgram.getAllPrograms();
        testAssertionValue(sprintf(name+':getAllPrograms size 0'), l.size(), 0);
    }

    debugBreakThread(int tid) {
        #printf("debugBreakThread running %d\n", tid);
        #printf("debugBreakThread addProgram\n");
        debugProgram.addProgram(thisProgram);
        usleep(100ms);  # leave some time to reach loop block
        if (tid >= 0) {
            #printf("debugBreakThread breakProgramThread(%d)\n", tid);
            debugProgram.breakProgramThread(thisProgram, tid);
        } else {
            #printf("debugBreakThread breakProgram()\n");
            debugProgram.breakProgram(thisProgram);
        }
        #printf("debugBreakThread waiting for program finish\n");
        if (semaphore.lock(1200ms) < 0) {  # waith till tst program finishes because of break infinite loop  etc.
            printf("ERROR: semaphore locked\n");
        } else {
            #printf("debugBreakThread done unlock itself\n");
            semaphore.unlock();
        }
        debugProgram.removeProgram(thisProgram);
    }

    checkBreakProgram(string name, int tid, bool breakRes) {
        printf("checkBreakProgram(%d)\n", tid);
        debugProgram.setActions(list(DebugRun, ("retCode": DebugBlockBreak, "sb": DebugDetach)), main_tid);
        semaphore.lock();
        background debugBreakThread(tid);
        testAssertionValue(sprintf(name+':program finished'), tstBreakProgram(), breakRes);
        semaphore.unlock();
        usleep(100ms);  # leave some time to finish thread
        if (breakRes) {
            assertTraceLog(name, list((/*SB_BLOCK || SB_STEP*/"tid": main_tid, "line": line.tstBreakProgram+5)));
        } else {
            assertTraceLog(name, list());
        }
    }

    singleThreadDebugTest() {
        list traceLog = (
            ("func": ST_STEP, "tid": main_tid, "file": stack.file),  # 0
            ("func": ST_FUNC_ENTER, "line": line.tstProgram+1),
            ("func": ST_BLOCK, "line": line.tstProgram+1),
            ("func": ST_STEP, "line": line.tstProgram+1),
            ("func": ST_FUNC_ENTER, "line": line.tstLoop+1),
            ("func": ST_BLOCK, "line": line.tstLoop+1),
            ("func": ST_STEP, "line": line.tstLoop+1),
            ("func": ST_STEP, "line": line.tstLoop+2),
            ("func": ST_STEP, "line": line.tstLoop+3), # while
            ("func": ST_BLOCK, "line": line.tstLoop+4),
            ("func": ST_STEP, "line": line.tstLoop+4), #10
            ("func": ST_STEP, "line": line.tstLoop+5),
            ("func": ST_BLOCK, "line": line.tstLoop+4),
            ("func": ST_STEP, "line": line.tstLoop+4),
            ("func": ST_STEP, "line": line.tstLoop+5),
            ("func": ST_BLOCK, "line": line.tstLoop+4),
            ("func": ST_STEP, "line": line.tstLoop+4),
            ("func": ST_STEP, "line": line.tstLoop+5),
            ("func": ST_STEP, "line": line.tstLoop+7),  # ret
            ("func": ST_FUNC_EXIT, "line": line.tstLoop+1),
            ("func": ST_STEP, "line": line.tstProgram+2),  #20
            ("func": ST_STEP, "line": line.tstProgram+3),
            ("func": ST_STEP, "line": line.tstProgram+4),
            ("func": ST_FUNC_ENTER, "line": line.tstFunction+1),
            ("func": ST_BLOCK, "line": line.tstFunction+1),
            ("func": ST_STEP, "line": line.tstFunction+1),
            ("func": ST_BLOCK, "line": line.tstFunction+2),
            ("func": ST_STEP, "line": line.tstFunction+2),
            ("func": ST_STEP, "line": line.tstFunction+6),
            ("func": ST_BLOCK, "line": line.tstFunction+7), # while
            ("func": ST_STEP, "line": line.tstFunction+7), #30
            ("func": ST_STEP, "line": line.tstFunction+9),
            ("func": ST_FUNC_ENTER, "line": line.tstFunction2+1),  #3
            ("func": ST_BLOCK, "line": line.tstFunction2+1),
            ("func": ST_STEP, "line": line.tstFunction2+1),
            ("func": ST_BLOCK, "line": line.tstFunction2+2),
            ("func": ST_STEP, "line": line.tstFunction2+2),
            ("func": ST_FUNC_ENTER, "line": line.tstFunction2+1),  #2
            ("func": ST_BLOCK, "line": line.tstFunction2+1),
            ("func": ST_STEP, "line": line.tstFunction2+1),
            ("func": ST_BLOCK, "line": line.tstFunction2+2), #40
            ("func": ST_STEP, "line": line.tstFunction2+2),
            ("func": ST_FUNC_ENTER, "line": line.tstFunction2+1),  #1
            ("func": ST_BLOCK, "line": line.tstFunction2+1),
            ("func": ST_STEP, "line": line.tstFunction2+1),
            ("func": ST_BLOCK, "line": line.tstFunction2+2),
            ("func": ST_STEP, "line": line.tstFunction2+2),
            ("func": ST_FUNC_ENTER, "line": line.tstFunction2+1),  #0
            ("func": ST_BLOCK, "line": line.tstFunction2+1),
            ("func": ST_STEP, "line": line.tstFunction2+1),
            ("func": ST_BLOCK, "line": line.tstFunction2+4), #50
            ("func": ST_STEP, "line": line.tstFunction2+4),
            ("func": ST_FUNC_EXIT, "line": line.tstFunction2+1, "retVal": 0), # 0
            ("func": ST_FUNC_EXIT, "line": line.tstFunction2+1, "retVal": 1), # 1
            ("func": ST_FUNC_EXIT, "line": line.tstFunction2+1, "retVal": 2), # 2
            ("func": ST_FUNC_EXIT, "line": line.tstFunction2+1, "retVal": 3), # 3
            ("func": ST_STEP, "line": line.tstFunction+10),
            ("func": ST_FUNC_EXIT, "line": line.tstFunction+1, "retVal": "DONE"),
            ("func": ST_STEP, "line": line.tstProgram+5),
            ("func": ST_FUNC_ENTER, "line": line.tstNoRetVal+0),  # when empty block then func and block has start_line exceptionally one line up
            ("func": ST_BLOCK, "line": line.tstNoRetVal+0),  #60
            ("func": ST_FUNC_EXIT, "line": line.tstNoRetVal+0),
            ("func": ST_STEP, "line": line.tstProgram+6),
            ("func": ST_STEP, "line": line.tstProgram+7),
            ("func": ST_BLOCK, "line": line.tstProgram+8),
            ("func": ST_STEP, "line": line.tstProgram+8),
            ("func": ST_EXCEPTION, "line": line.tstProgram+8, "ex": "DIVISION-BY-ZERO"),
            ("func": ST_BLOCK, "line": line.tstProgram+10),
            ("func": ST_STEP, "line": line.tstProgram+10),
            ("func": ST_STEP, "line": line.tstProgram+12),
            ("func": ST_FUNC_EXIT, "line": line.tstProgram+1),  #70
            ("func": ST_STEP),
        );
        list l;
        checkProgram("full trace", list(DebugStep), traceLog, "DIVISION-BY-ZERO");
        checkProgram("full trace dismiss exception", list(DebugStep, ("func": ST_EXCEPTION, "dismiss": True) ), splice(traceLog, 67, 2), "DONE");
        checkProgram("until return", list(DebugStep, ("func": ST_FUNC_ENTER), ("func": ST_FUNC_ENTER), ("func": ST_FUNC_ENTER, "sb": DebugUntilReturn), DebugStep), splice(traceLog, 24, 33), "DIVISION-BY-ZERO");
        l = splice(traceLog, 0, 66); # remove remove till exception
        l = splice(l, 1, 3);  # till func exit
        l = splice(l, 2);
        checkProgram("run till exception modify retVal", list(DebugRun, ("func": ST_EXCEPTION, "sb": DebugUntilReturn), ("func": ST_FUNC_EXIT, "retVal": "MODIFIED", "sb": DebugRun)), l, "MODIFIED");
        l = splice(traceLog, 4, 16); # remove tstLoop
        l = splice(l, 7, 35); # remove tstFunction
        l = splice(l, 8, 3);  # remove tstNoRetVal
        checkProgram("stepover", list(DebugStep, ("func": ST_FUNC_ENTER, "sb": DebugStepOver)), l, "DIVISION-BY-ZERO");
        l = splice(traceLog, 4, 16); # remove tstLoop
        l = splice(l, 7, 47);
        checkProgram("return", list(DebugStep, ("func": ST_FUNC_ENTER), DebugStep, DebugStepOver, DebugStep, DebugStep, ("func": ST_STEP, "retCode": DebugBlockReturn, "sb": DebugStep)), l, NOTHING);
        l = splice(traceLog, 20);
        l = splice(l, 15, 4);
        checkProgram("loop continue", list(DebugStep, ("func": ST_FUNC_ENTER), ("func": ST_FUNC_ENTER),
            ("func": ST_BLOCK), # in loop now i = 3  ... add two steps via continue
            ("func": ST_BLOCK), DebugStep, ("retCode": DebugBlockContinue),  # res+3, skip i--
            ("func": ST_BLOCK), DebugStep, ("retCode": DebugBlockContinue, "sb": DebugUntilReturn),  # res+3, skip i--, # res+3+2+1
            ("func": ST_FUNC_EXIT, "retVal": 15, "sb": DebugDetach)),
            l, "DIVISION-BY-ZERO");
        l = splice(traceLog, 20);
        l = splice(l, 12, 7);
        checkProgram("loop break", list(DebugStep, ("func": ST_FUNC_ENTER), ("func": ST_FUNC_ENTER),
            ("func": ST_BLOCK), # in loop now i = 3
            ("func": ST_BLOCK), DebugStep, ("retCode": DebugBlockBreak, "sb": DebugUntilReturn),  # res+3, skip i--
            ("func": ST_FUNC_EXIT, "retVal": 3, "sb": DebugDetach)),
            l, "DIVISION-BY-ZERO");
        checkProgram("detach", list(DebugStep, ("func": ST_FUNC_ENTER, "sb": DebugDetach)), splice(traceLog, 2), "DIVISION-BY-ZERO");
        /*l = ();
        for (my int i=0; i<traceLog.size()-2; i++) {  # why -2 ... exception has no checkAttach
            push l, ("func": ST_ATTACH);
        }*/
        checkProgram("do not attach", list(DebugDetach), list(), "DIVISION-BY-ZERO");
    }

    singleThreadBreakpointTest() {
        list traceLog2 = (
            ("func": ST_STEP, "line": line.tstFunction+1),
            ("func": ST_FUNC_EXIT, "line": line.tstFunction+1),
            ("func": ST_STEP, "line": line.tstProgram+5, "localVars": ("tid": gettid(), "s": "integer", "ret": "MODIFIED")),
        );
        list action2 = list(DebugRun, ("sb": DebugUntilReturn), ("func": ST_FUNC_EXIT, "retVal": "MODIFIED", "sb": DebugRun), ("localVars": ("tid", "s", "ret"), "sb": DebugDetach) );

        Breakpoint b1 = new Breakpoint();
        b1.setEnabled(True);
        b1.addThreadId(gettid());
        b1.assignProgram(thisProgram);
        b1.assignStatement(thisProgram.findStatementId('', line.tstProgram+5));

        Breakpoint b2 = new Breakpoint();;
        b2.setEnabled(True);
        b2.addThreadId(gettid());
        b2.assignProgram(thisProgram);
        b2.assignStatement(thisProgram.findStatementId('', line.tstFunction+1));

        checkProgram("breakpoint b1(enable), b2(enable)", action2, traceLog2, "DIVISION-BY-ZERO");
        # disable breakpoint 1
        list traceLog3 = (
            ("func": ST_STEP, "line": line.tstProgram+5, "localVars": ("tid": gettid(), "s": "integer", "ret": "DONE"), "sb": DebugDetach),
        );
        list action3 = list(DebugRun, ("localVars": ("tid", "s", "ret"), "sb": DebugDetach) );
        b2.setEnabled(False);
        checkProgram("breakpoint b1(enable), b2(dísable)", action3, traceLog3, "DIVISION-BY-ZERO");
        b2.setEnabled(True);
        b2.setPolicy(BreakpointPolicyReject);
        checkProgram("breakpoint b1(enable), b2(reject)", action3, traceLog3, "DIVISION-BY-ZERO");
        b2.clearThreadIds();
        b2.addThreadId(gettid()+1000);
        b2.setPolicy(BreakpointPolicyAccept);
        checkProgram("breakpoint b1(enable), b2(not accept)", action3, traceLog3, "DIVISION-BY-ZERO");
        b2.addThreadId(gettid());
        checkProgram("breakpoint b1(enable), b2(accept)", action2, traceLog2, "DIVISION-BY-ZERO");

        # breakpoint group
        b2.assignStatement(thisProgram.findStatementId('', line.tstFunction+6));
        b2.assignStatement(thisProgram.findStatementId('', line.tstFunction+9));
        checkProgram("breakpoint b1(enable), b2(accept,group3)", splice(action2, 1, 0, (DebugRun, DebugRun)), splice(traceLog2, 1, 0, (("func": ST_STEP, "line": line.tstFunction+6), ("func": ST_STEP, "line": line.tstFunction+9))), "DIVISION-BY-ZERO");        
        b2.unassignStatement(thisProgram.findStatementId('', line.tstFunction+9));
        checkProgram("breakpoint b1(enable), b2(accept,group2)", splice(action2, 1, 0, list(DebugRun)), splice(traceLog2, 1, 0, (("func": ST_STEP, "line": line.tstFunction+6))), "DIVISION-BY-ZERO");
        b2.setEnabled(False);
        checkProgram("breakpoint b1(enable), b2(disable,group)", action3, traceLog3, "DIVISION-BY-ZERO");

        testAssertionValue('getBreakpointId', b1.getBreakpointId().substr(0,2), "0x");
        testAssertionValue('getBreakpointId compare', b1.getBreakpointId() == b2.getBreakpointId(), False);
        assertThrows('BREAKPOINT-ERROR', \Breakpoint::resolveBreakpointId(), ("0x000000"), '.resolveBreakpointId() no breakpoint');
        testAssertionValue('resolveBreakpointId', Breakpoint::resolveBreakpointId(b1.getBreakpointId()).getBreakpointId(), b1.getBreakpointId());
    }

    multiThreadBreakTest() {
        checkBreakProgram("break program", -1, True);
        checkBreakProgram("break thread", gettid(), True);
        checkBreakProgram("break unknown thread", gettid()+10, False);
    }

    multiThreadDebugTest() {
    }

    breakpointTest() {
        string name = "Breakpoint::";
        Breakpoint b = new Breakpoint();
        testAssertionValue(name+'getEnabled() default', b.getEnabled(), False);
        b.setEnabled(True);
        testAssertionValue(name+'setEnabled(True)', b.getEnabled(), True);
        testAssertionValue(name+'getPolicy() default', b.getPolicy(), BreakpointPolicyNone);
        b.setPolicy(BreakpointPolicyAccept);
        testAssertionValue(name+'setPolicy(BreakpointPolicyAccept)', b.getPolicy(), BreakpointPolicyAccept);
        b.setPolicy(BreakpointPolicyReject);
        testAssertionValue(name+'setPolicy(BreakpointPolicyReject)', b.getPolicy(), BreakpointPolicyReject);

        testAssertionValue(name+'getThreadIds()', b.getThreadIds(), ());
        b.setThreadIds(list(1,2,3,4,5,6,7,8,9,10,20,21,30));
        testAssertionValue(name+'setThreadIds((1,2,3,4,5,6,7,8,9,10,20,21,30))', b.getThreadIds(), list(1,2,3,4,5,6,7,8,9,10,20,21,30));
        testAssertionValue(name+'isThreadId(0)', b.isThreadId(0), False);
        testAssertionValue(name+'isThreadId(1)', b.isThreadId(1), True);
        testAssertionValue(name+'isThreadId(5)', b.isThreadId(5), True);
        testAssertionValue(name+'isThreadId(10)', b.isThreadId(10), True);
        testAssertionValue(name+'isThreadId(11)', b.isThreadId(11), False);
        testAssertionValue(name+'isThreadId(10)', b.isThreadId(21), True);
        testAssertionValue(name+'isThreadId(10)', b.isThreadId(30), True);

        Breakpoint b2 = b;  # copy constructor test
        testAssertionValue(name+'setEnabled(True) copy', b2.getEnabled(), True);
        testAssertionValue(name+'setPolicy(BreakpointPolicyReject) copy', b2.getPolicy(), BreakpointPolicyReject);
        testAssertionValue(name+'setThreadIds((1,2,3,4,5,6,7,8,9,10,20,21,30)) copy', b2.getThreadIds(), list(1,2,3,4,5,6,7,8,9,10,20,21,30));

        b.setEnabled(False);
        testAssertionValue(name+'setEnabled(False)', b.getEnabled(), False);
        b.setPolicy(BreakpointPolicyNone);
        testAssertionValue(name+'setPolicy(BreakpointPolicyNone)', b.getPolicy(), BreakpointPolicyNone);

        b.addThreadId(1);
        testAssertionValue(name+'addThreadId(1)', b.getThreadIds(), list(1,2,3,4,5,6,7,8,9,10,20,21,30));
        b.addThreadId(30);
        testAssertionValue(name+'addThreadId(30)', b.getThreadIds(), list(1,2,3,4,5,6,7,8,9,10,20,21,30));
        b.addThreadId(40);
        testAssertionValue(name+'addThreadId(40)', b.getThreadIds(), list(1,2,3,4,5,6,7,8,9,10,20,21,30,40));
        b.addThreadId(39);
        testAssertionValue(name+'addThreadId(39)', b.getThreadIds(), list(1,2,3,4,5,6,7,8,9,10,20,21,30,39,40));
        b.addThreadId(41);
        testAssertionValue(name+'addThreadId(41)', b.getThreadIds(), list(1,2,3,4,5,6,7,8,9,10,20,21,30,39,40,41));
        testAssertionValue(name+'isThreadId(39)', b.isThreadId(39), True);
        testAssertionValue(name+'isThreadId(40)', b.isThreadId(40), True);
        testAssertionValue(name+'isThreadId(41)', b.isThreadId(41), True);
        b.removeThreadId(5);
        testAssertionValue(name+'removeThreadId(5)', b.getThreadIds(), list(1,2,3,4,6,7,8,9,10,20,21,30,39,40,41));
        b.removeThreadId(30);
        testAssertionValue(name+'removeThreadId(30)', b.getThreadIds(), list(1,2,3,4,6,7,8,9,10,20,21,39,40,41));
        b.removeThreadId(41);
        testAssertionValue(name+'removeThreadId(41)', b.getThreadIds(), list(1,2,3,4,6,7,8,9,10,20,21,39,40));
        b.removeThreadId(39);
        testAssertionValue(name+'removeThreadId(39)', b.getThreadIds(), list(1,2,3,4,6,7,8,9,10,20,21,40));
        b.addThreadId(5);
        testAssertionValue(name+'addThreadId(30)', b.getThreadIds(), list(1,2,3,4,5,6,7,8,9,10,20,21,40));

        b.setThreadIds((1));
        testAssertionValue(name+'setThreadIds((1))', b.getThreadIds(), list(1));

        b.clearThreadIds();
        testAssertionValue(name+'clearThreadIds()', b.getThreadIds(), ());

        b.assignProgram(thisProgram);
        b.setThreadIds((1,2));
        testAssertionValue(name+'setThreadIds((1))', b.getThreadIds(), list(1,2));

        testAssertionValue(name+'getStatementIds(())', b.getStatementIds(), list());

        string sid1 = thisProgram.findStatementId('', line.tstFunction2+1);
        string sid2 = thisProgram.findStatementId('', line.tstFunction+1);
        b.assignStatement(sid1);
        testAssertionValue(name+'getStatementIds(())', b.getStatementIds(), list(sid1));
        b.unassignStatement(sid1);
        testAssertionValue(name+'getStatementIds(())', b.getStatementIds(), list());
        b.assignStatement(sid1);
        b.assignStatement(sid2);
        testAssertionValue(name+'getStatementIds(())', b.getStatementIds(), list(sid1, sid2));

        b.unassignProgram();
        testAssertionValue(name+'getStatementIds(())', b.getStatementIds(), list());

        b.assignProgram(thisProgram);
        testAssertionValue(name+'setThreadIds((1))', b.getThreadIds(), list(1,2));

        b.setThreadIds(list());
        testAssertionValue(name+'setThreadIds(())', b.getThreadIds(), list());

        /*
        testAssertionValue(name+'setPolicy(BreakpointPolicyNone)', b.getPolicy(), BreakpointPolicyNone);
        assertThrows('BREAKPOINT-ERROR', \b.getThreadIds(), (), name+'.getThreadIds() no program');

        b.assignProgram(thisProgram); */
    }

    statementTest() {
        string name = "Program::";
        Program p();
        # stress statement test
        p.parsePending(
         "*string sub f1(int p, string s) {
            int a; int b;  #2
            a = 1; b = 2;  #3
            {
              a = a + b; doSleep(10); #5
              {
                if (p > 0) {  #7
                  b = 1+  #8
                      1+
                      1+
                      1+
                      1;  #12
                } #13
              }
            }
            return sprintf(\"%s,%d,%d,%d\", s, p, a, b);  #16
          }",
          "label-f1", NOTHING, "/usr/lib/my-source.q", 10, False);
        p.parse(
         "sub doSleep(int ms) {
            sleep(ms); #2
          }", "label-sleep", NOTHING, "/usr/lib/my-source.q", 100, False);
        p.parse("sub main() { f1(10, 'MAIN'); }", "main.q", NOTHING, NOTHING, NOTHING, False);
        p.parseCommit();

        assertThrows('PROGRAM-STATEMENT-ERROR', \p.findStatementId(), ("main.q", 10), name+'findStatementId("main.q", 10) line beyond');
        assertThrows('PROGRAM-STATEMENT-ERROR', \p.findStatementId(), ("non-exist.q", 1), name+'findStatementId("no-exist.q", 1) non exist');

        string sid;
        sid = p.findStatementId("main.q", 1);
        testAssertionValue(name+'findStatementId("main.q", 1)', sid.size() > 0, True);

        hash info;
        info = p.getStatementIdInfo(sid);
        testAssertionValue(name+'getStatementIdInfo("main.q", 1)', info, ('file': 'main.q', 'source': '', 'offset': 0, 'start_line': 1, 'end_line': 1) );

        sid = p.findStatementId("label-f1", 2);
        testAssertionValue(name+'findStatementId("label-f1", 2) label match', sid.size() > 0, True);

        testAssertionValue(name+'findStatementId("/usr/lib/my-source.q", 12) full file', p.findStatementId("/usr/lib/my-source.q", 12), sid);
        testAssertionValue(name+'findStatementId("/lib/my-source.q", 12) suffix', p.findStatementId("/lib/my-source.q", 12), sid);
        testAssertionValue(name+'findStatementId("lib/my-source.q", 12) suffix', p.findStatementId("lib/my-source.q", 12), sid);
        testAssertionValue(name+'findStatementId("my-source.q", 12) suffix', p.findStatementId("my-source.q", 12), sid);
        assertThrows('PROGRAM-STATEMENT-ERROR', \p.findStatementId(), ("ib/my-source.q", 102), name+'findStatementId("ib/my-source.q", 12) no suffix match');

        info = p.getStatementIdInfo(sid);
        testAssertionValue(name+'getStatementIdInfo("label-f1", 1)', info, ('file': 'label-f1', 'source': '/usr/lib/my-source.q', 'offset': 10, 'start_line': 2, 'end_line': 2) );


        sid = p.findStatementId("label-f1", 8);
        testAssertionValue(name+'findStatementId("label-f1", 8)', sid.size() > 0, True);
        testAssertionValue(name+'findStatementId("label-f1", 11) multiline', p.findStatementId("label-f1", 11), sid);
        info = p.getStatementIdInfo(sid);
        testAssertionValue(name+'getStatementIdInfo("label-f1", 11)', info, ('file': 'label-f1', 'source': '/usr/lib/my-source.q', 'offset': 10, 'start_line': 8, 'end_line': 12) );

        sid = p.findStatementId("label-sleep", 2);
        testAssertionValue(name+'findStatementId("label-sleep", 2)', sid.size() > 0, True);
        testAssertionValue(name+'findStatementId("/usr/lib/my-source.q", 102) full file', p.findStatementId("/usr/lib/my-source.q", 102), sid);
        info = p.getStatementIdInfo(sid);
        testAssertionValue(name+'getStatementIdInfo("label-sleep", 2)', info, ('file': 'label-sleep', 'source': '/usr/lib/my-source.q', 'offset': 100, 'start_line': 2, 'end_line': 2) );
    }

    programTest() {
        string name = "Program::";
        list l = Program::getAllPrograms();
        testAssertionValue(name+'getAllPrograms', l.size(), 3);
        testAssertionValue(name+'getProgramId', thisProgram.getProgramId().substr(0,2), "0x");
        testAssertionValue(name+'getProgramId 2', l[0].getProgramId(), thisProgram.getProgramId());
        assertThrows('PROGRAM-ERROR', \Program::resolveProgramId(), ("0x000000"), name+'.resolveProgramId() no program');
        testAssertionValue(name+'resolveProgramId', Program::resolveProgramId(thisProgram.getProgramId()).getProgramId(), thisProgram.getProgramId());
    }
}

