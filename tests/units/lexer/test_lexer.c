#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/parameterized.h>
#include <criterion/redirect.h>
#include <string.h>
#include <stdio.h>

#include "../../../src/iobackend/iobackend.h"
#include "../../../src/lexer/lexer.h"

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

ParameterizedTestParameters(Peak_token, SimpleWord)
{
    static struct my_params params[] = {
        { .input = "echo", .result = { { WORD, "echo" }, { ENDOFFILE, "" } } },
        { .input = "toto", .result = { { WORD, "toto" }, { ENDOFFILE, "" } } },
        { .input = "cat tata",
          .result = { { WORD, "cat" }, { WORD, "tata" }, { ENDOFFILE, "" } } },
        { .input = "if true then echo trou else echo 'faux le se' fi;",
          .result = { { IF, "" },
                      { WORD, "true" },
                      { THEN, "" },
                      {WORD, "echo"},
                      { WORD, "trou" },
                      { ELSE, "" },
                      { WORD, "echo" },
                      { WORD, "'faux le se'" },
                      { FI, "" },
                      { SEMICOLON, "" },
                      { ENDOFFILE, "" } } }
    };

    size_t nb_params = sizeof(params) / sizeof(struct my_params);
    return cr_make_param_array(struct my_params, params, nb_params);
}

ParameterizedTest(struct my_params *param, Peak_token, SimpleWord)
{
    io_setup_string(param->input);
    int i = 0;
    while (i < 32 && param->result[i].type != ENDOFFILE)
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
