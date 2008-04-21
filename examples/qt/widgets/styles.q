#!/usr/bin/env qore

# this is basically a direct port of the QT widget example
# "styles" to Qore using Qore's "qt" module.  

# Note that Qore's "qt" module requires QT 4.3 or above 

# use the "qt" module
%requires qt

# this is an object-oriented program; the application class is "styles_example"
%exec-class styles_example
# require all variables to be explicitly declared
%require-our
# enable all parse warnings
%enable-all-warnings

sub qSwap($a, $b)
{
    my $t = $a;
    $a = $b;
    $b = $t;
}

class NorwegianWoodStyle inherits QMotifStyle
{
    polish($palette)
    {
	if (!($palette instanceof QPalette)) {
	    if ($palette instanceof QPushButton || $palette instanceof QComboBox)
		$palette.setAttribute(Qt::WA_Hover, True);
	    return;
	}
	my $brown = new QColor(212, 140, 95);
	my $beige = new QColor(236, 182, 120);
	my $slightlyOpaqueBlack = new QColor(0, 0, 0, 63);

	my $backgroundImage = new QPixmap($dir + "images/woodbackground.png");
	my $buttonImage = new QPixmap($dir + "images/woodbutton.png");
	my $midImage = $buttonImage.copy();
        
	my $painter = new QPainter();
	$painter.begin($midImage);
	$painter.setPen(Qt::NoPen);
	$painter.fillRect($midImage.rect(), $slightlyOpaqueBlack);
	$painter.end();

	$palette.set(new QPalette($brown));

	$palette.setBrush(QPalette::BrightText, Qt::white);
	$palette.setBrush(QPalette::Base, $beige);
	$palette.setBrush(QPalette::Highlight, Qt::darkGreen);
	$.setTexture($palette, QPalette::Button, $buttonImage);
	$.setTexture($palette, QPalette::Mid, $midImage);
	$.setTexture($palette, QPalette::Window, $backgroundImage);

	my $brush = $palette.background();
	$brush.setColor($brush.color().dark());

	$palette.setBrush(QPalette::Disabled, QPalette::WindowText, $brush);
	$palette.setBrush(QPalette::Disabled, QPalette::Text, $brush);
	$palette.setBrush(QPalette::Disabled, QPalette::ButtonText, $brush);
	$palette.setBrush(QPalette::Disabled, QPalette::Base, $brush);
	$palette.setBrush(QPalette::Disabled, QPalette::Button, $brush);
	$palette.setBrush(QPalette::Disabled, QPalette::Mid, $brush);
   }

    unpolish($widget)
    {
	if ($widget instanceof QPushButton || $widget instanceof QComboBox)
	    $widget.setAttribute(Qt::WA_Hover, False);
    }

    pixelMetric($metric, $option, $widget)
    {
	switch ($metric) {
	    case PM_ComboBoxFrameWidth:
	        return 8;
	    case PM_ScrollBarExtent:
	        return QMotifStyle::$.pixelMetric($metric, $option, $widget) + 4;
	    default:
	        return QMotifStyle::$.pixelMetric($metric, $option, $widget);
	}
    }

    styleHint($hint, $option, $widget, $returnData)
    {
	switch ($hint) {
	    case SH_DitherDisabledText:
	        return False;
	    case SH_EtchDisabledText:
	        return True;
	    default:
	        return QMotifStyle::$.styleHint($hint, $option, $widget, \$returnData);
	}
    }
    
