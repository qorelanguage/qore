#!/usr/bin/env qore

%new-style
%require-our

%requires uuid

main();

const Iters = 10000;

sub timeit(string test, code code, hash info) {
    date t1 = now_us();
    code(info);
    date t2 = now_us();
    
    printf("%s test: %y\n", test, t2 - t1);
}

const Tests = (
    "foreach specialized": 
    sub (hash info) {
	for (int i = 0; i < info.total; ++i) {
	    foreach string k in (keys info.hash) {
		k = "a";
	    }
	}
    },

    "foreach non-specialized":
    sub (hash info) {
	for (int i = 0; i < info.total; ++i) {
	    list l = keys info.hash;
	    foreach string k in (l) {
		k = "a";
	    }
	}
    },

    "check type pseudo-method":
    sub (hash info) {
	for (int i = 0; i < info.total; ++i) {
	    bool b;
	    if (info.hash.typeCode() == NT_HASH)
		delete b;
	}
    },

    "check type string":
    sub (hash info) {
	for (int i = 0; i < info.total; ++i) {
	    bool b;
	    if (type(info.hash) == Type::Hash)
		delete b;
	}
    },

    "inlist comparison":
    sub (hash info) {
	for (int i = 0; i < info.total; ++i) {
	    int t = NT_STRING;
	    if (inlist(t, (NT_INT, NT_LIST, NT_FLOAT, NT_DATE, NT_STRING)))
		int x1 = 0;
	}
    },

    "if comparison":
    sub (hash info) {
	for (int i = 0; i < info.total; ++i) {
	    int t = NT_STRING;
	    if (t == NT_INT || t == NT_LIST || t == NT_FLOAT || t == NT_DATE || t == NT_STRING)
		int x1 = 0;
	}
    },

    "switch comparison":
    sub (hash info) {
	for (int i = 0; i < info.total; ++i) {
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

    hash info = (
	"total": total,
	"hash": h,
	);

    foreach string test in (AllTests)
	timeit(test, Tests{test}, info);
}

