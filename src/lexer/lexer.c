#define _POSIX_C_SOURCE 200809L
#include "lexer.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iobackend/iobackend.h"

// TODO: Use a vector instead of a static array.
#define BUFFER_SIZE 1024

static int g_has_cur = 0;
static char buffer[BUFFER_SIZE] = { 0 };
static enum token_type g_cur_type_before_policy = END_OF_FILE;
static struct token g_cur_token = { .type = END_OF_FILE, .data = buffer };

// TODO: Add future keywords here
static const char *keywords[KEYWORD_COUNT] = { [IF] = "if",
                                               [THEN] = "then",
                                               [ELIF] = "elif",
                                               [ELSE] = "else",
                                               [FI] = "fi" };

static int is_space(int c)
{
    return c == ' ' || c == '\t';
}

static enum token_type get_token_type(char *str)
{
    for (int i = 0; i < KEYWORD_COUNT; i++)
    {
        if (strcmp(str, keywords[i]) == 0)
            return (enum token_type)i;
    }

    return WORD;
}

static int pop_peek_chr()
{
    pop_chr();
    return peek_chr();
}

static int fill_buffer()
{
    int c = peek_chr();
    int index = 0;

    int is_quoted = 0;
    char quote_chr = '\0';
    int is_escaped = 0;

    while (c != EOF && index < BUFFER_SIZE)
    {
        // TODO: Add '>' to break loop

        if (!is_quoted && !is_escaped && (is_space(c) || c == '\n' || c == ';'))
            break;

        if (!is_escaped && (c == '\"' || c == '\'')
            && (!is_quoted || c == quote_chr))
        {
            is_quoted = !is_quoted;
            quote_chr = c;
        }

        if (!is_escaped && c == '\\')
            is_escaped = 1;
        else
            is_escaped = 0;

        buffer[index++] = c;
        c = pop_peek_chr();
    }

    // TODO: Remove when using vector.
    if (index == BUFFER_SIZE)
    {
        warnx("peek_token(): Token too long.");
        return 2;
    }

    // TODO: Handle missing quote error.
    if (is_quoted)
    {
        warnx("peek_token(): Missing quote.");
        return 2;
    }

    buffer[index] = '\0';
    return 0;
}

static void set_token_type_with_policy(enum keyword_policy policy)
{
    if (g_cur_type_before_policy < KEYWORD_COUNT && policy == DISABLE_KEYWORDS)
        g_cur_token.type = WORD;
    else
        g_cur_token.type = g_cur_type_before_policy;
}

static void skip_whitespace_and_comment()
{
    int c = peek_chr();

    // Skip whitespace.
    while (is_space(c))
        c = pop_peek_chr();

    // Skip comment.
    if (c == '#')
    {
        while (c != EOF && c != '\n')
            c = pop_peek_chr();
    }
}

struct token *peek_token(enum keyword_policy policy)
{
    if (g_has_cur)
    {
        set_token_type_with_policy(policy);
        return &g_cur_token;
    }

    skip_whitespace_and_comment();

    int c = peek_chr();

    if (c == EOF)
    {
        g_cur_type_before_policy = END_OF_FILE;
        set_token_type_with_policy(policy);
        g_has_cur = 1;
        return &g_cur_token;
    }

    if (c == '\n')
    {
        g_cur_type_before_policy = NEW_LINE;
        set_token_type_with_policy(policy);
        g_has_cur = 1;
        pop_chr();
        return &g_cur_token;
    }

    if (c == ';')
    {
        // TODO: Handle double semicolon token (For step 4).
        g_cur_type_before_policy = SEMICOLON;
        set_token_type_with_policy(policy);
        g_has_cur = 1;
        pop_chr();
        return &g_cur_token;
    }

    // TODO: Handle grammar errors.
    // if (fill_buffer() != 0)
    //     return NULL;
    fill_buffer();

    g_cur_type_before_policy = get_token_type(buffer);
    set_token_type_with_policy(policy);
    g_has_cur = 1;

    return &g_cur_token;
}

struct token *get_token(enum keyword_policy policy)
{
    struct token *token = peek_token(policy);
    g_has_cur = 0;
    return token;
}

// TODO: Call this function on crash or exit for memory leaks.
void pop_token(void)
{
    g_has_cur = 0;
}
