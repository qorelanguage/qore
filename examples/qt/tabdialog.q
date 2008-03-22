#!/usr/bin/env qore

# this is basically a direct port of the QT widget example
# "tabdialog" to Qore using Qore's "qt" module.  

# Note that Qore's "qt" module requires QT 4.3 or above 

# use the "qt" module
%requires qt

# this is an object-oriented program; the application class is "tabdialog_example"
%exec-class tabdialog_example
# require all variables to be explicitly declared
%require-our
# enable all parse warnings
%enable-all-warnings

sub do_date($d)
{
    return format_date("YYYY-MM-DD HH:mm:SS.ms", $d);
}

class GeneralTab inherits QWidget
{
    constructor($fileInfo, $parent) : QWidget($parent)
    {
	my $fileNameLabel = new QLabel(TR("File Name:"));
	my $fileNameEdit = new QLineEdit($fileInfo.fileName());

	my $pathLabel = new QLabel(TR("Path:"));
	my $pathValueLabel = new QLabel($fileInfo.absoluteFilePath());
	$pathValueLabel.setFrameStyle(QFrame::Panel | QFrame::Sunken);

	my $sizeLabel = new QLabel(TR("Size:"));
	my $size = $fileInfo.size()/1024;
	my $sizeValueLabel = new QLabel(sprintf(TR("%d K"), $size));
	$sizeValueLabel.setFrameStyle(QFrame::Panel | QFrame::Sunken);

	my $lastReadLabel = new QLabel(TR("Last Read:"));
	my $lastReadValueLabel = new QLabel(do_date($fileInfo.lastRead()));
	$lastReadValueLabel.setFrameStyle(QFrame::Panel | QFrame::Sunken);

	my $lastModLabel = new QLabel(TR("Last Modified:"));
	my $lastModValueLabel = new QLabel(do_date($fileInfo.lastModified()));
	$lastModValueLabel.setFrameStyle(QFrame::Panel | QFrame::Sunken);

	my $mainLayout = new QVBoxLayout();
	$mainLayout.addWidget($fileNameLabel);
	$mainLayout.addWidget($fileNameEdit);
	$mainLayout.addWidget($pathLabel);
	$mainLayout.addWidget($pathValueLabel);
	$mainLayout.addWidget($sizeLabel);
	$mainLayout.addWidget($sizeValueLabel);
	$mainLayout.addWidget($lastReadLabel);
	$mainLayout.addWidget($lastReadValueLabel);
	$mainLayout.addWidget($lastModLabel);
	$mainLayout.addWidget($lastModValueLabel);
	$mainLayout.addStretch(1);
	$.setLayout($mainLayout);
    }
}

class PermissionsTab inherits QWidget
{
    constructor($fileInfo, $parent) : QWidget($parent)
    {
	my $permissionsGroup = new QGroupBox(TR("Permissions"));

	my $readable = new QCheckBox(TR("Readable"));
	if ($fileInfo.isReadable())
	    $readable.setChecked(True);

	my $writable = new QCheckBox(TR("Writable"));
	if ($fileInfo.isWritable() )
	    $writable.setChecked(True);

	my $executable = new QCheckBox(TR("Executable"));
	if ($fileInfo.isExecutable() )
	    $executable.setChecked(True);

	my $ownerGroup = new QGroupBox(TR("Ownership"));

	my $ownerLabel = new QLabel(TR("Owner"));
	my $ownerValueLabel = new QLabel($fileInfo.owner());
	$ownerValueLabel.setFrameStyle(QFrame::Panel | QFrame::Sunken);

	my $groupLabel = new QLabel(TR("Group"));
	my $groupValueLabel = new QLabel($fileInfo.group());
	$groupValueLabel.setFrameStyle(QFrame::Panel | QFrame::Sunken);

	my $permissionsLayout = new QVBoxLayout();
	$permissionsLayout.addWidget($readable);
	$permissionsLayout.addWidget($writable);
	$permissionsLayout.addWidget($executable);
	$permissionsGroup.setLayout($permissionsLayout);

	my $ownerLayout = new QVBoxLayout();
	$ownerLayout.addWidget($ownerLabel);
	$ownerLayout.addWidget($ownerValueLabel);
	$ownerLayout.addWidget($groupLabel);
	$ownerLayout.addWidget($groupValueLabel);
	$ownerGroup.setLayout($ownerLayout);

	my $mainLayout = new QVBoxLayout();
	$mainLayout.addWidget($permissionsGroup);
	$mainLayout.addWidget($ownerGroup);
	$mainLayout.addStretch(1);
	$.setLayout($mainLayout);
    }
}

class ApplicationsTab inherits QWidget
{
    constructor($fileInfo, $parent) : QWidget($parent)
    {
	my $topLabel = new QLabel(TR("Open with:"));

	my $applicationsListBox = new QListWidget();
	my $applications = ();

	for (my $i = 1; $i <= 30; ++$i)
	    $applications += sprintf(TR("Application %d"), $i);
	$applicationsListBox.insertItems(0, $applications);

	my $alwaysCheckBox;

	if (!strlen($fileInfo.suffix()))
	    $alwaysCheckBox = new QCheckBox(TR("Always use this application to "
					       "open this type of file"));
	else
	    $alwaysCheckBox = new QCheckBox(sprintf(TR("Always use this application to "
						      "open files with the extension '%s'"), $fileInfo.suffix()));

	my $layout = new QVBoxLayout();
	$layout.addWidget($topLabel);
	$layout.addWidget($applicationsListBox);
	$layout.addWidget($alwaysCheckBox);
	$.setLayout($layout);
    }
}

class TabDialog inherits QDialog
{
    private $.tabWidget, $.buttonBox;

    constructor($fileName, $parent) : QDialog($parent)
    {
	my $fileInfo = new QFileInfo($fileName);

	$.tabWidget = new QTabWidget();
	$.tabWidget.addTab(new GeneralTab($fileInfo), TR("General"));
	$.tabWidget.addTab(new PermissionsTab($fileInfo), TR("Permissions"));
	$.tabWidget.addTab(new ApplicationsTab($fileInfo), TR("Applications"));

	$.buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
					   | QDialogButtonBox::Cancel);

	$.connect($.buttonBox, SIGNAL("accepted()"), SLOT("accept()"));
	$.connect($.buttonBox, SIGNAL("rejected()"), SLOT("reject()"));
	
	my $mainLayout = new QVBoxLayout();
	$mainLayout.addWidget($.tabWidget);
	$mainLayout.addWidget($.buttonBox);
	$.setLayout($mainLayout);

	$.setWindowTitle(TR("Tab Dialog"));
    }
}


class tabdialog_example inherits QApplication
{
    constructor()
    {
	my $fileName;

	if (elements $ARGV >= 1)
	    $fileName = $ARGV[0];
	else
	    $fileName = ".";

	my $tabdialog = new TabDialog($fileName);
	return $tabdialog.exec();
    }
}
