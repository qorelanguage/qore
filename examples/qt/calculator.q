#!/usr/bin/env qore

# This is bascially a direct port of the QT widget example
# "calculator" to Qore using Qore's "qt" module.  

# Note that Qore's "qt" module requires QT 4.3 or above 

# use the "qt" module
%requires qt

# this is an object-oriented program, the application class is "calculator_example"
%exec-class calculator_example
# require all variables to be explicitly  declared
%require-our
# enable all parse warnings
%enable-all-warnings

const NumDigitButtons = 10;

class Button inherits QToolButton
{
    constructor($text, $color, $parent) : QToolButton($parent)
    {
	$.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	$.setText($text);
	
	my $newPalette = new QPalette();
	#QPalette newPalette = palette();
	$newPalette.setColor(QPalette::Button, $color);
	$.setPalette($newPalette);
    }

    sizeHint()
    {
	my $size = QToolButton::$.sizeHint();
	$size.setHeight($size.height() + 20);
	$size.setWidth(max($size.width(), $size.height()));
	return $size;
    }
}

class Calculator inherits QDialog
{
    constructor($parent) : QDialog($parent)
    {
	$.sumInMemory = 0.0;
	$.sumSoFar = 0.0;
	$.factorSoFar = 0.0;
	$.waitingForOperand = True;

	$.display = new QLineEdit("0");
	$.display.setReadOnly(True);
	$.display.setAlignment(Qt::AlignRight);
	$.display.setMaxLength(15);

	my $font = $.display.font();
	$font.setPointSize($font.pointSize() + 8);
	$.display.setFont($font);

	my $digitColor = new QColor(150, 205, 205);
	my $backspaceColor = new QColor(225, 185, 135);
	my $memoryColor = new QColor(100, 155, 155);
	my $operatorColor = new QColor(155, 175, 195);

	for (my $i = 0; $i < NumDigitButtons; ++$i) {
	    $.digitButtons[$i] = $.createButton(string($i), $digitColor, SLOT("digitClicked()"));
	}

	my $pointButton = $.createButton(TR("."), $digitColor, SLOT("pointClicked()"));
	my $changeSignButton = $.createButton("±", $digitColor, SLOT("changeSignClicked()"));

	my $backspaceButton = $.createButton(TR("Backspace"), $backspaceColor, SLOT("backspaceClicked()"));
	my $clearButton = $.createButton(TR("Clear"), $backspaceColor, SLOT("clear()"));
	my $clearAllButton = $.createButton(TR("Clear All"), $backspaceColor.light(120), SLOT("clearAll()"));

	my $clearMemoryButton = $.createButton(TR("MC"), $memoryColor, SLOT("clearMemory()"));
	my $readMemoryButton = $.createButton(TR("MR"), $memoryColor, SLOT("readMemory()"));
	my $setMemoryButton = $.createButton(TR("MS"), $memoryColor, SLOT("setMemory()"));
	my $addToMemoryButton = $.createButton(TR("M+"), $memoryColor, SLOT("addToMemory()"));

	my $divisionButton = $.createButton($mult, $operatorColor, SLOT("multiplicativeOperatorClicked()"));
	my $timesButton = $.createButton($div, $operatorColor, SLOT("multiplicativeOperatorClicked()"));
	my $minusButton = $.createButton(TR("-"), $operatorColor, SLOT("additiveOperatorClicked()"));
	my $plusButton = $.createButton(TR("+"), $operatorColor, SLOT("additiveOperatorClicked()"));

	my $squareRootButton = $.createButton(TR("Sqrt"), $operatorColor, SLOT("unaryOperatorClicked()"));
	my $powerButton = $.createButton("x²", $operatorColor, SLOT("unaryOperatorClicked()"));
	my $reciprocalButton = $.createButton(TR("1/x"), $operatorColor, SLOT("unaryOperatorClicked()"));
	my $equalButton = $.createButton(TR("="), $operatorColor.light(120), SLOT("equalClicked()"));

	my $mainLayout = new QGridLayout();
	$mainLayout.setSizeConstraint(QLayout::SetFixedSize);

	$mainLayout.addWidget($.display, 0, 0, 1, 6);
	$mainLayout.addWidget($backspaceButton, 1, 0, 1, 2);
	$mainLayout.addWidget($clearButton, 1, 2, 1, 2);
	$mainLayout.addWidget($clearAllButton, 1, 4, 1, 2);

	$mainLayout.addWidget($clearMemoryButton, 2, 0);
	$mainLayout.addWidget($readMemoryButton, 3, 0);
	$mainLayout.addWidget($setMemoryButton, 4, 0);
	$mainLayout.addWidget($addToMemoryButton, 5, 0);

	for (my $i = 1; $i < NumDigitButtons; ++$i) {
	    my $row = ((9 - $i) / 3) + 2;
	    my $column = (($i - 1) % 3) + 1;
	    $mainLayout.addWidget($.digitButtons[$i], $row, $column);
	}

	$mainLayout.addWidget($.digitButtons[0], 5, 1);
	$mainLayout.addWidget($pointButton, 5, 2);
	$mainLayout.addWidget($changeSignButton, 5, 3);

	$mainLayout.addWidget($divisionButton, 2, 4);
	$mainLayout.addWidget($timesButton, 3, 4);
	$mainLayout.addWidget($minusButton, 4, 4);
	$mainLayout.addWidget($plusButton, 5, 4);

	$mainLayout.addWidget($squareRootButton, 2, 5);
	$mainLayout.addWidget($powerButton, 3, 5);
	$mainLayout.addWidget($reciprocalButton, 4, 5);
	$mainLayout.addWidget($equalButton, 5, 5);
	$.setLayout($mainLayout);

	$.setWindowTitle(TR("Calculator"));
    }

