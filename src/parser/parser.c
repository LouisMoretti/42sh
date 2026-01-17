#define _POSIX_C_SOURCE 200809L
#include "parser/parser.h"

#include <err.h>
#include <stddef.h>
#include <string.h>

#include "lexer/lexer.h"
#include "parser/ast.h"

static struct ast *parse_list(int *status_code);
static struct ast *parse_and_or(int *status_code);
static struct ast *parse_pipeline(int *status_code);
static struct ast *parse_cmd(int *status_code);
static struct ast *parse_simple_cmd(int *status_code);
static struct ast *parse_shell_cmd(int *status_code);
static struct ast *parse_word_list(int *status_code);
static struct ast *parse_rule_for(int *status_code);
static struct ast *parse_rule_while(int *status_code);
static struct ast *parse_rule_until(int *status_code);
static struct ast *parse_rule_if(int *status_code);
static struct ast *parse_compound_list(int *status_code);
static struct ast *parse_else_clause(int *status_code);

const char *type_name[] = { [IF] = "IF",
                            [THEN] = "THEN",
                            [ELIF] = "ELIF",
                            [ELSE] = "ELSE",
                            [FI] = "FI",
                            [FOR] = "FOR",
                            [DO] = "DO",
                            [DONE] = "DONE",
                            [WHILE] = "WHILE",
                            [UNTIL] = "UNTIL",
                            // [CASE] = "CASE",
                            [IN] = "IN",
                            // [ESAC] = "ESAC",
                            [NEGATION] = "NEGATION",
                            // [KEYWORD_COUNT] = "KEYWORD_COUNT",
                            [NEW_LINE] = "NEW_LINE",
                            [SEMICOLON] = "SEMICOLON",
                            // [DOUBLE_SEMICOLON] = "DOUBLE_SEMICOLON",
                            [PIPE] = "PIPE",
                            [DOUBLE_PIPE] = "DOUBLE_PIPE",
                            [AMPERSAND] = "AMPERSAND",
                            [DOUBLE_AMPERSAND] = "DOUBLE_AMPERSAND",
                            // [LEFT_PARANTHESIS] = "LEFT_PARANTHESIS",
                            // [RIGHT_PARANTHESIS] = "RIGHT_PARANTHESIS",
                            // [LEFT_BRACKET] = "LEFT_BRACKET",
                            // [RIGHT_BRACKET] = "RIGHT_BRACKET",
                            [WORD] = "WORD",
                            [END_OF_FILE] = "END_OF_FILE" };

struct ast *parse_input(int *status_code)
{
    struct ast *ast_input = init_ast(AST_INPUT);

    struct token *tok = peek_token(ENABLE_KEYWORDS);
    if (tok->type != END_OF_FILE && tok->type != NEW_LINE)
    {
        ((struct ast_input *)ast_input)->list = parse_list(status_code);
        if (*status_code)
            return ast_input;
    }

    if (tok->type != END_OF_FILE && tok->type != NEW_LINE)
    {
        warnx("parse_input: Wrong token type at end of input. Expected "
              "END_OF_FILE or NEW_LINE | Got: %s",
              type_name[tok->type]);
        *status_code = 2;
    }

    return ast_input;
}

static struct ast *parse_list(int *status_code)
{
    struct ast *list = init_ast(AST_LIST);

    ((struct ast_list *)list)->and_or = parse_and_or(status_code);
    if (*status_code)
        return list;

    if (peek_token(ENABLE_KEYWORDS)->type == SEMICOLON
        || peek_token(ENABLE_KEYWORDS)->type == AMPERSAND)
    {
        pop_token();
        if (peek_token(ENABLE_KEYWORDS)->type != END_OF_FILE)
        {
            ((struct ast_list *)list)->next = parse_list(status_code);
            if (*status_code)
                return list;
        }
    }

    if (peek_token(ENABLE_KEYWORDS)->type == SEMICOLON
        || peek_token(ENABLE_KEYWORDS)->type == AMPERSAND)
        pop_token();

