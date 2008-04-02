#!/usr/bin/env qore

# This is basically a direct port of the QT widget example
# "groupbox" to Qore using Qore's "qt" module.  

# Note that Qore's "qt" module requires QT 4.3 or above 

# use the "qt" module
%requires qt

# this is an object-oriented program, the application class is "groupbox_example"
%exec-class groupbox_example
# require all variables to be explicitly declared
%require-our
# enable all parse warnings
%enable-all-warnings

class Window inherits QWidget
{
    constructor($parent) : QWidget($parent)
    {
	my $grid = new QGridLayout();
	$grid.addWidget($.createFirstExclusiveGroup(), 0, 0);
	$grid.addWidget($.createSecondExclusiveGroup(), 1, 0);
	$grid.addWidget($.createNonExclusiveGroup(), 0, 1);
	$grid.addWidget($.createPushButtonGroup(), 1, 1);
	$.setLayout($grid);
	
	$.setWindowTitle(TR("Group Boxes"));
	$.resize(480, 320);
    }

    createFirstExclusiveGroup()
    {
	my $groupBox = new QGroupBox(TR("Exclusive Radio Buttons"));

	my $radio1 = new QRadioButton(TR("&Radio button 1"));
	my $radio2 = new QRadioButton(TR("R&adio button 2"));
	my $radio3 = new QRadioButton(TR("Ra&dio button 3"));

	$radio1.setChecked(True);

	my $vbox = new QVBoxLayout();
	$vbox.addWidget($radio1);
	$vbox.addWidget($radio2);
	$vbox.addWidget($radio3);
	$vbox.addStretch(1);
	$groupBox.setLayout($vbox);

	return $groupBox;
    }

    createSecondExclusiveGroup()
    {
	my $groupBox = new QGroupBox(TR("E&xclusive Radio Buttons"));
	$groupBox.setCheckable(True);
	$groupBox.setChecked(False);

	my $radio1 = new QRadioButton(TR("Rad&io button 1"));
	my $radio2 = new QRadioButton(TR("Radi&o button 2"));
	my $radio3 = new QRadioButton(TR("Radio &button 3"));
	$radio1.setChecked(True);
	my $checkBox = new QCheckBox(TR("Ind&ependent checkbox"));
	$checkBox.setChecked(True);

	my $vbox = new QVBoxLayout();
	$vbox.addWidget($radio1);
	$vbox.addWidget($radio2);
	$vbox.addWidget($radio3);
	$vbox.addWidget($checkBox);
	$vbox.addStretch(1);
	$groupBox.setLayout($vbox);

	return $groupBox;
    }

    createNonExclusiveGroup()
    {
	my $groupBox = new QGroupBox(TR("Non-Exclusive Checkboxes"));
	$groupBox.setFlat(True);

	my $checkBox1 = new QCheckBox(TR("&Checkbox 1"));
	my $checkBox2 = new QCheckBox(TR("C&heckbox 2"));
	$checkBox2.setChecked(True);
	my $tristateBox = new QCheckBox(TR("Tri-&state button"));
	$tristateBox.setTristate(True);
	$tristateBox.setCheckState(Qt::PartiallyChecked);

	my $vbox = new QVBoxLayout();
	$vbox.addWidget($checkBox1);
	$vbox.addWidget($checkBox2);
	$vbox.addWidget($tristateBox);
	$vbox.addStretch(1);
	$groupBox.setLayout($vbox);

	return $groupBox;
    }

    createPushButtonGroup()
    {
	my $groupBox = new QGroupBox(TR("&Push Buttons"));
	$groupBox.setCheckable(True);
	$groupBox.setChecked(True);

	my $pushButton = new QPushButton(TR("&Normal Button"));
	my $toggleButton = new QPushButton(TR("&Toggle Button"));
	$toggleButton.setCheckable(True);
	$toggleButton.setChecked(True);
	my $flatButton = new QPushButton(TR("&Flat Button"));
	$flatButton.setFlat(True);

	my $popupButton = new QPushButton(TR("Pop&up Button"));
	my $menu = new QMenu($self);
	$menu.addAction(TR("&First Item"));
	$menu.addAction(TR("&Second Item"));
	$menu.addAction(TR("&Third Item"));
	$menu.addAction(TR("F&ourth Item"));
	$popupButton.setMenu($menu);

	my $newAction = $menu.addAction(TR("Submenu"));
	my $subMenu = new QMenu(TR("Popup Submenu"));
	$subMenu.addAction(TR("Item 1"));
	$subMenu.addAction(TR("Item 2"));
	$subMenu.addAction(TR("Item 3"));
	$newAction.setMenu($subMenu);

	my $vbox = new QVBoxLayout();
	$vbox.addWidget($pushButton);
	$vbox.addWidget($toggleButton);
	$vbox.addWidget($flatButton);
	$vbox.addWidget($popupButton);
	$vbox.addStretch(1);
	$groupBox.setLayout($vbox);

	return $groupBox;
    }
}

class groupbox_example inherits QApplication
{
    constructor()
    {
	my $window = new Window();
	$window.show();
	$.exec();
    }
}