    digitClicked()
    {
	my $clickedButton = $.sender();
	my $digitValue = float($clickedButton.text());
	if ($.display.text() == "0" && $digitValue == 0.0)
	    return;
	
	if ($.waitingForOperand) {
	    $.display.clear();
	    $.waitingForOperand = False;
	}
	$.display.setText($.display.text() + string($digitValue));
    }

    unaryOperatorClicked()
    {
	my $clickedButton = $.sender();
	my $clickedOperator = $clickedButton.text();
	my $operand = float($.display.text());
	my $result = 0.0;

	if ($clickedOperator == TR("Sqrt")) {
	    if ($operand < 0.0) {
		$.abortOperation();
		return;
	    }
	    $result = sqrt($operand);
	} else if ($clickedOperator == TR("x\262")) {
	    $result = pow($operand, 2.0);
	} else if ($clickedOperator == TR("1/x")) {
	    if ($operand == 0.0) {
		$.abortOperation();
		return;
	    }
	    $result = 1.0 / $operand;
	}
	$.display.setText($result);
	$.waitingForOperand = True;
    }

    additiveOperatorClicked()
    {
	my $clickedButton = $.sender();
	my $clickedOperator = $clickedButton.text();
	my $operand = float($.display.text());

	if (strlen($.pendingMultiplicativeOperator)) {
	    if (!$.calculate($operand, $.pendingMultiplicativeOperator)) {
		$.abortOperation();
		return;
	    }
	    $.display.setText($.factorSoFar);
	    $operand = $.factorSoFar;
	    $.factorSoFar = 0.0;
	    $.pendingMultiplicativeOperator = "";
	}

	if (strlen($.pendingAdditiveOperator)) {
	    if (!$.calculate($operand, $.pendingAdditiveOperator)) {
		$.abortOperation();
		return;
	    }
	    $.display.setText($.sumSoFar);
	} else {
	    $.sumSoFar = $operand;
	}

	$.pendingAdditiveOperator = $clickedOperator;
	$.waitingForOperand = True;
    }

