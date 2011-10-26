#!/usr/bin/env qore

%new-style
%require-our

%requires uuid

main();

const Iters = 10000;

sub timeit(string test, code code, int total, hash h) {
    date t1 = now_us();
    code(total, h);
    date t2 = now_us();
    
    printf("%s test: %y\n", test, t2 - t1);
}

const Tests = (
    "foreach specialized": 
    sub (int total, hash h) {
	for (int i = 0; i < total; ++i) {
	    foreach string k in (keys h) {
		k = "a";
	    }
	}
    },

    "foreach non-specialized":
    sub (int total, hash h) {
	for (int i = 0; i < total; ++i) {
	    list l = keys h;
	    foreach string k in (l) {
		k = "a";
	    }
	}
    },

    "check type pseudo-method":
    sub (int total, hash h) {
	for (int i = 0; i < total; ++i) {
	    bool b;
	    if (h.typeCode() == NT_HASH)
		delete b;
	}
    },

    "check type string":
    sub (int total, hash h) {
	for (int i = 0; i < total; ++i) {
	    bool b;
	    if (type(h) == Type::Hash)
		delete b;
	}
    },

    "inlist comparison":
    sub (int total, hash h) {
	for (int i = 0; i < total; ++i) {
	    int t = NT_STRING;
	    if (inlist(t, (NT_INT, NT_LIST, NT_FLOAT, NT_DATE, NT_STRING)))
		int x1 = 0;
	}
    },

    "if comparison":
    sub (int total, hash h) {
	for (int i = 0; i < total; ++i) {
	    int t = NT_STRING;
	    if (t == NT_INT || t == NT_LIST || t == NT_FLOAT || t == NT_DATE || t == NT_STRING)
		int x1 = 0;
	}
    },

    "switch comparison":
    sub (int total, hash h) {
	for (int i = 0; i < total; ++i) {
	    int t = NT_STRING;
	    switch (t) {
		case NT_INT:
		case NT_LIST:
		case NT_FLOAT:
		case NT_DATE:
		case NT_STRING:
		int x1 = 0;
	    }
	}
    },
);

const ForEachTests = ("foreach specialized", "foreach non-specialized");
const CheckTypeTests = ("check type pseudo-method", "check type string");
const ListComparisonTests = ("inlist comparison", "if comparison", "switch comparison");

const AllTests = ()
    #+ ForEachTests
    + CheckTypeTests
    #+ ListComparisonTests
    ;

sub main() {
    softint total = exists ARGV[0] ? shift ARGV : Iters;

    printf("%d iterations\n", total);

    # generate a hash with 50 members
    hash h;
    for (int i = 0; i < 50; ++i)
	h.(UUID::get()) = UUID::get();

    foreach string test in (AllTests)
	timeit(test, Tests{test}, total, h);
}

