#ifndef EXPANSION_H
#define EXPANSION_H

#include <stddef.h>

char *expand_string(char *string);
char *expand_echo(char *word);
char **expand_for(char *string);

#endif // ! EXPANSION_H
