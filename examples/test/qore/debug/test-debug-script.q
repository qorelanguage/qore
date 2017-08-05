#!/usr/bin/env qore

# program being debugged
%new-style
#%allow-debugging

our string globalString = 'global string';
our any globalAny;

my hash localHash;

any sub func(string s, int i) {
    string localString = sprintf("%s-%d", s, i);
    int j = 0;
    while (j < 5) {
        hash lv = get_local_vars(j);
        printf("get_local_vars(%d)\n%N\n\n", j, lv);
        j++;
    }
    return localString;
}

nothing sub main() {
    list localList;
    int i = 0;
    int zero = 0;
    while (i < 3) {
        push localList, func("F", i);
        i++;
    }
    try {
        i = i / zero;
    } catch (hash ex) {
        print("exception\n");
    }
    printf("%y\n", localList);
}

main();


