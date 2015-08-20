class MyObject {
    public {
        string name;
    }

    constructor(string n) {
        name = n;
        printf("Creating %s\n", name);
    }
    
    destructor() {
        printf("Destroying %s\n", name);
    }
}

#TODO move to (unit) tests

TreeMap tm();

sub x(string key) {
    printf("%s -> %n\n", key, tm.get(key));
}

tm.put("def/g/hi", "5");
tm.put("abc", "1");
tm.put("def", "3");
tm.put("d", "2");
tm.put("def/ghi", "4");

x("x");                 # N
x("de");                # N
x("def");               # 3
x("defx");              # N
x("def/");              # 3
x("def/g");             # 3
x("def/x");             # 3
x("def/ghi");           # 4
x("def/ghij");          # 3
x("def/ghi/sd");        # 4
x("def/g/hisd");        # 3
x("def/g/h");           # 3
x("def/g/hi");          # 5
x("def/g/hi/");         # 5
x("def/g/hi/sd");       # 5
x("abc");               # 1
x("d?whatever");        # 2

{
    printf("Before outer scope\n");
    hash h;
    {
        printf("Enter outer scope\n");
        TreeMap tm2();
        printf("Before inner scope\n");
        {
            printf("Enter inner scope\n");
            {
                MyObject o1("tmp1");
                tm2.put("key", o1);
            }
            MyObject o2("tmp2");
            MyObject o3("tmp3");
            tm2.put("key", o2); # must destroy tmp1 here
            tm2.put("key", o3);
            h{"x"} = tm2.get("key");
            printf("Exit inner scope\n");
        }   # must destroy tmp2 here
        printf("After inner scope\n");
        printf("Exit outer scope\n");
    }
    printf("After outer scope\n");
}   # must destroy tmp3 here
printf("END\n");
