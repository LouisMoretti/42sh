#ifndef LEXER_H
#define LEXER_H

enum token_type
{
    IF,
    THEN,
    ELIF,
    ELSE,
    FI,
    FOR,
    DO,
    DONE,
    WHILE,
    UNTIL,
    // CASE,
    IN,
    // ESAC,
    NEGATION,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    KEYWORD_COUNT, // Keep this here, add keywords above only
    NEW_LINE,
    REDIRECTION,
    SEMICOLON,
    DOUBLE_SEMICOLON,
    PIPE,
    DOUBLE_PIPE,
    AMPERSAND,
    DOUBLE_AMPERSAND,
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    WORD,
    END_OF_FILE,
    ERROR
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
