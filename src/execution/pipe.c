#include "execution/pipe.h"

#include <assert.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "execution/execution.h"

enum pipe_type
{
    LEFT = 1,
    RIGHT = 0
};

static pid_t execute_pipe_command(struct ast *ast, enum pipe_type type,
                                  int fds[2])
{
    if (!ast)
        return 0;

    assert(ast->type == AST_PIPELINE);

    pid_t pid = fork();
    // Parent Process gives child pid
    if (pid)
        return pid;

    int fd_to_replace = LEFT == type ? STDOUT_FILENO : STDIN_FILENO;

    if (dup2(fds[type], fd_to_replace) == -1)
        exit(1);

    close(fds[0]);
    close(fds[1]);

    struct ast_pipeline *ast_pipeline = (struct ast_pipeline *)ast;
    int result;
    // Execute current command
    if (type == LEFT)
        result = execute_ast(ast_pipeline->cmd);
    // Pipe rest of the commands
    else
        result = execute_pipe((struct ast *)ast_pipeline->next);

    exit(result);
}

int execute_pipe(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_PIPELINE);
    struct ast_pipeline *ast_pipeline = (struct ast_pipeline *)ast;

    int fds[2];
    if (pipe(fds) == -1)
        return 1;

    int wstatus;
    if (!ast_pipeline->next)
    {
        return execute_ast(ast_pipeline->cmd);
        /*pid_t left_pid =
            execute_pipe_command((struct ast *)ast_pipeline, LEFT, fds);

        close(fds[0]);
        close(fds[1]);
        if (left_pid == -1)
            return 1;

        waitpid(left_pid, &wstatus, 0);*/
    }
    else
    {
        pid_t left_pid =
            execute_pipe_command((struct ast *)ast_pipeline, LEFT, fds);
        pid_t right_pid =
            execute_pipe_command((struct ast *)ast_pipeline, RIGHT, fds);

        close(fds[0]);
        close(fds[1]);

        if (left_pid == -1 || right_pid == -1)
            return 1;

        waitpid(left_pid, &wstatus, 0);
        waitpid(right_pid, &wstatus, 0);
    }

    return WEXITSTATUS(wstatus);
}
