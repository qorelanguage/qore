%requires CsvUtil
%exec-class Test


class Test
{
    private {
        const FNAME = "test.csv";
    }

    constructor() {
   
        my hash $opts = (
                "header-lines" : 1,
                "header-names" : True,
                "verify-columns" : True,
                "fields" : (
                           "mod_data" : ("type" : "date", "format" : "YYYY-MM-DDTHH:mm:SS", ),
                           "id" : "int",
                           ),

            );
        my hash $optsw = (
                "date-format" : "YYYY/MM/DD",
            );

        my CsvFileIterator $it1(FNAME, $opts);
        my CsvFileWriter $fw1(FNAME+".1", $optsw);
        my CsvStringWriter $sw1($optsw);
        while ($it1.next()) {
            $fw1.writeLine(hash_values($it1.getValue()));
            $sw1.writeLine(hash_values($it1.getValue()));
        }

        my File $f1();
        $f1.open2(FNAME+".1");
        if ($f1.read(-1) != $sw1.getContent()) 
            throw "TEST-ERROR", "case1: CsvFileWriter and CsvStringWriter result diff";

        $optsw.headers = $it1.getHeaders();

        my CsvFileIterator $it2(FNAME, $opts);
        my CsvFileIterator $it21(FNAME, $opts);
        my CsvFileWriter $fw2(FNAME+".2", $optsw);
        my CsvStringWriter $sw2($optsw);

        $fw2.write($it2);
        $sw2.write($it21);

        my File $f2();
        $f2.open2(FNAME+".2");

        if ($f2.read(-1) != $sw2.getContent())
            throw "TEST-ERROR", "case2: CsvFileWriter and CsvStringWriter result diff";
    }

}


