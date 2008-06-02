#!/usr/bin/env qore

# this is basically a direct port of the QT widget example
# "extension" to Qore using Qore's "qt" module.  

# Note that Qore's "qt" module requires QT 4.3 or above 

# use the "qt-gui" module
%requires qt-gui

# this is an object-oriented program; the application class is "extension_example"
%exec-class extension_example
# require all variables to be explicitly declared
%require-our
# enable all parse warnings
%enable-all-warnings

class FindDialog inherits QDialog
{
    private $.label, $.lineEdit, $.caseCheckBox, $.fromStartCheckBox, 
    $.wholeWordsCheckBox, $.searchSelectionCheckBox, $.backwardCheckBox, 
    $.buttonBox, $.findButton, $.moreButton, $.extension;

    constructor()
    {
	$.label = new QLabel(TR("Find &what:"));
	$.lineEdit = new QLineEdit();
	$.label.setBuddy($.lineEdit);

	$.caseCheckBox = new QCheckBox(TR("Match &case"));
	$.fromStartCheckBox = new QCheckBox(TR("Search from &start"));
	$.fromStartCheckBox.setChecked(True);

	$.findButton = new QPushButton(TR("&Find"));
	$.findButton.setDefault(True);

	$.moreButton = new QPushButton(TR("&More"));
	$.moreButton.setCheckable(True);
	$.moreButton.setAutoDefault(False);

	$.buttonBox = new QDialogButtonBox(Qt::Vertical);
	$.buttonBox.addButton($.findButton, QDialogButtonBox::ActionRole);
	$.buttonBox.addButton($.moreButton, QDialogButtonBox::ActionRole);

	$.extension = new QWidget();

	$.wholeWordsCheckBox = new QCheckBox(TR("&Whole words"));
	$.backwardCheckBox = new QCheckBox(TR("Search &backward"));
	$.searchSelectionCheckBox = new QCheckBox(TR("Search se&lection"));

	$.extension.connect($.moreButton, SIGNAL("toggled(bool)"), SLOT("setVisible(bool)"));

	my $extensionLayout = new QVBoxLayout();
	$extensionLayout.setMargin(0);
	$extensionLayout.addWidget($.wholeWordsCheckBox);
	$extensionLayout.addWidget($.backwardCheckBox);
	$extensionLayout.addWidget($.searchSelectionCheckBox);
	$.extension.setLayout($extensionLayout);
	
	my $topLeftLayout = new QHBoxLayout();
	$topLeftLayout.addWidget($.label);
	$topLeftLayout.addWidget($.lineEdit);

	my $leftLayout = new QVBoxLayout();
	$leftLayout.addLayout($topLeftLayout);
	$leftLayout.addWidget($.caseCheckBox);
	$leftLayout.addWidget($.fromStartCheckBox);
	$leftLayout.addStretch(1);

	my $mainLayout = new QGridLayout();
	$mainLayout.setSizeConstraint(QLayout::SetFixedSize);
	$mainLayout.addLayout($leftLayout, 0, 0);
	$mainLayout.addWidget($.buttonBox, 0, 1);
	$mainLayout.addWidget($.extension, 1, 0, 1, 2);
	$.setLayout($mainLayout);

	$.setWindowTitle(TR("Extension"));
	$.extension.hide();
    }
}


class extension_example inherits QApplication
{
    constructor()
    {      
	my $dialog = new FindDialog();
	$dialog.show();
	
    	$.exec();
    }
}
