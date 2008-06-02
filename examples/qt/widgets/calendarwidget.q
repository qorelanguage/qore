#!/usr/bin/env qore

# This is basically a direct port of the QT widget example
# "calendarwidget" to Qore using Qore's "qt" module.  

# Note that Qore's "qt" module requires QT 4.3 or above 

# use the "qt-gui" module
%requires qt-gui

# this is an object-oriented program, the application class is "calendarwidget_example"
%exec-class calendarwidget_example
# require all variables to be explicitly  declared
%require-our
# enable all parse warnings
%enable-all-warnings


class Window inherits QWidget
{
    constructor()
    {
	$.createPreviewGroupBox();
	$.createGeneralOptionsGroupBox();
	$.createDatesGroupBox();
	$.createTextFormatsGroupBox();
	
	my $layout = new QGridLayout();
	$layout.addWidget($.previewGroupBox, 0, 0);
	$layout.addWidget($.generalOptionsGroupBox, 0, 1);
	$layout.addWidget($.datesGroupBox, 1, 0);
	$layout.addWidget($.textFormatsGroupBox, 1, 1);
	$layout.setSizeConstraint(QLayout::SetFixedSize);
	$.setLayout($layout);
	
	$.previewLayout.setRowMinimumHeight(0, $.calendar.sizeHint().height());
	$.previewLayout.setColumnMinimumWidth(0, $.calendar.sizeHint().width());
	
	$.setWindowTitle(TR("Calendar Widget"));	
    }

    localeChanged($index)
    {
	$.calendar.setLocale($.localeCombo.itemData($index));
    }

    firstDayChanged($index)
    {
	$.calendar.setFirstDayOfWeek($.firstDayCombo.itemData($index));
    }

    selectionModeChanged($index)
    {
	$.calendar.setSelectionMode($.selectionModeCombo.itemData($index));
    }

    horizontalHeaderChanged($index)
    {
	$.calendar.setHorizontalHeaderFormat($.horizontalHeaderCombo.itemData($index));
    }

    verticalHeaderChanged($index)
    {
	$.calendar.setVerticalHeaderFormat($.verticalHeaderCombo.itemData($index));
    }

    selectedDateChanged()
    {
	$.currentDateEdit.setDate($.calendar.selectedDate());
    }

    minimumDateChanged($date)
    {
	$.calendar.setMinimumDate($date);
	$.maximumDateEdit.setDate($.calendar.maximumDate());
    }

    maximumDateChanged($date)
    {
	$.calendar.setMaximumDate($date);
	$.minimumDateEdit.setDate($.calendar.minimumDate());
    }

    weekdayFormatChanged()
    {
	my $format = new QTextCharFormat();
	
	$format.setForeground(new QBrush($.weekdayColorCombo.itemData($.weekdayColorCombo.currentIndex())));
	$.calendar.setWeekdayTextFormat(Qt::Monday, $format);
	$.calendar.setWeekdayTextFormat(Qt::Tuesday, $format);
	$.calendar.setWeekdayTextFormat(Qt::Wednesday, $format);
	$.calendar.setWeekdayTextFormat(Qt::Thursday, $format);
	$.calendar.setWeekdayTextFormat(Qt::Friday, $format);
    }

    weekendFormatChanged()
    {
	my $format = new QTextCharFormat();
	
	$format.setForeground(new QBrush($.weekendColorCombo.itemData($.weekendColorCombo.currentIndex())));
	$.calendar.setWeekdayTextFormat(Qt::Saturday, $format);
	$.calendar.setWeekdayTextFormat(Qt::Sunday, $format);
    }

    reformatHeaders()
    {
	my $text = $.headerTextFormatCombo.currentText();
	my $format = new QTextCharFormat();
	
	if ($text == TR("Bold")) {
	    $format.setFontWeight(QFont::Bold);
	} 
	else if ($text == TR("Italic")) {
	    $format.setFontItalic(True);
	} else if ($text == TR("Green")) {
	    $format.setForeground(Qt::green);
	}
	$.calendar.setHeaderTextFormat($format);
    }

    reformatCalendarPage()
    {
	my $mayFirstFormat = new QTextCharFormat();
	if ($.mayFirstCheckBox.isChecked())
	    $mayFirstFormat.setForeground(Qt::red);
	
	my $firstFridayFormat = new QTextCharFormat();
	if ($.firstFridayCheckBox.isChecked())
	    $firstFridayFormat.setForeground(Qt::blue);
	
	my $date = new QDate($.calendar.selectedDate());

	$.calendar.setDateTextFormat(new QDate($date.year(), 5, 1), $mayFirstFormat);
	
	$date.setDate($date.year(), $date.month(), 1);
	while ($date.dayOfWeek() != Qt::Friday)
	    $date = $date.addDays(1);
	$.calendar.setDateTextFormat($date, $firstFridayFormat);
    }

