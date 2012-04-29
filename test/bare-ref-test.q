#!/usr/bin/env qore

%allow-bare-refs

%exec-class App

const closure = sub () { return "global const closure"; };
const mclosure = sub () { return "global const mclosure"; };

namespace Ns {
    our code vclosure;
}

class App {
    const const1 = "const1";

    public {
	static string var1 = "var1";
	mem1 = "mem1";
	string mem2 = "mem2";

	code mclosure = sub () { return "App::mclosure member"; };
	static vclosure = sub () { return "App::vclosure var"; };
	const closure = sub () { return "App::closure const"; };
    }

    constructor() {
	Ns::vclosure = sub () { return "Ns var vclosure"; };
	our code ::vclosure = sub () { return "global var vclosure"; };

	lvartest();

	test_value(t1(), "t1", "static method call 1");
	my code func = \t1();
	test_value(func(), "t1", "static method call 2");

	test_value(App::t1(), "t1", "static method call 3");
	func = \App::t1();
	test_value(func(), "t1", "static method call 4");

	test_value(argtest("2", False), ("2", False), "arg test 1");
	func = \argtest();
	test_value(func("True", 2001-01-01), ("True", 2001-01-01), "arg test 2");

	test_value(App::argtest("2", False), ("2", False), "arg test 3");
	func = \App::argtest();
	test_value(func("True", 2001-01-01), ("True", 2001-01-01), "arg test 4");

	test_value(mclosure(), "App::mclosure member", "scope 1");
	func = mclosure;
	test_value(func(), "App::mclosure member", "scope 2");

	test_value(::mclosure(), "global const mclosure", "scope 3");
	func = ::mclosure;
	test_value(func(), "global const mclosure", "scope 4");

	test_value(closure(), "App::closure const", "scope 5");
	func = closure;
	test_value(func(), "App::closure const", "scope 6");

	test_value(App::closure(), "App::closure const", "scope 7");
	func = App::closure;
	test_value(func(), "App::closure const", "scope 8");

	test_value(::closure(), "global const closure", "scope 9");
	func = ::closure;
	test_value(func(), "global const closure", "scope 10");

	test_value(vclosure(), "App::vclosure var", "scope 11");
	func = vclosure;
	test_value(func(), "App::vclosure var", "scope 12");

	test_value(App::vclosure(), "App::vclosure var", "scope 13");
	func = App::vclosure;
	test_value(func(), "App::vclosure var", "scope 14");

	test_value(::vclosure(), "global var vclosure", "scope 15");
	func = ::vclosure;
	test_value(func(), "global var vclosure", "scope 16");

	test_value(Ns::vclosure(), "Ns var vclosure", "scope 17");
	func = Ns::vclosure;
	test_value(func(), "Ns var vclosure", "scope 18");
    }
    
    argtest(string str, any) {
	return (str, any);
    }

    lvartest() {
	my int a = 1;
	my b = "1";
	our c = 2.1;
	my (int a1, string b1, c1) = (2, "four", "hi");
	our (int a2, string b2, c2) = (4, "eight", "hello");

	test_value(a, 1, "var test 1");
	test_value(b, "1", "var test 2");
	test_value(c, 2.1, "var test 3");
	test_value(a1, 2, "var test 4");
	test_value(b1, "four", "var test 5");
	test_value(c1, "hi", "var test 6");
	test_value(a2, 4, "var test 7");
	test_value(b2, "eight", "var test 8");
	test_value(c2, "hello", "var test 9");
    }
    
    static t1() {
	return "t1";
    }
    
    static test_value(v1, v2, string msg) {
	if (v1 === v2) {
	    printf("OK: %s test\n", msg);
	}
	else {
	    printf("ERROR: %s test failed! (%N != %N)\n", msg, v1, v2);
	    #++errors;
	}
	#thash.msg = True;
    }
}
