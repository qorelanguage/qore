#!/usr/bin/env qore

# This is basically a direct port of the QT widget example
# "lineedits" to Qore using Qore's "qt" module.  

# Note that Qore's "qt" module requires QT 4.3 or above 

# use the "qt-gui" module
%requires qt-gui

# this is an object-oriented program, the application class is "lineedits_example"
%exec-class lineedits_example
# require all variables to be explicitly declared
%require-our
# enable all parse warnings
%enable-all-warnings

class Window inherits QWidget
{
    constructor()
    {
	my $echoGroup = new QGroupBox(TR("Echo"));

	my $echoLabel = new QLabel(TR("Mode:"));
	my $echoComboBox = new QComboBox();
	$echoComboBox.addItem(TR("Normal"));
	$echoComboBox.addItem(TR("Password"));
	$echoComboBox.addItem(TR("PasswordEchoOnEdit"));
	$echoComboBox.addItem(TR("No Echo"));

	$.echoLineEdit = new QLineEdit();
	$.echoLineEdit.setFocus();

	my $validatorGroup = new QGroupBox(TR("Validator"));

	my $validatorLabel = new QLabel(TR("Type:"));
	my $validatorComboBox = new QComboBox();
	$validatorComboBox.addItem(TR("No validator"));
	$validatorComboBox.addItem(TR("Integer validator"));
	$validatorComboBox.addItem(TR("Double validator"));

	$.validatorLineEdit = new QLineEdit();

	my $alignmentGroup = new QGroupBox(TR("Alignment"));

	my $alignmentLabel = new QLabel(TR("Type:"));
	my $alignmentComboBox = new QComboBox();
	$alignmentComboBox.addItem(TR("Left"));
	$alignmentComboBox.addItem(TR("Centered"));
	$alignmentComboBox.addItem(TR("Right"));

	$.alignmentLineEdit = new QLineEdit();

	my $inputMaskGroup = new QGroupBox(TR("Input mask"));

	my $inputMaskLabel = new QLabel(TR("Type:"));
	my $inputMaskComboBox = new QComboBox();
	$inputMaskComboBox.addItem(TR("No mask"));
	$inputMaskComboBox.addItem(TR("Phone number"));
	$inputMaskComboBox.addItem(TR("ISO date"));
	$inputMaskComboBox.addItem(TR("License key"));

	$.inputMaskLineEdit = new QLineEdit();

	my $accessGroup = new QGroupBox(TR("Access"));

	my $accessLabel = new QLabel(TR("Read-only:"));
	my $accessComboBox = new QComboBox();
	$accessComboBox.addItem(TR("False"));
	$accessComboBox.addItem(TR("True"));

	$.accessLineEdit = new QLineEdit();

	$.connect($echoComboBox, SIGNAL("activated(int)"), SLOT("echoChanged(int)"));
	$.connect($validatorComboBox, SIGNAL("activated(int)"),	SLOT("validatorChanged(int)"));
	$.connect($alignmentComboBox, SIGNAL("activated(int)"),	SLOT("alignmentChanged(int)"));
	$.connect($inputMaskComboBox, SIGNAL("activated(int)"), SLOT("inputMaskChanged(int)"));
	$.connect($accessComboBox, SIGNAL("activated(int)"), SLOT("accessChanged(int)"));

	my $echoLayout = new QGridLayout();
	$echoLayout.addWidget($echoLabel, 0, 0);
	$echoLayout.addWidget($echoComboBox, 0, 1);
	$echoLayout.addWidget($.echoLineEdit, 1, 0, 1, 2);
	$echoGroup.setLayout($echoLayout);
	
	my $validatorLayout = new QGridLayout();
	$validatorLayout.addWidget($validatorLabel, 0, 0);
	$validatorLayout.addWidget($validatorComboBox, 0, 1);
	$validatorLayout.addWidget($.validatorLineEdit, 1, 0, 1, 2);
	$validatorGroup.setLayout($validatorLayout);

	my $alignmentLayout = new QGridLayout();
	$alignmentLayout.addWidget($alignmentLabel, 0, 0);
	$alignmentLayout.addWidget($alignmentComboBox, 0, 1);
	$alignmentLayout.addWidget($.alignmentLineEdit, 1, 0, 1, 2);
	$alignmentGroup. setLayout($alignmentLayout);

	my $inputMaskLayout = new QGridLayout();
	$inputMaskLayout.addWidget($inputMaskLabel, 0, 0);
	$inputMaskLayout.addWidget($inputMaskComboBox, 0, 1);
	$inputMaskLayout.addWidget($.inputMaskLineEdit, 1, 0, 1, 2);
	$inputMaskGroup.setLayout($inputMaskLayout);

	my $accessLayout = new QGridLayout();
	$accessLayout.addWidget($accessLabel, 0, 0);
	$accessLayout.addWidget($accessComboBox, 0, 1);
	$accessLayout.addWidget($.accessLineEdit, 1, 0, 1, 2);
	$accessGroup.setLayout($accessLayout);

	my $layout = new QGridLayout();
	$layout.addWidget($echoGroup, 0, 0);
	$layout.addWidget($validatorGroup, 1, 0);
	$layout.addWidget($alignmentGroup, 2, 0);
	$layout.addWidget($inputMaskGroup, 0, 1);
	$layout.addWidget($accessGroup, 1, 1);
	$.setLayout($layout);

	$.setWindowTitle(TR("Line Edits"));
    }

