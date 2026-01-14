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
    char *str_stream;
    int pretty_print;
    char **arguments;
};

int set_conf(int argc, char **argv);
struct config *get_conf(void);

#endif // ! CONFIG_H
