#include "config.h"
static struct config my_conf;

int set_conf(int argc, char **argv)
{
    my_conf.method = STDIN; // Default method is stdin
    my_conf.str_stream = NULL; // Default stream is NULL
    int i = 1;
    while (i < argc)
    {
        if (argv[i][0] == '-')
        {
            if (argv[i][1] == 'c')
            {
                if (i + 1 >= argc)
                    return 2;

                my_conf.method = STRING;
                my_conf.str_stream = argv[i + 1];
                i++; // Skip string argument.
            }
            // Verbose - Pretty Print
        }
        else
        {
            my_conf.method = FILEPATH;
            my_conf.str_stream = argv[i];
        }
        i++;
    }
    return 0;
}

struct config *get_conf(void)
{
    return &my_conf;
}
