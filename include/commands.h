#ifndef COMMANDS_H
#define COMMANDS_H

#define CMD_CREATE_TABLE "CREATE TABLE"
#define CMD_INSERT_INTO "INSERT INTO"
#define CMD_SELECT "SELECT * FROM"
#define CMD_DELETE "DELETE FROM"
#define CMD_HELP "help"
#define CMD_EXIT  "exit"

#include <stdio.h>
#include "database.h"

void process_command(Database *db, char *command);

#endif
