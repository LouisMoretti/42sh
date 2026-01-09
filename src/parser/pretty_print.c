#define _POSIX_C_SOURCE 200809L

#include "parser/pretty_print.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/ast.h"

static char *pp_prefix(struct ast *ast)
{
    struct ast_prefix *pref = (struct ast_prefix *)ast;

    if (pref->assignment_word != NULL)
    {
        return strdup(pref->assignment_word);
    }
    else
    {
        // TODO: add pp for the redirection part
        return NULL;
    }
}

static char *pp_rule_if(struct ast *ast)
{
    if (ast == NULL)
        return NULL;
    // size_t len_res = 6;
    char *res = strdup("if { ");

    return res;
}

static char *pp_simple_cmd(struct ast *ast)
{
    // how to differenciate the two different cases of the simple cmd ?

    struct ast_simple_cmd *a = (struct ast_simple_cmd *)ast;

    char *res = pp_prefix(a->prefix);

    size_t len_res = strlen(res);

    struct ast_prefix_list *pref_list =
        (struct ast_prefix_list *)a->prefix_list;

    while (pref_list != NULL)
    {
        char *prefix_str = pp_prefix(pref_list->prefix);

        if (prefix_str == NULL)
        {
            free(res);
            return NULL;
        }

        len_res += strlen(prefix_str);
        res = realloc(res, sizeof(char) * len_res);
        res = strcat(res, prefix_str);
        free(prefix_str);
        pref_list = (struct ast_prefix_list *)pref_list->next;
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
