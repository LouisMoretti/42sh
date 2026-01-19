#ifndef EXPANSION_H
#define EXPANSION_H

#include <stddef.h>

char *expand_string(char *string);
char *expand_echo(char *word);
struct ast_word_list *expand_for(struct ast_word_list *word);

#endif // ! EXPANSION_H
