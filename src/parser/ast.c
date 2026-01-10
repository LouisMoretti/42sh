#include "ast.h"

#include <assert.h>
#include <stdlib.h>

// ====================
//      AST INPUT
// ====================

struct ast_input *init_ast_input()
{
    struct ast_input *input = malloc(sizeof(struct ast_input));

    input->base.type = AST_INPUT;
    input->list = NULL;

    return input;
}

void free_ast_input(struct ast_input *input)
{
    if (input == NULL)
        return;

    if (input->list != NULL)
    {
        assert(input->list->type == AST_LIST);
        free_ast_list((struct ast_list *)input->list);
    }

    free(input);
}

// ====================
//      AST LIST
// ====================

struct ast_list *init_ast_list()
{
    struct ast_list *list = malloc(sizeof(struct ast_list));

    list->base.type = AST_LIST;
    list->and_or = NULL;
    list->next = NULL;

    return list;
}

void free_ast_list(struct ast_list *list)
{
    if (list == NULL)
        return;

    if (list->and_or != NULL)
    {
        assert(list->and_or->type == AST_AND_OR);
        free_ast_and_or((struct ast_and_or *)list->and_or);
    }

    if (list->next != NULL)
    {
        assert(list->next->type == AST_LIST);
        free_ast_list((struct ast_list *)list->next);
    }

    free(list);
}

// ====================
//      AST AND OR
// ====================

struct ast_and_or *init_ast_and_or()
{
    struct ast_and_or *and_or = malloc(sizeof(struct ast_and_or));

    and_or->base.type = AST_AND_OR;
    and_or->pipeline = NULL;
    and_or->operand = AND; // Default is AND (not important)
    and_or->next = NULL;

    return and_or;
}

void free_ast_and_or(struct ast_and_or *and_or)
{
    if (and_or == NULL)
        return;

    if (and_or->pipeline != NULL)
    {
        assert(and_or->pipeline->type == AST_PIPELINE);
        free_ast_pipeline((struct ast_pipeline *)and_or->pipeline);
    }

    if (and_or->next != NULL)
    {
        assert(and_or->next->type == AST_AND_OR);
        free_ast_and_or((struct ast_and_or *)and_or->next);
    }

    free(and_or);
}

// ====================
//      AST PIPELINE
// ====================

struct ast_pipeline *init_ast_pipeline()
{
    struct ast_pipeline *pipeline = malloc(sizeof(struct ast_pipeline));

    pipeline->base.type = AST_PIPELINE;
    pipeline->negation = 0;
    pipeline->cmd = NULL;
    pipeline->next = NULL;

    return pipeline;
}

void free_ast_pipeline(struct ast_pipeline *pipeline)
{
    if (pipeline == NULL)
        return;

    if (pipeline->cmd != NULL)
    {
        assert(pipeline->cmd->type == AST_CMD);
        free_ast_cmd((struct ast_cmd *)pipeline->cmd);
    }

    if (pipeline->next != NULL)
    {
        assert(pipeline->next->type == AST_PIPELINE);
        free_ast_pipeline((struct ast_pipeline *)pipeline->next);
    }

    free(pipeline);
}

// ====================
//      AST CMD
// ====================

struct ast_cmd *init_ast_cmd()
{
    struct ast_cmd *cmd = malloc(sizeof(struct ast_cmd));

    cmd->base.type = AST_CMD;
    cmd->cmd = NULL;
    cmd->redirection = NULL;

    return cmd;
}

