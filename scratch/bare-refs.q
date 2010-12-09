#!/usr/bin/env qore

%allow-bare-refs

%exec-class App

class App {
    const const1 = "const1";

    public {
	static string var1 = "var1";
	mem1 = "mem1";
	string mem2 = "mem2";

	code closure1 = sub () { printf("closure1\n"); };
	const closure2 = sub () { printf("closure2\n"); };
	static closure3 = sub () { printf("closure3\n"); };
    }

    constructor() {
	test("hi", True);
	t1();
	App::test("2", False);
	App::t1();
	my code func = \test();
	func("hi", True);
	func = \t1();
	func();
	func = \App::test();
	func("2", False);
	func = \App::t1();
	func();
	closure1();
	func = closure1;
	func();
	#closure2();
	#func = closure2;
	#func();
	closure3();
	func = closure3;
	func();
	#App::closure2();
	#App::closure3();
    }

    test(string str, any) {
	my int a = 1;
	my b = "1";
	our c = 2.1;
	my (int a1, string b1, c1) = (2, "four", "hi");
	our (int a2, string b2, c2) = (4, "eight", "hello");
	printf("str=%n any=%n a=%n b=%n c=%n a1=%n b1=%n c1=%n a2=%n b2=%n c2=%n const1=%n var1=%n mem1=%n mem2=%n\n", str, any, a, b, c, a1, b1, c1, a2, b2, c2, const1, var1, mem1, mem2);
    }

    static t1() {
	printf("t1\n");
    }
}