    createPreviewGroupBox()
    {
	$.previewGroupBox = new QGroupBox(TR("Preview"));
	
	$.calendar = new QCalendarWidget();
	$.calendar.setMinimumDate(new QDate(1900, 1, 1));
	$.calendar.setMaximumDate(new QDate(3000, 1, 1));
	$.calendar.setGridVisible(True);
	
	$.connect($.calendar, SIGNAL("currentPageChanged(int, int)"), SLOT("reformatCalendarPage()"));

	$.previewLayout = new QGridLayout();
	$.previewLayout.addWidget($.calendar, 0, 0, Qt::AlignCenter);
	$.previewGroupBox.setLayout($.previewLayout);
    }

    createGeneralOptionsGroupBox()
    {
	$.generalOptionsGroupBox = new QGroupBox(TR("General Options"));
	
	$.localeCombo = new QComboBox();
	my $curLocaleIndex = -1;
	my $index = 0;
	for (my $lang = QLocale::C; $lang <= QLocale::LastLanguage; ++$lang) {

	    my $countries = QLocale_countriesForLanguage($lang);
	    foreach my $country in ($countries) {
		my $label = QLocale_languageToString($lang);
		$label += "/";
		$label += QLocale_countryToString($country);
		my $locale = new QLocale($lang, $country);
		if ($.locale().language() == $lang && $.locale().country() == $country)
		    $curLocaleIndex = $index;
		$.localeCombo.addItem($label, $locale);
		++$index;
	    }
	}
	if ($curLocaleIndex != -1)
	    $.localeCombo.setCurrentIndex($curLocaleIndex);
	$.localeLabel = new QLabel(TR("&Locale"));
	$.localeLabel.setBuddy($.localeCombo);
	
	$.firstDayCombo = new QComboBox();
	$.firstDayCombo.addItem(TR("Sunday"), Qt::Sunday);
	$.firstDayCombo.addItem(TR("Monday"), Qt::Monday);
	$.firstDayCombo.addItem(TR("Tuesday"), Qt::Tuesday);
	$.firstDayCombo.addItem(TR("Wednesday"), Qt::Wednesday);
	$.firstDayCombo.addItem(TR("Thursday"), Qt::Thursday);
	$.firstDayCombo.addItem(TR("Friday"), Qt::Friday);
	$.firstDayCombo.addItem(TR("Saturday"), Qt::Saturday);
	
	$.firstDayLabel = new QLabel(TR("Wee&k starts on:"));
	$.firstDayLabel.setBuddy($.firstDayCombo);
	
	$.selectionModeCombo = new QComboBox();
	$.selectionModeCombo.addItem(TR("Single selection"), QCalendarWidget::SingleSelection);
	$.selectionModeCombo.addItem(TR("None"), QCalendarWidget::NoSelection);
	
	$.selectionModeLabel = new QLabel(TR("&Selection mode:"));
	$.selectionModeLabel.setBuddy($.selectionModeCombo);
	
	$.gridCheckBox = new QCheckBox(TR("&Grid"));
	$.gridCheckBox.setChecked($.calendar.isGridVisible());
	
	$.navigationCheckBox = new QCheckBox(TR("&Navigation bar"));
	$.navigationCheckBox.setChecked(True);
	
	$.horizontalHeaderCombo = new QComboBox();
	$.horizontalHeaderCombo.addItem(TR("Single letter day names"), QCalendarWidget::SingleLetterDayNames);
	$.horizontalHeaderCombo.addItem(TR("Short day names"), QCalendarWidget::ShortDayNames);
	$.horizontalHeaderCombo.addItem(TR("None"), QCalendarWidget::NoHorizontalHeader);
	$.horizontalHeaderCombo.setCurrentIndex(1);
	
	$.horizontalHeaderLabel = new QLabel(TR("&Horizontal header:"));
	$.horizontalHeaderLabel.setBuddy($.horizontalHeaderCombo);
	
	$.verticalHeaderCombo = new QComboBox();
	$.verticalHeaderCombo.addItem(TR("ISO week numbers"), QCalendarWidget::ISOWeekNumbers);
	$.verticalHeaderCombo.addItem(TR("None"), QCalendarWidget::NoVerticalHeader);
	
	$.verticalHeaderLabel = new QLabel(TR("&Vertical header:"));
	$.verticalHeaderLabel.setBuddy($.verticalHeaderCombo);
	
	$.connect($.localeCombo,                 SIGNAL("currentIndexChanged(int)"), SLOT("localeChanged(int)"));
	$.connect($.firstDayCombo,               SIGNAL("currentIndexChanged(int)"), SLOT("firstDayChanged(int)"));
	$.connect($.selectionModeCombo,          SIGNAL("currentIndexChanged(int)"), SLOT("selectionModeChanged(int)"));
	$.calendar.connect($.gridCheckBox,       SIGNAL("toggled(bool)"),            SLOT("setGridVisible(bool)"));
	$.calendar.connect($.navigationCheckBox, SIGNAL("toggled(bool)"),            SLOT("setNavigationBarVisible(bool)"));
	$.connect($.horizontalHeaderCombo,       SIGNAL("currentIndexChanged(int)"), SLOT("horizontalHeaderChanged(int)"));
	$.connect($.verticalHeaderCombo,         SIGNAL("currentIndexChanged(int)"), SLOT("verticalHeaderChanged(int)"));
	
	my $checkBoxLayout = new QHBoxLayout();
	$checkBoxLayout.addWidget($.gridCheckBox);
	$checkBoxLayout.addStretch();
	$checkBoxLayout.addWidget($.navigationCheckBox);
	
	my $outerLayout = new QGridLayout();
	$outerLayout.addWidget($.localeLabel, 0, 0);
	$outerLayout.addWidget($.localeCombo, 0, 1);
	$outerLayout.addWidget($.firstDayLabel, 1, 0);
	$outerLayout.addWidget($.firstDayCombo, 1, 1);
	$outerLayout.addWidget($.selectionModeLabel, 2, 0);
	$outerLayout.addWidget($.selectionModeCombo, 2, 1);
	$outerLayout.addLayout($checkBoxLayout, 3, 0, 1, 2);
	$outerLayout.addWidget($.horizontalHeaderLabel, 4, 0);
	$outerLayout.addWidget($.horizontalHeaderCombo, 4, 1);
	$outerLayout.addWidget($.verticalHeaderLabel, 5, 0);
	$outerLayout.addWidget($.verticalHeaderCombo, 5, 1);
	$.generalOptionsGroupBox.setLayout($outerLayout);
	
	$.firstDayChanged($.firstDayCombo.currentIndex());
	$.selectionModeChanged($.selectionModeCombo.currentIndex());
	$.horizontalHeaderChanged($.horizontalHeaderCombo.currentIndex());
	$.verticalHeaderChanged($.verticalHeaderCombo.currentIndex());
    }