void free_ast_cmd(struct ast_cmd *cmd)
{
    if (cmd == NULL)
        return;

    if (cmd->cmd != NULL)
    {
        assert(cmd->cmd->type == AST_SIMPLE_CMD
               || cmd->cmd->type == AST_SHELL_CMD
               || cmd->cmd->type == AST_FUNCDEC);

        if (cmd->cmd->type == AST_SIMPLE_CMD)
            free_ast_simple_cmd((struct ast_simple_cmd *)cmd->cmd);
        else if (cmd->cmd->type == AST_SHELL_CMD)
            free_ast_shell_cmd((struct ast_shell_cmd *)cmd->cmd);
        else if (cmd->cmd->type == AST_FUNCDEC)
            free_ast_funcdec((struct ast_funcdec *)cmd->cmd);
    }

    if (cmd->redirection != NULL)
    {
        assert(cmd->redirection->type == AST_REDIRECTION);
        // TODO: Not implemented yet (free_ast_redirection).
        assert(1 == 0);
    }

    free(cmd);
}

// ====================
//      AST PREFIX
// ====================

struct ast_prefix *init_ast_prefix()
{
    struct ast_prefix *prefix = malloc(sizeof(struct ast_prefix));

    prefix->base.type = AST_PREFIX;
    prefix->assignment_word = NULL;
    prefix->redirection = NULL;

    return prefix;
}

void free_ast_prefix(struct ast_prefix *prefix)
{
    if (prefix == NULL)
        return;

    if (prefix->assignment_word != NULL)
        free(prefix->assignment_word);

    if (prefix->redirection != NULL)
    {
        assert(prefix->redirection->type == AST_REDIRECTION);
        // TODO: Not implemented yet (free_ast_redirection).
        assert(1 == 0);
    }

    free(prefix);
}

// ====================
//     AST PREFIX LIST
// ====================

struct ast_prefix_list *init_ast_prefix_list()
{
    struct ast_prefix_list *list = malloc(sizeof(struct ast_prefix_list));

    list->base.type = AST_PREFIX_LIST;
    list->prefix = NULL;
    list->next = NULL;

    return list;
}

void free_ast_prefix_list(struct ast_prefix_list *list)
{
    if (list == NULL)
        return;

    if (list->prefix != NULL)
    {
        assert(list->prefix->type == AST_PREFIX);
        free_ast_prefix((struct ast_prefix *)list->prefix);
    }

    if (list->next != NULL)
    {
        assert(list->next->type == AST_PREFIX_LIST);
        free_ast_prefix_list((struct ast_prefix_list *)list->next);
    }

    free(list);
}

// ====================
//     AST ELEMENT
// ====================

struct ast_element *init_ast_element()
{
    struct ast_element *element = malloc(sizeof(struct ast_element));

    element->base.type = AST_ELEMENT;
    element->word = NULL;
    element->redirection = NULL;

    return element;
}

void free_ast_element(struct ast_element *element)
{
    if (element == NULL)
        return;

    if (element->word != NULL)
        free(element->word);

    if (element->redirection != NULL)
    {
        assert(element->redirection->type == AST_REDIRECTION);
        // TODO: Not implemented yet (free_ast_redirection).
        assert(1 == 0);
    }

    free(element);
}

// ====================
//      AST ELEMENT LIST
// ====================

struct ast_element_list *init_ast_element_list()
{
    struct ast_element_list *list = malloc(sizeof(struct ast_element_list));

    list->base.type = AST_ELEMENT_LIST;
    list->element = NULL;
    list->next = NULL;

    return list;
}

void free_ast_element_list(struct ast_element_list *list)
{
    if (list == NULL)
        return;

    if (list->element != NULL)
    {
        assert(list->element->type == AST_ELEMENT);
        free_ast_element((struct ast_element *)list->element);
    }

    if (list->next != NULL)
    {
        assert(list->next->type == AST_ELEMENT_LIST);
        free_ast_element_list((struct ast_element_list *)list->next);
    }

    free(list);
}

// ====================
//      AST SIMPLE CMD
// ====================

struct ast_simple_cmd *init_ast_simple_cmd()
{
    struct ast_simple_cmd *cmd = malloc(sizeof(struct ast_simple_cmd));

    cmd->base.type = AST_SIMPLE_CMD;
    cmd->prefix = NULL;
    cmd->prefix_list = NULL;
    cmd->word = NULL;
    cmd->element_list = NULL;

