#!/usr/bin/env qore 

$f = new File("ISO-8859-1");
$f.open("iso-8859-1.txt", O_WRONLY | O_CREAT);
$str = "Öffentl. Körperschaft/-\n";

$f.write($str);