    multiplicativeOperatorClicked()
    {
	my $clickedButton = $.sender();
	my $clickedOperator = $clickedButton.text();
	my $operand = float($.display.text());

	if (strlen($.pendingMultiplicativeOperator)) {
	    if (!$.calculate($operand, $.pendingMultiplicativeOperator)) {
		$.abortOperation();
		return;
	    }
	    $.display.setText($.factorSoFar);
	} else {
	    $.factorSoFar = $operand;
	}

	$.pendingMultiplicativeOperator = $clickedOperator;
	$.waitingForOperand = True;
    }

    equalClicked()
    {
	my $operand = float($.display.text());

	if (strlen($.pendingMultiplicativeOperator)) {
	    if (!$.calculate($operand, $.pendingMultiplicativeOperator)) {
		$.abortOperation();
		return;
	    }
	    $operand = $.factorSoFar;
	    $.factorSoFar = 0.0;
	    $.pendingMultiplicativeOperator = "";
	}
	if (strlen($.pendingAdditiveOperator)) {
	    if (!$.calculate($operand, $.pendingAdditiveOperator)) {
		$.abortOperation();
		return;
	    }
	    $.pendingAdditiveOperator = "";
	} else {
	    $.sumSoFar = $operand;
	}

	$.display.setText($.sumSoFar);
	$.sumSoFar = 0.0;
	$.waitingForOperand = True;
    }

    pointClicked()
    {
	if ($.waitingForOperand)
	    $.display.setText("0");
	if ($.display.text() !~ /\./)
	    $.display.setText($.display.text() + TR("."));
	$.waitingForOperand = False;
    }

    changeSignClicked()
    {
	my $text = $.display.text();
	my $value = float($text);

	if ($value > 0.0) {
	    $text = TR("-") + $text;
	} else if ($value < 0.0) {
	    splice $text, 0, 1;
	}
	$.display.setText($text);
    }

    backspaceClicked()
    {
	if ($.waitingForOperand)
	    return;

	my $text = $.display.text();
	splice $text, -1;
	if (!strlen($text)) {
	    $text = "0";
	    $.waitingForOperand = True;
	}
	$.display.setText($text);
    }

    clear()
    {
	if ($.waitingForOperand)
	    return;

	$.display.setText("0");
	$.waitingForOperand = True;
    }

    clearAll()
    {
	$.sumSoFar = 0.0;
	$.factorSoFar = 0.0;
	$.pendingAdditiveOperator = "";
	$.pendingMultiplicativeOperator = "";
	$.display.setText("0");
	$.waitingForOperand = True;
    }

    clearMemory()
    {
	$.sumInMemory = 0.0;
    }

    readMemory()
    {
	$.display.setText($.sumInMemory);
	$.waitingForOperand = True;
    }

    setMemory()
    {
	$.equalClicked();
	$.sumInMemory = float($.display.text());
    }

    addToMemory()
    {
	$.equalClicked();
	$.sumInMemory += float($.display.text());
    }

    createButton($text, $color, $member)
    {
	my $button = new Button($text, $color);
	$.connect($button, SIGNAL("clicked()"), $member);
	return $button;
    }

    abortOperation()
    {
	$.clearAll();
	$.display.setText(TR("####"));
    }

    calculate($rightOperand, $pendingOperator)
    {
	#printf("%n\n", $pendingOperator);
	if ($pendingOperator == TR("+")) {
	    $.sumSoFar += $rightOperand;
	} else if ($pendingOperator == TR("-")) {
	    $.sumSoFar -= $rightOperand;
	} else if ($pendingOperator == $mult) {
	    $.factorSoFar *= $rightOperand;
	} else if ($pendingOperator == $div) {
	    if ($rightOperand == 0.0)
		return False;
	    $.factorSoFar /= $rightOperand;
	}
	return True;
    }
}

class calculator_example inherits QApplication
{
    constructor() 
    {
	our $mult = "×";
	our $div  = "÷";

	my $calc = new Calculator();
	$calc.show();
	$.exec();
    }
}
