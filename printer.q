#!/usr/bin/env qore
# -*- mode: qore; indent-tabs-mode: nil -*-

/********************************************************************

    Printer.q
    =============================================================
    A script printing debug info about AST tree of a target file.
    ---
    Usage: printer.q TARGET_FILE [OUTPUT_FILE]

 *******************************************************************/

%new-style
%enable-all-warnings
%require-types
%strict-args

%requires astparser

sub printUsage() {
    printf("usage: %s TARGET_FILE [OUTPUT_FILE]\n", get_script_name());
}

if (ARGV.size() < 1) {
    stderr.printf("not enough arguments!\n");
    printUsage();
    exit(1);
}

if (ARGV.size() > 2) {
    stderr.printf("too many arguments!\n");
    printUsage();
    exit(1);
}

string fileToParse = ARGV[0];
string outFile = (ARGV.size() == 2) ? ARGV[1] : "./tree.out.txt";

astparser::AstParser parser();
parser.parseFile(fileToParse);
parser.printTree(outFile);
