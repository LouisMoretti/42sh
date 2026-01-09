#include <assert.h>
#include <stdlib.h>

#include "ast.h"

void free_ast_input(struct ast *ast);
static void free_ast_list(struct ast *ast);
static void free_ast_and_or(struct ast *ast);
static void free_ast_pipeline(struct ast *ast);
static void free_ast_cmd(struct ast *ast);
static void free_ast_prefix(struct ast *ast);
static void free_ast_prefix_list(struct ast *ast);
static void free_ast_element(struct ast *ast);
static void free_ast_element_list(struct ast *ast);
static void free_ast_simple_cmd(struct ast *ast);
static void free_ast_shell_cmd(struct ast *ast);
static void free_ast_funcdec(struct ast *ast);
// static void free_ast_redirection(struct ast *ast);
static void free_ast_compound_list(struct ast *ast);
static void free_ast_word_list(struct ast *ast);
static void free_ast_rule_for(struct ast *ast);
static void free_ast_rule_while(struct ast *ast);
static void free_ast_rule_until(struct ast *ast);
static void free_ast_rule_case(struct ast *ast);
static void free_ast_rule_if(struct ast *ast);
static void free_ast_else_clause(struct ast *ast);
static void free_ast_case_item(struct ast *ast);
static void free_ast_case_item_list(struct ast *ast);
static void free_ast_case_clause(struct ast *ast);

// ====================
//      AST INPUT
// ====================

void free_ast_input(struct ast *ast)
{
    struct ast_input *input = (struct ast_input *)ast;
    if (input == NULL)
        return;

    if (input->list != NULL)
    {
        assert(input->list->type == AST_LIST);
        free_ast_list(input->list);
    }

    free(input);
}

// ====================
//      AST LIST
// ====================

static void free_ast_list(struct ast *ast)
{
    struct ast_list *list = (struct ast_list *)ast;
    if (list == NULL)
        return;

    if (list->and_or != NULL)
    {
        assert(list->and_or->type == AST_AND_OR);
        free_ast_and_or(list->and_or);
    }

    if (list->next != NULL)
    {
        assert(list->next->type == AST_LIST);
        free_ast_list(list->next);
    }

    free(list);
}

// ====================
//      AST AND OR
// ====================

static void free_ast_and_or(struct ast *ast)
{
    struct ast_and_or *and_or = (struct ast_and_or *)ast;
    if (and_or == NULL)
        return;

    if (and_or->pipeline != NULL)
    {
        assert(and_or->pipeline->type == AST_PIPELINE);
        free_ast_pipeline(and_or->pipeline);
    }

    if (and_or->next != NULL)
    {
        assert(and_or->next->type == AST_AND_OR);
        free_ast_and_or(and_or->next);
    }

    free(and_or);
}

// ====================
//      AST PIPELINE
// ====================

static void free_ast_pipeline(struct ast *ast)
{
    struct ast_pipeline *pipeline = (struct ast_pipeline *)ast;
    if (pipeline == NULL)
        return;

    if (pipeline->cmd != NULL)
    {
        assert(pipeline->cmd->type == AST_CMD);
        free_ast_cmd(pipeline->cmd);
    }

    if (pipeline->next != NULL)
    {
        assert(pipeline->next->type == AST_PIPELINE);
        free_ast_pipeline(pipeline->next);
    }

    free(pipeline);
}

// ====================
//      AST CMD
// ====================

