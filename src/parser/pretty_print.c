#define _POSIX_C_SOURCE 200809L

#include "parser/pretty_print.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/ast.h"

static void pp_input(struct ast *ast, int prefix);
static void pp_list(struct ast *ast, int prefix);
static void pp_and_or(struct ast *ast, int prefix);
static void pp_pipeline(struct ast *ast, int prefix);
static void pp_cmd(struct ast *ast, int prefix);
static void pp_simple_cmd(struct ast *ast, int prefix);
static void pp_shell_cmd(struct ast *ast, int prefix);
static void pp_funcdec(struct ast *ast, int prefix);
// static void pp_redirection(struct ast *ast, int prefix);
static void pp_prefix(struct ast *ast, int prefix);
static void pp_prefix_list(struct ast *ast, int prefix);
static void pp_element(struct ast *ast, int prefix);
static void pp_element_list(struct ast *ast, int prefix);
static void pp_compound_list(struct ast *ast, int prefix);
static void pp_word_list(struct ast *ast, int prefix);
static void pp_rule_for(struct ast *ast, int prefix);
static void pp_rule_while(struct ast *ast, int prefix);
static void pp_rule_until(struct ast *ast, int prefix);
static void pp_rule_case(struct ast *ast, int prefix);
static void pp_rule_if(struct ast *ast, int prefix);
static void pp_else_clause(struct ast *ast, int prefix);
static void pp_case_clause(struct ast *ast, int prefix);
static void pp_case_item(struct ast *ast, int prefix);
static void pp_case_item_list(struct ast *ast, int prefix);

typedef void (*fptr)(struct ast *, int prefix);

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
    (*pp_functions[ast->type])(ast, 0);
}

static void add_tab(int prefix)
{
    int i = 0;

    while (i < prefix)
    {
        printf("    ");
        i++;
    }
}

