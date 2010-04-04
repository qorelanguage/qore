#!/usr/bin/env qore

%exec-class FtpTest
%require-types
%require-our

class FtpTest inherits FtpClient {
    constructor() {
	my any $url = shift $ARGV;
	
	if (!exists $url) {
	    printf("syntax: %s <url>\n", get_script_name());
	    exit(1);
	}

	my hash $h = parse_url($url);
	my any $path = dirname($h.path);
	my any $file = basename($h.path);
	if (!exists $path || !exists $file) {
	    printf("url %n is missing a path to retrieve\n", $url);
	    exit(1);
	}

	$.setURL($url);

	if ($.connect()) {
	    printf("%s\n", strerror(errno()));
	    exit();
	}

	$.cwd($path);
	printf("PWD: %s\n", $.pwd());
	printf("list(%s): %n\n", $file, $.list($file));
	if (!$.put($file))
	    printf("successfully sent %s\n", $file);
	else
	    printf("ERROR: %s\n", strerror(errno()));
	
	printf("list(%s): %n\n", $file, $.list($file));
	$.disconnect();
    }
}
