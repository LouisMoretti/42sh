#define _POSIX_C_SOURCE 200809L

#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>
#include <stddef.h>
#include <string.h>

#include "config/config.h"
#include "execution/builtin.h"
#include "execution/execution.h"

TestSuite(Execution, .timeout = 1);

Test(Execution, test_simple_cmd, .init = cr_redirect_stdout)
{
    struct ast_simple_cmd *ast_simple_cmd =
        malloc(sizeof(struct ast_simple_cmd));

    ast_simple_cmd->base.type = AST_SIMPLE_CMD;
    ast_simple_cmd->prefix_list = NULL;
    // ast_simple_cmd->prefix = NULL;
    ast_simple_cmd->word = strdup("echo");

    struct ast_element_list *ast_element_list =
        malloc(sizeof(struct ast_element_list));

    ast_element_list->base.type = AST_ELEMENT_LIST;

    struct ast_element *ast_element = malloc(sizeof(struct ast_element));
    ast_element->base.type = AST_ELEMENT;
    ast_element->word = strdup("coucou");
    ast_element->redirection = NULL;

    ast_element_list->element = (struct ast *)ast_element;
    ast_element_list->next = NULL;

    ast_simple_cmd->element_list = (struct ast *)ast_element_list;

    struct ast *ast = (struct ast *)ast_simple_cmd;

    int res = execute_ast(ast);

    free(ast_element->word);
    free(ast_element);
    free(ast_element_list);
    free(ast_simple_cmd->word);
    free(ast_simple_cmd);

    cr_expect(res == 0);
}

Test(Execution, test_cmd_builtin_true)
{
    struct ast_simple_cmd *ast_simple_cmd =
        malloc(sizeof(struct ast_simple_cmd));

    ast_simple_cmd->base.type = AST_SIMPLE_CMD;
    ast_simple_cmd->prefix_list = NULL;
    // ast_simple_cmd->prefix = NULL;
    ast_simple_cmd->word = strdup("true");
    ast_simple_cmd->element_list = NULL;

    struct ast *ast = (struct ast *)ast_simple_cmd;

    int res = execute_ast(ast);

    free(ast_simple_cmd->word);
    free(ast_simple_cmd);

    cr_expect(res == 0);
}

Test(Execution, test_cmd_builtin_false)
{
    struct ast_simple_cmd *ast_simple_cmd =
        malloc(sizeof(struct ast_simple_cmd));

    ast_simple_cmd->base.type = AST_SIMPLE_CMD;
    ast_simple_cmd->prefix_list = NULL;
    // ast_simple_cmd->prefix = NULL;
    ast_simple_cmd->word = strdup("false");
    ast_simple_cmd->element_list = NULL;

    struct ast *ast = (struct ast *)ast_simple_cmd;

    int res = execute_ast(ast);

    free(ast_simple_cmd->word);
    free(ast_simple_cmd);

    cr_expect(res == 1);
}
