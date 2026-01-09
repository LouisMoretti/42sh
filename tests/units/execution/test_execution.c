#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/parameterized.h>
#include <criterion/redirect.h>
#include <stddef.h>
#include <string.h>

#include "config/config.h"
#include "parser/ast.h"

TestSuite(Execution);

Test(Execution, simple_cmd)
{
    struct ast_simple_cmd *ast_simple_cmd =
        malloc(sizeof(struct ast_simple_cmd));

    ast_simple_cmd->prefix_list = NULL;
    ast_simple_cmd->prefix = NULL;
    ast_simple_cmd->word = "ls";

    struct ast_element_list *ast_element_list =
        malloc(sizeof(struct ast_element_list));

    ast_element_list->base = AST_ELEMENT_LIST;

    struct ast_element *ast_element = malloc(sizeof(struct ast_element));
    ast_element->base = AST_ELEMENT;
    ast_element->word = "-a";
    ast_element->redirection = NULL;

    ast_element_list->element = ast_element;
    ast_element_list->next = NULL;

    ast_simple_cmd->element_list = ast_element_list;

    struct ast *ast = (struct ast *)ast_simple_cmd;

    int res = execute_ast(ast);
    cr_expect(res == 0);
}
