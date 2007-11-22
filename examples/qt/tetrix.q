#!/usr/bin/env qore

# $self is bascially a direct port of the QT widget example
# "tetrix" to Qore using Qore's "qt" module.  

# Note that Qore's "qt" module requires QT 4.3 or above 

# use the "qt" module
%requires qt

# $self is an object-oriented program; the application class is "tetrix_example"
%exec-class tetrix_example
# require all variables to be explicitly declared
%require-our
# enable all parse warnings
%enable-all-warnings

namespace TetrixShape { 
    const NoShape = 0;
    const ZShape = 1;
    const SShape = 2;
    const LineShape = 3;
    const TShape = 4;
    const SquareShape = 5;
    const LShape = 6;
    const MirroredLShape = 7;
}

namespace TetrixBoard {
    const BoardWidth = 10;
    const BoardHeight = 22;
}

namespace TetrixPiece {
    const coordsTable = (
	    ( ( 0, 0 ),   ( 0, 0 ),   ( 0, 0 ),   ( 0, 0 ) ),
	    ( ( 0, -1 ),  ( 0, 0 ),   ( -1, 0 ),  ( -1, 1 ) ),
	    ( ( 0, -1 ),  ( 0, 0 ),   ( 1, 0 ),   ( 1, 1 ) ),
	    ( ( 0, -1 ),  ( 0, 0 ),   ( 0, 1 ),   ( 0, 2 ) ),
	    ( ( -1, 0 ),  ( 0, 0 ),   ( 1, 0 ),   ( 0, 1 ) ),
	    ( ( 0, 0 ),   ( 1, 0 ),   ( 0, 1 ),   ( 1, 1 ) ),
	    ( ( -1, -1 ), ( 0, -1 ),  ( 0, 0 ),   ( 0, 1 ) ),
	    ( ( 1, -1 ),  ( 0, -1 ),  ( 0, 0 ),   ( 0, 1 ) )
	);
}

class TetrixBoard::TetrixBoard inherits QFrame
{
    private $.timer, $.nextPieceLabel, $.isStarted, $.isPaused, $.isWaitingAfterLine, 
            $.curPiece, $.nextPiece, $.curX, $.curY, $.numLinesRemoved, $.numPiecesDropped,
            $.score, $.level, $.board;

    constructor($parent) : QFrame($parent)
    {
	$.timer = new QBasicTimer();
	$.nextPiece = new TetrixPiece();
	$.curPiece = new TetrixPiece();
	

	$.createSignal("scoreChanged(int)");
	$.createSignal("levelChanged(int)");
	$.createSignal("linesRemovedChanged(int)");

	$.setFrameStyle(QFrame::Panel | QFrame::Sunken);
	$.setFocusPolicy(Qt::StrongFocus);
	$.isStarted = False;
	$.isPaused = False;
	$.clearBoard();
	
	$.nextPiece.setRandomShape();
    }
    
    setNextPieceLabel($label)
    {
	$.nextPieceLabel = $label;
    }

    sizeHint()
    {
	return new QSize(BoardWidth * 15 + $.frameWidth() * 2,
			 BoardHeight * 15 + $.frameWidth() * 2);
    }

    minimumSizeHint()
    {
	return new QSize(BoardWidth * 5 + $.frameWidth() * 2,
			 BoardHeight * 5 + $.frameWidth() * 2);
    }

    start()
    {
	if ($.isPaused)
	    return;

	$.isStarted = True;
	$.isWaitingAfterLine = False;
	$.numLinesRemoved = 0;
	$.numPiecesDropped = 0;
	$.score = 0;
	$.level = 1;
	$.clearBoard();

	$.emit("linesRemovedChanged(int)", $.numLinesRemoved);
	$.emit("scoreChanged(int)", $.score);
	$.emit("levelChanged(int)", $.level);

	$.newPiece();
	$.timer.start($.timeoutTime(), $self);
    }

    pause()
    {
	if (!$.isStarted)
	    return;

	$.isPaused = !$.isPaused;
	if ($.isPaused) {
	    $.timer.stop();
	} else {
	    $.timer.start($.timeoutTime(), $self);
	}
	$.update();
    }

