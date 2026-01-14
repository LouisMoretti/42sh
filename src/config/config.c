#include "config.h"

#include <stddef.h>
#include <string.h>

static struct config my_conf;

int set_conf(int argc, char **argv)
{
    my_conf.str_stream = NULL; // Default stream is NULL
    my_conf.arguments = NULL; // Default stream is NULL
    my_conf.pretty_print = 0; // No pretty print by default
    if (argc == 1)
    {
        my_conf.method = STDIN; // Default method is stdin
        return 0;
    }
    int i = 1;
    int went_string = 0;
    while (i < argc && argv[i][0] == '-')
    {
        if (argv[i][1] == 'c')
        {
            if (i + 1 >= argc)
                return 2;

            my_conf.method = STRING;
            went_string = 1;
        }
        else if (argv[i][1] == '-')
        {
            if (strcmp(argv[i] + 2, "pretty_print") == 0
                || strcmp(argv[i] + 2, "pretty-print") == 0)
            {
                my_conf.pretty_print = 1;
            }
        }
        i++;
    }
    if (!went_string)
        my_conf.method = FILEPATH;
    my_conf.str_stream = argv[i];
    if (i + 1 < argc)
    {
        my_conf.arguments = argv + i + 1;
    }
    return 0;
}

struct config *get_conf(void)
{
    return &my_conf;
}
