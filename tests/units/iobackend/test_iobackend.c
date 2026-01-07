#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>
#include <string.h>

#include "../../../src/config/config.h"
#include "../../../src/iobackend/iobackend.h"

TestSuite(Iobackend_usage);

Test(Usages, Usage_c)
{
    char *argv[2] = { "-c", "echo a" };
    int s = set_conf(2, argv);
    int res = io_setup();
    struct config *conf = get_conf();

    cr_expect(s == 0);
    cr_expect(res == 0);
    cr_expect(conf->method == STRING);
    cr_expect(strcmp(conf->str_stream, "echo a"));
}

Test(Usages, Usage_file)
{
    char *argv[1] = { "script.sh" };
    int s = set_conf(1, argv);
    int res = io_setup();
    struct config *conf = get_conf();

    cr_expect(s == 0);
    cr_expect(res == 0);
    cr_expect(conf->method == FILEPATH);
    cr_expect(strcmp(conf->str_stream, "script.sh"));
}

Test(Usages, Usage_stdin)
{
    char **argv = NULL;
    int s = set_conf(0, argv);
    int res = io_setup();
    struct config *conf = get_conf();

    cr_expect(s == 0);
    cr_expect(res == 0);
    cr_expect(conf->method == STDIN);
    cr_expect(conf->str_stream == NULL);
}
