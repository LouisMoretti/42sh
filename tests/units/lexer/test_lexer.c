#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/parameterized.h>
#include <criterion/redirect.h>
#include <string.h>

#include "config/config.h"
#include "iobackend/iobackend.h"
#include "lexer/lexer.h"

#define RESULT_TOKEN_ARRAY_SIZE 32
#define INDICES_ARRAY_SIZE 42

struct token_list_params
{
    char *name_test;
    char *input;
    struct token result[RESULT_TOKEN_ARRAY_SIZE];
    enum keyword_policy policy;
};

struct token_consistency_params
{
    char *name_test;
    char *input;
    struct token enable;
    struct token disable;
};

const char *type_name[] = { [IF] = "IF",
                            [THEN] = "THEN",
                            [ELIF] = "ELIF",
                            [ELSE] = "ELSE",
                            [FI] = "FI",
                            // [KEYWORD_COUNT] = "KEYWORD_COUNT",
                            [SEMICOLON] = "SEMICOLON",
                            [WORD] = "WORD",
                            [END_OF_FILE] = "END_OF_FILE" };

TestSuite(Lexer, .timeout = 1);

static struct token_list_params token_list_params[] = {
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
      .result = { { WORD, "echo' toto'" }, { END_OF_FILE, "" } },
      .policy = ENABLE_KEYWORDS },
    { .name_test = "squote_medium_odd",
      .input = "ec'''''''''ho' toto",
      .result = { { WORD, "ec'''''''''ho'" },
                  { WORD, "toto" },
                  { END_OF_FILE, "" } },
      .policy = ENABLE_KEYWORDS },
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
      .policy = ENABLE_KEYWORDS },
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
    { .name_test = "comment_newline",
      .input = "ls; #aaa\n cd;",
      .result = { { WORD, "ls" },
                  { SEMICOLON, "" },
                  { WORD, "cd" },
                  { SEMICOLON, "" },
                  { END_OF_FILE, "" } },
      .policy = ENABLE_KEYWORDS },
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

ParameterizedTestParameters(Lexer, test_get_token)
{
    static int indices[INDICES_ARRAY_SIZE] = { 0 };
    size_t nb_params =
        sizeof(token_list_params) / sizeof(struct token_list_params);
    cr_assert(nb_params < INDICES_ARRAY_SIZE,
              "Too many parameters for indices array");

    for (size_t i = 0; i < nb_params; i++)
        indices[i] = i;

    return cr_make_param_array(int, indices, nb_params);
}

