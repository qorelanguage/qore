#!/usr/bin/env qore

%enable-all-warnings

our Condition $gc;

class T {
    private $.t;

    private {
	$.a;
	int $.i = 1;
	float $.f = 0.25; 
	Thread::Mutex $.mt = new Mutex();
	#Condition $.c;
    }

    public {
	bool $.b;
	string $.str;
    }

    constructor() {
	#needs_condition($.c);
    }
}

class Sub inherits T {
    #private $.f, $.str;
    #public { float $.b float; $.i; }
}

sub main() {
    my T $t = new Sub();

    printf("%N\n", $t);
    #$t.f = 1;
    #$t.b1 = 2;
}

main();
