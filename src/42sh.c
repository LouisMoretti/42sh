#include <assert.h>
#include <stdio.h>

#include "config/config.h"
#include "execution/execution.h"
#include "iobackend/iobackend.h"
#include "parser/parser.h"
#include "parser/pretty_print.h"

int main(int argc, char **argv)
{
    // Set the config struct of the program
    if (set_conf(argc, argv) != 0)
        return 2;

    struct config *conf = get_conf();

    // Setup IO from config
    if (io_setup(conf) != 0)
        return 2;

    // Call parser for AST
    int status_code = 0;
    struct ast *input = parse_input(&status_code);
    if (status_code)
    {
        free_ast_input(input);
        return status_code;
    }

    // Execute AST
    assert(input->type == AST_INPUT);

    int exit_code = 0;
    if (conf->pretty_print)
        pretty_print((struct ast *)input);
    else
        exit_code = execute_ast(input);

    free_ast_input(input);

    io_close();
    return exit_code;
}
