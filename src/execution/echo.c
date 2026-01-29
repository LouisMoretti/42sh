#include <stdio.h>
#include <stdlib.h>

#include "execution/builtin.h"
#include "expansion/expansion.h"
#include "parser/ast.h"

static int update_flags(char *str, int *has_n, int *has_e, int *has_E)
{
    int i = 1;
    while (str[i])
    {
        if (str[i] == 'n')
            *has_n = 1;
        else if (str[i] == 'e')
        {
            if (*has_E)
                *has_E = 0;
            *has_e = 1;
        }
        else if (str[i] == 'E')
        {
            if (*has_e)
                *has_e = 0;
            *has_E = 1;
        }
        else
            break;
        i++;
    }
    if (str[i])
        return 0;
    return 1;
}

static int proceed_first(int has_e, struct ast_element *ast_element,
                         struct ast_element_list **cur)
{
    if (has_e)
    {
        char *expanded = expand_echo(ast_element->word);
        if (!expanded)
            return 1;
        printf("%s", expanded);
        free(expanded);
    }
    else
    {
        char *str = ast_element->word;
        printf("%s", str);
    }

    fflush(stdout);
    *cur = (struct ast_element_list *)(*cur)->next;
    return 0;
}

int builtin_echo(struct ast_simple_cmd *command)
{
    int has_n = 0;
    int has_e = 0;
    int has_E = 0;
    struct ast_element_list *cur =
        (struct ast_element_list *)command->element_list;
    if (!cur)
    {
        printf("\n");
        fflush(stdout);
        return 0;
    }
    struct ast_element *ast_element = (struct ast_element *)cur->element;
    char *str = ast_element->word;
    while (str[0] == '-' && update_flags(str, &has_n, &has_e, &has_E))
    {
        cur = (struct ast_element_list *)cur->next;
        ast_element = (struct ast_element *)cur->element;
        str = ast_element->word;
    }
    int code = proceed_first(has_e, ast_element, &cur);
    if (code != 0)
        return 1;
    while (cur != NULL)
    {
        ast_element = (struct ast_element *)cur->element;
        if (has_e)
        {
            char *expanded = expand_echo(ast_element->word);
            if (!expanded)
                return 1;
            printf(" %s", expanded);
            free(expanded);
        }
        else
        {
            char *str = ast_element->word;
            printf(" %s", str);
        }

        fflush(stdout);
        cur = (struct ast_element_list *)cur->next;
    }
    if (!has_n)
        printf("\n");
    fflush(stdout);
    return 0;
}