    paintEvent($event)
    {
	QFrame::$.paintEvent($event);

	my $painter = new QPainter($self);
	my $rect = $.contentsRect();

	if ($.isPaused) {
	    $painter.drawText($rect, Qt::AlignCenter, TR("Pause"));
	    return;
	}

	my $boardTop = $rect.bottom() - BoardHeight * $.squareHeight();

	for (my $i = 0; $i < BoardHeight; ++$i) {
	    for (my $j = 0; $j < BoardWidth; ++$j) {
		my $shape = $.shapeAt($j, BoardHeight - $i - 1);
		if ($shape != NoShape)
		    $.drawSquare($painter, $rect.left() + $j * $.squareWidth(),
				 $boardTop + $i * $.squareHeight(), $shape);
	    }
	}

	if ($.curPiece.shape() != NoShape) {
	    for (my $i = 0; $i < 4; ++$i) {
		my $x = $.curX + $.curPiece.x($i);
		my $y = $.curY - $.curPiece.y
		    ($i);
		$.drawSquare($painter, $rect.left() + $x * $.squareWidth(),
			     $boardTop + (BoardHeight - $y - 1) * $.squareHeight(),
			     $.curPiece.shape());
	    }
	}
    }
    
    keyPressEvent($event)
    {
	if (!$.isStarted || $.isPaused || $.curPiece.shape() == NoShape) {
	    QFrame::$.keyPressEvent($event);
	    return;
	}

	switch ($event.key()) {
	    case Qt::Key_Left: {
		$.tryMove($.curPiece, $.curX - 1, $.curY);
		break;
	    }
	    case Qt::Key_Right: {
		$.tryMove($.curPiece, $.curX + 1, $.curY);
		break;
	    }
	    case Qt::Key_Down: {
		$.tryMove($.curPiece.rotatedRight(), $.curX, $.curY);
		break;
	    }
	    case Qt::Key_Up: {
		$.tryMove($.curPiece.rotatedLeft(), $.curX, $.curY);
		break;
	    }
	    case Qt::Key_Space: {
		$.dropDown();
		break;
	    }
	    case Qt::Key_D: {
		$.oneLineDown();
		break;
	    }
	  default: {
	      QFrame::$.keyPressEvent($event);
	    }
	}
    }

    timerEvent($event)
    {
	if ($event.timerId() == $.timer.timerId()) {
	    if ($.isWaitingAfterLine) {
		$.isWaitingAfterLine = False;
		$.newPiece();
		$.timer.start($.timeoutTime(), $self);
	    } else {
		$.oneLineDown();
	    }
	} else {
	    QObject::$.timerEvent($event);
	}
    }

    clearBoard()
    {
	for (my $i = 0; $i < BoardHeight * BoardWidth; ++$i)
	    $.board[$i] = NoShape;
    }

    dropDown()
    {
	my $dropHeight = 0;
	my $newY = $.curY;
	while ($newY > 0) {
	    if (!$.tryMove($.curPiece, $.curX, $newY - 1))
		break;
	    --$newY;
	    ++$dropHeight;
	}
	$.pieceDropped($dropHeight);
    }

    oneLineDown()
    {
	if (!$.tryMove($.curPiece, $.curX, $.curY - 1))
	    $.pieceDropped(0);
    }

    pieceDropped($dropHeight)
    {
	for (my $i = 0; $i < 4; ++$i) {
	    my $x = $.curX + $.curPiece.x($i);
	    my $y = $.curY - $.curPiece.y
		($i);

	    $.board[($y * BoardWidth) + $x] = $.curPiece.shape();
	}

	++$.numPiecesDropped;
	if ($.numPiecesDropped % 25 == 0) {
	    ++$.level;
	    $.timer.start($.timeoutTime(), $self);
	    $.emit("levelChanged(int)", $.level);
	}

	$.score += $dropHeight + 7;
	$.emit("scoreChanged(int)", $.score);
	$.removeFullLines();

	if (!$.isWaitingAfterLine)
	    $.newPiece();
    }

    removeFullLines()
    {
	my $numFullLines = 0;

	for (my $i = BoardHeight - 1; $i >= 0; --$i) {
	    my $lineIsFull = True;

	    for (my $j = 0; $j < BoardWidth; ++$j) {
		if ($.shapeAt($j, $i) == NoShape) {
		    $lineIsFull = False;
		    break;
		}
	    }

	    if ($lineIsFull) {
		++$numFullLines;
		for (my $k = $i; $k < BoardHeight - 1; ++$k) {
		    for (my $j = 0; $j < BoardWidth; ++$j)
			$.board[($k * BoardWidth) + $j] = $.board[(($k + 1) * BoardWidth) + $j];
		}
		for (my $j = 0; $j < BoardWidth; ++$j)
		    $.board[((BoardHeight - 1) * BoardWidth) + $j] = NoShape;
	    }
	}

	if ($numFullLines > 0) {
	    $.numLinesRemoved += $numFullLines;
	    $.score += 10 * $numFullLines;
	    $.emit("linesRemovedChanged(int)", $.numLinesRemoved);
	    $.emit("scoreChanged(int)", $.score);

	    $.timer.start(500, $self);
	    $.isWaitingAfterLine = True;
	    $.curPiece.setShape(NoShape);
	    $.update();
	}
    }

