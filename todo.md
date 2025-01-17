# TODO

[] Restoring rows from disk:
    - I need to calculate the page offset
    - Only one row gets written on disk
    - The same row is being grabbed for all the tables (this is the offset fault anyway)

    

[] Optimize pager
[] SQL query parser is mid, how robust should i make it?
    - [] end with semicolon(;)
    - [] replace literal scans for a better approach
        e.g. delete from %s where id = %d, is bad.


[] Add tests for the database and tables

Offset (hex)  | Hexadecimal Data                                   | ASCII Representation
--------------|----------------------------------------------------|---------------------
00000000      | 03 00 00 00                                        | ....                # Number of tables: 3
00000004      | 75 73 65 72 73 00 00 00 00 00 00 00 00 00 00 00    | users...........    # Table "users" header (padded)
00000014      | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    | ................    # Empty rows for "users"
00000024      | 61 64 6D 69 6E 73 00 00 00 00 00 00 00 00 00 00    | admins..........    # Table "admins" header (padded)
00000034      | 02 00 00 00                                        | ....                # Number of rows: 2
00000038      | 62 61 6E 73 00 00 00 00 00 00 00 00 00 00 00 00    | bans............    # Table "bans" header (padded)
00000048      | 02 00 00 00                                        | ....                # Number of rows: 2

# Pages Start Here
0000004C      | 01 00 00 00 6C 75 63 61 73 00 6C 75 63 61 73 40    | ....lucas.lucas@    # Page 1: Row 1 of "admins"
0000005C      | 6C 75 63 61 73 2E 63 6F 6D 00 00 00 00 00 00 00    | lucas.com.......    # Continuation of Row 1
0000006C      | 02 00 00 00 6A 6F 68 6E 00 6A 6F 68 6E 40 6A 6F    | ....john.john@jo    # Page 2: Row 2 of "admins"
0000007C      | 68 6E 2E 63 6F 6D 00 00 00 00 00 00 00 00 00 00    | hn.com..........    # Continuation of Row 2

# Next table pages for "bans"
0000008C      | 01 00 00 00 6A 61 6E 65 00 6A 61 6E 65 40 6A 61    | ....jane.jane@ja    # Page 3: Row 1 of "bans"
0000009C      | 6E 65 2E 63 6F 6D 00 00 00 00 00 00 00 00 00 00    | ne.com..........    # Continuation of Row 1
000000AC      | 02 00 00 00 70 65 74 65 72 00 70 65 74 65 72 40    | ....peter.peter@    # Page 4: Row 2 of "bans"
000000BC      | 70 65 74 65 72 2E 63 6F 6D 00 00 00 00 00 00 00    | peter.com.......    # Continuation of Row 2


insert into admins ('lucas', 'lucas@lucas.com')
insert into admins ('john', 'john@john.com')
insert into bans ('jane', 'jane@jane.com')
insert into bans ('peter', 'peter@peter.com')
