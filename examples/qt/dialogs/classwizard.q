#!/usr/bin/env qore

# this is basically a direct port of the QT widget example
# "classwizard" to Qore using Qore's "qt" module.  

# Note that Qore's "qt" module requires QT 4.3 or above 

# use the "qt-gui" module
%requires qt-gui

# this is an object-oriented program; the application class is "classwizard_example"
%exec-class classwizard_example
# require all variables to be explicitly declared
%require-our
# enable all parse warnings
%enable-all-warnings

class ClassWizard inherits QWizard
{
    constructor($parent) : QWizard($parent)
    {
	$.addPage(new IntroPage());
	$.addPage(new ClassInfoPage());
	$.addPage(new CodeStylePage());
	$.addPage(new OutputFilesPage());
	$.addPage(new ConclusionPage());

	$.setPixmap(QWizard::BannerPixmap, new QPixmap($dir + "images/banner.png"));
	$.setPixmap(QWizard::BackgroundPixmap, new QPixmap($dir + "images/background.png"));

	$.setWindowTitle(TR("Class Wizard"));
    }

    accept()
    {
	my $className = $.field("className");
	my $baseClass = $.field("baseClass");
	my $macroName = $.field("macroName");
	my $baseInclude = $.field("baseInclude");

	my $outputDir = $.field("outputDir");
	my $header = $.field("header");
	my $implementation = $.field("implementation");

	printf("%N %N %N %N %N %N %N\n", $className, $baseClass, $macroName, $baseInclude, $outputDir, $header, $implementation);

	my $block;

	if ($.field("comment")) {
	    $block += "/*\n";
	    $block += "    " + $header + "\n";
	    $block += "*/\n";
	    $block += "\n";
	}
	if ($.field("protect")) {
	    $block += "#ifndef " + $macroName + "\n";
	    $block += "#define " + $macroName + "\n";
	    $block += "\n";
	}
	if ($.field("includeBase")) {
	    $block += "#include " + $baseInclude + "\n";
	    $block += "\n";
	}

	$block += "class " + $className;
	if (strlen($baseClass))
	    $block += " : public " + $baseClass;
	$block += "\n";
	$block += "{\n";

	if ($.field("qobjectMacro")) {
		$block += "    Q_OBJECT\n";
		$block += "\n";
	}
	$block += "public:\n";

	if ($.field("qobjectCtor")) {
	    $block += "    " + $className + "(QObject *parent);\n";
	} else if ($.field("qwidgetCtor")) {
	    $block += "    " + $className + "(QWidget *parent);\n";
	} else if ($.field("defaultCtor")) {
	    $block += "    " + $className + "();\n";
	    if ($.field("copyCtor")) {
		$block += "    " + $className + "(const " + $className + " &other);\n";
		$block += "\n";
		$block += "    " + $className + " &operator=" + "(const " + $className + " &other);\n";
	    }
	}
	$block += "};\n";

	if ($.field("protect")) {
	    $block += "\n";
	    $block += "#endif\n";
	}

	my $filename = $outputDir + "/" + $header;
	my $headerFile = new File();
	if ($headerFile.open($filename, O_CREAT | O_TRUNC | O_WRONLY)) {
	    QMessageBox_warning(0, TR("Simple Wizard"),
				sprintf(TR("Cannot write file %s:\n%s"), $filename, strerror(errno())));
	    return;
	}
	$headerFile.write($block);

	delete $block;

	if ($.field("comment")) {
	    $block += "/*\n";
	    $block += "    " + $implementation + "\n";
	    $block += "*/\n";
	    $block += "\n";
	}
	$block += "#include \"" + $header + "\"\n";
	$block += "\n";

	if ($.field("qobjectCtor")) {
	    $block += $className + "::" + $className + "(QObject *parent)\n";
	    $block += "    : " + $baseClass + "(parent)\n";
	    $block += "{\n";
	    $block += "}\n";
	} else if ($.field("qwidgetCtor")) {
	    $block += $className + "::" + $className + "(QWidget *parent)\n";
	    $block += "    : " + $baseClass + "(parent)\n";
	    $block += "{\n";
	    $block += "}\n";
	} else if ($.field("defaultCtor")) {
	    $block += $className + "::" + $className + "()\n";
	    $block += "{\n";
	    $block += "    // missing code\n";
	    $block += "}\n";

	    if ($.field("copyCtor")) {
		$block += "\n";
		$block += $className + "::" + $className + "(const " + $className
		    + " &other)\n";
		$block += "{\n";
		$block += "    *$self = other;\n";
		$block += "}\n";
		$block += "\n";
		$block += $className + " &" + $className + "::operator=(const " + $className + " &other)\n";
		$block += "{\n";
		if (strlen($baseClass))
		    $block += "    " + $baseClass + "::operator=(other);\n";
		$block += "    // missing code\n";
		$block += "    return *$self;\n";
		$block += "}\n";
	    }
	}
	
	$filename = $outputDir + "/" + $implementation;
	my $implementationFile = new File();
	if ($implementationFile.open($filename, O_CREAT | O_WRONLY | O_TRUNC)) {
	    QMessageBox_warning(0, TR("Simple Wizard"), sprintf(TR("Cannot write file %s:\n%s"), $filename, strerror(errno())));
	    return;
	}
	$implementationFile.write($block);

	QDialog::$.accept();
    }

}

