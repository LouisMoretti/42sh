#include <stdio.h>

#include "builtin.h"
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
    {
        *has_n = 0;
        *has_e = 0;
        *has_E = 0;
        return 0;
    }
    return 1;
}

int builtin_echo(struct ast_simple_cmd *command)
{
    struct element_list *cur = command->element_list;
    int has_n = 0;
    int has_e = 0;
    int has_E = 0;
    int has_left_flags = 0;
    while (cur != NULL)
    {
        char *str = cur->element->word;
        if (!has_left_flags)
        {
            if (str[0] != '-' || !update_flags(str, &has_n, &has_e, &has_E))
                has_left_flags = 1;
        }
        if (has_left_flags)
        {
            if (has_n)
                printf("%s", str);
            else
                printf("%s\n", str);
        }

        fflush(stdout);
        cur = cur->next;
    }
    return 0;
}
