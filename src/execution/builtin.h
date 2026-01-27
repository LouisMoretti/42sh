#ifndef BUILTIN_H
#define BUILTIN_H

#include "parser/ast.h"

int builtin_echo(struct ast_simple_cmd *command);
int builtin_false(void);
int builtin_true(void);
int builtin_cd(struct ast_simple_cmd *command);
int builtin_exit(struct ast_simple_cmd *ast_simple_cmd);

#endif // ! BUILTIN_H