static void pp_prefix(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
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

static void pp_element(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
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

static void pp_prefix_list(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
    assert(ast->type == AST_PREFIX_LIST);
    struct ast_prefix_list *ast_pref_list = (struct ast_prefix_list *)ast;

    if (ast_pref_list != NULL)
    {
        pp_prefix(ast_pref_list->prefix, prefix);
        ast_pref_list = (struct ast_prefix_list *)ast_pref_list->next;
    }

    while (ast_pref_list != NULL)
    {
        printf(" ");
        pp_prefix(ast_pref_list->prefix, prefix);
        ast_pref_list = (struct ast_prefix_list *)ast_pref_list->next;
    }
}

static void pp_element_list(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
    assert(ast->type == AST_ELEMENT_LIST);
    struct ast_element_list *ast_element_list = (struct ast_element_list *)ast;

    if (ast_element_list != NULL)
    {
        pp_element(ast_element_list->element, prefix);
        ast_element_list = (struct ast_element_list *)ast_element_list->next;
    }

    while (ast_element_list != NULL)
    {
        printf(" ");
        pp_element(ast_element_list->element, prefix);
        ast_element_list = (struct ast_element_list *)ast_element_list->next;
    }
}

static void pp_simple_cmd(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
    assert(ast->type == AST_SIMPLE_CMD);
    struct ast_simple_cmd *ast_simple_cmd = (struct ast_simple_cmd *)ast;

    if (ast_simple_cmd->prefix != NULL)
    {
        pp_prefix(ast_simple_cmd->prefix, prefix);
        printf(" ");
        if (ast_simple_cmd->prefix_list != NULL)
            pp_prefix_list(ast_simple_cmd->prefix_list, prefix);
    }
    else
    {
        if (ast_simple_cmd->prefix_list != NULL)
            pp_prefix_list(ast_simple_cmd->prefix_list, prefix);
        printf("%s", ast_simple_cmd->word);
        if (ast_simple_cmd->element_list != NULL)
	{
	    printf(" ");
            pp_element_list(ast_simple_cmd->element_list, prefix);
	}
    }

    printf(";");
}

static void pp_compound_list(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
    assert(ast->type == AST_COMPOUND_LIST);
    struct ast_compound_list *ast_compound_list =
        (struct ast_compound_list *)ast;

    add_tab(prefix);
    pp_and_or(ast_compound_list->ast_and_or, prefix);
    ast_compound_list = (struct ast_compound_list *)ast_compound_list->next;

    while (ast_compound_list != NULL)
    {
        pp_and_or(ast_compound_list->ast_and_or, prefix);
        ast_compound_list = (struct ast_compound_list *)ast_compound_list->next;
    }
}

static void pp_word_list(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
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

static void pp_rule_for(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
    assert(ast->type == AST_RULE_FOR);
    struct ast_rule_for *ast_rule_for = (struct ast_rule_for *)ast;

    printf("for %s", ast_rule_for->condition_word);

    if (ast_rule_for->in_word_list != NULL)
    {
        printf("in ");
        pp_word_list(ast_rule_for->in_word_list, prefix);
    }

    printf("; do\n");
    prefix += 1;
    pp_compound_list(ast_rule_for->body_compound_list, prefix);
    prefix -= 1;
    add_tab(prefix);
    printf("done\n");
}

static void pp_rule_while(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
    assert(ast->type == AST_RULE_WHILE);
    struct ast_rule_while *ast_rule_while = (struct ast_rule_while *)ast;

    printf("while ");
    pp_compound_list(ast_rule_while->condition_compound_list, prefix);
    printf("; do\n");
    prefix += 1;
    pp_compound_list(ast_rule_while->body_compound_list, prefix);
    prefix -= 1;
    add_tab(prefix);
    printf("done\n");
}

static void pp_rule_until(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
    assert(ast->type == AST_RULE_UNTIL);
    struct ast_rule_until *ast_rule_until = (struct ast_rule_until *)ast;

    printf("until ");
    pp_compound_list(ast_rule_until->condition_compound_list, prefix);
    printf("; do\n");
    prefix += 1;
    pp_compound_list(ast_rule_until->body_compound_list, prefix);
    prefix -= 1;
    add_tab(prefix);
    printf("done\n");
}

static void pp_else_clause(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
    assert(ast->type == AST_CLAUSE_ELSE);
    struct ast_else_clause *ast_else_clause = (struct ast_else_clause *)ast;

    if (ast_else_clause->body_compound_list != NULL)
    {
	add_tab(prefix);
        printf("else\n");
        prefix += 1;
        pp_compound_list(ast_else_clause->body_compound_list, prefix);
        prefix -= 1;
    }
    else
    {
        printf("elif ");
        pp_compound_list(ast_else_clause->condition_compound_list, prefix);
        printf("; then\n");
	prefix++;
        pp_compound_list(ast_else_clause->body_compound_list, prefix);
	prefix--;
        printf(";\n");
        if (ast_else_clause->else_clause != NULL)
            pp_else_clause(ast_else_clause->else_clause, prefix);
    }
}

static void pp_case_item(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
    assert(ast->type == AST_CASE_ITEM);
    struct ast_case_item *ast_case_item = (struct ast_case_item *)ast;

    pp_word_list(ast_case_item->word_list, prefix);
    printf("\n");

    if (ast_case_item->compound_list != NULL)
        pp_compound_list(ast_case_item->compound_list, prefix);
}

static void pp_case_item_list(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
    assert(ast->type == AST_CASE_ITEM_LIST);
    struct ast_case_item_list *ast_case_item_list =
        (struct ast_case_item_list *)ast;

    pp_case_item(ast_case_item_list->case_item, prefix);
    ast_case_item_list = (struct ast_case_item_list *)ast_case_item_list->next;

    while (ast_case_item_list != NULL)
    {
        pp_case_item(ast_case_item_list->case_item, prefix);
        ast_case_item_list =
            (struct ast_case_item_list *)ast_case_item_list->next;
    }
}

static void pp_case_clause(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
    assert(ast->type == AST_CLAUSE_CASE);
    struct ast_case_clause *ast_case_clause = (struct ast_case_clause *)ast;

    add_tab(prefix);
    pp_case_item_list(ast_case_clause->case_item_list, prefix);
}

static void pp_rule_case(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
    assert(ast->type == AST_RULE_CASE);
    struct ast_rule_case *ast_rule_case = (struct ast_rule_case *)ast;

    add_tab(prefix);
    printf("case %s", ast_rule_case->word);

    printf("in ");

    if (ast_rule_case->case_clause != NULL)
        pp_case_clause(ast_rule_case->case_clause, prefix);

    printf("esac\n");
}

static void pp_rule_if(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
    assert(ast->type == AST_RULE_IF);
    struct ast_rule_if *ast_rule_if = (struct ast_rule_if *)ast;

    printf("if ");
    int tmp = prefix;
    prefix = 0;
    pp_compound_list(ast_rule_if->condition_compound_list, prefix);
    prefix = tmp;
    printf(" then\n");

    prefix += 1;
    pp_compound_list(ast_rule_if->body_compound_list, prefix);
    prefix -= 1;

    if (ast_rule_if->else_clause != NULL)
    {
	printf("\n");
        pp_else_clause(ast_rule_if->else_clause, prefix);
    }

    printf("\n");
    add_tab(prefix);
    printf("fi;");
}

static void pp_shell_cmd(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
    assert(ast->type == AST_SHELL_CMD);
    struct ast_shell_cmd *ast_shell_cmd = (struct ast_shell_cmd *)ast;

    if (ast_shell_cmd->compound_list != NULL)
    {
        printf("(");
        pp_compound_list(ast_shell_cmd->compound_list, prefix);
        printf(")");
    }
    else
    {
        if (ast_shell_cmd->rule->type == AST_RULE_FOR)
            pp_rule_for(ast_shell_cmd->rule, prefix);
        else if (ast_shell_cmd->rule->type == AST_RULE_WHILE)
            pp_rule_while(ast_shell_cmd->rule, prefix);
        else if (ast_shell_cmd->rule->type == AST_RULE_UNTIL)
            pp_rule_until(ast_shell_cmd->rule, prefix);
        else if (ast_shell_cmd->rule->type == AST_RULE_CASE)
            pp_rule_case(ast_shell_cmd->rule, prefix);
        else if (ast_shell_cmd->rule->type == AST_RULE_IF)
            pp_rule_if(ast_shell_cmd->rule, prefix);
    }
}

static void pp_funcdec(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
    assert(ast->type == AST_FUNCDEC);
    struct ast_funcdec *ast_funcdec = (struct ast_funcdec *)ast;

    printf("%s ()\n", ast_funcdec->name);

    pp_shell_cmd(ast_funcdec->shell_cmd, prefix);
}

/* TODO: complete when redirection will be done
static void pp_redirection(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
    assert(ast->type == AST_REDIRECTION);
    struct ast_redirection *ast_redirection = (struct ast_redirection *)ast;

    add_tab(prefix);
    printf("%i ", ast_redirection->io_number);

    if (ast_redirection->redirection_type == 1)
    {
        printf("1 ");
        printf("%s", ast_redirection->word);
    }
}
*/

static void pp_cmd(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
    assert(ast->type == AST_CMD);
    struct ast_cmd *ast_cmd = (struct ast_cmd *)ast;

    if (ast_cmd != NULL)
    {
        if (ast_cmd->cmd->type == AST_SIMPLE_CMD)
            pp_simple_cmd(ast_cmd->cmd, prefix);
        else if (ast_cmd->cmd->type == AST_SHELL_CMD)
        {
            pp_shell_cmd(ast_cmd->cmd, prefix);
            // pp_redirection(ast_cdm->redirection);
        }
        else if (ast_cmd->cmd->type == AST_FUNCDEC)
        {
            pp_shell_cmd(ast_cmd->cmd, prefix);
            // pp_redirection(ast_cdm->redirection);
        }
    }
}

static void pp_pipeline(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
    assert(ast->type == AST_PIPELINE);
    struct ast_pipeline *ast_pipeline = (struct ast_pipeline *)ast;

    if (ast_pipeline->negation != 0)
    {
        printf("! ");
    }

    while (ast_pipeline != NULL)
    {
        pp_cmd(ast_pipeline->cmd, prefix);

        ast_pipeline = (struct ast_pipeline *)ast_pipeline->next;
    }
}

static void pp_and_or(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
    assert(ast->type == AST_AND_OR);
    struct ast_and_or *ast_and_or = (struct ast_and_or *)ast;

    while (ast_and_or->next != NULL)
    {
        pp_pipeline(ast_and_or->pipeline, prefix);

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

    pp_pipeline(ast_and_or->pipeline, prefix);
}

static void pp_list(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
    assert(ast->type == AST_LIST);
    struct ast_list *ast_list = (struct ast_list *)ast;

    while (ast_list != NULL)
    {
        pp_and_or(ast_list->and_or, prefix);
        printf("\n");
        ast_list = (struct ast_list *)ast_list->next;
    }
}

static void pp_input(struct ast *ast, int prefix)
{
    assert(ast != NULL && prefix >= 0);
    assert(ast->type == AST_INPUT);
    struct ast_input *ast_input = (struct ast_input *)ast;
    pp_list(ast_input->list, prefix);
    printf("\n");
}
