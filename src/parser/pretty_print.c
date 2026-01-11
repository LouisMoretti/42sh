#define _POSIX_C_SOURCE 200809L

#include "parser/pretty_print.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/ast.h"

static void pp_input(struct ast *ast);
static void pp_list(struct ast *ast);
static void pp_and_or(struct ast *ast);
static void pp_pipeline(struct ast *ast);
static void pp_cmd(struct ast *ast);
static void pp_simple_cmd(struct ast *ast);
static void pp_shell_cmd(struct ast *ast);
static void pp_funcdec(struct ast *ast);
// static void pp_redirection(struct ast *ast);
static void pp_prefix(struct ast *ast);
static void pp_prefix_list(struct ast *ast);
static void pp_element(struct ast *ast);
static void pp_element_list(struct ast *ast);
static void pp_compound_list(struct ast *ast);
static void pp_word_list(struct ast *ast);
static void pp_rule_for(struct ast *ast);
static void pp_rule_while(struct ast *ast);
static void pp_rule_until(struct ast *ast);
static void pp_rule_case(struct ast *ast);
static void pp_rule_if(struct ast *ast);
static void pp_else_clause(struct ast *ast);
static void pp_case_clause(struct ast *ast);
static void pp_case_item(struct ast *ast);
static void pp_case_item_list(struct ast *ast);

typedef void (*fptr)(struct ast *);

static fptr pp_functions[] = { [AST_INPUT] = &pp_input,
                               [AST_LIST] = &pp_list,
                               [AST_AND_OR] = &pp_and_or,
                               [AST_PIPELINE] = &pp_pipeline,
                               [AST_CMD] = &pp_cmd,
                               [AST_SIMPLE_CMD] = &pp_simple_cmd,
                               [AST_SHELL_CMD] = &pp_shell_cmd,
                               [AST_FUNCDEC] = &pp_funcdec,
                               // [AST_REDIRECTION] = &pp_redirection,
                               [AST_PREFIX] = &pp_prefix,
                               [AST_PREFIX_LIST] = &pp_prefix_list,
                               [AST_ELEMENT] = &pp_element,
                               [AST_ELEMENT_LIST] = &pp_element_list,
                               [AST_COMPOUND_LIST] = &pp_compound_list,
                               [AST_WORD_LIST] = &pp_word_list,
                               [AST_RULE_FOR] = &pp_rule_for,
                               [AST_RULE_WHILE] = &pp_rule_while,
                               [AST_RULE_UNTIL] = &pp_rule_until,
                               [AST_RULE_CASE] = &pp_rule_case,
                               [AST_RULE_IF] = &pp_rule_if,
                               [AST_CLAUSE_ELSE] = &pp_else_clause,
                               [AST_CLAUSE_CASE] = &pp_case_clause,
                               [AST_CASE_ITEM] = &pp_case_item,
                               [AST_CASE_ITEM_LIST] = &pp_case_item_list };

void pretty_print(struct ast *ast)
{
    (*pp_functions[ast->type])(ast);
}

static void pp_prefix(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_PREFIX);
    struct ast_prefix *pref = (struct ast_prefix *)ast;

    if (pref->assignment_word != NULL)
    {
        printf("%s", pref->assignment_word);
    }
    else
    {
        // TODO: add pp for the redirection part
        assert(1 == 0);
    }
}

static void pp_element(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_ELEMENT);
    struct ast_element *elm = (struct ast_element *)ast;

    if (elm->word != NULL)
    {
        printf("%s", elm->word);
    }
    else
    {
        // TODO: add pp for the redirection part
        assert(1 == 0);
    }
}

static void pp_prefix_list(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_PREFIX_LIST);
    struct ast_prefix_list *ast_pref_list = (struct ast_prefix_list *)ast;

    if (ast_pref_list != NULL)
    {
        pp_prefix(ast_pref_list->prefix);
        ast_pref_list = (struct ast_prefix_list *)ast_pref_list->next;
    }

    while (ast_pref_list != NULL)
    {
        printf(" ");
        pp_prefix(ast_pref_list->prefix);
        ast_pref_list = (struct ast_prefix_list *)ast_pref_list->next;
    }

    printf("\n");
}

static void pp_element_list(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_ELEMENT_LIST);
    struct ast_element_list *ast_element_list = (struct ast_element_list *)ast;

    if (ast_element_list != NULL)
    {
        pp_element(ast_element_list->element);
        ast_element_list = (struct ast_element_list *)ast_element_list->next;
    }

    while (ast_element_list != NULL)
    {
        printf(" ");
        pp_element(ast_element_list->element);
        ast_element_list = (struct ast_element_list *)ast_element_list->next;
    }

    printf("\n");
}

