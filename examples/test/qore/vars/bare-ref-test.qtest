#!/usr/bin/env qore

%requires UnitTest

%allow-bare-refs

%exec-class App

const closure = sub () { return "global const closure"; };
const mclosure = sub () { return "global const mclosure"; };

namespace Ns {
    our code vclosure;
}

class App inherits UnitTest {
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

	cmp(t1(), "t1", "static method call 1");
	my code func = \t1();
	cmp(func(), "t1", "static method call 2");

	cmp(App::t1(), "t1", "static method call 3");
	func = \App::t1();
	cmp(func(), "t1", "static method call 4");

	cmp(argtest("2", False), ("2", False), "arg test 1");
	func = \argtest();
	cmp(func("True", 2001-01-01), ("True", 2001-01-01), "arg test 2");

	cmp(App::argtest("2", False), ("2", False), "arg test 3");
	func = \App::argtest();
	cmp(func("True", 2001-01-01), ("True", 2001-01-01), "arg test 4");

	cmp(mclosure(), "App::mclosure member", "scope 1");
	func = mclosure;
	cmp(func(), "App::mclosure member", "scope 2");

	cmp(::mclosure(), "global const mclosure", "scope 3");
	func = ::mclosure;
	cmp(func(), "global const mclosure", "scope 4");

	cmp(closure(), "App::closure const", "scope 5");
	func = closure;
	cmp(func(), "App::closure const", "scope 6");

	cmp(App::closure(), "App::closure const", "scope 7");
	func = App::closure;
	cmp(func(), "App::closure const", "scope 8");

	cmp(::closure(), "global const closure", "scope 9");
	func = ::closure;
	cmp(func(), "global const closure", "scope 10");

	cmp(vclosure(), "App::vclosure var", "scope 11");
	func = vclosure;
	cmp(func(), "App::vclosure var", "scope 12");

	cmp(App::vclosure(), "App::vclosure var", "scope 13");
	func = App::vclosure;
	cmp(func(), "App::vclosure var", "scope 14");

	cmp(::vclosure(), "global var vclosure", "scope 15");
	func = ::vclosure;
	cmp(func(), "global var vclosure", "scope 16");

	cmp(Ns::vclosure(), "Ns var vclosure", "scope 17");
	func = Ns::vclosure;
	cmp(func(), "Ns var vclosure", "scope 18");
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

	cmp(a, 1, "var test 1");
	cmp(b, "1", "var test 2");
	cmp(c, 2.1, "var test 3");
	cmp(a1, 2, "var test 4");
	cmp(b1, "four", "var test 5");
	cmp(c1, "hi", "var test 6");
	cmp(a2, 4, "var test 7");
	cmp(b2, "eight", "var test 8");
	cmp(c2, "hello", "var test 9");
    }
    
    static t1() {
	return "t1";
    }
}
