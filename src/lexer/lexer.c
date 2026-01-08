#define _POSIX_C_SOURCE 200809L
#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iobackend/iobackend.h"

#define BUFFER_SIZE 1024

static int g_has_cur = 0;
static char buffer[BUFFER_SIZE] = { 0 };
static struct token g_cur_token = { .type = WORD, .data = buffer };

static const char *keywords[KEYWORD_COUNT] = {
    [IF] = "if",     [THEN] = "then", [ELIF] = "elif",
    [ELSE] = "else", [FI] = "fi",     [SEMICOLON] = ";"
};

static int is_space(int c)
{
    return c == ' ' || c == '\t' || c == '\n';
}

static enum token_type get_token_type(char *str, enum keyword_policy policy)
{
    if (policy == DISABLE_KEYWORDS)
        return WORD;

    for (int i = 0; i < KEYWORD_COUNT; i++)
    {
        if (strcmp(str, keywords[i]) == 0)
            return (enum token_type)i;
    }

    return WORD;
}

struct token *peek_token(enum keyword_policy policy)
{
    if (g_has_cur)
        return &g_cur_token;

    int c = peek_chr();

    // TODO: Loop skip whitespace and skip comment.

    // Skip whitespace.
    while (is_space(c))
    {
        pop_chr();
        c = peek_chr();
    }

    // TODO: Skip comment.
    if (c == '#')
    {
        while (c != EOF || c != '\n')
        {
            pop_chr();
            c = peek_chr();
        }
    }

    // TODO: Handle end token.
    if (c == EOF)
        return NULL;

    if (c == ';')
    {
        // TODO: Handle double semicolon token (For step 4).
        g_cur_token.type = SEMICOLON;
        g_has_cur = 1;
        return &g_cur_token;
    }

    int index = 0;

    int is_quoted = 0;
    char quote_chr = '\0';
    int is_escaped = 0;

    while (c != EOF && index < BUFFER_SIZE)
    {
        // 11 Rules from the SCL
        // TODO: Add '>' to break loop

        if (!is_quoted && !is_escaped && (is_space(c) || c == ';')) // Rule #8
            break;

        // Rule #4: Quoting.
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
        pop_chr();
        c = peek_chr();
    }

    // TODO: Handle token too long error.
    if (index == sizeof(buffer))
    {
        fprintf(stderr, "peek_token(): Token too long\n");
        return NULL;
    }

    // TODO: Handle missing quote error.
    if (is_quoted)
    {
        fprintf(stderr, "peek_token(): Missing quote\n");
        return NULL;
    }

    buffer[index] = '\0';

    g_cur_token.type = get_token_type(buffer, policy);
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