static void pp_simple_cmd(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_SIMPLE_CMD);
    struct ast_simple_cmd *ast_simple_cmd = (struct ast_simple_cmd *)ast;

    if (ast_simple_cmd->prefix != NULL)
    {
        pp_prefix(ast_simple_cmd->prefix);
        printf(" ");
        if (ast_simple_cmd->prefix_list != NULL)
            pp_prefix_list(ast_simple_cmd->prefix_list);
    }
    else
    {
        if (ast_simple_cmd->prefix_list != NULL)
            pp_prefix_list(ast_simple_cmd->prefix_list);
        printf("%s", ast_simple_cmd->word);
        if (ast_simple_cmd->element_list != NULL)
            pp_element_list(ast_simple_cmd->element_list);
    }
}

static void pp_compound_list(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_AND_OR);
    struct ast_compound_list *ast_compound_list = (struct ast_compound_list *)ast;

    pp_and_or(ast_compound_list->ast_and_or);
    printf("\n");
    ast_compound_list = (struct ast_compound_list *)ast_compound_list->next;

    while (ast_compound_list != NULL)
    {
        printf("\n");
        pp_and_or(ast_compound_list->ast_and_or);
        ast_compound_list = (struct ast_compound_list *)ast_compound_list->next;
    }

    printf("\n");
}

static void pp_word_list(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_WORD_LIST);
    struct ast_word_list *ast_word_list = (struct ast_word_list *)ast;

    if (ast_word_list != NULL)
    {
        printf("%s", ast_word_list->word);
        ast_word_list = (struct ast_word_list *)ast_word_list->next;
    }

    while (ast_word_list != NULL)
    {
        printf(" %s", ast_word_list->word);
        ast_word_list = (struct ast_word_list *)ast_word_list->next;
    }
}

static void pp_rule_for(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_RULE_FOR);
    struct ast_rule_for *ast_rule_for = (struct ast_rule_for *)ast;

    printf("for %s", ast_rule_for->condition_word);

    if (ast_rule_for->in_word_list != NULL)
    {
        printf("in {\n");
        pp_word_list(ast_rule_for->in_word_list);
        printf("}");
    }

    printf(" do {\n");
    pp_compound_list(ast_rule_for->body_compound_list);
    printf("} done\n");
}

static void pp_rule_while(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_RULE_WHILE);
    struct ast_rule_while *ast_rule_while = (struct ast_rule_while *)ast;

    printf("while {\n");
    pp_compound_list(ast_rule_while->condition_compound_list);
    printf("}\ndo {\n");
    pp_compound_list(ast_rule_while->body_compound_list);
    printf("} done\n");
}

static void pp_rule_until(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_RULE_UNTIL);
    struct ast_rule_until *ast_rule_until = (struct ast_rule_until *)ast;

    printf("until {\n");
    pp_compound_list(ast_rule_until->condition_compound_list);
    printf("}\ndo {\n");
    pp_compound_list(ast_rule_until->body_compound_list);
    printf("} done\n");
}

static void pp_else_clause(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_CLAUSE_ELSE);
    struct ast_else_clause *ast_else_clause = (struct ast_else_clause *)ast;

    if (ast_else_clause->body_compound_list == NULL)
    {
        printf("else {\n");
        pp_compound_list(ast_else_clause->condition_compound_list);
        printf("}\n");
    }
    else
    {
        printf("elif {\n");
        pp_compound_list(ast_else_clause->condition_compound_list);
        printf("}\nthen {\n}");
        pp_compound_list(ast_else_clause->body_compound_list);
        printf("}\n");
        if (ast_else_clause->else_clause != NULL)
            pp_else_clause(ast_else_clause->else_clause);
    }
    printf("\n");
}

static void pp_case_item(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_CASE_ITEM);
    struct ast_case_item *ast_case_item = (struct ast_case_item *)ast;

    pp_word_list(ast_case_item->word_list);
    printf("\n");

    if (ast_case_item->compound_list != NULL)
        pp_compound_list(ast_case_item->compound_list);
}

static void pp_case_item_list(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_CASE_ITEM_LIST);
    struct ast_case_item_list *ast_case_item_list =
        (struct ast_case_item_list *)ast;

    pp_case_item(ast_case_item_list->case_item);
    ast_case_item_list = (struct ast_case_item_list *)ast_case_item_list->next;

    while (ast_case_item_list != NULL)
    {
        pp_case_item(ast_case_item_list->case_item);
        ast_case_item_list =
            (struct ast_case_item_list *)ast_case_item_list->next;
    }
}

static void pp_case_clause(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_CLAUSE_CASE);
    struct ast_case_clause *ast_case_clause = (struct ast_case_clause *)ast;

    pp_case_item_list(ast_case_clause->case_item_list);
}

