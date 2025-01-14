# TODO

[] Pager
    - Files included:
        - database.c
        - table.c
        - row.c
        - pager.c
        - row_storage.c
        - statements.c
    
[] SQL query parser is mid, how robust should i make it?
    - [] end with semicolon(;)
    - [] replace literal scans for a better approach
        e.g. delete from %s where id = %d, is bad.


[] Add tests for the database and tables

- LOG_ERROR: It should be used one level above implementation