    return list;
}

static struct ast *parse_and_or(int *status_code)
{
    struct ast *and_or = init_ast(AST_AND_OR);

    ((struct ast_and_or *)and_or)->pipeline = parse_pipeline(status_code);
    if (*status_code)
        return and_or;

    if (peek_token(ENABLE_KEYWORDS)->type == DOUBLE_PIPE
        || peek_token(ENABLE_KEYWORDS)->type == DOUBLE_AMPERSAND)
    {
        ((struct ast_and_or *)and_or)->operand =
            peek_token(ENABLE_KEYWORDS)->type == DOUBLE_AMPERSAND ? AND : OR;
        pop_token();

        while (peek_token(ENABLE_KEYWORDS)->type == NEW_LINE)
            pop_token();

        ((struct ast_and_or *)and_or)->next = parse_and_or(status_code);
        if (*status_code)
            return and_or;
    }

    return and_or;
}

static struct ast *parse_pipeline(int *status_code)
{
    struct ast *pipeline = init_ast(AST_PIPELINE);

    if (peek_token(ENABLE_KEYWORDS)->type == NEGATION)
    {
        pop_token();
        ((struct ast_pipeline *)pipeline)->negation = 1;
    }

    ((struct ast_pipeline *)pipeline)->cmd = parse_cmd(status_code);
    if (*status_code)
        return pipeline;

    struct ast_pipeline *cur = (struct ast_pipeline *)pipeline;
    while (peek_token(ENABLE_KEYWORDS)->type == PIPE)
    {
        pop_token();

        if (peek_token(ENABLE_KEYWORDS)->type == NEGATION)
        {
            warnx("parse_pipeline: Wrong token type. No negation after pipe.");
            *status_code = 2;
            return pipeline;
        }

        while (peek_token(ENABLE_KEYWORDS)->type == NEW_LINE)
            pop_token();

        cur->next = init_ast(AST_PIPELINE);
        cur = (struct ast_pipeline *)cur->next;
        cur->cmd = parse_cmd(status_code);
        if (*status_code)
            return pipeline;
    }

    return pipeline;
}

static struct ast *parse_cmd(int *status_code)
{
    struct ast *cmd = init_ast(AST_CMD);

    if (peek_token(ENABLE_KEYWORDS)->type == WORD)
        ((struct ast_cmd *)cmd)->cmd = parse_simple_cmd(status_code);
    else
        ((struct ast_cmd *)cmd)->cmd = parse_shell_cmd(status_code);

    return cmd;
}

static struct ast *parse_element(int *status_code)
{
    struct ast *element = init_ast(AST_ELEMENT);

    struct token *tok = peek_token(DISABLE_KEYWORDS);
    if (tok->type != WORD) // TODO: && tok->type != REDIRECTION
    {
        warnx("parse_element: Wrong token type. Expected WORD | Got: %s",
              type_name[tok->type]);
        *status_code = 2;
        return element;
    }

    ((struct ast_element *)element)->word = strdup(tok->data);
    pop_token();

    return element;
}

static struct ast *parse_element_list(int *status_code)
{
    struct ast *element_list = init_ast(AST_ELEMENT_LIST);

    struct token *tok = peek_token(DISABLE_KEYWORDS);
    if (tok->type != WORD) // TODO: && tok->type != REDIRECTION
    {
        warnx("parse_element_list: Wrong token type. Expected WORD | Got: %s",
              type_name[tok->type]);
        *status_code = 2;
        return element_list;
    }

    ((struct ast_element_list *)element_list)->element =
        parse_element(status_code);
    if (*status_code)
        return element_list;

    tok = peek_token(DISABLE_KEYWORDS);
    if (tok->type == WORD) // TODO: || tok->type == REDIRECTION
    {
        ((struct ast_element_list *)element_list)->next =
            parse_element_list(status_code);
        if (*status_code)
            return element_list;
    }

    return element_list;
}

