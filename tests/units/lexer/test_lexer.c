#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/parameterized.h>
#include <criterion/redirect.h>
#include <stdio.h>
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

const char *type_name[9] = { "IF",         "THEN",      "ELIF",          "ELSE",
                             "FI",         "SEMICOLON", "KEYWORD_COUNT", "WORD",
                             "END_OF_FILE" };

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
                  { END_OF_FILE, "" } } },
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
                  { END_OF_FILE, "" } } },
    { .input = "echo     \t \t lol;",
      .result = { { WORD, "echo" },
                  { WORD, "lol" },
                  { SEMICOLON, "" },
                  { END_OF_FILE, "" } } },
    { .input = "cat '\\\\  \\`';",
      { { WORD, "cat" },
        { WORD, "'\\\\  \\`'" },
        { SEMICOLON, "" },
        { END_OF_FILE, "" } } },
    { .input = "echo '\\ \t$'",
      .result = { { WORD, "echo" },
                  { WORD, "'\\ \t$'" },
                  { END_OF_FILE, "" } } },
    { .input = "echo' toto'",
      .result = { { WORD, "echo' toto'" } } }, // Crashing
    { .input = "ec'''''''''ho' toto",
      .result = { { WORD, "ec'''''''''ho'" }, { WORD, "toto" } } }, // Crashing
    { .input = "if true;\nthen true;\n else if false;\n then false;",
      .result = { { IF, "" },
                  { WORD, "true" },
                  { SEMICOLON, "" },
                  { THEN, "" },
                  { WORD, "true" },
                  { SEMICOLON, "" },
                  { ELSE, "" },
                  { IF, "" },
                  { WORD, "false" },
                  { SEMICOLON, "" },
                  { THEN, "" },
                  { WORD, "false" },
                  { SEMICOLON, "" },
                  { END_OF_FILE, "" } } }, // Not Working but not crashing
    { .input = "if true then echo \"\\'\\\\\" else if false then cat "
               "'\\\"\\\\' else "
               "echo 'else "
               "if then "
               "else' fi;",
      .result = { { IF, "" },
                  { WORD, "true" },
                  { THEN, "" },
                  { WORD, "echo" },
                  { WORD, "\"\\'\\\\\"" },
                  { ELSE, "" },
                  { IF, "" },
                  { WORD, "false" },
                  { THEN, "" },
                  { WORD, "cat" },
                  { WORD, "'\\\"\\\\'" },
                  { ELSE, "" },
                  { WORD, "echo" },
                  { WORD, "'else if then else'" },
                  { FI, "" },
                  { SEMICOLON, "" },
                  { END_OF_FILE, "" } } },
    {.input = "ls; #aaa\n cd;", .result = {{WORD, "ls"}, {SEMICOLON, ""}, {WORD,"cd"}, {SEMICOLON, ""}, {END_OF_FILE, ""}} // Not working
}};

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
                     "Test %i - Wrong %dnth Token Type - Expected: %s | Got: %s",*index, i,
                     type_name[param->result[i].type], type_name[token->type]);
        if (param->result[i].type == WORD && token->type == WORD)
            cr_expect_str_eq(token->data, param->result[i].data,
                             "Test %i - Wrong %dnth Token - Expected: %s | Got: %s",*index, i,
                             param->result[i].data, token->data);
        i++;
    }
}