    echoChanged($index)
    {
	switch ($index) {
	    case 0:
	    $.echoLineEdit.setEchoMode(QLineEdit::Normal);
	    break;

	    case 1:
	    $.echoLineEdit.setEchoMode(QLineEdit::Password);
	    break;

	    case 2:
	    $.echoLineEdit.setEchoMode(QLineEdit::PasswordEchoOnEdit);
	    break;

	    case 3:
	    $.echoLineEdit.setEchoMode(QLineEdit::NoEcho);
	}
    }

    validatorChanged($index)
    {
	switch ($index) {
	    case 0:
	    $.validatorLineEdit.setValidator();
	    break;
	    case 1:
	    $.validatorLineEdit.setValidator(new QIntValidator($.validatorLineEdit));
	    break;
	    case 2:
	    $.validatorLineEdit.setValidator(new QDoubleValidator(-999.0, 999.0, 2, $.validatorLineEdit));
	}

	$.validatorLineEdit.clear();
    }

    alignmentChanged($index)
    {
	switch ($index) {
	    case 0:
	    $.alignmentLineEdit.setAlignment(Qt::AlignLeft);
	    break;
	    case 1:
	    $.alignmentLineEdit.setAlignment(Qt::AlignCenter);
	    break;
	    case 2:
	    $.alignmentLineEdit.setAlignment(Qt::AlignRight);
	}
    }

    inputMaskChanged($index)
    {
	switch ($index) {
	    case 0:
	    $.inputMaskLineEdit.setInputMask("");
	    break;
	    case 1:
	    $.inputMaskLineEdit.setInputMask("+99 99 99 99 99;_");
	    break;
	    case 2:
	    $.inputMaskLineEdit.setInputMask("0000-00-00");
	    $.inputMaskLineEdit.setText("00000000");
	    $.inputMaskLineEdit.setCursorPosition(0);
	    break;
	    case 3:
	    $.inputMaskLineEdit.setInputMask(">AAAAA-AAAAA-AAAAA-AAAAA-AAAAA;#");
	}
    }

    accessChanged($index)
    {
	switch ($index) {
	    case 0:
	    $.accessLineEdit.setReadOnly(False);
	    break;
	    case 1:
	    $.accessLineEdit.setReadOnly(True);
	}
    }
}

class lineedits_example inherits QApplication
{
    constructor()
    {
	my $window = new Window();
	$window.show();
	$.exec();
    }
}
