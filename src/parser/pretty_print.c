#define _POSIX_C_SOURCE 200809L

#include "parser/pretty_print.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/ast.h"

static char *pp_prefix(struct ast *)
{
    return NULL;
}

static char *pp_rule_if(struct ast *ast)
{
    char *res = malloc(sizeof(char));

    return res;
}

static char *pp_simple_cmd(struct ast *ast)
{
    size_t len_res = 6;
    char *res = strdup("if { ");

    struct ast_simple_cmd *a = (struct ast_simple_cmd *)ast;

    char *prefix_str = pp_prefix(a->prefix);

    res = strcat(res, prefix_str);
    free(prefix_str);

    struct ast_prefix_list *pref_list =
        (struct ast_prefix_list *)a->prefix_list;

    while (pref_list != NULL)
    {
        prefix_str = pp_prefix(pref_list->prefix);

        res = strcat(res, prefix_str);
        free(prefix_str);

        pref_list = pref_list->next;
    }

    return res;
}

char *pretty_print(struct ast *ast)
{
    if (ast->type == AST_RULE_IF)
    {
        return pp_rule_if(ast);
    }
    else if (ast->type == AST_SIMPLE_CMD)
    {
        return pp_simple_cmd(ast);
    }
}