ParameterizedTest(int *index, Lexer, test_get_token)
{
    struct token_list_params *param = &token_list_params[*index];

    struct config test_conf = { STRING, param->input, 0, NULL };
    io_setup(&test_conf);
    int i = 0;
    while (i < RESULT_TOKEN_ARRAY_SIZE && param->result[i].type != END_OF_FILE)
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

ParameterizedTestParameters(Lexer, test_peek_pop_token)
{
    static int indices[INDICES_ARRAY_SIZE] = { 0 };
    size_t nb_params =
        sizeof(token_list_params) / sizeof(struct token_list_params);
    cr_assert(nb_params < INDICES_ARRAY_SIZE,
              "Too many parameters for indices array");

    for (size_t i = 0; i < nb_params; i++)
        indices[i] = i;

    return cr_make_param_array(int, indices, nb_params);
}

ParameterizedTest(int *index, Lexer, test_peek_pop_token)
{
    struct token_list_params *param = &token_list_params[*index];

    struct config test_conf = { STRING, param->input, 0, NULL };
    io_setup(&test_conf);
    int i = 0;
    while (i < RESULT_TOKEN_ARRAY_SIZE && param->result[i].type != END_OF_FILE)
    {
        struct token *token = peek_token(param->policy);
        pop_token();
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

static struct token_consistency_params token_consistency_params[] = {
    { .name_test = "consistency_if",
      .input = "if",
      .enable = { IF, "" },
      .disable = { WORD, "if" } },
    { .name_test = "consistency_then",
      .input = "then",
      .enable = { THEN, "" },
      .disable = { WORD, "then" } },
    { .name_test = "consistency_elif",
      .input = "elif",
      .enable = { ELIF, "" },
      .disable = { WORD, "elif" } },
    { .name_test = "consistency_else",
      .input = "else",
      .enable = { ELSE, "" },
      .disable = { WORD, "else" } },
    { .name_test = "consistency_fi",
      .input = "fi",
      .enable = { FI, "" },
      .disable = { WORD, "fi" } },
    { .name_test = "consistency_semicolon",
      .input = ";",
      .enable = { SEMICOLON, "" },
      .disable = { SEMICOLON, "" } },
    { .name_test = "consistency_word",
      .input = "42sh",
      .enable = { WORD, "42sh" },
      .disable = { WORD, "42sh" } },
    { .name_test = "consistency_end_of_file",
      .input = "",
      .enable = { END_OF_FILE, "" },
      .disable = { END_OF_FILE, "" } }
};

ParameterizedTestParameters(Lexer, test_token_consistency_enable_enable)
{
    static int indices[INDICES_ARRAY_SIZE] = { 0 };
    size_t nb_params = sizeof(token_consistency_params)
        / sizeof(struct token_consistency_params);
    cr_assert(nb_params < INDICES_ARRAY_SIZE,
              "Too many parameters for indices array");

    for (size_t i = 0; i < nb_params; i++)
        indices[i] = i;

    return cr_make_param_array(int, indices, nb_params);
}

ParameterizedTest(int *index, Lexer, test_token_consistency_enable_enable)
{
    struct token_consistency_params *param = &token_consistency_params[*index];

    struct config test_conf = { STRING, param->input, 0, NULL };
    io_setup(&test_conf);

    struct token *token = peek_token(ENABLE_KEYWORDS);

    cr_expect_eq(token->type, param->enable.type,
                 "Test %s - First iteration - Expected: %s | Got: %s",
                 param->name_test, type_name[param->enable.type],
                 type_name[token->type]);

    if (token->type == WORD && param->enable.type == WORD)
        cr_expect_str_eq(token->data, param->enable.data,
                         "Test %s - First iteration - Expected: %s | Got: %s ",
                         param->name_test, param->enable.data, token->data);

    token = peek_token(ENABLE_KEYWORDS);

    cr_expect_eq(token->type, param->enable.type,
                 "Test %s - Second iteration - Expected: %s | Got: %s",
                 param->name_test, type_name[param->enable.type],
                 type_name[token->type]);

    if (token->type == WORD && param->enable.type == WORD)
        cr_expect_str_eq(token->data, param->enable.data,
                         "Test %s - Second iteration - Expected: %s | Got: %s ",
                         param->name_test, param->enable.data, token->data);
}

ParameterizedTestParameters(Lexer, test_token_consistency_disable_disable)
{
    static int indices[INDICES_ARRAY_SIZE] = { 0 };
    size_t nb_params = sizeof(token_consistency_params)
        / sizeof(struct token_consistency_params);
    cr_assert(nb_params < INDICES_ARRAY_SIZE,
              "Too many parameters for indices array");

    for (size_t i = 0; i < nb_params; i++)
        indices[i] = i;

    return cr_make_param_array(int, indices, nb_params);
}

ParameterizedTest(int *index, Lexer, test_token_consistency_disable_disable)
{
    struct token_consistency_params *param = &token_consistency_params[*index];

    struct config test_conf = { STRING, param->input, 0, NULL };
    io_setup(&test_conf);

    struct token *token = peek_token(DISABLE_KEYWORDS);

    cr_expect_eq(token->type, param->disable.type,
                 "Test %s - First iteration - Expected: %s | Got: %s",
                 param->name_test, type_name[param->disable.type],
                 type_name[token->type]);

    if (token->type == WORD && param->disable.type == WORD)
        cr_expect_str_eq(token->data, param->disable.data,
                         "Test %s - First iteration - Expected: %s | Got: %s ",
                         param->name_test, param->disable.data, token->data);

    token = peek_token(DISABLE_KEYWORDS);

    cr_expect_eq(token->type, param->disable.type,
                 "Test %s - Second iteration - Expected: %s | Got: %s",
                 param->name_test, type_name[param->disable.type],
                 type_name[token->type]);

    if (token->type == WORD && param->disable.type == WORD)
        cr_expect_str_eq(token->data, param->disable.data,
                         "Test %s - Second iteration - Expected: %s | Got: %s ",
                         param->name_test, param->disable.data, token->data);
}

ParameterizedTestParameters(Lexer, test_token_consistency_enable_disable)
{
    static int indices[INDICES_ARRAY_SIZE] = { 0 };
    size_t nb_params = sizeof(token_consistency_params)
        / sizeof(struct token_consistency_params);
    cr_assert(nb_params < INDICES_ARRAY_SIZE,
              "Too many parameters for indices array");

    for (size_t i = 0; i < nb_params; i++)
        indices[i] = i;

    return cr_make_param_array(int, indices, nb_params);
}

ParameterizedTest(int *index, Lexer, test_token_consistency_enable_disable)
{
    struct token_consistency_params *param = &token_consistency_params[*index];

    struct config test_conf = { STRING, param->input, 0, NULL };
    io_setup(&test_conf);

    struct token *token = peek_token(ENABLE_KEYWORDS);

    cr_expect_eq(token->type, param->enable.type,
                 "Test %s - First iteration - Expected: %s | Got: %s",
                 param->name_test, type_name[param->enable.type],
                 type_name[token->type]);

    if (token->type == WORD && param->enable.type == WORD)
        cr_expect_str_eq(token->data, param->enable.data,
                         "Test %s - First iteration - Expected: %s | Got: %s",
                         param->name_test, param->enable.data, token->data);

    token = peek_token(DISABLE_KEYWORDS);

    cr_expect_eq(token->type, param->disable.type,
                 "Test %s - Second iteration - Expected: %s | Got: %s",
                 param->name_test, type_name[param->disable.type],
                 type_name[token->type]);

    if (token->type == WORD && param->disable.type == WORD)
        cr_expect_str_eq(token->data, param->disable.data,
                         "Test %s - Second iteration - Expected: %s | Got: %s",
                         param->name_test, param->disable.data, token->data);
}

ParameterizedTestParameters(Lexer, test_token_consistency_disable_enable)
{
    static int indices[INDICES_ARRAY_SIZE] = { 0 };
    size_t nb_params = sizeof(token_consistency_params)
        / sizeof(struct token_consistency_params);
    cr_assert(nb_params < INDICES_ARRAY_SIZE,
              "Too many parameters for indices array");

    for (size_t i = 0; i < nb_params; i++)
        indices[i] = i;

    return cr_make_param_array(int, indices, nb_params);
}

ParameterizedTest(int *index, Lexer, test_token_consistency_disable_enable)
{
    struct token_consistency_params *param = &token_consistency_params[*index];

    struct config test_conf = { STRING, param->input, 0, NULL };
    io_setup(&test_conf);

    struct token *token = peek_token(DISABLE_KEYWORDS);

    cr_expect_eq(token->type, param->disable.type,
                 "Test %s - First iteration - Expected: %s | Got: %s",
                 param->name_test, type_name[param->disable.type],
                 type_name[token->type]);

    if (token->type == WORD && param->disable.type == WORD)
        cr_expect_str_eq(token->data, param->disable.data,
                         "Test %s - First iteration - Expected: %s | Got: %s",
                         param->name_test, param->disable.data, token->data);

    token = peek_token(ENABLE_KEYWORDS);

    cr_expect_eq(token->type, param->enable.type,
                 "Test %s - Second iteration - Expected: %s | Got: %s",
                 param->name_test, type_name[param->enable.type],
                 type_name[token->type]);

    if (token->type == WORD && param->enable.type == WORD)
        cr_expect_str_eq(token->data, param->enable.data,
                         "Test %s - Second iteration - Expected: %s | Got: %s",
                         param->name_test, param->enable.data, token->data);
}
