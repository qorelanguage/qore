#!/usr/bin/env qore
# -*- mode: qore; indent-tabs-mode: nil -*-

%new-style
%enable-all-warnings
%require-types
%strict-args

#%requires ../../../../qlib/Util.qm
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
    list log;
    list actions;
  }

  constructor(): DebugProgram() {
    printf("MyDebugProgram::constructor()\n");
  }

  nothing setActions(list act) {
    log = ();
    actions = act;
  }
  any getAction(int func) {
    if (!actions) return;
    printf("Act: %N\n", actions);
    if (!exists actions[0].func || actions[0].func == func) {
    printf("Act2: %N\n", actions);
        return actions[0];
    }
  }
  handle(int func, Program pgm, reference sb, *hash location, *hash extra) {
    printf("%y: pgm: %y, %y, %y, %y\n", func, pgm, sb, location, extra);
    hash it = (
      'func': stResolve{func},
      'file': location.file,
      'line': location.start_line,
      'tid': gettid(),
    );
    it += extra;
    push log, it;
    any act = getAction(func);
    if (exists act) {
      shift actions;
      if (act.typeCode() == NT_HASH) {
        if (exists act.sb)
          sb = act.sb;
      } else {
        sb = act;
      }
    }
printf("it:%y, sb:%d, act:%y\n", it, sb, act);
  }

  onAttach(Program pgm, reference sb) {
    handle(ST_ATTACH, pgm, \sb);
  }

  onDetach(Program pgm, reference sb) {
    handle(ST_DETACH, pgm, \sb); 
  }

  onStep(Program pgm, hash blockLocation, *hash location, reference retCode, reference sb) {
    int st = location ? ST_STEP : ST_BLOCK;
    any act = getAction(st);
    if (exists act.retCode) {
        retCode = act.retCode;
    }
    handle(st, pgm, \sb, location ?? blockLocation);
  }

  onFunctionEnter(Program pgm, hash location, reference sb) {
    handle(ST_FUNC_ENTER, pgm, \sb, location);
  }

  onFunctionExit(Program pgm, hash location, reference returnValue, reference sb) {
    any act = getAction(ST_FUNC_EXIT);
    if (exists act.retVal) {
        returnValue = act.retVal;
    }
    handle(ST_FUNC_EXIT, pgm, \sb, location, ("retVal": returnValue));
  }

  onException(Program pgm, hash location, hash ex, reference dismiss, reference sb) {
    any act = getAction(ST_EXCEPTION);
    if (exists act.dismiss) {
        dismiss = act.dismiss;
    }
    handle(ST_EXCEPTION, pgm, \sb, location, ("ex": ex));
  }

}

# we need it global because of mutithread test
our int main_tid;
our Mutex semaphore();
our Program thisProgram;
our MyDebugProgram debugProgram;
/*
sub debugThread() {
    try {
      while (semaphore.trylock()==-1);
     , getting reference wrongly from onFunctionExit d.breakProgramThread(p, tid);

      printf("debugThread(),tid:%d\n", gettid());
      MyDebugProgram d();
      printf("debugThread(): addProgram(%N)\n", p);
      d.addProgram(p);
      printf("D0\n");
      d.breakProgramThread(p, tid);

      printf("D1\n");
      while (semaphore.trylock()==-1);
      printf("D2\n");

    } catch (hash ex) {
      printf("Ex: %N\n");
    }
    printf("debug thread exiting\n");
    semaphore.unlock();
}


printf("Running\n");
# get current program
our int tid = gettid();
our Program p(PO_DEFAULT, True);
printf("program:%N, tid:%d\n", p, gettid());
our Mutex semaphore();

semaphore.lock();
background debugThread();

printf("Debug thread should be running\n");

sleep(2);
for (my int i=1; i<10; i++) {
    printf("Counting: %y\n", i);

}

printf("Exiting\n");
semaphore.unlock();

sleep(1);  # TODO: more robust waitfortermination();
*/

%exec-class DebugTest

class DebugTest inherits QUnit::Test {
    public {}


    private {
        hash stack;
        hash line;
        #Program thisProgram;
        #DebugProgram debugProgram;
    }

