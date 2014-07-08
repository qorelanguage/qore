#!/usr/bin/env qore 

printf("Qore %s %s %s HAVE_STACK_GUARD=%n\n", Qore::VersionString, Qore::PlatformCPU, Qore::PlatformOS, Option::HAVE_STACK_GUARD);

if (!Option::HAVE_STACK_GUARD) {
    printf("This qore library was not built with stack protection, therefore the test cannot be run.\n");
    exit(1);
}

sub t() { t(); }

sub do_test($str) {
    try
	t();
    catch ($ex)
	printf("TID %d: %s test OK (%s)\n", gettid(), $str, $ex.err);
}

do_test("main thread");
background do_test("background thread");
