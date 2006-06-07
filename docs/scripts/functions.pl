#!/usr/bin/perl -wT

# This is a quick hack to compare the documented qore functions 
# against the implemented functions.

# Parses some files
# Prints location of function appearance to STDOUT,
#    IF number of appearances < $FUNCTION_LIMIT
#    (i.e. 'somewhere' missing)

use strict;

use Getopt::Long;

# program and version information
my $PROGRAM = "functions.pl";
my $VERSION = "0.3";
my $PACKAGE = "qore documentation check";

## Scalars used by the option stuff
my $HELP_ARG        = 0;
my $VERSION_ARG     = 0;
my $VERBOSE         = 1;
my $DOC_FILE        = '../docbook/function_lib.docbook';
my $KATE_FILE       = '../../addon/kate/qore.xml';
my $SOURCE_DIR      = '../../lib/';
my $MONTH           = "";
my $TEST            = 0; # notifies to STDERR if 1 (default 0)

## other globals
my %SOURCE_FUNCTIONS      = ();
my %KATE_FUNCTIONS        = ();
my %DOC_TABLE_FUNCTIONS   = ();
my %DOC_SECTION_FUNCTIONS = ();
my %FUNCTION_COUNT        = ();
my $FUNCTION_LIMIT        = 4;

#======MAIN BEGIN============

# Handle options
GetOptions 
(
 "help|h"            => \$HELP_ARG,
 "version|v"         => \$VERSION_ARG,
 "verbose|x=s"       => \$VERBOSE,
 "doc-file|d=s"      => \$DOC_FILE,
 "kate-file|k=s"     => \$KATE_FILE,
 "source-dir|s=s"    => \$SOURCE_DIR,
 ) or &Console_WriteError_InvalidOption;

&Console_Write_Help if $HELP_ARG;
&Console_Write_Version if $VERSION_ARG;

read_source_files ( $SOURCE_DIR, \%SOURCE_FUNCTIONS, \%FUNCTION_COUNT );

read_doc_file ( $DOC_FILE, \%DOC_TABLE_FUNCTIONS, \%DOC_SECTION_FUNCTIONS, \%FUNCTION_COUNT );

read_kate_file ( $KATE_FILE, \%KATE_FUNCTIONS, \%FUNCTION_COUNT );


    foreach my $key ( sort keys %FUNCTION_COUNT ) {
       
        my $count = $FUNCTION_COUNT{$key};
        if ( $count < $FUNCTION_LIMIT ) {
            print "$key:\n";
            print "    $SOURCE_FUNCTIONS{$key}"      if ( exists $SOURCE_FUNCTIONS{$key} ) ;
            print "    $DOC_TABLE_FUNCTIONS{$key}"   if ( exists $DOC_TABLE_FUNCTIONS{$key} ) ;
            print "    $DOC_SECTION_FUNCTIONS{$key}" if ( exists $DOC_SECTION_FUNCTIONS{$key} ) ;
            print "    $KATE_FUNCTIONS{$key}"        if ( exists $KATE_FUNCTIONS{$key} ) ;
        }
    }

        
#=====MAIN END================

sub read_kate_file {
    my ( 
        $file,
        $hash_ref,
        $count_hash_ref
        ) = @_;
    local(*FIL);
    print STDERR "opening file \"$file\" for read\n"  if $VERBOSE >= 2;
    open(FIL, ($file) ) or 
        die "Cannot open $file for read: $!";
    
    my $list_name = '';
    my $line_no = 0;
    
LINE: while(my $line = <FIL>) {
        $line_no++;
        
        # <list name="functions">
        if ($line =~ m/<list\s*name\s*=\s*"(\w+)"\s*>/) {
            $list_name = $1;
            next LINE;
        }
        if ($line =~ m|</list\s*>|) {
            $list_name = '';
            next LINE;
        }
        
        if ( $list_name eq 'functions' ) {
            # <item>abs</item>
            if ($line =~ m|<item>\s*([^<]+)</item>| ) {
                my $function = $1;
                my $string = "$file" . "[$line_no]: $line";
                ${$hash_ref}{$function} = $string;
                print "$function:\n\t$string\n" if $VERBOSE >= 3;
                add_key ( $count_hash_ref, $function, 1 ); 
            }
        }
    }
    close (FIL);
}

