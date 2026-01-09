#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/parameterized.h>
#include <criterion/redirect.h>
#include <string.h>

#include "config/config.h"
#include "iobackend/iobackend.h"
#include "lexer/lexer.h"

TestSuite(Execution);

Test(Execution, simple_cmd)
{}
