#define _POSIX_C_SOURCE 200809L
#include "parser/parser.h"

#include <err.h>
#include <stddef.h>
#include <string.h>

#include "lexer/lexer.h"
#include "parser/ast.h"

// static struct token *pop_peek_token(enum keyword_policy policy)
// {
//     pop_token();
//     return peek_token(policy);
// }

const char *type_name[] = { [IF] = "IF",
                            [THEN] = "THEN",
                            [ELIF] = "ELIF",
                            [ELSE] = "ELSE",
                            [FI] = "FI",
                            // [KEYWORD_COUNT] = "KEYWORD_COUNT",
                            [NEW_LINE] = "NEW_LINE",
                            [SEMICOLON] = "SEMICOLON",
                            [WORD] = "WORD",
                            [END_OF_FILE] = "END_OF_FILE" };

struct ast *parse_input()
{
    struct ast *ast_input = init_ast(AST_INPUT);

    struct token *tok = peek_token(ENABLE_KEYWORDS);
    if (tok->type != END_OF_FILE && tok->type != NEW_LINE)
        ((struct ast_input *)ast_input)->list = parse_list();

    // TODO: Return error.
    if (tok->type != END_OF_FILE && tok->type != NEW_LINE)
        warnx("parse_input: Wrong token type at end of input. Expected "
              "END_OF_FILE or NEW_LINE | Got: %s",
              type_name[tok->type]);

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
    // TODO: Add handle token '&'.
    if (peek_token(ENABLE_KEYWORDS)->type == SEMICOLON)
        pop_token();

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
    // TODO: Return error.
    if (tok->type != WORD)
        warnx("parse_simple_cmd: Wrong token type. Expected WORD | Got: %s",
              type_name[tok->type]);
    ((struct ast_simple_cmd *)cmd)->word = strdup(tok->data);
    pop_token();

    // TODO: Refaco AST_ELEMENT_LIST, call sub function
    struct ast *element_list = NULL;
    tok = peek_token(DISABLE_KEYWORDS);
    // TODO: Can a token be null?
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

    struct token *tok = peek_token(ENABLE_KEYWORDS);

    // TODO: Step 2: Add other rules.
    if (tok->type != IF)
        warnx(
            "parse_shell_cmd: Unsupported shell command. Expected IF | Got: %s",
            type_name[tok->type]);

    if (tok->type == IF)
        ((struct ast_shell_cmd *)cmd)->rule = parse_rule_if();

    return cmd;
}

struct ast *parse_rule_if()
{
    struct ast *rule_if = init_ast(AST_RULE_IF);

    struct token *tok = peek_token(ENABLE_KEYWORDS);

    // TODO: Return error.
    if (tok->type != IF)
        warnx("parse_rule_if: Wrong token type. Expected IF | Got: %s",
              type_name[tok->type]);

    pop_token();

    ((struct ast_rule_if *)rule_if)->condition_compound_list =
        parse_compound_list();

    tok = peek_token(ENABLE_KEYWORDS);
    // TODO: Return error.
    if (tok->type != THEN)
        warnx("parse_rule_if: Wrong token type. Expected THEN | Got: %s",
              type_name[tok->type]);
    pop_token();

    ((struct ast_rule_if *)rule_if)->body_compound_list = parse_compound_list();

    tok = peek_token(ENABLE_KEYWORDS);
    if (tok->type == ELIF || tok->type == ELSE)
        ((struct ast_rule_if *)rule_if)->else_clause = parse_else_clause();

    tok = peek_token(ENABLE_KEYWORDS);
    // TODO: Return error.
    if (tok->type != FI)
        warnx("parse_rule_if: Wrong token type. Expected FI | Got: %s",
              type_name[tok->type]);
    pop_token();

    return rule_if;
}

struct ast *parse_compound_list()
{
    struct ast *compound_list = init_ast(AST_COMPOUND_LIST);

    while (peek_token(ENABLE_KEYWORDS)->type == NEW_LINE)
    {
        pop_token();
    }

    ((struct ast_compound_list *)compound_list)->ast_and_or = parse_and_or();

    // TODO: Add handle token '&'.
    struct token *tok = peek_token(ENABLE_KEYWORDS);
    if (tok->type == SEMICOLON || tok->type == NEW_LINE)
    {
        pop_token();
        tok = peek_token(ENABLE_KEYWORDS);

        // TODO: May be unnecessary.
        while (tok->type == NEW_LINE)
        {
            pop_token();
            tok = peek_token(ENABLE_KEYWORDS);
        }

        // TODO: Verify condition.
        if (tok->type > KEYWORD_COUNT)
            ((struct ast_compound_list *)compound_list)->next =
                parse_compound_list();
    }

    // TODO: Add handle token '&'.
    if (peek_token(ENABLE_KEYWORDS)->type == SEMICOLON)
        pop_token();

    while (peek_token(ENABLE_KEYWORDS)->type == NEW_LINE)
        pop_token();

    return compound_list;
}

struct ast *parse_else_clause()
{
    // TODO: Return error.
    if (peek_token(ENABLE_KEYWORDS)->type != ELIF
        && peek_token(ENABLE_KEYWORDS)->type != ELSE)
        warnx("parse_else_clause: Wrong entry token type. Expected ELIF or "
              "ELSE | "
              "Got: %s",
              type_name[peek_token(ENABLE_KEYWORDS)->type]);

    struct ast *else_clause = init_ast(AST_CLAUSE_ELSE);
    int is_elif = 0;

    if (peek_token(ENABLE_KEYWORDS)->type == ELIF)
        is_elif = 1;
    pop_token();

    if (is_elif)
    {
        ((struct ast_else_clause *)else_clause)->condition_compound_list =
            parse_compound_list();

        if (peek_token(ENABLE_KEYWORDS)->type != THEN)
            warnx(
                "parse_else_clause: Wrong token type. Expected THEN | Got: %s",
                type_name[peek_token(ENABLE_KEYWORDS)->type]);
        pop_token();
    }

    ((struct ast_else_clause *)else_clause)->body_compound_list =
        parse_compound_list();

    // TODO: Grammar error check: token can only be ELIF or ELSE or FI.
    if (is_elif && (peek_token(ENABLE_KEYWORDS)->type != FI))
    {
        if (peek_token(ENABLE_KEYWORDS)->type != ELIF
            && peek_token(ENABLE_KEYWORDS)->type != ELSE)
            warnx("parse_else_clause: Wrong token type. Expected ELIF or ELSE "
                  "| Got: %s",
                  type_name[peek_token(ENABLE_KEYWORDS)->type]);
        ((struct ast_else_clause *)else_clause)->else_clause =
            parse_else_clause();
    }

    return else_clause;
}
