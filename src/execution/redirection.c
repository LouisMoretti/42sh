#include "execution/redirection.h"

#include <assert.h>
#include <err.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "execution/execution.h"

#define PERMISSION_FLAGS 0755
#define NO_FD (-1)

static void close_file_descriptors(int fd1, int fd2)
{
    if (fd1 != NO_FD)
        close(fd1);
    if (fd2 != NO_FD)
        close(fd2);
}

static int get_file_descriptor(char *word, int flags)
{
    if (word[0] == '0' && !word[1])
        return 0;

    int fd = atoi(word);
    if (fd)
        return fd;

    fd = open(word, flags, PERMISSION_FLAGS);

    return fd;
}

static int execute_ast_redirection_out(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_REDIRECTION);
    struct ast_redirection *ast_redirection = (struct ast_redirection *)ast;
    assert(ast_redirection->word != NULL);

    // Default Value
    if (ast_redirection->io_number == -1)
        ast_redirection->io_number = 1;

    // Get the file descriptor corresponding to 'word'
    int fd = get_file_descriptor(ast_redirection->word,
                                 O_CREAT | O_TRUNC | O_WRONLY);
    if (fd == -1)
        return 1;

    // If clobber is not set, this redirection should raise an error
    if (!access(ast_redirection->word, F_OK) && !getenv("clobber"))
    {
        warnx("redirection_out: file exists (noclobber)");

        return 1;
    }

    // Create a copy of 'word' file descriptor
    int fd_backup = dup(ast_redirection->io_number);
    if (fd_backup == -1)
    {
        close_file_descriptors(fd, NO_FD);

        return 1;
    }

    // Redirect 'io_number' fd to 'word' file descriptor
    if (dup2(fd, ast_redirection->io_number) == -1)
    {
        close_file_descriptors(fd, fd_backup);

        return 1;
    }

    // Execute the next AST, whether it's a redirection or a command
    int result = execute_ast(ast_redirection->next);

    // Restore the file descriptor
    if (dup2(fd_backup, ast_redirection->io_number) == -1)
    {
        close_file_descriptors(fd, fd_backup);

        return 1;
    }

    close_file_descriptors(fd, fd_backup);

    return result;
}

static int execute_ast_redirection_in(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_REDIRECTION);
    struct ast_redirection *ast_redirection = (struct ast_redirection *)ast;
    assert(ast_redirection->word);

    // Default Value
    if (ast_redirection->io_number == -1)
        ast_redirection->io_number = 0;

    // Get the file descriptor corresponding to 'word'
    int fd = get_file_descriptor(ast_redirection->word, O_RDONLY);
    if (fd == -1)
        return 1;

    // Create a copy of 'word' file descriptor
    int fd_backup = dup(fd);
    if (fd_backup == -1)
    {
        close_file_descriptors(fd, NO_FD);

        return 1;
    }

    // Redirect 'word' fd to 'io_number' file descriptor
    if (dup2(ast_redirection->io_number, fd) == -1)
    {
        close_file_descriptors(fd, fd_backup);

        return 1;
    }

    // Execute the next AST, whether it's a redirection or a command
    int result = execute_ast(ast_redirection->next);

    // Restore the file descriptor
    if (dup2(fd_backup, fd) == -1)
    {
        close_file_descriptors(fd, fd_backup);

        return 1;
    }

    close_file_descriptors(fd, fd_backup);

    return result;
}

static int execute_ast_redirection_out_append(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_REDIRECTION);
    struct ast_redirection *ast_redirection = (struct ast_redirection *)ast;
    assert(ast_redirection->word);

    // Default Value
    if (ast_redirection->io_number == -1)
        ast_redirection->io_number = 1;

    // Get the file descriptor corresponding to 'word'
    int fd = get_file_descriptor(ast_redirection->word,
                                 O_CREAT | O_APPEND | O_WRONLY);
    if (fd == -1)
        return 1;

    // Create a copy of 'word' file descriptor
    int fd_backup = dup(ast_redirection->io_number);
    if (fd_backup == -1)
    {
        close_file_descriptors(fd, NO_FD);

        return 1;
    }

    // Redirect 'io_number' fd to 'word' file descriptor
    if (dup2(fd, ast_redirection->io_number) == -1)
    {
        close_file_descriptors(fd, fd_backup);

        return 1;
    }

    // Execute the next AST, whether it's a redirection or a command
    int result = execute_ast(ast_redirection->next);

    // Restore the file descriptor
    if (dup2(fd_backup, ast_redirection->io_number) == -1)
    {
        close_file_descriptors(fd, fd_backup);

        return 1;
    }

    close_file_descriptors(fd, fd_backup);

    return result;
}

static int execute_ast_redirection_out_dup(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_REDIRECTION);
    struct ast_redirection *ast_redirection = (struct ast_redirection *)ast;
    assert(ast_redirection->word != NULL);

    // Default Value
    if (ast_redirection->io_number == -1)
        ast_redirection->io_number = 1;

    // Close fd
    if (ast_redirection->word[0] == '-' && !ast_redirection->word[1])
    {
        close(ast_redirection->io_number);

        return 0;
    }

    // Get the file descriptor corresponding to 'word'
    int fd = get_file_descriptor(ast_redirection->word, O_WRONLY);
    if (fd == -1)
        return 1;

    // TODO: Check if the file descriptor is not open => error (fstat)
    // And check if word is writable

    // Create a copy of 'word' file descriptor
    int fd_backup = dup(ast_redirection->io_number);
    if (fd_backup == -1)
    {
        close_file_descriptors(fd, NO_FD);

        return 1;
    }
    //
    // Redirect 'io_number' fd to 'word' file descriptor
    if (dup2(fd, ast_redirection->io_number) == -1)
    {
        close_file_descriptors(fd, fd_backup);

        return 1;
    }

    // Execute the next AST, whether it's a redirection or a command
    int result = execute_ast(ast_redirection->next);

    // Restore the file descriptor
    if (dup2(fd_backup, ast_redirection->io_number) == -1)
    {
        close_file_descriptors(fd, fd_backup);

        return 1;
    }

    close_file_descriptors(fd, fd_backup);

    return result;
}

