#!/usr/bin/env qore

# this is basically a direct port of the QT widget example
# "windowflags" to Qore using Qore's "qt" module.  

# Note that Qore's "qt" module requires QT 4.3 or above 

# use the "qt-gui" module
%requires qt-gui

# this is an object-oriented program; the application class is "windowflags_example"
%exec-class windowflags_example
# require all variables to be explicitly declared
%require-our
# enable all parse warnings
%enable-all-warnings

class ControllerWindow inherits QWidget
{
    private $.previewWindow, $.typeGroupBox, $.hintsGroupBox, $.quitButton, $.windowRadioButton,
    $.dialogRadioButton, $.sheetRadioButton, $.drawerRadioButton, $.popupRadioButton, 
    $.toolRadioButton, $.toolTipRadioButton, $.splashScreenRadioButton, 
    $.msWindowsFixedSizeDialogCheckBox, $.x11BypassWindowManagerCheckBox, 
    $.framelessWindowCheckBox, $.windowTitleCheckBox, $.windowSystemMenuCheckBox, 
    $.windowMinimizeButtonCheckBox, $.windowMaximizeButtonCheckBox, 
    $.windowContextHelpButtonCheckBox, $.windowShadeButtonCheckBox, 
    $.windowStaysOnTopCheckBox, $.customizeWindowHintCheckBox;

    constructor()
    {
	$.previewWindow = new PreviewWindow($self);

	$.createTypeGroupBox();
	$.createHintsGroupBox();

	$.quitButton = new QPushButton(TR("&Quit"));
	QAPP().connect($.quitButton, SIGNAL("clicked()"), SLOT("quit()"));

	my $bottomLayout = new QHBoxLayout();
	$bottomLayout.addStretch();
	$bottomLayout.addWidget($.quitButton);

	my $mainLayout = new QVBoxLayout();
	$mainLayout.addWidget($.typeGroupBox);
	$mainLayout.addWidget($.hintsGroupBox);
	$mainLayout.addLayout($bottomLayout);
	$.setLayout($mainLayout);

	$.setWindowTitle(TR("Window Flags"));
	$.updatePreview();
    }
    
    updatePreview()
    {
	my $flags = 0;

	if ($.windowRadioButton.isChecked()) {
	    $flags = Qt::Window;
	} else if ($.dialogRadioButton.isChecked()) {
	    $flags = Qt::Dialog;
	} else if ($.sheetRadioButton.isChecked()) {
	    $flags = Qt::Sheet;
	} else if ($.drawerRadioButton.isChecked()) {
	    $flags = Qt::Drawer;
	} else if ($.popupRadioButton.isChecked()) {
	    $flags = Qt::Popup;
	} else if ($.toolRadioButton.isChecked()) {
	    $flags = Qt::Tool;
	} else if ($.toolTipRadioButton.isChecked()) {
	    $flags = Qt::ToolTip;
	} else if ($.splashScreenRadioButton.isChecked()) {
	    $flags = Qt::SplashScreen;
	}

	if ($.msWindowsFixedSizeDialogCheckBox.isChecked())
	    $flags |= Qt::MSWindowsFixedSizeDialogHint;
	if ($.x11BypassWindowManagerCheckBox.isChecked())
	    $flags |= Qt::X11BypassWindowManagerHint;
	if ($.framelessWindowCheckBox.isChecked())
	    $flags |= Qt::FramelessWindowHint;
	if ($.windowTitleCheckBox.isChecked())
	    $flags |= Qt::WindowTitleHint;
	if ($.windowSystemMenuCheckBox.isChecked())
	    $flags |= Qt::WindowSystemMenuHint;
	if ($.windowMinimizeButtonCheckBox.isChecked())
	    $flags |= Qt::WindowMinimizeButtonHint;
	if ($.windowMaximizeButtonCheckBox.isChecked())
	    $flags |= Qt::WindowMaximizeButtonHint;
	if ($.windowContextHelpButtonCheckBox.isChecked())
	    $flags |= Qt::WindowContextHelpButtonHint;
	if ($.windowShadeButtonCheckBox.isChecked())
	    $flags |= Qt::WindowShadeButtonHint;
	if ($.windowStaysOnTopCheckBox.isChecked())
	    $flags |= Qt::WindowStaysOnTopHint;
	if ($.customizeWindowHintCheckBox.isChecked())
	    $flags |= Qt::CustomizeWindowHint;

	$.previewWindow.setWindowFlags($flags);

	my $pos = $.previewWindow.pos();
	if ($pos.x() < 0)
	    $pos.setX(0);
	if ($pos.y
	    () < 0)
	    $pos.setY(0);
	$.previewWindow.move($pos);
	$.previewWindow.show();
    }

