#ifndef AST_H
#define AST_H

enum ast_type
{
    AST_INPUT,
    AST_LIST,
    AST_AND_OR,
    AST_PIPELINE,
    AST_CMD,
    AST_SIMPLE_CMD,
    AST_SHELL_CMD,
    AST_FUNCDEC,
    AST_REDIRECTION,
    AST_PREFIX,
    AST_ELEMENT,
    AST_COMPOUND_LIST,
    AST_RULE_FOR,
    AST_RULE_WHILE,
    AST_RULE_UNTIL,
    AST_RULE_CASE,
    AST_RULE_IF,
    AST_CLAUSE_ELSE,
    AST_CLAUSE_CASE,
    AST_CASE_ITEM
};

struct ast
{
    enum ast_type type;
};

struct ast_input
{
    struct ast base;
    struct ast *list;
};

struct ast_list
{
    struct ast base;
    struct ast *and_or;
    struct ast *next;
};

enum and_or
{
    AND,
    OR
};

struct ast_and_or
{
    struct ast base;
    struct ast *pipeline;
    enum and_or operand;
    struct ast *next;
};

struct ast_pipeline
{
    struct ast base;
    int negation;
    struct ast *cmd;
    struct ast *next;
};

struct ast_cmd
{
    struct ast base;
    struct ast *cmd;
    struct ast *redirection;
};

struct ast_prefix
{
    struct ast base;
    char *assignment_word;
    struct ast *redirection;
};

struct prefix_list
{
    struct prefix prefix;
    struct prefix_list *next;
};

struct ast_element
{
    struct ast base;
    char *word;
    struct ast *redirection;
};

struct element_list
{
    struct element element;
    struct element_list *next;
};

struct ast_simple_cmd
{
    struct ast base;
    struct prefix_list *prefix_list;
    char *word;
    struct element_list *element_list;
};

struct ast_shell_cmd
{
    struct ast base;
    struct ast *compound_list;
    struct ast *keyword;
};

struct ast_funcdec
{
    struct ast base;
    char *word;
    struct ast *shell_cmd;
};

struct ast_redirection
{
    struct ast base;
    int io_number;
    enum redirection_type;
    char *word;
};

#endif // ! AST_H