    return cmd;
}

void free_ast_simple_cmd(struct ast_simple_cmd *cmd)
{
    if (cmd == NULL)
        return;

    if (cmd->prefix != NULL)
    {
        assert(cmd->prefix->type == AST_PREFIX);
        free_ast_prefix((struct ast_prefix *)cmd->prefix);
    }

    if (cmd->prefix_list != NULL)
    {
        assert(cmd->prefix_list->type == AST_PREFIX_LIST);
        free_ast_prefix_list((struct ast_prefix_list *)cmd->prefix_list);
    }

    if (cmd->word != NULL)
        free(cmd->word);

    if (cmd->element_list != NULL)
    {
        assert(cmd->element_list->type == AST_ELEMENT_LIST);
        free_ast_element_list((struct ast_element_list *)cmd->element_list);
    }

    free(cmd);
}

// ====================
//      AST SHELL CMD
// ====================

struct ast_shell_cmd *init_ast_shell_cmd()
{
    struct ast_shell_cmd *shell_cmd = malloc(sizeof(struct ast_shell_cmd));

    shell_cmd->base.type = AST_SHELL_CMD;
    shell_cmd->compound_list = NULL;
    shell_cmd->rule = NULL;

    return shell_cmd;
}

void free_ast_shell_cmd(struct ast_shell_cmd *shell_cmd)
{
    if (shell_cmd == NULL)
        return;

    if (shell_cmd->compound_list != NULL)
    {
        assert(shell_cmd->compound_list->type == AST_COMPOUND_LIST);
        free_ast_compound_list(
            (struct ast_compound_list *)shell_cmd->compound_list);
    }

    if (shell_cmd->rule != NULL)
    {
        assert(shell_cmd->rule->type == AST_RULE_FOR
               || shell_cmd->rule->type == AST_RULE_WHILE
               || shell_cmd->rule->type == AST_RULE_UNTIL
               || shell_cmd->rule->type == AST_RULE_CASE
               || shell_cmd->rule->type == AST_RULE_IF);

        if (shell_cmd->rule->type == AST_RULE_FOR)
            free_ast_rule_for((struct ast_rule_for *)shell_cmd->rule);
        else if (shell_cmd->rule->type == AST_RULE_WHILE)
            free_ast_rule_while((struct ast_rule_while *)shell_cmd->rule);
        else if (shell_cmd->rule->type == AST_RULE_UNTIL)
            free_ast_rule_until((struct ast_rule_until *)shell_cmd->rule);
        else if (shell_cmd->rule->type == AST_RULE_CASE)
            free_ast_rule_case((struct ast_rule_case *)shell_cmd->rule);
        else if (shell_cmd->rule->type == AST_RULE_IF)
            free_ast_rule_if((struct ast_rule_if *)shell_cmd->rule);
    }

    free(shell_cmd);
}

// ====================
//      AST FUNCDEC
// ====================

struct ast_funcdec *init_ast_funcdec()
{
    struct ast_funcdec *funcdec = malloc(sizeof(struct ast_funcdec));

    funcdec->base.type = AST_FUNCDEC;
    funcdec->name = NULL;
    funcdec->shell_cmd = NULL;

    return funcdec;
}

void free_ast_funcdec(struct ast_funcdec *funcdec)
{
    if (funcdec == NULL)
        return;

    if (funcdec->name != NULL)
        free(funcdec->name);

    if (funcdec->shell_cmd != NULL)
    {
        assert(funcdec->shell_cmd->type == AST_SHELL_CMD);
        free_ast_shell_cmd((struct ast_shell_cmd *)funcdec->shell_cmd);
    }

    free(funcdec);
}

// // ====================
// //      AST REDIRECTION
// // ====================

// // ====================
// //      AND OR LIST
// // ====================