static void pp_rule_case(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_RULE_CASE);
    struct ast_rule_case *ast_rule_case = (struct ast_rule_case *)ast;

    printf("case %s", ast_rule_case->word);

    printf("in ");

    if (ast_rule_case->case_clause != NULL)
    {
        printf("{\n");
        pp_case_clause(ast_rule_case->case_clause);
        printf("} ");
    }

    printf("esac\n");
}

static void pp_rule_if(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_RULE_IF);
    struct ast_rule_if *ast_rule_if = (struct ast_rule_if *)ast;

    printf("if {\n");

    pp_compound_list(ast_rule_if->condition_compound_list);

    printf("\n} then {\n");

    pp_compound_list(ast_rule_if->body_compound_list);

    if (ast_rule_if->else_clause != NULL)
        pp_else_clause(ast_rule_if->else_clause);

    printf("\n} fi\n");
}

static void pp_shell_cmd(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_SHELL_CMD);
    struct ast_shell_cmd *ast_shell_cmd = (struct ast_shell_cmd *)ast;

    if (ast_shell_cmd->compound_list != NULL)
    {
        printf("(");
        pp_compound_list(ast_shell_cmd->compound_list);
        printf(")");
    }
    else
    {
        if (ast_shell_cmd->rule->type == AST_RULE_FOR)
            pp_rule_for(ast_shell_cmd->rule);
        else if (ast_shell_cmd->rule->type == AST_RULE_WHILE)
            pp_rule_while(ast_shell_cmd->rule);
        else if (ast_shell_cmd->rule->type == AST_RULE_UNTIL)
            pp_rule_until(ast_shell_cmd->rule);
        else if (ast_shell_cmd->rule->type == AST_RULE_CASE)
            pp_rule_case(ast_shell_cmd->rule);
        else if (ast_shell_cmd->rule->type == AST_RULE_IF)
            pp_rule_if(ast_shell_cmd->rule);
    }
}

static void pp_funcdec(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_FUNCDEC);
    struct ast_funcdec *ast_funcdec = (struct ast_funcdec *)ast;

    printf("%s ()\n", ast_funcdec->name);

    pp_shell_cmd(ast_funcdec->shell_cmd);
}

/* TODO: complete when redirection will be done
static void pp_redirection(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_REDIRECTION);
    struct ast_redirection *ast_redirection = (struct ast_redirection *)ast;

    printf("%i ", ast_redirection->io_number);

    if (ast_redirection->redirection_type == 1)
    {
        printf("1 ");
        printf("%s", ast_redirection->word);
    }
}
*/

static void pp_cmd(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_CMD);
    struct ast_cmd *ast_cmd = (struct ast_cmd *)ast;

    if (ast_cmd != NULL)
    {
        if (ast_cmd->cmd->type == AST_SIMPLE_CMD)
            pp_simple_cmd(ast_cmd->cmd);
        else if (ast_cmd->cmd->type == AST_SHELL_CMD)
        {
            pp_shell_cmd(ast_cmd->cmd);
            // pp_redirection(ast_cdm->redirection);
        }
        else if (ast_cmd->cmd->type == AST_FUNCDEC)
        {
            pp_shell_cmd(ast_cmd->cmd);
            // pp_redirection(ast_cdm->redirection);
        }
    }
}

static void pp_pipeline(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_PIPELINE);
    struct ast_pipeline *ast_pipeline = (struct ast_pipeline *)ast;

    if (ast_pipeline->negation != 0)
    {
        printf("! ");
    }

    while (ast_pipeline != NULL)
    {
        printf("\n|\n");
        pp_cmd(ast_pipeline->cmd);

        ast_pipeline = (struct ast_pipeline *)ast_pipeline->next;
    }
}

static void pp_and_or(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_AND_OR);
    struct ast_and_or *ast_and_or = (struct ast_and_or *)ast;

    while (ast_and_or->next != NULL)
    {
        pp_pipeline(ast_and_or->pipeline);
        printf("\n");

        if (ast_and_or->operand == AND)
        {
            printf("&&\n");
        }
        else
        {
            printf("||\n");
        }

        ast_and_or = (struct ast_and_or *)ast_and_or->next;
    }

    pp_pipeline(ast_and_or->pipeline);
}

static void pp_list(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_LIST);
    struct ast_list *ast_list = (struct ast_list *)ast;

    while (ast_list != NULL)
    {
        pp_and_or(ast_list->and_or);
        printf(" ; ");
        ast_list = (struct ast_list *)ast_list->next;
    }
}

static void pp_input(struct ast *ast)
{
    assert(ast != NULL);
    assert(ast->type == AST_INPUT);
    struct ast_input *ast_input = (struct ast_input *)ast;
    pp_list(ast_input->list);
    printf("\n");
}