    newPiece()
    {
	$.curPiece = $.nextPiece.copy();
	$.nextPiece.setRandomShape();
	$.showNextPiece();
	$.curX = BoardWidth / 2 + 1;
	$.curY = BoardHeight - 1 + $.curPiece.minY();

	if (!$.tryMove($.curPiece, $.curX, $.curY)) {
	    $.curPiece.setShape(NoShape);
	    $.timer.stop();
	    $.isStarted = False;
	}
    }

    showNextPiece()
    {
	if (!exists $.nextPieceLabel)
	    return;

	my $dx = $.nextPiece.maxX() - $.nextPiece.minX() + 1;
	my $dy = $.nextPiece.maxY() - $.nextPiece.minY() + 1;
	
	my $pixmap = new QPixmap($dx * $.squareWidth(), $dy * $.squareHeight());
	my $painter = new QPainter($pixmap);
	$painter.fillRect($pixmap.rect(), $.nextPieceLabel.palette().background());

	for (my $i = 0; $i < 4; ++$i) {
	    my $x = $.nextPiece.x($i) - $.nextPiece.minX();
	    my $y = $.nextPiece.y
		($i) - $.nextPiece.minY();
	    $.drawSquare($painter, $x * $.squareWidth(), $y * $.squareHeight(), $.nextPiece.shape());
	}
	$.nextPieceLabel.setPixmap($pixmap);
	#delete $painter;
    }

    tryMove($newPiece, $newX, $newY)
    {
	for (my $i = 0; $i < 4; ++$i) {
	    my $x = $newX + $newPiece.x($i);
	    my $y = $newY - $newPiece.y
		($i);
	    if ($x < 0 || $x >= BoardWidth || $y < 0 || $y >= BoardHeight)
		return False;
	    if ($.shapeAt($x, $y) != NoShape)
		return False;
	}

	$.curPiece = $newPiece;
	$.curX = $newX;
	$.curY = $newY;
	$.update();
	return True;
    }

    drawSquare($painter, $x, $y, $shape)
    {
	my $color = $colorTable[$shape];
	$painter.fillRect($x + 1, $y + 1, $.squareWidth() - 2, $.squareHeight() - 2, $color);

	$painter.setPen($color.light());
	$painter.drawLine($x, $y + $.squareHeight() - 1, $x, $y);
	$painter.drawLine($x, $y, $x + $.squareWidth() - 1, $y);

	$painter.setPen($color.dark());
	$painter.drawLine($x + 1, $y + $.squareHeight() - 1,
			  $x + $.squareWidth() - 1, $y + $.squareHeight() - 1);
	$painter.drawLine($x + $.squareWidth() - 1, $y + $.squareHeight() - 1,
			  $x + $.squareWidth() - 1, $y + 1);
    }

    private shapeAt($x, $y) { return $.board[($y * BoardWidth) + $x]; }
    private timeoutTime() { return 1000 / (1 + $.level); }
    private squareWidth() { return $.contentsRect().width() / BoardWidth; }
    private squareHeight() { return $.contentsRect().height() / BoardHeight; }
}

class TetrixPiece::TetrixPiece
{
    private $.pieceShape, $.coords;

   uctor() { $.setShape(NoShape); }

    shape() { return $.pieceShape; }
    x($index) { return $.coords[$index][0]; }
    y
	($index) { return $.coords[$index][1]; }

    setRandomShape()
    {
	$.setShape(qrand() % 7 + 1);
    }
    
    setShape($shape)
    {
	for (my $i = 0; $i < 4 ; $i++) {
	    for (my $j = 0; $j < 2; ++$j)
		$.coords[$i][$j] = coordsTable[$shape][$i][$j];
	}
	$.pieceShape = $shape;
    }

    minX()
    {
	my $min = $.coords[0][0];
	for (my $i = 1; $i < 4; ++$i)
	    $min = min($min, $.coords[$i][0]);
	return $min;
    }

    maxX()
    {
	my $max = $.coords[0][0];
	for (my $i = 1; $i < 4; ++$i)
	    $max = max($max, $.coords[$i][0]);
	return $max;
    }

    minY()
    {
	my $min = $.coords[0][1];
	for (my $i = 1; $i < 4; ++$i)
	    $min = min($min, $.coords[$i][1]);
	return $min;
    }

