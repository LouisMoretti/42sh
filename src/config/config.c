#include "config.h"

#include <stddef.h>
#include <string.h>

static struct config my_conf;

int set_conf(int argc, char **argv)
{
    my_conf.hash_map_variables = hash_map_init(8);
    // IO part
    my_conf.method = STDIN; // Default method is stdin
    my_conf.str_stream = NULL; // Default

    // Flags part
    my_conf.pretty_print = 0; // No pretty print by default

    // Arguments part
    my_conf.arg_count = 0; // Default
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
    }

    return 0;
}

struct config *get_conf(void)
{
    return &my_conf;
}
