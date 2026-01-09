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

struct ast_input *init_ast_input();
void free_ast_input(struct ast_input *input);

struct ast_list *init_ast_list();
void free_ast_list(struct ast_list *list);

struct ast_and_or *init_ast_and_or();
void free_ast_and_or(struct ast_and_or *and_or);

struct ast_pipeline *init_ast_pipeline();
void free_ast_pipeline(struct ast_pipeline *pipeline);

struct ast_cmd *init_ast_cmd();
void free_ast_cmd(struct ast_cmd *cmd);

struct ast_prefix *init_ast_prefix();
void free_ast_prefix(struct ast_prefix *prefix);

struct ast_prefix_list *init_ast_prefix_list();
void free_ast_prefix_list(struct ast_prefix_list *list);

struct ast_element *init_ast_element();
void free_ast_element(struct ast_element *element);

struct ast_element_list *init_ast_element_list();
void free_ast_element_list(struct ast_element_list *list);

struct ast_simple_cmd *init_ast_simple_cmd();
void free_ast_simple_cmd(struct ast_simple_cmd *cmd);

struct ast_shell_cmd *init_ast_shell_cmd();
void free_ast_shell_cmd(struct ast_shell_cmd *shell_cmd);

struct ast_funcdec *init_ast_funcdec();
void free_ast_funcdec(struct ast_funcdec *funcdec);

// struct ast_redirection *init_ast_redirection();
// void free_ast_redirection(struct ast_redirection *redirection);

// struct and_or_list *init_and_or_list();
// void free_and_or_list(struct and_or_list *list);

struct ast_compound_list *init_ast_compound_list();
void free_ast_compound_list(struct ast_compound_list *compound_list);

struct ast_word_list *init_ast_word_list();
void free_ast_word_list(struct ast_word_list *word_list);

struct ast_rule_for *init_ast_rule_for();
void free_ast_rule_for(struct ast_rule_for *rule_for);

struct ast_rule_while *init_ast_rule_while();
void free_ast_rule_while(struct ast_rule_while *rule_while);

struct ast_rule_until *init_ast_rule_until();
void free_ast_rule_until(struct ast_rule_until *rule_until);

struct ast_rule_case *init_ast_rule_case();
void free_ast_rule_case(struct ast_rule_case *rule_case);

struct ast_rule_if *init_ast_rule_if();
void free_ast_rule_if(struct ast_rule_if *rule_if);

struct ast_else_clause *init_ast_else_clause();
void free_ast_else_clause(struct ast_else_clause *else_clause);

struct ast_case_item *init_ast_case_item();
void free_ast_case_item(struct ast_case_item *case_item);

struct ast_case_item_list *init_ast_case_item_list();
void free_ast_case_item_list(struct ast_case_item_list *case_item_list);

struct ast_case_clause *init_ast_case_clause();
void free_ast_case_clause(struct ast_case_clause *case_clause);

#endif // ! AST_H
