# **Lightweight Database**  

## **Overview**  

This project is a lightweight database management system inspired by SQLite and written from scratch in C.

It serves as a practical exploration of database internals, focusing on core concepts such as query parsing, storage management, and execution while keeping the implementation simple and functional at this stage.

## **Components**  

### **1. Core Engine**  
- **Database**: Central structure that manages tables and coordinates operations.  
- **Table**: Represents database tables with columns, data types, and rows.
- **Row**: Handles serialization/deserialization of individual records.
- **Hashmap**: Provides table lookups by name. 

### **2. Storage Layer**  
- **Pager**: Manages reading/writing pages to disk with a simple paging system.  
- **Cursor**: Abstracts traversal through table rows.  
- **Header Management**: Functions that store and restore table metadata between sessions.  

### **3. Query Processing Pipeline**  
- **Tokenizer**: Converts SQL strings into tokens (lexical analysis).  
- **Parser**: Transforms tokens into an AST.
- **Semantic Analyzer**: Validates operations against the database schema.
- **Executor**: Performs the requested operations using the validated AST.

### **4. Logging**  
- **Debug Logger**: Captures and reports errors with component and operation context.
- **Parser Error Logger**: Specialized error reporting for syntax issues.
- **Semantic Error Logger**: Specialized error reporting for semantic issues.

## **Usage**

```bash
# Clone the repository
git clone https://github.com/LucasCoppola/database.c.git
cd database.c

# Build the project
make main

# Run with a database file
./main mydb.db
```

### **SQL Support**
Currently supported SQL commands:

```sql
-- Create a new table
CREATE TABLE products (id INT, name TEXT, price REAL, available BOOLEAN);

-- Insert data
INSERT INTO products VALUES (1, 'Apple', 1.99, true);

-- Query data
SELECT * FROM products;
SELECT name, price FROM products WHERE available = true;

-- Delete data
DELETE FROM products WHERE id = 1;

-- Drop a table
DROP TABLE products;

```
### **Testing**
The project includes both unit and integration tests that use Valgrind to check for memory leaks:

```bash
# Run unit tests
make test_hashmap
make test_tokenizer
make test_parser

# Run integration tests
make test_integration

# Run all tests
make test
```

### **Meta Commands**
Special commands available in the CLI:

- `.help` - Display available commands
- `.tables` - List all tables in the database
- `.schema` - Show the schema for all tables
- `.exit` - Exit the program

### **Development**
- **src/core/**: Core database structures (tables, rows, hashmap)
- **src/storage/**: Disk I/O and page management
- **src/parser/**: SQL parsing and AST generation
- **src/executor/**: Query execution logic
- **src/utils/**: Logging and utility functions

### **Requirements (Linux/macOS)**
- GCC compiler
- Valgrind (for memory leak detection in tests)


