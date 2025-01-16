# TODO

[x] Fix HeaderMetadata (supports a single table 'users')
[] Add metadata to store/restore rows
    
[] Optimize pager
[] SQL query parser is mid, how robust should i make it?
    - [] end with semicolon(;)
    - [] replace literal scans for a better approach
        e.g. delete from %s where id = %d, is bad.


[] Add tests for the database and tables

+----------------------+
|     Table Headers    |
+----------------------+
| Table 1 Header       |
| - Table Name: users  |
| - Num Rows: 100      |
| - Next ID: 101       |
+----------------------+
| Table 2 Header       |
| - Table Name: posts  |
| - Num Rows: 200      |
| - Next ID: 201       |
+----------------------+
| ...                  |
| (up to MAX_TABLES)   |
+----------------------+

+----------------------+
|      Data Pages      |
+----------------------+
| Users Data (Page 1)  |
+----------------------+
| Users Data (Page 2)  |
+----------------------+
| Posts Data (Page 1)  |
+----------------------+
| Posts Data (Page 2)  |
+----------------------+
| ...                  |
| (continuing data)    |
+----------------------+

insert into admins (username, email) values ('lucas', 'lucas@lucas.com')
insert into admins (username, email) values ('john', 'john@john.com')
insert into bans (username, email) values ('jane', 'jane@jane.com')
insert into bans (username, email) values ('peter', 'peter@peter.com')
