#!/usr/bin/env qore

# This is basically a direct port of the QT "borderlayout" example to Qore 
# using Qore's "qt" module.  

# Note that Qore's "qt" module requires QT 4.3 or above 

# use the "qt-gui" module
%requires qt-gui

# this is an object-oriented program, the application class is "borderlayout"
%exec-class borderlayout
# require all variables to be explicitly declared
%require-our
# enable all parse warnings
%enable-all-warnings

const West = 0;
const North = 1;
const South = 2;
const East = 3;
const Center = 4;

const MinimumSize = 0;
const SizeHint = 1;

class ItemWrapper
{
    constructor($i, $p) 
    {
	$.item = $i;
	$.position = $p;
    }
}

class BorderLayout inherits QLayout
{
    private $.list;

    constructor($parent, $margin, $spacing) : QLayout($parent instanceof QObject ? $parent : NOTHING)
    {
	$.list = ();
	if (!exists $margin) {
	    $.setSpacing($parent);
	    return;
	}
	$.setMargin($margin);
	$.setSpacing($spacing);
    }

    addItem($item)
    {
	$.add($item, West);
    }

    addWidget($widget, $position)
    {
	$.add(new QWidgetItem($widget), $position);
    }

    expandingDirections()
    {
	return Qt::Horizontal | Qt::Vertical;
    }

    hasHeightForWidth()
    {
	return False;
    }

    count()
    {
	return elements $.list;
    }

    itemAt($index)
    {
	my $wrapper = $.list[$index];
	if (exists $wrapper)
	    return $wrapper.item;
    }

    minimumSize()
    {
	return $.calculateSize(MinimumSize);
    }

    setGeometry($rect)
    {
	my $center;
	my $eastWidth;
	my $westWidth;
	my $northHeight;
	my $southHeight = 0;
	my $centerHeight = 0;
	my $i;

	QLayout::$.setGeometry($rect);

	for ($i = 0; $i < elements $.list; ++$i) {
	    my $wrapper = $.list[$i];
	    my $item = $wrapper.item;
	    my $position = $wrapper.position;

	    if ($position == North) {
		$item.setGeometry(new QRect($rect.x(), $northHeight, $rect.width(),
					    $item.sizeHint().height()));

		$northHeight += $item.geometry().height() + $.spacing();
	    } else if ($position == South) {
		$item.setGeometry(new QRect($item.geometry().x(),
				       $item.geometry().y 
					    (), $rect.width(),
					    $item.sizeHint().height()));

		$southHeight += $item.geometry().height() + $.spacing();

		$item.setGeometry(new QRect($rect.x(),
					    $rect.y
					    () + $rect.height() - $southHeight + $.spacing(),
					    $item.geometry().width(),
					    $item.geometry().height()));
	    } else if ($position == Center) {
		$center = $wrapper;
	    }
	}

	$centerHeight = $rect.height() - $northHeight - $southHeight;

	for ($i = 0; $i < elements $.list; ++$i) {
	    my $wrapper = $.list[$i];
	    my $item = $wrapper.item;
	    my $position = $wrapper.position;

	    if ($position == West) {
		$item.setGeometry(new QRect($rect.x() + $westWidth, $northHeight,
					    $item.sizeHint().width(), $centerHeight));
		
		$westWidth += $item.geometry().width() + $.spacing();
	    } else if ($position == East) {
		$item.setGeometry(new QRect($item.geometry().x(), $item.geometry().y
					    (),
					    $item.sizeHint().width(), $centerHeight));
		
		$eastWidth += $item.geometry().width() + $.spacing();

		$item.setGeometry(new QRect($rect.x() + $rect.width() - $eastWidth + $.spacing(),
					    $northHeight, $item.geometry().width(),
					    $item.geometry().height()));
	    }
	}

	if ($center)
	    $center.item.setGeometry(new QRect($westWidth, $northHeight,
					       $rect.width() - $eastWidth - $westWidth,
					       $centerHeight));
    }

    sizeHint()
    {
	return $.calculateSize(SizeHint);
    }

    takeAt($index)
    {
	if ($index >= 0 && $index < elements $.list) {
	    my $layoutStruct = $.list[$index];
	    splice $.list, $index, 1;
	    return $layoutStruct.item;
	}
    }

    add($item, $position)
    {
	$.list += new ItemWrapper($item, $position);
    }

    calculateSize($sizeType)
    {
	my ($height, $width);

	for (my $i = 0; $i < elements $.list; ++$i) {
	    my $wrapper = $.list[$i];
	    my $item = $wrapper.item;
	    my $position = $wrapper.position;

	    my $itemSize;

	    if ($sizeType == MinimumSize)
		$itemSize = $item.minimumSize();
	    else # (sizeType == SizeHint)
		$itemSize = $item.sizeHint();

	    if ($position == North || $position == South || $position == Center)
		$height += $itemSize.height();

	    if ($position == West || $position == East || $position == Center)
		$width += $itemSize.width();
	}
	return new QSize($width, $height);
    }
}

class Window inherits QWidget
{
    constructor()
    {
	my $centralWidget = new QTextBrowser();
	$centralWidget.setPlainText(TR("Central widget"));

	my $layout = new BorderLayout();
	$layout.addWidget($centralWidget, Center);
	$layout.addWidget($.createLabel("North"), North);
	$layout.addWidget($.createLabel("West"), West);
	$layout.addWidget($.createLabel("East 1"), East);
	$layout.addWidget($.createLabel("East 2") , East);
	$layout.addWidget($.createLabel("South"), South);
	$.setLayout($layout);

	$.setWindowTitle(TR("Border Layout"));
    }

    createLabel($text)
    {
	my $label = new QLabel($text);
	$label.setFrameStyle(QFrame::Box | QFrame::Raised);
	return $label;
    }
}

class borderlayout inherits QApplication 
{
    constructor() {
	my $window = new Window();	
        $window.show();
        return $.exec();
    }
}
