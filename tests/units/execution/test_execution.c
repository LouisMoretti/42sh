#define _POSIX_C_SOURCE 200809L

#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "config/config.h"
#include "execution/builtin.h"
#include "execution/execution.h"

static char *merge(char *src1, char *src2)
{
    // Return NULL if one the parameters is NULL.
    if (!src1 || !src2)
    {
        free(src1);
        free(src2);
        return NULL;
    }

    // If one string is empty return the other.
    if (src1[0] == 0)
    {
        free(src1);
        return src2;
    }
    else if (src2[0] == 0)
    {
        free(src2);
        return src1;
    }

    // Get total length and resize.
    size_t len = strlen(src1) + strlen(src2) + 1;

    char *res = realloc(src1, len);
    if (!res)
    {
        free(src1);
        free(src2);
        return NULL;
    }

    // Merge strings
    res = strcat(res, src2);

    // Free other string
    free(src2);
    res[len - 1] = 0;

    return res;
}

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

    cr_expect_eq(res, 0);
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

    cr_expect_eq(res, 0);
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

    cr_expect_eq(res, 1);
}

Test(Execution, test_cmd_builtin_cd_no_arg)
{
    struct ast_simple_cmd *ast_simple_cmd =
        malloc(sizeof(struct ast_simple_cmd));

    ast_simple_cmd->base.type = AST_SIMPLE_CMD;
    ast_simple_cmd->prefix_list = NULL;
    // ast_simple_cmd->prefix = NULL;
    ast_simple_cmd->word = strdup("cd");
    ast_simple_cmd->element_list = NULL;

    struct ast *ast = (struct ast *)ast_simple_cmd;

    int res = execute_ast(ast);

    free(ast_simple_cmd->word);
    free(ast_simple_cmd);

    cr_expect_eq(res, 0);
}

Test(Execution, test_cmd_builtin_cd_classic_arg)
{
    char *old_path = getenv("PWD");
    int pid = fork();
    if (!pid)
    {
        char *argv[] = { "env", "-i", NULL };
        execvp("env", argv);
    }
    else
    {
        int wstatus;
        waitpid(pid, &wstatus, 0);

        struct ast_simple_cmd *ast_simple_cmd =
            malloc(sizeof(struct ast_simple_cmd));

        ast_simple_cmd->base.type = AST_SIMPLE_CMD;
        ast_simple_cmd->prefix_list = NULL;
        // ast_simple_cmd->prefix = NULL;
        ast_simple_cmd->word = strdup("cd");

        struct ast_element_list *ast_element_list =
            malloc(sizeof(struct ast_element_list));

        ast_element_list->base.type = AST_ELEMENT_LIST;

        struct ast_element *ast_element = malloc(sizeof(struct ast_element));
        ast_element->base.type = AST_ELEMENT;
        ast_element->word = strdup("hello/");
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

        char *new_path = getenv("PWD");
        char *new_old_path = getenv("OLDPWD");

        cr_expect_eq(res, 0);
        char *path = merge(strdup(old_path), strdup("/hello"));
        cr_expect_str_eq(new_path, path);
        cr_expect_str_eq(new_old_path, old_path);
        free(path);
    }
}

Test(Execution, test_cmd_builtin_cd_dash)
{
    char *old_path = getenv("PWD");
    int pid = fork();
    if (!pid)
    {
        char *argv[] = { "env", "-i", NULL };
        execvp("env", argv);
    }
    else
    {
        int wstatus;
        waitpid(pid, &wstatus, 0);

        struct ast_simple_cmd *ast_simple_cmd =
            malloc(sizeof(struct ast_simple_cmd));

        ast_simple_cmd->base.type = AST_SIMPLE_CMD;
        ast_simple_cmd->prefix_list = NULL;
        // ast_simple_cmd->prefix = NULL;
        ast_simple_cmd->word = strdup("cd");

        struct ast_element_list *ast_element_list =
            malloc(sizeof(struct ast_element_list));

        ast_element_list->base.type = AST_ELEMENT_LIST;

        struct ast_element *ast_element = malloc(sizeof(struct ast_element));
        ast_element->base.type = AST_ELEMENT;
        ast_element->word = strdup("hello");
        ast_element->redirection = NULL;

        ast_element_list->element = (struct ast *)ast_element;
        ast_element_list->next = NULL;

        ast_simple_cmd->element_list = (struct ast *)ast_element_list;

        struct ast *ast = (struct ast *)ast_simple_cmd;

        // run to put the hello dir in pwd
        int res = execute_ast(ast);

        free(ast_element->word);

        cr_expect_eq(res, 0);

        // run to put the hello dir in oldpwd and the root dir to pwd
        ast_element->word = strdup("..");
        res = execute_ast(ast);

        // run to test the cd - to switch the pwd and oldpwd

        ast_element->word = strdup("-");
        res = execute_ast(ast);

        free(ast_element->word);
        free(ast_element);
        free(ast_element_list);
        free(ast_simple_cmd->word);
        free(ast_simple_cmd);

        char *new_path = getenv("PWD");
        char *new_old_path = getenv("OLDPWD");

        char *path = merge(strdup(old_path), strdup("/hello"));

        cr_expect_eq(res, 0);
        cr_expect_str_eq(new_path, path);
        cr_expect_str_eq(new_old_path, old_path);
        free(path);
    }
}

Test(Execution, test_cmd_builtin_cd_dot_dot)
{
    char *path = getenv("PWD");
    int pid = fork();
    if (!pid)
    {
        char *argv[] = { "env", "-i", NULL };
        execvp("env", argv);
    }
    else
    {
        int wstatus;
        waitpid(pid, &wstatus, 0);

        struct ast_simple_cmd *ast_simple_cmd =
            malloc(sizeof(struct ast_simple_cmd));

        ast_simple_cmd->base.type = AST_SIMPLE_CMD;
        ast_simple_cmd->prefix_list = NULL;
        // ast_simple_cmd->prefix = NULL;
        ast_simple_cmd->word = strdup("cd");

        struct ast_element_list *ast_element_list =
            malloc(sizeof(struct ast_element_list));

        ast_element_list->base.type = AST_ELEMENT_LIST;

        struct ast_element *ast_element = malloc(sizeof(struct ast_element));
        ast_element->base.type = AST_ELEMENT;
        ast_element->word = strdup("hello");
        ast_element->redirection = NULL;

        ast_element_list->element = (struct ast *)ast_element;
        ast_element_list->next = NULL;

        ast_simple_cmd->element_list = (struct ast *)ast_element_list;

        struct ast *ast = (struct ast *)ast_simple_cmd;

        // run to put the hello dir in pwd
        int res = execute_ast(ast);

        free(ast_element->word);

        cr_expect_eq(res, 0);

        // run to put the hello dir in oldpwd and the root dir to pwd
        ast_element->word = strdup("..");
        res = execute_ast(ast);

        // run to test the cd - to switch the pwd and oldpwd

        free(ast_element->word);
        free(ast_element);
        free(ast_element_list);
        free(ast_simple_cmd->word);
        free(ast_simple_cmd);

        char *new_path = getenv("PWD");
        char *new_old_path = getenv("OLDPWD");

        char *old_path = merge(strdup(path), strdup("/hello"));

        cr_expect_eq(res, 0);
        cr_expect_str_eq(new_path, path);
        cr_expect_str_eq(new_old_path, old_path);
        free(old_path);
    }
}