static int check_prefix()
{
    struct token *tok = peek_token(DISABLE_KEYWORDS);

    // TODO: Add redirections.

    if (tok->type != WORD)
        return 0;

    char *str = tok->data;
    size_t len = strlen(str);

    // First char [A-Za-z_]
    if ((str[0] < 'A' || (str[0] > 'Z' && str[0] < 'a') || (str[0] > 'z'))
        && str[0] != '_')
        return 0;

    // Other char until '=' [A-Za-z0-9_]
    size_t i = 1;
    for (; i < len; i++)
    {
        if (str[i] == '=')
            break;
        if ((str[i] < '0' || (str[i] > '9' && str[i] < 'A')
             || (str[i] > 'Z' && str[i] < 'a') || (str[i] > 'z'))
            && str[i] != '_')
            return 0;
    }

    // i == len <=> str[i] != '='
    // We check both to be sure.
    if (i == len || str[i] != '=')
        return 0;

    // Skip '='
    i++;

    // Anything is allowed after '=' ?!

    return 1;
}

static struct ast *parse_prefix(int *status_code)
{
    if (check_prefix() == 0)
    {
        warnx("parse_prefix: Not a prefix!");
        *status_code = 2;
        return NULL;
    }

    struct ast *prefix = init_ast(AST_PREFIX);

    struct token *tok = peek_token(DISABLE_KEYWORDS);
    ((struct ast_prefix *)prefix)->assignment_word = strdup(tok->data);
    pop_token();

    return prefix;
}

static struct ast *parse_prefix_list(int *status_code)
{
    if (check_prefix() == 0)
    {
        warnx("parse_prefix_list: Not a prefix!");
        *status_code = 2;
        return NULL;
    }

    struct ast *prefix_list = init_ast(AST_PREFIX_LIST);

    ((struct ast_prefix_list *)prefix_list)->prefix = parse_prefix(status_code);
    if (*status_code)
        return prefix_list;

    if (check_prefix())
    {
        ((struct ast_prefix_list *)prefix_list)->next =
            parse_prefix_list(status_code);
        if (*status_code)
            return prefix_list;
    }

    return prefix_list;
}

static struct ast *parse_simple_cmd(int *status_code)
{
    struct ast *cmd = init_ast(AST_SIMPLE_CMD);

    if (peek_token(ENABLE_KEYWORDS)->type
        != WORD) // && peek_token(DISABLE_KEYWORDS)->type != REDIRECTION
    {
        warnx("parse_simple_cmd: Wrong token type. Expected WORD | Got: %s",
              type_name[peek_token(ENABLE_KEYWORDS)->type]);
        *status_code = 2;
        return cmd;
    }

    if (check_prefix())
    {
        ((struct ast_simple_cmd *)cmd)->prefix_list =
            parse_prefix_list(status_code);
        if (*status_code)
            return cmd;

        // TODO: Check logic.
        if (peek_token(ENABLE_KEYWORDS)->type
            != WORD) // && peek_token(DISABLE_KEYWORDS)->type != REDIRECTION
            return cmd;
    }

    struct token *tok = peek_token(ENABLE_KEYWORDS);
    if (tok->type != WORD)
    {
        warnx("parse_simple_cmd: Wrong token type. Expected WORD | Got: %s",
              type_name[tok->type]);
        *status_code = 2;
        return cmd;
    }

    ((struct ast_simple_cmd *)cmd)->word = strdup(tok->data);
    pop_token();

    tok = peek_token(DISABLE_KEYWORDS);

    if (tok->type == WORD) // TODO: || tok->type == REDIRECTION
    {
        ((struct ast_simple_cmd *)cmd)->element_list =
            parse_element_list(status_code);
        if (*status_code)
            return cmd;
    }

    return cmd;
}

static struct ast *parse_shell_cmd(int *status_code)
{
    struct ast *cmd = init_ast(AST_SHELL_CMD);

    struct token *tok = peek_token(ENABLE_KEYWORDS);

