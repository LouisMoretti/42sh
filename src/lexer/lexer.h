#ifndef LEXER_H
#define LEXER_H

enum token_type
{
    IF,
    THEN,
    ELIF,
    ELSE,
    FI,
    SEMICOLON,
    KEYWORD_COUNT, // Keep this here, add keywords above only
    WORD // Same here
};

enum keyword_policy
{
    ENABLE_KEYWORDS,
    DISABLE_KEYWORDS
};

struct token
{
    enum token_type type;
    char *data;
};

struct token *peek_token(enum keyword_policy policy);
struct token *get_token(enum keyword_policy policy);
void pop_token(void);

#endif // ! LEXER_H
