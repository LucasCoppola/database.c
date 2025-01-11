#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "../include/database.h"
#include "../include/meta_commands.h"

void help() {
  printf("Available commands:\n");
  printf(".help\n");
  printf(".exit\n");
}

void process_meta_command(char *command, Database *db) {
  if (strcasecmp(command, META_CMD_HELP) == 0) {
    help();
  } else if (strcasecmp(command, META_CMD_EXIT) == 0) {
    database_close(db);
    exit(0);
  } else {
    printf("Error: unknown command or invalid arguments: '%s' Enter '.help' "
           "for help.\n",
           command);
  }
}
