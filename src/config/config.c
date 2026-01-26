#define _POSIX_C_SOURCE 200112L

#include "config.h"

#include <err.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static struct config my_conf;

void set_env_variables(void)
{
    if (getenv("PWD") == NULL)
    {
        char *pwd = getcwd(NULL, 0);
        if (!pwd)
        {
            warnx("set_env_variables get current working directory failed");
            return;
        }
        setenv("PWD", pwd, 1);
        free(pwd);
    }

    setenv("IFS", " \t\n", 1);
}

int set_conf(int argc, char **argv)
{
    // Random Setup Part
    srand(time(NULL));

    // IO part
    my_conf.method = STDIN; // Default method is stdin
    my_conf.str_stream = NULL; // Default

    // Flags part
    my_conf.pretty_print = 0; // No pretty print by default

    // Arguments part
    my_conf.arg_count = 0; // Default
    my_conf.previous_code = 0; // Default
    my_conf.arg_values = NULL; // Default

    if (argc == 1)
        return 0;

    int i = 1;
    while (i < argc && argv[i][0] == '-')
    {
        if (argv[i][1] == 'c')
        {
            // TODO: Return error.
            if (i + 1 >= argc)
                return 2;

            my_conf.method = STRING;
            my_conf.str_stream = argv[i + 1];
            i += 2;
            break;
        }
        else if (argv[i][1] == '-')
        {
            if (strcmp(argv[i] + 2, "pretty-print") == 0)
            {
                my_conf.pretty_print = 1;
            }
        }
        i++;
    }

    if (argc > i && my_conf.method != STRING)
    {
        my_conf.method = FILEPATH;
        my_conf.str_stream = argv[i];
        i++;
    }

    if (argc > i)
    {
        my_conf.arg_count = argc - i;
        my_conf.arg_values = argv + i;
        // Special case for filepath variable are 1 indexed
        if (my_conf.method == FILEPATH)
        {
            my_conf.arg_values--;
            my_conf.arg_count++;
        }
    }
    else if (my_conf.method == STRING)
    {
        // Special case for $0 in string without variable
        my_conf.arg_values = argv;
        my_conf.arg_count = 1;
    }

    return 0;
}

struct config *get_conf(void)
{
    return &my_conf;
}
