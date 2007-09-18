#!/usr/bin/env qore

# This is basically a direct port of the QT widget example
# "charactermap" to Qore using Qore's "qt" module.  

# Note that Qore's "qt" module requires QT 4.3 or above 

# use the "qt" module
%requires qt

# this is an object-oriented program, the application class is "charactermap_example"
%exec-class charactermap_example
# require all variables to be explicitly  declared
%require-our
# enable all parse warnings
%enable-all-warnings

class CharacterWidget inherits QWidget
{
    constructor($parent) : QWidget($parent)
    {
	$.createSignal("characterSelected(const QString &)");
	$.displayFont = new QFont();

	$.squareSize = 24;
	$.columns = 32;
	$.lastKey = -1;
	$.setMouseTracking(True);
    }

    updateFont($font)
    {
	$.displayFont.setFamily($font.family());
	$.squareSize = max(24, (new QFontMetrics($.displayFont)).xHeight() * 3);
	$.adjustSize();
	$.update();
    }

    updateSize($fontSize)
    {
	$.displayFont.setPointSize($fontSize);
	$.squareSize = max(24, (new QFontMetrics($.displayFont)).xHeight() * 3);
	$.adjustSize();
	$.update();
    }

    updateStyle($fontStyle)
    {
	my $fontDatabase = new QFontDatabase();
	my $oldStrategy = $.displayFont.styleStrategy();
	$.displayFont = $fontDatabase.font($.displayFont.family(), $fontStyle, $.displayFont.pointSize());
	$.displayFont.setStyleStrategy($oldStrategy);
	$.squareSize = max(24, (new QFontMetrics($.displayFont)).xHeight() * 3);
	$.adjustSize();
	$.update();
    }

    updateFontMerging($enable)
    {
	if ($enable)
	    $.displayFont.setStyleStrategy(QFont::PreferDefault);
	else
	    $.displayFont.setStyleStrategy(QFont::NoFontMerging);
	$.adjustSize();
	$.update();
    }

    sizeHint()
    {
	return new QSize($.columns*$.squareSize, (65536/$.columns)*$.squareSize);
    }

    mouseMoveEvent($event)
    {
	my $widgetPosition = $.mapFromGlobal($event.globalPos());
	my $key = ($widgetPosition.y
		   ()/$.squareSize)*$.columns + $widgetPosition.x()/$.squareSize;

	my $text = sprintf("<p>Character: <span style=\"font-size: 24pt; font-family: %s\">%s</span><p>Value: 0x%x", 
			   $.displayFont.family(), $key, $key);
	QToolTip_showText($event.globalPos(), $text, $self);
    }

    mousePressEvent($event)
    {
	if ($event.button() == Qt::LeftButton) {
	    $.lastKey = ($event.y
			 ()/$.squareSize)*$.columns + $event.x()/$.squareSize;
	    my $lkc = new QChar($.lastKey);
	    if ($lkc.category() != QChar::NoCategory)
		$.emit("characterSelected(QString)", $lkc);
	    $.update();
	}
	else
	    QWidget::$.mousePressEvent($event);
    }

    paintEvent($event)
    {
	my $painter = new QPainter($self);
	$painter.fillRect($event.rect(), new QBrush(Qt::white));
	$painter.setFont($.displayFont);

	my $redrawRect = $event.rect();
	my $beginRow = $redrawRect.top()/$.squareSize;
	my $endRow = $redrawRect.bottom()/$.squareSize;
	my $beginColumn = $redrawRect.left()/$.squareSize;
	my $endColumn = $redrawRect.right()/$.squareSize;

	$painter.setPen(new QPen(Qt::gray));
	for (my $row = $beginRow; $row <= $endRow; ++$row) {
	    for (my $column = $beginColumn; $column <= $endColumn; ++$column) {
		$painter.drawRect($column*$.squareSize, $row*$.squareSize, $.squareSize, $.squareSize);
	    }
	}

	my $fontMetrics = new QFontMetrics($.displayFont);
	$painter.setPen(new QPen(Qt::black));
	for (my $row = $beginRow; $row <= $endRow; ++$row) {

	    for (my $column = $beginColumn; $column <= $endColumn; ++$column) {

		my $key = $row*$.columns + $column;
		$painter.setClipRect($column*$.squareSize, $row*$.squareSize, $.squareSize, $.squareSize);

		if ($key == $.lastKey)
		    $painter.fillRect($column*$.squareSize + 1, $row*$.squareSize + 1, $.squareSize, $.squareSize, new QBrush(Qt::red));

		$painter.drawText($column*$.squareSize + ($.squareSize / 2) - $fontMetrics.width(new QChar($key))/2,
				  $row*$.squareSize + 4 + $fontMetrics.ascent(),
				  new QChar($key));
	    }
	}
    }
}

