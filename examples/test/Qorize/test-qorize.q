#!/usr/bin/env qore

%requires Qorize


const OLDSTYLE = False;
const NEWSTYLE = True;


sub test(string $desc, any $in, string $name, bool $newstyle) {
    my string $code = qorize($in, $name, $newstyle);
    testParse($desc, $code, $newstyle);
}
sub testNamed(string $desc, hash $in, string $name, bool $newstyle) {
    my string $code = qorizeNamed($in, $name, $newstyle);
    testParse($desc, $code, $newstyle);
}

sub testParse(string $desc, string $code, bool $newstyle) {
    printf("\nTEST NAME (%s): %s\n", $newstyle ? "newstyle" : "oldstyle", $desc);
    printf("%s\n", $code);
    my Program $p( $newstyle ? Qore::PO_NEW_STYLE : NOTHING );
    my *hash $ret = $p.parse($code, "test");
    printf("    Parse OK> %N\n\n", $ret);
}



test("boolean", True, "b", OLDSTYLE);
test("boolean", True, "b", NEWSTYLE);

test("string", "lorem ipsum", "str", OLDSTYLE);
test("string", "lorem ipsum", "str", NEWSTYLE);

test("string (escaped", 'foo \n"bar"\n', "str", OLDSTYLE);
test("string (escaped", 'foo \n"bar"\n', "str", NEWSTYLE);

test("float", 10.34, "f", OLDSTYLE);
test("float", 10.34, "f", NEWSTYLE);

test("number", 5.23928173726123e50n, "n", OLDSTYLE);
test("number", 5.23928173726123e50n, "n", NEWSTYLE);

test("date", now(), "d", OLDSTYLE);
test("date", now(), "d", NEWSTYLE);

test("date relative", 3h, "d", OLDSTYLE);
test("date relative", 3h, "d", NEWSTYLE);

test("date relative", -1D, "d", OLDSTYLE);
test("date relative", -1D, "d", NEWSTYLE);


test("binary", binary("foo"), "b", OLDSTYLE);
test("binary", binary("foo"), "b", NEWSTYLE);

test("list", (True, False, now(), 12, 12.1, ( 'a', 'b'), ) , "l", OLDSTYLE);
test("list", (True, False, now(), 12, 12.1, ( 'a', 'b'), ) , "l", NEWSTYLE);

my hash $h1 = (
        "foo" : "bar",
        "key1" : now(),
        "key2" : 12,
        "key3" : ( 1, 2, 3),
        "key4" : ( "subkey1" : 1, "subkey2" : 2, ),
    );
test("hash", $h1, "hs", OLDSTYLE);
test("hash", $h1, "hs", NEWSTYLE);

testNamed("hash named", $h1, "name", OLDSTYLE);
testNamed("hash named", $h1, "name", NEWSTYLE);


%requires xml
my string $fname = get_script_dir() + '/complex.xml';
my File $f();
$f.open2($fname);
my string $xml = $f.read(-1);

my hash $h = parseXMLAsData($xml);
test("hash complex", $h, "name", OLDSTYLE);
test("hash complex", $h, "name", NEWSTYLE);

testNamed("hash complex named", $h, "name", OLDSTYLE);
testNamed("hash complex named", $h, "name", NEWSTYLE);

my string $code = qorizeNamed($h, "foo");
$code += "\n%requires xml\n\nreturn ('xml' : makeFormattedXMLString($foo));\n";
my Program $p();
$p.parse($code, "test1");
my string $checkXml = $p.run().xml;
my hash $check = parseXMLAsData($checkXml);
if ($check != $h)
    throw "TEST-ERROR", "original and final hash are different";
else
    printf("OK> original and final hash are the same\n");

