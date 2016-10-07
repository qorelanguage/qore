# #!/usr/bin/env qore

# To test syntax hightlighting in editor

# set the exec-class option and give the class name in case the file is renamed

/*
  Multiline comment
  next line

*/
%requires qore >= 0.8.12

%include /usr/qore/lib/functions.ql

# assume local var scope, do not use "$" for vars, members, and method calls
%new-style
%enable-all-warnings
%require-types
%strict-args
%unknown-directive with params
%wrong_directive with params

%enable-warning deprecated
%disable-warning excess-args

%define EXEC_CLASS 1
%ifdef EXEC_CLASS

module foo {
    version = "1.0";
    desc = "test module";
    author = "Foobar Industries, inc";
    url = "http://example.com";
    license = "MIT";
    
    init = sub () {
        Foo::initialized = True;
        Bar::OurBool = False;
    };
    del = sub () {
        print("goodbye, cruel world!\n");
        $aa
        aa
    }
}

%exec-class HelloWorld

# an object-oriented, threaded hello world :-)
class HelloWorld
{
    constructor() {
        background $.output("Hello, world!");
    }
    private output($arg) {
        printf("%s\n", $arg);
    }
}

%else

# do not use "$", assume local scope for variables
%new-style
# nothing in namespace ::Foo is exported
namespace Foo {
    # inline global variable declarations cannot be initialized when declared
    our bool initialized;
     
    class SuperClass {
    }
    class NotSoGreatClass {
    }
    class UnstableClass {
    }
}

Foo::initialized = False;
const PI_SQUARED = pow(Qore::M_PI, 2);


our int a = 1; # this is a global variable
our (string b, any c, hash d); # list of global variables
# datatypes
our bool b1 = False;
our softbool b2 = True;
our string s1 = "str1\"\n\\";
our string s2 = 'str1';
our int i1 = 4;
our softint i_2 = 452;
our float f1 = -500.494;
our float f2 = 2.35e10;
our number n1 = -500.494n;
our number n2 = 2.35e10n;
our date d1 = 2012-02-17T19:05:54+01:00;
our dates list = (
    2012-02-17T19:05:54.447-01:00,
    2012-02-17,
    2012-02-17T19:05:54.44745+01:00,
    2012-02-17-19:05:54.00-01:00,
    112s,
    55M,
    1D,
    02ms,
    P1Y,
    P1Y2MT4H50S,
    P1Y2MT4H487u,
    P2004-01-30T10:20:30,
    P2004-01-30T10:20:30.796,
    /ass/,
    True,
    False,
    ds
    
        
);
our softdate d2 = 2016-01-31;
our binary bin1 = <0feba023ffdca6291>;
our timeout t1 = 1250ms;
our null n = NULL;

our string mls = "
  l1
  l2
  l3
";

code closure = int sub (int a) { return a + b; };

list l = map $#, list, ($1 < 0);

our list l1 = (
    1,
    2,
    "three",
    4.0,
    5e20n,
    6,
    2001-01-15Z,
    9
);

our hash h1 = (
    "apple" : 1 + 1,
    "pear"  : "good",
);
our hash h2 = {
    "boat" : 1 + 1,
    "car"  : "good",
    {},
};

reference r1 = \h1;

# public members of namespace ::Bar are exported
public namespace Bar {
    # Bar::SomeClass is exported
    constructor() {}
    destructor() {}
    copy() {}
    
    public class SomeClass {
        my a = 2;
        d2 = int(2006-01-20);
        any a = l1[2];
        a = h1{"pe" + "ar"};
        a = h1.pear;
        a = h1."pear";
        a = h1.("pe" + "ar")
        return exists NOTHING;
    }

    # Bar::add(int, int) is exported
    public int sub add(int x, int y) { return x + y; }
    
    # Bar::OurBool is exported
    public our bool OurBool;

    # Bar::PrivateClass is not exported
    class PrivateClass {
    }
    
    final private int private() {}
    static synchronized *int static_softbool() {}
    
    abstract int abstractFunc();
    
}

bar = new Bar();
bar.add(452, 7552);

any ref1 = \bar;

string sub (int c) { return sprintf("temp=%d", c); }
any v = bar;

nothing sub bar() { };
synchronized sub foo(*softnumber n) {
        print(); abs()
}