    // TODO: Step 3: Add other rules.
    if (tok->type != IF && tok->type != WHILE && tok->type != UNTIL
        && tok->type != FOR)
    {
        warnx("parse_shell_cmd: Unsupported shell command. Expected IF or "
              "WHILE or UNTIL or FOR | Got: %s",
              type_name[tok->type]);
        *status_code = 2;
        return cmd;
    }

    if (tok->type == IF)
        ((struct ast_shell_cmd *)cmd)->rule = parse_rule_if(status_code);
    else if (tok->type == WHILE)
        ((struct ast_shell_cmd *)cmd)->rule = parse_rule_while(status_code);
    else if (tok->type == UNTIL)
        ((struct ast_shell_cmd *)cmd)->rule = parse_rule_until(status_code);
    else if (tok->type == FOR)
        ((struct ast_shell_cmd *)cmd)->rule = parse_rule_for(status_code);

    return cmd;
}

static struct ast *parse_word_list(int *status_code)
{
    struct ast *word_list = init_ast(AST_WORD_LIST);

    if (peek_token(DISABLE_KEYWORDS)->type != WORD)
    {
        warnx("parse_word_list: Wrong token type. Expected WORD | Got: %s",
              type_name[peek_token(DISABLE_KEYWORDS)->type]);
        *status_code = 2;
        return word_list;
    }

    ((struct ast_word_list *)word_list)->word =
        strdup(peek_token(DISABLE_KEYWORDS)->data);
    pop_token();

    if (peek_token(DISABLE_KEYWORDS)->type == WORD)
    {
        ((struct ast_word_list *)word_list)->next =
            parse_word_list(status_code);
        if (*status_code)
            return word_list;
    }

    return word_list;
}

static struct ast *parse_rule_for(int *status_code)
{
    struct ast *rule_for = init_ast(AST_RULE_FOR);

    if (peek_token(ENABLE_KEYWORDS)->type != FOR)
    {
        warnx("parse_rule_for: Wrong token type. Expected FOR | Got: %s",
              type_name[peek_token(ENABLE_KEYWORDS)->type]);
        *status_code = 2;
        return rule_for;
    }
    pop_token();

    if (peek_token(DISABLE_KEYWORDS)->type != WORD)
    {
        warnx("parse_rule_for: Wrong token type. Expected WORD | Got: %s",
              type_name[peek_token(DISABLE_KEYWORDS)->type]);
        *status_code = 2;
        return rule_for;
    }

    ((struct ast_rule_for *)rule_for)->condition_word =
        strdup(peek_token(DISABLE_KEYWORDS)->data);
    pop_token();

    if (peek_token(ENABLE_KEYWORDS)->type == SEMICOLON)
        pop_token();

    while (peek_token(ENABLE_KEYWORDS)->type == NEW_LINE)
    {
        pop_token();
    }

    if (peek_token(ENABLE_KEYWORDS)->type != IN
        && peek_token(ENABLE_KEYWORDS)->type != DO)
    {
        warnx("parse_rule_for: Wrong token type. Expected IN or DO | Got: %s",
              type_name[peek_token(ENABLE_KEYWORDS)->type]);
        *status_code = 2;
        return rule_for;
    }

    if (peek_token(ENABLE_KEYWORDS)->type == IN)
    {
        pop_token();

        ((struct ast_rule_for *)rule_for)->in_word_list =
            parse_word_list(status_code);
        if (*status_code)
            return rule_for;

        if (peek_token(ENABLE_KEYWORDS)->type == SEMICOLON)
            pop_token();

        while (peek_token(ENABLE_KEYWORDS)->type == NEW_LINE)
        {
            pop_token();
        }
    }

    if (peek_token(ENABLE_KEYWORDS)->type != DO)
    {
        warnx("parse_rule_for: Wrong token type. Expected DO | Got: %s",
              type_name[peek_token(ENABLE_KEYWORDS)->type]);
        *status_code = 2;
        return rule_for;
    }
    pop_token();

    ((struct ast_rule_for *)rule_for)->body_compound_list =
        parse_compound_list(status_code);
    if (*status_code)
        return rule_for;

