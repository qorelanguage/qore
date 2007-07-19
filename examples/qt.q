#!/usr/bin/env qore

%requires qt

%exec-class qt_example
%require-our
%enable-all-warnings

class qt_example inherits QApplication 
{
    constructor() {
	my $pb = new QPushButton("hello there");
	$pb.resize(100, 30);
	$pb.show();

	# run QApplication::exec()
	$.exec();
    }
}
