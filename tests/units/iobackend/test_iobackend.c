#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>
#include <stddef.h>
#include <string.h>

#include "config/config.h"
#include "iobackend/iobackend.h"

TestSuite(Iobackend, .timeout = 1);

Test(Iobackend, test_setup_string)
{
    struct config conf = { 0 };
    conf.method = STRING;
    conf.str_stream = "echo a";

    int res = io_setup(&conf);

    cr_expect(res == 0);

    io_close();
}

Test(Iobackend, test_setup_file)
{
    struct config conf = { 0 };
    conf.method = FILEPATH;
    conf.str_stream = "script.sh";

    int res = io_setup(&conf);

    cr_expect(res == 0);

    io_close();
}

Test(Iobackend, test_setup_file_not_existant)
{
    struct config conf = { 0 };
    conf.method = FILEPATH;
    conf.str_stream = "not_existant.sh";

    int res = io_setup(&conf);

    cr_expect(res == 2);

    io_close();
}

Test(Iobackend, test_setup_stdin)
{
    struct config conf = { 0 };
    conf.method = STDIN;
    conf.str_stream = NULL;

    int res = io_setup(&conf);

    cr_expect(res == 0);

    io_close();
}

Test(Iobackend, test_string_peek)
{
    struct config conf = { 0 };
    conf.method = STRING;
    conf.str_stream = "echo a";

    io_setup(&conf);

    int res = peek_chr();

    cr_expect(res == 'e');

    io_close();
}

Test(Iobackend, test_string_pop_peek)
{
    struct config conf = { 0 };
    conf.method = STRING;
    conf.str_stream = "echo a";

    io_setup(&conf);

    // Skip one char
    pop_chr();

    // Check second char
    int res = peek_chr();

    cr_expect(res == 'c');

    io_close();
}

Test(Iobackend, test_string_peek_pop_peek)
{
    struct config conf = { 0 };
    conf.method = STRING;
    conf.str_stream = "echo a";

    io_setup(&conf);

    // Skip one char
    peek_chr();
    pop_chr();

    // Check second char
    int res = peek_chr();

    cr_expect(res == 'c');

    io_close();
}

Test(Iobackend, test_string_get)
{
    struct config conf = { 0 };
    conf.method = STRING;
    conf.str_stream = "echo a";

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
