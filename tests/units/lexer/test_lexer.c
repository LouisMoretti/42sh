#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/parameterized.h>
#include <criterion/redirect.h>
#include <string.h>

#include "config/config.h"
#include "iobackend/iobackend.h"
#include "lexer/lexer.h"

#define INDICES_ARRAY_SIZE 42

struct my_params
{
    char *input;
    struct token result[32];
};

TestSuite(Peak_token);

/*Test(Peak_token, simple_word)
{
    io_setup_string("echo");
    struct token *token = peek_token(DISABLE_KEYWORDS);
    cr_expect_eq(token->type, WORD);
    cr_expect_str_eq(token->data, "echo");
}*/

static struct my_params params[] = {
    { .input = "echo", .result = { { WORD, "echo" }, { END_OF_FILE, "" } } },
    { .input = "toto", .result = { { WORD, "toto" }, { END_OF_FILE, "" } } },
    { .input = "cat tata",
      .result = { { WORD, "cat" }, { WORD, "tata" }, { END_OF_FILE, "" } } },
    { .input = "if true then echo trou else echo 'faux le se' fi;",
      .result = { { IF, "" },
                  { WORD, "true" },
                  { THEN, "" },
                  { WORD, "echo" },
                  { WORD, "trou" },
                  { ELSE, "" },
                  { WORD, "echo" },
                  { WORD, "'faux le se'" },
                  { FI, "" },
                  { SEMICOLON, "" },
                  { END_OF_FILE, "" } } }
};

ParameterizedTestParameters(Peak_token, SimpleWord)
{
    static int indices[INDICES_ARRAY_SIZE] = { 0 };
    size_t nb_params = sizeof(params) / sizeof(struct my_params);
    cr_assert(nb_params < INDICES_ARRAY_SIZE,
              "Too many parameters for indices array");

    for (size_t i = 0; i < nb_params; i++)
        indices[i] = i;

    return cr_make_param_array(int, indices, nb_params);
}

ParameterizedTest(int *index, Peak_token, SimpleWord)
{
    // int argc = 3;
    // char *argv[] = { "./42sh", "-c", param->input };
    // set_conf(argc, argv);
    // io_setup();

    struct my_params *param = &params[*index];

    struct config test_conf = { STRING, param->input };
    io_setup(&test_conf);

    int i = 0;
    while (i < 32 && param->result[i].type != END_OF_FILE)
    {
        struct token *token = get_token(ENABLE_KEYWORDS);
        cr_expect_eq(token->type, param->result[i].type,
                     "Wrong %dnth Token Type - Expected: %i | Got: %i", i,
                     param->result[i].type, token->type);
        if (param->result[i].type == WORD && token->type == WORD)
            cr_expect_str_eq(token->data, param->result[i].data,
                             "Wrong %dnth Token - Expected: %s | Got: %s", i,
                             param->result[i].data, token->data);
        i++;
    }
}
