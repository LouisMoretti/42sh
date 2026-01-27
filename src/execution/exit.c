#include <stdlib.h>

#include "builtin.h"
#include "config/config.h"

int builtin_exit(struct ast_simple_cmd *ast_simple_cmd)
{
    int res = 0;

    if (ast_simple_cmd->element_list != NULL)
    {
        struct ast_element_list *ast_element_list =
            (struct ast_element_list *)ast_simple_cmd->element_list;

        struct ast_element *ast_element =
            (struct ast_element *)ast_element_list->element;

        if (ast_element != NULL && ast_element->word != NULL)
        {
            res = atoi(ast_element->word);
        }
    }
    else
    {
        struct config *config = get_conf();
        res = config->previous_code;
    }

    return res;
}
