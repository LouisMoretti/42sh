#ifndef EXPANSION_H
#define EXPANSION_H

#include <stddef.h>

char *expand_string(char *string);
char *expand_single_quote(char *result, char *copy, size_t *beg, size_t *i);
char *expand_escape(char *result, char *copy, size_t *beg, size_t *i);
char *quote_removal(char *string);

#endif // ! EXPANSION_H
