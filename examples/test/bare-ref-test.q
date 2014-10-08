#!/usr/bin/env qore

%requires UnitTest

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
        my UnitTest t(True);

	Ns::vclosure = sub () { return "Ns var vclosure"; };
	our code ::vclosure = sub () { return "global var vclosure"; };

	lvartest();

	t.cmp(t1(), "t1", "static method call 1");
	my code func = \t1();
	t.cmp(func(), "t1", "static method call 2");

	t.cmp(App::t1(), "t1", "static method call 3");
	func = \App::t1();
	t.cmp(func(), "t1", "static method call 4");

	t.cmp(argtest("2", False), ("2", False), "arg test 1");
	func = \argtest();
	t.cmp(func("True", 2001-01-01), ("True", 2001-01-01), "arg test 2");

	t.cmp(App::argtest("2", False), ("2", False), "arg test 3");
	func = \App::argtest();
	t.cmp(func("True", 2001-01-01), ("True", 2001-01-01), "arg test 4");

	t.cmp(mclosure(), "App::mclosure member", "scope 1");
	func = mclosure;
	t.cmp(func(), "App::mclosure member", "scope 2");

	t.cmp(::mclosure(), "global const mclosure", "scope 3");
	func = ::mclosure;
	t.cmp(func(), "global const mclosure", "scope 4");

	t.cmp(closure(), "App::closure const", "scope 5");
	func = closure;
	t.cmp(func(), "App::closure const", "scope 6");

	t.cmp(App::closure(), "App::closure const", "scope 7");
	func = App::closure;
	t.cmp(func(), "App::closure const", "scope 8");

	t.cmp(::closure(), "global const closure", "scope 9");
	func = ::closure;
	t.cmp(func(), "global const closure", "scope 10");

	t.cmp(vclosure(), "App::vclosure var", "scope 11");
	func = vclosure;
	t.cmp(func(), "App::vclosure var", "scope 12");

	t.cmp(App::vclosure(), "App::vclosure var", "scope 13");
	func = App::vclosure;
	t.cmp(func(), "App::vclosure var", "scope 14");

	t.cmp(::vclosure(), "global var vclosure", "scope 15");
	func = ::vclosure;
	t.cmp(func(), "global var vclosure", "scope 16");

	t.cmp(Ns::vclosure(), "Ns var vclosure", "scope 17");
	func = Ns::vclosure;
	t.cmp(func(), "Ns var vclosure", "scope 18");
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

        my UnitTest t(True);

	t.cmp(a, 1, "var test 1");
	t.cmp(b, "1", "var test 2");
	t.cmp(c, 2.1, "var test 3");
	t.cmp(a1, 2, "var test 4");
	t.cmp(b1, "four", "var test 5");
	t.cmp(c1, "hi", "var test 6");
	t.cmp(a2, 4, "var test 7");
	t.cmp(b2, "eight", "var test 8");
	t.cmp(c2, "hello", "var test 9");
    }
    
    static t1() {
	return "t1";
    }
}
