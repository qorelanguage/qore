#!/usr/bin/env qore

namespace Qore {
    namespace SSH2 {
	class SSH2Client {
	    public {
		string $.url;
	    }

	    constructor(string $url) {
		$.url = $url;
		printf("url: %y\n");
	    }
	}
    }
}

load_module("ssh2");