static void free_ast_cmd(struct ast *ast)
{
    struct ast_cmd *cmd = (struct ast_cmd *)ast;
    if (cmd == NULL)
        return;

    if (cmd->cmd != NULL)
    {
        assert(cmd->cmd->type == AST_SIMPLE_CMD
               || cmd->cmd->type == AST_SHELL_CMD
               || cmd->cmd->type == AST_FUNCDEC);

        if (cmd->cmd->type == AST_SIMPLE_CMD)
            free_ast_simple_cmd(cmd->cmd);
        else if (cmd->cmd->type == AST_SHELL_CMD)
            free_ast_shell_cmd(cmd->cmd);
        else if (cmd->cmd->type == AST_FUNCDEC)
            free_ast_funcdec(cmd->cmd);
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

static void free_ast_prefix(struct ast *ast)
{
    struct ast_prefix *prefix = (struct ast_prefix *)ast;
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

static void free_ast_prefix_list(struct ast *ast)
{
    struct ast_prefix_list *list = (struct ast_prefix_list *)ast;
    if (list == NULL)
        return;

    if (list->prefix != NULL)
    {
        assert(list->prefix->type == AST_PREFIX);
        free_ast_prefix(list->prefix);
    }

    if (list->next != NULL)
    {
        assert(list->next->type == AST_PREFIX_LIST);
        free_ast_prefix_list(list->next);
    }

    free(list);
}

// ====================
//     AST ELEMENT
// ====================

static void free_ast_element(struct ast *ast)
{
    if (ast == NULL)
        return;

    struct ast_element *element = (struct ast_element *)ast;

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

static void free_ast_element_list(struct ast *ast)
{
    if (ast == NULL)
        return;

    struct ast_element_list *list = (struct ast_element_list *)ast;

    if (list->element != NULL)
    {
        assert(list->element->type == AST_ELEMENT);
        free_ast_element(list->element);
    }

    if (list->next != NULL)
    {
        assert(list->next->type == AST_ELEMENT_LIST);
        free_ast_element_list(list->next);
    }

    free(list);
}

// ====================
//      AST SIMPLE CMD
// ====================

static void free_ast_simple_cmd(struct ast *ast)
{
    if (ast == NULL)
        return;

    struct ast_simple_cmd *cmd = (struct ast_simple_cmd *)ast;

    if (cmd->prefix != NULL)
    {
        assert(cmd->prefix->type == AST_PREFIX);
        free_ast_prefix(cmd->prefix);
    }

    if (cmd->prefix_list != NULL)
    {
        assert(cmd->prefix_list->type == AST_PREFIX_LIST);
        free_ast_prefix_list(cmd->prefix_list);
    }

    if (cmd->word != NULL)
        free(cmd->word);

    if (cmd->element_list != NULL)
    {
        assert(cmd->element_list->type == AST_ELEMENT_LIST);
        free_ast_element_list(cmd->element_list);
    }

    free(cmd);
}

// ====================
//      AST SHELL CMD
// ====================

static void free_ast_shell_cmd(struct ast *ast)
{
    if (ast == NULL)
        return;

    struct ast_shell_cmd *shell_cmd = (struct ast_shell_cmd *)ast;

    if (shell_cmd->compound_list != NULL)
    {
        assert(shell_cmd->compound_list->type == AST_COMPOUND_LIST);
        free_ast_compound_list(shell_cmd->compound_list);
    }

    if (shell_cmd->rule != NULL)
    {
        assert(shell_cmd->rule->type == AST_RULE_FOR
               || shell_cmd->rule->type == AST_RULE_WHILE
               || shell_cmd->rule->type == AST_RULE_UNTIL
               || shell_cmd->rule->type == AST_RULE_CASE
               || shell_cmd->rule->type == AST_RULE_IF);

        if (shell_cmd->rule->type == AST_RULE_FOR)
            free_ast_rule_for(shell_cmd->rule);
        else if (shell_cmd->rule->type == AST_RULE_WHILE)
            free_ast_rule_while(shell_cmd->rule);
        else if (shell_cmd->rule->type == AST_RULE_UNTIL)
            free_ast_rule_until(shell_cmd->rule);
        else if (shell_cmd->rule->type == AST_RULE_CASE)
            free_ast_rule_case(shell_cmd->rule);
        else if (shell_cmd->rule->type == AST_RULE_IF)
            free_ast_rule_if(shell_cmd->rule);
    }

    free(shell_cmd);
}

// ====================
//      AST FUNCDEC
// ====================

static void free_ast_funcdec(struct ast *ast)
{
    if (ast == NULL)
        return;

    struct ast_funcdec *funcdec = (struct ast_funcdec *)ast;

    if (funcdec->name != NULL)
        free(funcdec->name);

    if (funcdec->shell_cmd != NULL)
    {
        assert(funcdec->shell_cmd->type == AST_SHELL_CMD);
        free_ast_shell_cmd(funcdec->shell_cmd);
    }

    free(funcdec);
}

// // ====================
// //      AST REDIRECTION
// // ====================

// // ====================
// //      AND OR LIST
// // ====================

// static void free_and_or_list(struct and_or_list *list)
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

static void free_ast_compound_list(struct ast *ast)
{
    if (ast == NULL)
        return;

    struct ast_compound_list *compound_list = (struct ast_compound_list *)ast;

    if (compound_list->ast_and_or != NULL)
    {
        assert(compound_list->ast_and_or->type == AST_AND_OR);
        free_ast_and_or(compound_list->ast_and_or);
    }

    if (compound_list->next != NULL)
    {
        assert(compound_list->next->type == AST_COMPOUND_LIST);
        free_ast_compound_list(compound_list->next);
    }

    free(compound_list);
}

// ====================
//      AST WORD LIST
// ====================

static void free_ast_word_list(struct ast *ast)
{
    if (ast == NULL)
        return;

    struct ast_word_list *list = (struct ast_word_list *)ast;

    if (list->word != NULL)
        free(list->word);

    if (list->next != NULL)
    {
        assert(list->next->type == AST_WORD_LIST);
        free_ast_word_list(list->next);
    }

    free(list);
}

// ====================
//      AST RULE FOR
// ====================

static void free_ast_rule_for(struct ast *ast)
{
    if (ast == NULL)
        return;

    struct ast_rule_for *rule_for = (struct ast_rule_for *)ast;

    if (rule_for->condition_word != NULL)
        free(rule_for->condition_word);

    if (rule_for->in_word_list != NULL)
    {
        assert(rule_for->in_word_list->type == AST_WORD_LIST);
        free_ast_word_list(rule_for->in_word_list);
    }

    if (rule_for->body_compound_list != NULL)
    {
        assert(rule_for->body_compound_list->type == AST_COMPOUND_LIST);
        free_ast_compound_list(rule_for->body_compound_list);
    }

    free(rule_for);
}

// ====================
//     AST RULE WHILE
// ====================

static void free_ast_rule_while(struct ast *ast)
{
    if (ast == NULL)
        return;

    struct ast_rule_while *rule_while = (struct ast_rule_while *)ast;

    if (rule_while->condition_compound_list != NULL)
    {
        assert(rule_while->condition_compound_list->type == AST_COMPOUND_LIST);
        free_ast_compound_list(rule_while->condition_compound_list);
    }

    if (rule_while->body_compound_list != NULL)
    {
        assert(rule_while->body_compound_list->type == AST_COMPOUND_LIST);
        free_ast_compound_list(rule_while->body_compound_list);
    }

    free(rule_while);
}

// ====================
//     AST RULE UNTIL
// ====================

static void free_ast_rule_until(struct ast *ast)
{
    if (ast == NULL)
        return;

    struct ast_rule_until *rule_until = (struct ast_rule_until *)ast;

    if (rule_until->condition_compound_list != NULL)
    {
        assert(rule_until->condition_compound_list->type == AST_COMPOUND_LIST);
        free_ast_compound_list(rule_until->condition_compound_list);
    }

    if (rule_until->body_compound_list != NULL)
    {
        assert(rule_until->body_compound_list->type == AST_COMPOUND_LIST);
        free_ast_compound_list(rule_until->body_compound_list);
    }

    free(rule_until);
}

// ====================
//     AST RULE CASE
// ====================

static void free_ast_rule_case(struct ast *ast)
{
    if (ast == NULL)
        return;

    struct ast_rule_case *rule_case = (struct ast_rule_case *)ast;

    if (rule_case->word != NULL)
        free(rule_case->word);

    if (rule_case->case_clause != NULL)
    {
        assert(rule_case->case_clause->type == AST_CLAUSE_CASE);
        free_ast_case_clause(rule_case->case_clause);
    }

    free(rule_case);
}

// ====================
//      AST RULE IF
// ====================

static void free_ast_rule_if(struct ast *ast)
{
    if (ast == NULL)
        return;

    struct ast_rule_if *rule_if = (struct ast_rule_if *)ast;

    if (rule_if->condition_compound_list != NULL)
    {
        assert(rule_if->condition_compound_list->type == AST_COMPOUND_LIST);
        free_ast_compound_list(rule_if->condition_compound_list);
    }

    if (rule_if->body_compound_list != NULL)
    {
        assert(rule_if->body_compound_list->type == AST_COMPOUND_LIST);
        free_ast_compound_list(rule_if->body_compound_list);
    }

    if (rule_if->else_clause != NULL)
    {
        assert(rule_if->else_clause->type == AST_CLAUSE_ELSE);
        free_ast_else_clause(rule_if->else_clause);
    }

    free(rule_if);
}

// ====================
//      AST ELSE CLAUSE
// ====================

static void free_ast_else_clause(struct ast *ast)
{
    if (ast == NULL)
        return;

    struct ast_else_clause *else_clause = (struct ast_else_clause *)ast;

    if (else_clause->condition_compound_list != NULL)
    {
        assert(else_clause->condition_compound_list->type == AST_COMPOUND_LIST);
        free_ast_compound_list(else_clause->condition_compound_list);
    }

    if (else_clause->body_compound_list != NULL)
    {
        assert(else_clause->body_compound_list->type == AST_COMPOUND_LIST);
        free_ast_compound_list(else_clause->body_compound_list);
    }

    if (else_clause->else_clause != NULL)
    {
        assert(else_clause->else_clause->type == AST_CLAUSE_ELSE);
        free_ast_else_clause(else_clause->else_clause);
    }

    free(else_clause);
}

// ====================
//      AST CASE ITEM
// ====================

static void free_ast_case_item(struct ast *ast)
{
    if (ast == NULL)
        return;

    struct ast_case_item *case_item = (struct ast_case_item *)ast;

    // if (case_item->word != NULL)
    //     free(case_item->word);

    if (case_item->word_list != NULL)
    {
        assert(case_item->word_list->type == AST_WORD_LIST);
        free_ast_word_list(case_item->word_list);
    }

    if (case_item->compound_list != NULL)
    {
        assert(case_item->compound_list->type == AST_COMPOUND_LIST);
        free_ast_compound_list(case_item->compound_list);
    }

    free(case_item);
}

// ====================
//      AST CASE ITEM LIST
// ====================

static void free_ast_case_item_list(struct ast *ast)
{
    if (ast == NULL)
        return;

    struct ast_case_item_list *list = (struct ast_case_item_list *)ast;

    if (list->case_item != NULL)
    {
        assert(list->case_item->type == AST_CASE_ITEM);
        free_ast_case_item(list->case_item);
    }

    if (list->next != NULL)
    {
        assert(list->next->type == AST_CASE_ITEM_LIST);
        free_ast_case_item_list(list->next);
    }

    free(list);
}

// ====================
//      AST CASE CLAUSE
// ====================

static void free_ast_case_clause(struct ast *ast)
{
    if (ast == NULL)
        return;

    struct ast_case_clause *case_clause = (struct ast_case_clause *)ast;

    if (case_clause->case_item_list != NULL)
    {
        assert(case_clause->case_item_list->type == AST_CASE_ITEM_LIST);
        free_ast_case_item_list(case_clause->case_item_list);
    }

    free(case_clause);
}