// struct and_or_list *init_and_or_list()
// {
//     struct and_or_list *list = malloc(sizeof(struct and_or_list));

//     list->ast_and_or.base.type = AST_AND_OR;
//     list->ast_and_or.pipeline = NULL;
//     list->ast_and_or.operand = AND; // Default is AND (not important)
//     list->ast_and_or.next = NULL;
//     list->next = NULL;

//     return list;
// }

// void free_and_or_list(struct and_or_list *list)
// {
//     if (list == NULL)
//         return;

//     struct ast_and_or *and_or = &list->ast_and_or;

//     if (and_or->pipeline != NULL)
//     {
//         assert(and_or->pipeline->type == AST_PIPELINE);
//         free_ast_pipeline((struct ast_pipeline *)and_or->pipeline);
//     }

//     if (and_or->next != NULL)
//     {
//         assert(and_or->next->type == AST_AND_OR);
//         free_ast_and_or((struct ast_and_or *)and_or->next);
//     }

//     if (list->next != NULL)
//         free_and_or_list((struct and_or_list *)list->next);

//     free(list);
// }

// ====================
//      AST COMPOUND LIST
// ====================

struct ast_compound_list *init_ast_compound_list()
{
    struct ast_compound_list *compound_list =
        malloc(sizeof(struct ast_compound_list));

    compound_list->base.type = AST_COMPOUND_LIST;
    compound_list->ast_and_or = NULL;
    compound_list->next = NULL;

    return compound_list;
}

void free_ast_compound_list(struct ast_compound_list *compound_list)
{
    if (compound_list == NULL)
        return;

    if (compound_list->ast_and_or != NULL)
    {
        assert(compound_list->ast_and_or->type == AST_AND_OR);
        free_ast_and_or((struct ast_and_or *)compound_list->ast_and_or);
    }

    if (compound_list->next != NULL)
    {
        assert(compound_list->next->type == AST_COMPOUND_LIST);
        free_ast_compound_list((struct ast_compound_list *)compound_list->next);
    }

    free(compound_list);
}

// ====================
//      AST WORD LIST
// ====================

struct ast_word_list *init_ast_word_list()
{
    struct ast_word_list *list = malloc(sizeof(struct ast_word_list));

    list->base.type = AST_WORD_LIST;
    list->word = NULL;
    list->next = NULL;

    return list;
}

void free_ast_word_list(struct ast_word_list *list)
{
    if (list == NULL)
        return;

    if (list->word != NULL)
        free(list->word);

    if (list->next != NULL)
    {
        assert(list->next->type == AST_WORD_LIST);
        free_ast_word_list((struct ast_word_list *)list->next);
    }

    free(list);
}

// ====================
//      AST RULE FOR
// ====================

struct ast_rule_for *init_ast_rule_for()
{
    struct ast_rule_for *rule_for = malloc(sizeof(struct ast_rule_for));

    rule_for->base.type = AST_RULE_FOR;
    rule_for->condition_word = NULL;
    rule_for->in_word_list = NULL;
    rule_for->body_compound_list = NULL;

    return rule_for;
}

void free_ast_rule_for(struct ast_rule_for *rule_for)
{
    if (rule_for == NULL)
        return;

    if (rule_for->condition_word != NULL)
        free(rule_for->condition_word);

    if (rule_for->in_word_list != NULL)
    {
        assert(rule_for->in_word_list->type == AST_WORD_LIST);
        free_ast_word_list((struct ast_word_list *)rule_for->in_word_list);
    }

    if (rule_for->body_compound_list != NULL)
    {
        assert(rule_for->body_compound_list->type == AST_COMPOUND_LIST);
        free_ast_compound_list(
            (struct ast_compound_list *)rule_for->body_compound_list);
    }

    free(rule_for);
}

// ====================
//     AST RULE WHILE
// ====================

struct ast_rule_while *init_ast_rule_while()
{
    struct ast_rule_while *rule_while = malloc(sizeof(struct ast_rule_while));

