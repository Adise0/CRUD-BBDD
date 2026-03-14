# CRUD

### 1. What is the CRUD concept?

`CRUD` is an acronym for `Create`, `Read`, `Update` and `Delete`. In the context of a BBDD It's usually refering to the program that handles these actions on the database.

### 1.1 Object and Structure

For this exercise I choose the following structure:

- **DATABASE NAME**: `classroom`
- **TABLE NAME**: `computers`
- **OBJECT NAME**: `Computer`

```json
// COMPUTER STRUCTURE
{
  "id": "INT",
  "name": "String",
  "isActive": "BOOL",
  "type": ["WORKSTATION", "GAMING", "LAPTOP"]
}
```

### 2. Creating the table and enum

You can find the `SQL` statements used to create the table and enum in the [2-CreatingStructures_Table-Enum.sql](./sql/2-CreatingStructures_Table-Enum.sql) file.

### 3. Creating the table and enum

You can find the `SQL` statements used to create the `Computer` struct in the [3-CreatingStructures_Computer.sql](./sql/3-CreatingStructures_Computer.sql) file.