    createDatesGroupBox()
    {
	$.datesGroupBox = new QGroupBox(TR("Dates"));
	
	$.minimumDateEdit = new QDateEdit();
	$.minimumDateEdit.setDateRange($.calendar.minimumDate(), $.calendar.maximumDate());
	$.minimumDateEdit.setDate($.calendar.minimumDate());
	
	$.minimumDateLabel = new QLabel(TR("&Minimum Date:"));
	$.minimumDateLabel.setBuddy($.minimumDateEdit);
	
	$.currentDateEdit = new QDateEdit();
	$.currentDateEdit.setDate($.calendar.selectedDate());
	$.currentDateEdit.setDateRange($.calendar.minimumDate(), $.calendar.maximumDate());
	
	$.currentDateLabel = new QLabel(TR("&Current Date:"));
	$.currentDateLabel.setBuddy($.currentDateEdit);
	
	$.maximumDateEdit = new QDateEdit();
	$.maximumDateEdit.setDateRange($.calendar.minimumDate(), $.calendar.maximumDate());
	$.maximumDateEdit.setDate($.calendar.maximumDate());
	
	$.maximumDateLabel = new QLabel(TR("Ma&ximum Date:"));
	$.maximumDateLabel.setBuddy($.maximumDateEdit);
	
	$.calendar.connect($.currentDateEdit, SIGNAL("dateChanged(const QDate &)"), SLOT("setSelectedDate(const QDate &)"));
	$.connect($.calendar, SIGNAL("selectionChanged()"), SLOT("selectedDateChanged()"));
	$.connect($.minimumDateEdit, SIGNAL("dateChanged(const QDate &)"), SLOT("minimumDateChanged(const QDate &)"));
	$.connect($.maximumDateEdit, SIGNAL("dateChanged(const QDate &)"), SLOT("maximumDateChanged(const QDate &)"));
	
	my $dateBoxLayout = new QGridLayout();
	$dateBoxLayout.addWidget($.currentDateLabel, 1, 0);
	$dateBoxLayout.addWidget($.currentDateEdit, 1, 1);
	$dateBoxLayout.addWidget($.minimumDateLabel, 0, 0);
	$dateBoxLayout.addWidget($.minimumDateEdit, 0, 1);
	$dateBoxLayout.addWidget($.maximumDateLabel, 2, 0);
	$dateBoxLayout.addWidget($.maximumDateEdit, 2, 1);
	$dateBoxLayout.setRowStretch(3, 1);
	
	$.datesGroupBox.setLayout($dateBoxLayout);
    }

