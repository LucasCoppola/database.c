#include <stdbool.h>
#include <stdio.h>

#include "core/database.h"
#include "executor/executor.h"
#include "parser/ast.h"

ExecuteResult execute_ast(Database *db, ASTNode *node) {
  if (!node) {
    fprintf(stderr, "Error: Null AST node\n");
    return EXECUTE_FAILURE;
  }

  switch (node->type) {
  case NODE_CREATE_TABLE:
    return execute_create_table(db, node);
  case NODE_DROP_TABLE:
    return execute_drop_table(db, node);
  // case NODE_INSERT:
  //   return execute_insert(node);
  // case NODE_SELECT:
  //   return execute_select(node);
  // case NODE_DELETE:
  //   return execute_delete(node);
  default:
    fprintf(stderr, "Error: Unknown node type %d\n", node->type);
    return EXECUTE_FAILURE;
  }
}
