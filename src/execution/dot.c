#define _POSIX_C_SOURCE 200809L

#include <err.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtin.h"

int builtin_dot(struct ast_simple_cmd *command)
{
    struct ast_element_list *ast_element_list =
        (struct ast_element_list *)command->element_list;

    if (!ast_element_list)
    {
        warnx("builtin_dot: error, no filename after dot");
        return -1;
    }

    struct ast_element *ast_element =
        (struct ast_element *)ast_element_list->element;

    if (!ast_element || !ast_element->word)
    {
        warnx("builtin_dot: error, no element or no word in element list");
        return -1;
    }

    char *cmd = ast_element->word;
    char *args[] = { cmd, NULL };

    int res = 0;

    int pid = fork();

    if (pid == -1)
        return -1;
    else if (!pid)
    {
        execv(cmd, args);
    }
    else
    {
        int wstatus;
        waitpid(pid, &wstatus, 0);

        if (wstatus == -1)
            warnx("builtin_dot: error during execution, file not found or "
                  "could not execute");

        res = wstatus;
    }

    return res;
}
