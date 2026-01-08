#include <stdio.h>

#include "config/config.h"
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
    struct ast_simple_cmd *cmd = simple_cmd();

    struct ast test = cmd->base;

    if (test.type != AST_SIMPLE_CMD)
        return 42;

    // Execute AST

    io_close();
    return 0;
}
