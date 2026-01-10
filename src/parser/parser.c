#define _POSIX_C_SOURCE 200809L
#include "parser/parser.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "lexer/lexer.h"
#include "parser/ast.h"

struct ast *parse_input()
{
    struct ast *ast_input = init_ast(AST_INPUT);

    if (peek_token(ENABLE_KEYWORDS)->type != END_OF_FILE)
        ((struct ast_input *)ast_input)->list = parse_list();

    return ast_input;
}

struct ast *parse_list()
{
    struct ast *list = init_ast(AST_LIST);

    ((struct ast_list *)list)->and_or = parse_and_or();

    // TODO: Add handle token '&'.
    if (peek_token(ENABLE_KEYWORDS)->type == SEMICOLON)
    {
        pop_token();
        if (peek_token(ENABLE_KEYWORDS)->type != END_OF_FILE)
            ((struct ast_list *)list)->next = parse_list();
    }

    return list;
}

struct ast *parse_and_or()
{
    struct ast *and_or = init_ast(AST_AND_OR);

    ((struct ast_and_or *)and_or)->pipeline = parse_pipeline();

    // TODO: Step 2.
    // if (peek_token(ENABLE_KEYWORDS)->type == DOUBLE_PIPE
    //     || peek_token(ENABLE_KEYWORDS)->type == DOUBLE_AMPERSAND)
    // {
    // }

    return and_or;
}

struct ast *parse_pipeline()
{
    struct ast *pipeline = init_ast(AST_PIPELINE);

    // TODO: Step 2.
    // if (peek_token(ENABLE_KEYWORDS)->type == NEGATION)
    // {
    // }

    ((struct ast_pipeline *)pipeline)->cmd = parse_cmd();

    // TODO: Step 2.
    // if (peek_token(ENABLE_KEYWORDS)->type == PIPE)
    // {
    // }

    return pipeline;
}

struct ast *parse_cmd()
{
    struct ast *cmd = init_ast(AST_CMD);

    if (peek_token(ENABLE_KEYWORDS)->type == WORD)
        ((struct ast_cmd *)cmd)->cmd = parse_simple_cmd();
    else
        ((struct ast_cmd *)cmd)->cmd = parse_shell_cmd();

    return cmd;
}

struct ast *parse_simple_cmd()
{
    struct ast *cmd = init_ast(AST_SIMPLE_CMD);

    // TODO: Command with prefix.

    struct token *tok = peek_token(ENABLE_KEYWORDS);
    assert(tok->type == WORD);
    ((struct ast_simple_cmd *)cmd)->word = strdup(tok->data);
    pop_token();

    // TODO: Refaco AST_ELEMENT_LIST, call sub function
    struct ast *element_list = NULL;
    tok = peek_token(DISABLE_KEYWORDS);
    while (tok != NULL && tok->type == WORD)
    {
        struct ast *tmp_element_list = init_ast(AST_ELEMENT_LIST);
        if (!element_list)
            ((struct ast_simple_cmd *)cmd)->element_list = tmp_element_list;
        else
            ((struct ast_element_list *)element_list)->next = tmp_element_list;
        element_list = tmp_element_list;

        struct ast *tmp_element = init_ast(AST_ELEMENT);
        ((struct ast_element *)tmp_element)->word = strdup(tok->data);

        ((struct ast_element_list *)element_list)->element = tmp_element;

        pop_token();
        tok = peek_token(DISABLE_KEYWORDS);
    }

    return cmd;
}

struct ast *parse_shell_cmd()
{
    struct ast *cmd = init_ast(AST_SHELL_CMD);

    // TODO: Step 2: Add other rules.
    assert(peek_token(ENABLE_KEYWORDS)->type == IF);
    if (peek_token(ENABLE_KEYWORDS)->type == IF)
        ((struct ast_shell_cmd *)cmd)->rule = parse_rule_if();

    return cmd;
}

struct ast *parse_rule_if()
{
    struct ast *rule_if = init_ast(AST_RULE_IF);

    assert(peek_token(ENABLE_KEYWORDS)->type == IF);
    pop_token();

    // assert(peek_token(DISABLE_KEYWORDS)->type == WORD);
    ((struct ast_rule_if *)rule_if)->condition_compound_list =
        parse_compound_list();

    assert(peek_token(ENABLE_KEYWORDS)->type == THEN);
    pop_token();

    // assert(peek_token(DISABLE_KEYWORDS)->type == WORD);
    ((struct ast_rule_if *)rule_if)->body_compound_list = parse_compound_list();

    if (peek_token(ENABLE_KEYWORDS)->type == ELIF
        || peek_token(ENABLE_KEYWORDS)->type == ELSE)
        ((struct ast_rule_if *)rule_if)->else_clause = parse_else_clause();

    assert(peek_token(ENABLE_KEYWORDS)->type == FI);
    pop_token();

    return rule_if;
}

struct ast *parse_compound_list()
{
    struct ast *compound_list = init_ast(AST_COMPOUND_LIST);

    ((struct ast_compound_list *)compound_list)->ast_and_or = parse_and_or();

    // TODO: Add handle token '&' and '\n'.
    if (peek_token(ENABLE_KEYWORDS)->type == SEMICOLON)
    {
        pop_token();
        if (peek_token(ENABLE_KEYWORDS)->type > KEYWORD_COUNT)
            ((struct ast_compound_list *)compound_list)->next =
                parse_compound_list();
    }

    return compound_list;
}

struct ast *parse_else_clause()
{
    assert(peek_token(ENABLE_KEYWORDS)->type == ELIF
           || peek_token(ENABLE_KEYWORDS)->type == ELSE);

    struct ast *else_clause = init_ast(AST_CLAUSE_ELSE);
    int is_elif = 0;

    if (peek_token(ENABLE_KEYWORDS)->type == ELIF)
        is_elif = 1;
    pop_token();

    if (is_elif)
    {
        ((struct ast_else_clause *)else_clause)->condition_compound_list =
            parse_compound_list();

        assert(peek_token(ENABLE_KEYWORDS)->type == THEN);
        pop_token();
    }

    ((struct ast_else_clause *)else_clause)->body_compound_list =
        parse_compound_list();

    if (is_elif)
    {
        assert(peek_token(ENABLE_KEYWORDS)->type == ELIF
               || peek_token(ENABLE_KEYWORDS)->type == ELSE);
        ((struct ast_else_clause *)else_clause)->else_clause =
            parse_else_clause();
    }

    return else_clause;
}
