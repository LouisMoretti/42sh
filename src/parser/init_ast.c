#include <assert.h>
#include <stdlib.h>

#include "ast.h"

static struct ast *init_ast_input();
static struct ast *init_ast_list();
static struct ast *init_ast_and_or();
static struct ast *init_ast_pipeline();
static struct ast *init_ast_cmd();
static struct ast *init_ast_prefix();
static struct ast *init_ast_prefix_list();
static struct ast *init_ast_element();
static struct ast *init_ast_element_list();
static struct ast *init_ast_simple_cmd();
static struct ast *init_ast_shell_cmd();
static struct ast *init_ast_funcdec();
// static struct ast *init_ast_redirection();
static struct ast *init_ast_compound_list();
static struct ast *init_ast_word_list();
static struct ast *init_ast_rule_for();
static struct ast *init_ast_rule_while();
static struct ast *init_ast_rule_until();
static struct ast *init_ast_rule_case();
static struct ast *init_ast_rule_if();
static struct ast *init_ast_else_clause();
static struct ast *init_ast_case_item();
static struct ast *init_ast_case_item_list();
static struct ast *init_ast_case_clause();

typedef struct ast *(*fptr)();

// TODO: Add redirection init function
static fptr init_functions[] = { [AST_INPUT] = &init_ast_input,
                                 [AST_LIST] = &init_ast_list,
                                 [AST_AND_OR] = &init_ast_and_or,
                                 [AST_PIPELINE] = &init_ast_pipeline,
                                 [AST_CMD] = &init_ast_cmd,
                                 [AST_PREFIX] = &init_ast_prefix,
                                 [AST_PREFIX_LIST] = &init_ast_prefix_list,
                                 [AST_ELEMENT] = &init_ast_element,
                                 [AST_ELEMENT_LIST] = &init_ast_element_list,
                                 [AST_SIMPLE_CMD] = &init_ast_simple_cmd,
                                 [AST_SHELL_CMD] = &init_ast_shell_cmd,
                                 [AST_FUNCDEC] = &init_ast_funcdec,
                                 [AST_COMPOUND_LIST] = &init_ast_compound_list,
                                 [AST_WORD_LIST] = &init_ast_word_list,
                                 [AST_RULE_FOR] = &init_ast_rule_for,
                                 [AST_RULE_WHILE] = &init_ast_rule_while,
                                 [AST_RULE_UNTIL] = &init_ast_rule_until,
                                 [AST_RULE_CASE] = &init_ast_rule_case,
                                 [AST_RULE_IF] = &init_ast_rule_if,
                                 [AST_CLAUSE_ELSE] = &init_ast_else_clause,
                                 [AST_CLAUSE_CASE] = &init_ast_case_clause,
                                 [AST_CASE_ITEM] = &init_ast_case_item,
                                 [AST_CASE_ITEM_LIST] =
                                     &init_ast_case_item_list };

struct ast *init_ast(enum ast_type type)
{
    return (*init_functions[type])();
}

// ====================
//      AST INPUT
// ====================

static struct ast *init_ast_input()
{
    struct ast_input *input = malloc(sizeof(struct ast_input));
    assert(input != NULL);

    input->base.type = AST_INPUT;
    input->list = NULL;

    return (struct ast *)input;
}

// ====================
//      AST LIST
// ====================

static struct ast *init_ast_list()
{
    struct ast_list *list = malloc(sizeof(struct ast_list));
    assert(list != NULL);

    list->base.type = AST_LIST;
    list->and_or = NULL;
    list->next = NULL;

    return (struct ast *)list;
}

// ====================
//      AST AND OR
// ====================

static struct ast *init_ast_and_or()
{
    struct ast_and_or *and_or = malloc(sizeof(struct ast_and_or));
    assert(and_or != NULL);

    and_or->base.type = AST_AND_OR;
    and_or->pipeline = NULL;
    and_or->operand = AND; // Default is AND (not important)
    and_or->next = NULL;

    return (struct ast *)and_or;
}

// ====================
//      AST PIPELINE
// ====================

