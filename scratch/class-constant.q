#!/usr/bin/env qore

class T {
    private {
	#string $.str;
	const PRC = "private";
    }

    public {
	#string $.pubstr;
	const PUC = "public";
    }

    const Other = "other";

    constructor () {
	printf("PUC=%n\n", T::PUC);
	printf("PUC=%n\n", PUC);
	printf("PRC=%n\n", T::PRC);
	printf("PRC=%n\n", PRC);
	printf("Other=%n\n", T::Other);
	printf("Other=%n\n", Other);
    }
}

printf("PUC=%n\n", T::PUC);
printf("PUC=%n\n", PUC);
printf("Other=%n\n", T::Other);
printf("Other=%n\n", Other);
#printf("PRC=%n\n", T::PRC);
#printf("PRC=%n\n", PRC);

my T $t();