class MainWindow inherits QMainWindow
{
    constructor()
    {
	my $centralWidget = new QWidget();

	my $fontLabel = new QLabel(TR("Font:"));
	$.fontCombo = new QFontComboBox();
	my $sizeLabel = new QLabel(TR("Size:"));
	$.sizeCombo = new QComboBox();
	my $styleLabel = new QLabel(TR("Style:"));
	$.styleCombo = new QComboBox();
	my $fontMergingLabel = new QLabel(TR("Automatic Font Merging:"));
	$.fontMerging = new QCheckBox();
	$.fontMerging.setChecked(True);

	$.scrollArea = new QScrollArea();
	$.characterWidget = new CharacterWidget();
	$.scrollArea.setWidget($.characterWidget);

	$.findStyles($.fontCombo.currentFont());
	$.findSizes($.fontCombo.currentFont());

	$.lineEdit = new QLineEdit();
	my $clipboardButton = new QPushButton(TR("&To clipboard"));

	$.clipboard = QApplication_clipboard();

	$.connect($.fontCombo, SIGNAL("currentFontChanged(const QFont &)"), SLOT("findStyles(const QFont &)"));
	$.connect($.fontCombo, SIGNAL("currentFontChanged(const QFont &)"), SLOT("findSizes(const QFont &)"));
	$.characterWidget.connect($.fontCombo, SIGNAL("currentFontChanged(const QFont &)"), SLOT("updateFont(const QFont &)"));
	$.characterWidget.connect($.sizeCombo, SIGNAL("currentIndexChanged(const QString &)"), SLOT("updateSize(const QString &)"));
	$.characterWidget.connect($.styleCombo, SIGNAL("currentIndexChanged(const QString &)"), SLOT("updateStyle(const QString &)"));
	$.connect($.characterWidget, SIGNAL("characterSelected(const QString &)"), SLOT("insertCharacter(const QString &)"));
	$.connect($clipboardButton, SIGNAL("clicked()"), SLOT("updateClipboard()"));
	$.characterWidget.connect($.fontMerging, SIGNAL("toggled(bool)"), SLOT("updateFontMerging(bool)"));

	my $controlsLayout = new QHBoxLayout();
	$controlsLayout.addWidget($fontLabel);
	$controlsLayout.addWidget($.fontCombo, 1);
	$controlsLayout.addWidget($sizeLabel);
	$controlsLayout.addWidget($.sizeCombo, 1);
	$controlsLayout.addWidget($styleLabel);
	$controlsLayout.addWidget($.styleCombo, 1);
	$controlsLayout.addWidget($fontMergingLabel);
	$controlsLayout.addWidget($.fontMerging, 1);
	$controlsLayout.addStretch(1);

	my $lineLayout = new QHBoxLayout();
	$lineLayout.addWidget($.lineEdit, 1);
	$lineLayout.addSpacing(12);
	$lineLayout.addWidget($clipboardButton);

	my $centralLayout = new QVBoxLayout();
	$centralLayout.addLayout($controlsLayout);
	$centralLayout.addWidget($.scrollArea, 1);
	$centralLayout.addSpacing(4);
	$centralLayout.addLayout($lineLayout);
	$centralWidget.setLayout($centralLayout);

	$.setCentralWidget($centralWidget);
	$.setWindowTitle(TR("Character Map"));
    }

    findStyles($font)
    {
	my $fontDatabase = new QFontDatabase();

	my $currentItem = $.styleCombo.currentText();
	$.styleCombo.clear();

	foreach my $style in ($fontDatabase.styles($font.family()))
	    $.styleCombo.addItem($style);

	my $styleIndex = $.styleCombo.findText($currentItem);

	if ($styleIndex == -1)
	    $.styleCombo.setCurrentIndex(0);
	else
	    $.styleCombo.setCurrentIndex($styleIndex);
    }

    findSizes($font)
    {
	my $fontDatabase = new QFontDatabase();

	my $currentSize = $.sizeCombo.currentText();
	$.sizeCombo.blockSignals(True);
	$.sizeCombo.clear();

	my $size;
	if ($fontDatabase.isSmoothlyScalable($font.family(), $fontDatabase.styleString($font))) {
	    foreach $size in (QFontDatabase_standardSizes()) {
		$.sizeCombo.addItem(string($size));
		$.sizeCombo.setEditable(True);
	    }

	} else {
	    foreach $size in ($fontDatabase.smoothSizes($font.family(), $fontDatabase.styleString($font))) {
		$.sizeCombo.addItem(string($size));
		$.sizeCombo.setEditable(False);
	    }
	}

	$.sizeCombo.blockSignals(False);

	my $sizeIndex = $.sizeCombo.findText($currentSize);

	if ($sizeIndex == -1)
	    $.sizeCombo.setCurrentIndex(max(0, $.sizeCombo.count() / 3));
	else
	    $.sizeCombo.setCurrentIndex($sizeIndex);
    }

    insertCharacter($character)
    {
	$.lineEdit.insert($character);
    }

    updateClipboard()
    {
	$.clipboard.setText($.lineEdit.text(), QClipboard::Clipboard);
	$.clipboard.setText($.lineEdit.text(), QClipboard::Selection);
    }

}

class charactermap_example inherits QApplication
{
    constructor()
    {
	my $window = new MainWindow();
	$window.show();
	$.exec();
    }
}
