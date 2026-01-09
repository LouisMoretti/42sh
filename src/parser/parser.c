#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "lexer/lexer.h"
#include "parser/ast.h"

struct ast_simple_cmd *simple_cmd()
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