static int execute_ast_redirection_in_dup(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_REDIRECTION);
    struct ast_redirection *ast_redirection = (struct ast_redirection *)ast;
    assert(ast_redirection->word);

    // Default Value
    if (ast_redirection->io_number == -1)
        ast_redirection->io_number = 0;

    // Close fd
    if (ast_redirection->word[0] == '-' && !ast_redirection->word[1])
    {
        close(ast_redirection->io_number);

        return 0;
    }

    // Get the file descriptor corresponding to 'word'
    int fd = get_file_descriptor(ast_redirection->word, O_RDONLY);
    if (fd == -1)
        return 1;

    // TODO: Check if the file descriptor is not open => error (fstat)
    // And check if word is writable

    // Create a copy of 'word' file descriptor
    int fd_backup = dup(fd);
    if (fd_backup == -1)
    {
        close_file_descriptors(fd, NO_FD);

        return 1;
    }

    // Redirect 'word' fd to 'io_number' file descriptor
    if (dup2(ast_redirection->io_number, fd) == -1)
    {
        close_file_descriptors(fd, fd_backup);

        return 1;
    }

    // Execute the next AST, whether it's a redirection or a command
    int result = execute_ast(ast_redirection->next);

    // Restore the file descriptor
    if (dup2(fd_backup, fd) == -1)
    {
        close_file_descriptors(fd, fd_backup);

        return 1;
    }

    close_file_descriptors(fd, fd_backup);

    return result;
}

static int execute_ast_redirection_out_force(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_REDIRECTION);
    struct ast_redirection *ast_redirection = (struct ast_redirection *)ast;
    assert(ast_redirection->word != NULL);

    // Default Value
    if (ast_redirection->io_number == -1)
        ast_redirection->io_number = 1;

    // Get the file descriptor corresponding to 'word'
    int fd = get_file_descriptor(ast_redirection->word,
                                 O_CREAT | O_TRUNC | O_WRONLY);
    if (fd == -1)
        return 1;

    // Create a copy of 'word' file descriptor
    int fd_backup = dup(ast_redirection->io_number);
    if (fd_backup == -1)
    {
        close_file_descriptors(fd, NO_FD);

        return 1;
    }

    // Redirect 'io_number' fd to 'word' file descriptor
    if (dup2(fd, ast_redirection->io_number) == -1)
    {
        close_file_descriptors(fd, fd_backup);

        return 1;
    }

    // Execute the next AST, whether it's a redirection or a command
    int result = execute_ast(ast_redirection->next);

    // Restore the file descriptor
    if (dup2(fd_backup, ast_redirection->io_number) == -1)
    {
        close_file_descriptors(fd, fd_backup);

        return 1;
    }

    close_file_descriptors(fd, fd_backup);

    return result;
}

static int execute_ast_redirection_in_out(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_REDIRECTION);
    struct ast_redirection *ast_redirection = (struct ast_redirection *)ast;
    assert(ast_redirection->word);

    // Default Value
    if (ast_redirection->io_number == -1)
        ast_redirection->io_number = 0;

    // Get the file descriptor corresponding to 'word'
    int fd = get_file_descriptor(ast_redirection->word, O_CREAT | O_RDWR);
    if (fd == -1)
        return 1;

    // Create a copy of 'word' file descriptor
    int fd_backup = dup(fd);
    if (fd_backup == -1)
    {
        close_file_descriptors(fd, NO_FD);

        return 1;
    }

    // Redirect 'word' fd to 'io_number' file descriptor
    if (dup2(ast_redirection->io_number, fd) == -1)
    {
        close_file_descriptors(fd, fd_backup);

        return 1;
    }

    // Execute the next AST, whether it's a redirection or a command
    int result = execute_ast(ast_redirection->next);

    // Restore the file descriptor
    if (dup2(fd_backup, fd) == -1)
    {
        close_file_descriptors(fd, fd_backup);

        return 1;
    }

    close_file_descriptors(fd, fd_backup);

    return result;
}

int execute_ast_redirection(struct ast *ast)
{
    if (!ast)
        return 0;

    assert(ast->type == AST_REDIRECTION);
    struct ast_redirection *ast_redirection = (struct ast_redirection *)ast;
    switch (ast_redirection->type)
    {
    case REDIRECT_OUT:
        return execute_ast_redirection_out((struct ast *)ast_redirection);
    case REDIRECT_IN:
        return execute_ast_redirection_in((struct ast *)ast_redirection);
    case REDIRECT_OUT_APPEND:
        return execute_ast_redirection_out_append((struct ast *)ast_redirection);
    case REDIRECT_OUT_DUP:
        return execute_ast_redirection_out_dup((struct ast *)ast_redirection);
    case REDIRECT_IN_DUP:
        return execute_ast_redirection_in_dup((struct ast *)ast_redirection);
    case REDIRECT_OUT_FORCE:
        return execute_ast_redirection_out_force((struct ast *)ast_redirection);
    case REDIRECT_IN_OUT:
        return execute_ast_redirection_in_out((struct ast *)ast_redirection);
    default:
        return 0;
    }
}