static struct ast *init_ast_pipeline()
{
    struct ast_pipeline *pipeline = malloc(sizeof(struct ast_pipeline));
    assert(pipeline != NULL);

    pipeline->base.type = AST_PIPELINE;
    pipeline->negation = 0;
    pipeline->cmd = NULL;
    pipeline->next = NULL;

    return (struct ast *)pipeline;
}

// ====================
//      AST CMD
// ====================

static struct ast *init_ast_cmd()
{
    struct ast_cmd *cmd = malloc(sizeof(struct ast_cmd));
    assert(cmd != NULL);

    cmd->base.type = AST_CMD;
    cmd->cmd = NULL;
    cmd->redirection = NULL;

    return (struct ast *)cmd;
}

// ====================
//      AST PREFIX
// ====================

static struct ast *init_ast_prefix()
{
    struct ast_prefix *prefix = malloc(sizeof(struct ast_prefix));
    assert(prefix != NULL);

    prefix->base.type = AST_PREFIX;
    prefix->assignment_word = NULL;
    prefix->redirection = NULL;

    return (struct ast *)prefix;
}

// ====================
//     AST PREFIX LIST
// ====================

static struct ast *init_ast_prefix_list()
{
    struct ast_prefix_list *list = malloc(sizeof(struct ast_prefix_list));
    assert(list != NULL);

    list->base.type = AST_PREFIX_LIST;
    list->prefix = NULL;
    list->next = NULL;

    return (struct ast *)list;
}

// ====================
//     AST ELEMENT
// ====================

static struct ast *init_ast_element()
{
    struct ast_element *element = malloc(sizeof(struct ast_element));
    assert(element != NULL);

    element->base.type = AST_ELEMENT;
    element->word = NULL;
    element->redirection = NULL;

    return (struct ast *)element;
}

// ====================
//      AST ELEMENT LIST
// ====================

static struct ast *init_ast_element_list()
{
    struct ast_element_list *list = malloc(sizeof(struct ast_element_list));
    assert(list != NULL);

    list->base.type = AST_ELEMENT_LIST;
    list->element = NULL;
    list->next = NULL;

    return (struct ast *)list;
}

// ====================
//      AST SIMPLE CMD
// ====================

static struct ast *init_ast_simple_cmd()
{
    struct ast_simple_cmd *cmd = malloc(sizeof(struct ast_simple_cmd));
    assert(cmd != NULL);

    cmd->base.type = AST_SIMPLE_CMD;
    cmd->prefix = NULL;
    cmd->prefix_list = NULL;
    cmd->word = NULL;
    cmd->element_list = NULL;

    return (struct ast *)cmd;
}

// ====================
//      AST SHELL CMD
// ====================

static struct ast *init_ast_shell_cmd()
{
    struct ast_shell_cmd *shell_cmd = malloc(sizeof(struct ast_shell_cmd));
    assert(shell_cmd != NULL);

    shell_cmd->base.type = AST_SHELL_CMD;
    shell_cmd->compound_list = NULL;
    shell_cmd->rule = NULL;

    return (struct ast *)shell_cmd;
}

// ====================
//      AST FUNCDEC
// ====================

static struct ast *init_ast_funcdec()
{
    struct ast_funcdec *funcdec = malloc(sizeof(struct ast_funcdec));
    assert(funcdec != NULL);

    funcdec->base.type = AST_FUNCDEC;
    funcdec->name = NULL;
    funcdec->shell_cmd = NULL;

    return (struct ast *)funcdec;
}

// // ====================
// //      AST REDIRECTION
// // ====================

// TODO: init_ast_redirection

// ====================
//      AST COMPOUND LIST
// ====================

static struct ast *init_ast_compound_list()
{
    struct ast_compound_list *compound_list =
        malloc(sizeof(struct ast_compound_list));
    assert(compound_list != NULL);

    compound_list->base.type = AST_COMPOUND_LIST;
    compound_list->ast_and_or = NULL;
    compound_list->next = NULL;

    return (struct ast *)compound_list;
}

// ====================
//      AST WORD LIST
// ====================