    constructor() : Test("Debug Test", "1.0") {
        addTestCase("SingleThreadDebugTest1", \singleThreadDebugTest1());
        #addTestCase("MultiThreadDebugTest1", \multiThreadDebugTest1());

        thisProgram = new Program(PO_ATTACH);  # TODO: PO_xxx
        debugProgram = new MyDebugProgram();
        main_tid = gettid();
        stack = tstGetStack();
        line.tstFunction2 = stack.line+1;
        line.tstFunction = line.tstFunction2+7;
        line.tstNoRetVal = line.tstFunction+12;
        line.tstProgram = line.tstNoRetVal+2;
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
    any tstProgram() {
      int i = 1;
      string s = i.type();
      int tid = gettid();
      string ret = tstFunction(i, "Func");
      tstNoRetVal();
      s = sprintf("%s", ret);  # just test if ret do not coredumps
      try {
        i = (tid+s.size()+ret.size()) / (tid-tid);
      } catch (hash ex) {
        s = ex.err;
      }
      return s;
    }
    # end of tested program

    checkProgram(string name, list actions, list traceLog, any expectedRet) {
        printf("checkProgram(%y)\n", name);
        debugProgram.setActions(actions);
printf("Actions: %N\n", debugProgram.actions);
        debugProgram.addProgram(thisProgram);
        any ret = tstProgram();
        debugProgram.removeProgram(thisProgram);
printf("stack: %N\n", stack);
printf("expected log: %N\n", traceLog);
printf("actuallog: %N\n", debugProgram.log);
        hash l = shift debugProgram.log;
        testAssertionValue(name+':method check', l.func, "onAttach");

        int i = 0;
        while (my *hash el = shift traceLog) {
            *hash al = shift debugProgram.log;
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
            if (exists el.ex) {
                testAssertionValue(sprintf(name+':tracelog %d exception err', i), al.ex.err, el.ex);
            }
            i++;
        }
        testAssertionValue(sprintf(name+':tracelog end check, i:%d', i), debugProgram.log, ());
        testAssertionValue(sprintf(name+':return value', i), ret, expectedRet);
    }

    singleThreadDebugTest1() {
        list traceLog = (
            ("func": ST_STEP, "tid": main_tid, "file": stack.file),
            ("func": ST_FUNC_ENTER, "line": line.tstProgram+1),
            ("func": ST_BLOCK, "line": line.tstProgram+1),
            ("func": ST_STEP, "line": line.tstProgram+1),
            ("func": ST_STEP, "line": line.tstProgram+2),
            ("func": ST_STEP, "line": line.tstProgram+3),
            ("func": ST_STEP, "line": line.tstProgram+4),
            ("func": ST_FUNC_ENTER, "line": line.tstFunction+1),
            ("func": ST_BLOCK, "line": line.tstFunction+1),
            ("func": ST_STEP, "line": line.tstFunction+1),
            ("func": ST_BLOCK, "line": line.tstFunction+2), # 10
            ("func": ST_STEP, "line": line.tstFunction+2),
            ("func": ST_STEP, "line": line.tstFunction+6),
            ("func": ST_BLOCK, "line": line.tstFunction+7),
            ("func": ST_STEP, "line": line.tstFunction+7),
            ("func": ST_STEP, "line": line.tstFunction+9),
            ("func": ST_FUNC_ENTER, "line": line.tstFunction2+1),  #3
            ("func": ST_BLOCK, "line": line.tstFunction2+1),
            ("func": ST_STEP, "line": line.tstFunction2+1),
            ("func": ST_BLOCK, "line": line.tstFunction2+2),
            ("func": ST_STEP, "line": line.tstFunction2+2), # 20
            ("func": ST_FUNC_ENTER, "line": line.tstFunction2+1),  #2
            ("func": ST_BLOCK, "line": line.tstFunction2+1),
            ("func": ST_STEP, "line": line.tstFunction2+1),
            ("func": ST_BLOCK, "line": line.tstFunction2+2),
            ("func": ST_STEP, "line": line.tstFunction2+2),
            ("func": ST_FUNC_ENTER, "line": line.tstFunction2+1),  #1
            ("func": ST_BLOCK, "line": line.tstFunction2+1),
            ("func": ST_STEP, "line": line.tstFunction2+1),
            ("func": ST_BLOCK, "line": line.tstFunction2+2),
            ("func": ST_STEP, "line": line.tstFunction2+2),  # 30
            ("func": ST_FUNC_ENTER, "line": line.tstFunction2+1),  #0
            ("func": ST_BLOCK, "line": line.tstFunction2+1),
            ("func": ST_STEP, "line": line.tstFunction2+1),
            ("func": ST_BLOCK, "line": line.tstFunction2+4),
            ("func": ST_STEP, "line": line.tstFunction2+4),
            ("func": ST_FUNC_EXIT, "line": line.tstFunction2+1, "retVal": 0), # 0
            ("func": ST_FUNC_EXIT, "line": line.tstFunction2+1, "retVal": 1), # 1
            ("func": ST_FUNC_EXIT, "line": line.tstFunction2+1, "retVal": 2), # 2
            ("func": ST_FUNC_EXIT, "line": line.tstFunction2+1, "retVal": 3), # 3
            ("func": ST_STEP, "line": line.tstFunction+10),   # 40
            ("func": ST_FUNC_EXIT, "line": line.tstFunction+1, "retVal": "DONE"),
            ("func": ST_STEP, "line": line.tstProgram+5),
            ("func": ST_FUNC_ENTER, "line": line.tstNoRetVal+0),  # when empty block then func and block has start_line exceptionally one line up
            ("func": ST_BLOCK, "line": line.tstNoRetVal+0),
            ("func": ST_FUNC_EXIT, "line": line.tstNoRetVal+0),
            ("func": ST_STEP, "line": line.tstProgram+6),
            ("func": ST_STEP, "line": line.tstProgram+7),
            ("func": ST_BLOCK, "line": line.tstProgram+8),
            ("func": ST_STEP, "line": line.tstProgram+8),
            ("func": ST_EXCEPTION, "line": line.tstProgram+8, "ex": "DIVISION-BY-ZERO"), #50
            ("func": ST_BLOCK, "line": line.tstProgram+10),
            ("func": ST_STEP, "line": line.tstProgram+10),
            ("func": ST_STEP, "line": line.tstProgram+12),
            ("func": ST_FUNC_EXIT, "line": line.tstProgram+1),
            ("func": ST_STEP),
        );
        list l;
        checkProgram("full trace", list(DebugStep), traceLog, "DIVISION-BY-ZERO");
        checkProgram("full trace dismiss exception", list(DebugStep, ("func": ST_EXCEPTION, "dismiss": True) ), splice(traceLog, 51, 2), "DONE");
        checkProgram("until return", list(DebugStep, ("func": ST_FUNC_ENTER), ("func": ST_FUNC_ENTER, "sb": DebugUntilReturn), DebugStep), splice(traceLog, 8, 33), "DIVISION-BY-ZERO");
        l = splice(traceLog, 0, 50); # remove remove till exception
        l = splice(l, 1, 3);  # till func exit
        l = splice(l, 2, 1);
        checkProgram("run till exception modify retVal", list(DebugRun, ("func": ST_EXCEPTION, "sb": DebugUntilReturn), ("func": ST_FUNC_EXIT, "retVal": "MODIFIED", "sb": DebugRun)), l, "MODIFIED");
        l = splice(traceLog, 7, 35); # remove tstFunction
        l = splice(l, 8, 3);  # remove tstNoRetVal
        checkProgram("stepover", list(DebugStep, ("func": ST_FUNC_ENTER, "sb": DebugStepOver)), l, "DIVISION-BY-ZERO");
        checkProgram("detach", list(DebugStep, ("func": ST_FUNC_ENTER, "sb": DebugDetach)), splice(traceLog, 2), "DIVISION-BY-ZERO");
        l = ();
        /*for (my int i=0; i<traceLog.size()-2; i++) {  # why -2 ... exception has no checkAttach
            push l, ("func": ST_ATTACH);
        }*/
        checkProgram("do not attach", list(DebugDetach), list(), "DIVISION-BY-ZERO");
        # TODO: DebugBlockReturn, DebugBlockBreak, DebugBlockContinue
        # breakProgramThread, breakProgram
        # multithread
    }

    multiThreadDebugTest1() {
/*        setActions(());
        debugProgram.addProgram(thisProgram);
        semaphore.lock();
        background debugThread();
        tstProgram();


printf("Running\n");
# get current program
our int tid = gettid();
our Program p(PO_DEFAULT, True);
printf("program:%N, tid:%d\n", p, gettid());
our Mutex semaphore();

semaphore.lock();
background debugThread();
*/

    }
}

