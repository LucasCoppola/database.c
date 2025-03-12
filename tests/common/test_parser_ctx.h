#ifndef TEST_CONTEXT_H
#define TEST_CONTEXT_H

#include "parser/tokenizer.h"
#include "parser/ast.h"
#include "test_utils.h"

typedef struct TestParserCtx {
    char *query;
    TokenizerState *state;
    ASTNode *node;
} TestParserCtx;

static inline void test_parser_context_init(TestParserCtx *ctx, const char *query) {
    ctx->query = strdup(query);
    ctx->state = setup_tokenizer(query);
    ctx->node = NULL;
}

static inline void test_parser_context_teardown(TestParserCtx *ctx) {
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
