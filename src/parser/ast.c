#include "ast.h"

#include <stdlib.h>

struct ast_simple_cmd *init_ast_simple_cmd()
{
    struct ast_simple_cmd *cmd = malloc(sizeof(struct ast_simple_cmd));

    cmd->base.type = AST_SIMPLE_CMD;
    cmd->element_list = NULL;
    cmd->prefix = NULL;
    cmd->prefix_list = NULL;
    cmd->word = NULL;

    return cmd;
}

struct element_list *init_element_list()
{
    struct element_list *list = malloc(sizeof(struct element_list));

    list->element.base.type = AST_ELEMENT;
    list->element.word = NULL;
    list->element.redirection = NULL;
    list->next = NULL;

    return list;
}
