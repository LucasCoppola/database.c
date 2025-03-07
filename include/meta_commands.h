#ifndef META_COMMANDS_H
#define META_COMMANDS_H

#define META_CMD_HELP ".help"
#define META_CMD_EXIT  ".exit"
#define META_CMD_TABLES  ".tables"

typedef struct Database Database;

void process_meta_command(char *command, Database *db);

#endif
