#!/usr/bin/env qore

class T { destructor() { printf("hi\n"); throw True; } }
# object references Program, Program contains constant that contains object -> circular reference
const t = ("A": new T());