sub read_doc_file {
    my ( 
        $file,
        $table_hash_ref,
        $section_hash_ref,
        $count_hash_ref
        ) = @_;
    local(*FIL);
    print STDERR "opening file \"$file\" for read\n"  if $VERBOSE >= 2;
    open(FIL, ($file) ) or 
        die "Cannot open $file for read: $!";
    
    my $line_no = 0;
    while(my $line = <FIL>) {
        $line_no++;
        
        # <entry><para>chr()</para></entry>
        if ($line =~ m/<entry>\s*<para>\s*(\w+)\(\)</) {
            my $function = $1;
            my $string = "$file" . "[$line_no]: $line";
            ${$table_hash_ref}{$function} = $string;
            print "$function:\n\t$string\n" if $VERBOSE >= 3;
            add_key ( $count_hash_ref, $function, 1 ); 
        }
        # <title>ceil()</title>
        if ($line =~ m/<title>\s*(\w+)\(\)</) {
            my $function = $1;
            my $string = "$file" . "[$line_no]: $line";
            ${$section_hash_ref}{$function} = $string;
            print "$function:\n\t$string\n" if $VERBOSE >= 3;
            add_key ( $count_hash_ref, $function, 1 ); 
        }
    }
    close (FIL);
}

sub read_source_file {
    my (
        $path, 
        $file,
        $hash_ref,
        $count_hash_ref
        ) = @_;
    local(*FIL);
    print STDERR "opening file \"$file\" for read\n"  if $VERBOSE >= 2;
    open(FIL, ($path . $file) ) or 
        die "Cannot open $file for read: $!";
    
    my $line_no = 0;
    my $msgstr = 0;
    while(my $line = <FIL>) {
        $line_no++;
        
        # builtinFunctions.add("dbg_node_info", f_dbg_node_info);
        if ($line =~ m/^\s*builtinFunctions.add\s*\(\s*"(\w+)"/) {
            my $function = $1;
            my $string = "$path" . "$file" . "[$line_no]: $line";
            ${$hash_ref}{$function} = $string;
            print "$function:\n\t$string\n" if $VERBOSE >= 3;
            add_key ( $count_hash_ref, $function, 1 ); 
        }
    }
    close (FIL);
}


sub read_source_files {
    my (
        $dir,
        $hash_ref,
        $count_hash_ref
        ) = @_;
        
    local(*DIR);
    
    print STDERR "opening dir \"$dir\" for read\n"  if $VERBOSE >= 2;
    opendir(DIR,"$dir") or die "Can't open directory \"$dir\"\n";
    
    print STDERR "selecting files\n"  if $VERBOSE >= 3;
    my @entries = readdir DIR ;
    
    closedir (DIR);
    
    #print "dir entries \t [ @files ] \n"  if $VERBOSE >= 3;
    
    for my $dir_entry ( sort @entries ) {
        
        if ( $dir_entry =~ m/^.*\.cc$/ ) {
            read_source_file ( $dir, $dir_entry, $hash_ref, $count_hash_ref );
        }
        
    }
}

sub add_key {
    my (
        $hash_ref,
        $key,
        $number
        ) = @_;
    if ( exists ${$hash_ref}{$key} ) {
        my $value = ${$hash_ref}{$key};
        $value = $value + $number;
        ${$hash_ref}{$key} = $value;
    }
    else { ${$hash_ref}{$key} = $number; }
}
 
sub Console_Write_Version {
    print <<_EOF_;
${PROGRAM} (${PACKAGE}) $VERSION

Copyright (C) 2005 by Helmut Wollmersdorfer
_EOF_
    exit;
}

sub Console_WriteError_InvalidOption
{
    ## Handle invalid arguments
    notify_it ('err', "Invalid options, Try '${PROGRAM} --help' for more information.");
    exit 1;
}

sub Console_Write_Help
{
    print <<_EOF_;
Usage: ${PROGRAM} [OPTIONS] 

Options:
     
  -x, --verbose=NUMBER        display lots of messages
                              default is 1, quiet is 0
  -h, --help                  display this help and exit
  -v, --version               output version information and exit

Examples of use:
${PROGRAM} > REPORTFILE       compare documented against implemented functions

_EOF_
    exit;
}
    
