#define _POSIX_C_SOURCE 200809L

#include "execution/execution.h"

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "config/config.h"
#include "execution/builtin.h"
#include "execution/pipe.h"
#include "execution/redirection.h"
#include "expansion/expansion.h"
#include "iobackend/iobackend.h"
#include "utils/hash_map/hash_map.h"

#define BUILTIN_ECHO "echo"
#define BUILTIN_FALSE "false"
#define BUILTIN_TRUE "true"
#define BUILTIN_CD "cd"
#define BUILTIN_EXIT "exit"
#define BUILTIN_DOT "."
#define COMMAND_NOT_FOUND_ERROR 127
#define DEFAULT_ERROR 1

typedef int (*exec)(struct ast *);

static int execute_ast_shell_cmd(struct ast *ast);

static int evaluate_command(char **command)
{
    int pid = fork();
    if (pid == -1)
        return 1;
    if (!pid)
    {
        execvp(command[0], command);
        switch (errno)
        {
        case ENOENT:
            warnx("evaluate_command: Command Not Found. Got: '%s'", command[0]);
            io_close();
            exit(COMMAND_NOT_FOUND_ERROR);
        default:
            warnx("evaluate_command: %s. Got: '%s'", strerror(errno),
                  command[0]);
            io_close();
            exit(DEFAULT_ERROR);
        }
    }
    else
    {
        int wstatus;
        waitpid(pid, &wstatus, 0);

        if (WIFEXITED(wstatus))
            return WEXITSTATUS(wstatus);
        else if (WIFSIGNALED(wstatus))
            return WTERMSIG(wstatus);
        else if (WIFSTOPPED(wstatus))
            return WSTOPSIG(wstatus);
        else
            return DEFAULT_ERROR;
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

static int assignement_var(char *assignment_word)
{
    char *saveptr;
    char *var_name = strtok_r(assignment_word, "=", &saveptr);
    if (!var_name)
        return 1;
    char *var_val = saveptr;
    if (var_val[0] == '\0')
        return 1;
    if (strcmp(var_name, "IFS") == 0)
    {
        setenv("IFS", var_val, 1);
        return 0;
    }
    struct hash_map *variables = get_hm();
    bool has_insert = false;
    bool code = hash_map_insert(variables, var_name, var_val, &has_insert);
    if (code == false)
        return 2;

    return 0;
}

static int execute_ast_prefix(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_PREFIX);
    struct ast_prefix *ast_prefix = (struct ast_prefix *)ast;
    assert(ast_prefix->assignment_word);

    return assignement_var(ast_prefix->assignment_word);
}

static int execute_ast_prefix_list(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_PREFIX_LIST);
    struct ast_prefix_list *ast_prefix = (struct ast_prefix_list *)ast;

    while (ast_prefix && !is_exit())
    {
        struct ast *cur_prefix = ast_prefix->prefix;
        int code = execute_ast_prefix(cur_prefix);
        if (code != 0)
            return 1;

        ast_prefix = (struct ast_prefix_list *)ast_prefix->next;
    }

    return 0;
}

static int check_which_cmd(struct ast_simple_cmd *ast_simple_cmd)
{
    if (!strcmp(ast_simple_cmd->word, BUILTIN_ECHO))
        return builtin_echo(ast_simple_cmd);
    else if (!strcmp(ast_simple_cmd->word, BUILTIN_FALSE))
        return builtin_false();
    else if (!strcmp(ast_simple_cmd->word, BUILTIN_TRUE))
        return builtin_true();
    else if (!strcmp(ast_simple_cmd->word, BUILTIN_CD))
        return builtin_cd(ast_simple_cmd);
    else if (!strcmp(ast_simple_cmd->word, BUILTIN_EXIT))
        return builtin_exit(ast_simple_cmd);
    else if (!strcmp(ast_simple_cmd->word, BUILTIN_DOT))
        return builtin_dot(ast_simple_cmd);
    else
    {
        int size = count_ast_element(ast_simple_cmd->element_list) + 1;
        char **command = (char **)calloc(size + 1, sizeof(char *));
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
        free((void *)command);

        return exit_code;
    }
}

