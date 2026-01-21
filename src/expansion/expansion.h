#ifndef EXPANSION_H
#define EXPANSION_H

#include <stddef.h>

#include "parser/ast.h"

char *expand_string(char *string);
char *expand_echo(char *word);
struct ast_word_list *expand_for(struct ast_word_list *word);
int init_expansion(void);
void reset_expansion(void);
struct hash_map *get_hm(void);

#endif // ! EXPANSION_H