    maxY()
    {
	my $max = $.coords[0][1];
	for (my $i = 1; $i < 4; ++$i)
	    $max = max($max, $.coords[$i][1]);
	return $max;
    }

    rotatedLeft()
    {
	if ($.pieceShape == SquareShape)
	    return $self;
	
	my $result = new TetrixPiece();
	$result.pieceShape = $.pieceShape;
	for (my $i = 0; $i < 4; ++$i) {
	    $result.setX($i, $.y
			 ($i));
	    $result.setY($i, -$.x($i));
	}
	return $result;
    }

    rotatedRight()
    {
	if ($.pieceShape == SquareShape)
	    return $self;
	
	my $result = new TetrixPiece();
	$result.pieceShape = $.pieceShape;
	for (my $i = 0; $i < 4; ++$i) {
	    $result.setX($i, -$.y
			 ($i));
	    $result.setY($i, $.x($i));
	}
	return $result;
    }
    private setX($index, $x) { $.coords[$index][0] = $x; }
    private setY($index, $y) { $.coords[$index][1] = $y; }
}

class TetrixWindow inherits QWidget
{
    private $.board, $.nextPieceLabel, $.scoreLcd, $.levelLcd,
            $.linesLcd, $.startButton, $.quitButton, $.pauseButton;

    constructor()
    {
	$.board = new TetrixBoard();

	$.nextPieceLabel = new QLabel();
	$.nextPieceLabel.setFrameStyle(QFrame::Box | QFrame::Raised);
	$.nextPieceLabel.setAlignment(Qt::AlignCenter);
	$.board.setNextPieceLabel($.nextPieceLabel);

	$.scoreLcd = new QLCDNumber(5);
	$.scoreLcd.setSegmentStyle(QLCDNumber::Filled);
	$.levelLcd = new QLCDNumber(2);
	$.levelLcd.setSegmentStyle(QLCDNumber::Filled);
	$.linesLcd = new QLCDNumber(5);
	$.linesLcd.setSegmentStyle(QLCDNumber::Filled);

	$.startButton = new QPushButton(TR("&Start"));
	$.startButton.setFocusPolicy(Qt::NoFocus);
	$.quitButton = new QPushButton(TR("&Quit"));
	$.quitButton.setFocusPolicy(Qt::NoFocus);
	$.pauseButton = new QPushButton(TR("&Pause"));
	$.pauseButton.setFocusPolicy(Qt::NoFocus);

	$.board.connect($.startButton, SIGNAL("clicked()"), SLOT("start()"));
	QAPP().connect($.quitButton , SIGNAL("clicked()"), SLOT("quit()"));
	$.board.connect($.pauseButton, SIGNAL("clicked()"), SLOT("pause()"));
	$.scoreLcd.connect($.board, SIGNAL("scoreChanged(int)"), SLOT("display(int)"));
	$.levelLcd.connect($.board, SIGNAL("levelChanged(int)"), SLOT("display(int)"));
	$.linesLcd.connect($.board, SIGNAL("linesRemovedChanged(int)"), SLOT("display(int)"));

	my $layout = new QGridLayout();
	$layout.addWidget($.createLabel(TR("NEXT")), 0, 0);
	$layout.addWidget($.nextPieceLabel, 1, 0);
	$layout.addWidget($.createLabel(TR("LEVEL")), 2, 0);
	$layout.addWidget($.levelLcd, 3, 0);
	$layout.addWidget($.startButton, 4, 0);
	$layout.addWidget($.board, 0, 1, 6, 1);
	$layout.addWidget($.createLabel(TR("SCORE")), 0, 2);
	$layout.addWidget($.scoreLcd, 1, 2);
	$layout.addWidget($.createLabel(TR("LINES REMOVED")), 2, 2);
	$layout.addWidget($.linesLcd, 3, 2);
	$layout.addWidget($.quitButton, 4, 2);
	$layout.addWidget($.pauseButton, 5, 2);
	$.setLayout($layout);

	$.setWindowTitle(TR("Tetrix"));
	$.resize(550, 370);      
    }

    private createLabel($text)
    {
	my $lbl = new QLabel($text);
	$lbl.setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
	return $lbl;
    }
}

class tetrix_example inherits QApplication
{
    constructor()
    {
	our $colorTable = ( new QColor(0x000000), new QColor(0xCC6666), 
			    new QColor(0x66CC66), new QColor(0x6666CC),
			    new QColor(0xCCCC66), new QColor(0xCC66CC), 
			    new QColor(0x66CCCC), new QColor(0xDAAA00) );

	my $window = new TetrixWindow();
	$window.show();
        qsrand(int(now() - get_midnight(now())));
	$.exec();
    }
}
