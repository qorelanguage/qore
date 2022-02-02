#! contains all public definitions in the DbDataProvider module
public namespace DbDataProvider {
#! The type definition for one index
public class DbIndexDataType inherits HashDataType {
    #! Creates the type
    constructor() {
        # add members
        addField(new QoreDataField("columns", "the list of columns making up the index", new Type("softlist<string>")));
        addField(new QoreDataField("unique", "if the index should be unique", BoolType, False));
    }
}
}
