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


{
    printf("Before outer scope\n");
    hash h;
    {
        printf("Enter outer scope\n");
        TreeMap tm2();
        printf("Before inner scope\n");
        {
            printf("Enter inner scope\n");
            MyObject o("tmp");
            printf("Exit inner scope\n");
            tm2.put("key", o);
            h{"x"} = tm2.get("key");
        }
        printf("After inner scope\n");
        printf("Exit outer scope\n");
    }
    printf("After outer scope\n");
}
printf("END\n");
