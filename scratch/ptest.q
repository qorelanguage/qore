#!/usr/bin/env qore

%new-style
%require-our

%requires uuid

main();

const Iters = 10000;

sub timeit(string test, code code) {
    date t1 = now_us();
    code();
    date t2 = now_us();
    
    printf("%s test: %y\n", test, t2 - t1);
}

sub main() {
    softint total = exists ARGV[0] ? shift ARGV : Iters;

    printf("%d iterations\n", total);

    # generate a hash with 50 members
    hash h;
    for (int i = 0; i < 50; ++i)
	h.(UUID::get()) = UUID::get();

    /*
    timeit("foreach specialized", 
	sub () {
	    for (int i = 0; i < total; ++i) {
		foreach string k in (keys h) {
		    k = "a";
		}
	    }
	});

    timeit("foreach non-specialized", 
	sub () {
	    for (int i = 0; i < total; ++i) {
		list l = keys h;
		foreach string k in (l) {
		    k = "a";
		}
	    }
	});

    timeit("check type pseudo-method", 
	sub () {
	    for (int i = 0; i < total; ++i) {
		bool b;
		if (h.typeCode() == NT_HASH)
		    delete b;
	    }
	});

    timeit("check type string", 
           sub () {
	    for (int i = 0; i < total; ++i) {
		bool b;
		if (type(h) == Type::Hash)
		    delete b;
	    }
	});
    */

    timeit("inlist comparison",
	   sub () {
	       for (int i = 0; i < total; ++i) {
		   int t = NT_STRING;
		   if (inlist(t, (NT_INT, NT_LIST, NT_FLOAT, NT_DATE, NT_STRING)))
		       int x1 = 0;
	       }
	   }
	);

    timeit("if comparison",
	   sub () {
	       for (int i = 0; i < total; ++i) {
		   int t = NT_STRING;
		   if (t == NT_INT || t == NT_LIST || t == NT_FLOAT || t == NT_DATE || t == NT_STRING)
		       int x1 = 0;
	       }
	   }
	);

    timeit("switch comparison",
	   sub () {
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
	   }
	);
}