class IntroPage inherits QWizardPage
{
    private $.label;

    constructor($parent) : QWizardPage($parent)
    {
	$.setTitle(TR("Introduction"));
	$.setPixmap(QWizard::WatermarkPixmap, new QPixmap($dir + "images/watermark1.png"));

	$.label = new QLabel(TR("this wizard will generate a skeleton C++ class "
				"definition, including a few functions. You simply "
				"need to specify the class name and set a few "
				"options to produce a header file and an "
				"implementation file for your new C++ class."));
	$.label.setWordWrap(True);

	my $layout = new QVBoxLayout();
	$layout.addWidget($.label);
	$.setLayout($layout);
    }

}

class ClassInfoPage inherits QWizardPage
{
     private $.classNameLabel, $.baseClassLabel, $.classNameLineEdit, $.baseClassLineEdit, 
     $.qobjectMacroCheckBox, $.groupBox, $.qobjectCtorRadioButton, $.qwidgetCtorRadioButton,
     $.defaultCtorRadioButton, $.copyCtorCheckBox;

     constructor($parent) : QWizardPage($parent)
     {
	 $.setTitle(TR("Class Information"));
	 $.setSubTitle(TR("Specify basic information about the class for which you "
			  "want to generate skeleton source code files."));
	 $.setPixmap(QWizard::LogoPixmap, new QPixmap($dir + "images/logo1.png"));

	 $.classNameLabel = new QLabel(TR("&Class name:"));
	 $.classNameLineEdit = new QLineEdit();
	 $.classNameLabel.setBuddy($.classNameLineEdit);

	 $.baseClassLabel = new QLabel(TR("B&ase class:"));
	 $.baseClassLineEdit = new QLineEdit();
	 $.baseClassLabel.setBuddy($.baseClassLineEdit);

	 $.qobjectMacroCheckBox = new QCheckBox(TR("Generate Q_OBJECT &macro"));

	 $.groupBox = new QGroupBox(TR("C&onstructor"));

	 $.qobjectCtorRadioButton = new QRadioButton(TR("&QObject-style constructor"));
	 $.qwidgetCtorRadioButton = new QRadioButton(TR("Q&Widget-style constructor"));
	 $.defaultCtorRadioButton = new QRadioButton(TR("&Default constructor"));
	 $.copyCtorCheckBox = new QCheckBox(TR("&Generate copy constructor and "
					       "operator="));

	 $.defaultCtorRadioButton.setChecked(True);

	 $.copyCtorCheckBox.connect($.defaultCtorRadioButton, SIGNAL("toggled(bool)"), SLOT("setEnabled(bool)"));

	 $.registerField("className*", $.classNameLineEdit);
	 $.registerField("baseClass", $.baseClassLineEdit);
	 $.registerField("qobjectMacro", $.qobjectMacroCheckBox);
	 $.registerField("qobjectCtor", $.qobjectCtorRadioButton);
	 $.registerField("qwidgetCtor", $.qwidgetCtorRadioButton);
	 $.registerField("defaultCtor", $.defaultCtorRadioButton);
	 $.registerField("copyCtor", $.copyCtorCheckBox);

	 my $groupBoxLayout = new QVBoxLayout();
	 $groupBoxLayout.addWidget($.qobjectCtorRadioButton);
	 $groupBoxLayout.addWidget($.qwidgetCtorRadioButton);
	 $groupBoxLayout.addWidget($.defaultCtorRadioButton);
	 $groupBoxLayout.addWidget($.copyCtorCheckBox);
	 $.groupBox.setLayout($groupBoxLayout);

	 my $layout = new QGridLayout();
	 $layout.addWidget($.classNameLabel, 0, 0);
	 $layout.addWidget($.classNameLineEdit, 0, 1);
	 $layout.addWidget($.baseClassLabel, 1, 0);
	 $layout.addWidget($.baseClassLineEdit, 1, 1);
	 $layout.addWidget($.qobjectMacroCheckBox, 2, 0, 1, 2);
	 $layout.addWidget($.groupBox, 3, 0, 1, 2);
	 $.setLayout($layout);
     }
}

