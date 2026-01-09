#include "builtin.h"

int builtin_false(struct ast_simple_cmd *ast_simple_cmd)
{
    return 1;
}

int builtin_true(struct ast_simple_cmd *ast_simple_cmd)
{
    return 0;
}