static struct ast *init_ast_word_list()
{
    struct ast_word_list *list = malloc(sizeof(struct ast_word_list));
    assert(list != NULL);

    list->base.type = AST_WORD_LIST;
    list->word = NULL;
    list->next = NULL;

    return (struct ast *)list;
}

// ====================
//      AST RULE FOR
// ====================

static struct ast *init_ast_rule_for()
{
    struct ast_rule_for *rule_for = malloc(sizeof(struct ast_rule_for));
    assert(rule_for != NULL);

    rule_for->base.type = AST_RULE_FOR;
    rule_for->condition_word = NULL;
    rule_for->in_word_list = NULL;
    rule_for->body_compound_list = NULL;

    return (struct ast *)rule_for;
}

// ====================
//     AST RULE WHILE
// ====================

static struct ast *init_ast_rule_while()
{
    struct ast_rule_while *rule_while = malloc(sizeof(struct ast_rule_while));
    assert(rule_while != NULL);

    rule_while->base.type = AST_RULE_WHILE;
    rule_while->condition_compound_list = NULL;
    rule_while->body_compound_list = NULL;

    return (struct ast *)rule_while;
}

// ====================
//     AST RULE UNTIL
// ====================

static struct ast *init_ast_rule_until()
{
    struct ast_rule_until *rule_until = malloc(sizeof(struct ast_rule_until));
    assert(rule_until != NULL);

    rule_until->base.type = AST_RULE_UNTIL;
    rule_until->condition_compound_list = NULL;
    rule_until->body_compound_list = NULL;

    return (struct ast *)rule_until;
}

// ====================
//     AST RULE CASE
// ====================

static struct ast *init_ast_rule_case()
{
    struct ast_rule_case *rule_case = malloc(sizeof(struct ast_rule_case));
    assert(rule_case != NULL);

    rule_case->base.type = AST_RULE_CASE;
    rule_case->word = NULL;
    rule_case->case_clause = NULL;

    return (struct ast *)rule_case;
}

// ====================
//      AST RULE IF
// ====================

static struct ast *init_ast_rule_if()
{
    struct ast_rule_if *rule_if = malloc(sizeof(struct ast_rule_if));
    assert(rule_if != NULL);

    rule_if->base.type = AST_RULE_IF;
    rule_if->condition_compound_list = NULL;
    rule_if->body_compound_list = NULL;
    rule_if->else_clause = NULL;

    return (struct ast *)rule_if;
}

// ====================
//      AST ELSE CLAUSE
// ====================

static struct ast *init_ast_else_clause()
{
    struct ast_else_clause *else_clause =
        malloc(sizeof(struct ast_else_clause));
    assert(else_clause != NULL);

    else_clause->base.type = AST_CLAUSE_ELSE;
    else_clause->condition_compound_list = NULL;
    else_clause->body_compound_list = NULL;
    else_clause->else_clause = NULL;

    return (struct ast *)else_clause;
}

// ====================
//      AST CASE ITEM
// ====================

static struct ast *init_ast_case_item()
{
    struct ast_case_item *case_item = malloc(sizeof(struct ast_case_item));
    assert(case_item != NULL);

    case_item->base.type = AST_CASE_ITEM;
    // case_item->word = NULL;
    case_item->word_list = NULL;
    case_item->compound_list = NULL;

    return (struct ast *)case_item;
}

// ====================
//      AST CASE ITEM LIST
// ====================

static struct ast *init_ast_case_item_list()
{
    struct ast_case_item_list *list = malloc(sizeof(struct ast_case_item_list));
    assert(list != NULL);

    list->base.type = AST_CASE_ITEM_LIST;
    list->case_item = NULL;
    list->next = NULL;

    return (struct ast *)list;
}

// ====================
//      AST CASE CLAUSE
// ====================

static struct ast *init_ast_case_clause()
{
    struct ast_case_clause *case_clause =
        malloc(sizeof(struct ast_case_clause));
    assert(case_clause != NULL);

    case_clause->base.type = AST_CLAUSE_CASE;
    case_clause->case_item_list = NULL;

    return (struct ast *)case_clause;
}

