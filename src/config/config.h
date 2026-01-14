#ifndef CONFIG_H
#define CONFIG_H

enum input_method
{
    STRING,
    STDIN,
    FILEPATH
};

struct config
{
    enum input_method method;
    // Always initialize all parameters to 0 or NULL
    char *str_stream;
    int pretty_print;
    int arg_count;
    char **arg_values;
};

int set_conf(int argc, char **argv);
struct config *get_conf(void);

#endif // ! CONFIG_H