    rule_while->base.type = AST_RULE_WHILE;
    rule_while->condition_compound_list = NULL;
    rule_while->body_compound_list = NULL;

    return rule_while;
}

void free_ast_rule_while(struct ast_rule_while *rule_while)
{
    if (rule_while == NULL)
        return;

    if (rule_while->condition_compound_list != NULL)
    {
        assert(rule_while->condition_compound_list->type == AST_COMPOUND_LIST);
        free_ast_compound_list(
            (struct ast_compound_list *)rule_while->condition_compound_list);
    }

    if (rule_while->body_compound_list != NULL)
    {
        assert(rule_while->body_compound_list->type == AST_COMPOUND_LIST);
        free_ast_compound_list(
            (struct ast_compound_list *)rule_while->body_compound_list);
    }

    free(rule_while);
}

// ====================
//     AST RULE UNTIL
// ====================

struct ast_rule_until *init_ast_rule_until()
{
    struct ast_rule_until *rule_until = malloc(sizeof(struct ast_rule_until));

    rule_until->base.type = AST_RULE_UNTIL;
    rule_until->condition_compound_list = NULL;
    rule_until->body_compound_list = NULL;

    return rule_until;
}

void free_ast_rule_until(struct ast_rule_until *rule_until)
{
    if (rule_until == NULL)
        return;

    if (rule_until->condition_compound_list != NULL)
    {
        assert(rule_until->condition_compound_list->type == AST_COMPOUND_LIST);
        free_ast_compound_list(
            (struct ast_compound_list *)rule_until->condition_compound_list);
    }

    if (rule_until->body_compound_list != NULL)
    {
        assert(rule_until->body_compound_list->type == AST_COMPOUND_LIST);
        free_ast_compound_list(
            (struct ast_compound_list *)rule_until->body_compound_list);
    }

    free(rule_until);
}

// ====================
//     AST RULE CASE
// ====================

struct ast_rule_case *init_ast_rule_case()
{
    struct ast_rule_case *rule_case = malloc(sizeof(struct ast_rule_case));

    rule_case->base.type = AST_RULE_CASE;
    rule_case->word = NULL;
    rule_case->case_clause = NULL;

    return rule_case;
}

void free_ast_rule_case(struct ast_rule_case *rule_case)
{
    if (rule_case == NULL)
        return;

    if (rule_case->word != NULL)
        free(rule_case->word);

    if (rule_case->case_clause != NULL)
    {
        assert(rule_case->case_clause->type == AST_CLAUSE_CASE);
        free_ast_case_clause((struct ast_case_clause *)rule_case->case_clause);
    }

    free(rule_case);
}

// ====================
//      AST RULE IF
// ====================

struct ast_rule_if *init_ast_rule_if()
{
    struct ast_rule_if *rule_if = malloc(sizeof(struct ast_rule_if));

    rule_if->base.type = AST_RULE_IF;
    rule_if->condition_compound_list = NULL;
    rule_if->body_compound_list = NULL;
    rule_if->else_clause = NULL;

    return rule_if;
}

void free_ast_rule_if(struct ast_rule_if *rule_if)
{
    if (rule_if == NULL)
        return;

    if (rule_if->condition_compound_list != NULL)
    {
        assert(rule_if->condition_compound_list->type == AST_COMPOUND_LIST);
        free_ast_compound_list(
            (struct ast_compound_list *)rule_if->condition_compound_list);
    }

    if (rule_if->body_compound_list != NULL)
    {
        assert(rule_if->body_compound_list->type == AST_COMPOUND_LIST);
        free_ast_compound_list(
            (struct ast_compound_list *)rule_if->body_compound_list);
    }

    if (rule_if->else_clause != NULL)
    {
        assert(rule_if->else_clause->type == AST_CLAUSE_ELSE);
        free_ast_else_clause((struct ast_else_clause *)rule_if->else_clause);
    }

    free(rule_if);
}

