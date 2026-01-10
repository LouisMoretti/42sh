#include <assert.h>
#include <stdio.h>

#include "config/config.h"
#include "execution/execution.h"
#include "iobackend/iobackend.h"
#include "parser/parser.h"

int main(int argc, char **argv)
{
    // Set the config struct of the program
    if (set_conf(argc, argv) != 0)
        return 2;

    // Setup IO from config
    if (io_setup(get_conf()) != 0)
        return 2;

    // Call parser for AST
    struct ast *input = parse_input();

    // Execute AST
    assert(input->type == AST_INPUT);

    execute_ast(input);

    free_ast_input(input);

    io_close();
    return 0;
}
