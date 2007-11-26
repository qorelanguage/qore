#!/usr/bin/env qore

# $self is bascially a direct port of the QT widget example
# "tooltips" to Qore using Qore's "qt" module.  

# Note that Qore's "qt" module requires QT 4.3 or above 

# use the "qt" module
%requires qt

# $self is an object-oriented program; the application class is "tooltips_example"
%exec-class tooltips_example
# require all variables to be explicitly declared
%require-our
# enable all parse warnings
%enable-all-warnings

class SortingBox inherits QWidget
{
    private $.shapeItems, $.circlePath, $.squarePath, $.trianglePath, $.previousPosition,
            $.itemInMotion, $.newCircleButton, $.newSquareButton, $.newTriangleButton;

    constructor()
    {
	$.circlePath = new QPainterPath();
	$.squarePath = new QPainterPath();
	$.trianglePath = new QPainterPath();
	$.previousPosition = new QPoint();
	$.shapeItems = ();

	$.setAttribute(Qt::WA_StaticContents);
	$.setMouseTracking(True);
	$.setBackgroundRole(QPalette::Base);

	$.itemInMotion = 0;

	$.newCircleButton = $.createToolButton(TR("New Circle"), new QIcon("images/circle.png"), SLOT("createNewCircle()"));

	$.newSquareButton = $.createToolButton(TR("New Square"), new QIcon("images/square.png"), SLOT("createNewSquare()"));

	$.newTriangleButton = $.createToolButton(TR("New Triangle"), new QIcon("images/triangle.png"), SLOT("createNewTriangle()"));

	$.circlePath.addEllipse(new QRectF(0, 0, 100, 100));
	$.squarePath.addRect(new QRectF(0, 0, 100, 100));

	my $x = $.trianglePath.currentPosition().x();
	my $y = $.trianglePath.currentPosition().y
	    ();
	$.trianglePath.moveTo($x + 120 / 2, $y);
	$.trianglePath.lineTo(0, 100);
	$.trianglePath.lineTo(120, 100);
	$.trianglePath.lineTo($x + 120 / 2, $y);

	$.setWindowTitle(TR("Tool Tips"));
	$.resize(500, 300);

	$.createShapeItem($.circlePath, TR("Circle"), $.initialItemPosition($.circlePath), $.initialItemColor());
	$.createShapeItem($.squarePath, TR("Square"), $.initialItemPosition($.squarePath), $.initialItemColor());
	$.createShapeItem($.trianglePath, TR("Triangle"), $.initialItemPosition($.trianglePath), $.initialItemColor());
    }

    event($event)
    {
	if ($event.type() == QEvent::ToolTip) {
	    my $index = $.itemAt($event.pos());
	    if ($index != -1)
		QToolTip_showText($event.globalPos(), $.shapeItems[$index].toolTip());
	    else
		QToolTip_hideText();
	}
	return QWidget::$.event($event);
    }

    resizeEvent($event)
    {
	my $margin = $.style().pixelMetric(QStyle::PM_DefaultTopLevelMargin);
	my $x = $.width() - $margin;
	my $y = $.height() - $margin;

	$y = $.updateButtonGeometry($.newCircleButton, $x, $y);
	$y = $.updateButtonGeometry($.newSquareButton, $x, $y);
	$.updateButtonGeometry($.newTriangleButton, $x, $y);
    }

    paintEvent($event)
    {
	my $painter = new QPainter($self);
	$painter.setRenderHint(QPainter::Antialiasing);
	foreach my $shapeItem in ($.shapeItems) {
	    $painter.translate($shapeItem.position());
	    $painter.setBrush($shapeItem.color());
	    $painter.drawPath($shapeItem.path());
	    $painter.translate($shapeItem.position().unaryMinus());
	}
    }

    mousePressEvent($event)
    {
	if ($event.button() == Qt::LeftButton) {
	    my $index = $.itemAt($event.pos());
	    if ($index != -1) {
		$.itemInMotion = $.shapeItems[$index];
		$.previousPosition = $event.pos();
		my $save = $.shapeItems[$index];
		splice $.shapeItems, $index, 1;
		$.shapeItems += $save;
		$.update();
	    }
	}
    }

    mouseMoveEvent($event)
    {
	if (($event.buttons() & Qt::LeftButton) && $.itemInMotion)
	    $.moveItemTo($event.pos());
    }

