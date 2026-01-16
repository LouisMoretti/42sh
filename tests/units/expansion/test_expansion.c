#define _POSIX_C_SOURCE 200809L

#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <stdlib.h>
#include <string.h>

#include "expansion/expansion.h"

TestSuite(Expansion, .timeout = 1);

// ==================
//       STRING
// ==================
static char *expand_ref_execution(char *to_expand)
{
    char *tmp = strndup(to_expand, strlen(to_expand));
    char *result = expand_string(tmp);
    free(tmp);

    return result;
}

Test(Expansion, test_expansion_string_simple)
{
    char *to_be_expanded = "abcdefgh";
    char *expansion_expected = "abcdefgh";

    char *result = expand_ref_execution(to_be_expanded);
    cr_expect_str_eq(result, expansion_expected);

    free(result);
}

Test(Expansion, test_expansion_string_escaped)
{
    char *to_be_expanded = "ab\\\\cdefgh";
    char *expansion_expected = "ab\\cdefgh";

    char *result = expand_ref_execution(to_be_expanded);
    cr_expect_str_eq(result, expansion_expected);

    free(result);
}

Test(Expansion, test_expansion_string_simple_quoted)
{
    char *to_be_expanded = "ab\'cdefg\'h";
    char *expansion_expected = "abcdefgh";

    char *result = expand_ref_execution(to_be_expanded);
    cr_expect_str_eq(result, expansion_expected);

    free(result);
}

Test(Expansion, test_expansion_string_many_quotes)
{
    char *to_be_expanded = "a'bc'''de'fg''h'";
    char *expansion_expected = "abcdefgh";

    char *result = expand_ref_execution(to_be_expanded);
    cr_expect_str_eq(result, expansion_expected);

    free(result);
}

Test(Expansion, test_expansion_string_quoted_inner_tricky_backslash)
{
    char *to_be_expanded = "a'\\bc'defgh";
    char *expansion_expected = "a\\bcdefgh";

    char *result = expand_ref_execution(to_be_expanded);
    cr_expect_str_eq(result, expansion_expected);

    free(result);
}

Test(Expansion, test_expansion_string_quote_escaped)
{
    char *to_be_expanded = "a\\'bcdefgh";
    char *expansion_expected = "a\'bcdefgh";

    char *result = expand_ref_execution(to_be_expanded);
    cr_expect_str_eq(result, expansion_expected);

    free(result);
}

Test(Expansion, test_expansion_string_quote_escaped_and_not_escaped)
{
    char *to_be_expanded = "a\\'bc\\''d'''efg'h'''";
    char *expansion_expected = "a'bc'defgh";

    char *result = expand_ref_execution(to_be_expanded);
    cr_expect_str_eq(result, expansion_expected);

    free(result);
}

// ==================
//        ECHO
// ==================

static char *expand_ref_echo(char *to_expand)
{
    char *tmp = strndup(to_expand, strlen(to_expand));
    char *result = expand_echo(tmp);
    free(tmp);

    return result;
}

Test(Expansion, test_expansion_echo_robust)
{
    char *to_be_expanded =
        "\\n\\n\\n\\t\\t\\t\\n\\'\\'\\t\\'\\\\\\'\\'\\'\\\\\\t";
    char *expansion_expected = "\n\n\n\t\t\t\n\\\'\\\'\t\\\'\\\\'\\\'\\\'\\\t";

    char *result = expand_ref_echo(to_be_expanded);
    cr_expect_str_eq(result, expansion_expected);

    free(result);
}

Test(Expansion, test_expansion_echo_newline)
{
    char *to_be_expanded = "abcdefgh\\n";
    char *expansion_expected = "abcdefgh\n";

    char *result = expand_ref_echo(to_be_expanded);
    cr_expect_str_eq(result, expansion_expected);

    free(result);
}

Test(Expansion, test_expansion_echo_tab)
{
    char *to_be_expanded = "\\tabcdefgh";
    char *expansion_expected = "\tabcdefgh";

    char *result = expand_ref_echo(to_be_expanded);
    cr_expect_str_eq(result, expansion_expected);

    free(result);
}
