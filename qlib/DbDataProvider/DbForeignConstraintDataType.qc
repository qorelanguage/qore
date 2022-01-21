#! contains all public definitions in the DbDataProvider module
public namespace DbDataProvider {
#! The type definition for one foreign constraint
public class DbForeignConstraintDataType inherits HashDataType {
    #! Creates the type
    constructor() {
        # add members
        addField(new QoreDataField("columns", "the list of columns making up the foreign constraint",
            new Type("softlist<string>")));
        addField(new QoreDataField("table", "if the table the constraint depends on", StringType));
        addField(new QoreDataField("target_columns", "this value is only necessary if the columns in the foreign "
            "table have different names than in the current table; if so, it must be assigned to a single column "
            "name or a list of column names in the foreign table; if this key is present then the same number of "
            "columns must appear in this option and in the columns option", new Type("softlist<string>")));
    }
}
}