// ====================
//      AST ELSE CLAUSE
// ====================

struct ast_else_clause *init_ast_else_clause()
{
    struct ast_else_clause *else_clause =
        malloc(sizeof(struct ast_else_clause));

    else_clause->base.type = AST_CLAUSE_ELSE;
    else_clause->condition_compound_list = NULL;
    else_clause->body_compound_list = NULL;
    else_clause->else_clause = NULL;

    return else_clause;
}

void free_ast_else_clause(struct ast_else_clause *else_clause)
{
    if (else_clause == NULL)
        return;

    if (else_clause->condition_compound_list != NULL)
    {
        assert(else_clause->condition_compound_list->type == AST_COMPOUND_LIST);
        free_ast_compound_list(
            (struct ast_compound_list *)else_clause->condition_compound_list);
    }

    if (else_clause->body_compound_list != NULL)
    {
        assert(else_clause->body_compound_list->type == AST_COMPOUND_LIST);
        free_ast_compound_list(
            (struct ast_compound_list *)else_clause->body_compound_list);
    }

    if (else_clause->else_clause != NULL)
    {
        assert(else_clause->else_clause->type == AST_CLAUSE_ELSE);
        free_ast_else_clause(
            (struct ast_else_clause *)else_clause->else_clause);
    }

    free(else_clause);
}

// ====================
//      AST CASE ITEM
// ====================

struct ast_case_item *init_ast_case_item()
{
    struct ast_case_item *case_item = malloc(sizeof(struct ast_case_item));

    case_item->base.type = AST_CASE_ITEM;
    // case_item->word = NULL;
    case_item->word_list = NULL;
    case_item->compound_list = NULL;

    return case_item;
}

void free_ast_case_item(struct ast_case_item *case_item)
{
    if (case_item == NULL)
        return;

    // if (case_item->word != NULL)
    //     free(case_item->word);

    if (case_item->word_list != NULL)
    {
        assert(case_item->word_list->type == AST_WORD_LIST);
        free_ast_word_list((struct ast_word_list *)case_item->word_list);
    }

    if (case_item->compound_list != NULL)
    {
        assert(case_item->compound_list->type == AST_COMPOUND_LIST);
        free_ast_compound_list(
            (struct ast_compound_list *)case_item->compound_list);
    }

    free(case_item);
}

// ====================
//      AST CASE ITEM LIST
// ====================

struct ast_case_item_list *init_ast_case_item_list()
{
    struct ast_case_item_list *list = malloc(sizeof(struct ast_case_item_list));

    list->base.type = AST_CASE_ITEM_LIST;
    list->case_item = NULL;
    list->next = NULL;

    return list;
}

void free_ast_case_item_list(struct ast_case_item_list *list)
{
    if (list == NULL)
        return;

    if (list->case_item != NULL)
    {
        assert(list->case_item->type == AST_CASE_ITEM);
        free_ast_case_item((struct ast_case_item *)list->case_item);
    }

    if (list->next != NULL)
    {
        assert(list->next->type == AST_CASE_ITEM_LIST);
        free_ast_case_item_list((struct ast_case_item_list *)list->next);
    }

    free(list);
}

// ====================
//      AST CASE CLAUSE
// ====================

struct ast_case_clause *init_ast_case_clause()
{
    struct ast_case_clause *case_clause =
        malloc(sizeof(struct ast_case_clause));

    case_clause->base.type = AST_CLAUSE_CASE;
    case_clause->case_item_list = NULL;

    return case_clause;
}

void free_ast_case_clause(struct ast_case_clause *case_clause)
{
    if (case_clause == NULL)
        return;

    if (case_clause->case_item_list != NULL)
    {
        assert(case_clause->case_item_list->type == AST_CASE_ITEM_LIST);
        free_ast_case_item_list(
            (struct ast_case_item_list *)case_clause->case_item_list);
    }

    free(case_clause);
}
