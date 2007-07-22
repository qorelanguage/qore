#!/usr/bin/env qore

%requires qt

%exec-class qt_example
%require-our
%enable-all-warnings

class qt_example inherits QApplication 
{
    constructor() {

	my $window = new QWidget();
	$window.resize(200, 120);

	my $quit = new QPushButton("Quit", $window);
	$quit.setFont(new QFont("Times", 18, QFont::Bold));
	$quit.setGeometry(10, 40, 180, 40);
	QObject_connect($quit, SIGNAL("clicked"), $self, SLOT("quit"));

	$window.show();
	$.exec();
    }
}
