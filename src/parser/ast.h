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
    struct ast *list; // NULL if empty input
};

struct ast_list
{
    struct ast base;
    struct ast *and_or; // NOT NULL
    struct ast *next; // NULL if last element of list
};

enum and_or
{
    AND,
    OR
};

struct ast_and_or
{
    struct ast base;
    struct ast *pipeline; // NOT NULL
    enum and_or operand; // Only set if next is NULL
    struct ast *next; // NULL if last element of list
};

struct ast_pipeline
{
    struct ast base;
    int negation;
    struct ast *cmd; // NOT NULL
    struct ast *next; // NULL if last element of list
};

struct ast_cmd
{
    struct ast base;
    struct ast *cmd; // Can be ast_redirection or ast_simple_cmd or
                     // ast_shell_cmd or ast_funcdec.
    // struct ast *redirection; // Only for shell_cmd or funcdec.
};

struct ast_prefix
{
    struct ast base;
    // Must be either assignment_word
    char *assignment_word;
    // struct ast *redirection;
};

struct ast_prefix_list
{
    struct ast base;
    struct ast *prefix; // NOT NULL
    struct ast *next; // NULL if last element of list
};

struct ast_element
{
    struct ast base;
    // Must be either word or redirection
    char *word;
    // struct ast *redirection;
};

struct ast_element_list
{
    struct ast base;
    struct ast *element; // NOT NULL
    struct ast *next; // NULL if last element of list
};

struct ast_simple_cmd
{
    struct ast base;
    struct ast *prefix_list; // Can be NULL if word isn't.
    char *word; // If word is NULL so is element_list.
    struct ast *element_list; // Can be NULL
};

struct ast_shell_cmd
{
    struct ast base;
    // Must be either compound_list or rule
    struct ast *compound_list;
    struct ast *rule;
};

struct ast_funcdec
{
    struct ast base;
    char *name; // NOT NULL
    struct ast *shell_cmd; // NOT NULL
};

enum redirection_type
{
    REDIRECT_OUT, // > (default io number: 1)
    REDIRECT_IN, // < (default io number: 0)
    REDIRECT_OUT_APPEND, // >> (default io number: 1)
    // REDIRECT_IN_APPEND, // << (Heredoc)
    // <<- (Heredoc)
    REDIRECT_OUT_DUP, // >&
    REDIRECT_IN_DUP, // <&
    REDIRECT_OUT_FORCE, // >| (noclobber) (default io number: 1)
    REDIRECT_IN_OUT // <>
};

struct ast_redirection
{
    struct ast base;
    int io_number; // -1 if no io number.
    enum redirection_type type;
    char *word; // NOT NULL
    struct ast *next; // ast_redirection or ast_simple_cmd or ast_shell_cmd
};

// struct and_or_list
// {
//     struct ast_and_or ast_and_or;
//     struct ast *next;
// };

struct ast_compound_list
{
    struct ast base;
    // struct and_or_list *and_or_list;
    struct ast *ast_and_or; // NOT NULL
    struct ast *next; // NULL if last element of list
};

struct ast_word_list
{
    struct ast base;
    char *word; // NOT NULL
    struct ast *next; // NULL if last element of list
};

struct ast_rule_for
{
    struct ast base;
    char *condition_word; // NOT NULL
    struct ast *in_word_list; // Can be NULL
    struct ast *body_compound_list; // NOT NULL
};

struct ast_rule_while
{
    struct ast base;
    struct ast *condition_compound_list; // NOT NULL
    struct ast *body_compound_list; // NOT NULL
};

struct ast_rule_until
{
    struct ast base;
    struct ast *condition_compound_list; // NOT NULL
    struct ast *body_compound_list; // NOT NULL
};

struct ast_rule_case
{
    struct ast base;
    char *word; // NOT NULL
    struct ast *case_clause; // Can be NULL
};

struct ast_rule_if
{
    struct ast base;
    struct ast *condition_compound_list; // NOT NULL
    struct ast *body_compound_list; // NOT NULL
    struct ast *else_clause; // Can be NULL
};

struct ast_else_clause
{
    struct ast base;

    struct ast *condition_compound_list; // Can be NULL with else_clause
    struct ast *body_compound_list; // NOT NULL
    struct ast *else_clause; // Can be NULL with condition_compound_list
};

struct ast_case_item
{
    struct ast base;
    // char *word;
    struct ast *word_list; // NOT NULL
    struct ast *compound_list; // Can be NULL
};

struct ast_case_item_list
{
    struct ast base;
    struct ast *case_item; // NOT NULL
    struct ast *next; // NULL if last element of list
};

struct ast_case_clause
{
    struct ast base;
    struct ast *case_item_list; // NOT NULL
};

struct ast *init_ast(enum ast_type type);
void free_ast(struct ast *ast);

#endif // ! AST_H