    mouseReleaseEvent($event)
    {
	if ($event.button() == Qt::LeftButton && $.itemInMotion) {
	    $.moveItemTo($event.pos());
	    $.itemInMotion = 0;
	}
    }

    createNewCircle()
    {
	$.createShapeItem($.circlePath, sprintf(TR("Circle <%d>"), ++$circle_count),
			  $.randomItemPosition(), $.randomItemColor());
    }

    createNewSquare()
    {
	$.createShapeItem($.squarePath, sprintf(TR("Square <%d>"), ++$square_count),
			  $.randomItemPosition(), $.randomItemColor());
    }

    createNewTriangle()
    {
	$.createShapeItem($.trianglePath, sprintf(TR("Triangle <%d>"), ++$triangle_count),
			  $.randomItemPosition(), $.randomItemColor());
    }

    private itemAt($pos)
    {
	for (my $i = (elements $.shapeItems) - 1; $i >= 0; --$i) {
	    my $item = $.shapeItems[$i];
	    if ($item.path().contains(new QPointF($pos.x() - $item.position().x(), $pos.y
						  () - $item.position().y
						  ())))
		return $i;
	}
	return -1;
    }

    private moveItemTo($pos)
    {
	my $offset = new QPoint($pos.x() - $.previousPosition.x(), $pos.y
				() - $.previousPosition.y
				());
	$.itemInMotion.setPosition($.itemInMotion.position().x() + $offset.x(), $.itemInMotion.position().y
				   () + $offset.y
				   ());
	$.previousPosition = $pos;
	$.update();
    }
    
    private updateButtonGeometry($button, $x, $y)
    {
	my $size = $button.sizeHint();
	$button.setGeometry($x - $size.width(), $y - $size.height(), $size.width(), $size.height());

	return $y - $size.height()
	    - $.style().pixelMetric(QStyle::PM_DefaultLayoutSpacing);
    }

    private createShapeItem($path, $toolTip, $pos, $color)
    {
	my $shapeItem = new ShapeItem();
	$shapeItem.setPath($path);
	$shapeItem.setToolTip($toolTip);
	$shapeItem.setPosition($pos);
	$shapeItem.setColor($color);
	$.shapeItems += $shapeItem;
	$.update();
    }

    private createToolButton($toolTip, $icon, $member)
    {
	my $button = new QToolButton($self);
	$button.setToolTip($toolTip);
	$button.setIcon($icon);
	$button.setIconSize(new QSize(32, 32));
	$.connect($button, SIGNAL("clicked()"), $member);

	return $button;
    }

    private initialItemPosition($path)
    {
	my $x;
	my $y = ($.height() - $path.controlPointRect().height()) / 2;
	if (!elements $.shapeItems)
	    $x = ((3 * $.width()) / 2 - $path.controlPointRect().width()) / 2;
	else
	    $x = ($.width() / (elements $.shapeItems)
		  - $path.controlPointRect().width()) / 2;
	
	return new QPoint($x, $y);
    }
    
    private randomItemPosition()
    {
	return new QPoint(qrand() % ($.width() - 120), qrand() % ($.height() - 120));
    }
    
    private initialItemColor()
    {
	return QColor_fromHsv((((elements $.shapeItems) + 1) * 85) % 256, 255, 190);
    }
    
    private randomItemColor()
    {
	return QColor_fromHsv(qrand() % 256, 255, 190);
    }
}

class ShapeItem 
{
    private $.myPath, $.myPosition, $.myColor, $.myToolTip;

    constructor()
    {
	$.myPath = new QPainterPath();
	$.myPosition = new QPoint();
	$.myColor = new QColor();
	$.myToolTip = "";
    }

    path()
    {
	return $.myPath;
    }

    position()
    {
	return $.myPosition;
    }

    color()
    {
	return $.myColor;
    }

    toolTip()
    {
	return $.myToolTip;
    }

    setPath($path)
    {
	$.myPath = $path;
    }

    setToolTip($toolTip)
    {
	$.myToolTip = $toolTip;
    }

    setPosition($position)
    {
	$.myPosition = $position;
    }

    setColor($color)
    {
	$.myColor = $color;
    }
}

class tooltips_example inherits QApplication
{
    constructor()
    {
	our $circle_count = 1;
	our $square_count = 1;
	our $triangle_count = 1;

        qsrand(int(now() - get_midnight(now())));
	my $sortingBox = new SortingBox();
	$sortingBox.show();
	$.exec();
    }
}