    createTextFormatsGroupBox()
    {
	$.textFormatsGroupBox = new QGroupBox(TR("Text Formats"));
	
	$.weekdayColorCombo = $.createColorComboBox();
	$.weekdayColorCombo.setCurrentIndex($.weekdayColorCombo.findText(TR("Black")));
	
	$.weekdayColorLabel = new QLabel(TR("&Weekday color:"));
	$.weekdayColorLabel.setBuddy($.weekdayColorCombo);
	
	$.weekendColorCombo = $.createColorComboBox();
	$.weekendColorCombo.setCurrentIndex($.weekendColorCombo.findText(TR("Red")));

	$.weekendColorLabel = new QLabel(TR("Week&end color:"));
	$.weekendColorLabel.setBuddy($.weekendColorCombo);

	$.headerTextFormatCombo = new QComboBox();
	$.headerTextFormatCombo.addItem(TR("Bold"));
	$.headerTextFormatCombo.addItem(TR("Italic"));
	$.headerTextFormatCombo.addItem(TR("Plain"));

	$.headerTextFormatLabel = new QLabel(TR("&Header text:"));
	$.headerTextFormatLabel.setBuddy($.headerTextFormatCombo);

	$.firstFridayCheckBox = new QCheckBox(TR("&First Friday in blue"));

	$.mayFirstCheckBox = new QCheckBox(TR("May &1 in red"));

	$.connect($.weekdayColorCombo,     SIGNAL("currentIndexChanged(int)"),             SLOT("weekdayFormatChanged()"));
	$.connect($.weekendColorCombo,     SIGNAL("currentIndexChanged(int)"),             SLOT("weekendFormatChanged()"));
	$.connect($.headerTextFormatCombo, SIGNAL("currentIndexChanged(const QString &)"), SLOT("reformatHeaders()"));
	$.connect($.firstFridayCheckBox,   SIGNAL("toggled(bool)"),                        SLOT("reformatCalendarPage()"));
	$.connect($.mayFirstCheckBox,      SIGNAL("toggled(bool)"),                        SLOT("reformatCalendarPage()"));

	my $checkBoxLayout = new QHBoxLayout();
	$checkBoxLayout.addWidget($.firstFridayCheckBox);
	$checkBoxLayout.addStretch();
	$checkBoxLayout.addWidget($.mayFirstCheckBox);

	my $outerLayout = new QGridLayout();
	$outerLayout.addWidget($.weekdayColorLabel, 0, 0);
	$outerLayout.addWidget($.weekdayColorCombo, 0, 1);
	$outerLayout.addWidget($.weekendColorLabel, 1, 0);
	$outerLayout.addWidget($.weekendColorCombo, 1, 1);
	$outerLayout.addWidget($.headerTextFormatLabel, 2, 0);
	$outerLayout.addWidget($.headerTextFormatCombo, 2, 1);
	$outerLayout.addLayout($checkBoxLayout, 3, 0, 1, 2);
	$.textFormatsGroupBox.setLayout($outerLayout);

	$.weekdayFormatChanged();
	$.weekendFormatChanged();
	$.reformatHeaders();
	$.reformatCalendarPage();
    }

    createColorComboBox()
    {
	my $comboBox = new QComboBox();
	$comboBox.addItem(TR("Red"), Qt::red);
	$comboBox.addItem(TR("Blue"), Qt::blue);
	$comboBox.addItem(TR("Black"), Qt::black);
	$comboBox.addItem(TR("Magenta"), Qt::magenta);
	return $comboBox;
    }
}

class calendarwidget_example inherits QApplication
{
    constructor()
    {
	my $window = new Window();
	$window.show();
	$.exec();
    }
}
