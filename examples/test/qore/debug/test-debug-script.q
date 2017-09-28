#!/usr/bin/env qore

# program being debugged
%new-style

class TestClass {
    public {
        string public_str;
    }
    private {
        int private_int;
    }
    nothing log(string fmt) {
        vprintf(fmt, argv);
    }
}

sub log(string fmt) {
    vprintf(fmt, argv);
}

our string globalString = 'global string';
our any globalAny;

my hash localHash;

any sub func(string s, int i) {
    string localString = sprintf("%s-%d", s, i);
    int j = 0;
    while (j < 5) {
        *hash lv;# = get_local_vars(j);
        printf("get_local_vars(%d)\n%N\n\n", j, lv);
        j++;
    }
    return localString;
}

nothing sub main() {
    list localList;
    int i = 0;
    int zero = 0;
    # this commands should raise parsing error as they are DOMAIN_DEBUGGER
    #Program pgm = ProgramBase::getProgram();
    #printf("Pgm: %N\n%N\n", pgm, get_local_vars(0));
    TestClass tc();
    tc.log("TestClass::log() %s\n", "ABC");
    log("log() %s\n", "CDE");
    while (i < 3) {
        push localList, func("F", i);
        i++;
    }
    try {
        i = i / zero;
    } catch (hash ex) {
        print("exception\n");
    }
    printf("%y\n", localList);
}

main();


