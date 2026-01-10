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
    AST_PREFIX_LIST,
    AST_ELEMENT,
    AST_ELEMENT_LIST,
    AST_COMPOUND_LIST,
    AST_WORD_LIST,
    AST_RULE_FOR,
    AST_RULE_WHILE,
    AST_RULE_UNTIL,
    AST_RULE_CASE,
    AST_RULE_IF,
    AST_CLAUSE_ELSE,
    AST_CLAUSE_CASE,
    AST_CASE_ITEM,
    AST_CASE_ITEM_LIST
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
    int negation; // TODO: Use bool or enum.
    struct ast *cmd;
    struct ast *next;
};

struct ast_cmd
{
    struct ast base;
    struct ast *cmd; // Can be ast_simple_cmd or ast_shell_cmd or ast_funcdec.
    struct ast *redirection; // Only for shell_cmd or funcdec.
};

struct ast_prefix
{
    struct ast base;
    char *assignment_word;
    struct ast *redirection;
};

struct ast_prefix_list
{
    struct ast base;
    struct ast *prefix;
    struct ast *next;
};

struct ast_element
{
    struct ast base;
    char *word;
    struct ast *redirection;
};

struct ast_element_list
{
    struct ast base;
    struct ast *element;
    struct ast *next;
};

struct ast_simple_cmd
{
    struct ast base;
    struct ast *prefix;
    struct ast *prefix_list;
    char *word;
    struct ast *element_list;
};

struct ast_shell_cmd
{
    struct ast base;
    // TODO: Choose if we let these two fields or use just one, beacause only
    // one of the two will be used at the same time.
    struct ast *compound_list;
    struct ast *rule;
};

struct ast_funcdec
{
    struct ast base;
    char *name;
    struct ast *shell_cmd;
};

// struct ast_redirection
// {
//     struct ast base;
//     int io_number;
//     enum redirection_type;
//     char *word;
// };

// struct and_or_list
// {
//     struct ast_and_or ast_and_or;
//     struct ast *next;
// };

struct ast_compound_list
{
    struct ast base;
    // struct and_or_list *and_or_list;
    struct ast *ast_and_or;
    struct ast *next;
};

struct ast_word_list
{
    struct ast base;
    char *word;
    struct ast *next;
};

struct ast_rule_for
{
    struct ast base;
    char *condition_word;
    struct ast *in_word_list;
    struct ast *body_compound_list;
};

struct ast_rule_while
{
    struct ast base;
    struct ast *condition_compound_list;
    struct ast *body_compound_list;
};

struct ast_rule_until
{
    struct ast base;
    struct ast *condition_compound_list;
    struct ast *body_compound_list;
};

struct ast_rule_case
{
    struct ast base;
    char *word;
    struct ast *case_clause;
};

struct ast_rule_if
{
    struct ast base;
    struct ast *condition_compound_list;
    struct ast *body_compound_list;
    struct ast *else_clause;
};

struct ast_else_clause
{
    struct ast base;
    struct ast *condition_compound_list;
    struct ast *body_compound_list;
    struct ast *else_clause;
};

struct ast_case_item
{
    struct ast base;
    // char *word;
    struct ast *word_list;
    struct ast *compound_list;
};

struct ast_case_item_list
{
    struct ast base;
    struct ast *case_item;
    struct ast *next;
};

struct ast_case_clause
{
    struct ast base;
    struct ast *case_item_list;
};

struct ast *init_ast(enum ast_type type);
void free_ast_input(struct ast *ast);

#endif // ! AST_H