    if (peek_token(ENABLE_KEYWORDS)->type != DONE)
    {
        warnx("parse_rule_for: Wrong token type. Expected DONE | Got: %s",
              type_name[peek_token(ENABLE_KEYWORDS)->type]);
        *status_code = 2;
        return rule_for;
    }
    pop_token();

    return rule_for;
}

static struct ast *parse_rule_while(int *status_code)
{
    struct ast *rule_while = init_ast(AST_RULE_WHILE);

    struct token *tok = peek_token(ENABLE_KEYWORDS);
    if (tok->type != WHILE)
    {
        warnx("parse_rule_while: Wrong token type. Expected WHILE | Got: %s",
              type_name[tok->type]);
        *status_code = 2;
        return rule_while;
    }
    pop_token();

    ((struct ast_rule_while *)rule_while)->condition_compound_list =
        parse_compound_list(status_code);
    if (*status_code)
        return rule_while;

    tok = peek_token(ENABLE_KEYWORDS);
    if (tok->type != DO)
    {
        warnx("parse_rule_while: Wrong token type. Expected DO | Got: %s",
              type_name[tok->type]);
        *status_code = 2;
        return rule_while;
    }
    pop_token();

    ((struct ast_rule_while *)rule_while)->body_compound_list =
        parse_compound_list(status_code);
    if (*status_code)
        return rule_while;

    tok = peek_token(ENABLE_KEYWORDS);
    if (tok->type != DONE)
    {
        warnx("parse_rule_while: Wrong token type. Expected DONE | Got: %s",
              type_name[tok->type]);
        *status_code = 2;
        return rule_while;
    }
    pop_token();

    return rule_while;
}

static struct ast *parse_rule_until(int *status_code)
{
    struct ast *rule_until = init_ast(AST_RULE_UNTIL);

    struct token *tok = peek_token(ENABLE_KEYWORDS);
    if (tok->type != UNTIL)
    {
        warnx("parse_rule_until: Wrong token type. Expected UNTIL | Got: %s",
              type_name[tok->type]);
        *status_code = 2;
        return rule_until;
    }
    pop_token();

    ((struct ast_rule_until *)rule_until)->condition_compound_list =
        parse_compound_list(status_code);
    if (*status_code)
        return rule_until;

    tok = peek_token(ENABLE_KEYWORDS);
    if (tok->type != DO)
    {
        warnx("parse_rule_until: Wrong token type. Expected DO | Got: %s",
              type_name[tok->type]);
        *status_code = 2;
        return rule_until;
    }
    pop_token();

    ((struct ast_rule_until *)rule_until)->body_compound_list =
        parse_compound_list(status_code);
    if (*status_code)
        return rule_until;

    tok = peek_token(ENABLE_KEYWORDS);
    if (tok->type != DONE)
    {
        warnx("parse_rule_until: Wrong token type. Expected DONE | Got: %s",
              type_name[tok->type]);
        *status_code = 2;
        return rule_until;
    }
    pop_token();

    return rule_until;
}

static struct ast *parse_rule_if(int *status_code)
{
    struct ast *rule_if = init_ast(AST_RULE_IF);

    struct token *tok = peek_token(ENABLE_KEYWORDS);
    if (tok->type != IF)
    {
        warnx("parse_rule_if: Wrong token type. Expected IF | Got: %s",
              type_name[tok->type]);
        *status_code = 2;
        return rule_if;
    }
    pop_token();

    ((struct ast_rule_if *)rule_if)->condition_compound_list =
        parse_compound_list(status_code);
    if (*status_code)
        return rule_if;

    tok = peek_token(ENABLE_KEYWORDS);
    if (tok->type != THEN)
    {
        warnx("parse_rule_if: Wrong token type. Expected THEN | Got: %s",
              type_name[tok->type]);
        *status_code = 2;
        return rule_if;
    }
    pop_token();

    ((struct ast_rule_if *)rule_if)->body_compound_list =
        parse_compound_list(status_code);
    if (*status_code)
        return rule_if;

