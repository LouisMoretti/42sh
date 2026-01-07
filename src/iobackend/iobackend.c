#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>

#include "config/config.h"

static FILE *g_stream;
static int g_stream_ownership;
static int g_has_peek = 0;
static int g_peek;

int peek_chr(void)
{
    if (!g_stream)
        return EOF;
    if (!g_has_peek)
    {
        int c = fgetc(g_stream);
        if (c == EOF)
            return EOF;
        g_peek = c;
        g_has_peek = 1;
    }
    return g_peek;
}

void pop_chr(void)
{
    if (g_has_peek == 0)
        fgetc(g_stream);
    g_has_peek = 0;
}

int get_chr(void)
{
    peek_chr();
    g_has_peek = 0;
    return g_peek;
}

static int io_setup_file(char *filepath)
{
    g_stream = fopen(filepath, "r");
    if (!g_stream)
        return 2;
    g_stream_ownership = 1;
    return 0;
}

int io_setup_string(char *string) // Not static in order for testing
{
    int size = strlen(string);
    g_stream = fmemopen(string, size, "r");
    if (!g_stream)
        return 2;
    g_stream_ownership = 1;
    return 0;
}
static int io_setup_stdin()
{
    g_stream = stdin;
    g_stream_ownership = 0;
    return 0;
}

int io_setup(void)
{
    struct config *my_conf = get_conf();
    switch (my_conf->method)
    {
    case STRING:
        return io_setup_string(my_conf->str_stream);
    case STDIN:
        return io_setup_stdin();
    case FILEPATH:
    default:
        return io_setup_file(my_conf->str_stream);
    }
}

void io_close(void)
{
    if (g_stream && g_stream_ownership)
    {
        fclose(g_stream);
    }
}
