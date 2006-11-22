#!/usr/bin/env qore

%enable-all-warnings
%disable-warning undeclared-var

# examples of qore statements

#########################
# the "foreach" statement 
# #######################
# "foreach" iterates through the elements in a list
# syntax: foreach <lvalue> in ( <expr.> )

$list = ( 1, "two", 3.1 );
foreach my $elem in ($list)
    printf("foreach 1: elem = %n\n", $elem);

# note that you can change the list by using a reference
foreach my $elem in (\$list)
    $elem = sprintf("%s-new", $elem);

printf("after foreach 2: new list is %n\n", $list);

#####################
# the "for" statement
#####################
# "for" executes code until a condition becomes false 
# syntax: for ( <initializer expr.> ; <conditional expr.> ; <iterator expr.> ) <statement or block>

for ($i = 0; $i < elements $list; $i++)
    printf("for: $list[%d] = %s\n", $i, $list[$i]);

#######################
# the "while" statement
#######################
# "while" executes code until a condition becomes false
# syntax: while ( <conditional expr.> ) <statement or block>

while ($i)
    printf("while: i = %n\n", $i--);

####################
# the "if" statement
####################
# "if" statements execute code conditionally
# syntax: if ( <conditional expr.> ) <statement or block> [else <statement or block>]

if ($i == 0)
    printf("if: $i = %n\n", $i);
else
    printf("ERROR\n");

##########################
# the "do while" statement
##########################
# "do while" executes code at least once based on a conditional expression
# syntax: do  <statement or block> while ( <conditional expr.> );

do
    printf("do while: i = %n\n", $i);
while ($i);

#############################
# the "switch case" statement
#############################
# "switch case" executes code based on the value of the switch expression
# note that case values must match exactly, no type conversion is done
# syntax: switch (<expr.>) { case <const. expr.>: <statement(s)>... }

$i = (1, 2, "three");
switch ($i)
{
    case 0:
    case "string":
    case 3.5:
        printf("ERROR: i=%n\n", $i);
        break;

    case (1, 2, "three"):
        printf("switch: $i = %n\n", $i);
        break;

    case ("key" : 143, "key2" : "value"):
    case NOTHING:
        printf("ERROR: i=%n\n", $i);
        break;

    default:
        printf("ERROR: $i = %n\n", $i);
        break;
}

# switch also provides ability to match by simple relation operators
# <, >, <= and >=. The case statements are evaluated in the same order 
# as they were written and the first match is executed.
$i = 1;

switch ($i) {
    case > 10: 
        printf("ERROR\n");
        break;
    case < -1.0:
        printf("ERROR\n");
        break;
    case >= 2:
        printf("ERROR\n");
        break;
    case <= 1:
        printf("switch $i <= 1\n", $i);
        break;
    case 0:
    default:
        printf("ERROR\n");
        break;
}

#########################
# the "context" statement
#########################
# "context" iterates through lists of a hash
# (such as those returned from the Datasource::select() method)

$q = ( "name" : ( "Arnie", "Sylvia", "Carol" ),
       "dob"  : ( 1981-04-23, 1972-11-22, 1995-03-11 ) );

context ($q)
    printf("context: %s's birthday is on %s\n", %name, format_date("Month DD, YYYY", %dob));

#######################
# the "break" statement
#######################
# "break" exits a loop (such as do, do while, for, foreach, context)
# and also exits execution of a switch statement

while (True)
{
    printf("about to execute break\n");
    break;
}

##########################
# the "continue" statement
##########################
# "continue" jumps to the next iteration of a loop

$i = 0;
while ($i < 2)
{
    ++$i;
    if ($i == 1)
	continue;
    printf("after continue: i = %d\n", $i);
}

########################
# the "return" statement
########################
# "return" returns from a function/subroutine

sub test()
{
    return "string";
}

printf("return: test() = %n\n", test());

########################
# the "throw" statement
# "try/catch" statements
########################
# "throw" throws an exception
# "try/catch" statements allow exceptions to be caught and handled

try {
    throw "TEST", "this is a string";
}
catch ($ex)
{
    printf("throw, try/catch: %s:%d: %n, %n\n", $ex.file, $ex.line, $ex.err, $ex.desc);
}

########################
# the "delete" statement
########################
# "delete" deletes values or objects

$i = 1;
delete $i;
printf("delete: $i = %n\n", $i);

#############################
# the "thread_exit" statement
#############################
# "thread_exit" allows a thread to exit immediately without terminating the entire process

sub thread_exit_test()
{
    printf("before thread_exit: TID %d\n", gettid());
    thread_exit;
    printf("ERROR: after thread_exit: TID %d\n", gettid());
}

background thread_exit_test();