static int handle_expand_list(struct ast_element_list *ast_element_list,
                              struct ast_simple_cmd *ast_simple_cmd)
{
    if (ast_element_list)
    {
        struct ast_element_list *expanded_list =
            (struct ast_element_list *)init_ast(AST_ELEMENT_LIST);

        ast_simple_cmd->element_list = (struct ast *)expanded_list;

        // first iteration for the copy
        struct ast_element *ast_element =
            (struct ast_element *)ast_element_list->element;
        if (ast_element->word)
        {
            char *expanded = expand_string(ast_element->word);
            if (!expanded)
                return 1;

            // keeps a copy of the expanded elements
            struct ast_element *act_elm =
                (struct ast_element *)init_ast(AST_ELEMENT);
            act_elm->word = expanded;
            expanded_list->element = (struct ast *)act_elm;
        }

        ast_element_list = (struct ast_element_list *)ast_element_list->next;

        while (ast_element_list)
        {
            struct ast_element *ast_element =
                (struct ast_element *)ast_element_list->element;

            struct ast_element_list *new_list_elm =
                (struct ast_element_list *)init_ast(AST_ELEMENT_LIST);

            char *expanded = expand_string(ast_element->word);
            if (!expanded)
                return 1;

            // keeps a copy of the expanded elements
            struct ast_element *act_elm =
                (struct ast_element *)init_ast(AST_ELEMENT);
            act_elm->word = expanded;
            new_list_elm->element = (struct ast *)act_elm;
            expanded_list->next = (struct ast *)new_list_elm;
            expanded_list = (struct ast_element_list *)expanded_list->next;

            ast_element_list =
                (struct ast_element_list *)ast_element_list->next;
        }
    }
    return 0;
}

static int execute_ast_simple_cmd(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_SIMPLE_CMD);
    struct ast_simple_cmd *ast_simple_cmd = (struct ast_simple_cmd *)ast;
    assert(ast_simple_cmd->word != NULL || ast_simple_cmd->prefix_list != NULL);

    if (!ast_simple_cmd->word)
        return execute_ast_prefix_list(ast_simple_cmd->prefix_list);

    char *expanded = expand_string(ast_simple_cmd->word);
    if (!expanded)
        return 1;
    free(ast_simple_cmd->word);
    ast_simple_cmd->word = expanded;

    struct ast_element_list *ast_element_list =
        (struct ast_element_list *)ast_simple_cmd->element_list;

    struct ast_element_list *good_list = ast_element_list;

    if (handle_expand_list(ast_element_list, ast_simple_cmd) != 0)
        return 1;

    int exit_code = check_which_cmd(ast_simple_cmd);

    // free the expanded element list and put back the first element list
    free_ast((struct ast *)ast_simple_cmd->element_list);
    ast_simple_cmd->element_list = (struct ast *)good_list;

    return exit_code;
}

static int execute_ast_cmd(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_CMD);
    struct ast_cmd *ast_cmd = (struct ast_cmd *)ast;
    assert(ast_cmd->cmd != NULL);
    int return_code;
    switch (ast_cmd->cmd->type)
    {
    case AST_SIMPLE_CMD:
        return_code = execute_ast_simple_cmd(ast_cmd->cmd);
        set_return_code(return_code);
        return return_code;
    case AST_SHELL_CMD:
        return execute_ast_shell_cmd(ast_cmd->cmd);
    case AST_REDIRECTION:
        return execute_ast_redirection(ast_cmd->cmd);
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

    int result = execute_pipe((struct ast *)ast_pipeline);

    if (is_exit())
        return result;

    if (ast_pipeline->negation)
        result = !result;

    set_return_code(result);

    return result;
}

static int execute_ast_and_or(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_AND_OR);
    struct ast_and_or *ast_and_or = (struct ast_and_or *)ast;
    assert(ast_and_or->pipeline != NULL);

    int result = 0;
    while (!result && ast_and_or && !is_exit())
    {
        result = execute_ast_pipeline(ast_and_or->pipeline);
        // Absorbing Elements
        if (result)
        {
            while (ast_and_or && ast_and_or->operand == AND)
                ast_and_or = (struct ast_and_or *)ast_and_or->next;
            if (ast_and_or)
                result = 0;
        }
        else if (!result)
        {
            while (ast_and_or && ast_and_or->operand == OR)
                ast_and_or = (struct ast_and_or *)ast_and_or->next;
            if (ast_and_or)
                result = 0;
        }

        if (ast_and_or)
            ast_and_or = (struct ast_and_or *)ast_and_or->next;
    }

    return result;
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
        int res = execute_ast_and_or(ast_compound_list->ast_and_or);

        if (is_exit())
            return res;

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
    if (!ast_else_clause->condition_compound_list)
        assert(ast_else_clause->else_clause == NULL);
    if (ast_else_clause->else_clause)
        assert(ast_else_clause->condition_compound_list != NULL);

    int condition_exit_code =
        execute_ast_compound_list(ast_else_clause->condition_compound_list);

    if (is_exit())
        return condition_exit_code;

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

    if (is_exit())
        return condition_exit_code;

    if (!condition_exit_code)
        return execute_ast_compound_list(ast_rule_if->body_compound_list);
    else
        return execute_ast_else_clause(ast_rule_if->else_clause);
}

static int execute_ast_while(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_RULE_WHILE);
    struct ast_rule_while *ast_rule_while = (struct ast_rule_while *)ast;
    assert(ast_rule_while->condition_compound_list != NULL);
    assert(ast_rule_while->body_compound_list != NULL);

    int result = 0;
    while (!execute_ast_compound_list(ast_rule_while->condition_compound_list)
           && !is_exit())
        result = execute_ast_compound_list(ast_rule_while->body_compound_list);

    return result;
}

