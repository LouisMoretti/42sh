#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>
#include <string.h>

#include "../../../src/config/config.h"
#include "../../../src/iobackend/iobackend.h"

TestSuite(Iobackend_setup);

Test(Iobackend_setup, setup_c)
{
    char *argv[2] = { "-c", "echo a" };
    int s = set_conf(2, argv);
    int res = io_setup();
    struct config *conf = get_conf();

    cr_expect(s == 0);
    cr_expect(res == 0);
    cr_expect(conf->method == STRING);
    cr_expect(strcmp(conf->str_stream, "echo a"));

    io_close();
}

Test(Iobackend_setup, setup_file)
{
    char *argv[1] = { "script.sh" };
    int s = set_conf(1, argv);
    int res = io_setup();
    struct config *conf = get_conf();

    cr_expect(s == 0);
    cr_expect(res == 0);
    cr_expect(conf->method == FILEPATH);
    cr_expect(strcmp(conf->str_stream, "script.sh"));

    io_close();
}

Test(Iobackend_setup, setup_stdin)
{
    char **argv = NULL;
    int s = set_conf(0, argv);
    int res = io_setup();
    struct config *conf = get_conf();

    cr_expect(s == 0);
    cr_expect(res == 0);
    cr_expect(conf->method == STDIN);
    cr_expect(conf->str_stream == NULL);

    io_close();
}

Test(Iobackend_setup, test_peek)
{
    char *argv[2] = { "-c", "echo a" };
    set_conf(2, argv);
    io_setup();
    struct config *conf = get_conf();

    int res = peek_chr();

    cr_expect(conf->method == STRING);
    cr_expect(strcmp(conf->str_stream, "echo a"));
    cr_expect(res == 'e');

    io_close();
}

Test(Iobackend_setup, test_pop)
{
    char *argv[2] = { "-c", "echo a" };
    set_conf(2, argv);
    io_setup();
    struct config *conf = get_conf();

    pop_chr();
    int res = peek_chr();

    cr_expect(conf->method == STRING);
    cr_expect(strcmp(conf->str_stream, "echo a"));
    cr_expect(res == 'c');

    io_close();
}

Test(Iobackend_setup, test_get)
{
    char *argv[2] = { "-c", "echo a" };
    set_conf(2, argv);
    io_setup();
    struct config *conf = get_conf();

    char *st = "echo a";
    size_t i = 0;

    while (st[i] != '\0')
    {
        int res = get_chr();

        cr_expect(conf->method == STRING);
        cr_expect(strcmp(conf->str_stream, st + i));
        cr_expect(res == st[i]);
        i++;
    }

    int res = get_chr();

    cr_expect(res == EOF);

    io_close();
}
