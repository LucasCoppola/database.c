#include <stdbool.h>
#include <stdio.h>

#include "../../include/ast.h"
#include "../../include/database.h"
#include "../../include/executor.h"

ExecuteResult execute_ast(Database *db, ASTNode *node) {
  if (!node) {
    fprintf(stderr, "Error: Null AST node\n");
    return EXECUTE_FAILURE;
  }

  switch (node->type) {
  case NODE_CREATE_TABLE:
    return execute_create_table(db, node);
    break;
  // case NODE_INSERT:
  //   execute_insert(node);
  //   break;
  // case NODE_SELECT:
  //   execute_select(node);
  //   break;
  // case NODE_DROP_TABLE:
  //   execute_drop_table(node);
  //   break;
  // case NODE_DELETE:
  //   execute_delete(node);
  //   break;
  default:
    fprintf(stderr, "Error: Unknown node type %d\n", node->type);
    return EXECUTE_FAILURE;
  }
}
