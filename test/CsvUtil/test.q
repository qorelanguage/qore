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
        while ($it1.next()) {
            $fw1.writeLine(hash_values($it1.getValue()));
        }


        $optsw.headers = $it1.getHeaders();

        my CsvFileIterator $it2(FNAME, $opts);
        my CsvFileWriter $fw2(FNAME+".2", $optsw);
        $fw2.write($it2);


    }

}


