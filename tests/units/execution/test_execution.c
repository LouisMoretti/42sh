#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>
#include <stddef.h>
#include <string.h>

#include "config/config.h"
#include "execution/builtins.h"
#include "execution/execution.h"
#include "iobackend/iobackend.h"
#include "parser/ast.h"
#include "parser/parser.h"

#define INDICES_ARRAY_SIZE 42

TestSuite(Execution_easy);

Test(Execution_easy, exec_echo_a)
{}
