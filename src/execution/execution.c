#include "execution/execution.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "execution/builtin.h"
#include "expansion/expansion.h"

#define BUILTIN_ECHO "echo"
#define BUILTIN_FALSE "false"
#define BUILTIN_TRUE "true"

typedef int (*exec)(struct ast *);

static int execute_ast_shell_cmd(struct ast *ast);

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

    char *expanded = expand_string(ast_simple_cmd->word);
    if (expanded)
    {
        free(ast_simple_cmd->word);
        ast_simple_cmd->word = expanded;
    }

    struct ast_element_list *ast_element_list =
        (struct ast_element_list *)ast_simple_cmd->element_list;

    while (ast_element_list)
    {
        struct ast_element *ast_element =
            (struct ast_element *)ast_element_list->element;
        if (ast_element->word)
        {
            char *expanded = expand_string(ast_element->word);
            if (!expanded)
                return 1;

            free(ast_element->word);
            ast_element->word = expanded;
        }

        ast_element_list = (struct ast_element_list *)ast_element_list->next;
    }

    if (!strcmp(ast_simple_cmd->word, BUILTIN_ECHO))
        return builtin_echo(ast_simple_cmd);
    else if (!strcmp(ast_simple_cmd->word, BUILTIN_FALSE))
        return builtin_false();
    else if (!strcmp(ast_simple_cmd->word, BUILTIN_TRUE))
        return builtin_true();
    else
    {
        int size = count_ast_element(ast_simple_cmd->element_list) + 1;
        char **command = calloc(size + 1, sizeof(char *));
        if (!command)
            return 1;

        command[0] = ast_simple_cmd->word;

        int i = 1;
        struct ast_element_list *ast_element_list =
            (struct ast_element_list *)ast_simple_cmd->element_list;
        while (i < size)
        {
            struct ast_element *ast_element =
                (struct ast_element *)ast_element_list->element;
            command[i] = ast_element->word;
            ast_element_list =
                (struct ast_element_list *)ast_element_list->next;

            i++;
        }

        int exit_code = evaluate_command(command);
        free(command);

        return exit_code;
    }
}

static int execute_ast_cmd(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_CMD);
    struct ast_cmd *ast_cmd = (struct ast_cmd *)ast;
    assert(ast_cmd->cmd != NULL);
    switch (ast_cmd->cmd->type)
    {
    case AST_SIMPLE_CMD:
        return execute_ast_simple_cmd(ast_cmd->cmd);
    case AST_SHELL_CMD:
        return execute_ast_shell_cmd(ast_cmd->cmd);
    default: // May not fall through
        return 1;
    }
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

static int execute_ast_and_or(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_AND_OR);
    struct ast_and_or *ast_and_or = (struct ast_and_or *)ast;
    assert(ast_and_or->pipeline != NULL);

    return execute_ast_pipeline(ast_and_or->pipeline);
}

static int execute_ast_compound_list(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_COMPOUND_LIST);
    struct ast_compound_list *ast_compound_list =
        (struct ast_compound_list *)ast;
    assert(ast_compound_list->ast_and_or != NULL);
    if (!ast_compound_list->next)
    {
        return execute_ast_and_or(ast_compound_list->ast_and_or);
    }
    else
    {
        execute_ast_and_or(ast_compound_list->ast_and_or);

        return execute_ast_compound_list(ast_compound_list->next);
    }
}

static int execute_ast_else_clause(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_CLAUSE_ELSE);
    struct ast_else_clause *ast_else_clause = (struct ast_else_clause *)ast;
    assert(ast_else_clause->body_compound_list != NULL);
    if (!ast_else_clause->else_clause)
        assert(ast_else_clause->condition_compound_list != NULL);

    int condition_exit_code =
        execute_ast_compound_list(ast_else_clause->condition_compound_list);
    if (!condition_exit_code)
        return execute_ast_compound_list(ast_else_clause->body_compound_list);
    else
        return execute_ast_else_clause(ast_else_clause->else_clause);
}

static int execute_ast_rule_if(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_RULE_IF);
    struct ast_rule_if *ast_rule_if = (struct ast_rule_if *)ast;
    assert(ast_rule_if->condition_compound_list != NULL);
    assert(ast_rule_if->body_compound_list != NULL);

    int condition_exit_code =
        execute_ast_compound_list(ast_rule_if->condition_compound_list);
    if (!condition_exit_code)
        return execute_ast_compound_list(ast_rule_if->body_compound_list);
    else
        return execute_ast_else_clause(ast_rule_if->else_clause);
}

static int execute_ast_shell_cmd(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_SHELL_CMD);
    struct ast_shell_cmd *ast_shell_cmd = (struct ast_shell_cmd *)ast;
    assert(ast_shell_cmd->rule != NULL);

    return execute_ast_rule_if(ast_shell_cmd->rule);
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
}

static int execute_ast_input(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_INPUT);
    struct ast_input *ast_input = (struct ast_input *)ast;

    return execute_ast_list(ast_input->list);
}

static exec execute_functions[] = { [AST_INPUT] = &execute_ast_input,
                                    [AST_LIST] = &execute_ast_list,
                                    [AST_AND_OR] = &execute_ast_and_or,
                                    [AST_PIPELINE] = &execute_ast_pipeline,
                                    [AST_CMD] = &execute_ast_cmd,
                                    [AST_SIMPLE_CMD] = &execute_ast_simple_cmd,
                                    [AST_SHELL_CMD] = &execute_ast_shell_cmd,
                                    [AST_COMPOUND_LIST] =
                                        &execute_ast_compound_list,
                                    [AST_RULE_IF] = &execute_ast_rule_if,
                                    [AST_CLAUSE_ELSE] =
                                        &execute_ast_else_clause };

int execute_ast(struct ast *ast)
{
    if (!ast)
        return 0;

    return (*execute_functions[ast->type])(ast);
}
