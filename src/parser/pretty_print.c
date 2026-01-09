#define _POSIX_C_SOURCE 200809L

#include "parser/pretty_print.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/ast.h"

static void pp_prefix(struct ast *ast)
{
    struct ast_prefix *pref = (struct ast_prefix *)ast;

    if (pref->assignment_word != NULL)
    {
        printf(pref->assignment_word);
    }
    else
    {
        // TODO: add pp for the redirection part
    }
}

static void pp_rule_if(struct ast *ast)
{
    if (ast == NULL)
        return;
    printf("if { ");
}

static char *pp_simple_cmd(struct ast *ast)
{
    // how to differenciate the two different cases of the simple cmd ?

    struct ast_simple_cmd *a = (struct ast_simple_cmd *)ast;

    pp_prefix(a->prefix);

    struct ast_prefix_list *pref_list =
        (struct ast_prefix_list *)a->prefix_list;

    while (pref_list != NULL)
    {
        printf(" ");
        pp_prefix(pref_list->prefix);
        pref_list = (struct ast_prefix_list *)pref_list->next;
    }
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
