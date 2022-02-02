#! contains all public definitions in the DbDataProvider module
public namespace DbDataProvider {
#! The type definition for foreign constraints
public class DbForeignConstraintsDataType inherits HashDataType {
    #! Creates the type
    constructor() {
        setDefaultOtherFieldType(new DbForeignConstraintDataType());
    }
}
}
