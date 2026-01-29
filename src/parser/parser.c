#define _POSIX_C_SOURCE 200809L
#include "parser/parser.h"

#include <err.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/lexer.h"
#include "parser/ast.h"

static struct ast *parse_list(int *status_code);
static struct ast *parse_and_or(int *status_code);
static struct ast *parse_pipeline(int *status_code);
static struct ast *parse_cmd(int *status_code);
static struct ast *parse_redirection(int *status_code);
static struct ast *parse_simple_cmd(int *status_code);
static struct ast *parse_shell_cmd(int *status_code);
static struct ast *parse_word_list(int *status_code);
static struct ast *parse_rule_for(int *status_code);
static struct ast *parse_rule_while(int *status_code);
static struct ast *parse_rule_until(int *status_code);
static struct ast *parse_rule_if(int *status_code);
static struct ast *parse_compound_list(int *status_code);
static struct ast *parse_else_clause(int *status_code);
static struct ast *parse_simple_cmd_or_funcdec(int *status_code);

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
                            // [LEFT_BRACKET] = "LEFT_BRACKET",
                            // [RIGHT_BRACKET] = "RIGHT_BRACKET",
                            [KEYWORD_COUNT] = "KEYWORD_COUNT (INVALID)",
                            [NEW_LINE] = "NEW_LINE",
                            [REDIRECTION] = "REDIRECTION",
                            [SEMICOLON] = "SEMICOLON",
                            [DOUBLE_SEMICOLON] = "DOUBLE_SEMICOLON",
                            [PIPE] = "PIPE",
                            [DOUBLE_PIPE] = "DOUBLE_PIPE",
                            [AMPERSAND] = "AMPERSAND",
                            [DOUBLE_AMPERSAND] = "DOUBLE_AMPERSAND",
                            [LEFT_PARENTHESIS] = "LEFT_PARENTHESIS",
                            [RIGHT_PARENTHESIS] = "RIGHT_PARENTHESIS",
                            [WORD] = "WORD",
                            [END_OF_FILE] = "END_OF_FILE",
                            [ERROR] = "ERROR" };

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
    pop_token();

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
        if (peek_token(ENABLE_KEYWORDS)->type != END_OF_FILE
            && peek_token(ENABLE_KEYWORDS)->type != NEW_LINE)
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

static enum redirection_type get_redirection_type(char *redir)
{
    enum redirection_type type = REDIRECT_OUT;
    char c = redir[0];

    // TODO: Handle heredoc.
    if (c == '>')
    {
        c = redir[1];
        if (c == '\0')
            type = REDIRECT_OUT;
        else if (c == '>')
            type = REDIRECT_OUT_APPEND;
        else if (c == '&')
            type = REDIRECT_OUT_DUP;
        else if (c == '|')
            type = REDIRECT_OUT_FORCE;
        else
            warnx("get_redirection_type: Wrong redirection type: %s.", redir);
    }
    else if (c == '<')
    {
        c = redir[1];
        if (c == '\0')
            type = REDIRECT_IN;
        else if (c == '&')
            type = REDIRECT_IN_DUP;
        else if (c == '>')
            type = REDIRECT_IN_OUT;
        else
            warnx("get_redirection_type: Wrong redirection type: %s.", redir);
    }
    else
        warnx("get_redirection_type: Wrong redirection type: %s.", redir);

    return type;
}

static struct ast *parse_redirection(int *status_code)
{
    struct ast *redirection = init_ast(AST_REDIRECTION);

    struct token *tok = peek_token(DISABLE_KEYWORDS);

    // TODO: Check for errors.

    int i = 0;
    while (tok->data[i] != '>' && tok->data[i] != '<')
        i++;

    int io_number = -1;
    if (i > 0)
    {
        char *tmp = strndup(tok->data, i);
        io_number = atoi(tmp);
        if (io_number == 0 && (tmp[0] != '0' || tmp[1] != '\0'))
        {
            warnx("parse_redirection: Not a valid io number, got: %s.", tmp);
            free(tmp);
            *status_code = 2;
            return redirection;
        }
        free(tmp);
    }

    ((struct ast_redirection *)redirection)->io_number = io_number;

    ((struct ast_redirection *)redirection)->type =
        get_redirection_type(tok->data + i);
    pop_token();

    tok = peek_token(DISABLE_KEYWORDS);
    if (tok->type != WORD)
    {
        warnx("parse_element: Wrong token type. Expected WORD | Got: %s",
              type_name[tok->type]);
        *status_code = 2;
        return redirection;
    }
    ((struct ast_redirection *)redirection)->word = strdup(tok->data);
    pop_token();

