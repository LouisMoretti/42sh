#define _POSIX_C_SOURCE 200809L
#include "parser/parser.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "lexer/lexer.h"
#include "parser/ast.h"

struct ast_input *parse_input()
{
    struct ast_input *ast_input = init_ast_input();

    struct ast_list *list = init_ast_list();
    ast_input->list = (struct ast *)list;

    struct ast_and_or *and_or = init_ast_and_or();
    list->and_or = (struct ast *)and_or;

    struct ast_pipeline *pipeline = init_ast_pipeline();
    and_or->pipeline = (struct ast *)pipeline;

    struct ast_cmd *cmd = init_ast_cmd();
    pipeline->cmd = (struct ast *)cmd;

    struct ast_simple_cmd *simple_command = parse_simple_cmd();
    cmd->cmd = (struct ast *)simple_command;

    return ast_input;
}

struct ast_simple_cmd *parse_simple_cmd()
{
    struct ast_simple_cmd *cmd = init_ast_simple_cmd();

    struct token *tok = peek_token(ENABLE_KEYWORDS);
    assert(tok->type == WORD);
    cmd->word = strdup(tok->data);
    pop_token();

    struct ast_element_list *list = NULL;
    tok = peek_token(DISABLE_KEYWORDS);
    while (tok != NULL && tok->type == WORD)
    {
        struct ast_element_list *tmp_element_list = init_ast_element_list();
        if (!list)
            cmd->element_list = (struct ast *)tmp_element_list;
        else
            list->next = (struct ast *)tmp_element_list;
        list = tmp_element_list;

        struct ast_element *tmp_element = init_ast_element();
        tmp_element->word = strdup(tok->data);

        list->element = (struct ast *)tmp_element;

        pop_token();
        tok = peek_token(DISABLE_KEYWORDS);
    }

    return cmd;
}
