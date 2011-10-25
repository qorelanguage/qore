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

}