    drawPrimitive($element, $option, $painter, $widget)
    {
	switch ($element) {
	    case PE_PanelButtonCommand:
	    {
		my $delta = ($option.state() & State_MouseOver) ? 64 : 0;
		my $slightlyOpaqueBlack = new QColor(0, 0, 0, 63);
		my $semiTransparentWhite = new QColor(255, 255, 255, 127 + $delta);
		my $semiTransparentBlack = new QColor(0, 0, 0, 127 - $delta);
		
		my $rect = $option.rect();
		my ($x, $y, $width, $height) = $rect.getRect();

		my $roundRect = $.roundRectPath($rect);
		my $radius = min($width, $height) / 2;

		my $brush;
		my $darker;
	    
		if ($option instanceof QStyleOptionButton && ($option.features() & QStyleOptionButton::Flat)) {
		    $brush = $option.palette().background();
		    $darker = ($option.state() & (State_Sunken | State_On));
		} else {
		    if ($option.state() & (State_Sunken | State_On)) {
			$brush = $option.palette().mid();
			$darker = !($option.state() & State_Sunken);
		    } else {
			$brush = $option.palette().button();
			$darker = False;
		    }
		}
		
		$painter.save();
		$painter.setRenderHint(QPainter::Antialiasing, True);
		$painter.fillPath($roundRect, $brush);
		if ($darker)
		    $painter.fillPath($roundRect, $slightlyOpaqueBlack);
		
		my $penWidth;
		if ($radius < 10)
		    $penWidth = 3;
		else if ($radius < 20)
		    $penWidth = 5;
		else
		    $penWidth = 7;
		
		my $topPen = new QPen($semiTransparentWhite, $penWidth);
		my $bottomPen = new QPen($semiTransparentBlack, $penWidth);
		
		if ($option.state() & (State_Sunken | State_On)) {
		    qSwap(\$topPen, \$bottomPen);
		}
		
		my $x1 = $x;
		my $x2 = $x + $radius;
		my $x3 = $x + $width - $radius;
		my $x4 = $x + $width;
		
		if ($option.direction() == Qt::RightToLeft) {
		    qSwap(\$x1, \$x4);
		    qSwap(\$x2, \$x3);
		}
		
		my $topHalf = new QPolygon((new QPoint($x1, $y), 
					    new QPoint($x4, $y),
					    new QPoint($x3, $y + $radius),
					    new QPoint($x2, $y + $height - $radius),
					    new QPoint($x1, $y + $height)));
		
		$painter.setClipPath($roundRect);
		$painter.setClipRegion(new QRegion($topHalf), Qt::IntersectClip);
		$painter.setPen($topPen);
		$painter.drawPath($roundRect);
		
		my $bottomHalf = $topHalf.copy();
		$bottomHalf.setPoint(0, new QPoint($x4, $y + $height));
		
		$painter.setClipPath($roundRect);
		$painter.setClipRegion(new QRegion($bottomHalf), Qt::IntersectClip);
		$painter.setPen($bottomPen);
		$painter.drawPath($roundRect);
		
		$painter.setPen($option.palette().foreground().color());
		$painter.setClipping(False);
		$painter.drawPath($roundRect);
		
		$painter.restore();
	    }
	    break;
	  default:
	    QMotifStyle::$.drawPrimitive($element, $option, $painter, $widget);
	}
    }
    
    drawControl($element, $option, $painter, $widget)
    {
	switch ($element) {
	    case CE_PushButtonLabel:
	    {
		my $myButtonOption;
		
		if ($option instanceof QStyleOptionButton) {
		    $myButtonOption = $option.copy();
		    if ($myButtonOption.palette().currentColorGroup() != QPalette::Disabled) {
			if ($myButtonOption.state() & (State_Sunken | State_On)) {
			    $myButtonOption.palette().setBrush(QPalette::ButtonText, $myButtonOption.palette().brightText());
			}
		    }
		}
		else
		    $myButtonOption = new QStyleOptionButton();
		QMotifStyle::$.drawControl($element, $myButtonOption, $painter, $widget);
	    }
	    break;
	    default:
	        QMotifStyle::$.drawControl($element, $option, $painter, $widget);
	}
    }

    setTexture($palette, $role, $pixmap)
    {
	for (my $i = 0; $i < QPalette::NColorGroups; ++$i) {
	    my $color = $palette.brush($i, $role).color();
	    my $brush = new QBrush($color);
	    $brush.setTexture($pixmap);
	    $palette.setBrush($i, $role, $brush);
	}
    }
    
    roundRectPath($rect)
    {
	my $radius = min($rect.width(), $rect.height()) / 2;
	my $diam = 2 * $radius;
	
	my ($x1, $y1, $x2, $y2) = $rect.getCoords();
	#printf("radius = %n: %n %n %n %n\n", $radius, $x1, $y1, $x2, $y2);
	
	my $path = new QPainterPath();
	$path.moveTo($x2, $y1 + $radius);
	$path.arcTo(new QRectF($x2 - $diam, $y1, $diam, $diam), 0.0, 90.0);
	$path.lineTo($x1 + $radius, $y1);
	$path.arcTo(new QRectF($x1, $y1, $diam, $diam), 90.0, 90.0);
	$path.lineTo($x1, $y2 - $radius);
	$path.arcTo(new QRectF($x1, $y2 - $diam, $diam, $diam), 180.0, 90.0);
	$path.lineTo($x1 + $radius, $y2);
	$path.arcTo(new QRectF($x2 - $diam, $y2 - $diam, $diam, $diam), 270.0, 90.0);
	$path.closeSubpath();
	return $path;
    }
}

