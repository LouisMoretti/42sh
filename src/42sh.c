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

    // Setup IO from config
    if (io_setup(get_conf()) != 0)
        return 2;

    // Call parser for AST
    struct ast_input *input = parse_input();

    struct ast test = input->base;
    if (test.type != AST_INPUT)
        return 42;

    // Execute AST
    pretty_print((struct ast *)input);

    free_ast_input(input);

    io_close();
    return 0;
}
