#ifndef TEST_CONTEXT_H
#define TEST_CONTEXT_H

#include "parser/tokenizer.h"
#include "parser/ast.h"
#include "test_utils.h"

typedef struct TestContext {
    char *query;
    TokenizerState *state;
    ASTNode *node;
} TestContext;

static inline void test_context_init(TestContext *ctx, const char *query) {
    ctx->query = strdup(query);
    ctx->state = setup_tokenizer(query);
    ctx->node = NULL;
}

static inline void test_context_teardown(TestContext *ctx) {
    if (ctx->node) {
        ast_free(ctx->node);
        ctx->node = NULL;
    }
    if (ctx->state) {
        tokenizer_free(ctx->state);
        ctx->state = NULL;
    }
    if (ctx->query) {
        free(ctx->query);
        ctx->query = NULL;
    }
}

#endif