class CodeStylePage inherits QWizardPage
{
     private $.commentCheckBox, $.protectCheckBox, $.includeBaseCheckBox, $.macroNameLabel,
     $.baseIncludeLabel, $.macroNameLineEdit, $.baseIncludeLineEdit;
     
     constructor($parent) : QWizardPage($parent)
     {
	 $.setTitle(TR("Code Style Options"));
	 $.setSubTitle(TR("Choose the formatting of the generated code."));
	 $.setPixmap(QWizard::LogoPixmap, new QPixmap($dir + "images/logo2.png"));

	 $.commentCheckBox = new QCheckBox(TR("&Start generated files with a "
					    "comment"));
	 $.commentCheckBox.setChecked(True);

	 $.protectCheckBox = new QCheckBox(TR("&Protect header file against multiple "
					    "inclusions"));
	 $.protectCheckBox.setChecked(True);

	 $.macroNameLabel = new QLabel(TR("&Macro name:"));
	 $.macroNameLineEdit = new QLineEdit();
	 $.macroNameLabel.setBuddy($.macroNameLineEdit);

	 $.includeBaseCheckBox = new QCheckBox(TR("&Include base class definition"));
	 $.baseIncludeLabel = new QLabel(TR("Base class include:"));
	 $.baseIncludeLineEdit = new QLineEdit();
	 $.baseIncludeLabel.setBuddy($.baseIncludeLineEdit);

	 $.macroNameLabel.connect($.protectCheckBox, SIGNAL("toggled(bool)"), SLOT("setEnabled(bool)"));
	 $.macroNameLineEdit.connect($.protectCheckBox, SIGNAL("toggled(bool)"), SLOT("setEnabled(bool)"));
	 $.baseIncludeLabel.connect($.includeBaseCheckBox, SIGNAL("toggled(bool)"), SLOT("setEnabled(bool)"));
	 $.baseIncludeLineEdit.connect($.includeBaseCheckBox, SIGNAL("toggled(bool)"), SLOT("setEnabled(bool)"));

	 $.registerField("comment", $.commentCheckBox);
	 $.registerField("protect", $.protectCheckBox);
	 $.registerField("macroName", $.macroNameLineEdit);
	 $.registerField("includeBase", $.includeBaseCheckBox);
	 $.registerField("baseInclude", $.baseIncludeLineEdit);

	 my $layout = new QGridLayout();
	 $layout.setColumnMinimumWidth(0, 20);
	 $layout.addWidget($.commentCheckBox, 0, 0, 1, 3);
	 $layout.addWidget($.protectCheckBox, 1, 0, 1, 3);
	 $layout.addWidget($.macroNameLabel, 2, 1);
	 $layout.addWidget($.macroNameLineEdit, 2, 2);
	 $layout.addWidget($.includeBaseCheckBox, 3, 0, 1, 3);
	 $layout.addWidget($.baseIncludeLabel, 4, 1);
	 $layout.addWidget($.baseIncludeLineEdit, 4, 2);
	 $.setLayout($layout);
     }
     
