#!/usr/bin/env qore
# -*- mode: qore; indent-tabs-mode: nil -*-

%new-style
%enable-all-warnings
%require-types
%strict-args

#%requires ../../../../qlib/Util.qm
%requires ../../../../qlib/QUnit.qm

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
  int handle(string func, Program pgm, *hash location, *hash extra) {
    printf("%y: pgm: %y, %y, %y\n", func, pgm, location, extra);
    hash it = (
      'func': func,
      'file': location.file,
      'line': location.start_line,
      'tid': gettid(),
    );
    it += extra;
    push log, it;
    *hash act = shift actions;
    if (act.typeCode() == NT_HASH) {
      return exists act.rc ? act.rc : DebugStep;
    } else {
      return exists act ? act : DebugStep;
    }
  }

  int onAttach(Program pgm) {
    return handle("onAttach", pgm);
  }

  int onDetach(Program pgm) {
    return handle("onDetach", pgm); 
  }

  int onStep(Program pgm, hash blockLocation, *hash location, reference retCode) {
    *hash act = actions[0];
    if (exists act.retCode) {
        retCode = act.retCode;
    }
    return handle("onStep", pgm, location ?? blockLocation);
  }

  int onFunctionEnter(Program pgm, hash location) {
    return handle("onFunctionEnter", pgm, location);
  }

  int onFunctionExit(Program pgm, hash location, reference returnValue) {
    return handle("onFunctionExit", pgm, location, ("returnValue": returnValue));
  }

  int onException(Program pgm, hash location) {
    return handle("onException", pgm, location);
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
        #Program thisProgram;
        #DebugProgram debugProgram;
    }

    constructor() : Test("Debug Test", "1.0") {
        addTestCase("SingleThreadDebugTest1", \singleThreadDebugTest1());
        #addTestCase("MultiThreadDebugTest1", \multiThreadDebugTest1());

        thisProgram = new Program(PO_DEFAULT, True);  # TODO: PO_xxx
        debugProgram = new MyDebugProgram();
        main_tid = gettid();
        stack = tstGetStack();
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
    nothing tstProgram() {
      int i = 1;
      string s = i.type();
      int tid = gettid();
      any /*TODO:string, getting reference wrongly from onFunctionExit*/ ret = tstFunction(i, "Func");
      tstNoRetVal();
      print("res: %y\n", ret);
      try {
        i = (tid+s.size()+ret.size()) / (tid-tid);
      } catch (hash ex) {
        printf("Ex:%y\n", ex);
      }
    }
    # end of tested program


    singleThreadDebugTest1() {
        debugProgram.setActions(list());
        debugProgram.addProgram(thisProgram);
        tstProgram();
        debugProgram.removeProgram(thisProgram);



/*        testAssertionValue('end position check', fr.getPos(), 11);
        testAssertionValue('read string check', s, String);
        testAssertionValue('ReadOnlyFile::readTextFile() string check', ReadOnlyFile::readTextFile(file), String);
        assertThrows("FILE-OPEN2-ERROR", \ReadOnlyFile::readTextFile(), tmp_location() + DirSep + get_random_string()); */
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

