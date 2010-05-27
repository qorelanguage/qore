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
	if (!exists $h.path) {
	    printf("url %n is missing a path to retrieve\n", $url);
	    exit(1);
	}
	if (!exists $h.host) {
	    printf("url %n is missing the hostname\n", $url);
	    exit(1);
	}

	my any $path = dirname($h.path);
	my any $file = basename($h.path);

	$.setURL($url);

	if ($.connect()) {
	    printf("%s\n", strerror(errno()));
	    exit();
	}

	$.cwd($path);
	printf("PWD: %s\n", $.pwd());
	printf("list(%s): %n\n", $file, $.list($file));
	if (!$.get($file))
	    printf("successfully got %s\n", $file);
	else
	    printf("ERROR: %s\n", strerror(errno()));
	
	$.disconnect();
    }
}
