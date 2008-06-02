#!/usr/bin/env qore

# This is basically a direct port of the QT widget example
# "sliders" to Qore using Qore's "qt" module.  

# Note that Qore's "qt" module requires QT 4.3 or above 

# use the "qt-gui" module
%requires qt-gui

# this is an object-oriented program; the application class is "sliders_example"
%exec-class sliders_example
# require all variables to be explicitly declared
%require-our
# enable all parse warnings
%enable-all-warnings

class Window inherits QWidget
{
    constructor() : QWidget()
    {
	$.horizontalSliders = new SlidersGroup(Qt::Horizontal, TR("Horizontal"));
	$.verticalSliders = new SlidersGroup(Qt::Vertical, TR("Vertical"));

	$.stackedWidget = new QStackedWidget();
	$.stackedWidget.addWidget($.horizontalSliders);
	$.stackedWidget.addWidget($.verticalSliders);

	$.createControls(TR("Controls"));

	$.verticalSliders.connect($.horizontalSliders, SIGNAL("valueChanged(int)"), SLOT("setValue(int)"));
	$.valueSpinBox.connect($.verticalSliders, SIGNAL("valueChanged(int)"), SLOT("setValue(int)"));
	$.horizontalSliders.connect($.valueSpinBox, SIGNAL("valueChanged(int)"), SLOT("setValue(int)"));

	my $layout = new QHBoxLayout();
	$layout.addWidget($.controlsGroup);
	$layout.addWidget($.stackedWidget);
	$.setLayout($layout);

	$.minimumSpinBox.setValue(0);
	$.maximumSpinBox.setValue(20);
	$.valueSpinBox.setValue(5);

	$.setWindowTitle(TR("Sliders"));    
    }

    createControls($title)
    {
	$.controlsGroup = new QGroupBox($title);

	$.minimumLabel = new QLabel(TR("Minimum value:"));
	$.maximumLabel = new QLabel(TR("Maximum value:"));
	$.valueLabel = new QLabel(TR("Current value:"));

	$.invertedAppearance = new QCheckBox(TR("Inverted appearance"));
	$.invertedKeyBindings = new QCheckBox(TR("Inverted key bindings"));

	$.minimumSpinBox = new QSpinBox();
	$.minimumSpinBox.setRange(-100, 100);
	$.minimumSpinBox.setSingleStep(1);

	$.maximumSpinBox = new QSpinBox();
	$.maximumSpinBox.setRange(-100, 100);
	$.maximumSpinBox.setSingleStep(1);

	$.valueSpinBox = new QSpinBox();
	$.valueSpinBox.setRange(-100, 100);
	$.valueSpinBox.setSingleStep(1);

	$.orientationCombo = new QComboBox();
	$.orientationCombo.addItem(TR("Horizontal slider-like widgets"));
	$.orientationCombo.addItem(TR("Vertical slider-like widgets"));

	$.stackedWidget.connect($.orientationCombo, SIGNAL("activated(int)"), SLOT("setCurrentIndex(int)"));
	$.horizontalSliders.connect($.minimumSpinBox, SIGNAL("valueChanged(int)"), SLOT("setMinimum(int)"));
	$.verticalSliders.connect($.minimumSpinBox, SIGNAL("valueChanged(int)"), SLOT("setMinimum(int)"));
	$.horizontalSliders.connect($.maximumSpinBox, SIGNAL("valueChanged(int)"), SLOT("setMaximum(int)"));
	$.verticalSliders.connect($.maximumSpinBox, SIGNAL("valueChanged(int)"), SLOT("setMaximum(int)"));
	$.horizontalSliders.connect($.invertedAppearance, SIGNAL("toggled(bool)"), SLOT("invertAppearance(bool)"));
	$.verticalSliders.connect($.invertedAppearance, SIGNAL("toggled(bool)"), SLOT("invertAppearance(bool)"));
	$.horizontalSliders.connect($.invertedKeyBindings, SIGNAL("toggled(bool)"), SLOT("invertKeyBindings(bool)"));
	$.verticalSliders.connect($.invertedKeyBindings, SIGNAL("toggled(bool)"), SLOT("invertKeyBindings(bool)"));

	my $controlsLayout = new QGridLayout();
	$controlsLayout.addWidget($.minimumLabel, 0, 0);
	$controlsLayout.addWidget($.maximumLabel, 1, 0);
	$controlsLayout.addWidget($.valueLabel, 2, 0);
	$controlsLayout.addWidget($.minimumSpinBox, 0, 1);
	$controlsLayout.addWidget($.maximumSpinBox, 1, 1);
	$controlsLayout.addWidget($.valueSpinBox, 2, 1);
	$controlsLayout.addWidget($.invertedAppearance, 0, 2);
	$controlsLayout.addWidget($.invertedKeyBindings, 1, 2);
	$controlsLayout.addWidget($.orientationCombo, 3, 0, 1, 3);
	$.controlsGroup.setLayout($controlsLayout);
    }
}

class SlidersGroup inherits QGroupBox
{
     constructor($orientation, $title, $parent)  : QGroupBox($title, $parent)
     {
	 $.createSignal("valueChanged(int)");

	 $.slider = new QSlider($orientation);
	 $.slider.setFocusPolicy(Qt::StrongFocus);
	 $.slider.setTickPosition(QSlider::TicksBothSides);
	 $.slider.setTickInterval(10);
	 $.slider.setSingleStep(1);

	 $.scrollBar = new QScrollBar($orientation);
	 $.scrollBar.setFocusPolicy(Qt::StrongFocus);

	 $.dial = new QDial();
	 $.dial.setFocusPolicy(Qt::StrongFocus);

	 $.scrollBar.connect($.slider, SIGNAL("valueChanged(int)"), SLOT("setValue(int)"));
	 $.dial.connect($.scrollBar, SIGNAL("valueChanged(int)"), SLOT("setValue(int)"));
	 $.slider.connect($.dial, SIGNAL("valueChanged(int)"), SLOT("setValue(int)"));
	 $.connect($.dial, SIGNAL("valueChanged(int)"), SIGNAL("valueChanged(int)"));

	 my $direction;

	 if ($orientation == Qt::Horizontal)
	     $direction = QBoxLayout::TopToBottom;
	 else
	     $direction = QBoxLayout::LeftToRight;

	 my $slidersLayout = new QBoxLayout($direction);
	 $slidersLayout.addWidget($.slider);
	 $slidersLayout.addWidget($.scrollBar);
	 $slidersLayout.addWidget($.dial);
	 $.setLayout($slidersLayout);
     }

     setValue($value)
     {
	 $.slider.setValue($value);
     }

     setMinimum($value)
     {
	 $.slider.setMinimum($value);
	 $.scrollBar.setMinimum($value);
	 $.dial.setMinimum($value);
     }

     setMaximum($value)
     {
	 $.slider.setMaximum($value);
	 $.scrollBar.setMaximum($value);
	 $.dial.setMaximum($value);
     }

     invertAppearance($invert)
     {
	 $.slider.setInvertedAppearance($invert);
	 $.scrollBar.setInvertedAppearance($invert);
	 $.dial.setInvertedAppearance($invert);
     }

     invertKeyBindings($invert)
     {
	 $.slider.setInvertedControls($invert);
	 $.scrollBar.setInvertedControls($invert);
	 $.dial.setInvertedControls($invert);
     }
}

class sliders_example inherits QApplication
{
    constructor()
    {
	my $window = new Window();
	$window.show();
	$.exec();
    }
}
