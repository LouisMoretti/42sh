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

    struct ast *list = init_ast(AST_LIST);
    ((struct ast_input *)ast_input)->list = list;

    struct ast *and_or = init_ast(AST_AND_OR);
    ((struct ast_list *)list)->and_or = and_or;

    struct ast *pipeline = init_ast(AST_PIPELINE);
    ((struct ast_and_or *)and_or)->pipeline = pipeline;

    struct ast *cmd = init_ast(AST_CMD);
    ((struct ast_pipeline *)pipeline)->cmd = cmd;

    struct ast *simple_command = parse_simple_cmd();
    ((struct ast_cmd *)cmd)->cmd = simple_command;

    return ast_input;
}

struct ast *parse_simple_cmd()
{
    struct ast *cmd = init_ast(AST_SIMPLE_CMD);

    struct token *tok = peek_token(ENABLE_KEYWORDS);
    assert(tok->type == WORD);
    ((struct ast_simple_cmd *)cmd)->word = strdup(tok->data);
    pop_token();

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
