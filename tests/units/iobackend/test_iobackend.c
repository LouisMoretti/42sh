#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>
#include <stddef.h>
#include <string.h>

#include "config/config.h"
#include "iobackend/iobackend.h"

TestSuite(Iobackend_setup);

Test(Iobackend_setup, setup_c)
{
    struct config conf = { STRING, "echo a" };
    int res = io_setup(&conf);

    cr_expect(res == 0);

    io_close();
}

Test(Iobackend_setup, setup_file)
{
    /*
    char *argv[1] = { "script.sh" };
    int s = set_conf(1, argv);
    int res = io_setup();
    struct config *conf = get_conf();
    */

    struct config conf = { FILEPATH, "script.sh" };
    int res = io_setup(&conf);

    cr_expect(res == 0);

    io_close();
}

Test(Iobackend_setup, setup_stdin)
{
    /*
    char **argv = NULL;
    int s = set_conf(0, argv);
    int res = io_setup();
    struct config *conf = get_conf();
    */

    struct config conf = { STDIN, NULL };
    int res = io_setup(&conf);

    cr_expect(res == 0);

    io_close();
}

Test(Iobackend_setup, test_peek)
{
    /*
    char *argv[2] = { "-c", "echo a" };
    set_conf(2, argv);
    io_setup();
    struct config *conf = get_conf();
    */

    struct config conf = { STRING, "echo a" };
    io_setup(&conf);

    int res = peek_chr();

    cr_expect(res == 'e');

    io_close();
}

Test(Iobackend_setup, test_pop)
{
    /*
    char *argv[2] = { "-c", "echo a" };
    set_conf(2, argv);
    io_setup();
    struct config *conf = get_conf();
    */

    struct config conf = { STRING, "echo a" };
    io_setup(&conf);

    pop_chr();
    int res = peek_chr();

    cr_expect(res == 'c');

    io_close();
}

Test(Iobackend_setup, test_get)
{
    /*
    char *argv[2] = { "-c", "echo a" };
    set_conf(2, argv);
    io_setup();
    struct config *conf = get_conf();
    */

    struct config conf = { STRING, "echo a" };
    io_setup(&conf);

    char *st = "echo a";
    size_t i = 0;

    while (st[i] != '\0')
    {
        int res = get_chr();
        cr_expect(res == st[i]);
        i++;
    }

    int res = get_chr();
    cr_expect(res == EOF);

    io_close();
}