    createTypeGroupBox()
    {
	$.typeGroupBox = new QGroupBox(TR("Type"));

	$.windowRadioButton = $.createRadioButton(TR("Window"));
	$.dialogRadioButton = $.createRadioButton(TR("Dialog"));
	$.sheetRadioButton = $.createRadioButton(TR("Sheet"));
	$.drawerRadioButton = $.createRadioButton(TR("Drawer"));
	$.popupRadioButton = $.createRadioButton(TR("Popup"));
	$.toolRadioButton = $.createRadioButton(TR("Tool"));
	$.toolTipRadioButton = $.createRadioButton(TR("Tooltip"));
	$.splashScreenRadioButton = $.createRadioButton(TR("Splash screen"));
	$.windowRadioButton.setChecked(True);

	my $layout = new QGridLayout();
	$layout.addWidget($.windowRadioButton, 0, 0);
	$layout.addWidget($.dialogRadioButton, 1, 0);
	$layout.addWidget($.sheetRadioButton, 2, 0);
	$layout.addWidget($.drawerRadioButton, 3, 0);
	$layout.addWidget($.popupRadioButton, 0, 1);
	$layout.addWidget($.toolRadioButton, 1, 1);
	$layout.addWidget($.toolTipRadioButton, 2, 1);
	$layout.addWidget($.splashScreenRadioButton, 3, 1);
	$.typeGroupBox.setLayout($layout);
    }

    createHintsGroupBox()
    {
	$.hintsGroupBox = new QGroupBox(TR("Hints"));

	$.msWindowsFixedSizeDialogCheckBox = $.createCheckBox(TR("MS Windows fixed size dialog"));
	$.x11BypassWindowManagerCheckBox = $.createCheckBox(TR("X11 bypass window manager"));
	$.framelessWindowCheckBox = $.createCheckBox(TR("Frameless window"));
	$.windowTitleCheckBox = $.createCheckBox(TR("Window title"));
	$.windowSystemMenuCheckBox = $.createCheckBox(TR("Window system menu"));
	$.windowMinimizeButtonCheckBox = $.createCheckBox(TR("Window minimize button"));
	$.windowMaximizeButtonCheckBox = $.createCheckBox(TR("Window maximize button"));
	$.windowContextHelpButtonCheckBox = $.createCheckBox(TR("Window context help button"));
	$.windowShadeButtonCheckBox = $.createCheckBox(TR("Window shade button"));
	$.windowStaysOnTopCheckBox = $.createCheckBox(TR("Window stays on top"));
	$.customizeWindowHintCheckBox= $.createCheckBox(TR("Customize window"));

	my $layout = new QGridLayout();
	$layout.addWidget($.msWindowsFixedSizeDialogCheckBox, 0, 0);
	$layout.addWidget($.x11BypassWindowManagerCheckBox, 1, 0);
	$layout.addWidget($.framelessWindowCheckBox, 2, 0);
	$layout.addWidget($.windowTitleCheckBox, 3, 0);
	$layout.addWidget($.windowSystemMenuCheckBox, 4, 0);
	$layout.addWidget($.windowMinimizeButtonCheckBox, 0, 1);
	$layout.addWidget($.windowMaximizeButtonCheckBox, 1, 1);
	$layout.addWidget($.windowContextHelpButtonCheckBox, 2, 1);
	$layout.addWidget($.windowShadeButtonCheckBox, 3, 1);
	$layout.addWidget($.windowStaysOnTopCheckBox, 4, 1);
	$layout.addWidget($.customizeWindowHintCheckBox, 5, 0);
	$.hintsGroupBox.setLayout($layout);
    }

