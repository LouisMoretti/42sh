#include "execution_loop.h"

#include <assert.h>
#include <stdio.h>

#include "config/config.h"
#include "execution/execution.h"
#include "expansion/expansion.h"
#include "iobackend/iobackend.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "parser/pretty_print.h"

int init_modules(struct config *conf)
{
    // Setup IO from config
    if (io_setup(conf) != 0)
        return 2;

    // Init hash table
    if (init_expansion() != 0)
        return 2;

    return 0;
}

void reset_modules(void)
{
    io_close();
    pop_token();
    reset_expansion();
}

int execute_loop(int argc, char **argv)
{
    set_env_variables();

    // Set the config struct of the program
    if (set_conf(argc, argv) != 0)
        return 2;

    struct config *conf = get_conf();

    if (init_modules(conf) != 0)
        return 2;

    // Call parser for AST
    int status_code = 0;
    int exit_code = 0;
    while (peek_chr() != EOF && !is_exit())
    {
        struct ast *input = parse_input(&status_code);
        if (status_code)
        {
            free_ast(input);
            return status_code;
        }

        // Execute AST
        assert(input->type == AST_INPUT);

        exit_code = 0;
        if (conf->pretty_print)
            pretty_print(input);
        else
            exit_code = execute_ast(input);

        free_ast(input);
        conf->previous_code = exit_code;
    }

    reset_modules();
    return exit_code;
}
