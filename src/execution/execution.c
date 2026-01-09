#include "execution/execution.h"

#include <assert.h>
#include <stddef.h>

#include "execution/builtin.h"

#define BUILTIN_ECHO "echo"
#define BUILTIN_FALSE "false"
#define BUILTIN_TRUE "true"

typedef int (*fptr)(struct ast *);

static fptr execute_functions = { [AST_INPUT] = &execute_ast_input;
[AST_LIST] = &execute_ast_list;
[AST_AND_OR] = &execute_ast_and_or;
[AST_PIPELINE] = &execute_ast_pipeline;
[AST_CMD] = &execute_ast_cmd;
[AST_SIMPLE_CMD] = &execute_ast_simple_cmd;
}
;

static int evaluate_command(char **command)
{
    int pid = fork();
    if (pid == -1)
        return 1;
    if (!pid)
    {
        exit(execvp(command[0], command));
    }
    else
    {
        int wstatus;
        wait(&wstatus);

        return WEXITSTATUS(wstatus);
    }
}

int execute_ast(struct ast *ast)
{
    if (!ast)
        return 0;
    return (*fptr)[ast->type](ast);
}

static int execute_ast_input(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_INPUT);
    struct ast_input *ast_input = (struct ast_input *)ast;
    return execute_ast_list(ast_input->list);
}

static int execute_ast_list(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_LIST);
    struct ast_list *ast_list = (struct ast_list *)ast;
    assert(ast_list->and_or != NULL);
    if (!ast_list->next)
    {
        return execute_ast_and_or(ast_list->and_or);
    }
    else
    {
        execute_ast_and_or(ast_list->and_or);
        return execute_ast_list(ast_list->next);
    }
};

static int execute_ast_and_or(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_AND_OR);
    struct ast_and_or *ast_and_or = (struct ast_and_or *)ast;
    assert(ast_and_or->pipeline != NULL);

    return execute_ast_pipeline(ast_and_or->pipeline);
}

static int execute_ast_pipeline(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_PIPELINE);
    struct ast_pipeline *ast_pipeline = (struct ast_pipeline *)ast;
    assert(ast_pipeline->cmd != NULL);

    return execute_ast_cmd(ast_pipeline->cmd);
}

static int execute_ast_cmd(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_CMD);
    struct ast_cmd *ast_cmd = (struct ast_cmd *)ast;
    assert(ast_cmd->cmd != NULL);

    return execute_ast_simple_cmd(ast_cmd->cmd);
}

static int count_ast_element(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_ELEMENT_LIST);
    struct ast_element_list *ast_element_list = (struct ast_element_list *)ast;

    return 1 + count_ast_element(ast_element_list->next);
}

static int execute_ast_simple_cmd(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_SIMPLE_CMD);
    struct ast_simple_cmd *ast_simple_cmd = (struct ast_simple_cmd *)ast;
    assert(ast_simple_cmd->word != NULL);
    if (!strcmp(ast_simple_cmd->word, BUILTIN_ECHO))
        return builtin_echo(ast_simple_cmd);
    else if (!strcmp(ast_simple_cmd->word, BUILTIN_FALSE))
        return builtin_false(ast_simple_cmd);
    else if (!strcmp(ast_simple_cmd->word, BUILTIN_TRUE))
        return builtin_true(ast_simple_cmd);
    else
    {
        int size = count_ast_element(ast_simple_cmd->element_list) + 1;
        char **command = calloc(size + 1, sizeof(char *));
        if (!command)
            return 1;

        command[0] = ast_simple_cmd->word;

        int i = 1;
        struct ast_element_list *ast_element_list =
            ast_simple_cmd->element_list;
        while (i < size)
        {
            struct ast_element *ast_element = ast_element_list->element;
            command[i] = ast_element->word;
            ast_element_list = ast_element_list->next;

            i++;
        }

        int exit_code = evaluate_command(command);
        free(command);

        return exit_code;
    }
}
