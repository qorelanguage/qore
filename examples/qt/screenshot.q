#!/usr/bin/env qore

# this is basically a direct port of the QT widget example
# "screenshot" to Qore using Qore's "qt" module.  

# Note that Qore's "qt" module requires QT 4.3 or above 

# use the "qt" module
%requires qt

# this is an object-oriented program; the application class is "screenshot_example"
%exec-class screenshot_example
# require all variables to be explicitly declared
%require-our
# enable all parse warnings
%enable-all-warnings

class Screenshot inherits QWidget
{
    private $.originalPixmap, $.screenshotLabel, $.optionsGroupBox, $.delaySpinBox,
    $.delaySpinBoxLabel, $.hideThisWindowCheckBox, $.newScreenshotButton,
    $.saveScreenshotButton, $.quitScreenshotButton, $.mainLayout,
    $.optionsGroupBoxLayout, $.buttonsLayout;
    
    constructor()
    {
	$.originalPixmap = new QPixmap();

	$.screenshotLabel = new QLabel();
	$.screenshotLabel.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	$.screenshotLabel.setAlignment(Qt::AlignCenter);
	$.screenshotLabel.setMinimumSize(240, 160);

	$.createOptionsGroupBox();
	$.createButtonsLayout();

	$.mainLayout = new QVBoxLayout();
	$.mainLayout.addWidget($.screenshotLabel);
	$.mainLayout.addWidget($.optionsGroupBox);
	$.mainLayout.addLayout($.buttonsLayout);
	$.setLayout($.mainLayout);

	$.shootScreen();
	$.delaySpinBox.setValue(5);

	$.setWindowTitle(TR("Screenshot"));
	$.resize(300, 200);
    }

    resizeEvent($event)
    {
	my $scaledSize = $.originalPixmap.size();
	$scaledSize.scale($.screenshotLabel.size(), Qt::KeepAspectRatio);
	if (!$.screenshotLabel.pixmap()
	    || $scaledSize != $.screenshotLabel.pixmap().size())
	    $.updateScreenshotLabel();
    }

    newScreenshot()
    {
	if ($.hideThisWindowCheckBox.isChecked())
	    $.hide();
	$.newScreenshotButton.setDisabled(True);

	QTimer_singleShot($.delaySpinBox.value() * 1000, $self, SLOT("shootScreen()"));
    }

    saveScreenshot()
    {
	my $format = "png";
	my $initialPath = QDir_currentPath() + TR("/untitled.") + $format;

	my $fileName = QFileDialog_getSaveFileName($self, TR("Save As"),
						   $initialPath,
						   sprintf(TR("%s Files (*.%s);;All Files (*)"), toupper($format), $format));
	if (!strlen($fileName))
	    $.originalPixmap.save($fileName, $format);
    }

    shootScreen()
    {
	if ($.delaySpinBox.value() != 0)
	    QApplication_beep();

	$.originalPixmap = QPixmap_grabWindow(QApplication_desktop().winId());
	$.updateScreenshotLabel();

	$.newScreenshotButton.setDisabled(False);
	if ($.hideThisWindowCheckBox.isChecked())
	    $.show();
    }

    updateCheckBox()
    {
	if ($.delaySpinBox.value() == 0)
	    $.hideThisWindowCheckBox.setDisabled(True);
	else
	    $.hideThisWindowCheckBox.setDisabled(False);
    }

    createOptionsGroupBox()
    {
	$.optionsGroupBox = new QGroupBox(TR("Options"));

	$.delaySpinBox = new QSpinBox();
	$.delaySpinBox.setSuffix(TR(" s"));
	$.delaySpinBox.setMaximum(60);
	$.connect($.delaySpinBox, SIGNAL("valueChanged(int)"), SLOT("updateCheckBox()"));

	$.delaySpinBoxLabel = new QLabel(TR("Screenshot Delay:"));

	$.hideThisWindowCheckBox = new QCheckBox(TR("Hide This Window"));

	$.optionsGroupBoxLayout = new QGridLayout();
	$.optionsGroupBoxLayout.addWidget($.delaySpinBoxLabel, 0, 0);
	$.optionsGroupBoxLayout.addWidget($.delaySpinBox, 0, 1);
	$.optionsGroupBoxLayout.addWidget($.hideThisWindowCheckBox, 1, 0, 1, 2);
	$.optionsGroupBox.setLayout($.optionsGroupBoxLayout);
    }

    createButtonsLayout()
    {
	$.newScreenshotButton = $.createButton(TR("New Screenshot"), $self, SLOT("newScreenshot()"));

	$.saveScreenshotButton = $.createButton(TR("Save Screenshot"), $self, SLOT("saveScreenshot()"));

	$.quitScreenshotButton = $.createButton(TR("Quit"), $self, SLOT("close()"));

	$.buttonsLayout = new QHBoxLayout();
	$.buttonsLayout.addStretch();
	$.buttonsLayout.addWidget($.newScreenshotButton);
	$.buttonsLayout.addWidget($.saveScreenshotButton);
	$.buttonsLayout.addWidget($.quitScreenshotButton);
    }

    createButton($text, $receiver, $member)
    {
	my $button = new QPushButton($text);
	$receiver.connect($button, SIGNAL("clicked()"), $member);
	return $button;
    }

    updateScreenshotLabel()
    {
	$.screenshotLabel.setPixmap($.originalPixmap.scaled($.screenshotLabel.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

class screenshot_example inherits QApplication
{
    constructor()
    {
	my $screenshot = new Screenshot();
	$screenshot.show();
	$.exec();
    }
}