deprecated sub foo(*code c) {
    `ls -l`
    hash{"na" + "me"}
    hash.name
    obj.method()
    list[1]
    ++a
    a++
    --a
    a--
    new Socket()
    background mainThread()
    delete var
    remove var
    cast<SubClass>(var)
    if (!(a > 10)) {}
    var = ~var
    v{.py}ar = +var
    var = -var
    shift list
    pop list
    chomp string
    trim string
    elements list
    keys hash
    var = a * 10
    var = a / 10
    var = a % 10
    a + 10
    "hello" + "there"
    list + "new value"
    hash + ( "newkey" : 100 )
    a - 10
    0xff00 >> 8
    0xff00 << 8
    exists var
    instanceof Qore::Mutex
    a < 10
    a > 10
    a == 10
    a != 10
    a <= 10
    a >= 10
    a <=> b
    a === 10
    a !== 10
    a =~ /text/
    a !~ /text/
    a =~ s/text/text/
    a =~ x/(\w+):(\w+)/
    a =~ tr/a-z/A-Z/
    a & 0xff
    a ^ 0xff
    a | 0xff
    (a = 1) && (b < 10)
    (a = 1) || (b < 10)
    a == 2 ? "yes" : 'no'
    any var = a ?? b
    any var = a ?* b
    1, 2, 3, 4, 5
    unshift list, val
    push list, val
    splice list, 2, 2, (1, 2, 3)
    my sublist = extract list, 2, 2, (1, 2, 3)
    map closure($1), list
    map {expr($1) : expr($1): blabl($#) }, list
    foldl closure($1 - $2), list
    foldr closure($1 - $2), list
    select list, $1 > 1
    var = 1
    var += 5
    var -= 5
    var &= 0x2000
    var |= 0x2000
    var %= 100
    var *= 10
    var /= 10
    var ^= 0x2000
    var <<= 0x2000
    var >>= 0x2000

    # regex
    # call process() if the string starts with an alphanumeric character
    if (str =~ /^[[:alnum:]]/)
        process(str);
    # example of using regular expressions in a switch statement
    switch (str) {
        case /^[^[:alnum:]]/: return True;
        case /^[0-9]/: return False;
        default: throw "ERROR", sprintf("invalid string %y", str);
    }
    # regular expression substitution + ignore case & global options
    str =~ s/abc/xyz/gi;
    # prefix all non-alphanumeric characters with a backslash
    str =~ s/([^[:alnum:]])/\\$1/g;
    # regular expression substring extraction
    *list l = (str =~ x/(?:(\w+):(\w+):)(\w+)/);
    
    # prefix all non-alphanumeric characters with a backslash
    str =~ s/([^[:alnum:]])/\\$1/g;
    # remove parentheses from string at the beginning of the line
    str =~ s/^\((.*)\)/$1/;

    # statement
    var = 1;
    var += 5;
    var[1].count++;
    shift var.key[i];
    new ObjectClass(1, 2, 3);
    background function();
    call_reference(arg1, arg2);
    object.method(1, 2, 3);
    if (var == 3) {}
    if (var == 3) {} else {}
    while (var < 10) {}
    do {} while (True);
    for (int i = 0; i < 10; ++ i) {
    }
    foreach softint i in (list) {
    }
    switch (var) { 
        case < -1:
            break;
        case > 2007-01-22T15:00:00:
            printf("greater than 2007-01-22 15:00:00\n");
            break;
        case /abc/:
            break;
        case =~ /error/:
            throw "ERROR", var; 
        default: 
            printf("%s\n", var); 
    }
            
    return val;
    string var;
    my (int a, string b, bool c);
    our int var;
    our (float a, int b, hash c);
    calculate(this, that, the_other);
    continue;
    break;
    {}
    throw "ERROR", description;
    try { func(); 
    } 
    catch (hash ex) { printf("%s:%d: %s: %s\n", ex.file, ex.line, ex.err, ex.desc); 
        rethrow;
    }
    rethrow;
    thread_exit;
    context top (q) {}    
    summarize (q) by (%date) where (%id != NULL) {}
    subcontext where (%type == "INBOUND" ) {}
    context (service_history) where (%service_type == "voice")
    sortBy (%effective_start_date) {
        printf("%s: start date: %s\n", %msisdn, format_date("YYYY-MM-DD HH:mm:SS", %effective_start_date));
    }
    summarize (services)
         by (%effective_start_date)
         where (%service_type == "voice")
         sortBy (%effective_start_date) {
         printf("account has %d service(s) starting on %s\n",
                context_rows(),
                format_date("YYYY-MM-DD HH:mm:SS", %effective_start_date));
         subcontext sortDescendingBy (%effective_end_date) {
             printf("\tservice %s: ends: %s\n", %msisdn, format_date("YYYY-MM-DD HH:mm:SS", %effective_end_date));
         }
    }
    on_exit l.unlock();
    on_success ds.commit();
    on_error ds.rollback();
    
    
}


%endif
