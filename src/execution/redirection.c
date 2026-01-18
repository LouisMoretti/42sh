#include "execution/redirection.h"

#include <unistd.h>

static int execute_ast_redirection_right(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_REDIRECTION);
    struct ast_redirection *ast_redirection = (struct ast_redirection *)ast;
    int fd = open(ast_redirection->right, O_CREAT | O_TRUNC | O_WRONLY, 0755);
    if (fd == -1)
        return 1;

    int file_fd_backup = dup(ast_redirection->fd);
    if (file_fd_backup == -1)
    {
        close(fd);
        return 1;
    }

    if (dup2(fd, STDOUT_FILENO) == -1)
    {
        close(fd);
        close(file_fd_backup);

        return 1;
    }
    
    // TODO: Generate **args for evaluate command 
    int result = evaluate_command();

    fflush(stdout);
    
    if (dup2(file_fd_backup, STDOUT_FILENO) == -1)
    {
        close(fd);
        close(file_fd_backup);

        return 1;
    }

    close(file_fd_backup);

    return 0;
}

static int execute_ast_redirection_right_append(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_REDIRECTION);
    struct ast_redirection *ast_redirection = (struct ast_redirection *)ast;
    int fd = open(ast_redirection->right, O_APPEND | O_WRONLY, 0755);
    if (fd == -1)
        return 1;

    int file_fd_backup = dup(ast_redirection->fd);
    if (file_fd_backup == -1)
    {
        close(fd);
        return 1;
    }

    if (dup2(fd, STDOUT_FILENO) == -1)
    {
        close(fd);
        close(file_fd_backup);

        return 1;
    }
    
    // TODO: Generate **args for evaluate command 
    int result = evaluate_command();

    fflush(stdout);
    
    if (dup2(file_fd_backup, STDOUT_FILENO) == -1)
    {
        close(fd);
        close(file_fd_backup);

        return 1;
    }

    close(file_fd_backup);

    return 0;
}

static int execute_ast_redirection_left(struct ast *ast)
{
    if (!ast)
        return 0;

}

static int execute_ast_redirection_left_append(struct ast *ast)
{
    if (!ast)
        return 0;

}

int execute_ast_redirection(struct ast *ast)
{
    if (!ast)
        return 0;

    struct ast_redirection *ast_redirection = (struct ast_redirection *)ast;
    switch (ast_redirection->type)
    {
        default:
            return 0;
    }
}
