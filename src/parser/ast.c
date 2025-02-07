#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../include/parser.h"

ASTNode *create_ast_node(NodeType type) {
  ASTNode *node = malloc(sizeof(ASTNode));
  if (!node) {
    fprintf(stderr, "Failed to allocate new ast node");
    return NULL;
  }

  node->type = type;
  return node;
}

void ast_free(ASTNode *node) {
  if (!node)
    return;

  switch (node->type) {
  case NODE_CREATE_TABLE:
    free(node->create_table.columns);
    break;
  case NODE_SELECT:
    for (int i = 0; i < node->select_rows.num_columns; i++) {
      free(node->select_rows.select_columns[i]);
    }
    free(node->select_rows.select_columns);
    break;
  default:
    break;
  }

  free(node);
}