    return redirection;
}

static void place_redir(struct ast **ret, struct ast **last_redirection,
                        struct ast *redirection)
{
    if (*last_redirection == NULL)
        *ret = redirection;
    else
        ((struct ast_redirection *)*last_redirection)->next = redirection;

    *last_redirection = redirection;
}

static struct ast *parse_cmd(int *status_code)
{
    struct ast *cmd = init_ast(AST_CMD);
    struct ast *redir = NULL;

    if (peek_token(ENABLE_KEYWORDS)->type == WORD
        || peek_token(ENABLE_KEYWORDS)->type == REDIRECTION)
        ((struct ast_cmd *)cmd)->cmd = parse_simple_cmd_or_funcdec(status_code);
    else
        ((struct ast_cmd *)cmd)->cmd = parse_shell_cmd(status_code);

    struct ast *last_redirection = NULL;
    while (*status_code == 0
           && peek_token(DISABLE_KEYWORDS)->type == REDIRECTION)
    {
        struct ast *redirection = parse_redirection(status_code);
        place_redir(&redir, &last_redirection, redirection);
        ((struct ast_redirection *)last_redirection)->next =
            ((struct ast_cmd *)cmd)->cmd;
        // if (*status_code)
        //     return ret;
    }

    if (redir != NULL)
        ((struct ast_cmd *)cmd)->cmd = redir;

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

static int check_prefix()
{
    struct token *tok = peek_token(DISABLE_KEYWORDS);

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

static void place_prefix(struct ast **cmd, struct ast **last_prefix_list,
                         struct ast *prefix)
{
    struct ast *prefix_list = init_ast(AST_PREFIX_LIST);
    ((struct ast_prefix_list *)prefix_list)->prefix = prefix;

    // if (((struct ast_simple_cmd *)*cmd)->prefix_list == NULL)
    if (*last_prefix_list == NULL)
        ((struct ast_simple_cmd *)*cmd)->prefix_list = prefix_list;
    else
        ((struct ast_prefix_list *)*last_prefix_list)->next = prefix_list;

    *last_prefix_list = prefix_list;
}

static void place_element(struct ast **cmd, struct ast **last_element_list,
                          struct ast *element)
{
    struct ast *element_list = init_ast(AST_ELEMENT_LIST);
    ((struct ast_element_list *)element_list)->element = element;

    // if (((struct ast_simple_cmd *)*cmd)->element_list == NULL)
    if (*last_element_list == NULL)
        ((struct ast_simple_cmd *)*cmd)->element_list = element_list;
    else
        ((struct ast_element_list *)*last_element_list)->next = element_list;

    *last_element_list = element_list;
}

static void add_redirection(struct ast **ret, struct ast **last_redirection,
                            struct ast **cmd, int *status_code)
{
    struct ast *redirection = parse_redirection(status_code);
    place_redir(ret, last_redirection, redirection);
    ((struct ast_redirection *)*last_redirection)->next = *cmd;
}

static struct ast *parse_simple_cmd(int *status_code)
{
    struct ast *last_redirection = NULL;
    struct ast *last_prefix_list = NULL;
    struct ast *last_element_list = NULL;

    struct ast *cmd = init_ast(AST_SIMPLE_CMD);
    struct ast *ret = cmd;

    enum token_type token_type = peek_token(DISABLE_KEYWORDS)->type;

    if (token_type != WORD && token_type != REDIRECTION)
    {
        warnx("parse_simple_cmd: Wrong token type. Expected WORD | Got: %s",
              type_name[token_type]);
        *status_code = 2;
        return ret;
    }

    while (check_prefix() || token_type == REDIRECTION)
    {
        if (token_type == REDIRECTION)
        {
            add_redirection(&ret, &last_redirection, &cmd, status_code);
        }
        else
        {
            // struct ast *prefix = parse_prefix(status_code);
            place_prefix(&cmd, &last_prefix_list, parse_prefix(status_code));
        }

        if (*status_code)
            return ret;

        token_type = peek_token(DISABLE_KEYWORDS)->type;
    }

    if (token_type != WORD)
    {
        if (last_redirection != NULL || last_prefix_list != NULL)
        {
            // ((struct ast_redirection *)last_redirection)->next = cmd;
            return ret;
        }

        warnx("parse_simple_cmd: Wrong token type. Expected WORD | Got: %s",
              type_name[token_type]);
        *status_code = 2;
        return ret;
    }

    ((struct ast_simple_cmd *)cmd)->word =
        strdup(peek_token(DISABLE_KEYWORDS)->data);
    pop_token();

    token_type = peek_token(DISABLE_KEYWORDS)->type;
    while (token_type == WORD || token_type == REDIRECTION)
    {
        if (token_type == REDIRECTION)
        {
            add_redirection(&ret, &last_redirection, &cmd, status_code);
        }
        else
        {
            // struct ast *element = parse_element(status_code);
            place_element(&cmd, &last_element_list, parse_element(status_code));
        }

        if (*status_code)
            return ret;

        token_type = peek_token(DISABLE_KEYWORDS)->type;
    }

    return ret;
}

static struct ast *finish_simple_cmd(struct ast *cmd, int *status_code)
{
    struct ast *last_redirection = NULL;
    struct ast *last_element_list = NULL;
    struct ast *ret = cmd;

    enum token_type token_type = peek_token(DISABLE_KEYWORDS)->type;
    while (token_type == WORD || token_type == REDIRECTION)
    {
        if (token_type == REDIRECTION)
        {
            add_redirection(&ret, &last_redirection, &cmd, status_code);
        }
        else
        {
            // struct ast *element = parse_element(status_code);
            place_element(&cmd, &last_element_list, parse_element(status_code));
        }

        if (*status_code)
            return ret;

        token_type = peek_token(DISABLE_KEYWORDS)->type;
    }

    return ret;
}

static struct ast *finish_funcdec(struct ast *funcdec, int *status_code)
{
    if (peek_token(ENABLE_KEYWORDS)->type != LEFT_PARENTHESIS)
    {
        warnx("finish_funcdec: Wrong token type. Expected LEFT_PARENTHESIS | "
              "Got: %s",
              type_name[peek_token(ENABLE_KEYWORDS)->type]);
        *status_code = 2;
        return funcdec;
    }
    pop_token();

    if (peek_token(ENABLE_KEYWORDS)->type != RIGHT_PARENTHESIS)
    {
        warnx("finish_funcdec: Wrong token type. Expected RIGHT_PARENTHESIS | "
              "Got: %s",
              type_name[peek_token(ENABLE_KEYWORDS)->type]);
        *status_code = 2;
        return funcdec;
    }
    pop_token();

    while (peek_token(ENABLE_KEYWORDS)->type == NEW_LINE)
        pop_token();

    ((struct ast_funcdec *)funcdec)->shell_cmd = parse_shell_cmd(status_code);

    return funcdec;
}

static struct ast *parse_simple_cmd_or_funcdec(int *status_code)
{
    if (check_prefix() || peek_token(ENABLE_KEYWORDS)->type == REDIRECTION)
        return parse_simple_cmd(status_code);

    if (peek_token(ENABLE_KEYWORDS)->type != WORD)
    {
        warnx("parse_simple_cmd_or_funcdec: Wrong token type. Expected WORD | "
              "Got: %s",
              type_name[peek_token(ENABLE_KEYWORDS)->type]);
        *status_code = 2;
        return NULL;
    }

    char *tmp = strdup(peek_token(ENABLE_KEYWORDS)->data);
    pop_token();

    if (peek_token(ENABLE_KEYWORDS)->type == LEFT_PARENTHESIS)
    {
        // Funcdec
        struct ast *funcdec = init_ast(AST_FUNCDEC);
        ((struct ast_funcdec *)funcdec)->name = tmp;

        return finish_funcdec(funcdec, status_code);
    }

    // Simple cmd
    struct ast *cmd = init_ast(AST_SIMPLE_CMD);
    ((struct ast_simple_cmd *)cmd)->word = tmp;

    return finish_simple_cmd(cmd, status_code);
}

static struct ast *parse_subshell(int *status_code)
{
    if (peek_token(ENABLE_KEYWORDS)->type != LEFT_PARENTHESIS)
    {
        warnx("parse_subshell: Wrong token type. Expected LEFT_PARENTHESIS | "
              "Got: %s",
              type_name[peek_token(ENABLE_KEYWORDS)->type]);
        *status_code = 2;
        return NULL;
    }
    pop_token();

    struct ast *compound_list = parse_compound_list(status_code);
    if (*status_code)
        return compound_list;

    if (peek_token(ENABLE_KEYWORDS)->type != RIGHT_PARENTHESIS)
    {
        warnx("parse_subshell: Wrong token type. Expected RIGHT_PARENTHESIS | "
              "Got: %s",
              type_name[peek_token(ENABLE_KEYWORDS)->type]);
        *status_code = 2;
        return compound_list;
    }
    pop_token();

    return compound_list;
}

static struct ast *parse_command_block(int *status_code)
{
    if (peek_token(ENABLE_KEYWORDS)->type != LEFT_BRACKET)
    {
        warnx("parse_command_block: Wrong token type. Expected LEFT_BRACKET | "
              "Got: %s",
              type_name[peek_token(ENABLE_KEYWORDS)->type]);
        *status_code = 2;
        return NULL;
    }
    pop_token();

    struct ast *compound_list = parse_compound_list(status_code);
    if (*status_code)
        return compound_list;

    if (peek_token(ENABLE_KEYWORDS)->type != RIGHT_BRACKET)
    {
        warnx("parse_command_block: Wrong token type. Expected RIGHT_BRACKET | "
              "Got: %s",
              type_name[peek_token(ENABLE_KEYWORDS)->type]);
        *status_code = 2;
        return compound_list;
    }
    pop_token();

    return compound_list;
}

static struct ast *parse_shell_cmd(int *status_code)
{
    struct ast *cmd = init_ast(AST_SHELL_CMD);

    struct token *tok = peek_token(ENABLE_KEYWORDS);

    // TODO: Step 3: Add other rules.
    if (tok->type != IF && tok->type != WHILE && tok->type != UNTIL
        && tok->type != FOR && tok->type != LEFT_PARENTHESIS)
    {
        warnx("parse_shell_cmd: Unsupported shell command. Expected IF or "
              "WHILE or UNTIL or FOR | Got: %s",
              type_name[tok->type]);
        *status_code = 2;
        return cmd;
    }

    // cmd_type defaults to RULE
    if (tok->type == IF)
        ((struct ast_shell_cmd *)cmd)->rule = parse_rule_if(status_code);
    else if (tok->type == WHILE)
        ((struct ast_shell_cmd *)cmd)->rule = parse_rule_while(status_code);
    else if (tok->type == UNTIL)
        ((struct ast_shell_cmd *)cmd)->rule = parse_rule_until(status_code);
    else if (tok->type == FOR)
        ((struct ast_shell_cmd *)cmd)->rule = parse_rule_for(status_code);
    else if (tok->type == LEFT_PARENTHESIS)
    {
        ((struct ast_shell_cmd *)cmd)->cmd_type = SUBSHELL;
        ((struct ast_shell_cmd *)cmd)->compound_list =
            parse_subshell(status_code);
    }
    else if (tok->type == LEFT_BRACKET)
    {
        ((struct ast_shell_cmd *)cmd)->cmd_type = COMMAND_BLOCK;
        ((struct ast_shell_cmd *)cmd)->compound_list =
            parse_command_block(status_code);
    }

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

static int check_for_word(int *status_code)
{
    if (peek_token(ENABLE_KEYWORDS)->type != FOR)
    {
        warnx("parse_rule_for: Wrong token type. Expected FOR | Got: %s",
              type_name[peek_token(ENABLE_KEYWORDS)->type]);
        *status_code = 2;
        return 1;
    }
    pop_token();

    if (peek_token(DISABLE_KEYWORDS)->type != WORD)
    {
        warnx("parse_rule_for: Wrong token type. Expected WORD | Got: %s",
              type_name[peek_token(DISABLE_KEYWORDS)->type]);
        *status_code = 2;
        return 1;
    }

    return 0;
}

static void handle_semicol_newline(void)
{
    if (peek_token(ENABLE_KEYWORDS)->type == SEMICOLON)
        pop_token();

    while (peek_token(ENABLE_KEYWORDS)->type == NEW_LINE)
    {
        pop_token();
    }
}

static struct ast *parse_rule_for(int *status_code)
{
    struct ast *rule_for = init_ast(AST_RULE_FOR);
    if (check_for_word(status_code) != 0)
        return rule_for;

    ((struct ast_rule_for *)rule_for)->condition_word =
        strdup(peek_token(DISABLE_KEYWORDS)->data);
    pop_token();

    handle_semicol_newline();

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

static int check_elif_else(int *status_code)
{
    if (peek_token(ENABLE_KEYWORDS)->type != ELIF
        && peek_token(ENABLE_KEYWORDS)->type != ELSE)
    {
        warnx("parse_else_clause: Wrong entry token type. Expected ELIF or "
              "ELSE | Got: %s",
              type_name[peek_token(ENABLE_KEYWORDS)->type]);
        *status_code = 2;
        return 1;
    }
    return 0;
}

static struct ast *parse_else_clause(int *status_code)
{
    if (check_elif_else(status_code) != 0)
        return NULL;

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
