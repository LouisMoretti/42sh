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
    char *name_test;
    char *input;
    struct token result[32];
    enum keyword_policy policy;
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
    { .name_test = "simple_word",
      .input = "echo",
      .result = { { WORD, "echo" }, { END_OF_FILE, "" } },
      .policy = ENABLE_KEYWORDS },
    { .name_test = "semicolon",
      .input = "toto;",
      .result = { { WORD, "toto" }, { SEMICOLON, "" }, { END_OF_FILE, "" } },
      .policy = ENABLE_KEYWORDS },
    { .name_test = "multiple_words",
      .input = "cat tata",
      .result = { { WORD, "cat" }, { WORD, "tata" }, { END_OF_FILE, "" } },
      .policy = ENABLE_KEYWORDS },
    { .name_test = "if_else",
      .input = "if true then echo trou else echo 'faux le se' fi;",
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
                  { END_OF_FILE, "" } },
      .policy = ENABLE_KEYWORDS },
    { .name_test = "single_quote_simple",
      .input = "if true then echo trou else echo 'faux le se' fi;",
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
                  { END_OF_FILE, "" } },
      .policy = ENABLE_KEYWORDS },
    { .name_test = "blank_chars",
      .input = "echo     \t \t lol;",
      .result = { { WORD, "echo" },
                  { WORD, "lol" },
                  { SEMICOLON, "" },
                  { END_OF_FILE, "" } },
      .policy = ENABLE_KEYWORDS },
    { .name_test = "backslash_singleq_graveaccent`",
      .input = "cat '\\\\  \\`';",
      .result = { { WORD, "cat" },
                  { WORD, "'\\\\  \\`'" },
                  { SEMICOLON, "" },
                  { END_OF_FILE, "" } },
      .policy = ENABLE_KEYWORDS },
    { .name_test = "dollar",
      .input = "echo '\\ \t$'",
      .result = { { WORD, "echo" }, { WORD, "'\\ \t$'" }, { END_OF_FILE, "" } },
      .policy = ENABLE_KEYWORDS },
    { .name_test = "squote_medium_cursed",
      .input = "echo' toto'",
      .result = { { WORD, "echo' toto'" } },
      .policy = ENABLE_KEYWORDS }, // Crashing
    { .name_test = "squote_medium_odd",
      .input = "ec'''''''''ho' toto",
      .result = { { WORD, "ec'''''''''ho'" }, { WORD, "toto" } },
      .policy = ENABLE_KEYWORDS }, // Crashing
    { .name_test = "squote_medium_even",
      .input = "c''''''at; '42.sh'",
      .result = { { WORD, "c''''''at" },
                  { SEMICOLON, "" },
                  { WORD, "'42.sh'" },
                  { END_OF_FILE, "" } },
      .policy = ENABLE_KEYWORDS },
    { .name_test = "multiple_newlines",
      .input = "if true;\nthen true;\n else if false;\n then false;",
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
                  { END_OF_FILE, "" } },
      .policy = ENABLE_KEYWORDS }, // Not Working but not crashing
    { .name_test = "dquote_backslash",
      .input = "if true then echo \"\\'\\\\\" else if false then cat "
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
                  { END_OF_FILE, "" } },
      .policy = ENABLE_KEYWORDS },
    {
        .name_test = "comment_newline",
        .input = "ls; #aaa\n cd;",
        .result = { { WORD, "ls" },
                    { SEMICOLON, "" },
                    { WORD, "cd" },
                    { SEMICOLON, "" },
                    { END_OF_FILE, "" } },
        .policy = ENABLE_KEYWORDS // Not working
    },
    { .name_test = "comment",
      .input = "a #b",
      .result = { { WORD, "a" }, { END_OF_FILE, "" } },
      .policy = ENABLE_KEYWORDS },
    { .input = "echo then if else fi then",
      .result = { { WORD, "echo" },
                  { WORD, "then" },
                  { WORD, "if" },
                  { WORD, "else" },
                  { WORD, "fi" },
                  { WORD, "then" },
                  { END_OF_FILE, "" } },
      .policy = DISABLE_KEYWORDS },
    { .input = "",
      .result = { { END_OF_FILE, "" } },
      .policy = ENABLE_KEYWORDS }
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

    struct config test_conf = { STRING, param->input, 0 };
    io_setup(&test_conf);
    int i = 0;
    while (i < 32 && param->result[i].type != END_OF_FILE)
    {
        struct token *token = get_token(param->policy);
        cr_expect_eq(
            token->type, param->result[i].type,
            "Test %s - Wrong %dnth Token Type - Expected: %s | Got: %s",
            param->name_test, i, type_name[param->result[i].type],
            type_name[token->type]);
        if (param->result[i].type == WORD && token->type == WORD)
            cr_expect_str_eq(
                token->data, param->result[i].data,
                "Test %i - Wrong %dnth Token - Expected: %s | Got: %s",
                param->name_test, i, param->result[i].data, token->data);
        i++;
    }
}
