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
    // DOUBLE_SEMICOLON,
    // PIPE,
    // DOUBLE_PIPE,
    // AMPERSAND,
    // DOUBLE_AMPERSAND,
    // NEGATION,
    // LEFT_PARANTHESIS,
    // RIGHT_PARANTHESIS,
    // LEFT_BRACKET,
    // RIGHT_BRACKET,
    // FOR,
    // DO,
    // DONE,
    // WHILE,
    // UNTIL,
    // CASE,
    // IN,
    // ESAC,
    KEYWORD_COUNT, // Keep this here, add keywords above only
    WORD,
    END_OF_FILE
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