    tok = peek_token(ENABLE_KEYWORDS);
    if (tok->type == ELIF || tok->type == ELSE)
    {
        ((struct ast_rule_if *)rule_if)->else_clause =
            parse_else_clause(status_code);
        if (*status_code)
            return rule_if;
    }

    tok = peek_token(ENABLE_KEYWORDS);
    if (tok->type != FI)
    {
        warnx("parse_rule_if: Wrong token type. Expected FI | Got: %s",
              type_name[tok->type]);
        *status_code = 2;
        return rule_if;
    }
    pop_token();

    return rule_if;
}

static struct ast *parse_compound_list(int *status_code)
{
    struct ast *compound_list = init_ast(AST_COMPOUND_LIST);

    while (peek_token(ENABLE_KEYWORDS)->type == NEW_LINE)
    {
        pop_token();
    }

    ((struct ast_compound_list *)compound_list)->ast_and_or =
        parse_and_or(status_code);
    if (*status_code)
        return compound_list;

    struct token *tok = peek_token(ENABLE_KEYWORDS);
    if (tok->type == SEMICOLON || tok->type == NEW_LINE
        || tok->type == AMPERSAND)
    {
        pop_token();
        tok = peek_token(ENABLE_KEYWORDS);

        while (tok->type == NEW_LINE)
        {
            pop_token();
            tok = peek_token(ENABLE_KEYWORDS);
        }

        if (tok->type > KEYWORD_COUNT)
        {
            ((struct ast_compound_list *)compound_list)->next =
                parse_compound_list(status_code);
            if (*status_code)
                return compound_list;
        }
    }

    if (peek_token(ENABLE_KEYWORDS)->type == SEMICOLON
        || peek_token(ENABLE_KEYWORDS)->type == AMPERSAND)
        pop_token();

    while (peek_token(ENABLE_KEYWORDS)->type == NEW_LINE)
        pop_token();

    return compound_list;
}

static struct ast *parse_else_clause(int *status_code)
{
    if (peek_token(ENABLE_KEYWORDS)->type != ELIF
        && peek_token(ENABLE_KEYWORDS)->type != ELSE)
    {
        warnx("parse_else_clause: Wrong entry token type. Expected ELIF or "
              "ELSE | Got: %s",
              type_name[peek_token(ENABLE_KEYWORDS)->type]);
        *status_code = 2;
        return NULL;
    }

    struct ast *else_clause = init_ast(AST_CLAUSE_ELSE);
    int is_elif = 0;

    if (peek_token(ENABLE_KEYWORDS)->type == ELIF)
        is_elif = 1;
    pop_token();

    if (is_elif)
    {
        ((struct ast_else_clause *)else_clause)->condition_compound_list =
            parse_compound_list(status_code);
        if (*status_code)
            return else_clause;

        if (peek_token(ENABLE_KEYWORDS)->type != THEN)
        {
            warnx("parse_else_clause: Wrong token type. Expected THEN | "
                  "Got: %s",
                  type_name[peek_token(ENABLE_KEYWORDS)->type]);
            *status_code = 2;
            return else_clause;
        }
        pop_token();
    }

    ((struct ast_else_clause *)else_clause)->body_compound_list =
        parse_compound_list(status_code);
    if (*status_code)
        return else_clause;

    // TODO: Grammar error check: token can only be ELIF or ELSE or FI.
    if (is_elif && (peek_token(ENABLE_KEYWORDS)->type != FI))
    {
        if (peek_token(ENABLE_KEYWORDS)->type != ELIF
            && peek_token(ENABLE_KEYWORDS)->type != ELSE)
        {
            warnx("parse_else_clause: Wrong token type. Expected ELIF or "
                  "ELSE | Got: %s",
                  type_name[peek_token(ENABLE_KEYWORDS)->type]);
            *status_code = 2;
            return else_clause;
        }
        ((struct ast_else_clause *)else_clause)->else_clause =
            parse_else_clause(status_code);
        if (*status_code)
            return else_clause;
    }

    return else_clause;
}