static int execute_ast_until(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_RULE_UNTIL);
    struct ast_rule_until *ast_rule_until = (struct ast_rule_until *)ast;
    assert(ast_rule_until->condition_compound_list != NULL);
    assert(ast_rule_until->body_compound_list != NULL);

    int result = 0;
    while (execute_ast_compound_list(ast_rule_until->condition_compound_list)
           && !is_exit())
        result = execute_ast_compound_list(ast_rule_until->body_compound_list);

    return result;
}

static int execute_ast_for(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_RULE_FOR);
    struct ast_rule_for *ast_rule_for = (struct ast_rule_for *)ast;
    assert(ast_rule_for->condition_word != NULL);
    assert(ast_rule_for->body_compound_list != NULL);

    int result = 0;
    struct ast_word_list *ast_word_list =
        (struct ast_word_list *)ast_rule_for->in_word_list;

    struct ast_word_list *good_list = ast_word_list;

    int in = 0;
    if (ast_word_list)
    {
        in = 1;

        ast_rule_for->in_word_list = (struct ast *)expand_for(ast_word_list);
        ast_word_list = (struct ast_word_list *)ast_rule_for->in_word_list;
    }
    struct hash_map *hm = get_hm();

    while (ast_word_list && !is_exit())
    {
        assert(ast_word_list->word != NULL);
        // insert the variable in the hash map
        hash_map_insert(hm, ast_rule_for->condition_word, ast_word_list->word,
                        NULL);

        result = execute_ast_compound_list(ast_rule_for->body_compound_list);

        // remove the variable in the hash map
        hash_map_remove(hm, ast_rule_for->condition_word);
        ast_word_list = (struct ast_word_list *)ast_word_list->next;
    }

    if (in)
    {
        free_ast((struct ast *)ast_rule_for->in_word_list);
        ast_rule_for->in_word_list = (struct ast *)good_list;
    }

    return result;
}

static int execute_subshell(struct ast *ast)
{
    int pid = fork();

    if (pid == -1)
        return 1;
    else if (!pid)
    {
        int res = execute_ast_compound_list(ast);

        if (res != 0)
            warnx("execute_subshell: an error occured in the subshell");

        _exit(res);
    }
    else
    {
        int wstatus;
        waitpid(pid, &wstatus, 0);

        if (wstatus == 1)
        {
            warnx("execute_subshell: error during execution, file not found or "
                  "could not execute");
        }
    }

    return 0;
}

static int execute_ast_shell_cmd(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_SHELL_CMD);
    struct ast_shell_cmd *ast_shell_cmd = (struct ast_shell_cmd *)ast;

    if (ast_shell_cmd->cmd_type == SUBSHELL)
    {
        return execute_subshell(ast_shell_cmd->compound_list);
    }
    else if (ast_shell_cmd->cmd_type == COMMAND_BLOCK)
    {
        return execute_ast_compound_list(ast_shell_cmd->compound_list);
    }
    else
    {
        switch (ast_shell_cmd->rule->type)
        {
        case AST_RULE_IF:
            return execute_ast_rule_if(ast_shell_cmd->rule);
        case AST_RULE_WHILE:
            return execute_ast_while(ast_shell_cmd->rule);
        case AST_RULE_UNTIL:
            return execute_ast_until(ast_shell_cmd->rule);
        case AST_RULE_FOR:
            return execute_ast_for(ast_shell_cmd->rule);
        default:
            return 0;
        }
    }
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
        int res = execute_ast_and_or(ast_list->and_or);

        if (is_exit())
            return res;

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

static exec execute_functions[] = {
    [AST_INPUT] = &execute_ast_input,
    [AST_LIST] = &execute_ast_list,
    [AST_AND_OR] = &execute_ast_and_or,
    [AST_PIPELINE] = &execute_ast_pipeline,
    [AST_CMD] = &execute_ast_cmd,
    [AST_SIMPLE_CMD] = &execute_ast_simple_cmd,
    [AST_SHELL_CMD] = &execute_ast_shell_cmd,
    [AST_COMPOUND_LIST] = &execute_ast_compound_list,
    [AST_RULE_IF] = &execute_ast_rule_if,
    [AST_CLAUSE_ELSE] = &execute_ast_else_clause,
    [AST_RULE_WHILE] = &execute_ast_while,
    [AST_RULE_UNTIL] = &execute_ast_until,
    [AST_RULE_FOR] = &execute_ast_for,
    [AST_REDIRECTION] = &execute_ast_redirection
};

int execute_ast(struct ast *ast)
{
    if (!ast)
        return 0;

    return (*execute_functions[ast->type])(ast);
}
