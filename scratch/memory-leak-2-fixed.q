#!/usr/bin/env qore

class T { destructor() { printf("Memory leak fixed\n"); } }
# object references Program, Program contains constant that contains object -> circular reference (now fixed)
const t = ("A": new T());

