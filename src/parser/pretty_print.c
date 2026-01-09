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
        printf("%s", pref->assignment_word);
    }
    else
    {
        // TODO: add pp for the redirection part
    }
}

static void pp_element(struct ast *ast)
{
    struct ast_element *elm = (struct ast_element *)ast;

    if (elm->word != NULL)
    {
        printf("%s", elm->word);
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

    printf("\n");
}

static void pp_simple_cmd(struct ast *ast)
{
    struct ast_simple_cmd *a = (struct ast_simple_cmd *)ast;

    if (a->prefix != NULL)
    {
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
    else
    {
        struct ast_prefix_list *pref_list =
            (struct ast_prefix_list *)a->prefix_list;

        while (pref_list != null)
        {
            pp_prefix(pref_list->prefix);
            pref_list = (struct ast_prefix_list *)pref_list->next;
            printf(" ");
        }

        printf(a->word);
        printf(" ");

        struct ast_element_list *elm_list =
            (struct ast_element_list *)a->element_list;

        while (element_list != null)
        {
            printf(" ");
            pp_element(elm_list->element);
            elm_list = (struct ast_element_list *)elm_list->next;
        }
    }

    printf("\n");
}

void pretty_print(struct ast *ast)
{
    if (ast->type == AST_RULE_IF)
    {
        pp_rule_if(ast);
    }
    else if (ast->type == AST_SIMPLE_CMD)
    {
        pp_simple_cmd(ast);
    }
}