     private initializePage()
     {
	 my $className = $.field("className");
	 $.macroNameLineEdit.setText(toupper($className) + "_H");

	 my $baseClass = $.field("baseClass");

	 $.includeBaseCheckBox.setChecked(strlen($baseClass));
	 $.includeBaseCheckBox.setEnabled(strlen($baseClass));
	 $.baseIncludeLabel.setEnabled(strlen($baseClass));
	 $.baseIncludeLineEdit.setEnabled(strlen($baseClass));

	 if (!strlen($baseClass)) {
	     $.baseIncludeLineEdit.clear();
	 } else if ((new QRegExp("Q[A-Z].*")).exactMatch($baseClass)) {
	     $.baseIncludeLineEdit.setText("<" + $baseClass + ">");
	 } else {
	     $.baseIncludeLineEdit.setText("\"" + tolower($baseClass) + ".h\"");
	 }	 
     }
}

class OutputFilesPage inherits QWizardPage
{
    private $.outputDirLabel, $.headerLabel, $.implementationLabel, $.outputDirLineEdit,
    $.headerLineEdit, $.implementationLineEdit;

    constructor($parent) : QWizardPage($parent)
    {
	$.setTitle(TR("Output Files"));
	$.setSubTitle(TR("Specify where you want the wizard to put the generated "
			 "skeleton code."));
	$.setPixmap(QWizard::LogoPixmap, new QPixmap($dir + "images/logo3.png"));

	$.outputDirLabel = new QLabel(TR("&Output directory:"));
	$.outputDirLineEdit = new QLineEdit();
	$.outputDirLabel.setBuddy($.outputDirLineEdit);

	$.headerLabel = new QLabel(TR("&Header file name:"));
	$.headerLineEdit = new QLineEdit();
	$.headerLabel.setBuddy($.headerLineEdit);

	$.implementationLabel = new QLabel(TR("&Implementation file name:"));
	$.implementationLineEdit = new QLineEdit();
	$.implementationLabel.setBuddy($.implementationLineEdit);

	$.registerField("outputDir*", $.outputDirLineEdit);
	$.registerField("header*", $.headerLineEdit);
	$.registerField("implementation*", $.implementationLineEdit);

	my $layout = new QGridLayout();
	$layout.addWidget($.outputDirLabel, 0, 0);
	$layout.addWidget($.outputDirLineEdit, 0, 1);
	$layout.addWidget($.headerLabel, 1, 0);
	$layout.addWidget($.headerLineEdit, 1, 1);
	$layout.addWidget($.implementationLabel, 2, 0);
	$layout.addWidget($.implementationLineEdit, 2, 1);
	$.setLayout($layout);
    }

    private initializePage()
    {
	my $className = $.field("className");
	$.headerLineEdit.setText(tolower($className) + ".h");
	$.implementationLineEdit.setText(tolower($className) + ".cpp");
	$.outputDirLineEdit.setText(QDir_tempPath());
    }
}

class ConclusionPage inherits QWizardPage
{
    private $.label;

    constructor($parent) : QWizardPage($parent)
    {
	$.setTitle(TR("Conclusion"));
	$.setPixmap(QWizard::WatermarkPixmap, new QPixmap($dir + "images/watermark2.png"));

	$.label = new QLabel();
	$.label.setWordWrap(True);

	my $layout = new QVBoxLayout();
	$layout.addWidget($.label);
	$.setLayout($layout);
    }

    private initializePage()
    {
	my $finishText = $.wizard().buttonText(QWizard::FinishButton);
	$finishText =~ s/&//g;
	$.label.setText(sprintf(TR("Click %s to generate the class skeleton."), $finishText));
    }
}

class classwizard_example inherits QApplication
{
    constructor()
    {      
	our $dir = get_script_dir();

	my $translatorFileName = "qt_";
	$translatorFileName += QLocale_system().name();
	my $translator = new QTranslator(QAPP());
	if ($translator.load($translatorFileName, QLibraryInfo_location(QLibraryInfo::TranslationsPath)))
	    QCoreApplication_installTranslator($translator);
	
	my $wizard = new ClassWizard();
	$wizard.show();
	
    	$.exec();
    }
}