class WidgetGallery inherits QDialog
{
    private $.originalPalette, $.styleLabel, $.styleComboBox, $.useStylePaletteCheckBox, 
       $.disableWidgetsCheckBox, $.topLeftGroupBox, $.radioButton1, $.radioButton2, $.radioButton3, 
       $.checkBox, $.topRightGroupBox, $.defaultPushButton, $.togglePushButton, $.flatPushButton, 
       $.bottomLeftTabWidget, $.tableWidget, $.textEdit, $.bottomRightGroupBox, $.lineEdit, 
       $.spinBox, $.dateTimeEdit, $.slider, $.scrollBar, $.dial, $.progressBar;

    constructor($parent) : QDialog($parent)
    {
	$.originalPalette = QApplication_palette();

	$.styleComboBox = new QComboBox();
	$.styleComboBox.addItem("NorwegianWood");
	$.styleComboBox.addItems(QStyleFactory_keys());

	$.styleLabel = new QLabel(TR("&Style:"));
	$.styleLabel.setBuddy($.styleComboBox);

	$.useStylePaletteCheckBox = new QCheckBox(TR("&Use style's standard palette"));
	$.useStylePaletteCheckBox.setChecked(True);

	$.disableWidgetsCheckBox = new QCheckBox(TR("&Disable widgets"));

	$.createTopLeftGroupBox();
	$.createTopRightGroupBox();
	$.createBottomLeftTabWidget();
	$.createBottomRightGroupBox();
	$.createProgressBar();

	$.connect($.styleComboBox, SIGNAL("activated(const QString &)"), SLOT("changeStyle(const QString &)"));
	$.connect($.useStylePaletteCheckBox, SIGNAL("toggled(bool)"), SLOT("changePalette()"));
	$.topLeftGroupBox.connect($.disableWidgetsCheckBox, SIGNAL("toggled(bool)"), SLOT("setDisabled(bool)"));
	$.topRightGroupBox.connect($.disableWidgetsCheckBox, SIGNAL("toggled(bool)"), SLOT("setDisabled(bool)"));
	$.bottomLeftTabWidget.connect($.disableWidgetsCheckBox, SIGNAL("toggled(bool)"), SLOT("setDisabled(bool)"));
	$.bottomRightGroupBox.connect($.disableWidgetsCheckBox, SIGNAL("toggled(bool)"), SLOT("setDisabled(bool)"));

	my $topLayout = new QHBoxLayout();
	$topLayout.addWidget($.styleLabel);
	$topLayout.addWidget($.styleComboBox);
	$topLayout.addStretch(1);
	$topLayout.addWidget($.useStylePaletteCheckBox);
	$topLayout.addWidget($.disableWidgetsCheckBox);

	my $mainLayout = new QGridLayout();
	$mainLayout.addLayout($topLayout, 0, 0, 1, 2);
	$mainLayout.addWidget($.topLeftGroupBox, 1, 0);
	$mainLayout.addWidget($.topRightGroupBox, 1, 1);
	$mainLayout.addWidget($.bottomLeftTabWidget, 2, 0);
	$mainLayout.addWidget($.bottomRightGroupBox, 2, 1);
	$mainLayout.addWidget($.progressBar, 3, 0, 1, 2);
	$mainLayout.setRowStretch(1, 1);
	$mainLayout.setRowStretch(2, 1);
	$mainLayout.setColumnStretch(0, 1);
	$mainLayout.setColumnStretch(1, 1);
	$.setLayout($mainLayout);

	$.setWindowTitle(TR("Styles"));
	$.changeStyle("NorwegianWood");
    }

    changeStyle($styleName)
    {
	if ($styleName == "NorwegianWood") {
	    QApplication_setStyle(new NorwegianWoodStyle());
	} else {
	    QApplication_setStyle(QStyleFactory_create($styleName));
	}
	$.changePalette();
    }

    changePalette()
    {
	if ($.useStylePaletteCheckBox.isChecked())
	    QApplication_setPalette(QApplication_style().standardPalette());
	else
	    QApplication_setPalette($.originalPalette);
    }

    advanceProgressBar()
    {
	my $curVal = $.progressBar.value();
	my $maxVal = $.progressBar.maximum();
	$.progressBar.setValue($curVal + ($maxVal - $curVal) / 100);
    }

    createTopLeftGroupBox()
    {
	$.topLeftGroupBox = new QGroupBox(TR("Group 1"));

	$.radioButton1 = new QRadioButton(TR("Radio button 1"));
	$.radioButton2 = new QRadioButton(TR("Radio button 2"));
	$.radioButton3 = new QRadioButton(TR("Radio button 3"));
	$.radioButton1.setChecked(True);

	$.checkBox = new QCheckBox(TR("Tri-state check box"));
	$.checkBox.setTristate(True);
	$.checkBox.setCheckState(Qt::PartiallyChecked);

	my $layout = new QVBoxLayout();
	$layout.addWidget($.radioButton1);
	$layout.addWidget($.radioButton2);
	$layout.addWidget($.radioButton3);
	$layout.addWidget($.checkBox);
	$layout.addStretch(1);
	$.topLeftGroupBox.setLayout($layout);
    }

