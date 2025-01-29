# **Lightweight Database**  

## **Overview**  

This project is a lightweight database management system inspired by SQLite and written from scratch in C.

It serves as a practical exploration of database internals, focusing on core concepts such as query parsing, storage management, and indexing while keeping the implementation simple and functional at this stage.

## **Components**  

### **1. Database & Storage**  
- **Database File**: A single file that stores table metadata and row data.  
- **Pager**: Each table manages its own pager, which handles storage as an array rather than a B-tree. This keeps storage straightforward while maintaining clear table separation. 

### **2. Table Management**  
- **Table**: Represents a collection of rows within the database.  
- **Row**: Each row is a record within a table.  

### **3. HashMap**  
- **HashMap**: A data structure used for storing tables in memory, allowing for efficient retrieval and management.  

### **4. Storage Management**  
- **Header Store/Restore**: Functions that manage the metadata of tables, ensuring that everything can be correctly restored after closing the database.  

### **5. Query Parsing**  
- **Tokenizer**: Breaks down SQL-like commands into manageable tokens for easier processing.  
- **Query Parser**: Interprets those tokens and prepares statements for execution.  

### **6. Error Logging**  
- **Logger**: A simple logging mechanism to capture and report errors that occur during database operations.  

### **7. Testing**  
- **Unit Tests**: A suite of tests designed to validate the functionality of the HashMap and other components, ensuring reliability and correctness.  

## **A Note on Optimization**  

Many components in this project are not fully optimized yet. The goal was to keep things straightforward to better understand how each feature works. 

For instance, storage is currently managed using an array instead of a B-tree, and table management relies on a HashMap rather than a B+ tree. While these choices make implementation simpler, they leave room for future improvements in efficiency and scalability.

## **Future Work**  

- Building a more robust query execution engine.  
- Improving error handling and user feedback.  
- Optimizing the pager to handle pages more efficiently.  
- Replacing the array-based storage with a B-tree for better indexing and retrieval.  
- Replacing the HashMap used for table management with a B+ tree for improved performance and scalability.
- Adding support for more data types and operations.

Contributions and suggestions for improvement are always welcome.
