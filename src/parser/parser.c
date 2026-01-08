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

    struct element_list *list = NULL;
    tok = peek_token(DISABLE_KEYWORDS);
    while (tok->type == WORD)
    {
        struct element_list *tmp = init_element_list();
        if (!list)
            cmd->element_list = tmp;
        else
            list->next = tmp;
        list = tmp;

        pop_token();
        tok = peek_token(DISABLE_KEYWORDS);
    }

    return cmd;
}
