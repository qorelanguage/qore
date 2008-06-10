#!/usr/bin/env qore

# this is basically a direct port of the QT widget example
# "findfiles" to Qore using Qore's "qt" module.  

# Note that Qore's "qt" module requires QT 4.3 or above 

# use the "qt-gui" module
%requires qt-gui

# this is an object-oriented program; the application class is "findfiles_example"
%exec-class findfiles_example
# require all variables to be explicitly declared
%require-our
# enable all parse warnings
%enable-all-warnings

class Window inherits QDialog
{
    private $.fileComboBox, $.textComboBox, $.directoryComboBox, $.fileLabel, 
            $.textLabel, $.directoryLabel, $.filesFoundLabel, 
            $.browseButton, $.findButton, $.filesTable;

    constructor($parent) : QDialog($parent)
    {
	$.browseButton = $.createButton(TR("&Browse..."), SLOT("browse()"));
	$.findButton = $.createButton(TR("&Find"), SLOT("find()"));

	$.fileComboBox = $.createComboBox(TR("*"));
	$.textComboBox = $.createComboBox("");
	$.directoryComboBox = $.createComboBox(QDir::currentPath());

	$.fileLabel = new QLabel(TR("Named:"));
	$.textLabel = new QLabel(TR("Containing text:"));
	$.directoryLabel = new QLabel(TR("In directory:"));
	$.filesFoundLabel = new QLabel();

	$.createFilesTable();

	my $buttonsLayout = new QHBoxLayout();
	$buttonsLayout.addStretch();
	$buttonsLayout.addWidget($.findButton);

	my $mainLayout = new QGridLayout();
	$mainLayout.addWidget($.fileLabel, 0, 0);
	$mainLayout.addWidget($.fileComboBox, 0, 1, 1, 2);
	$mainLayout.addWidget($.textLabel, 1, 0);
	$mainLayout.addWidget($.textComboBox, 1, 1, 1, 2);
	$mainLayout.addWidget($.directoryLabel, 2, 0);
	$mainLayout.addWidget($.directoryComboBox, 2, 1);
	$mainLayout.addWidget($.browseButton, 2, 2);
	$mainLayout.addWidget($.filesTable, 3, 0, 1, 3);
	$mainLayout.addWidget($.filesFoundLabel, 4, 0);
	$mainLayout.addLayout($buttonsLayout, 5, 0, 1, 3);
	$.setLayout($mainLayout);

	$.setWindowTitle(TR("Find Files"));
	$.resize(700, 300);
    }

    browse()
    {
	my $directory = QFileDialog::getExistingDirectory($self, TR("Find Files"), QDir::currentPath());
	if (strlen($directory)) {
	    $.directoryComboBox.addItem($directory);
	    $.directoryComboBox.setCurrentIndex($.directoryComboBox.currentIndex() + 1);
	}
    }

    find()
    {
	$.filesTable.setRowCount(0);

	my $fileName = $.fileComboBox.currentText();
	my $text = $.textComboBox.currentText();
	my $path = $.directoryComboBox.currentText();

	my $directory = new QDir($path);
	my $files;
	if (!strlen($fileName))
	    $fileName = "*";
	$files = $directory.entryList(list($fileName), QDir::Files | QDir::NoSymLinks);

	if (strlen($text))
	    $files = $.findFiles($directory, $files, $text);
	$.showFiles($directory, $files);
    }

    findFiles($directory, $files, $text)
    {
	my $progressDialog = new QProgressDialog($self);
	$progressDialog.setCancelButtonText(TR("&Cancel"));
	$progressDialog.setRange(0, elements $files - 1);
	$progressDialog.setWindowTitle(TR("Find Files"));
	$progressDialog.show();

	my $foundFiles = ();

	for (my $i = 0; $i < elements $files; ++$i) {
	    $progressDialog.setValue($i);
	    $progressDialog.setLabelText(sprintf(TR("Searching file number %d of %d..."), $i, elements $files));
	    QCoreApplication::processEvents();

	    if ($progressDialog.wasCanceled())
		break;

	    my $file = new File();

	    if (!$file.open($directory.absoluteFilePath($files[$i]))) {
		while (exists (my $line = $file.readLine())) {
		    if ($progressDialog.wasCanceled())
			break;
		    if (regex($line, $text)) {
			$foundFiles += $files[$i];
			break;
		    }
		}
	    }
	}
	return $foundFiles;
    }

    showFiles($directory, $files)
    {
	foreach my $file in ($files) {
	    my $size = hstat($directory.absoluteFilePath($file)).size;

	    my $fileNameItem = new QTableWidgetItem($file);
	    $fileNameItem.setFlags(Qt::ItemIsEnabled);
	    my $sizeItem = new QTableWidgetItem(sprintf(TR("%d KB"), ($size + 1023) / 1024));
	    $sizeItem.setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	    $sizeItem.setFlags(Qt::ItemIsEnabled);

	    my $row = $.filesTable.rowCount();
	    $.filesTable.insertRow($row);
	    $.filesTable.setItem($row, 0, $fileNameItem);
	    $.filesTable.setItem($row, 1, $sizeItem);
	}
	$.filesFoundLabel.setText(sprintf(TR("%d file%s found"), elements $files, elements $files == 1 ? "" : "s"));
    }

    createButton($text, $member)
    {
	my $button = new QPushButton($text);
	$.connect($button, SIGNAL("clicked()"), $member);
	return $button;
    }

    createComboBox($text)
    {
	my $comboBox = new QComboBox();
	$comboBox.setEditable(True);
	$comboBox.addItem($text);
	$comboBox.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	return $comboBox;
    }

    createFilesTable()
    {
	$.filesTable = new QTableWidget(0, 2);
	my $labels = (TR("File Name"), TR("Size"));
	$.filesTable.setHorizontalHeaderLabels($labels);
	$.filesTable.horizontalHeader().setResizeMode(0, QHeaderView::Stretch);
	$.filesTable.verticalHeader().hide();
	$.filesTable.setShowGrid(False);
    }
}

class findfiles_example inherits QApplication
{
    constructor()
    {      
        my $window = new Window();
        $window.show();
        
        $.exec();
    }
}