    createTopRightGroupBox()
    {
	$.topRightGroupBox = new QGroupBox(TR("Group 2"));

	$.defaultPushButton = new QPushButton(TR("Default Push Button"));
	$.defaultPushButton.setDefault(True);

	$.togglePushButton = new QPushButton(TR("Toggle Push Button"));
	$.togglePushButton.setCheckable(True);
	$.togglePushButton.setChecked(True);

	$.flatPushButton = new QPushButton(TR("Flat Push Button"));
	$.flatPushButton.setFlat(True);

	my $layout = new QVBoxLayout();
	$layout.addWidget($.defaultPushButton);
	$layout.addWidget($.togglePushButton);
	$layout.addWidget($.flatPushButton);
	$layout.addStretch(1);
	$.topRightGroupBox.setLayout($layout);
    }

    createBottomLeftTabWidget()
    {
	$.bottomLeftTabWidget = new QTabWidget();
	$.bottomLeftTabWidget.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);

	my $tab1 = new QWidget();
	$.tableWidget = new QTableWidget(10, 10);

	my $tab1hbox = new QHBoxLayout();
	$tab1hbox.setMargin(5);
	$tab1hbox.addWidget($.tableWidget);
	$tab1.setLayout($tab1hbox);

	my $tab2 = new QWidget();
	$.textEdit = new QTextEdit();

	$.textEdit.setPlainText(TR("Twinkle, twinkle, little star,\n"
				 "How I wonder what you are.\n"
				 "Up above the world so high,\n"
				 "Like a diamond in the sky.\n"
				 "Twinkle, twinkle, little star,\n"
				 "How I wonder what you are!\n"));

	my $tab2hbox = new QHBoxLayout();
	$tab2hbox.setMargin(5);
	$tab2hbox.addWidget($.textEdit);
	$tab2.setLayout($tab2hbox);

	$.bottomLeftTabWidget.addTab($tab1, TR("&Table"));
	$.bottomLeftTabWidget.addTab($tab2, TR("Text &Edit"));
    }

    createBottomRightGroupBox()
    {
	$.bottomRightGroupBox = new QGroupBox(TR("Group 3"));
	$.bottomRightGroupBox.setCheckable(True);
	$.bottomRightGroupBox.setChecked(True);

	$.lineEdit = new QLineEdit("s3cRe7");
	$.lineEdit.setEchoMode(QLineEdit::Password);

	$.spinBox = new QSpinBox($.bottomRightGroupBox);
	$.spinBox.setValue(50);

	$.dateTimeEdit = new QDateTimeEdit($.bottomRightGroupBox);
	$.dateTimeEdit.setDateTime(now());

	$.slider = new QSlider(Qt::Horizontal, $.bottomRightGroupBox);
	$.slider.setValue(40);
       
	$.scrollBar = new QScrollBar(Qt::Horizontal, $.bottomRightGroupBox);
	$.scrollBar.setValue(60);

	$.dial = new QDial($.bottomRightGroupBox);
	$.dial.setValue(30);
	$.dial.setNotchesVisible(True);

	my $layout = new QGridLayout();
	$layout.addWidget($.lineEdit, 0, 0, 1, 2);
	$layout.addWidget($.spinBox, 1, 0, 1, 2);
	$layout.addWidget($.dateTimeEdit, 2, 0, 1, 2);
	$layout.addWidget($.slider, 3, 0);
	$layout.addWidget($.scrollBar, 4, 0);
	$layout.addWidget($.dial, 3, 1, 2, 1);
	$layout.setRowStretch(5, 1);
	$.bottomRightGroupBox.setLayout($layout);
    }

    createProgressBar()
    {
	$.progressBar = new QProgressBar();
	$.progressBar.setRange(0, 10000);
	$.progressBar.setValue(0);

	my $timer = new QTimer($self);
	$.connect($timer, SIGNAL("timeout()"), SLOT("advanceProgressBar()"));
	$timer.start(1000);
    }
}

class styles_example inherits QApplication
{
    constructor()
    {
	# get script directory for loading resources
	our $dir = get_script_dir();
      
	# check for texture resources
	if (!is_file($dir + "images/woodbackground.png") || !is_file($dir + "images/woodbutton.png")) {
	    QMessageBox_information(0, TR("Style Example"), TR("Cannot load bitmap resources required for style; try running in the same directory as the script"));
	    exit(1);
	}

	my $gallery = new WidgetGallery();
	$gallery.show();
	$.exec();
    }
}