    createCheckBox($text)
    {
	my $checkBox = new QCheckBox($text);
	$.connect($checkBox, SIGNAL("clicked()"), SLOT("updatePreview()"));
	return $checkBox;
    }

    createRadioButton($text)
    {
	my $button = new QRadioButton($text);
	$.connect($button, SIGNAL("clicked()"), SLOT("updatePreview()"));
	return $button;
    }
}

class PreviewWindow inherits QWidget
{
    private $.textEdit, $.closeButton;

    constructor($parent) : QWidget($parent)
    {
	$.textEdit = new QTextEdit();
	$.textEdit.setReadOnly(True);
	$.textEdit.setLineWrapMode(QTextEdit::NoWrap);

	$.closeButton = new QPushButton(TR("&Close"));
	$.connect($.closeButton, SIGNAL("clicked()"), SLOT("close()"));

	my $layout = new QVBoxLayout();
	$layout.addWidget($.textEdit);
	$layout.addWidget($.closeButton);
	$.setLayout($layout);

	$.setWindowTitle(TR("Preview"));   
    }

    setWindowFlags($flags)
    {
	QWidget::$.setWindowFlags($flags);

	my $text;
	
	my $type = ($flags & Qt::WindowType_Mask);
	if ($type == Qt::Window) {
	    $text = "Qt::Window";
	} else if ($type == Qt::Dialog) {
	    $text = "Qt::Dialog";
	} else if ($type == Qt::Sheet) {
	    $text = "Qt::Sheet";
	} else if ($type == Qt::Drawer) {
	    $text = "Qt::Drawer";
	} else if ($type == Qt::Popup) {
	    $text = "Qt::Popup";
	} else if ($type == Qt::Tool) {
	    $text = "Qt::Tool";
	} else if ($type == Qt::ToolTip) {
	    $text = "Qt::ToolTip";
	} else if ($type == Qt::SplashScreen) {
	    $text = "Qt::SplashScreen";
	}

	if ($flags & Qt::MSWindowsFixedSizeDialogHint)
	    $text += "\n| Qt::MSWindowsFixedSizeDialogHint";
	if ($flags & Qt::X11BypassWindowManagerHint)
	    $text += "\n| Qt::X11BypassWindowManagerHint";
	if ($flags & Qt::FramelessWindowHint)
	    $text += "\n| Qt::FramelessWindowHint";
	if ($flags & Qt::WindowTitleHint)
	    $text += "\n| Qt::WindowTitleHint";
	if ($flags & Qt::WindowSystemMenuHint)
	    $text += "\n| Qt::WindowSystemMenuHint";
	if ($flags & Qt::WindowMinimizeButtonHint)
	    $text += "\n| Qt::WindowMinimizeButtonHint";
	if ($flags & Qt::WindowMaximizeButtonHint)
	    $text += "\n| Qt::WindowMaximizeButtonHint";
	if ($flags & Qt::WindowContextHelpButtonHint)
	    $text += "\n| Qt::WindowContextHelpButtonHint";
	if ($flags & Qt::WindowShadeButtonHint)
	    $text += "\n| Qt::WindowShadeButtonHint";
	if ($flags & Qt::WindowStaysOnTopHint)
	    $text += "\n| Qt::WindowStaysOnTopHint";
	if ($flags & Qt::CustomizeWindowHint)
	    $text += "\n| Qt::CustomizeWindowHint";

	$.textEdit.setPlainText($text);
    }
}

class windowflags_example inherits QApplication
{
    constructor()
    {
	my $controller = new ControllerWindow();
	$controller.show();
	$.exec();
    }
}
