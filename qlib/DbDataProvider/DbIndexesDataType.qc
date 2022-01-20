#! contains all public definitions in the DbDataProvider module
public namespace DbDataProvider {
#! The type definition for one index
public class DbIndexesDataType inherits HashDataType {
    #! Creates the type
    constructor() {
        setDefaultOtherFieldType(new DbIndexDataType());
    }
}
}
