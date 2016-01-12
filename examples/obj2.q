#!/usr/bin/env qore

#
# simple test programm to demonstrate the
# creation of objects in qore
#

# by Wolfgang Ritzinger <aargon@rat.at>

# here we make an out-of-line namespace declaration for namespace "dubist"
namespace dubist;

# here we make an out-of-line class declaration for class "pr" in namespace "dubist"
class dubist::pr;

$name="dubist";

# out-of-line definition: method intit() in class "pr" in namespace "dubist"
dubist::pr::intit() {
    print("name::pr::intit printn\n");
}

# out-of-line namespace declaration for namespace "eris"
namespace eris;
# out-of-line class declaration for class "pr" in namespace "eris"
class eris::pr;

# out-of-line method definition, constructor for class "pr" in namespace "eris"
eris::pr::constructor($name) {
    # $.name will be created as a member of class "pr"
    if($name == "")
        $.name="eris(default)";
    else
        $.name=$name;
    printf("%s constructor\n", $.name);
}

eris::pr::intit() {
    printf("%s::pr::intit printn\n", $.name);
}

printf("starting %s for '%s'\n", $ENV."_", $ENV."USER");

# instantiate class "pr" in namespace "dubist"
$pr=new dubist::pr();
# call the "intit" method
$pr.intit();

# instantiate class "pr" in namespace "eris"
$pr=new eris::pr();
# call the "intit" method
$pr.intit();

# instantiate class "pr" in namespace "eris" and pass an argument to the constructor
$pr=new eris::pr("neiganaum");
# call the "intit" method
$pr.intit();


